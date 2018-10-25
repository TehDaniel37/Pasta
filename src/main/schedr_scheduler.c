#include <sys/types.h>      // pid_t
#include <stdlib.h>         // getenv()
#include <unistd.h>         // fork(), execve(), sleep()
#include <sys/wait.h>       // waitpid()
#include <stdbool.h>        // false
#include <string.h>
#include <linux/limits.h>   // PATH_MAX
#include <sys/stat.h>       // mkdir()

#include "schedr_scheduler.h"

#define MAX_RUNNING_JOBS 100
static int started_jobs_count = 0;

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;
static int (*forker)(void) = fork;
static unsigned int (*sleeper)(unsigned int seconds) = sleep;

struct JobProcMap 
{
    Job *job;
    pid_t pid;
};

typedef struct JobProcMap JobProcMap;

static JobProcMap started_jobs[MAX_RUNNING_JOBS];

#ifdef TEST
void schedr_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void schedr_scheduler_reset_exec() { exec = execve; }
void schedr_scheduler_set_forker(int (*fork_func)(void)) { forker = fork_func; }
void schedr_scheduler_reset_forker() { forker = fork; }
void schedr_scheduler_set_sleeper(unsigned int (*sleep_func)(unsigned int seconds)) { sleeper = sleep_func; }
void schedr_scheduler_reset_sleeper() { sleeper = sleep; }

void schedr_scheduler_kill_children() 
{
    for (int i = started_jobs_count - 1; i >= 0; i--)
    {
        pid_t pid = started_jobs[i].pid;
        
        kill(started_jobs[i].pid, SIGTERM);
        waitpid(pid, NULL, 0);
        
        started_jobs[i].job = NULL;
        started_jobs[i].pid = 0;
    }
    
    started_jobs_count = 0;
}

void schedr_scheduler_associate_pid_with_jod(Job *const job, pid_t pid) 
{ 
    started_jobs[started_jobs_count].job = job;
    started_jobs[started_jobs_count].pid = pid;
    started_jobs_count++;
}

void __gcov_flush();
#endif

static void cmd_proc(Job *job_p)
{
    char *shell = getenv("SHELL");
    char *argv[] = { shell, "-c", job_p->command, NULL };

    #ifdef TEST
    __gcov_flush();
    #endif
    
    exec(argv[0], argv, environ);   // GCOVR_EXCL_LINE
    
    _exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
}

static int start_job_cmd(Job *job_p)
{
    pid_t cmd_pid;
    
    if ((cmd_pid = forker()) < 0) 
    {
        /*
         * For some reason the following line is not reported as executed by GCOV
         * when running the tests, even though a printf statement verifies 
         * (on 2017-11-20) that it is. Probably has something to do with the use of
         * fork() as GCOV also has trouble with the use _Exit/_exit when terminating
         * a child process.  
         */
         
        return SCHEDR_ERROR_FORK_FAILED;    // GCOVR_EXCL_LINE
    }
    else if (cmd_pid == 0)
    {
        cmd_proc(job_p);    // will not return
        
        return SCHEDR_FAILURE;  // GCOVR_EXCL_LINE  (return statement added to silence compiler)
    }
    else 
    {
        int cmd_status;
        waitpid(cmd_pid, &cmd_status, 0);
        return WEXITSTATUS(cmd_status);
    }
}

static void child_proc(Job *job_p)
{
    int cmd_status = EXIT_SUCCESS;
    
    while (cmd_status == EXIT_SUCCESS)
    {
        cmd_status = start_job_cmd(job_p);
        
        if (cmd_status == EXIT_SUCCESS) 
        {
            sleeper(job_p->interval_seconds);
        }
    }
    
    #ifdef TEST
    __gcov_flush();
    #endif
    
    _exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
}

static Status parent_proc(Job *job_p)
{
    job_p->state = Running;
    
    return SCHEDR_SUCCESS;
}

Status schedr_scheduler_start_job(Job *const job_p)
{
    pid_t job_pid;

    if ((job_pid = forker()) < 0)  { return SCHEDR_ERROR_FORK_FAILED; }
    else if (job_pid == 0) { child_proc(job_p); }
    else 
    {
        started_jobs[started_jobs_count].job = job_p;
        started_jobs[started_jobs_count].pid = job_pid;
        started_jobs_count++;
        
        return parent_proc(job_p);
    }
    
    return SCHEDR_FAILURE; // GCOVR_EXCL_LINE (will never be executed)
}

static int find_job_index(const Job *const job_p)
{
    for (int i = started_jobs_count - 1; i >= 0; i--)
    {
        if (started_jobs[i].job == job_p)
        {
            return i;
        }
    }
    
    return -1;
}

Status schedr_scheduler_stop_job(Job *const job_p)
{
    job_p->state = Stopped;
    
    int index = find_job_index(job_p);
    
    if (index != -1) 
    { 
        pid_t pid = started_jobs[index].pid;
      
        kill(started_jobs[index].pid, SIGTERM);
        waitpid(pid, NULL, 0);
        
        started_jobs[index].job = NULL;
        started_jobs[index].pid = 0;
        
        if (index != started_jobs_count - 1)
        {
            for (int i = index; i < started_jobs_count - 1; i++)
            {
                started_jobs[i] = started_jobs[i + 1];
            }
        }
        
        started_jobs_count--;
    }
    
    return SCHEDR_SUCCESS;
}

// TODO: Should this really be here?
static void create_config_dir()
{
    const char *dirs[] = {"/.config", "/schedr", "/bin", NULL};
    
    char *home = getenv("HOME");
    char path[PATH_MAX];
    int len = strlen(home);
    
    strcpy(path, home);
    path[len] = '\0';
    
    for (int i = 0; dirs[i] != NULL; i++)
    {
        strcat(path, dirs[i]);
        len += strlen(dirs[i]);
        path[len] = '\0';
        mkdir(path, 0755);
    }
}

// TODO: Should this really be here?
void schedr_scheduler_set_path()
{
    create_config_dir();
    
    const char *path_ext = "/.config/schedr/bin/";
    const char *seperator = ":";
    char *old_path = getenv("PATH");
    char *home = getenv("HOME");
    int len = strlen(path_ext) + strlen(seperator) + strlen(old_path) + strlen(home);
    char *new_path = (char *)malloc(len + 1);
    
    strcpy(new_path, old_path);
    strcat(new_path, seperator);
    strcat(new_path, home);
    strcat(new_path, path_ext);
    new_path[len] = '\0';
    
    setenv("PATH", new_path, true);
    free(new_path);
}
