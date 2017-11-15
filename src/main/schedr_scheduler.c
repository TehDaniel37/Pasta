#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>         // getenv
#include <unistd.h>         // fork

#include "schedr_scheduler.h"

extern char *environ[];

static volatile sig_atomic_t job_has_started;

static pid_t job_pid;

#ifdef TEST
pid_t schedr_scheduler_get_job_pid(const Job *const job)
{
    if (job_has_started) { return job_pid; }
    else { return -1; }
}
#endif

// static void on_job_started(int signal);
// static void on_stop_job(int signal);

Status schedr_scheduler_start_job(Job *job_p)
{
    // TODO:    - Need some way to store the PIDs of the started jobs. Map?
    //            Store pid in Job struct?
    //          - Look up best practices for setting up signal handlers and
    //            sending signals

    job_has_started = false;
    
    // set signal handler to 'on_job_started'

    job_pid = fork();

    //if (job_pid < 0)                // error, could not fork
    //{
    //    // find cause of error and return appropriate error code
    //}
    /*else*/ if (job_pid == 0)        // executed by child
    {
        // set signal handler for TERM (stop) to 'on_stop_job'

        pid_t cmd_pid = fork();

        if (cmd_pid == 0)
        {
            char *shell = getenv("SHELL");

            char *argv[] = { shell, "-c", job_p->command, NULL };
            execve(argv[0], argv, environ);
            _Exit(EXIT_FAILURE);
        }
        else
        {
            while (1) { sleep(1); } 
        }
        // signal to parent that process is ready

        // until request to stop job received:
        //     fork() and exec() the Job command     (will this be slow? alternative?)
        //     wait for child to complete (waitpid)     (what happens if child doesn't finish?)
        //     sleep for Job interval_seconds
    }
    else                        // executed by parent
    {
        // wait until job has signaled that it has started

        job_has_started = true;     // reset flag for next job

        // Set job state to Running
        // Store child PID
    }

    return SCHEDR_SUCCESS;
}

Status schedr_scheduler_stop_job(Job *job_p)
{
    // Look up the PID of the specified job

    // Send SIGTERM to that job

    // Set job state to Stopped

    // Remove child PID

    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}

// static void on_job_started(int signal)
// {
//     // job_has_started = true;
//     
//     // re-enable signal?
// }
// 
// static void on_stop_job(int signal)
// {
//     // perform cleanup:
//     //     terminate child proccess if running
//     //
//     // _Exit or _exit
// }
