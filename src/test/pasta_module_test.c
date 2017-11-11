#include <stdlib.h>             // malloc
#include <string.h>             // strncmp
#include <stdbool.h>            // bool, true, false
#include <stddef.h>             // size_t

#include "test.h"               // test_assert(), test_summary()
#include "pasta_status_codes.h" // Status, error codes
#include "pasta_module.h"       // Module, pasta_module_set_*()

// Test prototypes
static void module_should_set_all_module_members_when_setters_are_called();
static void module_setters_should_set_all_module_members_when_module_is_dynamically_allocated();

static void init_should_return_null_argument_error_when_module_argument_is_null();
static void init_should_set_members_to_default_values();

static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols();
static void set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols();
static void set_name_should_return_null_argument_error_when_module_argument_is_null();
static void set_name_should_return_null_argument_error_when_name_argument_is_null();
static void set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero();
static void set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed();
static void set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed();
static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();

static void set_command_should_set_command_member_when_command_argument_is_a_valid_command();
static void set_command_should_return_null_argument_error_when_module_argument_is_null();
static void set_command_should_return_null_argument_error_when_command_argument_is_null();
static void set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero();
static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
static void set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed();
static void set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed();

static void set_interval_should_return_null_argument_error_when_module_argument_is_null();
static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();

static void set_state_should_return_null_argument_error_when_module_argument_is_null();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();

int main(void)
{
    module_should_set_all_module_members_when_setters_are_called();
    module_setters_should_set_all_module_members_when_module_is_dynamically_allocated();

    init_should_return_null_argument_error_when_module_argument_is_null();
    init_should_set_members_to_default_values();

    set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols();
    set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols();
    set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero();
    set_name_should_return_null_argument_error_when_module_argument_is_null();
    set_name_should_return_null_argument_error_when_name_argument_is_null();
    set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed();
    set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed();
    set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();

    set_command_should_set_command_member_when_command_argument_is_a_valid_command();
    set_command_should_return_null_argument_error_when_module_argument_is_null();
    set_command_should_return_null_argument_error_when_command_argument_is_null();
    set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
    set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero();
    set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed();
    set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed();

    set_interval_should_return_null_argument_error_when_module_argument_is_null();
    set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();

    set_state_should_return_null_argument_error_when_module_argument_is_null();
    set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
    set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();

    test_print_summary();

    return EXIT_SUCCESS;
}

