#include <stdlib.h>             // malloc
#include <string.h>             // strncmp
#include <stdbool.h>            // bool, true, false
#include <stddef.h>             // size_t

#include "ssct.h"
#include "dodee_status_codes.h" // DodeeStatus, error codes
#include "dodee_task.h"       // DodeeTask, dodee_task_set_*()

// Test prototypes
static void should_set_all_task_members_when_setters_are_called();
static void task_setters_should_set_all_task_members_when_task_is_dynamically_allocated();

static void init_should_return_null_argument_error_when_task_argument_is_null();
static void init_should_set_members_to_default_values();

static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols();
static void set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols();
static void set_name_should_return_null_argument_error_when_task_argument_is_null();
static void set_name_should_return_null_argument_error_when_name_argument_is_null();
static void set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero();
static void set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed();
static void set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed();
static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();

static void set_command_should_set_command_member_when_command_argument_is_a_valid_command();
static void set_command_should_return_null_argument_error_when_task_argument_is_null();
static void set_command_should_return_null_argument_error_when_command_argument_is_null();
static void set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero();
static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
static void set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed();
static void set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed();

static void set_interval_should_return_null_argument_error_when_task_argument_is_null();
static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();

static void set_state_should_return_null_argument_error_when_task_argument_is_null();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();

int main(void)
{
    ssct_run(should_set_all_task_members_when_setters_are_called);
    ssct_run(task_setters_should_set_all_task_members_when_task_is_dynamically_allocated);

    ssct_run(init_should_return_null_argument_error_when_task_argument_is_null);
    ssct_run(init_should_set_members_to_default_values);

    ssct_run(set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols);
    ssct_run(set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols);
    ssct_run(set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero);
    ssct_run(set_name_should_return_null_argument_error_when_task_argument_is_null);
    ssct_run(set_name_should_return_null_argument_error_when_name_argument_is_null);
    ssct_run(set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed);
    ssct_run(set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed);
    ssct_run(set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string);

    ssct_run(set_command_should_set_command_member_when_command_argument_is_a_valid_command);
    ssct_run(set_command_should_return_null_argument_error_when_task_argument_is_null);
    ssct_run(set_command_should_return_null_argument_error_when_command_argument_is_null);
    ssct_run(set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string);
    ssct_run(set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero);
    ssct_run(set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed);
    ssct_run(set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed);

    ssct_run(set_interval_should_return_null_argument_error_when_task_argument_is_null);
    ssct_run(set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative);

    ssct_run(set_state_should_return_null_argument_error_when_task_argument_is_null);
    ssct_run(set_state_should_return_invalid_argument_error_when_state_argument_is_negative);
    ssct_run(set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range);

    ssct_print_summary();

    return EXIT_SUCCESS;
}

static void init_should_return_null_argument_error_when_task_argument_is_null()
{
    DodeeStatus status = dodee_task_init(NULL);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void init_should_set_members_to_default_values()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_init(&task);

    ssct_assert_empty(task.name);
    ssct_assert_empty(task.command);
    ssct_assert_zero(task.interval_seconds);
    ssct_assert_equals(task.state, Stopped);
    ssct_assert_equals(status, DODEE_SUCCESS);
}

