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

PHYTO_TEST_SUITE_FUNC(hash) {
    PHYTO_TEST_RUN(allocation);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(hash, &state);
    return 0;
}
