#include <sys/types.h>      // pid_t
#include <stdlib.h>         // getenv()
#include <unistd.h>         // fork(), execve(), sleep()
#include <sys/wait.h>       // waitpid()
#include <stdbool.h>        // false
#include <string.h>
#include <signal.h>         // kill, SIGTERM
#include <linux/limits.h>   // PATH_MAX
#include <sys/stat.h>       // mkdir()

#include "dodee_scheduler.h"

#define MAX_RUNNING_TASKS 100
static int started_tasks_count = 0;

static int (*exec)(const char *fn, char *const argv[], char *const envp[]) = execve;
static int (*forker)(void) = fork;
static unsigned int (*sleeper)(unsigned int seconds) = sleep;

struct TaskProcMap 
{
    DodeeTask *task;
    pid_t pid;
};

typedef struct TaskProcMap TaskProcMap;

static TaskProcMap started_tasks[MAX_RUNNING_TASKS];

#ifdef TEST
void dodee_scheduler_set_exec(int (*exec_func)(const char *fn, char *const argv[], char *const envp[])) { exec = exec_func; }
void dodee_scheduler_reset_exec() { exec = execve; }
void dodee_scheduler_set_forker(int (*fork_func)(void)) { forker = fork_func; }
void dodee_scheduler_reset_forker() { forker = fork; }
void dodee_scheduler_set_sleeper(unsigned int (*sleep_func)(unsigned int seconds)) { sleeper = sleep_func; }
void dodee_scheduler_reset_sleeper() { sleeper = sleep; }

void dodee_scheduler_kill_children() 
{
    for (int i = started_tasks_count - 1; i >= 0; i--)
    {
        pid_t pid = started_tasks[i].pid;
        
        kill(started_tasks[i].pid, SIGTERM);
        waitpid(pid, NULL, 0);
        
        started_tasks[i].task = NULL;
        started_tasks[i].pid = 0;
    }
    
    started_tasks_count = 0;
}

void dodee_scheduler_associate_pid_with_task(DodeeTask *const task, pid_t pid) 
{ 
    started_tasks[started_tasks_count].task = task;
    started_tasks[started_tasks_count].pid = pid;
    started_tasks_count++;
}

void __gcov_flush();
#endif

static void cmd_proc(DodeeTask *task_p)
{
    char *shell = getenv("SHELL");
    char *argv[] = { shell, "-c", task_p->command, NULL };

    #ifdef TEST
    __gcov_flush();
    #endif
    
    exec(argv[0], argv, environ);   // GCOVR_EXCL_LINE
    
    _exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
}

static int start_task_cmd(DodeeTask *task_p)
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
         
        return DODEE_ERROR_FORK_FAILED;    // GCOVR_EXCL_LINE
    }
    else if (cmd_pid == 0)
    {
        cmd_proc(task_p);    // will not return
        
        return DODEE_FAILURE;  // GCOVR_EXCL_LINE  (return statement added to silence compiler)
    }
    else 
    {
        int cmd_status;
        waitpid(cmd_pid, &cmd_status, 0);
        return WEXITSTATUS(cmd_status);
    }
}

static void child_proc(DodeeTask *task_p)
{
    int cmd_status = EXIT_SUCCESS;
    
    while (cmd_status == EXIT_SUCCESS)
    {
        cmd_status = start_task_cmd(task_p);
        
        if (cmd_status == EXIT_SUCCESS) 
        {
            sleeper(task_p->interval_seconds);
        }
    }
    
    #ifdef TEST
    __gcov_flush();
    #endif
    
    _exit(EXIT_FAILURE);    // GCOVR_EXCL_LINE
}

static DodeeStatus parent_proc(DodeeTask *task_p)
{
    task_p->state = Running;
    
    return DODEE_SUCCESS;
}

DodeeStatus dodee_scheduler_start_task(DodeeTask *const task_p)
{
    pid_t task_pid;

    if ((task_pid = forker()) < 0)  { return DODEE_ERROR_FORK_FAILED; }
    else if (task_pid == 0) { child_proc(task_p); }
    else 
    {
        started_tasks[started_tasks_count].task = task_p;
        started_tasks[started_tasks_count].pid = task_pid;
        started_tasks_count++;
        
        return parent_proc(task_p);
    }
    
    return DODEE_FAILURE; // GCOVR_EXCL_LINE (will never be executed)
}

static int find_task_index(const DodeeTask *const task_p)
{
    for (int i = started_tasks_count - 1; i >= 0; i--)
    {
        if (started_tasks[i].task == task_p)
        {
            return i;
        }
    }
    
    return -1;
}

DodeeStatus dodee_scheduler_stop_task(DodeeTask *const task_p)
{
    task_p->state = Stopped;
    
    int index = find_task_index(task_p);
    
    if (index != -1) 
    { 
        pid_t pid = started_tasks[index].pid;
      
        kill(started_tasks[index].pid, SIGTERM);
        waitpid(pid, NULL, 0);
        
        started_tasks[index].task = NULL;
        started_tasks[index].pid = 0;
        
        if (index != started_tasks_count - 1)
        {
            for (int i = index; i < started_tasks_count - 1; i++)
            {
                started_tasks[i] = started_tasks[i + 1];
            }
        }
        
        started_tasks_count--;
    }
    
    return DODEE_SUCCESS;
}

// TODO: Should this really be here?
static void create_config_dir()
{
    const char *dirs[] = {"/.config", "/dodee", "/bin", NULL};
    
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
void dodee_scheduler_set_path()
{
    create_config_dir();
    
    const char *path_ext = "/.config/dodee/bin/";
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
