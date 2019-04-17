/*
 * dodee_task.h
 *
 * Describes an instance of a task. A task has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef DODEE_JOB_H
#define DODEE_JOB_H

#include <stddef.h> // size_t

#include "dodee_status_codes.h" // Status

#define DODEE_TASK_MAX_NAME_LEN 100
#define DODEE_TASK_MAX_CMD_LEN 1000
#define DODEE_TASK_STATE_VALUES 2

enum DodeeTaskState
{
    Running = 0,
    Stopped = 1
};

typedef enum DodeeTaskState DodeeTaskState;

struct DodeeTask 
{
    char name[DODEE_TASK_MAX_NAME_LEN + 1];
    char command[DODEE_TASK_MAX_CMD_LEN + 1];
    int interval_seconds;
    DodeeTaskState state;
};

typedef struct DodeeTask DodeeTask;

/*
 * Initializes a task to default values. 
 *
 * Default values are: 
 * name: "", command: "", interval_seconds: 0, state: Stopped
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' is NULL,
 *          DODEE_SUCCESS otherwise
 */
DodeeStatus dodee_task_init(DodeeTask *const task_p);

/*
 * Sets the name of a task.
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' or 'name' is NULL,
 *          DODEE_ERROR_INVALID_ARGUMENT if 'name' is empty, 'name_len' is 0 or 'name' contains non printable ASCII symbols
 *          DODEE_ERROR_BUFFER_OVERFLOW if 'name_len' is > DODEE_TASK_MAX_NAME_LEN,
 *          DODEE_SUCCESS otherwise
 */
DodeeStatus dodee_task_set_name(DodeeTask *const task_p, const char *name, size_t name_len);

/*
 * Sets the command of a task.
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' or 'name' is NULL,
 *          DODEE_ERROR_INVALID_ARGUMENT if 'command' is empty or 'cmd_len' is 0,
 *          DODEE_ERROR_BUFFER_OVERFLOW if 'cmd_len' is > DODEE_TASK_MAX_CMD_LEN,
 *          DODEE_SUCCESS otherwise
 */
DodeeStatus dodee_task_set_command(DodeeTask *const task_p, const char *command, size_t cmd_len);

/*
 * Sets the interval, in seconds, of a task. The interval specifies how often the command associated with a task 
 * should be executed.
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' is NULL,
 *          DODEE_ERROR_INVALID_ARGUMENT if interval is < 0
 *          DODEE_SUCCESS, otherwise
 */
DodeeStatus dodee_task_set_interval(DodeeTask *const task_p, int interval);

/*
 * Sets the state of a task.
 *
 * returns  DODEE_ERROR_NULL_ARGUMENT if 'task_p' is NULL,
 *          DODEE_ERROR_INVALID_ARGUMENT if state is < 0 or >= DODEE_TASK_STATE_VALUES
 *          DODEE_SUCCESS, otherwise
 */
DodeeStatus dodee_task_set_state(DodeeTask *const task_p, DodeeTaskState state);

#endif /* DODEE_JOB_H */
