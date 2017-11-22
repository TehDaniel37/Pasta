#include <stdio.h>                  // FILE, fopen(), fclose(), fread()
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>                  // fileno

#include "schedr_config_parser.h"
#include "schedr_job.h"

static const size_t MAX_WORD_LEN = 1000;

static void *(*allocator)(size_t bytes) = malloc;

static bool is_unit(const char *str);
static bool is_digit(const char *str);
static Status find_number_of_jobs(FILE *fp, char **file_contents, size_t *number_of_jobs);
static Status parse_file_contents(char *file_contents, Job **loaded_jobs, int *jobs_count, int expected_jobs_len);

#ifdef TEST
void schedr_config_set_allocator(void *(*alloc_func)(size_t bytes)) { allocator = alloc_func; }
void schedr_config_reset_allocator() { allocator = malloc; }
#endif

Status schedr_config_load_jobs(Job *jobs[], int *loaded_jobs_count, const char *filepath)
{
    if (*jobs != NULL)
    {
        return SCHEDR_ERROR_INVALID_ARGUMENT;
    }

    FILE *file_p = fopen(filepath, "r");

    if (file_p == NULL)
    {
        if (errno == EACCES) { return SCHEDR_ERROR_PERMISSION_DENIED; }
        if (errno == ENOENT) { return SCHEDR_ERROR_FILE_NOT_FOUND; }
        
        return SCHEDR_FAILURE;
    }

    char *file_contents = NULL;
    size_t expected_jobs_len = 0;
    
    Status status = find_number_of_jobs(file_p, &file_contents, &expected_jobs_len);

    if (status != SCHEDR_SUCCESS) { return status; }

    int jobs_count = 0;
    Job *loaded_jobs = (Job *)allocator(sizeof (Job) * expected_jobs_len);

    if (loaded_jobs == NULL)
    {
        free(file_contents);
        file_contents = NULL;
        
        return SCHEDR_ERROR_ALLOCATION_FAILED;
    }

    status = parse_file_contents(file_contents, &loaded_jobs, &jobs_count, expected_jobs_len);
    
    free(file_contents);
    
    if (status == SCHEDR_SUCCESS)
    {
        *jobs = loaded_jobs;
        *loaded_jobs_count = jobs_count;
    }
    else
    {
        free(loaded_jobs);
    }

    return status;
}

static bool is_unit(const char *str)
{
    if (( strcmp("s", str) == 0 || strcmp("sec", str) == 0 
       || strcmp("second", str) == 0) || strcmp("seconds", str) == 0)
    {
        return true;
    }
    else if (( strcmp("m", str) == 0 || strcmp("min", str) == 0 
       || strcmp("minute", str) == 0) || strcmp("minutes", str) == 0)
    {
        return true;
    }
    else if (( strcmp("h", str) == 0 || strcmp("hour", str) == 0 || strcmp("hours", str) == 0))
    {
        return true;
    }

    return false;
}

static bool is_digit(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }

    return true;
}

static Status find_number_of_jobs(FILE *fp, char **file_contents, size_t *number_of_jobs)
{
    struct stat st;
    fstat(fileno(fp), &st);
    size_t file_len = st.st_size;

    if (S_ISDIR(st.st_mode)) { return SCHEDR_ERROR_INVALID_ARGUMENT; }

    *file_contents = (char *)allocator(file_len + 1);
    
    if (*file_contents == NULL) 
    {
        fclose(fp);
        return SCHEDR_ERROR_ALLOCATION_FAILED;
    }

    fread(*file_contents, sizeof (char), file_len, fp);
    
    // TODO: handle fread error
    
    fclose(fp);
    (*file_contents)[file_len] = '\0';
    
    static const char NEW_LINE[] = "\r\n";
    static const size_t JOB_LEN = 3;

    size_t result = 0;
    
    char *cpy = (char *)allocator(file_len + 1);
    
    if (cpy == NULL)
    {
        free(*file_contents);
        *file_contents = NULL;
        fclose(fp);
        
        return SCHEDR_ERROR_ALLOCATION_FAILED;
    }

    strncpy(cpy, *file_contents, file_len + 1);
    cpy[file_len] = '\0';

    char *line = strtok(cpy, NEW_LINE);

    while (line != NULL)
    {
        if (strncmp("Job", line, JOB_LEN) == 0) { result++; }

        line = strtok(NULL, NEW_LINE);
    }

    free(cpy);

    *number_of_jobs = result;

    if (result == 0)
    {
        return SCHEDR_WARNING_NO_JOBS;
    }

    return SCHEDR_SUCCESS;
}

