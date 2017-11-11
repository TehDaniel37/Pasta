#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "pasta_module.h"

static void copy_string(char *destination, const char *const source, size_t source_len, size_t max_len);
static bool is_empty_str(const char *const str, size_t str_len);
static bool contains_invalid_chars(const char *const name, size_t name_len);

Status pasta_module_init(Module *const module_p)
{
    if (module_p == NULL) { return PASTA_ERROR_NULL_ARGUMENT; }

    module_p->name[0] = '\0';
    module_p->command[0] = '\0';
    pasta_module_set_interval(module_p, 0);
    pasta_module_set_state(module_p, Stopped);

    return PASTA_SUCCESS;
}

Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len)
{
    if (module_p == NULL || name == NULL) { return PASTA_ERROR_NULL_ARGUMENT; }
    if (name_len > PASTA_MODULE_MAX_NAME_LEN) { return PASTA_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(name, name_len)) { return PASTA_ERROR_INVALID_ARGUMENT; }
    if (contains_invalid_chars(name, name_len)) { return PASTA_ERROR_INVALID_ARGUMENT; }

    copy_string(&(module_p->name[0]), name, name_len, PASTA_MODULE_MAX_NAME_LEN);

    return PASTA_SUCCESS;
}

Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len)
{
    if (module_p == NULL || command == NULL) { return PASTA_ERROR_NULL_ARGUMENT; }
    if (cmd_len > PASTA_MODULE_MAX_CMD_LEN) { return PASTA_ERROR_BUFFER_OVERFLOW; }
    if (is_empty_str(command, cmd_len)) { return PASTA_ERROR_INVALID_ARGUMENT; }

    copy_string(&(module_p->command[0]), command, cmd_len, PASTA_MODULE_MAX_CMD_LEN);

    return PASTA_SUCCESS;
}

Status pasta_module_set_interval(Module *const module_p, int interval)
{
    if (module_p == NULL) { return PASTA_ERROR_NULL_ARGUMENT; }
    if (interval < 0) { return PASTA_ERROR_INVALID_ARGUMENT; }

    module_p->interval_seconds = interval;

    return PASTA_SUCCESS;
}

Status pasta_module_set_state(Module *const module_p, ModuleState state)
{
    if (module_p == NULL) { return PASTA_ERROR_NULL_ARGUMENT; }
    if (state < 0 || state >= PASTA_MODULE_STATE_VALUES) { return PASTA_ERROR_INVALID_ARGUMENT; }

    module_p->state = state;

    return PASTA_SUCCESS;
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

