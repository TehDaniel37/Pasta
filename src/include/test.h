#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define test_assert(test) \
    do \
    { \
        if ((test)) \
        { \
            printf("Assertion succeeded for %s()\n", __func__); \
            tests_completed++; \
        } \
        else \
        { \
            printf("Assertion failed for %s()\n", __func__); \
            tests_failed++; \
        } \
        tests_run++; \
    } while (0)

#define test_print_summary() \
    do \
    { \
        printf("Test %s: Ran %d test. %d succeeded and %d failed.\n", __FILE__, tests_run, tests_completed, tests_failed); \
    } while(0)

int tests_run = 0;
int tests_completed = 0;
int tests_failed = 0;

#endif /* TEST_H */
