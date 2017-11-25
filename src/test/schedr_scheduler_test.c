#include <stdlib.h>         // EXIT_FAILURE
#include <sys/types.h>      // pid_t
#include <signal.h>         // kill
#include <stdbool.h>        // bool, true, false
#include <sys/mman.h>       // mmap(), munmap()
#include <unistd.h>         // sleep()
#include <sys/stat.h>       // stat()
#include <errno.h>          // errno

#include "ssct.h"
#include "schedr_status_codes.h"
#include "schedr_scheduler.h"

#define mock_exec_expected_params "echo 'should call exec'"
#define mock_sleep_expected_param 3600

const int DEFAULT_WAIT_TIMEOUT = 5000;
const int MICROSECS_PER_MILLISEC = 1000;
const int WAIT_MILLISEC = 10;

static bool *mock_exec_called;
static bool *mock_exec_correct_params;
static int *times_exec_called;
static bool *mock_sleep_correct_params;
static bool *mock_exec_file_exists;
static bool *mock_exec_file_is_executable;

#define wait_until(expression, timeout) do {                    \
    int time_waited_millis = 0;                                 \
                                                                \
    while (!(expression))                                       \
    {                                                           \
        time_waited_millis += WAIT_MILLISEC;                    \
        usleep(MICROSECS_PER_MILLISEC * WAIT_MILLISEC);         \
                                                                \
        if (time_waited_millis > timeout) { break; }            \
    }                                                           \
} while (false)

/*
 * Credit: slezica
 * https://stackoverflow.com/a/5656561
 */
static void *create_shared_memory(size_t bytes) 
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;
    return mmap(NULL, bytes, protection, visibility, 0, 0);
}

static int file_exists(const char *path)
{
    struct stat file_stat;
    
    int status = stat(path, &file_stat);
    
    if (status < 0 && errno == ENOENT) { return false; }
    else { return true; }
}

static int mock_exec_will_verify_params(const char *file_name, char *const argv[], char *const envp[])
{
    *mock_exec_called = true;

    if (strcmp(argv[2], mock_exec_expected_params) == 0)
    {
        *mock_exec_correct_params = true;
    }

    _exit(EXIT_FAILURE);
}

static int mock_exec_will_exit_unsuccessfully(const char *file_name, char *const argv[], char *const envp[])
{
    _exit(EXIT_FAILURE);
}

static int mock_exec_will_count_times_called(const char *file_name, char *const argv[], char *const envp[])
{
    *times_exec_called += 1;
    
    _exit(EXIT_SUCCESS);
}

static int mock_exec_will_check_if_file_exists_and_is_executable(const char *file_name, char *const argv[], char *const envp[]) 
{
    struct stat file_stat;
    char *file = argv[2];
    const char *prefix = "command -v ";
    int len = strlen(file) + strlen(prefix);
    char *popen_cmd = (char *)malloc(sizeof (char) * len + 1);
    strcpy(popen_cmd, prefix);
    strcat(popen_cmd, file);
    popen_cmd[len] = '\0';
    char buf[200];
    
    FILE *fp = popen(popen_cmd, "r");
    
    int out_len = fread(buf, sizeof (char), 200, fp);
    buf[out_len - 1] = '\0';
    pclose(fp);
    
    if (stat(buf, &file_stat) == 0) 
    {
        *mock_exec_file_exists = true;
        
        if (file_stat.st_mode & S_IXUSR)
        {
            *mock_exec_file_is_executable = true;
        }
    }
    
    free(popen_cmd);
    _exit(EXIT_FAILURE);
}

static void will_wait_forever()
{
    while (true)
    {
        sleep(1);
    }
    
    _exit(EXIT_FAILURE);
}

static pid_t mock_fork_will_fail(void) { return -1; }

static unsigned int mock_sleep(unsigned int seconds) 
{
    if (seconds == mock_sleep_expected_param)
    {
        *mock_sleep_correct_params = true; 
    }
    
    return 0;
}

static void setup() 
{
    schedr_scheduler_set_exec(mock_exec_will_exit_unsuccessfully);
}