static void should_set_all_task_members_when_setters_are_called()
{
    static const char TASK_NAME[] = "test task";
    static const int TASK_NAME_LEN = sizeof (TASK_NAME) - 1;
    static const char TASK_CMD[] = "echo testing";
    static const int TASK_CMD_LEN = sizeof (TASK_CMD) - 1;
    static const int TASK_INTERVAL = 10;
    static const DodeeTaskState TASK_STATE = Running;

    DodeeTask task;
    
    DodeeStatus name_status = dodee_task_set_name(&task, TASK_NAME, TASK_NAME_LEN);
    DodeeStatus cmd_status = dodee_task_set_command(&task, TASK_CMD, TASK_CMD_LEN);
    DodeeStatus interval_status = dodee_task_set_interval(&task, TASK_INTERVAL);
    DodeeStatus state_status = dodee_task_set_state(&task, TASK_STATE);

    ssct_assert_equals(name_status, DODEE_SUCCESS);
    ssct_assert_equals(cmd_status, DODEE_SUCCESS);
    ssct_assert_equals(interval_status, DODEE_SUCCESS);
    ssct_assert_equals(state_status, DODEE_SUCCESS);

    ssct_assert_equals(task.name, strlen(task.name), TASK_NAME, TASK_NAME_LEN);
    ssct_assert_equals(task.command, strlen(task.command), TASK_CMD, TASK_CMD_LEN);
    ssct_assert_equals(task.interval_seconds, TASK_INTERVAL);
    ssct_assert_equals(task.state, TASK_STATE);
}

static void task_setters_should_set_all_task_members_when_task_is_dynamically_allocated()
{
    static const char TASK_NAME[] = "dyn task";
    static const int TASK_NAME_LEN = sizeof (TASK_NAME) - 1;
    static const char TASK_CMD[] = "echo dynamic";
    static const int TASK_CMD_LEN = sizeof (TASK_CMD) - 1;
    static const int TASK_INTERVAL = 5;
    static const DodeeTaskState TASK_STATE = Stopped;

    DodeeTask *task_p = (DodeeTask *)malloc(sizeof (DodeeTask));

    DodeeStatus name_status = dodee_task_set_name(task_p, TASK_NAME, TASK_NAME_LEN);
    DodeeStatus cmd_status = dodee_task_set_command(task_p, TASK_CMD, TASK_CMD_LEN);
    DodeeStatus interval_status = dodee_task_set_interval(task_p, TASK_INTERVAL);
    DodeeStatus state_status = dodee_task_set_state(task_p, TASK_STATE);

    ssct_assert_equals(name_status, DODEE_SUCCESS);
    ssct_assert_equals(cmd_status, DODEE_SUCCESS);
    ssct_assert_equals(interval_status, DODEE_SUCCESS);
    ssct_assert_equals(state_status, DODEE_SUCCESS);

    ssct_assert_equals(task_p->name, strlen(task_p->name), TASK_NAME, TASK_NAME_LEN);
    ssct_assert_equals(task_p->command, strlen(task_p->command), TASK_CMD, TASK_CMD_LEN);
    ssct_assert_equals(task_p->interval_seconds, TASK_INTERVAL);
    ssct_assert_equals(task_p->state, TASK_STATE);

    free(task_p);
}

static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols()
{
    static const char PRINTABLE_ASCII_CHARS[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    static const int PRINTABLE_ASCII_CHARS_LEN = sizeof (PRINTABLE_ASCII_CHARS) - 1;

    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, PRINTABLE_ASCII_CHARS, PRINTABLE_ASCII_CHARS_LEN);

    ssct_assert_equals(status, DODEE_SUCCESS);
    ssct_assert_equals(task.name, strlen(task.name), PRINTABLE_ASCII_CHARS, PRINTABLE_ASCII_CHARS_LEN);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols()
{
    static const char FIRST_NON_PRINTABLE_ASCII_CHAR = '\x1';
    static const char FIRST_PRINTABLE_ASCII_CHAR = ' ';
    static const int BUFFER_SIZE = 2;

    for (char non_printable = FIRST_NON_PRINTABLE_ASCII_CHAR; non_printable < FIRST_PRINTABLE_ASCII_CHAR; non_printable++)
    {
        char buffer[BUFFER_SIZE];
        buffer[0] = non_printable;
        buffer[BUFFER_SIZE - 1] = '\0';
        DodeeTask task;

        DodeeStatus status = dodee_task_set_name(&task, buffer, BUFFER_SIZE - 1);

        if (status != DODEE_ERROR_INVALID_ARGUMENT)
        {
            ssct_fail();
        }
    }
}

static void set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, "hello", 0);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_task_argument_is_null()
{
    const char *null_task_name = "null task";

    DodeeStatus status = dodee_task_set_name(NULL, null_task_name, sizeof (null_task_name) - 1);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_name_argument_is_null()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, NULL, sizeof(task.name) - 1);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed()
{
    static const char STR_100_RND_CHARS[] = "L2jX0CMU2W39t4Lt8eYZYZmTniyiySdhRTKQyyZpgvlmdLeIoFJWyIlPdZjE8fUllpdZPgM5sRZ0S4ormacyIEe27xrh9WbgiPcE";
    static const int STR_100_RND_CHARS_SIZE = sizeof (STR_100_RND_CHARS) - 1;

    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE);

    ssct_assert_equals(task.name, strlen(task.name), STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE);
    ssct_assert_equals(status, DODEE_SUCCESS);
}

