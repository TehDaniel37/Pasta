#include <stdlib.h>         // EXIT_FAILURE
#include <sys/types.h>      // pid_t
#include <signal.h>         // kill
#include <stdbool.h>        // bool, true, false
#include <sys/mman.h>       // mmap(), munmap()
#include <unistd.h>         // sleep()

#include "ssct.h"
#include "schedr_status_codes.h"
#include "schedr_scheduler.h"

static bool *mock_exec_called;
static bool *mock_exec_correct_params;

#define mock_exec_expected_params "echo should call exec"

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

static int mock_exec(const char *file_name, char *const argv[], char *const envp[])
{
    *mock_exec_called = true;

    if (strcmp(argv[2], mock_exec_expected_params) == 0)
    {
        *mock_exec_correct_params = true;
    }

    return 0;
}

static int mock_exec_will_exec_true(const char *file_name, char *const argv[], char *const envp[])
{
    return execlp("true", "true", NULL);
}

static pid_t mock_fork_will_fail(void) { return -1; }

static void setup() 
{

}

static void teardown()
{
    schedr_scheduler_reset_exec();
    schedr_scheduler_reset_forker();
    schedr_scheduler_remove_on_fork_hook();
}

static void start_job_should_call_exec_with_correct_params()
{
    mock_exec_called = (bool *)create_shared_memory(sizeof (bool));
    mock_exec_correct_params = (bool *)create_shared_memory(sizeof (bool));
    *mock_exec_called = false;
    *mock_exec_correct_params = false; 
    
    schedr_scheduler_set_exec(mock_exec);

    Job job = { .name = "Test", .command = mock_exec_expected_params, .interval_seconds = 0, .state = Stopped };

    schedr_scheduler_start_job(&job);

    ssct_assert_true(*mock_exec_called);
    ssct_assert_true(*mock_exec_correct_params);

    munmap(mock_exec_called, sizeof (bool));
    munmap(mock_exec_correct_params, sizeof(bool));
}

static void start_job_should_return_success_when_job_starts_successfully()
{
    Job job = { .name = "Test", .command = "echo hello", .interval_seconds = 0, .state = Stopped };

    schedr_scheduler_set_exec(mock_exec_will_exec_true);

    Status status = schedr_scheduler_start_job(&job);

    ssct_assert_equals(status, SCHEDR_SUCCESS);
}

static void start_job_should_set_job_state_to_running()
{
    Job job = { .name = "Test", .command = "echo hello", .interval_seconds = 0, .state = Stopped };

    schedr_scheduler_set_exec(mock_exec_will_exec_true);
    schedr_scheduler_start_job(&job);

    ssct_assert_equals(job.state, Running);
}

static void start_job_should_return_command_not_found_error() 
{
    Job job = { .name = "Test", .command = "ehco &>/dev/null", .interval_seconds = 1, .state = Stopped };

    Status status = schedr_scheduler_start_job(&job);

    ssct_assert_equals(status, SCHEDR_ERROR_JOB_COMMAND_NOT_FOUND);
}

static void start_job_should_return_fork_failed_error()
{
    Job job = { .name = "Test", .command = "ehco &>/dev/null", .interval_seconds = 1, .state = Stopped };

    schedr_scheduler_set_forker(mock_fork_will_fail);
    Status status = schedr_scheduler_start_job(&job);
    
    ssct_assert_equals(status, SCHEDR_ERROR_FORK_FAILED);
}

static void on_fork(void)
{
    schedr_scheduler_set_forker(mock_fork_will_fail);
}

static void start_job_should_return_fork_failed_error_when_child_fork_fails()
{   
    Job job = { .name = "Test", .command = "ehco &>/dev/null", .interval_seconds = 1, .state = Stopped };
    
    schedr_scheduler_set_on_fork(on_fork);
    Status status = schedr_scheduler_start_job(&job);
    
    ssct_assert_equals(status, SCHEDR_ERROR_FORK_FAILED);
}

int main(void)
{
    ssct_setup = setup;
    ssct_teardown = teardown;

    ssct_run(start_job_should_call_exec_with_correct_params);
    ssct_run(start_job_should_return_success_when_job_starts_successfully);
    ssct_run(start_job_should_set_job_state_to_running);
    ssct_run(start_job_should_return_command_not_found_error);
    ssct_run(start_job_should_return_fork_failed_error);

    ssct_print_summary();

    return EXIT_SUCCESS;
}
