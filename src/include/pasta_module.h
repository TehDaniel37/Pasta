/*
 * pasta_module.h
 *
 * Describes an instance of a module. A module has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef PASTA_MODULE_H
#define PASTA_MODULE_H

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
    char *name;
    char *command;
    int interval_seconds;
    ModuleState state;
};

typedef struct Module Module;

#ifdef TEST
#include <stddef.h> // size_t

void pasta_module_set_allocator(void *(*alloc_func)(size_t bytes));
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
Status pasta_module_create(Module *module_p);

/*
 * Frees the memory allocated for the provided module, as well as the memory
 * allocated for the name and command members.
 */
Status pasta_module_destroy(Module *const module_p);

Status pasta_module_set_name(Module *const module_p, const char *name);
Status pasta_module_set_command(Module *const module_p, const char *command);
Status pasta_module_set_interval(Module *const module_p, int interval);
Status pasta_module_set_state(Module *const module_p, ModuleState state);

#endif /* PASTA_MODULE_H */
