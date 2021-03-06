#include <ansi_esc/ansi_esc.h>
#include <inttypes.h>
#include <phyto/hash/hash.h>
#include <phyto/string/string.h>
#include <phyto/test/test.h>
#include <stdint.h>

#include "phyto_hash_test/pcre.h"

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
    return phyto_hash_fnv1a(phyto_string_span_from_array((char*)&a, sizeof(a)));
}

static void intprint(FILE* fp, int a) {
    fprintf(fp, "%d", a);
}

static const int_map_key_ops_t default_key_ops = {
    .hash = phyto_hash_fnv1a,
};

static const int_map_key_ops_t djb2_key_ops = {
    .hash = phyto_hash_djb2,
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
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_reallocate) {
    int_map_t* map = int_map_new(1, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_contains(map, phyto_string_span_from_c("hello")), int_map_free(map),
                      "reallocation deleted key");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(duplicate_key) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    int_map_insert(map, phyto_string_span_from_c("hello"), 42);
    PHYTO_TEST_ASSERT(!int_map_insert(map, phyto_string_span_from_c("hello"), 43),
                      int_map_free(map), "int_map_insert() succeeded");
    PHYTO_TEST_RUN_SUBTEST(assert_error, int_map_free(map), map, phyto_hash_flag_duplicate);
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(not_found) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_span_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(!int_map_update(map, phyto_string_span_from_c("goodbye"), 43, NULL),
                      int_map_free(map), "int_map_update() succeeded");
    PHYTO_TEST_RUN_SUBTEST(assert_error, int_map_free(map), map, phyto_hash_flag_not_found);
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_update) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_update(map, phyto_string_span_from_c("hello"), 43, NULL),
                      int_map_free(map), "int_map_update() failed");
    PHYTO_TEST_ASSERT(int_map_contains(map, phyto_string_span_from_c("hello")), int_map_free(map),
                      "int_map_contains() failed");
    PHYTO_TEST_ASSERT(int_map_get(map, phyto_string_span_from_c("hello")) == 43, int_map_free(map),
                      "int_map_get() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(insert_remove) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_remove(map, phyto_string_span_from_c("hello"), NULL),
                      int_map_free(map), "int_map_remove() failed");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_span_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    int value;
    PHYTO_TEST_ASSERT(int_map_remove(map, phyto_string_span_from_c("hello"), &value),
                      int_map_free(map), "int_map_remove() failed");
    PHYTO_TEST_ASSERT(value == 42, int_map_free(map),
                      "int_map_remove() didn't return the correct value");
    PHYTO_TEST_ASSERT(!int_map_contains(map, phyto_string_span_from_c("hello")), int_map_free(map),
                      "int_map_contains() succeeded");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(max) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &djb2_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("foo"), 44), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("bar"), 45), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("baz"), 46), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("qux"), 47), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("quux"), 48), int_map_free(map),
                      "int_map_insert() failed");
    phyto_string_t k;
    int v;
    PHYTO_TEST_ASSERT(int_map_max(map, &k, &v), int_map_free(map), "int_map_max() failed");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(k), phyto_string_span_from_c("world")),
        int_map_free(map), "int_map_max() didn't return the correct key");
    PHYTO_TEST_ASSERT(v == 43, int_map_free(map), "int_map_max() didn't return the correct value");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(min) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("world"), 43), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("foo"), 44), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("bar"), 45), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("baz"), 46), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("qux"), 47), int_map_free(map),
                      "int_map_insert() failed");
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("quux"), 48), int_map_free(map),
                      "int_map_insert() failed");
    phyto_string_t k;
    int v;
    PHYTO_TEST_ASSERT(int_map_min(map, &k, &v), int_map_free(map), "int_map_min() failed");
    PHYTO_TEST_ASSERT(
        phyto_string_span_equal(phyto_string_as_span(k), phyto_string_span_from_c("bar")),
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
    PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_c("hello"), 42), int_map_free(map),
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
        phyto_string_span_equal(int_map_iter_key(&iter), phyto_string_span_from_c("hello")),
        int_map_free(map), "int_map_iter_key() wasn't 'hello'");
    PHYTO_TEST_ASSERT(int_map_iter_value(&iter) == 42, int_map_free(map),
                      "int_map_iter_value() wasn't 42");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUBTEST_FUNC(insert_alphabet, int_map_t* map) {
    for (int x = 'a'; x <= 'z'; ++x) {
        char cx = (char)x;
        PHYTO_TEST_ASSERT(int_map_insert(map, phyto_string_span_from_array(&cx, 1), x + 42),
                          int_map_free(map), "int_map_insert() failed");
    }
    PHYTO_TEST_SUBTEST_PASS();
}

