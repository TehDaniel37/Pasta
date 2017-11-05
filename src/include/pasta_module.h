/*
 * pasta_module.h
 *
 * Describes an instance of a module. A module has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef PASTA_MODULE_H
#define PASTA_MODULE_H

#include <stddef.h> // size_t

#include "pasta_error.h"

#define PASTA_MODULE_MAX_NAME_LEN 100
#define PASTA_MODULE_MAX_CMD_LEN 1000
#define PASTA_MODULE_STATE_VALUES 2

enum ModuleState
{
    Running = 0,
    Stopped = 1
};

typedef enum ModuleState ModuleState;

struct Module 
{
    // TODO: Consider letting name and command be stack-allocated.
    //       Pros seem to vastly outweigh cons. Maybe there are limitations
    //       for stack-allocated struct members?
    char *name;
    char *command;
    int interval_seconds;
    ModuleState state;
};

typedef struct Module Module;

#ifdef TEST
void pasta_module_set_allocator(void *(*alloc_func)(size_t bytes));

// Hooks
void pasta_module_hook_into_destroy_before_struct_free(void (*hook)(char *name, char *command));
void pasta_module_clear_destroy_hooks();
#endif

/*
 * pasta_module_create
 *
 * Allocates a new Module struct instance and points the provided Module pointer
 * to it.
 *
 * Allocates space for name and command members in the Module struct.
 *
 * A call to pasta_module_create should always be paired with a call to
 * pasta_module_destroy
 */
Status pasta_module_create(Module **module_pp);

/*
 * Frees the memory allocated for the provided module, as well as the memory
 * allocated for the name and command members.
 */
Status pasta_module_destroy(Module **module_pp);

Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len);
Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len);
Status pasta_module_set_interval(Module *const module_p, int interval);
Status pasta_module_set_state(Module *const module_p, ModuleState state);

#endif /* PASTA_MODULE_H */
