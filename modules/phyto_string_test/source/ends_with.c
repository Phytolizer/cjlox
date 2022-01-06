#include "phyto/string_test/ends_with.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>

PHYTO_TEST_FUNC(string_ends_with_empty) {
    phyto_string_view_t empty = phyto_string_view_empty();
    phyto_string_view_t sub = phyto_string_view_from_c("test");
    PHYTO_TEST_ASSERT(!phyto_string_ends_with(empty, sub), (void)0,
                      "ends_with('', 'test') should return false");
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_ends_with_empty_sub) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t empty = phyto_string_view_empty();
    PHYTO_TEST_ASSERT(!phyto_string_ends_with(str, empty), (void)0,
                      "ends_with('test', '') should return false");
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_ends_with_entire) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t sub = phyto_string_view_from_c("test");
    PHYTO_TEST_ASSERT(phyto_string_ends_with(str, sub), (void)0,
                      "ends_with('test', 'test') should return true");
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_ends_with_sanity_check) {
    phyto_string_view_t str = phyto_string_view_from_c("test");
    phyto_string_view_t sub = phyto_string_view_from_c("est");
    PHYTO_TEST_ASSERT(phyto_string_ends_with(str, sub), (void)0,
                      "ends_with('test', 'est') should return true");
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_ends_with) {
    PHYTO_TEST_RUN(string_ends_with_empty);
    PHYTO_TEST_RUN(string_ends_with_empty_sub);
    PHYTO_TEST_RUN(string_ends_with_entire);
    PHYTO_TEST_RUN(string_ends_with_sanity_check);
}
