#include "phyto/string_test/own.h"

#include <phyto/string/string.h>

static PHYTO_TEST_FUNC(basic) {
    phyto_string_t str = phyto_string_own(phyto_string_span_from_c("Hello, world!"));
    phyto_string_span_t span = phyto_string_span_from_c("Hello, world!");
    PHYTO_TEST_ASSERT(phyto_string_span_equal(phyto_string_as_span(str), span),
                      phyto_string_free(&str), "Owned string is not equal to the original string");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_FUNC(subspan) {
    phyto_string_span_t span = phyto_string_span_from_c("Hello, world!");
    phyto_string_t str = phyto_string_own(phyto_string_span_subspan(span, 0, 5));
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(str), phyto_string_span_from_c("Hello")),
        phyto_string_free(&str), "Owned string is not equal to the original string");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(own) {
    PHYTO_TEST_RUN(basic);
    PHYTO_TEST_RUN(subspan);
}
