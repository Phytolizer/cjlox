#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <ansi_esc/ansi_esc.h>
#include <nonstd/asprintf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint64_t tests_passed;
    uint64_t tests_failed;
    uint64_t assert_count;
} phyto_test_state_t;

#define PHYTO_TEST_ASSERT(Test, Cleanup, ...)       \
    do {                                            \
        ++phyto_test_state->assert_count;           \
        if (!(Test)) {                              \
            char* message;                          \
            nonstd_asprintf(&message, __VA_ARGS__); \
            Cleanup;                                \
            return message;                         \
        }                                           \
    } while (false)

#define PHYTO_TEST_FAIL(...) PHYTO_TEST_ASSERT(false, (void)0, __VA_ARGS__)

#define PHYTO_TEST_RUN_SUITE(Name, State)     \
    do {                                      \
        fprintf(stderr, "SUITE " #Name "\n"); \
        phyto_test_suite_##Name(State);       \
    } while (false)

#define PHYTO_TEST_RUN(Name)                                                                  \
    do {                                                                                      \
        fprintf(stderr, "| " ANSI_ESC_FG_YELLOW "TEST" ANSI_ESC_RESET " " #Name "\n");        \
        char* message = phyto_test_##Name(phyto_test_state);                                  \
        if (message != NULL) {                                                                \
            ++phyto_test_state->tests_failed;                                                 \
            fprintf(stderr, "| -> " ANSI_ESC_FG_RED "FAIL" ANSI_ESC_RESET ": %s\n", message); \
            free(message);                                                                    \
        } else {                                                                              \
            fprintf(stderr, "| -> " ANSI_ESC_FG_GREEN "PASS" ANSI_ESC_RESET "\n");            \
            ++phyto_test_state->tests_passed;                                                 \
        }                                                                                     \
    } while (false)

#define PHYTO_TEST_RUN_SUBTEST(Name, Cleanup, ...)              \
    do {                                                        \
        char* message = phyto_test_subtest_##Name(__VA_ARGS__); \
        if (message != NULL) {                                  \
            Cleanup;                                            \
            return message;                                     \
        }                                                       \
    } while (false)

#define PHYTO_TEST_SUITE_FUNC(Name) \
    void phyto_test_suite_##Name(phyto_test_state_t* phyto_test_state)
#define PHYTO_TEST_FUNC(Name) char* phyto_test_##Name(phyto_test_state_t* phyto_test_state)
#define PHYTO_TEST_SUBTEST_FUNC(Name, ...) char* phyto_test_subtest_##Name(__VA_ARGS__)
#define PHYTO_TEST_PASS() return NULL
#define PHYTO_TEST_SUBTEST_PASS() return NULL

#endif  // TEST_TEST_H_
