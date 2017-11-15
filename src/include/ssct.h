#ifndef SSCT_H
#define SSCT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

static void (*ssct_setup)() = NULL;
static void (*ssct_teardown)() = NULL;

static int ssct_assertions_failed;
static int ssct_tests_run = 0;
static int ssct_tests_successful = 0;
static int ssct_tests_failed = 0;

#define ssct_run(func)                                                        \
    do                                                                        \
    {                                                                         \
        if (ssct_setup != NULL) { ssct_setup(); }                             \
        ssct_assertions_failed = 0;                                           \
        func();                                                               \
        if (ssct_assertions_failed > 0)                                       \
        {                                                                     \
            ssct_tests_failed++;                                              \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            ssct_tests_successful++;                                          \
        }                                                                     \
        ssct_tests_run++;                                                     \
        if (ssct_teardown != NULL) { ssct_teardown(); }                       \
    }                                                                         \
    while (false)

#define GET_TRUE_MACRO(_1, _2, NAME, ...) NAME
#define ssct_assert_true(...) GET_TRUE_MACRO(__VA_ARGS__,                     \
        ssct_assert_true_msg,                                                 \
        ssct_assert_true_no_msg,                                              \
        UNUSED)(__VA_ARGS__)

#define ssct_assert_true_msg(test, msg)                                       \
do                                                                            \
{                                                                             \
    if ( !(test))                                                             \
    {                                                                         \
        if ((msg)[0] == '\0')                                                 \
        {                                                                     \
            printf("Assertion failed for %s() on line %d.\n", __func__, __LINE__); \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            printf("Assertion failed for %s() on line %d: '%s'\n", __func__, __LINE__, (msg)); \
        }                                                                     \
        ssct_assertions_failed++;                                                  \
    }                                                                         \
} while (false)

#define ssct_assert_true_no_msg(test) ssct_assert_true_msg(test, "")

#define ssct_assert_false(test) ssct_assert_true_no_msg(!(test))

#define GET_EQUALS_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define ssct_assert_equals(...) GET_EQUALS_MACRO(__VA_ARGS__,                 \
        ssct_assert_string_equals,                                            \
        ssct_assert_float_equals,                                             \
        ssct_assert_int_equals,                                               \
        UNUSED)(__VA_ARGS__)

#define ssct_assert_int_equals(actual, expected)                              \
    do                                                                        \
    {                                                                         \
        if ((actual) != (expected))                                           \
        {                                                                     \
            printf("Assertion failed for %s() on line %d. Was %d but expected %d.\n",    \
                    (__func__), (__LINE__), (actual), (expected));                        \
            ssct_assertions_failed++;                                              \
        }                                                                     \
    } while (false)

#define ssct_assert_float_equals(actual, expected, error)                     \
    do                                                                        \
    {                                                                         \
        if (fabs((actual) - (expected)) > (error))                            \
        {                                                                     \
            printf("Assertion failed for %s() on line %d. Was %f but expected %f.\n",    \
                    (__func__), __LINE__, (actual), (expected));                        \
            ssct_assertions_failed++;                                              \
        }                                                                     \
    } while (false)

#define ssct_assert_string_equals(actual, actual_len, expected, expected_len) \
    do                                                                        \
    {                                                                         \
        if (strncmp(actual, expected,                                         \
                    ((actual_len) < (expected_len) ?                          \
                     (actual_len) :                                           \
                     (expected_len)) == 0))                                   \
        {                                                                     \
            printf("Assertion failed for %s() on line %d. Was %s but expected %s.\n",    \
                    (__func__), __LINE__, (actual), (expected));              \
            ssct_assertions_failed++;                                              \
        }                                                                     \
    } while (false)

#define ssct_assert_zero(actual) ssct_assert_equals(actual, 0);
#define ssct_assert_empty(actual) ssct_assert_equals(actual, 1, "", 0)

#define ssct_succeed() ssct_assert_true_no_msg(true)
#define ssct_fail() ssct_assert_true_no_msg(false)

#define ssct_print_summary() \
    do \
    { \
        printf("Test %s: Ran %d test. %d succeeded and %d failed.\n\n", __FILE__, ssct_tests_run, ssct_tests_successful, ssct_tests_failed); \
    } while(false)

#endif /* SSCT_H */
