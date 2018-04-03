#include <stdio.h>                  // FILE, fopen(), fclose(), fread(), fileno()
#include <stdlib.h>
#include <stddef.h>
#include <string.h>                 // strncmp
#include <stdbool.h>                // bool, true, false
#include <sys/stat.h>               // fstat()
#include <errno.h>                  // errno
#include <ctype.h>                  // tolower()
#include <regex.h>

#include "schedr_config_parser.h"
#include "schedr_job.h"

struct Token {
    regex_t *pattern;
    void (*handler)(Job **, char *, regmatch_t *);
    struct Token *inner_tokens;
};

typedef struct Token Token;

static void (*on_number_of_jobs_found_hook)(int expected_jobs) = NULL;
static void *(*allocator)(size_t bytes) = malloc;

#ifdef TEST
void schedr_config_set_allocator(void *(*alloc_func)(size_t bytes)) { allocator = alloc_func; }
void schedr_config_reset_allocator() { allocator = malloc; }
void schedr_config_set_on_number_of_jobs_found_hook(void (*hook)(int expected_jobs)) { on_number_of_jobs_found_hook = hook; }
void schedr_config_remove_on_number_of_jobs_found_hook() { on_number_of_jobs_found_hook = NULL; }
#endif

static Status load_config_file(FILE *fp, char **file_contents);
static Status parse_file_contents(char *file_contents);
static Status init_tokens(Token *tokens);
static void run_regex(char *lines, Job **current_job, Token tokens[], int nr_of_tokens);
static Token create_token(const char *pattern, void (*handler)(Job **, char *, regmatch_t *));
static void job_handler(Job **current_job, char *text, regmatch_t matches[]);
static void command_handler(Job **current_job, char *text, regmatch_t matches[]);
static void interval_handler(Job **current_job, char *text, regmatch_t matches[]);

#define MAX_JOBS    64
#define MAX_TOKENS  32

static Token tokens[MAX_TOKENS];
static Job *loaded_jobs;
static int jobs_found;

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

    Status status = load_config_file(file_p, &file_contents);
    
    if (status != SCHEDR_SUCCESS) { return status; }

    loaded_jobs = (Job *)malloc(sizeof (Job) * MAX_JOBS);
    jobs_found = 0;

    status = parse_file_contents(file_contents);
    
    free(file_contents);
    
    if (status == SCHEDR_SUCCESS)
    {
        *jobs = &loaded_jobs[0];
        *loaded_jobs_count = jobs_found;
    }

    return status;
}

static Status load_config_file(FILE *fp, char **file_contents)
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

    return SCHEDR_SUCCESS;
}

static Status parse_file_contents(char *file_contents)
{
    if (init_tokens(tokens) != SCHEDR_SUCCESS) 
    {
        exit(EXIT_FAILURE);
    }

    int nr_of_tokens = 1;
    Job *current_job = &(loaded_jobs[jobs_found]);

    run_regex(file_contents, &current_job, tokens, nr_of_tokens);

    return SCHEDR_SUCCESS;
}

static Status init_tokens(Token *tokens)
{
    Token *inner_job_tokens = (Token *)malloc(sizeof (Token) * 2);
    inner_job_tokens[0] = create_token("run\\s+`(.*)`", command_handler);
    inner_job_tokens[1] = create_token("every\\s*([0-9]+)?\\s+(hours|hour|h|minutes|minute|min|m|seconds|second|sec|s)", interval_handler);

    tokens[0] = create_token("^Job\\s+\"([^\"]*)\"\\s+((\n?.)*)", job_handler);
    tokens[0].inner_tokens = inner_job_tokens;

    return SCHEDR_SUCCESS;
}

static void run_regex(char *lines, Job **current_job, Token tokens[], int nr_of_tokens)
{
    int MAX_GROUPS = 20;
    long offset = 0;

    for (int i = 0; i < nr_of_tokens; i++)
    {
        regmatch_t matches[MAX_GROUPS]; 

        while (regexec(tokens[i].pattern, lines + offset, MAX_GROUPS, matches, 0) == 0)
        {
            tokens[i].handler(current_job, lines + offset, matches);
            offset += matches[0].rm_eo;
        }
    }
}

static Token create_token(const char *pattern, void (*handler)(Job **, char *, regmatch_t *)) 
{
    Token result;

    regex_t *regex = (regex_t *)malloc(sizeof (regex_t));
    regcomp(regex, pattern, REG_ICASE | REG_NEWLINE | REG_EXTENDED);
    result.pattern = regex;
    result.handler = handler;

    return result;
}

static void job_handler(Job **current_job, char *text, regmatch_t matches[])
{
    schedr_job_init(*current_job);
    schedr_job_set_name(*current_job, text + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);

    int job_text_len = matches[2].rm_eo - matches[2].rm_so;
    char *job_text = (char *)malloc(job_text_len + 1);
    strncpy(job_text, text + matches[2].rm_so, job_text_len);
    job_text[job_text_len] = '\0';

    run_regex(job_text, current_job, tokens[0].inner_tokens, 2);
    free(job_text);

    jobs_found = jobs_found + 1;
    *current_job = &(loaded_jobs[jobs_found]);
}

static void interval_handler(Job **current_job, char *text, regmatch_t matches[])
{
    int interval_raw_len = matches[1].rm_eo - matches[1].rm_so;
    char *interval_raw = (char *)malloc(interval_raw_len + 1);
    strncpy(interval_raw, text + matches[1].rm_so, interval_raw_len);
    interval_raw[interval_raw_len] = '\0';

    int interval = atoi(interval_raw);
    if (interval == 0) 
    {
        interval = 1;
    }
    free(interval_raw);

    int unit_len = matches[2].rm_eo - matches[2].rm_so;
    char *unit = (char *)malloc(unit_len + 1);
    strncpy(unit, text + matches[2].rm_so, unit_len);
    unit[unit_len] = '\0';

    if (strncmp(unit, "m", 1) == 0) 
    {
        interval = interval * 60;
    } 
    else if (strncmp(unit, "h", 1) == 0) 
    {
        interval = interval * 3600;
    }

    free(unit);

    schedr_job_set_interval(*current_job, interval);
}

static void command_handler(Job **current_job, char *text, regmatch_t matches[])
{
    schedr_job_set_command(*current_job, text + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
}
