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

#include "pasta_status_codes.h"

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
    char name[PASTA_MODULE_MAX_NAME_LEN + 1];
    char command[PASTA_MODULE_MAX_CMD_LEN + 1];
    int interval_seconds;
    ModuleState state;
};

typedef struct Module Module;

Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len);
Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len);
Status pasta_module_set_interval(Module *const module_p, int interval);
Status pasta_module_set_state(Module *const module_p, ModuleState state);

#endif /* PASTA_MODULE_H */
