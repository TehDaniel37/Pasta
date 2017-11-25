#include <stdio.h>                  // FILE, fopen(), fclose(), fread(), fileno()
#include <stdlib.h>
#include <stddef.h>
#include <string.h>                 // strncmp
#include <stdbool.h>                // bool, true, false
#include <sys/stat.h>               // fstat()
#include <errno.h>                  // errno
#include <ctype.h>                  // tolower()

#include "schedr_config_parser.h"
#include "schedr_job.h"

static void (*on_number_of_jobs_found_hook)(int expected_jobs) = NULL;

static void *(*allocator)(size_t bytes) = malloc;

static bool is_unit(const char *str);
static bool is_digit(const char *str);
static Status find_number_of_jobs(FILE *fp, char **file_contents, size_t *number_of_jobs);
static Status parse_file_contents(char *file_contents, Job **loaded_jobs, int *jobs_count, int expected_jobs_len);
static bool str_equals_ign_case(const char *str_1, const char *str2);

#ifdef TEST
void schedr_config_set_allocator(void *(*alloc_func)(size_t bytes)) { allocator = alloc_func; }
void schedr_config_reset_allocator() { allocator = malloc; }
void schedr_config_set_on_number_of_jobs_found_hook(void (*hook)(int expected_jobs)) { on_number_of_jobs_found_hook = hook; }
void schedr_config_remove_on_number_of_jobs_found_hook() { on_number_of_jobs_found_hook = NULL; }
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

    if (status != SCHEDR_SUCCESS) 
    {
        free(file_contents); 
        return status;
    }

    if (on_number_of_jobs_found_hook != NULL ) { on_number_of_jobs_found_hook(expected_jobs_len); }

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
    if (str_equals_ign_case("s", str) || str_equals_ign_case("sec", str)
        || str_equals_ign_case("second", str) || str_equals_ign_case("seconds", str))
    {
        return true;
    }
    else if (str_equals_ign_case("m", str) || str_equals_ign_case("min", str)
        || str_equals_ign_case("minute", str) || str_equals_ign_case("minutes", str))
    {
        return true;
    }
    else if (str_equals_ign_case("h", str) || str_equals_ign_case("hour", str) 
        || str_equals_ign_case("hours", str))
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
    char *cpy = (char *)allocator(file_len + 1);
    
    if (*file_contents == NULL || cpy == NULL) 
    {
        fclose(fp);
        return SCHEDR_ERROR_ALLOCATION_FAILED;
    }

    fread(*file_contents, sizeof (char), file_len, fp);
    
    // TODO: handle fread error
    
    fclose(fp);
    (*file_contents)[file_len] = '\0';
    
    static const char NEW_LINE[] = "\r\n";

    size_t result = 0;

    strncpy(cpy, *file_contents, file_len + 1);
    cpy[file_len] = '\0';

    char *line = strtok(cpy, NEW_LINE);

    while (line != NULL)
    {
        if (str_equals_ign_case("Job", line)) { result++; }

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

        if (str_equals_ign_case("Job", word))
        {
            current_job = &(job_list[*jobs_count]);
            schedr_job_init(current_job);
            (*jobs_count)++;

            word = strtok(NULL, NAME_DELIM); 

            if (word == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
            else { schedr_job_set_name(current_job, word, strlen(word)); }
        }

        else if (str_equals_ign_case("run", word))
        {
            if (current_job != NULL)
            {
                word = strtok(NULL, CMD_DELIM);
                
                if (word == NULL) { return SCHEDR_ERROR_CONFIG_FORMAT; }
                else { schedr_job_set_command(current_job, word, strlen(word)); }
            }
        }
        // TODO: Refactor crap code below 
        else if (str_equals_ign_case("every", word))
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

                if (str_equals_ign_case("s", unit) || str_equals_ign_case("sec", unit) 
                    || str_equals_ign_case("second", unit) || str_equals_ign_case("seconds", unit))
                {
                    seconds = seconds * 1;
                }
                else if (str_equals_ign_case("m", unit) || str_equals_ign_case("min", unit) 
                    || str_equals_ign_case("minute", unit) || str_equals_ign_case("minutes", unit))
                {
                    seconds = seconds * 60;
                }
                else if (str_equals_ign_case("h", unit) || str_equals_ign_case("hour", unit) 
                    || str_equals_ign_case("hours", unit))
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

static bool str_equals_ign_case(const char *str_1, const char *str_2)
{
    for (int i = 0; str_1[i]; i++)
    {
        if (tolower(str_1[i]) != tolower(str_2[i]))
        {
            return false;
        }
    }
    
    return true;
}
