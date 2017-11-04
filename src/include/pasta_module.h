/*
 * pasta_module.h
 *
 * Describes an instance of a module. A module has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef PASTA_MODULE_H
#define PASTA_MODULE_H

#include <stdarg.h>

enum ModuleState
{
    Running,
    Stopped
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

/*
 * pasta_module_create
 *
 * Allocates a new Module struct instance and returns a pointer to it.
 *
 * Allocates space for name and command members in the Module struct.
 *
 * A call to pasta_module_create should always be paired with a call to
 * pasta_module_destroy
 */
Module *pasta_module_create();

/*
 * Frees the memory allocated for the provided module, as well as the memory
 * allocated for the name and command members.
 */
void pasta_module_destroy(Module *const module_p);

void pasta_module_set_name(Module *const module_p, const char *name);
void pasta_module_set_command(Module *const module_p, const char *command);
void pasta_module_set_interval(Module *const module_p, int interval);
void pasta_module_set_state(Module *const module_p, ModuleState state);

void pasta_module_print(const Module *const module_p);

#endif /* PASTA_MODULE_H */
