/*
 * dodee_scheduler.h
 *
 * Responsible for starting, managing and stopping tasks.
 */
#ifndef DODEE_SCHEDULER_H
#define DODEE_SCHEDULER_H

#include <dodee_task.h>
#include <dodee_status_codes.h>

extern char **environ;

#ifdef TEST
#include <sys/types.h>

void dodee_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[]));
void dodee_scheduler_reset_exec();
void dodee_scheduler_set_forker(int (*fork_func)(void));
void dodee_scheduler_reset_forker();
void dodee_scheduler_set_sleeper(unsigned int (*sleep_func)(unsigned int seconds));
void dodee_scheduler_reset_sleeper();
void dodee_scheduler_kill_children();
void dodee_scheduler_associate_pid_with_task(DodeeTask *const task, pid_t pid);
#endif

/*
 * dodee_scheduler_start_task
 *
 * Starts a new process that manages the provided task, executing it with the
 * interval provided in the task.
 *
 */
DodeeStatus dodee_scheduler_start_task(DodeeTask *const job_p);

/*
 * dodee_scheduler_stop_task
 *
 * Stops the provided task if it is running.
 */
DodeeStatus dodee_scheduler_stop_task(DodeeTask *const job_p);

void dodee_scheduler_set_path();

#endif /* DODEE_SCHEDULER_H */
