#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "schedr_job.h"

static void copy_string(char *destination, const char *const source, size_t source_len, size_t max_len);
static bool is_empty_str(const char *const str, size_t str_len);
static bool contains_invalid_chars(const char *const name, size_t name_len);

Status schedr_job_init(Job *const job_p)
{
    if (job_p == NULL) { return SCHEDR_ERROR_NULL_ARGUMENT; }

    job_p->name[0] = '\0';
    job_p->command[0] = '\0';
    schedr_job_set_interval(job_p, 0);
    schedr_job_set_state(job_p, Stopped);

    return SCHEDR_SUCCESS;
}

Status schedr_job_set_name(Job *const job_p, const char *name, size_t name_len)
{
    if (job_p == NULL || name == NULL) { return SCHEDR_ERROR_NULL_ARGUMENT; }
    if (name_len > SCHEDR_JOB_MAX_NAME_LEN) { return SCHEDR_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(name, name_len)) { return SCHEDR_ERROR_INVALID_ARGUMENT; }
    if (contains_invalid_chars(name, name_len)) { return SCHEDR_ERROR_INVALID_ARGUMENT; }

    copy_string(&(job_p->name[0]), name, name_len, SCHEDR_JOB_MAX_NAME_LEN);

    return SCHEDR_SUCCESS;
}

Status schedr_job_set_command(Job *const job_p, const char *command, size_t cmd_len)
{
    if (job_p == NULL || command == NULL) { return SCHEDR_ERROR_NULL_ARGUMENT; }
    if (cmd_len > SCHEDR_JOB_MAX_CMD_LEN) { return SCHEDR_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(command, cmd_len)) { return SCHEDR_ERROR_INVALID_ARGUMENT; }

    copy_string(&(job_p->command[0]), command, cmd_len, SCHEDR_JOB_MAX_CMD_LEN);

    return SCHEDR_SUCCESS;
}

Status schedr_job_set_interval(Job *const job_p, int interval)
{
    if (job_p == NULL) { return SCHEDR_ERROR_NULL_ARGUMENT; }
    if (interval < 0) { return SCHEDR_ERROR_INVALID_ARGUMENT; }

    job_p->interval_seconds = interval;

    return SCHEDR_SUCCESS;
}

Status schedr_job_set_state(Job *const job_p, JobState state)
{
    if (job_p == NULL) { return SCHEDR_ERROR_NULL_ARGUMENT; }
    if (state < 0 || state >= SCHEDR_JOB_STATE_VALUES) { return SCHEDR_ERROR_INVALID_ARGUMENT; }

    job_p->state = state;

    return SCHEDR_SUCCESS;
}

static void copy_string(char *destination, const char *const source, size_t source_len, size_t max_len)
{
    size_t cpy_at_most = (max_len <= source_len) ?
                          max_len:
                          source_len;

    strncpy(destination, source, cpy_at_most);
    destination[cpy_at_most] = '\0';
}

static bool is_empty_str(const char *const str, size_t str_len)
{
    return ( str_len == 0 || (str != NULL && str[0] == '\0') );
}

static bool contains_invalid_chars(const char *const str, size_t str_len)
{
    static const char FIRST_NON_NULL_ASCII_CHAR = '\x1';
    static const char FIRST_PRINTABLE_ASCII_CHAR = ' ';

    for (int char_index = 0; char_index <= str_len; char_index++)
    {
        if (str[char_index] == '\0') { return false; }

        int char_code = (int)str[char_index];

        if (char_code >= FIRST_NON_NULL_ASCII_CHAR && char_code < FIRST_PRINTABLE_ASCII_CHAR)
        {
            return true;
        }
    }

    return true; // name is not null terminated
}

