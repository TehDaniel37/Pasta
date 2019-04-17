#include <stdio.h>          // printf()
#include <stdlib.h>         // malloc(), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>         // strncpy(), strncat()
#include <stdbool.h>        // bool, true, false

#include "ssct.h"
#include "dodee_config_parser.h"
#include "dodee_task.h"
#include "dodee_status_codes.h"

static DodeeTask *tasks_actual;
static int tasks_actual_len;
static char *conf_file = NULL;

static void setup()
{
    tasks_actual = NULL;
    tasks_actual_len = 0;
}

static void teardown()
{
    dodee_config_reset_allocator();
    
    free(tasks_actual);
    free(conf_file);
}

static void *mock_allocator_will_return_null(size_t bytes) { return NULL; }

static char *get_test_resource(const char *file_name, size_t len)
{
    static const char TEST_RES_PATH[] = "/home/danalm/git/dodee/res/debug/test/";
    
    char *buffer_p = (char *)malloc(len + sizeof(TEST_RES_PATH));

    if (buffer_p == NULL)
    {
        printf("Malloc failed for %s in %s\n.", __func__, __FILE__);
        exit(EXIT_FAILURE);
    }

    strcpy(buffer_p, TEST_RES_PATH);
    strncat(buffer_p, file_name, len);

    return buffer_p;
}

static bool task_arrays_equal(const DodeeTask *const arr1, size_t arr1_len, const DodeeTask *const arr2, size_t arr2_len)
{
    if (arr1_len != arr2_len) { return false; }

    for (int i = 0; i < arr1_len; i++)
    {
        if (strcmp(arr1[i].name, arr2[i].name) != 0) { return false; }
        if (strcmp(arr1[i].command, arr2[i].command) != 0) { return false; }
        if (arr1[i].interval_seconds != arr2[i].interval_seconds) { return false; }
        if (arr1[i].state != arr2[i].state) { return false; }
    }

    return true;
}

static void load_tasks_should_load_correct_values()
{
    static const char TEST_CONF[] = "test_correct_format.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    static const DodeeTask expected[] = {
        {.name = "echo_every_10_s", .command = "echo 'testing interval every 10 s'", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_10_sec", .command = "echo 'testing interval every 10 sec'", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_10_seconds", .command = "echo 'testing interval every 10 seconds'", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_second", .command = "echo 'testing interval every second'", .interval_seconds = 1, .state = Stopped },
        {.name = "echo_every_5_m", .command = "echo 'testing interval every 5 m'", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_5_min", .command = "echo 'testing interval every 5 min'", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_5_minutes", .command = "echo 'testing interval every 5 minutes'", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_minute", .command = "echo 'testing interval every minute'", .interval_seconds = 60, .state = Stopped },
        {.name = "echo_every_2_h", .command = "echo 'testing interval every 2 h'", .interval_seconds = 7200, .state = Stopped },
        {.name = "echo_every_2_hours", .command = "echo 'testing interval every 2 hours'", .interval_seconds = 7200, .state = Stopped },
        {.name = "echo_every_hour", .command = "echo 'testing interval every hour'", .interval_seconds = 3600, .state = Stopped}
    };
    static const int expected_len = sizeof (expected) / sizeof (DodeeTask);

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);

    ssct_assert_equals(tasks_actual_len, expected_len);
    ssct_assert_true(task_arrays_equal(expected, expected_len, tasks_actual, tasks_actual_len));
    ssct_assert_equals(status, DODEE_SUCCESS);
}

