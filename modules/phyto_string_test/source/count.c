#include "phyto/string_test/count.h"

#include <phyto/string/string.h>
#include <stddef.h>

static PHYTO_TEST_FUNC(empty) {
    phyto_string_span_t str = phyto_string_span_empty();
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("hello"));
    PHYTO_TEST_ASSERT(count == 0, (void)0, "count should be 0, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(empty_sub) {
    phyto_string_span_t str = phyto_string_span_from_c("hello");
    size_t count = phyto_string_count_sub(str, phyto_string_span_empty());
    PHYTO_TEST_ASSERT(count == 0, (void)0, "count should be 0, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(entire) {
    phyto_string_span_t str = phyto_string_span_from_c("hello");
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("hello"));
    PHYTO_TEST_ASSERT(count == 1, (void)0, "count should be 1, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(single_char) {
    phyto_string_span_t str = phyto_string_span_from_c("hello");
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("h"));
    PHYTO_TEST_ASSERT(count == 1, (void)0, "count should be 1, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(single_char_twice) {
    phyto_string_span_t str = phyto_string_span_from_c("hello");
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("l"));
    PHYTO_TEST_ASSERT(count == 2, (void)0, "count should be 2, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(overlapping) {
    phyto_string_span_t str = phyto_string_span_from_c("banana");
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("ana"));
    PHYTO_TEST_ASSERT(count == 2, (void)0, "count should be 2, not %zu", count);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(thrice) {
    phyto_string_span_t str = phyto_string_span_from_c("aaa");
    size_t count = phyto_string_count_sub(str, phyto_string_span_from_c("a"));
    PHYTO_TEST_ASSERT(count == 3, (void)0, "count should be 3, not %zu", count);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(count) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(empty_sub);
    PHYTO_TEST_RUN(entire);
    PHYTO_TEST_RUN(single_char);
    PHYTO_TEST_RUN(single_char_twice);
    PHYTO_TEST_RUN(overlapping);
    PHYTO_TEST_RUN(thrice);
}