static void init_should_return_null_argument_error_when_module_argument_is_null()
{
    Status status = pasta_module_init(NULL);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void init_should_set_members_to_default_values()
{
    Module module;

    Status status = pasta_module_init(&module);

    bool names_match = (module.name[0] == '\0');
    bool cmds_match = (module.command[0] == '\0');
    bool intervals_match = (module.interval_seconds == 0);
    bool states_match = (module.state == Stopped);

    test_assert(status == PASTA_SUCCESS && names_match && cmds_match && intervals_match && states_match);
}

static void module_should_set_all_module_members_when_setters_are_called()
{
    static const char MOD_NAME[] = "test module";
    static const size_t MOD_NAME_LEN = sizeof (MOD_NAME) - 1;
    static const char MOD_CMD[] = "echo testing";
    static const size_t MOD_CMD_LEN = sizeof (MOD_CMD) - 1;
    static const int MOD_INTERVAL = 10;
    static const ModuleState MOD_STATE = Running;

    Module mod;
    
    pasta_module_set_name(&mod, MOD_NAME, MOD_NAME_LEN);
    pasta_module_set_command(&mod, MOD_CMD, MOD_CMD_LEN);
    pasta_module_set_interval(&mod, MOD_INTERVAL);
    pasta_module_set_state(&mod, MOD_STATE);

    bool names_match = (strncmp(mod.name, MOD_NAME, MOD_NAME_LEN) == 0);
    bool cmds_match = (strncmp(mod.command, MOD_CMD, MOD_CMD_LEN) == 0);
    bool intervals_match = (mod.interval_seconds == MOD_INTERVAL);
    bool states_match = (mod.state == MOD_STATE);

    test_assert(names_match && cmds_match && intervals_match && states_match);
}

static void module_setters_should_set_all_module_members_when_module_is_dynamically_allocated()
{
    static const char MOD_NAME[] = "dyn module";
    static const size_t MOD_NAME_LEN = sizeof (MOD_NAME) - 1;
    static const char MOD_CMD[] = "echo dynamic";
    static const size_t MOD_CMD_LEN = sizeof (MOD_CMD) - 1;
    static const int MOD_INTERVAL = 5;
    static const ModuleState MOD_STATE = Stopped;

    Module *mod_p = (Module *)malloc(sizeof (Module));

    pasta_module_set_name(mod_p, MOD_NAME, MOD_NAME_LEN);
    pasta_module_set_command(mod_p, MOD_CMD, MOD_CMD_LEN);
    pasta_module_set_interval(mod_p, MOD_INTERVAL);
    pasta_module_set_state(mod_p, MOD_STATE);

    bool names_match = (strncmp(mod_p->name, MOD_NAME, MOD_NAME_LEN) == 0);
    bool cmds_match = (strncmp(mod_p->command, MOD_CMD, MOD_CMD_LEN) == 0);
    bool intervals_match = (mod_p->interval_seconds == MOD_INTERVAL);
    bool states_match = (mod_p->state == MOD_STATE);

    free(mod_p);

    test_assert(names_match && cmds_match && intervals_match && states_match);
}

static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols()
{
    static const char PRINTABLE_ASCII_CHARS[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    static const size_t PRINTABLE_ASCII_CHARS_LEN = sizeof (PRINTABLE_ASCII_CHARS) - 1;

    Module module;

    Status status = pasta_module_set_name(&module, PRINTABLE_ASCII_CHARS, PRINTABLE_ASCII_CHARS_LEN);
    size_t min = (PRINTABLE_ASCII_CHARS_LEN <= PASTA_MODULE_MAX_NAME_LEN) ?
                  PRINTABLE_ASCII_CHARS_LEN:
                  PASTA_MODULE_MAX_NAME_LEN;

    test_assert(strncmp(module.name, PRINTABLE_ASCII_CHARS, min) == 0
            && status == PASTA_SUCCESS);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_contains_non_printable_ascii_symbols()
{
    static const char FIRST_NON_PRINTABLE_ASCII_CHAR = '\x1';
    static const char FIRST_PRINTABLE_ASCII_CHAR = ' ';
    static const int BUFFER_SIZE = 2;

    bool expected_result = true;

    for (char non_printable = FIRST_NON_PRINTABLE_ASCII_CHAR; non_printable < FIRST_PRINTABLE_ASCII_CHAR; non_printable++)
    {
        char buffer[BUFFER_SIZE];
        buffer[0] = non_printable;
        buffer[BUFFER_SIZE - 1] = '\0';
        Module module;

        Status status = pasta_module_set_name(&module, buffer, BUFFER_SIZE - 1);

        if (status != PASTA_ERROR_INVALID_ARGUMENT)
        {
            expected_result = false;
            break;
        }
    }

    test_assert(expected_result);
}

static void set_name_should_return_invalid_argument_error_when_name_len_argument_is_equal_to_zero()
{
    Module mod;

    Status status = pasta_module_set_name(&mod, "hello", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_module_argument_is_null()
{
    const char *null_module_name = "null module";

    Status status = pasta_module_set_name(NULL, null_module_name, sizeof (null_module_name) - 1);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_name_argument_is_null()
{
    Module module;

    Status status = pasta_module_set_name(&module, NULL, sizeof(module.name) - 1);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_set_entire_name_member_when_name_argument_length_is_equal_to_max_allowed()
{
    static const char STR_100_RND_CHARS[] = "L2jX0CMU2W39t4Lt8eYZYZmTniyiySdhRTKQyyZpgvlmdLeIoFJWyIlPdZjE8fUllpdZPgM5sRZ0S4ormacyIEe27xrh9WbgiPcE";
    static const size_t STR_100_RND_CHARS_SIZE = sizeof (STR_100_RND_CHARS) - 1;

    Module module;

    Status status = pasta_module_set_name(&module, STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE);

    test_assert(strncmp(module.name, STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE) == 0 && status == PASTA_SUCCESS);
}

static void set_name_should_return_buffer_overflow_error_when_name_argument_length_is_longer_than_max_allowed()
{
    static const char STR_101_RND_CHARS[] = "ysrCek0ygdDVOZ7TTfxkAorGfER1YJEQ0zsUvhAILtgN9mI3hfDiS3GQaOqOlXJQRzV4pPVDBYtSiKuPXh5o6GDA0xP5Q4wuBSWQe";
    static const size_t STR_101_RND_CHARS_SIZE = sizeof(STR_101_RND_CHARS) - 1;

    Module module;

    Status status = pasta_module_set_name(&module, STR_101_RND_CHARS, STR_101_RND_CHARS_SIZE);

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string()
{
    Module module;

    Status status = pasta_module_set_name(&module, "", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_set_command_member_when_command_argument_is_a_valid_command()
{
    static const char VALID_CMD[] = "echo hello";
    static const int VALID_CMD_LEN = sizeof(VALID_CMD) - 1;

    Module mod;

    Status status = pasta_module_set_command(&mod, VALID_CMD, VALID_CMD_LEN);

    test_assert(status == PASTA_SUCCESS);
}

static void set_command_should_return_null_argument_error_when_module_argument_is_null()
{
    static const char NULL_MOD_CMD[] = "echo 'hello'";

    Status status = pasta_module_set_command(NULL, NULL_MOD_CMD, sizeof (NULL_MOD_CMD) - 1);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_command_argument_is_null()
{
    Module module;

    Status status = pasta_module_set_command(&module, NULL, 0);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string()
{
    Module module;

    Status status = pasta_module_set_command(&module, "", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_invalid_argument_error_when_cmd_len_argument_is_equal_to_zero()
{
    Module mod;

    Status status = pasta_module_set_command(&mod, "hello", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_set_entire_command_member_when_command_argument_length_is_equal_to_max_allowed()
{
    static const char CMD_1000_CHARS[] = "echo 'HAbkJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'";
    static const size_t CMD_1000_CHARS_LEN = sizeof (CMD_1000_CHARS) - 1;

    Module module;

    Status status = pasta_module_set_command(&module, CMD_1000_CHARS, CMD_1000_CHARS_LEN);

    test_assert(strncmp(module.command, CMD_1000_CHARS, CMD_1000_CHARS_LEN) == 0 && status == PASTA_SUCCESS);
}

static void set_command_should_return_buffer_overflow_error_when_command_argument_length_is_longer_than_max_allowed()
{
    static const char CMD_1001_CHARS[] = "echo 'HAb6kJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'";

    Module module;

    Status status = pasta_module_set_command(&module, CMD_1001_CHARS, sizeof (CMD_1001_CHARS) - 1);

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_interval_should_return_null_argument_error_when_module_argument_is_null()
{
    Status status = pasta_module_set_interval(NULL, 1);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative()
{
    Module module;

    Status status = pasta_module_set_interval(&module, -1);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_state_should_return_null_argument_error_when_module_argument_is_null()
{
    Status status = pasta_module_set_state(NULL, Stopped);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative()
{
    Module module;

    Status status = pasta_module_set_state(&module, -1);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range()
{
    Module module;

    Status status = pasta_module_set_state(&module, PASTA_MODULE_STATE_VALUES);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

