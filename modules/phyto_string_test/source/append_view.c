#include "phyto/string_test/append_view.h"

#include <phyto/string/string.h>

PHYTO_TEST_FUNC(string_append_view_empty) {
    phyto_string_t str = phyto_string_new();
    phyto_string_view_t view = phyto_string_view_from_c("");
    phyto_string_append_view(&str, view);
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), view),
                      phyto_string_free(&str),
                      "append_view('') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_append_view_basic) {
    phyto_string_t str = phyto_string_from_c("Hello, ");
    phyto_string_view_t view = phyto_string_view_from_c("world!");
    phyto_string_append_view(&str, view);
    phyto_string_view_t expected = phyto_string_view_from_c("Hello, world!");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), expected),
                      phyto_string_free(&str),
                      "append_view('world!') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_append_view_twice) {
    phyto_string_t str = phyto_string_from_c("Hello, ");
    phyto_string_view_t view = phyto_string_view_from_c("world!");
    phyto_string_append_view(&str, view);
    phyto_string_append_view(&str, view);
    phyto_string_view_t expected = phyto_string_view_from_c("Hello, world!world!");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), expected),
                      phyto_string_free(&str),
                      "append_view('world!') twice was not equal to append_view('world!')");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_append_view) {
    PHYTO_TEST_RUN(string_append_view_empty);
    PHYTO_TEST_RUN(string_append_view_basic);
    PHYTO_TEST_RUN(string_append_view_twice);
}