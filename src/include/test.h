#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdbool.h>

#define test_assert(test) test_assert_name(test, __func__) 

#define test_assert_name(test, func_name) \
    do \
    { \
        if ((test)) \
        { \
            tests_completed++; \
        } \
        else \
        { \
            printf("Assertion failed for %s()\n", (func_name)); \
            tests_failed++; \
        } \
        tests_run++; \
    } while (false)

#define test_succeed() test_assert(true)
#define test_fail() test_assert(false)

#define test_print_summary() \
    do \
    { \
        printf("Test %s: Ran %d test. %d succeeded and %d failed.\n", __FILE__, tests_run, tests_completed, tests_failed); \
    } while(false)

int tests_run = 0;
int tests_completed = 0;
int tests_failed = 0;

#endif /* TEST_H */
