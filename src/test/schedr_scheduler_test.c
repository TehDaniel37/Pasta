#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#include "ssct.h"
#include "schedr_status_codes.h"
#include "schedr_scheduler.h"

static bool process_exists(pid_t pid);

static void start_job_should_start_a_new_process();
static void start_job_should_run_executable_file();
static void start_job_should_run_job_every_sec();
static void start_job_should_execute_correct_command_when_command_has_many_arguments();
static void start_job_should_execute_correct_command_when_command_contains_shell_builtins();
static void start_job_should_return_permission_denied_error_when_not_permitted_to_run_command();
static void start_job_should_return_command_not_found_error_when_command_does_not_exist();

static void stop_job_should_terminate_process();

int main(void)
{
    ssct_run(start_job_should_start_a_new_process);
    ssct_run(start_job_should_run_executable_file);
    ssct_run(start_job_should_run_job_every_sec);
    ssct_run(start_job_should_execute_correct_command_when_command_has_many_arguments);
    ssct_run(start_job_should_execute_correct_command_when_command_contains_shell_builtins);
    ssct_run(start_job_should_return_permission_denied_error_when_not_permitted_to_run_command);
    ssct_run(start_job_should_return_command_not_found_error_when_command_does_not_exist);

    ssct_run(stop_job_should_terminate_process);

    ssct_print_summary();

    return EXIT_SUCCESS;
}

static bool process_exists(pid_t pid)
{
    int return_code = kill(pid, 0);

    if (return_code != 0 && errno == ESRCH)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static void start_job_should_start_a_new_process()
{
    Job job = {.name = "testing", .command = "echo hello", .interval_seconds = 1, .state = Stopped};

    Status status = schedr_scheduler_start_job(&job);
    pid_t job_pid = schedr_scheduler_get_job_pid(&job);

    ssct_assert_true(process_exists(job_pid)); 
    ssct_assert_equals(status, SCHEDR_SUCCESS);
}

static void start_job_should_run_executable_file()
{
    ssct_fail();
}

static void start_job_should_run_job_every_sec()
{
    ssct_fail();
}

static void start_job_should_execute_correct_command_when_command_has_many_arguments()
{
    ssct_fail();
}

static void start_job_should_execute_correct_command_when_command_contains_shell_builtins()
{
    ssct_fail();
}

static void start_job_should_return_permission_denied_error_when_not_permitted_to_run_command()
{
    ssct_fail();
}

static void start_job_should_return_command_not_found_error_when_command_does_not_exist()
{
    ssct_fail();
}

static void stop_job_should_terminate_process()
{
    ssct_fail();
}

