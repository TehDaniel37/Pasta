#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "dodee_task.h"

static void copy_string(char *destination, const char *const source, size_t source_len, size_t max_len);
static bool is_empty_str(const char *const str, size_t str_len);
static bool contains_invalid_chars(const char *const name, size_t name_len);

DodeeStatus dodee_task_init(DodeeTask *const task_p)
{
    if (task_p == NULL) { return DODEE_ERROR_NULL_ARGUMENT; }

    task_p->name[0] = '\0';
    task_p->command[0] = '\0';
    dodee_task_set_interval(task_p, 0);
    dodee_task_set_state(task_p, Stopped);

    return DODEE_SUCCESS;
}

DodeeStatus dodee_task_set_name(DodeeTask *const task_p, const char *name, size_t name_len)
{
    if (task_p == NULL || name == NULL) { return DODEE_ERROR_NULL_ARGUMENT; }
    if (name_len > DODEE_TASK_MAX_NAME_LEN) { return DODEE_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(name, name_len)) { return DODEE_ERROR_INVALID_ARGUMENT; }
    if (contains_invalid_chars(name, name_len)) { return DODEE_ERROR_INVALID_ARGUMENT; }

    copy_string(&(task_p->name[0]), name, name_len, DODEE_TASK_MAX_NAME_LEN);

    return DODEE_SUCCESS;
}

DodeeStatus dodee_task_set_command(DodeeTask *const task_p, const char *command, size_t cmd_len)
{
    if (task_p == NULL || command == NULL) { return DODEE_ERROR_NULL_ARGUMENT; }
    if (cmd_len > DODEE_TASK_MAX_CMD_LEN) { return DODEE_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(command, cmd_len)) { return DODEE_ERROR_INVALID_ARGUMENT; }

    copy_string(&(task_p->command[0]), command, cmd_len, DODEE_TASK_MAX_CMD_LEN);

    return DODEE_SUCCESS;
}

DodeeStatus dodee_task_set_interval(DodeeTask *const task_p, int interval)
{
    if (task_p == NULL) { return DODEE_ERROR_NULL_ARGUMENT; }
    if (interval < 0) { return DODEE_ERROR_INVALID_ARGUMENT; }

    task_p->interval_seconds = interval;

    return DODEE_SUCCESS;
}

DodeeStatus dodee_task_set_state(DodeeTask *const task_p, DodeeTaskState state)
{
    if (task_p == NULL) { return DODEE_ERROR_NULL_ARGUMENT; }
    if (state < 0 || state >= DODEE_TASK_STATE_VALUES) { return DODEE_ERROR_INVALID_ARGUMENT; }

    task_p->state = state;

    return DODEE_SUCCESS;
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

