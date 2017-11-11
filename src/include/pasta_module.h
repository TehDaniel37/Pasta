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

#include "pasta_status_codes.h" // Status

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

/*
 * Initializes a module to default values. 
 *
 * Default values are: 
 * name: "", command: "", interval_seconds: 0, state: Stopped
 *
 * returns  PASTA_ERROR_NULL_ARGUMENT if 'module_p' is NULL,
 *          PASTA_SUCCESS otherwise
 */
Status pasta_module_init(Module *const module_p);

/*
 * Sets the name of a module.
 *
 * returns  PASTA_ERROR_NULL_ARGUMENT if 'module_p' or 'name' is NULL,
 *          PASTA_ERROR_INVALID_ARGUMENT if 'name' is empty, 'name_len' is 0 or 'name' contains non printable ASCII symbols
 *          PASTA_ERROR_BUFFER_OVERFLOW if 'name_len' is > PASTA_MODULE_MAX_NAME_LEN,
 *          PASTA_SUCCESS otherwise
 */
Status pasta_module_set_name(Module *const module_p, const char *name, size_t name_len);

/*
 * Sets the command of a module.
 *
 * returns  PASTA_ERROR_NULL_ARGUMENT if 'module_p' or 'name' is NULL,
 *          PASTA_ERROR_INVALID_ARGUMENT if 'command' is empty or 'cmd_len' is 0,
 *          PASTA_ERROR_BUFFER_OVERFLOW if 'cmd_len' is > PASTA_MODULE_MAX_CMD_LEN,
 *          PASTA_SUCCESS otherwise
 */
Status pasta_module_set_command(Module *const module_p, const char *command, size_t cmd_len);

/*
 * Sets the interval, in seconds, of a module. The interval specifies how often the command associated with a module
 * should be executed.
 *
 * returns  PASTA_ERROR_NULL_ARGUMENT if 'module_p' is NULL,
 *          PASTA_ERROR_INVALID_ARGUMENT if interval is < 0
 *          PASTA_SUCCESS, otherwise
 */
Status pasta_module_set_interval(Module *const module_p, int interval);

/*
 * Sets the state of a module.
 *
 * returns  PASTA_ERROR_NULL_ARGUMENT if 'module_p' is NULL,
 *          PASTA_ERROR_INVALID_ARGUMENT if state is < 0 or >= PASTA_MODULE_STATE_VALUES
 *          PASTA_SUCCESS, otherwise
 */
Status pasta_module_set_state(Module *const module_p, ModuleState state);

#endif /* PASTA_MODULE_H */
