#include "phyto/string_test/center.h"

#include <phyto/string/string.h>

static PHYTO_TEST_FUNC(empty) {
    phyto_string_span_t str = phyto_string_span_empty();
    phyto_string_t centered = phyto_string_center(str, 10, ' ');
    PHYTO_TEST_ASSERT(phyto_string_span_equal(phyto_string_as_span(centered),
                                              phyto_string_span_from_c("          ")),
                      phyto_string_free(&centered),
                      "center('', 10, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(identity) {
    phyto_string_span_t str = phyto_string_span_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 10, ' ');
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(centered), str), phyto_string_free(&centered),
        "center('Hello, world!', 10, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(even) {
    phyto_string_span_t str = phyto_string_span_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 14, ' ');
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(centered),
                                phyto_string_span_from_c("Hello, world! ")),
        phyto_string_free(&centered),
        "center('Hello, world!', 14, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(longer) {
    phyto_string_span_t str = phyto_string_span_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 40, '=');
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(
            phyto_string_as_span(centered),
            phyto_string_span_from_c("=============Hello, world!==============")),
        phyto_string_free(&centered),
        "center('Hello, world!', 40, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(center) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(identity);
    PHYTO_TEST_RUN(even);
    PHYTO_TEST_RUN(longer);
}
