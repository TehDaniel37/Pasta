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
        puts("NULL_ARGUMENT");
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    size_t cmp_at_most = (PASTA_MODULE_MAX_NAME_LEN <= name_len) ?
                          PASTA_MODULE_MAX_NAME_LEN:
                          name_len;

    printf("at most = %zd\n", cmp_at_most);

    if (name[0] == '\0' || name_len <= 0)
    {
        puts("INVALID ARGUMENT");
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
            puts("INVALID ARGUMENT");
            return PASTA_ERROR_INVALID_ARGUMENT;
        }
    }

    if (strnlen(name, cmp_at_most) == cmp_at_most)
    {
        puts("BUFFER OVERFLOW");
        return PASTA_ERROR_BUFFER_OVERFLOW;
    }

    strncpy(module_p->name, name, cmp_at_most);

    puts("SUCCESS");
    return PASTA_SUCCESS;
}

Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len)
{
    if (module_p == NULL || command == NULL)
    {
        puts("NULL ARGUMENT");
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    size_t cmp_at_most = (PASTA_MODULE_MAX_CMD_LEN <= cmd_len) ?
                          PASTA_MODULE_MAX_CMD_LEN + 1:
                          cmd_len;

    printf("at most = %zd\n", cmp_at_most);

    if (command[0] == '\0')
    {
        puts("INVALID ARGUMENT");
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    if (strnlen(command, cmp_at_most) == cmp_at_most)
    {
        puts("BUFFER OVERFLOW");
        return PASTA_ERROR_BUFFER_OVERFLOW;
    }

    static const char system_prefix[] = "/bin/sh -n -c \'";
    static const char system_postfix[] = "\' &> /dev/null";
    char system_wrapper[cmd_len + sizeof(system_prefix) + sizeof(system_postfix)];

    strncpy(system_wrapper, system_prefix, sizeof(system_prefix));
    strncat(system_wrapper, command, cmd_len);
    strncat(system_wrapper, system_postfix, sizeof(system_postfix));
    int exit_code = system(system_wrapper);

    if (exit_code != EXIT_SUCCESS)
    {
        puts("INVALID COMMAND SYNTAX");
        return PASTA_ERROR_INVALID_COMMAND_SYNTAX;
    }

    strncpy(module_p->command, command, cmp_at_most);

    puts("SUCCESS");
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

