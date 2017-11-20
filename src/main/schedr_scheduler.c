#include <sys/types.h>      // pid_t
#include <stdlib.h>         // getenv()
#include <unistd.h>         // fork(), execve(), sleep()
#include <sys/wait.h>       // waitpid()
#include <stdbool.h>        // false

#include "schedr_scheduler.h"

#define SHELL_ERROR_CMD_NOT_FOUND 127

extern char *environ[];

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;
static int (*forker)(void) = fork;

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void schedr_scheduler_reset_exec() { exec = execve; }
void schedr_scheduler_set_forker(int (*fork_func)(void)) { forker = fork_func; }
void schedr_scheduler_reset_forker() { forker = fork; }
void __gcov_flush();
#endif

Status schedr_scheduler_start_job(Job *job_p)
{
    int pipe_return;
    int pipe_file_descs[2];
    int file_desc_parent;
    int file_desc_child;
    pid_t job_pid;
    
    if ((pipe_return = pipe(pipe_file_descs)) < 0) { return SCHEDR_FAILURE; }

    file_desc_parent = pipe_file_descs[0];
    file_desc_child = pipe_file_descs[1];

    if ((job_pid = fork()) < 0) { return SCHEDR_FAILURE; }
    else if (job_pid == 0)          // executed by child
    {
        close(file_desc_parent);    // Close parent end of pipe since child doesn't need it

        pid_t cmd_pid;
        int exit_status;

        if ((cmd_pid = fork()) < 0) 
        {
            exit_status = EXIT_FAILURE;
            write(file_desc_child, &exit_status, sizeof(exit_status));
            close(file_desc_child);
            
            #ifdef TEST
            __gcov_flush();
            #endif
            
            _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
        }
        else if (cmd_pid == 0)
        {
            close(file_desc_child);

            char *shell = getenv("SHELL");
            char *argv[] = { shell, "-c", job_p->command, NULL };
            
            exec(argv[0], argv, environ);
            
            #ifdef TEST
            __gcov_flush();
            #endif
            
            _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
        }
        else 
        {
            int cmd_status;

            waitpid(cmd_pid, &cmd_status, 0);
            exit_status = WEXITSTATUS(cmd_status);
            write(file_desc_child, &exit_status, sizeof(exit_status));
            close(file_desc_child);
            
            #ifdef TEST
            __gcov_flush();
            #endif
            
            _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
        }
    }
    else
    {
        int buffer;

        close(file_desc_child);     // Close child end of pipe since parent doesn't need it

        read(file_desc_parent, &buffer, sizeof(buffer));

        if (buffer == 0) 
        {
            job_p->state = Running;
            close(file_desc_parent);

            return SCHEDR_SUCCESS;
        }
        else 
        {
            close(file_desc_parent);

            if (buffer == SHELL_ERROR_CMD_NOT_FOUND ) { return SCHEDR_ERROR_JOB_COMMAND_NOT_FOUND; }
            else { return SCHEDR_FAILURE; }
        }
    }
}

Status schedr_scheduler_stop_job(Job *job_p)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}
