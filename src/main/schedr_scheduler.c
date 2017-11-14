#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>         // fork

#include "schedr_scheduler.h"

#ifdef TEST
pid_t schedr_scheduler_get_job_pid(const Job *const job)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}
#endif

// static volatile sig_atomic_t job_has_started;

// static void on_job_started(int signal);
// static void on_stop_job(int signal);

Status schedr_scheduler_start_job(Job *job_p)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
    
    // TODO:    - Need some way to store the PIDs of the started jobs. Map?
    //            Store pid in Job struct?
    //          - Look up best practices for setting up signal handlers and
    //            sending signals

    // job_has_started = false;
    
    // set signal handler to 'on_job_started'

    // pid_t job_pid = fork();

    // if (job_pid < 0)                // error, could not fork
    // {
    //     // find cause of error and return appropriate error code
    // }
    // else if (job_pid == 0)        // executed by child
    // {
    //     // set signal handler for TERM (stop) to 'on_stop_job'

    //     // signal to parent that process is ready

    //     // until request to stop job received:
    //     //     fork() and exec() the Job command     (will this be slow? alternative?)
    //     //     wait for child to complete (waitpid)     (what happens if child doesn't finish?)
    //     //     sleep for Job interval_seconds
    // }
    // else                        // executed by parent
    // {
    //     // wait until job has signaled that it has started

    //     job_has_started = false;     // reset flag for next job

    //     // Set job state to Running
    //     // Store child PID
    // }

    // return SCHEDR_SUCCESS;
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
