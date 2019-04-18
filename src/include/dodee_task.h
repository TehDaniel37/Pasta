/*
 * dodee_task.h
 *
 * Describes an instance of a task. A task has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef DODEE_TASK_H
#define DODEE_TASK_H

#include <stddef.h> // size_t

#include "dodee_status_codes.h" // Status

#define DODEE_TASK_HASH_LEN 32
#define DODEE_TASK_STATE_VALUES 2
#define DODEE_TASK_SCRIPT_ARG_LEN 256

enum DodeeTaskState
{
    Running = 0,
    Stopped = 1
};

typedef enum DodeeTaskState DodeeTaskState;

struct DodeeTaskScript
{
    FILE *file;
    char *arguments[DODEE_TASK_SCRIPT_ARG_LEN + 1];
};

typedef struct DodeeTaskScript DodeeTaskScript;

struct DodeeTask 
{
    char hash[DODEE_TASK_HASH_LEN + 1];
    DodeeTaskScript *interval;
    DodeeTaskScript **events;
    DodeeTaskScript **actions;
    DodeeTaskState state;
};

typedef struct DodeeTask DodeeTask;

/*
 * Initializes a task to default values. 
 *
 * Default values are: 
 * hash: "", interval: NULL, events: NULL, actions: NULL, state: Stopped
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' is NULL,
 *          DODEE_SUCCESS otherwise
 */
DodeeStatus dodee_task_init(DodeeTask *const task_p);

DodeeStatus dodee_task_set_interval(DodeeTask *const task_p, DodeeTaskScript *const interval_p);

DodeeStatus dodee_task_add_event(DodeeTask *const task_p, DodeeTaskScript *const event_p);

DodeeStatus dodee_task_add_action(DodeeTask *const task_p, DodeeTaskScript *const action_p);

DodeeStatus dodee_task_generate_md5sum(DodeeTask *const task_p);

/*
 * Sets the state of a task.
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' is NULL,
 *          DODEE_ERROR_INVALID_ARGUMENT if state is < 0 or >= DODEE_TASK_STATE_VALUES
 *          DODEE_SUCCESS, otherwise
 */
DodeeStatus dodee_task_set_state(DodeeTask *const task_p, DodeeTaskState state);

#endif /* DODEE_TASK_H */
