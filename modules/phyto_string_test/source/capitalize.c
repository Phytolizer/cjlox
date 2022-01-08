#include "phyto/string_test/capitalize.h"

#include "phyto/string/string.h"

static PHYTO_TEST_FUNC(empty) {
    phyto_string_t str = phyto_string_new();
    phyto_string_t caps = phyto_string_capitalize(phyto_string_as_span(str));
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(caps), phyto_string_span_from_c("")),
        do {
            phyto_string_free(&str);
            phyto_string_free(&caps);
        } while (0),
        "capitalize('') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(allcaps) {
    phyto_string_t str = phyto_string_from_c("HELLO WORLD");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_as_span(str));
    phyto_string_span_t expected = phyto_string_span_from_c("Hello world");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(caps), expected),
        do {
            phyto_string_free(&str);
            phyto_string_free(&caps);
        } while (0),
        "capitalize('HELLO WORLD') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(alllower) {
    phyto_string_t str = phyto_string_from_c("hello world");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_as_span(str));
    phyto_string_span_t expected = phyto_string_span_from_c("Hello world");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(caps), expected),
        do {
            phyto_string_free(&str);
            phyto_string_free(&caps);
        } while (0),
        "capitalize('hello world') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(mixed) {
    phyto_string_t str = phyto_string_from_c("hElLo wOrLd");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_as_span(str));
    phyto_string_span_t expected = phyto_string_span_from_c("Hello world");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(caps), expected),
        do {
            phyto_string_free(&str);
            phyto_string_free(&caps);
        } while (0),
        "capitalize('hElLo wOrLd') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(nochange) {
    phyto_string_t str = phyto_string_from_c("Hello world");
    phyto_string_t caps = phyto_string_capitalize(phyto_string_as_span(str));
    phyto_string_span_t expected = phyto_string_span_from_c("Hello world");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(caps), expected),
        do {
            phyto_string_free(&str);
            phyto_string_free(&caps);
        } while (0),
        "capitalize('Hello world') modified the string to '%" PHYTO_STRING_FORMAT "'",
        PHYTO_STRING_PRINTF_ARGS(str));
    phyto_string_free(&str);
    phyto_string_free(&caps);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(capitalize) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(allcaps);
    PHYTO_TEST_RUN(alllower);
    PHYTO_TEST_RUN(mixed);
    PHYTO_TEST_RUN(nochange);
}