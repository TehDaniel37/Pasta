#include <stdio.h>                  // FILE, fopen(), fclose(), fread(), fileno()
#include <stdlib.h>
#include <stddef.h>
#include <string.h>                 // strncmp
#include <stdbool.h>                // bool, true, false
#include <sys/stat.h>               // fstat()
#include <errno.h>                  // errno
#include <ctype.h>                  // tolower()
#include <regex.h>

#include "dodee_config_parser.h"
#include "dodee_task.h"

struct DodeeToken {
    regex_t *pattern;
    DodeeStatus (*handler)(DodeeTask **, char *, regmatch_t *);
    struct DodeeToken *inner_tokens;
};

typedef struct DodeeToken DodeeToken;

static void (*on_number_of_tasks_found_hook)(int expected_tasks) = NULL;
static void *(*allocator)(size_t bytes) = malloc;

#ifdef TEST
void dodee_config_set_allocator(void *(*alloc_func)(size_t bytes)) { allocator = alloc_func; }
void dodee_config_reset_allocator() { allocator = malloc; }
void dodee_config_set_on_number_of_tasks_found_hook(void (*hook)(int expected_tasks)) { on_number_of_tasks_found_hook = hook; }
void dodee_config_remove_on_number_of_tasks_found_hook() { on_number_of_tasks_found_hook = NULL; }
#endif

static DodeeStatus load_config_file(FILE *fp, char **file_contents);
static DodeeStatus parse_file_contents(char *file_contents);
static DodeeStatus init_tokens(DodeeToken *tokens);
static DodeeStatus run_regex(char *lines, DodeeTask **current_task, DodeeToken tokens[], int nr_of_tokens);
static DodeeStatus create_token(DodeeToken *out, const char *pattern, DodeeStatus (*handler)(DodeeTask **, char *, regmatch_t *));
static DodeeStatus task_handler(DodeeTask **current_task, char *text, regmatch_t matches[]);
static DodeeStatus command_handler(DodeeTask **current_task, char *text, regmatch_t matches[]);
static DodeeStatus interval_handler(DodeeTask **current_task, char *text, regmatch_t matches[]);

#define MAX_TASKS    64
#define MAX_TOKENS  32

static DodeeToken tokens[MAX_TOKENS];
static DodeeTask *loaded_tasks;
static int tasks_found;

DodeeStatus dodee_config_load_tasks(DodeeTask *tasks[], int *loaded_tasks_count, const char *filepath)
{
    if (*tasks != NULL) { return DODEE_ERROR_INVALID_ARGUMENT; }

    FILE *file_p = fopen(filepath, "r");

    if (file_p == NULL)
    {
        if (errno == EACCES) { return DODEE_ERROR_PERMISSION_DENIED; }
        if (errno == ENOENT) { return DODEE_ERROR_FILE_NOT_FOUND; }
        
        return DODEE_FAILURE;
    }

    char *file_contents = NULL;

    DodeeStatus status = load_config_file(file_p, &file_contents);
    
    if (status != DODEE_SUCCESS) { return status; }

    loaded_tasks = (DodeeTask *)allocator(sizeof (DodeeTask) * MAX_TASKS);

    if (loaded_tasks == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }

    tasks_found = 0;

    status = parse_file_contents(file_contents);
    
    free(file_contents);
    
    if (status == DODEE_SUCCESS)
    {
        *tasks = &loaded_tasks[0];
        *loaded_tasks_count = tasks_found;

        if (tasks_found == 0) { return DODEE_WARNING_NO_TASKS; }
    }

    return status;
}

static DodeeStatus load_config_file(FILE *fp, char **file_contents)
{
    struct stat st;
    fstat(fileno(fp), &st);
    size_t file_len = st.st_size;

    if (S_ISDIR(st.st_mode)) { return DODEE_ERROR_INVALID_ARGUMENT; }

    *file_contents = (char *)allocator(file_len + 1);
    
    if (*file_contents == NULL) 
    {
        fclose(fp);
        return DODEE_ERROR_ALLOCATION_FAILED;
    }

    fread(*file_contents, sizeof (char), file_len, fp);
    
    // TODO: handle fread error
    
    fclose(fp);
    (*file_contents)[file_len] = '\0';

    return DODEE_SUCCESS;
}

static DodeeStatus parse_file_contents(char *file_contents)
{
    if (init_tokens(tokens) != DODEE_SUCCESS) 
    {
        exit(EXIT_FAILURE);
    }

    int nr_of_tokens = 1;
    DodeeTask *current_task = &(loaded_tasks[tasks_found]);

    DodeeStatus status = run_regex(file_contents, &current_task, tokens, nr_of_tokens);

    return status;
}

