#include <stdio.h>          // puts()
#include <stdlib.h>         // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>         // strncpy(), strncat()

#include "test.h"
#include "pasta_config_parser.h"
#include "pasta_module.h"
#include "pasta_status_codes.h"

// mock prototypes
static void *mock_allocator_will_return_null(size_t bytes);

// test helper prototypes
static bool module_arrays_equal(const Module *const arr1, size_t arr1_len, const Module *const arr2, size_t arr2_len);
static char *get_test_resource(const char *file_name, size_t len);

// test prototypes
static void load_modules_should_load_correct_values_when_config_file_exists_and_has_correct_format();
static void load_modules_should_return_file_not_found_error_when_file_does_not_exist();
static void load_modules_should_return_config_format_error_when_config_file_has_incorrect_format();
static void load_modules_should_return_allocation_error_when_allocation_fails();
static void load_modules_should_return_permission_denied_error_when_not_permitted_to_read_config_file();
static void load_modules_should_return_no_modules_warning_when_config_file_has_no_modules();

int main(void) 
{
    load_modules_should_load_correct_values_when_config_file_exists_and_has_correct_format();
    load_modules_should_return_file_not_found_error_when_file_does_not_exist();
    load_modules_should_return_config_format_error_when_config_file_has_incorrect_format();
    load_modules_should_return_allocation_error_when_allocation_fails();
    load_modules_should_return_permission_denied_error_when_not_permitted_to_read_config_file();
    load_modules_should_return_no_modules_warning_when_config_file_has_no_modules();

    test_print_summary();

    return EXIT_SUCCESS;
}

static void *mock_allocator_will_return_null(size_t bytes) { return NULL; }

static char *get_test_resource(const char *file_name, size_t len)
{
    static const char TEST_RES_PATH[] = "../res/debug/test/";
    
    char *buffer_p = (char *)malloc(len + sizeof(TEST_RES_PATH));

    if (buffer_p == NULL)
    {
        printf("Malloc failed for %s in %s\n.", __func__, __FILE__);
        exit(EXIT_FAILURE);
    }

    strncpy(buffer_p, TEST_RES_PATH, sizeof(TEST_RES_PATH) - 1);
    strncat(buffer_p, file_name, len);

    return buffer_p;
}

static bool module_arrays_equal(const Module *const arr1, size_t arr1_len, const Module *const arr2, size_t arr2_len)
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

static void load_modules_should_load_correct_values_when_config_file_exists_and_has_correct_format()
{
    static const char TEST_CONF[] = "test_config_correct_format.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    static const Module expected[] = {
        {.name = "echo_every_10_s", .command = "`echo 'testing interval every 10 s'`", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_10_sec", .command = "`echo 'testing interval every 10 sec'`", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_10_seconds", .command = "`echo 'testing interval every 10 seconds'`", .interval_seconds = 10, .state = Stopped },
        {.name = "echo_every_second", .command = "`echo 'testing interval every second'`", .interval_seconds = 1, .state = Stopped },
        {.name = "echo_every_5_m", .command = "`echo 'testing interval every 5 m'`", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_5_min", .command = "`echo 'testing interval every 5 min'`", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_5_minutes", .command = "`echo 'testing interval every 5 minutes'`", .interval_seconds = 300, .state = Stopped },
        {.name = "echo_every_minute", .command = "`echo 'testing interval every minute'`", .interval_seconds = 60, .state = Stopped },
        {.name = "echo_every_2_h", .command = "`echo 'testing interval every 2 h'`", .interval_seconds = 7200, .state = Stopped },
        {.name = "echo_every_2_hours", .command = "`echo 'testing interval every 2 hours'`", .interval_seconds = 7200, .state = Stopped },
        {.name = "echo_every_hour", .command = "`echo 'testing interval every hour'`", .interval_seconds = 3600, .state = Stopped}
    };
    static const size_t expected_len = sizeof (expected) / sizeof (Module);

    char *test_config_valid = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *actual = NULL;
    int actual_len = 0;

    Status status = pasta_config_load_modules(&actual, &actual_len, test_config_valid);

    if (actual_len != expected_len)
    {
        test_fail();
        return;
    }

    if (!module_arrays_equal(expected, expected_len, actual, actual_len))
    {
        test_fail();
        return;
    }

    test_assert(status == PASTA_SUCCESS);
}

static void load_modules_should_return_file_not_found_error_when_file_does_not_exist()
{
    static const char TEST_CONF[] = "non_existant.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    char *test_conf_non_existant = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *modules = NULL;
    int modules_len = 0;

    Status status = pasta_config_load_modules(&modules, &modules_len, test_conf_non_existant);

    test_assert(status == PASTA_ERROR_FILE_NOT_FOUND);
}

static void load_modules_should_return_config_format_error_when_config_file_has_incorrect_format()
{
    static const char TEST_CONF[] = "test_config_incorrect_format.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    char *test_conf_incorrect_format = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *modules = NULL;
    int modules_len = 0;

    Status status = pasta_config_load_modules(&modules, &modules_len, test_conf_incorrect_format);

    test_assert(status == PASTA_ERROR_CONFIG_FORMAT);
}

static void load_modules_should_return_allocation_error_when_allocation_fails()
{
    static const char TEST_CONF[] = "test_config_correct_format.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    char *test_conf = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *modules = NULL;
    int modules_len = 0;

    pasta_config_set_allocator(mock_allocator_will_return_null);

    Status status = pasta_config_load_modules(&modules, &modules_len, test_conf);

    pasta_config_reset_allocator();

    test_assert(status == PASTA_ERROR_ALLOCATION);
}

static void load_modules_should_return_permission_denied_error_when_not_permitted_to_read_config_file()
{
    static const char TEST_CONF[] = "test_config_no_permission.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    char *test_conf_no_permission = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *modules = NULL;
    int modules_len = 0;

    Status status = pasta_config_load_modules(&modules, &modules_len, test_conf_no_permission);

    test_assert(status == PASTA_ERROR_PERMISSION_DENIED);
}

static void load_modules_should_return_no_modules_warning_when_config_file_has_no_modules()
{
    static const char TEST_CONF[] = "test_config_no_modules.conf";
    static const size_t TEST_CONF_LEN = sizeof (TEST_CONF) - 1;

    char *test_conf_no_modules = get_test_resource(TEST_CONF, TEST_CONF_LEN);
    Module *modules = NULL;
    int modules_len = 0;

    Status status = pasta_config_load_modules(&modules, &modules_len, test_conf_no_modules);

    test_assert(status == PASTA_WARNING_NO_MODULES && modules == NULL && modules_len == 0);
}

