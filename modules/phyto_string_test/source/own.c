#include "phyto/string_test/own.h"

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>

PHYTO_TEST_FUNC(string_own_basic) {
    phyto_string_t str = phyto_string_own(phyto_string_view_from_c("Hello, world!"));
    phyto_string_view_t view = phyto_string_view_from_c("Hello, world!");
    PHYTO_TEST_ASSERT(phyto_string_view_equal(phyto_string_view(str), view),
                      phyto_string_free(&str), "Owned string is not equal to the original string");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(string_own_subview) {
    phyto_string_view_t view = phyto_string_view_from_c("Hello, world!");
    phyto_string_t str = phyto_string_own(phyto_string_view_substr(view, 0, 5));
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(str), phyto_string_view_from_c("Hello")),
        phyto_string_free(&str), "Owned string is not equal to the original string");
    phyto_string_free(&str);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string_own) {
    PHYTO_TEST_RUN(string_own_basic);
    PHYTO_TEST_RUN(string_own_subview);
}
