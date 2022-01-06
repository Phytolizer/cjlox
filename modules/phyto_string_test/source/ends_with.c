#include "phyto/string_test/ends_with.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>

static PHYTO_TEST_FUNC(empty) {
    phyto_string_view_t empty = phyto_string_view_empty();
    phyto_string_view_t sub = phyto_string_view_from_c("test");
    PHYTO_TEST_ASSERT(!phyto_string_ends_with(empty, sub), (void)0,
                      "ends_with('', 'test') should return false");
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(empty_sub) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t empty = phyto_string_view_empty();
    PHYTO_TEST_ASSERT(!phyto_string_ends_with(str, empty), (void)0,
                      "ends_with('test', '') should return false");
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(entire) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t sub = phyto_string_view_from_c("test");
    PHYTO_TEST_ASSERT(phyto_string_ends_with(str, sub), (void)0,
                      "ends_with('test', 'test') should return true");
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(sanity_check) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t sub = phyto_string_view_from_c("est");
    PHYTO_TEST_ASSERT(phyto_string_ends_with(str, sub), (void)0,
                      "ends_with('test', 'est') should return true");
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(ends_with) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(empty_sub);
    PHYTO_TEST_RUN(entire);
    PHYTO_TEST_RUN(sanity_check);
}
