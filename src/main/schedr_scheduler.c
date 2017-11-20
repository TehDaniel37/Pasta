#include <sys/types.h>      // pid_t
#include <stdlib.h>         // getenv()
#include <unistd.h>         // fork(), execve(), sleep()
#include <sys/wait.h>       // waitpid()
#include <stdbool.h>        // false

#include "schedr_scheduler.h"

#define SHELL_ERROR_CMD_NOT_FOUND 127

extern char *environ[];

static void (*on_fork_hook)(void) = NULL;

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;
static int (*forker)(void) = fork;

static pid_t temp_job_pid;

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void schedr_scheduler_reset_exec() { exec = execve; }
void schedr_scheduler_set_forker(int (*fork_func)(void)) { forker = fork_func; }
void schedr_scheduler_reset_forker() { forker = fork; }
void schedr_scheduler_set_on_fork_hook(void (*on_fork)(void)) { on_fork_hook = on_fork; }
void schedr_scheduler_remove_on_fork_hook() { on_fork_hook = NULL; }
pid_t schedr_scheduler_get_child_pid() { return temp_job_pid; }
void __gcov_flush();
#endif

static void cmd_proc(Job *job_p)
{
    char *shell = getenv("SHELL");
    char *argv[] = { shell, "-c", job_p->command, NULL };
    
    exec(argv[0], argv, environ);
    
    #ifdef TEST
    __gcov_flush();
    #endif
    
    _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
}

static void child_proc(Job *job_p, int pipe)
{
    pid_t cmd_pid;
    int exit_status = 0;
    bool pipe_closed = false;

    if (on_fork_hook != NULL) { on_fork_hook(); }

    while (exit_status == 0)
    {
        if ((cmd_pid = forker()) < 0) 
        {
            exit_status = SCHEDR_ERROR_FORK_FAILED;
            
            if (!pipe_closed)
            {
                write(pipe, &exit_status, sizeof(exit_status));
                close(pipe);
            }
            
            #ifdef TEST
            __gcov_flush();
            #endif
            
            _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
        }
        else if (cmd_pid == 0)
        {
            close(pipe);

            cmd_proc(job_p);
        }
        else 
        {
            int cmd_status;
            
            waitpid(cmd_pid, &cmd_status, 0);
            exit_status = WEXITSTATUS(cmd_status);
            
            if (!pipe_closed)
            {
                write(pipe, &exit_status, sizeof(exit_status));
                close(pipe);
            }
            
            if (exit_status != 0)
            {
                #ifdef TEST
                __gcov_flush();
                #endif
            
                _Exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
            }
            else
            {
                sleep(job_p->interval_seconds);
            }
        }
    }
}

static int parent_proc(Job *job_p, int pipe)
{
    int buffer;

    if (on_fork_hook != NULL) { on_fork_hook(); }
    read(pipe, &buffer, sizeof(buffer));

    if (buffer == 0) 
    {
        job_p->state = Running;
        close(pipe);

        return SCHEDR_SUCCESS;
    }
    else 
    {
        close(pipe);
        
        switch (buffer)
        {
            case SHELL_ERROR_CMD_NOT_FOUND:
                return SCHEDR_ERROR_JOB_COMMAND_NOT_FOUND;
            
            case SCHEDR_ERROR_FORK_FAILED:
                return buffer;
                
            default:
                return SCHEDR_FAILURE;
        }
    }
}

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

    if ((job_pid = forker()) < 0) 
    {
        close(file_desc_parent);
        close(file_desc_child);
         
        return SCHEDR_ERROR_FORK_FAILED;
    }
    else if (job_pid == 0)          // executed by child
    {
        close(file_desc_parent);    // Close parent end of pipe since child doesn't need it
        child_proc(job_p, file_desc_child);
    }
    else
    {
        temp_job_pid = job_pid;
        close(file_desc_child);     // Close child end of pipe since parent doesn't need it
        return parent_proc(job_p, file_desc_parent);
    }
    
    return SCHEDR_FAILURE; // GCOVR_EXCL_LINE (will never be executed)
}

Status schedr_scheduler_stop_job(Job *job_p)
{
    return SCHEDR_ERROR_NOT_IMPLEMENTED;
}
