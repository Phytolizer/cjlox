#include <ansi_esc/ansi_esc.h>
#include <inttypes.h>
#include <phyto/hash/hash.h>
#include <phyto/string_view/string_view.h>
#include <phyto/test/test.h>

PHYTO_HASH_DECL(int_map, int);
PHYTO_HASH_IMPL(int_map, int);
PHYTO_HASH_DECL_ITER(int_map, int);
PHYTO_HASH_IMPL_ITER(int_map, int);
PHYTO_HASH_DECL_STR(int_map, int);
PHYTO_HASH_IMPL_STR(int_map, int);

static PHYTO_TEST_SUBTEST_FUNC(assert_error, int_map_t* map, phyto_hash_flag_t expected) {
    phyto_hash_flag_t actual = int_map_flag(map);
    PHYTO_TEST_ASSERT(expected == actual, (void)0,
                      "expected: %" PHYTO_STRING_FORMAT ", actual: %" PHYTO_STRING_FORMAT,
                      PHYTO_STRING_VIEW_PRINTF_ARGS(phyto_hash_flag_explain(expected)),
                      PHYTO_STRING_VIEW_PRINTF_ARGS(phyto_hash_flag_explain(actual)));
    PHYTO_TEST_SUBTEST_PASS();
}

static uint64_t sview_fnv1a(phyto_string_view_t s) {
    uint64_t hash = 0xcbf29ce484222325;
    for (size_t i = 0; i < s.size; i++) {
        hash ^= (uint8_t)s.begin[i];
        hash *= 0x100000001b3;
    }
    return hash;
}

static uint64_t sview_djb2(phyto_string_view_t s) {
    uint64_t hash = 5381;
    for (size_t i = 0; i < s.size; i++) {
        hash = ((hash << 5) + hash) + (uint8_t)s.begin[i];
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

static const int_map_key_ops_t default_key_ops = {
    .hash = sview_fnv1a,
};

static const int_map_key_ops_t djb2_key_ops = {
    .hash = sview_djb2,
};

static const int_map_value_ops_t default_value_ops = {
    .compare = intcmp,
    .copy = intcpy,
    .free = intfree,
    .hash = int_fnv1a,
    .print = intprint,
};

PHYTO_TEST_FUNC(allocation) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_once) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_reallocate) {
    int_map_t* map = int_map_new(1, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_contains(map, phyto_string_view_from_c("hello")), int_map_free(map),
                      "reallocation deleted key");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(duplicate_key) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    int_map_insert(map, phyto_string_view_from_c("hello"), 42);
    PHYTO_TEST_ASSERT(!int_map_insert(map, phyto_string_view_from_c("hello"), 43),
                      int_map_free(map), "int_map_insert() succeeded");
    PHYTO_TEST_RUN_SUBTEST(assert_error, int_map_free(map), map, phyto_hash_flag_duplicate);
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(not_found) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_view_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(!int_map_update(map, phyto_string_view_from_c("goodbye"), 43, NULL),
                      int_map_free(map), "int_map_update() succeeded");
    PHYTO_TEST_RUN_SUBTEST(assert_error, int_map_free(map), map, phyto_hash_flag_not_found);
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_update) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_update(map, phyto_string_view_from_c("hello"), 43, NULL),
                      int_map_free(map), "int_map_update() failed");
    PHYTO_TEST_ASSERT(int_map_contains(map, phyto_string_view_from_c("hello")), int_map_free(map),
                      "int_map_contains() failed");
    PHYTO_TEST_ASSERT(int_map_get(map, phyto_string_view_from_c("hello")) == 43, int_map_free(map),
                      "int_map_get() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_remove) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_remove(map, phyto_string_view_from_c("hello"), NULL),
                      int_map_free(map), "int_map_remove() failed");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_view_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    int value;
    PHYTO_TEST_ASSERT(int_map_remove(map, phyto_string_view_from_c("hello"), &value),
                      int_map_free(map), "int_map_remove() failed");
    PHYTO_TEST_ASSERT(value == 42, int_map_free(map),
                      "int_map_remove() didn't return the correct value");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_view_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(max) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &djb2_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("foo"), 44), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("bar"), 45), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("baz"), 46), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("qux"), 47), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("quux"), 48), int_map_free(map),
                      "int_map_insert() failed");
    phyto_string_t k;
    int v;
    PHYTO_TEST_ASSERT(int_map_max(map, &k, &v), int_map_free(map), "int_map_max() failed");
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(k), phyto_string_view_from_c("world")),
        int_map_free(map), "int_map_max() didn't return the correct key");
    PHYTO_TEST_ASSERT(v == 43, int_map_free(map), "int_map_max() didn't return the correct value");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(min) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("foo"), 44), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("bar"), 45), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("baz"), 46), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("qux"), 47), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("quux"), 48), int_map_free(map),
                      "int_map_insert() failed");
    phyto_string_t k;
    int v;
    PHYTO_TEST_ASSERT(int_map_min(map, &k, &v), int_map_free(map), "int_map_min() failed");
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(phyto_string_view(k), phyto_string_view_from_c("bar")),
        int_map_free(map), "int_map_min() didn't return the correct key");
    PHYTO_TEST_ASSERT(v == 45, int_map_free(map), "int_map_min() didn't return the correct value");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(basics) {
    PHYTO_TEST_RUN(allocation);
    PHYTO_TEST_RUN(insert_once);
    PHYTO_TEST_RUN(insert_reallocate);
    PHYTO_TEST_RUN(duplicate_key);
    PHYTO_TEST_RUN(not_found);
    PHYTO_TEST_RUN(insert_update);
    PHYTO_TEST_RUN(insert_remove);
    PHYTO_TEST_RUN(max);
    PHYTO_TEST_RUN(min);
}

