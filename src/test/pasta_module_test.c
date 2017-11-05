#include <stdlib.h>         // malloc
#include <string.h>         // strncmp
#include <stdbool.h>        // bool, true, false
#include <stddef.h>         // size_t

#include "test.h"           // test_assert(), test_summary()
#include "pasta_error.h"    // Status, error codes
#include "pasta_module.h"   // Module, pasta_module_set_*() 

// Mock prototypes
static void *mock_allocator_will_return_null(size_t bytes);

// Hook prototypes 
static void when_members_free_check_name_is_null(char *name, char *command);
static void when_members_free_check_command_is_null(char *name, char *command);

// Test prototypes
static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols();
static void set_name_should_return_invalid_argument_error_when_argument_contains_non_printable_ascii_symbols();
static void set_name_should_return_null_argument_error_when_module_argument_is_null();
static void set_name_should_return_null_argument_error_when_name_argument_is_null();
static void set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long();
static void set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars();
static void set_name_should_return_buffer_overflow_error_when_name_argument_is_not_null_terminated();
static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();

static void set_command_should_return_null_argument_error_when_module_argument_is_null();
static void set_command_should_return_null_argument_error_when_command_argument_is_null();
static void set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax();
static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
static void set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars();
static void set_command_should_return_buffer_overflow_error_when_command_argument_is_not_null_terminated();

static void set_interval_should_return_null_argument_error_when_module_argument_is_null();
static void set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();

static void set_state_should_return_null_argument_error_when_module_argument_is_null();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
static void set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();

static void create_should_allocate_module_when_argument_is_null_pointer();
static void create_should_return_invalid_argument_error_when_module_argument_is_not_null();
static void create_should_return_malloc_fail_error_when_memory_allocation_fails();

static void destroy_should_free_module_when_argument_is_a_dynamically_allocated_module();
static void destroy_should_free_name_member_when_argument_is_a_dynamically_allocated_module();
static void destroy_should_free_command_member_when_argument_is_a_dynamically_allocated_module();

int main(void)
{
    set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols();
    set_name_should_return_invalid_argument_error_when_argument_contains_non_printable_ascii_symbols();
    set_name_should_return_null_argument_error_when_module_argument_is_null();
    set_name_should_return_null_argument_error_when_name_argument_is_null();
    set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long();
    set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars();
    set_name_should_return_buffer_overflow_error_when_name_argument_is_not_null_terminated();
    set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string();
    
    set_command_should_return_null_argument_error_when_module_argument_is_null();
    set_command_should_return_null_argument_error_when_command_argument_is_null();
    set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax();
    set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string();
    set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars();
    set_command_should_return_buffer_overflow_error_when_command_argument_is_not_null_terminated();
    
    set_interval_should_return_null_argument_error_when_module_argument_is_null();
    set_interval_should_return_invalid_argument_error_when_interval_argument_is_negative();
    
    set_state_should_return_null_argument_error_when_module_argument_is_null();
    set_state_should_return_invalid_argument_error_when_state_argument_is_negative();
    set_state_should_return_invalid_argument_error_when_state_argument_is_out_of_range();
    
    create_should_allocate_module_when_argument_is_null_pointer();
    create_should_return_malloc_fail_error_when_memory_allocation_fails();
    create_should_return_invalid_argument_error_when_module_argument_is_not_null();
    
    destroy_should_free_module_when_argument_is_a_dynamically_allocated_module();
    destroy_should_free_name_member_when_argument_is_a_dynamically_allocated_module();
    destroy_should_free_command_member_when_argument_is_a_dynamically_allocated_module();

    test_print_summary(); 

    return EXIT_SUCCESS;
}

static void *mock_allocator_will_return_null(size_t bytes) { return NULL; }


static void set_name_should_set_name_member_when_argument_consists_of_printable_ascii_symbols()
{
    static const char PRINTABLE_ASCII_CHARS[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    static const size_t PRINTABLE_ASCII_CHARS_LEN = sizeof (PRINTABLE_ASCII_CHARS);

    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, PRINTABLE_ASCII_CHARS, PRINTABLE_ASCII_CHARS_LEN);
    size_t min = PRINTABLE_ASCII_CHARS_LEN ?
                 PRINTABLE_ASCII_CHARS_LEN <= PASTA_MODULE_MAX_NAME_LEN :
                 PASTA_MODULE_MAX_NAME_LEN;

    test_assert(strncmp(module.name, PRINTABLE_ASCII_CHARS, min) == 0
            && status == PASTA_SUCCESS);
}

static void set_name_should_return_invalid_argument_error_when_argument_contains_non_printable_ascii_symbols()
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
        char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
        module.name = &(module_name[0]);

        Status status = pasta_module_set_name(&module, buffer, BUFFER_SIZE);

        if (status != PASTA_ERROR_INVALID_ARGUMENT)
        {
            expected_result = false;
            break;
        }
    }

    test_assert(expected_result); 
}

