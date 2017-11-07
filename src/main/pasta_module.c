#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pasta_module.h"

static const char FIRST_NON_NULL_ASCII_CHAR = '\x1';
static const char FIRST_PRINTABLE_ASCII_CHAR = ' ';

Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len)
{
    if (module_p == NULL || name == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    if (name_len > PASTA_MODULE_MAX_NAME_LEN)
    {
        return PASTA_ERROR_BUFFER_OVERFLOW;
    }

    size_t cpy_at_most = (PASTA_MODULE_MAX_NAME_LEN <= name_len) ?
                          PASTA_MODULE_MAX_NAME_LEN:
                          name_len;

    if (name_len == 0 || name[0] == '\0')
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    for (int char_index = 0; char_index <= name_len; char_index++)
    {
        if (name[char_index] == '\0')
        {
            break;
        }

        int char_code = (int)name[char_index];

        if (char_code >= FIRST_NON_NULL_ASCII_CHAR && char_code < FIRST_PRINTABLE_ASCII_CHAR)
        {
            return PASTA_ERROR_INVALID_ARGUMENT;
        }
    }

    strncpy(module_p->name, name, cpy_at_most);

    return PASTA_SUCCESS;
}

Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len)
{
    if (module_p == NULL || command == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    if (cmd_len > PASTA_MODULE_MAX_CMD_LEN)
    {
        return PASTA_ERROR_BUFFER_OVERFLOW;
    }

    size_t cpy_at_most = (PASTA_MODULE_MAX_CMD_LEN <= cmd_len) ?
                          PASTA_MODULE_MAX_CMD_LEN:
                          cmd_len;

    if (cmd_len == 0 || command[0] == '\0')
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    static const char system_prefix[] = "/bin/sh -n -c \'";
    static const char system_postfix[] = "\' &> /dev/null";
    char system_wrapper[cmd_len + strlen(system_prefix) + strlen(system_postfix) + 1];

    strncpy(system_wrapper, system_prefix, strlen(system_prefix));
    strncat(system_wrapper, command, cmd_len);
    strncat(system_wrapper, system_postfix, strlen(system_postfix));
    int exit_code = system(system_wrapper);

    if (exit_code != EXIT_SUCCESS)
    {
        return PASTA_ERROR_INVALID_SYNTAX;
    }

    strncpy(module_p->command, command, cpy_at_most);

    return PASTA_SUCCESS;
}

Status pasta_module_set_interval(Module *const module_p, int interval)
{
    if (module_p == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    if (interval < 0)
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    module_p->interval_seconds = interval;

    return PASTA_SUCCESS;
}

Status pasta_module_set_state(Module *const module_p, ModuleState state)
{
    if (module_p == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    int state_as_number = (int)state;

    if (state_as_number < 0 || state_as_number >= PASTA_MODULE_STATE_VALUES)
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    module_p->state = state;

    return PASTA_SUCCESS;
}

