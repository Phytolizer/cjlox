#include "phyto/string_test/center.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>

PHYTO_TEST_FUNC(string_center_empty) {
    phyto_string_view_t str = phyto_string_view_empty();
    phyto_string_t centered = phyto_string_center(str, 10, ' ');
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(centered),
                                              phyto_string_view_from_c("          ")),
                      phyto_string_free(&centered),
                      "center('', 10, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_center_identity) {
    phyto_string_view_t str = phyto_string_view_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 10, ' ');
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(centered), str), phyto_string_free(&centered),
        "center('Hello, world!', 10, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_center_even) {
    phyto_string_view_t str = phyto_string_view_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 14, ' ');
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(centered),
                                phyto_string_view_from_c("Hello, world! ")),
        phyto_string_free(&centered),
        "center('Hello, world!', 14, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_center_long) {
    phyto_string_view_t str = phyto_string_view_from_c("Hello, world!");
    phyto_string_t centered = phyto_string_center(str, 40, '=');
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(
            phyto_string_view(centered),
            phyto_string_view_from_c("=============Hello, world!==============")),
        phyto_string_free(&centered),
        "center('Hello, world!', 40, ' ') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(centered));
    phyto_string_free(&centered);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_center) {
    PHYTO_TEST_RUN(string_center_empty);
    PHYTO_TEST_RUN(string_center_identity);
    PHYTO_TEST_RUN(string_center_even);
    PHYTO_TEST_RUN(string_center_long);
}
