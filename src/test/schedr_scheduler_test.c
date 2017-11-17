#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <stdatomic.h>
#include <sys/mman.h>

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
    
    if (strcmp(argv[2], mock_exec_expected_params))
    {
       *mock_exec_correct_params = true;
    }

    return 0;
}

static void start_job_should_call_exec_with_correct_params()
{
    mock_exec_called = (bool *)create_shared_memory(sizeof (bool));
    mock_exec_correct_params = (bool *)create_shared_memory(sizeof (bool));

    Job job = {.name = "Test", .command = mock_exec_expected_params,
        .interval_seconds = 0, .state = Stopped};

    schedr_scheduler_set_exec(mock_exec);

    Status status = schedr_scheduler_start_job(&job);

    ssct_assert_true(*mock_exec_called);
    ssct_assert_true(*mock_exec_correct_params);
    ssct_assert_equals(status, SCHEDR_SUCCESS);

    schedr_scheduler_reset_exec();
    
    munmap(mock_exec_called, sizeof (bool));
    munmap(mock_exec_correct_params, sizeof(bool));
}

int main(void)
{
    ssct_run(start_job_should_call_exec_with_correct_params);

    ssct_print_summary();

    return EXIT_SUCCESS;
}
