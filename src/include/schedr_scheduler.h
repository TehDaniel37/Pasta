/*
 * schedr_scheduler.h
 *
 * Responsible for starting, managing and stopping jobs.
 */
#ifndef SCHEDR_SCHEDULER_H
#define SCHEDR_SCHEDULER_H

#include <schedr_job.h>
#include <schedr_status_codes.h>

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[]));
void schedr_scheduler_reset_exec();
pid_t schedr_scheduler_get_job_pid(Job *job_p);
#endif

/*
 * schedr_scheduler_start_job
 *
 * Starts a new process that manages the provided job, executing it with the
 * interval provided in the job.
 *
 */
Status schedr_scheduler_start_job(Job *const job_p);

/*
 * schedr_scheduler_stop_job
 *
 * Stops the provided job if it is running.
 */
Status schedr_scheduler_stop_job(Job *const job_p);

#endif /* SCHEDR_SCHEDULER_H */
