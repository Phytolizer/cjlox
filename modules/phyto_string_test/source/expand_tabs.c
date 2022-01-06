#include "phyto/string_test/expand_tabs.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>

static PHYTO_TEST_FUNC(empty) {
    phyto_string_view_t empty = phyto_string_view_empty();
    phyto_string_t expanded = phyto_string_expand_tabs(empty, 4);
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(expanded), empty),
                      phyto_string_free(&expanded),
                      "expand_tabs('') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(expanded));
    phyto_string_free(&expanded);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(zero) {
    phyto_string_view_t str = phyto_string_view_from_c("hello\tworld");
    phyto_string_t expanded = phyto_string_expand_tabs(str, 0);
    phyto_string_view_t expected = phyto_string_view_from_c("helloworld");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(expanded), expected),
                      phyto_string_free(&expanded),
                      "expand_tabs('hello\\tworld') modified the string to '%" PHYTO_STRING_FORMAT
                      "'",
                      PHYTO_STRING_PRINTF_ARGS(expanded));
    phyto_string_free(&expanded);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(eight) {
    phyto_string_view_t str = phyto_string_view_from_c("hello\tworld");
    phyto_string_t expanded = phyto_string_expand_tabs(str, 8);
    phyto_string_view_t expected = phyto_string_view_from_c("hello   world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(expanded), expected),
                      phyto_string_free(&expanded),
                      "expand_tabs('hello\\tworld') modified the string to '%" PHYTO_STRING_FORMAT
                      "'",
                      PHYTO_STRING_PRINTF_ARGS(expanded));
    phyto_string_free(&expanded);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(edge_cases) {
    phyto_string_view_t str = phyto_string_view_from_c("hello  \tworld");
    phyto_string_t expanded = phyto_string_expand_tabs(str, 8);
    phyto_string_view_t expected = phyto_string_view_from_c("hello   world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(expanded), expected),
                      phyto_string_free(&expanded),
                      "expand_tabs('hello  \\tworld') modified the string to '%" PHYTO_STRING_FORMAT
                      "'",
                      PHYTO_STRING_PRINTF_ARGS(expanded));
    phyto_string_free(&expanded);
    str = phyto_string_view_from_c("hello   \tworld");
    expanded = phyto_string_expand_tabs(str, 8);
    expected = phyto_string_view_from_c("hello           world");
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(expanded), expected),
        phyto_string_free(&expanded),
        "expand_tabs('hello   \\tworld') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(expanded));
    phyto_string_free(&expanded);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(expand_tabs) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(zero);
    PHYTO_TEST_RUN(eight);
    PHYTO_TEST_RUN(edge_cases);
}
