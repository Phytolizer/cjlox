#include "phyto/string_test/extend.h"

#include <phyto/string/string.h>

static PHYTO_TEST_FUNC(empty) {
    phyto_string_t str = phyto_string_new();
    phyto_string_span_t span = phyto_string_span_from_c("");
    phyto_string_extend(&str, span);
    PHYTO_TEST_ASSERT(phyto_string_span_equal(phyto_string_as_span(str), span),
                      phyto_string_free(&str),
                      "append_span('') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(basic) {
    phyto_string_t str = phyto_string_from_c("Hello, ");
    phyto_string_span_t span = phyto_string_span_from_c("world!");
    phyto_string_extend(&str, span);
    phyto_string_span_t expected = phyto_string_span_from_c("Hello, world!");
    PHYTO_TEST_ASSERT(phyto_string_span_equal(phyto_string_as_span(str), expected),
                      phyto_string_free(&str),
                      "append_span('world!') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(twice) {
    phyto_string_t str = phyto_string_from_c("Hello, ");
    phyto_string_span_t span = phyto_string_span_from_c("world!");
    phyto_string_extend(&str, span);
    phyto_string_extend(&str, span);
    phyto_string_span_t expected = phyto_string_span_from_c("Hello, world!world!");
    PHYTO_TEST_ASSERT(phyto_string_span_equal(phyto_string_as_span(str), expected),
                      phyto_string_free(&str),
                      "append_span('world!') twice was not equal to append_span('world!')");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(extend) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(basic);
    PHYTO_TEST_RUN(twice);
}