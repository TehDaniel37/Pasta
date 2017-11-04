#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "test.h"
#include "pasta_error.h"
#include "pasta_module.h"

#define STRING_100_CHARS "L2jX0CMU2W39t4Lt8eYZYZmTniyiySdhRTKQyyZpgvlmdLeIoFJWyIlPdZjE8fUllpdZPgM5sRZ0S4ormacyIEe27xrh9WbgiPcE"
#define STRING_101_CHARS "ysrCek0ygdDVOZ7TTfxkAorGfER1YJEQ0zsUvhAILtgN9mI3hfDiS3GQaOqOlXJQRzV4pPVDBYtSiKuPXh5o6GDA0xP5Q4wuBSWQe"
#define COMMAND_INVALID_SYNTAX "while ture; do ehco 'hello there!'; dnoe"
#define COMMAND_1001_CHARS "echo 'HAb6kJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'"

static void set_name_should_return_null_argument_error_when_module_argument_is_null();
static void set_name_should_return_null_argument_error_when_name_argument_is_null();
static void set_name_should_return_null_member_error_when_module_is_uninitialized();
static void set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long();
static void set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars();
static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();

static void set_command_should_return_null_argument_error_when_module_argument_is_null();
static void set_command_should_return_null_argument_error_when_command_argument_is_null();
static void set_command_should_return_null_member_error_when_module_is_uninitialized();
static void set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax();
static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
static void set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars();

static void set_interval_should_return_null_argument_error_when_module_argument_is_null();
static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();

static void set_state_should_return_null_argument_error_when_module_argument_is_null();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();

static void create_should_return_malloc_error_if_memory_allocation_fails();

static void destroy_should_return_free_null_error_if_module_argument_is_null();
static void destroy_should_return_free_null_error_if_module_name_is_null();
static void destroy_should_return_free_null_error_if_module_command_is_null();

int main(void)
{
    set_name_should_return_null_argument_error_when_module_argument_is_null();
    set_name_should_return_null_argument_error_when_name_argument_is_null();
    set_name_should_return_null_member_error_when_module_is_uninitialized();
    set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long();
    set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars();
    set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();
    
    set_command_should_return_null_argument_error_when_module_argument_is_null();
    set_command_should_return_null_argument_error_when_command_argument_is_null();
    set_command_should_return_null_member_error_when_module_is_uninitialized();
    set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax();
    set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
    set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars();
    
    set_interval_should_return_null_argument_error_when_module_argument_is_null();
    set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();
    
    set_state_should_return_null_argument_error_when_module_argument_is_null();
    set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
    set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();
    
    create_should_return_malloc_error_if_memory_allocation_fails();
    
    destroy_should_return_free_null_error_if_module_argument_is_null();
    destroy_should_return_free_null_error_if_module_name_is_null();
    destroy_should_return_free_null_error_if_module_command_is_null();

    test_print_summary(); 

    return EXIT_SUCCESS;
}

static void set_name_should_return_null_argument_error_when_module_argument_is_null()
{
    Status status = pasta_module_set_name(NULL, "null module");

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_name_argument_is_null()
{
    Module module;
    char module_name[100];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, NULL);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_return_null_member_error_when_module_is_uninitialized()
{
    Module uninitialized_module;
    
    Status status = pasta_module_set_name(&uninitialized_module, "name");

    test_assert(status == PASTA_ERROR_NULL_MEMBER);
}

static void set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long()
{
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    pasta_module_set_name(&module, STRING_100_CHARS);
    
    test_assert(strncmp(module.name, STRING_100_CHARS, PASTA_MODULE_MAX_NAME_LEN) == 0);
}

static void set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars()
{
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 2];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, STRING_101_CHARS);

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string()
{
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, "");

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_module_argument_is_null()
{
    Status status = pasta_module_set_command(NULL, "echo 'hello'");

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_command_argument_is_null()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, NULL);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_null_member_error_when_module_is_uninitialized()
{
    Module module;

    Status status = pasta_module_set_command(&module, "echo 'hello'");

    test_assert(status == PASTA_ERROR_NULL_MEMBER);
}

static void set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, COMMAND_INVALID_SYNTAX);

    test_assert(status == PASTA_ERROR_COMMAND_SYNTAX);
}

static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, "");

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, COMMAND_1001_CHARS);

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
    // TODO: Also test to assure that module command is not set 
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

static void create_should_return_malloc_error_if_memory_allocation_fails()
{
    // TODO: Implement hook to be able to specify which allocator to use
    test_assert(false);
}

static void destroy_should_return_free_null_error_if_module_argument_is_null()
{
    Status status = pasta_module_destroy(NULL);

    test_assert(status == PASTA_ERROR_FREE_NULL);
}

static void destroy_should_return_free_null_error_if_module_name_is_null()
{
    char *module_command = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_CMD_LEN + 1));
    Module *module = (Module *)malloc(sizeof (Module));
    module->name = NULL;
    module->command = module_command;

    Status status = pasta_module_destroy(module);

    test_assert(status == PASTA_ERROR_FREE_NULL);
}

static void destroy_should_return_free_null_error_if_module_command_is_null()
{
    char *module_name = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_CMD_LEN + 1));
    Module *module = (Module *)malloc(sizeof (Module));
    module->name = module_name;
    module->command = NULL;

    Status status = pasta_module_destroy(module);

    test_assert(status == PASTA_ERROR_FREE_NULL);
}