static void set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed()
{
    static const char STR_101_RND_CHARS[] = "ysrCek0ygdDVOZ7TTfxkAorGfER1YJEQ0zsUvhAILtgN9mI3hfDiS3GQaOqOlXJQRzV4pPVDBYtSiKuPXh5o6GDA0xP5Q4wuBSWQe";
    static const int STR_101_RND_CHARS_SIZE = sizeof(STR_101_RND_CHARS) - 1;

    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, STR_101_RND_CHARS, STR_101_RND_CHARS_SIZE);

    ssct_assert_equals(status, DODEE_ERROR_BUFFER_OVERFLOW);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_name(&task, "", 0);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_set_command_member_when_command_argument_is_a_valid_command()
{
    static const char VALID_CMD[] = "echo hello";
    static const int VALID_CMD_LEN = sizeof(VALID_CMD) - 1;

    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, VALID_CMD, VALID_CMD_LEN);

    ssct_assert_equals(status, DODEE_SUCCESS);
}

static void set_command_should_return_null_argument_error_when_task_argument_is_null()
{
    static const char NULL_TASK_CMD[] = "echo 'hello'";

    DodeeStatus status = dodee_task_set_command(NULL, NULL_TASK_CMD, sizeof (NULL_TASK_CMD) - 1);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_command_argument_is_null()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, NULL, 0);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, "", 0);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, "hello", 0);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed()
{
    static const char CMD_1000_CHARS[] = "echo 'HAbkJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'";
    static const int CMD_1000_CHARS_LEN = sizeof (CMD_1000_CHARS) - 1;

    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, CMD_1000_CHARS, CMD_1000_CHARS_LEN);

    ssct_assert_equals(task.command, strlen(task.command), CMD_1000_CHARS, CMD_1000_CHARS_LEN);
    ssct_assert_equals(status, DODEE_SUCCESS);
}

static void set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed()
{
    static const char CMD_1001_CHARS[] = "echo 'HAb6kJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'";

    DodeeTask task;

    DodeeStatus status = dodee_task_set_command(&task, CMD_1001_CHARS, sizeof (CMD_1001_CHARS) - 1);

    ssct_assert_equals(status, DODEE_ERROR_BUFFER_OVERFLOW);
}

static void set_interval_should_return_null_argument_error_when_task_argument_is_null()
{
    DodeeStatus status = dodee_task_set_interval(NULL, 1);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_interval(&task, -1);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_state_should_return_null_argument_error_when_task_argument_is_null()
{
    DodeeStatus status = dodee_task_set_state(NULL, Stopped);

    ssct_assert_equals(status, DODEE_ERROR_NULL_ARGUMENT);
}

static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_state(&task, -1);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range()
{
    DodeeTask task;

    DodeeStatus status = dodee_task_set_state(&task, DODEE_TASK_STATE_VALUES);

    ssct_assert_equals(status, DODEE_ERROR_INVALID_ARGUMENT);
}