static void set_name_should_return_null_argument_error_when_module_argument_is_null()
{
    const char *null_module_name = "null module";

    Status status = pasta_module_set_name(NULL, null_module_name, sizeof (null_module_name));

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_return_null_argument_error_when_name_argument_is_null()
{
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, NULL, sizeof(module_name));

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_name_should_set_entire_name_argument_when_name_argument_is_100_chars_long()
{
    static const char STR_100_RND_CHARS[] = "L2jX0CMU2W39t4Lt8eYZYZmTniyiySdhRTKQyyZpgvlmdLeIoFJWyIlPdZjE8fUllpdZPgM5sRZ0S4ormacyIEe27xrh9WbgiPcE";
    static const size_t STR_100_RND_CHARS_SIZE = sizeof (STR_100_RND_CHARS);

    Module module;
    char module_name[STR_100_RND_CHARS_SIZE];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE);
    
    test_assert(strncmp(module.name, STR_100_RND_CHARS, STR_100_RND_CHARS_SIZE) == 0 && status == PASTA_SUCCESS);
}

static void set_name_should_return_buffer_overflow_error_when_name_argument_is_longer_than_100_chars()
{
    static const char STR_101_RND_CHARS[] = "ysrCek0ygdDVOZ7TTfxkAorGfER1YJEQ0zsUvhAILtgN9mI3hfDiS3GQaOqOlXJQRzV4pPVDBYtSiKuPXh5o6GDA0xP5Q4wuBSWQe";
    static const size_t STR_101_RND_CHARS_SIZE = sizeof(STR_101_RND_CHARS);

    Module module;
    char module_name[STR_101_RND_CHARS_SIZE];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, STR_101_RND_CHARS, STR_101_RND_CHARS_SIZE);

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_name_should_return_buffer_overflow_error_when_name_argument_is_not_null_terminated()
{
    static const char NON_NULL_TERMINATED_NAME[] = { 'H', 'e', 'l', 'l', 'o' };
    
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, NON_NULL_TERMINATED_NAME, sizeof (NON_NULL_TERMINATED_NAME));

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_name_should_return_invalid_argument_error_when_name_argument_is_an_empty_string()
{
    Module module;
    char module_name[PASTA_MODULE_MAX_NAME_LEN + 1];
    module.name = &(module_name[0]);

    Status status = pasta_module_set_name(&module, "", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_module_argument_is_null()
{
    static const char NULL_MOD_CMD[] = "echo 'hello'";

    Status status = pasta_module_set_command(NULL, NULL_MOD_CMD, sizeof (NULL_MOD_CMD));

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_null_argument_error_when_command_argument_is_null()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, NULL, 0);

    test_assert(status == PASTA_ERROR_NULL_ARGUMENT);
}

static void set_command_should_return_command_syntax_error_when_command_argument_has_invalid_syntax()
{
    static const char CMD_INVALID_SYNTAX[] = "while ture; do ehco 'hello there!'; dnoe";

    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, CMD_INVALID_SYNTAX, sizeof (CMD_INVALID_SYNTAX));

    test_assert(status == PASTA_ERROR_INVALID_COMMAND_SYNTAX);
}

static void set_command_should_return_invalid_argument_error_when_command_argument_is_an_empty_string()
{
    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, "", 0);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void set_command_should_return_buffer_overflow_error_when_command_argument_is_longer_than_1000_chars()
{
    static const char CMD_1001_CHARS[] = "echo 'HAb6kJxzWH1OXvYzFlnCvPHvd2LbICMcIl8E9JwbbMGiAIwm2q1UGV91apjORHy9dM4NhFTxXmY9gL858pivwzEU1M1wRSEiNqyGe7p3Hjk3NfRUN0SdRISEkQZUWSphSFHrjQCPLMQrxapzsr8KLU95rzzCADyp28fONouI6ke8Hk6lHPImegBWHwZ4hFqVyASyfIpatfUWzafRIIVABkwF6diPFBl9RbFaocyHslWddgQEJTiMY3tjMm5kzNisqqw5t1KVagFMP2I3814Bv1iHjXevXt5oI65UFWbxKnItbFRfQ98f3kjP3Tiwl8BsJ6CGUz02GrmDc6sueTJoy1I35RWWWe0Ud0y4nW8134qsi5g26yO8g05hLyPF0TJ3uL4hArvE21doiLyATeuFkzgu5tTydNgjMEvprMMRAgmRvc4Cm0Lq7UCJCdk7C2Ox5PBRdQfFOlcJINHcPsV5GQPkuwqfplhes6s6zC2dx7F5suuEGAM0iEavjrFM24DxVmoeg2wU9D5F2XvWRDiHIgV6lFnnw3GbGdnFHwHOlS07bRxoDUzaKuaSizF55j2CmIJ6sMgu84l0AQZro7EyXmmYtUH5N4krgPlV1ZYwChVHiL14zYe3xkgbJA0aZ5Hr4ude8bEidVU7wWff7sbgr9SjzYj8zRBtRx8XaNcupgivf6fVj9cFHxzkvTL24IhZeVC07eqa4VzBGgOvd6plMG2lF4JUwTxawNfYxO3HWooumA2OwfVGlObE9KxNAGYshFpptJ5fDOAhmz0WRFVdBWFkWwtmxm80IWN2fOWM0UvgYu4O3booYAXhhv1eM6QMBb3aIUuKMyGSZzjtUMAPlQeiHUc7d3sR9YA4uynmYHNOoi5zrZLBppFRgkmtk2H02XVQUEH3Xn1S3xqbhshETzs3Cc03sPtBtm03LVnNBW9E9RdkJCu59OsFWrTpZGOYExLEfqi0WFBegL4dipZPzUHmiaNKorE7m9'";

    Module module;
    char module_command[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_command[0]);

    Status status = pasta_module_set_command(&module, CMD_1001_CHARS, sizeof (CMD_1001_CHARS));

    test_assert(status == PASTA_ERROR_BUFFER_OVERFLOW);
}

