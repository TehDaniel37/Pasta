/*
 * schedr_job.h
 *
 * Describes an instance of a job. A job has a name, a command to run,
 * an interval in seconds for how often it is to run and a state, indicating
 * if it is currently running or stopped.
 */
#ifndef SCHEDR_JOB_H
#define SCHEDR_JOB_H

#include <stddef.h> // size_t

#include "schedr_status_codes.h" // Status

#define SCHEDR_JOB_MAX_NAME_LEN 100
#define SCHEDR_JOB_MAX_CMD_LEN 1000
#define SCHEDR_JOB_STATE_VALUES 2

enum JobState
{
    Running = 0,
    Stopped = 1
};

typedef enum JobState JobState;

struct Job 
{
    char name[SCHEDR_JOB_MAX_NAME_LEN + 1];
    char command[SCHEDR_JOB_MAX_CMD_LEN + 1];
    int interval_seconds;
    JobState state;
};

typedef struct Job Job;

/*
 * Initializes a job to default values. 
 *
 * Default values are: 
 * name: "", command: "", interval_seconds: 0, state: Stopped
 *
 * returns  SCHEDR_ERROR_NULL_ARGUMENT if 'job_p' is NULL,
 *          SCHEDR_SUCCESS otherwise
 */
Status schedr_job_init(Job *const job_p);

/*
 * Sets the name of a job.
 *
 * returns  SCHEDR_ERROR_NULL_ARGUMENT if 'job_p' or 'name' is NULL,
 *          SCHEDR_ERROR_INVALID_ARGUMENT if 'name' is empty, 'name_len' is 0 or 'name' contains non printable ASCII symbols
 *          SCHEDR_ERROR_BUFFER_OVERFLOW if 'name_len' is > SCHEDR_JOB_MAX_NAME_LEN,
 *          SCHEDR_SUCCESS otherwise
 */
Status schedr_job_set_name(Job *const job_p, const char *name, size_t name_len);

/*
 * Sets the command of a job.
 *
 * returns  SCHEDR_ERROR_NULL_ARGUMENT if 'job_p' or 'name' is NULL,
 *          SCHEDR_ERROR_INVALID_ARGUMENT if 'command' is empty or 'cmd_len' is 0,
 *          SCHEDR_ERROR_BUFFER_OVERFLOW if 'cmd_len' is > SCHEDR_JOB_MAX_CMD_LEN,
 *          SCHEDR_SUCCESS otherwise
 */
Status schedr_job_set_command(Job *const job_p, const char *command, size_t cmd_len);

/*
 * Sets the interval, in seconds, of a job. The interval specifies how often the command associated with a job 
 * should be executed.
 *
 * returns  SCHEDR_ERROR_NULL_ARGUMENT if 'job_p' is NULL,
 *          SCHEDR_ERROR_INVALID_ARGUMENT if interval is < 0
 *          SCHEDR_SUCCESS, otherwise
 */
Status schedr_job_set_interval(Job *const job_p, int interval);

/*
 * Sets the state of a job.
 *
 * returns  SCHEDR_ERROR_NULL_ARGUMENT if 'job_p' is NULL,
 *          SCHEDR_ERROR_INVALID_ARGUMENT if state is < 0 or >= SCHEDR_JOB_STATE_VALUES
 *          SCHEDR_SUCCESS, otherwise
 */
Status schedr_job_set_state(Job *const job_p, JobState state);

#endif /* SCHEDR_JOB_H */
