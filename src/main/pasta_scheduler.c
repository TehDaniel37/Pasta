#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>

#include "pasta_scheduler.h"

static volatile sig_atomic_t module_has_started;

static void on_module_started(int signal);
static void on_stop_module(int signal);

Status pasta_scheduler_start_module(Module *module_p)
{
    // TODO:    - Need some way to store the PIDs of the started modules. Map?
    //            Store pid in Module struct?
    //          - Look up best practices for setting up signal handlers and
    //            sending signals

    module_has_started = false;
    
    // set signal handler to 'on_module_started'

    pid_t module_pid = fork();

    if (module_p < 0)                // error, could not fork
    {
        // find cause of error and return appropriate error code
    }
    else if (module_pid == 0)        // executed by child
    {
        // set signal handler for TERM (stop) to 'on_stop_module'

        // signal to parent that process is ready

        // until request to stop module received:
        //     fork() and exec() the Module command     (will this be slow? alternative?)
        //     wait for child to complete (waitpid)     (what happens if child doesn't finish?)
        //     sleep for Module interval_seconds
    }
    else                        // executed by parent
    {
        // wait until module has signaled that it has started

        module_has_started = false;     // reset flag for next module
    }

    return PASTA_SUCCESS;
}

Status pasta_scheduler_stop_module(Module *module_p)
{
    // Look up the PID of the specified module

    // Send SIGTERM to that module

    return PASTA_ERROR_NOT_IMPLEMENTED;
}

static void on_module_started(int signal)
{
    module_has_started = true;
    
    // re-enable signal?
}

static void on_stop_module(int signal)
{
    // perform cleanup:
    //     terminate child proccess if running
    //
    // exit
}
