#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>         // getenv
#include <unistd.h>         // fork

#include "schedr_scheduler.h"

extern char *environ[];

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void schedr_scheduler_reset_exec() { exec = execve; }
#endif

Status schedr_scheduler_start_job(Job *job_p)
{
    pid_t job_pid = fork();

    if (job_pid == 0)        // executed by child
    {
        char *shell = getenv("SHELL");

        char *argv[] = { shell, "-c", job_p->command, NULL };
        exec(argv[0], argv, environ);
        _Exit(EXIT_FAILURE);
    }
    else                        // executed by parent
    {
    }

    return SCHEDR_SUCCESS;
}

Status schedr_scheduler_stop_job(Job *job_p)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}
