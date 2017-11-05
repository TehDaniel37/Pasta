#include <stdlib.h>
#include <string.h>

#include "pasta_module.h"

static const char FIRST_NON_NULL_ASCII_CHAR = '\x1';
static const char FIRST_PRINTABLE_ASCII_CHAR = ' ';

// Prototypes
static void hook_destroy_before_struct_free_default(char *name, char *command);

// Hooks
static void *(*allocator)(size_t bytes) = malloc;
static void (*hook_destroy_before_struct_free)(char *name, char *command) = hook_destroy_before_struct_free_default;

#ifdef TEST
void pasta_module_set_allocator(void *(*alloc_func)(size_t bytes)) 
{
    allocator = alloc_func;
}

void pasta_module_hook_into_destroy_before_struct_free(void (*hook)(char *name, char *command))
{
    hook_destroy_before_struct_free = hook;
}

void pasta_module_clear_destroy_hooks()
{
    hook_destroy_before_struct_free = hook_destroy_before_struct_free_default;
}
#endif

Status pasta_module_create(Module **module_pp)
{
    if (module_pp == NULL || *module_pp != NULL)
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    Module *new_module = (Module *)allocator(sizeof (Module)); 

    if (new_module == NULL)
    {
        return PASTA_ERROR_MALLOC_FAIL;
    }

    new_module->command = NULL;
    new_module->name = NULL;

    *module_pp = new_module;

    return PASTA_SUCCESS;
}

Status pasta_module_destroy(Module **module_pp)
{
    if (module_pp == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    free((*module_pp)->name);
    (*module_pp)->name = NULL;

    free((*module_pp)->command);
    (*module_pp)->command = NULL;

    hook_destroy_before_struct_free((*module_pp)->name, (*module_pp)->command);

    free(*module_pp);
    *module_pp = NULL;

    return PASTA_SUCCESS;
}

Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len)
{
    if (module_p == NULL || name == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    size_t cmp_at_most = (PASTA_MODULE_MAX_NAME_LEN <= name_len) ?
                          PASTA_MODULE_MAX_NAME_LEN + 1: 
                          name_len ;

    if (name[0] == '\0')
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    for (int char_index = 0; char_index < name_len; char_index++)
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

    if (strnlen(name, cmp_at_most) == cmp_at_most)
    {
        return PASTA_ERROR_BUFFER_OVERFLOW;
    }

    if (module_p->name == NULL)
    {
        char *module_name = (char *)allocator(name_len);

        if (module_name == NULL)
        {
            return PASTA_ERROR_MALLOC_FAIL;
        }

        module_p->name = module_name;
    }

    strncpy(module_p->name, name, cmp_at_most);

    return PASTA_SUCCESS;
}

Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len)
{
    if (module_p == NULL || command == NULL)
    {
        return PASTA_ERROR_NULL_ARGUMENT;
    }

    size_t cmp_at_most = (PASTA_MODULE_MAX_CMD_LEN <= cmd_len) ?
                          PASTA_MODULE_MAX_CMD_LEN + 1: 
                          cmd_len ;

    if (command[0] == '\0')
    {
        return PASTA_ERROR_INVALID_ARGUMENT;
    }

    if (strnlen(command, cmp_at_most) == cmp_at_most)
    {
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
        return PASTA_ERROR_INVALID_COMMAND_SYNTAX;
    }

    if (module_p->command == NULL)
    {
        char *mod_cmd = (char *)allocator(cmd_len);

        if (mod_cmd == NULL)
        {
            return PASTA_ERROR_MALLOC_FAIL;
        }

        module_p->name = mod_cmd;
    }

    strncpy(module_p->command, command, cmp_at_most);

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

static void hook_destroy_before_struct_free_default(char *name, char *command){ }