static void teardown()
{
    schedr_scheduler_kill_children();
    schedr_scheduler_reset_exec();
    schedr_scheduler_reset_forker();
    schedr_scheduler_reset_sleeper();
}

static void start_job_should_call_exec_with_correct_params()
{
    mock_exec_called = (bool *)create_shared_memory(sizeof (bool));
    mock_exec_correct_params = (bool *)create_shared_memory(sizeof (bool));
    *mock_exec_called = false;
    *mock_exec_correct_params = false; 

    Job job = { .name = "Test", .command = mock_exec_expected_params, .interval_seconds = 0, .state = Stopped };
    schedr_scheduler_set_exec(mock_exec_will_verify_params);

    schedr_scheduler_start_job(&job);
    
    wait_until(*mock_exec_called && *mock_exec_correct_params, DEFAULT_WAIT_TIMEOUT);

    ssct_assert_true(*mock_exec_called);
    ssct_assert_true(*mock_exec_correct_params);
    
    munmap(mock_exec_called, sizeof (bool));
    munmap(mock_exec_correct_params, sizeof(bool));
}

static void start_job_should_return_success_when_job_starts_successfully()
{
    Job job = { .name = "Test", .command = "echo hello", .interval_seconds = 0, .state = Stopped };

    Status status = schedr_scheduler_start_job(&job);

    ssct_assert_equals(status, SCHEDR_SUCCESS);
}

static void start_job_should_set_job_state_to_running()
{
    Job job = { .name = "Test", .command = "echo hello", .interval_seconds = 0, .state = Stopped };

    schedr_scheduler_start_job(&job);

    ssct_assert_equals(job.state, Running);
}

static void start_job_should_return_fork_failed_error()
{
    Job job = { .name = "Test", .command = "ehco", .interval_seconds = 1, .state = Stopped };

    schedr_scheduler_set_forker(mock_fork_will_fail);
    Status status = schedr_scheduler_start_job(&job);
    
    ssct_assert_equals(status, SCHEDR_ERROR_FORK_FAILED);
}

static void start_job_should_call_exec_repeatedly()
{
    Job job = { .name = "Test", .command = "echo", .interval_seconds = 0, .state = Stopped };
    times_exec_called = (int *)create_shared_memory(sizeof (int));
    *times_exec_called = 0;
    
    schedr_scheduler_set_exec(mock_exec_will_count_times_called);
    
    schedr_scheduler_start_job(&job);
    
    wait_until(*times_exec_called >= 10, DEFAULT_WAIT_TIMEOUT);
    
    ssct_assert_true(*times_exec_called >= 10);

    munmap(times_exec_called, sizeof (int));
}

static void start_job_should_pass_3600_seconds_to_sleep()
{
    Job job = { .name = "Test", .command = "echo", .interval_seconds = mock_sleep_expected_param, .state = Stopped };
    
    mock_sleep_correct_params = (bool *)create_shared_memory(sizeof (bool));
    
    schedr_scheduler_set_sleeper(mock_sleep);
    schedr_scheduler_start_job(&job);
    
    ssct_assert_true(mock_sleep_correct_params);
    
    munmap(mock_sleep_correct_params, sizeof (bool));
}

static void start_job_should_exec_executable_file_with_absolute_path()
{
    Job job = { .name = "Test", .command = "/home/danalm/git/schedr/res/debug/test/test_script.sh", .interval_seconds = 0, .state = Stopped };
    
    mock_exec_file_exists = (bool *)create_shared_memory(sizeof (bool));
    mock_exec_file_is_executable = (bool *)create_shared_memory(sizeof (bool));
    *mock_exec_file_exists = false;
    *mock_exec_file_is_executable = false;
    
    schedr_scheduler_set_exec(mock_exec_will_check_if_file_exists_and_is_executable);
    
    Status status = schedr_scheduler_start_job(&job);
    
    wait_until(*mock_exec_file_exists && *mock_exec_file_is_executable, DEFAULT_WAIT_TIMEOUT);
    
    ssct_assert_equals(status, SCHEDR_SUCCESS);
    ssct_assert_true(*mock_exec_file_exists);
    ssct_assert_true(*mock_exec_file_is_executable);
    
    munmap(mock_exec_file_exists, sizeof (bool));
    munmap(mock_exec_file_is_executable, sizeof(bool));
}

