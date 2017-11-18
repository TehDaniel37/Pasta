#include <sys/types.h>      // pid_t
#include <stdlib.h>         // getenv()
#include <unistd.h>         // fork(), execve(), sleep()
#include <sys/wait.h>       // waitpid()
#include <stdbool.h>        // false

#include "schedr_scheduler.h"

extern char *environ[];

static pid_t job_pid;
static volatile sig_atomic_t job_ready = false; 

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void schedr_scheduler_reset_exec() { exec = execve; }
pid_t schedr_scheduler_get_job_pid(Job *job_p) { return job_pid; }
#endif

static void on_job_ready(int signal) { job_ready = true; }

Status schedr_scheduler_start_job(Job *job_p)
{
    job_pid = fork();

    signal(SIGUSR1, on_job_ready);

    if (job_pid == 0)        // executed by child
    {
        char *shell = getenv("SHELL");

        char *argv[] = { shell, "-c", job_p->command, NULL };
        kill(getppid(), SIGUSR1);
        exec(argv[0], argv, environ);
        _Exit(EXIT_FAILURE);
    }
    else
    {
        while (!job_ready)
        {
            sleep(1);
        }

        sleep(1);
    }

    return SCHEDR_SUCCESS;
}

Status schedr_scheduler_stop_job(Job *job_p)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}