static DodeeStatus init_tokens(DodeeToken *tokens)
{
    DodeeToken *inner_task_tokens = (DodeeToken *)allocator(sizeof (DodeeToken) * 2);

    if (inner_task_tokens == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }

    DodeeStatus status = create_token(&inner_task_tokens[0], "run\\s+`(.*)`", command_handler);
    if (status != DODEE_SUCCESS) { return status; }

    status = create_token(&inner_task_tokens[1], "every\\s*([0-9]+)?\\s+(hours|hour|h|minutes|minute|min|m|seconds|second|sec|s)", interval_handler);
    if (status != DODEE_SUCCESS) { return status; }

    status = create_token(&tokens[0], "^DodeeTask\\s+\"([^\"]*)\"\\s+((\n?.)*)", task_handler);
    tokens[0].inner_tokens = inner_task_tokens;

    return status;
}

static DodeeStatus run_regex(char *lines, DodeeTask **current_task, DodeeToken tokens[], int nr_of_tokens)
{
    int MAX_GROUPS = 20;
    long offset = 0;

    for (int i = 0; i < nr_of_tokens; i++)
    {
        regmatch_t matches[MAX_GROUPS];

        while (regexec(tokens[i].pattern, lines + offset, MAX_GROUPS, matches, 0) != REG_NOMATCH)
        {
            DodeeStatus status = tokens[i].handler(current_task, lines + offset, matches);
            if (status != DODEE_SUCCESS) { return status; }
            offset += matches[0].rm_eo;
        }
    }

    return DODEE_SUCCESS;
}

static DodeeStatus create_token(DodeeToken *out, const char *pattern, DodeeStatus (*handler)(DodeeTask **, char *, regmatch_t *)) 
{
    regex_t *regex = (regex_t *)allocator(sizeof (regex_t));
    if (regex == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    regcomp(regex, pattern, REG_ICASE | REG_NEWLINE | REG_EXTENDED);
    out->pattern = regex;
    out->handler = handler;

    return DODEE_SUCCESS;
}

static DodeeStatus task_handler(DodeeTask **current_task, char *text, regmatch_t matches[])
{
    dodee_task_init(*current_task);

    int task_name_len = matches[1].rm_eo - matches[1].rm_so;
    char *task_name = (char *)allocator(task_name_len + 1);
    if (task_name == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    strncpy(task_name, text + matches[1].rm_so, task_name_len);
    task_name[task_name_len] = '\0';

    dodee_task_set_name(*current_task, task_name, task_name_len);
    free(task_name);

    int task_text_len = matches[2].rm_eo - matches[2].rm_so;
    char *task_text = (char *)allocator(task_text_len + 1);
    if (task_text == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    strncpy(task_text, text + matches[2].rm_so, task_text_len);
    task_text[task_text_len] = '\0';

    DodeeStatus status = run_regex(task_text, current_task, tokens[0].inner_tokens, 2);
    free(task_text);

    if (status != DODEE_SUCCESS) { return status; }

    if ((*current_task)->name[0] == '\0' 
            || (*current_task)->command[0] == '\0'
            || (*current_task)->interval_seconds == 0) {
        return DODEE_ERROR_CONFIG_FORMAT;
    }

    tasks_found = tasks_found + 1;
    *current_task = &(loaded_tasks[tasks_found]);

    return DODEE_SUCCESS;
}

static DodeeStatus interval_handler(DodeeTask **current_task, char *text, regmatch_t matches[])
{
    int interval_raw_len = matches[1].rm_eo - matches[1].rm_so;
    char *interval_raw = (char *)allocator(interval_raw_len + 1);
    if (interval_raw == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    strncpy(interval_raw, text + matches[1].rm_so, interval_raw_len);
    interval_raw[interval_raw_len] = '\0';

    int interval = atoi(interval_raw);
    if (interval == 0) 
    {
        interval = 1;
    }
    free(interval_raw);

    int unit_len = matches[2].rm_eo - matches[2].rm_so;
    char *unit = (char *)allocator(unit_len + 1);
    if (unit == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    strncpy(unit, text + matches[2].rm_so, unit_len);
    unit[unit_len] = '\0';

    if (tolower(unit[0]) == 'm')
    {
        interval = interval * 60;
    } 
    else if (tolower(unit[0]) == 'h')
    {
        interval = interval * 3600;
    }

    free(unit);

    dodee_task_set_interval(*current_task, interval);

    return DODEE_SUCCESS;
}

static DodeeStatus command_handler(DodeeTask **current_task, char *text, regmatch_t matches[])
{
    int cmd_len = matches[1].rm_eo - matches[1].rm_so;
    char *cmd = (char *)allocator(cmd_len + 1);
    if (cmd == NULL) { return DODEE_ERROR_ALLOCATION_FAILED; }
    strncpy(cmd, text + matches[1].rm_so, cmd_len);
    cmd[cmd_len] = '\0';

    dodee_task_set_command(*current_task, cmd, cmd_len);
    free(cmd);

    return DODEE_SUCCESS;
}