PHYTO_TEST_FUNC(empty) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_empty(map), int_map_free(map), "int_map_empty() failed");
    int_map_iter_t iter = int_map_iter_start(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    PHYTO_TEST_ASSERT(int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() failed");
    PHYTO_TEST_ASSERT(!int_map_iter_next(&iter), int_map_free(map),
                      "int_map_iter_next() succeeded");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(one) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_view_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(!int_map_empty(map), int_map_free(map), "int_map_empty() succeeded");
    int_map_iter_t iter = int_map_iter_start(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    PHYTO_TEST_ASSERT(!int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() succeeded");
    PHYTO_TEST_ASSERT(!int_map_iter_next(&iter), int_map_free(map),
                      "int_map_iter_next() succeeded");
    PHYTO_TEST_ASSERT(int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() failed");
    PHYTO_TEST_ASSERT(
        phyto_string_view_equal(int_map_iter_key(&iter), phyto_string_view_from_c("hello")),
        int_map_free(map), "int_map_iter_key() wasn't 'hello'");
    PHYTO_TEST_ASSERT(int_map_iter_value(&iter) == 42, int_map_free(map),
                      "int_map_iter_value() wasn't 42");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(iteration) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(one);
    // PHYTO_TEST_RUN(many);
    // PHYTO_TEST_RUN(reverse);
    // PHYTO_TEST_RUN(steps);
    // PHYTO_TEST_RUN(go_to);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(basics, &state);
    PHYTO_TEST_RUN_SUITE(iteration, &state);
    printf(ANSI_ESC_FG_YELLOW "%" PRIu64 ANSI_ESC_RESET " tests, " ANSI_ESC_FG_GREEN
                              "%" PRIu64 ANSI_ESC_RESET " passes, " ANSI_ESC_FG_RED
                              "%" PRIu64 ANSI_ESC_RESET " failures, " ANSI_ESC_FG_BLUE
                              "%" PRIu64 ANSI_ESC_RESET " assertions\n",
           state.tests_passed + state.tests_failed, state.tests_passed, state.tests_failed,
           state.assert_count);
    return (int)state.tests_failed;
}
