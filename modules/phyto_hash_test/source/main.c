#include <ansi_esc/ansi_esc.h>
#include <inttypes.h>
#include <phyto/hash/hash.h>
#include <phyto/string_view/string_view.h>
#include <phyto/test/test.h>

PHYTO_HASH_DECL(test, int);
PHYTO_HASH_IMPL(test, int);

static uint64_t sview_fnv1a(phyto_string_view_t s) {
    uint64_t hash = 0xcbf29ce484222325;
    for (size_t i = 0; i < s.size; i++) {
        hash ^= s.begin[i];
        hash *= 0x100000001b3;
    }
    return hash;
}

static int32_t intcmp(int a, int b) {
    return a - b;
}

static int intcpy(int a) {
    return a;
}

static void intfree(int a) {
    (void)a;
}

static uint64_t int_fnv1a(int a) {
    return sview_fnv1a(phyto_string_view_from_ptr_length((char*)&a, sizeof(a)));
}

static bool intprint(FILE* fp, int a) {
    fprintf(fp, "%d", a);
    return true;
}

static const test_key_ops_t default_key_ops = {
    .hash = sview_fnv1a,
};

static const test_value_ops_t default_value_ops = {
    .compare = intcmp,
    .copy = intcpy,
    .free = intfree,
    .hash = int_fnv1a,
    .print = intprint,
};

PHYTO_TEST_FUNC(allocation) {
    test_t* test = test_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(test != NULL, (void)0, "test_new() failed");
    test_free(test);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_once) {
    test_t* test = test_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(test != NULL, (void)0, "test_new() failed");
    PHYTO_TEST_ASSERT(test_insert(test, phyto_string_view_from_c("hello"), 42), test_free(test),
                      "test_insert() failed");
    test_free(test);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(basics) {
    PHYTO_TEST_RUN(allocation);
    PHYTO_TEST_RUN(insert_once);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(basics, &state);
    printf(ANSI_ESC_FG_YELLOW "%" PRIu64 ANSI_ESC_RESET " tests, " ANSI_ESC_FG_GREEN
                              "%" PRIu64 ANSI_ESC_RESET " passes, " ANSI_ESC_FG_RED
                              "%" PRIu64 ANSI_ESC_RESET " failures, " ANSI_ESC_FG_BLUE
                              "%" PRIu64 ANSI_ESC_RESET " assertions\n",
           state.tests_passed + state.tests_failed, state.tests_passed, state.tests_failed,
           state.assert_count);
    return (int)state.tests_failed;
}