static void set_command_should_return_buffer_overflow_error_when_command_argument_is_not_null_terminated()
{
    static const char NON_NULL_TERM_CMD[] = { 'e', 'c', 'h', 'o', ' ', 'H','e','l', 'l', 'o' };
    
    Module module;
    char module_cmd[PASTA_MODULE_MAX_CMD_LEN + 1];
    module.command = &(module_cmd[0]);

    Status status = pasta_module_set_command(&module, NON_NULL_TERM_CMD, sizeof (NON_NULL_TERM_CMD));

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

static void create_should_allocate_module_when_argument_is_null_pointer()
{
    Module *module = NULL;

    Status status = pasta_module_create(&module);

    test_assert(&module != NULL && status == PASTA_SUCCESS);
}

static void create_should_return_malloc_fail_error_when_memory_allocation_fails()
{
    pasta_module_set_allocator(mock_allocator_will_return_null);
    Module *module = NULL;

    Status status = pasta_module_create(&module);

    test_assert(status == PASTA_ERROR_MALLOC_FAIL);

    pasta_module_set_allocator(malloc);
}

static void create_should_return_invalid_argument_error_when_module_argument_is_not_null()
{
    Module *module = (Module *)malloc(sizeof (Module));
    
    Status status = pasta_module_create(&module);

    test_assert(status == PASTA_ERROR_INVALID_ARGUMENT);
}

static void destroy_should_free_module_when_argument_is_a_dynamically_allocated_module()
{
    Module *module_p = (Module *)malloc(sizeof (Module));
    char *module_name = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_NAME_LEN + 1));
    char *module_cmd = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_CMD_LEN + 1));
    module_p->name = module_name;
    module_p->command = module_cmd;

    Status status = pasta_module_destroy(&module_p);

    test_assert(module_p == NULL && status == PASTA_SUCCESS);
}

static void destroy_should_free_name_member_when_argument_is_a_dynamically_allocated_module()
{
    Module *module_p = (Module *)malloc(sizeof (Module));
    char *module_name = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_NAME_LEN + 1));
    char *module_cmd = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_CMD_LEN + 1));
    module_p->name = module_name;
    module_p->command = module_cmd;

    pasta_module_hook_into_destroy_before_struct_free(when_members_free_check_name_is_null);

    pasta_module_destroy(&module_p);

    pasta_module_clear_destroy_hooks();
}

static void when_members_free_check_name_is_null(char *name, char *command)
{
    test_assert_name(name == NULL, "destroy_should_free_name_member_when_argument_is_a_dynamically_allocated_module");
}

static void destroy_should_free_command_member_when_argument_is_a_dynamically_allocated_module()
{
    Module *module_p = (Module *)malloc(sizeof (Module));
    char *module_name = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_NAME_LEN + 1));
    char *module_cmd = (char *)malloc(sizeof (char) * (PASTA_MODULE_MAX_CMD_LEN + 1));
    module_p->name = module_name;
    module_p->command = module_cmd;

    pasta_module_hook_into_destroy_before_struct_free(when_members_free_check_command_is_null);

    pasta_module_destroy(&module_p);
    
    pasta_module_clear_destroy_hooks();
}

static void when_members_free_check_command_is_null(char *name, char *command)
{
    test_assert_name(command == NULL, "destroy_should_free_command_member_when_argument_is_dynamically_allocated_module");
}

