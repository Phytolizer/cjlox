#include "phyto/string_test/capitalize.h"

#include "phyto/string/string.h"

PHYTO_TEST_FUNC(string_capitalize_empty) {
    phyto_string_t str = phyto_string_new();
    phyto_string_t caps = phyto_string_capitalize(phyto_string_view(str));
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(caps), phyto_string_view_from_c("")),
        phyto_string_free(&str), "capitalize('') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_capitalize_allcaps) {
    phyto_string_t str = phyto_string_from_c("HELLO WORLD");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_view(str));
    phyto_string_view_t expected = phyto_string_view_from_c("Hello world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(caps), expected),
                      phyto_string_free(&str),
                      "capitalize('HELLO WORLD') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_capitalize_alllower) {
    phyto_string_t str = phyto_string_from_c("hello world");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_view(str));
    phyto_string_view_t expected = phyto_string_view_from_c("Hello world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(caps), expected),
                      phyto_string_free(&str),
                      "capitalize('hello world') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_capitalize_mixed) {
    phyto_string_t str = phyto_string_from_c("hElLo wOrLd");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_view(str));
    phyto_string_view_t expected = phyto_string_view_from_c("Hello world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(caps), expected),
                      phyto_string_free(&str),
                      "capitalize('hElLo wOrLd') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_capitalize_nochange) {
    phyto_string_t str = phyto_string_from_c("Hello world");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_view(str));
    phyto_string_view_t expected = phyto_string_view_from_c("Hello world");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(caps), expected),
                      phyto_string_free(&str),
                      "capitalize('Hello world') modified the string to '%" PHYTO_STRING_FORMAT "'",
                      PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_capitalize) {
    PHYTO_TEST_RUN(string_capitalize_empty);
    PHYTO_TEST_RUN(string_capitalize_allcaps);
    PHYTO_TEST_RUN(string_capitalize_alllower);
    PHYTO_TEST_RUN(string_capitalize_mixed);
    PHYTO_TEST_RUN(string_capitalize_nochange);
}