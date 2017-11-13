#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#include "test.h"
#include "pasta_status_codes.h"
#include "pasta_scheduler.h"

static bool process_exists(pid_t pid);

static void start_module_should_start_a_new_process_when_module_has_valid_command();

int main(void)
{
    start_module_should_start_a_new_process_when_module_has_valid_command();

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

static void start_module_should_start_a_new_process_when_module_has_valid_command()
{
    Module module = {.name = "testing", .command = "echo hello", .interval_seconds = 1, .state = Stopped};

    Status status = pasta_scheduler_start_module(&module);
    pid_t mod_pid = pasta_scheduler_get_module_pid(&module);

    test_assert(process_exists(mod_pid) && status == PASTA_SUCCESS);
}
