/*
 * pasta_module.h
 *
 * Describes an instance of a module. A module has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef PASTA_MODULE_H
#define PASTA_MODULE_H

static const int NAME_MAX_CHARS = 100;
static const int COMMAND_MAX_CHARS = 1000;

enum ModuleState
{
    Running,
    Stopped
};

typedef struct ModuleState ModuleState;

struct Module 
{
    char name[NAME_MAX_CHARS + 1];
    int interval_seconds;
    char command[COMMAND_MAX_CHARS + 1];
    ModuleState state;
};

typedef struct Module Module;

#endif /* PASTA_MODULE_H */