PHYTO_TEST_FUNC(many) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_RUN_SUBTEST(insert_alphabet, int_map_free(map), map);
    int_map_iter_t iter = int_map_iter_start(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    for (size_t i = 0; i < 26; ++i) {
        phyto_string_span_t k = int_map_iter_key(&iter);
        int v = int_map_iter_value(&iter);
        PHYTO_TEST_ASSERT(k.size == 1, int_map_free(map), "int_map_iter_key() wasn't 1 byte");
        PHYTO_TEST_ASSERT(k.begin[0] >= 'a' && k.begin[0] <= 'z', int_map_free(map),
                          "int_map_iter_key() wasn't a letter");
        PHYTO_TEST_ASSERT(v == (int)(k.begin[0]) + 42, int_map_free(map),
                          "int_map_iter_value() wasn't the correct value");
        int_map_iter_next(&iter);
    }
    PHYTO_TEST_ASSERT(int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() failed");
    PHYTO_TEST_ASSERT(!int_map_iter_next(&iter), int_map_free(map),
                      "int_map_iter_next() succeeded");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(reverse) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &djb2_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_RUN_SUBTEST(insert_alphabet, int_map_free(map), map);
    int_map_iter_t iter = int_map_iter_end(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() failed");
    for (size_t i = 0; i < 26; ++i) {
        phyto_string_span_t k = int_map_iter_key(&iter);
        int v = int_map_iter_value(&iter);
        PHYTO_TEST_ASSERT(k.size == 1, int_map_free(map), "int_map_iter_key() wasn't 1 byte");
        PHYTO_TEST_ASSERT(k.begin[0] >= 'a' && k.begin[0] <= 'z', int_map_free(map),
                          "int_map_iter_key() wasn't a letter");
        PHYTO_TEST_ASSERT(v == (int)(k.begin[0]) + 42, int_map_free(map),
                          "int_map_iter_value() wasn't the correct value");
        int_map_iter_prev(&iter);
    }
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(steps) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &djb2_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_RUN_SUBTEST(insert_alphabet, int_map_free(map), map);
    int_map_iter_t iter = int_map_iter_start(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    for (size_t i = 0; !int_map_iter_at_end(&iter); ++i) {
        phyto_string_span_t k = int_map_iter_key(&iter);
        int v = int_map_iter_value(&iter);
        PHYTO_TEST_ASSERT(k.size == 1, int_map_free(map), "int_map_iter_key() wasn't 1 byte");
        PHYTO_TEST_ASSERT(k.begin[0] >= 'a' && k.begin[0] <= 'z', int_map_free(map),
                          "int_map_iter_key() wasn't a letter");
        PHYTO_TEST_ASSERT(v == (int)(k.begin[0]) + 42, int_map_free(map),
                          "int_map_iter_value() wasn't the correct value");
        if (!int_map_iter_advance(&iter, 2)) {
            int_map_iter_next(&iter);
        }
    }
    PHYTO_TEST_ASSERT(int_map_iter_at_end(&iter), int_map_free(map),
                      "int_map_iter_at_end() failed");
    PHYTO_TEST_ASSERT(!int_map_iter_next(&iter), int_map_free(map),
                      "int_map_iter_next() succeeded");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(go_to) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &djb2_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_RUN_SUBTEST(insert_alphabet, int_map_free(map), map);
    int_map_iter_t iter = int_map_iter_start(map);
    PHYTO_TEST_ASSERT(int_map_iter_at_start(&iter), int_map_free(map),
                      "int_map_iter_at_start() failed");
    for (size_t i = 0; i < 26; ++i) {
        phyto_string_span_t k = int_map_iter_key(&iter);
        int v = int_map_iter_value(&iter);
        PHYTO_TEST_ASSERT(k.size == 1, int_map_free(map), "int_map_iter_key() wasn't 1 byte");
        PHYTO_TEST_ASSERT(k.begin[0] >= 'a' && k.begin[0] <= 'z', int_map_free(map),
                          "int_map_iter_key() wasn't a letter");
        PHYTO_TEST_ASSERT(v == (int)(k.begin[0]) + 42, int_map_free(map),
                          "int_map_iter_value() wasn't the correct value");
        int_map_iter_go_to(&iter, i);
    }
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(iteration) {
    PHYTO_TEST_RUN(empty);
    PHYTO_TEST_RUN(one);
    PHYTO_TEST_RUN(many);
    PHYTO_TEST_RUN(reverse);
    PHYTO_TEST_RUN(steps);
    PHYTO_TEST_RUN(go_to);
}

static void string_cb(const char* str, size_t len, void* data) {
    phyto_string_t* s = (phyto_string_t*)data;
    phyto_string_extend(s, phyto_string_span_from_array(str, len));
}

PHYTO_TEST_FUNC(to_string) {
    int_map_t* map = int_map_new(10, phyto_hash_default_load, &default_key_ops, &default_value_ops);
    PHYTO_TEST_ASSERT(map != NULL, (void)0, "int_map_new() failed");
    PHYTO_TEST_RUN_SUBTEST(insert_alphabet, int_map_free(map), map);

    phyto_string_t result = phyto_string_new();
    int_map_string(map, string_cb, &result);
    int error_code;
    size_t error_offset;
    pcre2_code* regex_code = pcre2_compile(
        (PCRE2_SPTR) "int_map_t<phyto_string_t, int> at 0x[0-9a-fA-F]+ { buffer:0x[0-9a-fA-F]+, capacity:53, count:26, load:0\\.[0-9]+, flag:ok, key_ops:0x[0-9a-fA-F]+, value_ops:0x[0-9a-fA-F]+ }",
        PCRE2_ZERO_TERMINATED, 0, &error_code, &error_offset, 0);
    PHYTO_TEST_ASSERT(
        regex_code != NULL,
        do {
            int_map_free(map);
            phyto_string_free(&result);
        } while (false),
        "pcre2_compile() failed");
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex_code, NULL);
    int ret = pcre2_match(regex_code, (PCRE2_SPTR)result.data, result.size, 0, 0, match_data, NULL);
    PHYTO_TEST_ASSERT(
        ret >= 0,
        do {
            int_map_free(map);
            phyto_string_free(&result);
            pcre2_match_data_free(match_data);
            pcre2_code_free(regex_code);
        } while (false),
        "int_map_string() did not match the expected output");
    pcre2_match_data_free(match_data);
    pcre2_code_free(regex_code);
    phyto_string_print_nosep(result, stdout);
    printf("\n");
    phyto_string_free(&result);
    printf("\n");
    int_map_print(map, stdout, phyto_string_span_from_c("==int_map_print==\n"),
                  phyto_string_span_from_c("\n"),
                  phyto_string_span_from_c("\n==end int_map_print=="),
                  phyto_string_span_from_c(":"));
    printf("\n");
    int_map_free(map);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(string) {
    PHYTO_TEST_RUN(to_string);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(basics, &state);
    PHYTO_TEST_RUN_SUITE(iteration, &state);
    PHYTO_TEST_RUN_SUITE(string, &state);
    printf(ANSI_ESC_FG_YELLOW "%" PRIu64 ANSI_ESC_RESET " tests, " ANSI_ESC_FG_GREEN
                              "%" PRIu64 ANSI_ESC_RESET " passes, " ANSI_ESC_FG_RED
                              "%" PRIu64 ANSI_ESC_RESET " failures, " ANSI_ESC_FG_BLUE
                              "%" PRIu64 ANSI_ESC_RESET " assertions\n",
           state.tests_passed + state.tests_failed, state.tests_passed, state.tests_failed,
           state.assert_count);
    return (int)state.tests_failed;
}
