#include "phyto/string_test/append_fill.h"

#include <phyto/string/string.h>
#include <phyto/test/test.h>

PHYTO_TEST_FUNC(string_append_fill_zero) {
    phyto_string_t str = phyto_string_own(phyto_string_view_from_c("Hello, world!"));
    phyto_string_append_fill(&str, 0, '!');
    phyto_string_view_t view = phyto_string_view_from_c("Hello, world!");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), view),
                      phyto_string_free(&str),
                      "append_fill(0, '!') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_append_fill_basic) {
    phyto_string_t str = phyto_string_new();
    phyto_string_append_fill(&str, 5, '!');
    phyto_string_view_t view = phyto_string_view_from_c("!!!!!");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), view),
                      phyto_string_free(&str),
                      "append_fill(5, '!') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_append_fill_twice) {
    phyto_string_t str = phyto_string_new();
    phyto_string_append_fill(&str, 5, '!');
    phyto_string_append_fill(&str, 5, '!');
    phyto_string_t str2 = phyto_string_new();
    phyto_string_append_fill(&str2, 10, '!');
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), phyto_string_view(str2)),
                      phyto_string_free(&str),
                      "append_fill(5, '!') twice was not equal to append_fill(10, '!')");
    phyto_string_free(&str);
    phyto_string_free(&str2);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_append_fill) {
    PHYTO_TEST_RUN(string_append_fill_zero);
    PHYTO_TEST_RUN(string_append_fill_basic);
    PHYTO_TEST_RUN(string_append_fill_twice);
}
