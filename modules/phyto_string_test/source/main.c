#include <inttypes.h>
#include <phyto/string/string.h>
#include <phyto/test/test.h>
#include <stdio.h>

#include "phyto/string_test/append_fill.h"
#include "phyto/string_test/append_view.h"
#include "phyto/string_test/capitalize.h"
#include "phyto/string_test/center.h"
#include "phyto/string_test/count.h"
#include "phyto/string_test/ends_with.h"
#include "phyto/string_test/expand_tabs.h"
#include "phyto/string_test/own.h"
#include "phyto/string_test/starts_with.h"

void all_tests(phyto_test_state_t* state) {
    PHYTO_TEST_RUN_SUITE(append_fill, state);
    PHYTO_TEST_RUN_SUITE(append_view, state);
    PHYTO_TEST_RUN_SUITE(capitalize, state);
    PHYTO_TEST_RUN_SUITE(center, state);
    PHYTO_TEST_RUN_SUITE(count, state);
    PHYTO_TEST_RUN_SUITE(ends_with, state);
    PHYTO_TEST_RUN_SUITE(expand_tabs, state);
    // TODO more tests
    PHYTO_TEST_RUN_SUITE(own, state);
    PHYTO_TEST_RUN_SUITE(starts_with, state);
}

int main(void) {
    phyto_test_state_t state = {0};
    all_tests(&state);
    printf("%" PRIu64 " tests, %" PRIu64 " failures, %" PRIu64 " assertions\n",
           state.tests_passed + state.tests_failed, state.tests_failed, state.assert_count);
    return (int)state.tests_failed;
}