static void start_job_should_exec_executable_file_with_relative_path()
{
    system("mkdir -p $HOME/.config/schedr/bin");
    system("cp res/debug/test/test_script.sh $HOME/.config/schedr/bin");
    Job job = { .name = "Test", .command = "test_script.sh", .interval_seconds = 0, .state = Stopped };
    
    mock_exec_file_exists = (bool *)create_shared_memory(sizeof (bool));
    mock_exec_file_is_executable = (bool *)create_shared_memory(sizeof (bool));
    *mock_exec_file_exists = false;
    *mock_exec_file_is_executable = false;
    
    schedr_scheduler_set_exec(mock_exec_will_check_if_file_exists_and_is_executable);
    
    Status status = schedr_scheduler_start_job(&job);
    
    wait_until(*mock_exec_file_exists && *mock_exec_file_is_executable, DEFAULT_WAIT_TIMEOUT);
    
    ssct_assert_equals(status, SCHEDR_SUCCESS);
    ssct_assert_true(*mock_exec_file_exists);
    ssct_assert_true(*mock_exec_file_is_executable);
    
    munmap(mock_exec_file_exists, sizeof (bool));
    munmap(mock_exec_file_is_executable, sizeof(bool));    
}

static void set_path_should_create_config_dirs_when_they_do_not_exist()
{
    const char config_dir[] = "/home/danalm/.config/schedr";
    const char config_bin_dir[] = "/home/danalm/.config/schedr/bin";
    
    ssct_assert_false(file_exists(config_dir));
    ssct_assert_false(file_exists(config_bin_dir));
    
    schedr_scheduler_set_path();
    
    ssct_assert_true(file_exists(config_dir));
    ssct_assert_true(file_exists(config_bin_dir));
}

static void stop_job_should_set_state_to_stopped()
{
    Job job = { .name = "Test", .command = "echo", .interval_seconds = 0, .state = Running };
    
    schedr_scheduler_stop_job(&job);
    
    ssct_assert_equals(job.state, Stopped);
}

static void stop_job_should_stop_process_associated_with_job()
{
    Job job = { .name = "Test", .command = "echo", .interval_seconds = 0, .state = Running };
    
    pid_t child_pid;
    
    if ((child_pid = fork()) < 0)
    {
        ssct_fail();
    }
    if (child_pid == 0)
    {
        will_wait_forever();
    }
    else
    {
        wait_until(kill(child_pid, 0) == 0, DEFAULT_WAIT_TIMEOUT);
        
        schedr_scheduler_associate_pid_with_jod(&job, child_pid);
        schedr_scheduler_stop_job(&job);
        
        wait_until((kill(child_pid, 0) < 0) && errno == ESRCH, DEFAULT_WAIT_TIMEOUT);
        
        ssct_assert_true((kill(child_pid, 0) < 0) && errno == ESRCH);
        kill(child_pid, SIGTERM);
    }
}

int main(void)
{
    system("rm -rf $HOME/.config/schedr");
    
    ssct_run(set_path_should_create_config_dirs_when_they_do_not_exist);
    
    schedr_scheduler_set_path();
    
    ssct_setup = setup;
    ssct_teardown = teardown;

    ssct_run(start_job_should_call_exec_with_correct_params);
    ssct_run(start_job_should_return_success_when_job_starts_successfully);
    ssct_run(start_job_should_set_job_state_to_running);
    ssct_run(start_job_should_return_fork_failed_error);
    ssct_run(start_job_should_call_exec_repeatedly);
    ssct_run(start_job_should_pass_3600_seconds_to_sleep);
    ssct_run(start_job_should_exec_executable_file_with_absolute_path);
    ssct_run(start_job_should_exec_executable_file_with_relative_path);
    
    ssct_run(stop_job_should_set_state_to_stopped);
    ssct_run(stop_job_should_stop_process_associated_with_job);
    
    ssct_print_summary();

    return EXIT_SUCCESS;
}