static Status parse_file_contents(char *file_contents, Job **loaded_jobs, int *jobs_count, int expected_jobs_len)
{
    static const char DEFAULT_DELIM[] = " \t\r\n\v\f";
    static const char NAME_DELIM[] = "\"";
    static const char CMD_DELIM[] = "`";
    
    Job *job_list = *loaded_jobs;
    Job *current_job = NULL;
    char *word = strtok(file_contents, DEFAULT_DELIM); 

    while (word != NULL)
    {
        if (*jobs_count > expected_jobs_len)
        {
            return SCHEDR_ERROR_CONFIG_FORMAT;
        }

        if (strncmp("Job", word, MAX_WORD_LEN) == 0)
        {
            current_job = &(job_list[*jobs_count]);
            schedr_job_init(current_job);
            (*jobs_count)++;

            word = strtok(NULL, NAME_DELIM); 

            if (word == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
            else { schedr_job_set_name(current_job, word, strlen(word)); }
        }

        else if (strncmp("run", word, MAX_WORD_LEN) == 0)
        {
            if (current_job != NULL)
            {
                word = strtok(NULL, CMD_DELIM);
                
                if (word == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
                else { schedr_job_set_command(current_job, word, strlen(word)); }
            }
        }

        else if (strncmp("every", word, MAX_WORD_LEN) == 0)
        {
            if (current_job != NULL)
            {
                int seconds = 0;
                char *tok = strtok(NULL, DEFAULT_DELIM);
                char val[12];
                char unit[20];

                if (tok == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
                else if (is_unit(tok)) 
                {
                    strncpy(unit, tok, sizeof (unit) - 1);
                    strncpy(val, "1", 2);
                }
                else if (is_digit(tok))
                {
                    strncpy(val, tok, sizeof (val) - 1);
                    tok = strtok(NULL, DEFAULT_DELIM);

                    if (tok == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
                    else if (is_unit(tok)) { strncpy(unit, tok, sizeof (unit) - 1); }
                    else { return SCHEDR_ERROR_CONFIG_FORMAT; }
                }
                else { return SCHEDR_ERROR_CONFIG_FORMAT; }

                seconds = atoi(val);

                if (( strcmp("s", unit) == 0 || strcmp("sec", unit) == 0 
                   || strcmp("second", unit) == 0) || strcmp("seconds", unit) == 0)
                {
                    seconds = seconds * 1;
                }
                else if (( strcmp("m", unit) == 0 || strcmp("min", unit) == 0 
                   || strcmp("minute", unit) == 0) || strcmp("minutes", unit) == 0)
                {
                    seconds = seconds * 60;
                }
                else if (( strcmp("h", unit) == 0 || strcmp("hour", unit) == 0 || strcmp("hours", unit) == 0))
                {
                    seconds = seconds * 3600;
                }
                else { return SCHEDR_ERROR_CONFIG_FORMAT; }

                schedr_job_set_interval(current_job, seconds);
            }
        }
        else { return SCHEDR_ERROR_CONFIG_FORMAT; }

        if (word != NULL) { word = strtok(NULL, DEFAULT_DELIM); }
    }
    
    *loaded_jobs = job_list;
    
    return SCHEDR_SUCCESS;
}
