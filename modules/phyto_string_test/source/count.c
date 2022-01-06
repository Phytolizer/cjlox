#include "phyto/string_test/count.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>
#include <stddef.h>

PHYTO_TEST_FUNC(string_count_empty) {
    phyto_string_view_t str = phyto_string_view_empty();
    size_t count = phyto_string_count(str, phyto_string_view_from_c("hello"));
    PHYTO_TEST_ASSERT(count == 0, (void)0, "count should be 0, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_empty_sub) {
    phyto_string_view_t str = phyto_string_view_from_c("hello");
    size_t count = phyto_string_count(str, phyto_string_view_empty());
    PHYTO_TEST_ASSERT(count == 0, (void)0, "count should be 0, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_entire) {
    phyto_string_view_t str = phyto_string_view_from_c("hello");
    size_t count = phyto_string_count(str, phyto_string_view_from_c("hello"));
    PHYTO_TEST_ASSERT(count == 1, (void)0, "count should be 1, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_single_char) {
    phyto_string_view_t str = phyto_string_view_from_c("hello");
    size_t count = phyto_string_count(str, phyto_string_view_from_c("h"));
    PHYTO_TEST_ASSERT(count == 1, (void)0, "count should be 1, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_single_char_twice) {
    phyto_string_view_t str = phyto_string_view_from_c("hello");
    size_t count = phyto_string_count(str, phyto_string_view_from_c("l"));
    PHYTO_TEST_ASSERT(count == 2, (void)0, "count should be 2, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_overlapping) {
    phyto_string_view_t str = phyto_string_view_from_c("banana");
    size_t count = phyto_string_count(str, phyto_string_view_from_c("ana"));
    PHYTO_TEST_ASSERT(count == 2, (void)0, "count should be 2, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_count_thrice) {
    phyto_string_view_t str = phyto_string_view_from_c("aaa");
    size_t count = phyto_string_count(str, phyto_string_view_from_c("a"));
    PHYTO_TEST_ASSERT(count == 3, (void)0, "count should be 3, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_count) {
    PHYTO_TEST_RUN(string_count_empty);
    PHYTO_TEST_RUN(string_count_empty_sub);
    PHYTO_TEST_RUN(string_count_entire);
    PHYTO_TEST_RUN(string_count_single_char);
    PHYTO_TEST_RUN(string_count_single_char_twice);
    PHYTO_TEST_RUN(string_count_overlapping);
    PHYTO_TEST_RUN(string_count_thrice);
}