static void load_tasks_should_return_invalid_argument_error_when_task_argument_is_not_null()
{
    static const char TEST_CONF[] = "test_default.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    DodeeTask not_null;
    DodeeTask *not_null_p = &not_null;

    DodeeStatus status = dodee_config_load_tasks(&not_null_p, &tasks_actual_len, conf_file);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void load_tasks_should_return_file_not_found_error()
{
    static const char TEST_CONF[] = "non_existant.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);
    
    ssct_assert_equals(status, DODEE_ERROR_FILE_NOT_FOUND);
}

static void load_tasks_should_return_config_format_error()
{
    static const char TEST_CONF[] = "test_incorrect_format.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);

    ssct_assert_equals(status, DODEE_ERROR_CONFIG_FORMAT);
}

static void load_tasks_should_return_allocation_failed_error()
{
    static const char TEST_CONF[] = "test_default.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    dodee_config_set_allocator(mock_allocator_will_return_null);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);
    
    ssct_assert_equals(status, DODEE_ERROR_ALLOCATION_FAILED);
}

static void load_tasks_should_return_permission_denied_error()
{
    static const char TEST_CONF[] = "test_no_permission.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);

    ssct_assert_equals(status, DODEE_ERROR_PERMISSION_DENIED);
}

static void load_tasks_should_return_no_tasks_warning()
{
    static const char TEST_CONF[] = "test_no_tasks.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);

    ssct_assert_equals(status, DODEE_WARNING_NO_TASKS);
    ssct_assert_zero(tasks_actual_len);
}

static void load_tasks_should_return_invalid_argument_error_when_file_is_directory()
{
    static const char CONF_THAT_IS_DIR[] = "conf_dir";
    static const int CONF_THAT_IS_DIR_LEN = sizeof (CONF_THAT_IS_DIR) - 1;
    
    conf_file = get_test_resource(CONF_THAT_IS_DIR, CONF_THAT_IS_DIR_LEN);
    
    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);
    
    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);    
}

static void load_tasks_should_return_failure_when_unlikely_open_file_error_occurs()
{
    static const char TEST_CONF[] = "circular-link.conf";
    static const int TEST_CONF_LEN = sizeof (TEST_CONF) - 1;
    
    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    
    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);
    
    ssct_assert_equals(status, DODEE_FAILURE);
}

static void load_tasks_should_load_config_file_case_insensitive() 
{
    static const char TEST_CONF[] = "test_mixed_case.conf";
    static const char TEST_CONF_LEN = sizeof (TEST_CONF) - 1;
    
    static const int expected_len = 3;
    
    static const DodeeTask expected_tasks[] = {
        {.name = "all lowercase", .command = "echo 'lowercase'", .interval_seconds = 10, .state = Stopped },
        {.name = "ALL UPPERCASE", .command = "echo 'AAAAAHHHHHH'", .interval_seconds = 600, .state = Stopped },
        {.name = "MiXeD CaSe", .command = "echo 'MiXeD CaSe'", .interval_seconds = 3600, .state = Stopped }
    };
    
    conf_file = get_test_resource(TEST_CONF, TEST_CONF_LEN);

    DodeeStatus status = dodee_config_load_tasks(&tasks_actual, &tasks_actual_len, conf_file);
    
    ssct_assert_equals(tasks_actual_len, expected_len);
    ssct_assert_true(task_arrays_equal(expected_tasks, expected_len, tasks_actual, tasks_actual_len));
    ssct_assert_equals(status, DODEE_SUCCESS);
}

int main(void) 
{
    ssct_setup = setup;
    ssct_teardown = teardown;
    
    ssct_run(load_tasks_should_load_correct_values);
    ssct_run(load_tasks_should_return_invalid_argument_error_when_task_argument_is_not_null);
    ssct_run(load_tasks_should_return_file_not_found_error);
    ssct_run(load_tasks_should_return_config_format_error);
    ssct_run(load_tasks_should_return_allocation_failed_error);
    ssct_run(load_tasks_should_return_permission_denied_error);
    ssct_run(load_tasks_should_return_no_tasks_warning);
    ssct_run(load_tasks_should_return_invalid_argument_error_when_file_is_directory);
    ssct_run(load_tasks_should_return_failure_when_unlikely_open_file_error_occurs);
    ssct_run(load_tasks_should_load_config_file_case_insensitive);

    ssct_print_summary();

    return EXIT_SUCCESS;
}
