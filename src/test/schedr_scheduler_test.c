#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#include "test.h"
#include "schedr_status_codes.h"
#include "schedr_scheduler.h"

static bool process_exists(pid_t pid);

static void start_job_should_start_a_new_process_when_job_has_valid_command();

int main(void)
{
    start_job_should_start_a_new_process_when_job_has_valid_command();

    test_print_summary();

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

static void start_job_should_start_a_new_process_when_job_has_valid_command()
{
    Job job = {.name = "testing", .command = "echo hello", .interval_seconds = 1, .state = Stopped};

    Status status = schedr_scheduler_start_job(&job);
    pid_t job_pid = schedr_scheduler_get_job_pid(&job);

    test_assert(process_exists(job_pid) && status == SCHEDR_SUCCESS);
}
