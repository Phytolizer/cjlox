#include <inttypes.h>
#include <phyto/test/test.h>
#include <phyto/vec/v2/vec.h>

typedef PHYTO_VEC_TYPE(int) vec_int_t;
typedef PHYTO_VEC_TYPE(double) vec_double_t;

int compare_ints(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int compare_doubles(const void* a, const void* b) {
    if (*(double*)a < *(double*)b) {
        return -1;
    }
    if (*(double*)a > *(double*)b) {
        return 1;
    }
    return 0;
}

PHYTO_TEST_FUNC(vec_push) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_PUSH(&vec, i * 2);
    }
    PHYTO_TEST_ASSERT(vec.data[1] == 2, PHYTO_VEC_FREE(&vec), "vec.data[1] == %d, expected 2",
                      vec.data[1]);
    PHYTO_TEST_ASSERT(vec.data[999] == 1998, PHYTO_VEC_FREE(&vec),
                      "vec.data[999] == %d, expected 1998", vec.data[999]);
    PHYTO_TEST_ASSERT(PHYTO_VEC_PUSH(&vec, 10), PHYTO_VEC_FREE(&vec),
                      "PHYTO_VEC_PUSH(&vec, 10) reported failure");
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_pop) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    PHYTO_VEC_PUSH(&vec, 123);
    PHYTO_VEC_PUSH(&vec, 456);
    PHYTO_VEC_PUSH(&vec, 789);
    int value = 0;
    PHYTO_VEC_POP(&vec, &value);
    PHYTO_TEST_ASSERT(value == 789, PHYTO_VEC_FREE(&vec), "value == %d, expected 789", value);
    PHYTO_VEC_POP(&vec, &value);
    PHYTO_TEST_ASSERT(value == 456, PHYTO_VEC_FREE(&vec), "value == %d, expected 456", value);
    PHYTO_VEC_POP(&vec, &value);
    PHYTO_TEST_ASSERT(value == 123, PHYTO_VEC_FREE(&vec), "value == %d, expected 123", value);
    PHYTO_TEST_ASSERT(!PHYTO_VEC_POP(&vec, &value), PHYTO_VEC_FREE(&vec),
                      "PHYTO_VEC_POP(&vec, &value) reported success");
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_splice) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_PUSH(&vec, i);
    }
    PHYTO_VEC_SPLICE(&vec, 0, 10);
    PHYTO_TEST_ASSERT(vec.data[0] == 10, PHYTO_VEC_FREE(&vec), "vec.data[0] == %d, expected 10",
                      vec.data[0]);
    PHYTO_VEC_SPLICE(&vec, 10, 10);
    PHYTO_TEST_ASSERT(vec.data[10] == 30, PHYTO_VEC_FREE(&vec), "vec.data[10] == %d, expected 30",
                      vec.data[10]);
    PHYTO_VEC_SPLICE(&vec, vec.size - 50, 50);
    PHYTO_TEST_ASSERT(vec.data[vec.size - 1] == 949, PHYTO_VEC_FREE(&vec),
                      "vec.data[PHYTO_VEC_SIZE(&vec) - 1] == %d, expected 949",
                      vec.data[vec.size - 1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_swap_splice) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 10; ++i) {
        PHYTO_VEC_PUSH(&vec, i);
    }
    PHYTO_VEC_SWAPSPLICE(&vec, 0, 3);
    PHYTO_TEST_ASSERT(vec.data[0] == 7 && vec.data[1] == 8 && vec.data[2] == 9,
                      PHYTO_VEC_FREE(&vec),
                      "vec.data[0] == %d, expected 7, vec.data[1] == %d, expected 8, vec.data[2] "
                      "== %d, expected 9",
                      vec.data[0], vec.data[1], vec.data[2]);
    PHYTO_VEC_SWAPSPLICE(&vec, vec.size - 1, 1);
    PHYTO_TEST_ASSERT(vec.data[vec.size - 1] == 5, PHYTO_VEC_FREE(&vec),
                      "vec.data[vec.size - 1] == %d, expected 5", vec.data[vec.size - 1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_insert) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_INSERT(&vec, 0, i);
    }
    PHYTO_TEST_ASSERT(vec.data[0] == 999, PHYTO_VEC_FREE(&vec), "vec.data[0] == %d, expected 999",
                      vec.data[0]);
    PHYTO_TEST_ASSERT(vec.data[vec.size - 1] == 0, PHYTO_VEC_FREE(&vec),
                      "vec.data[vec.size - 1] == %d, expected 0", vec.data[vec.size - 1]);
    PHYTO_VEC_INSERT(&vec, 10, 123);
    PHYTO_TEST_ASSERT(vec.data[10] == 123, PHYTO_VEC_FREE(&vec), "vec.data[10] == %d, expected 123",
                      vec.data[10]);
    PHYTO_TEST_ASSERT(vec.size == 1001, PHYTO_VEC_FREE(&vec), "vec.size == %d, expected 1001",
                      vec.size);
    PHYTO_VEC_INSERT(&vec, vec.size - 2, 678);
    PHYTO_TEST_ASSERT(vec.data[999] == 678, PHYTO_VEC_FREE(&vec),
                      "vec.data[999] == %d, expected 678", vec.data[999]);
    PHYTO_TEST_ASSERT(PHYTO_VEC_INSERT(&vec, 10, 123), PHYTO_VEC_FREE(&vec),
                      "PHYTO_VEC_INSERT(&vec, 10, 123) reported failure");
    PHYTO_VEC_INSERT(&vec, vec.size, 789);
    PHYTO_TEST_ASSERT(vec.data[vec.size - 1] == 789, PHYTO_VEC_FREE(&vec),
                      "vec.data[vec.size - 1] == %d, expected 789", vec.data[vec.size - 1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_sort) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    PHYTO_VEC_PUSH(&vec, 3);
    PHYTO_VEC_PUSH(&vec, -1);
    PHYTO_VEC_PUSH(&vec, 0);
    PHYTO_VEC_SORT(&vec, compare_ints);
    PHYTO_TEST_ASSERT(vec.data[0] == -1 && vec.data[1] == 0 && vec.data[2] == 3,
                      PHYTO_VEC_FREE(&vec),
                      "vec.data[0] == %d, expected -1, vec.data[1] == %d, expected 0, vec.data[2] "
                      "== %d, expected 3",
                      vec.data[0], vec.data[1], vec.data[2]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_swap) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    PHYTO_VEC_PUSH(&vec, 'a');
    PHYTO_VEC_PUSH(&vec, 'b');
    PHYTO_VEC_PUSH(&vec, 'c');
    PHYTO_VEC_SWAP(&vec, 0, 2);
    PHYTO_TEST_ASSERT(vec.data[0] == 'c' && vec.data[2] == 'a', PHYTO_VEC_FREE(&vec),
                      "vec.data[0] == %c, expected 'c', vec.data[2] == %c, expected 'a'",
                      vec.data[0], vec.data[2]);
    PHYTO_VEC_SWAP(&vec, 0, 1);
    PHYTO_TEST_ASSERT(vec.data[0] == 'b' && vec.data[1] == 'c', PHYTO_VEC_FREE(&vec),
                      "vec.data[0] == %c, expected 'b', vec.data[1] == %c, expected 'c'",
                      vec.data[0], vec.data[1]);
    PHYTO_VEC_SWAP(&vec, 1, 2);
    PHYTO_TEST_ASSERT(vec.data[1] == 'a' && vec.data[2] == 'c', PHYTO_VEC_FREE(&vec),
                      "vec.data[1] == %c, expected 'a', vec.data[2] == %c, expected 'c'",
                      vec.data[1], vec.data[2]);
    PHYTO_VEC_SWAP(&vec, 1, 1);
    PHYTO_TEST_ASSERT(vec.data[1] == 'a', PHYTO_VEC_FREE(&vec), "vec.data[1] == %c, expected 'a'",
                      vec.data[1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_truncate) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_PUSH(&vec, 0);
    }
    PHYTO_VEC_TRUNCATE(&vec, 10000);
    PHYTO_TEST_ASSERT(vec.size == 1000, PHYTO_VEC_FREE(&vec), "vec.size == %d, expected 1000",
                      vec.size);
    PHYTO_VEC_TRUNCATE(&vec, 900);
    PHYTO_TEST_ASSERT(vec.size == 900, PHYTO_VEC_FREE(&vec), "vec.size == %d, expected 900",
                      vec.size);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_clear) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    PHYTO_VEC_PUSH(&vec, 1);
    PHYTO_VEC_PUSH(&vec, 2);
    PHYTO_VEC_CLEAR(&vec);
    PHYTO_TEST_ASSERT(vec.size == 0, PHYTO_VEC_FREE(&vec), "vec.size == %zu, expected 0", vec.size);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_compact) {
    vec_int_t vec;
    PHYTO_VEC_INIT(&vec);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_PUSH(&vec, 0);
    }
    PHYTO_VEC_TRUNCATE(&vec, 3);
    PHYTO_VEC_COMPACT(&vec);
    PHYTO_TEST_ASSERT(vec.size == vec.capacity, PHYTO_VEC_FREE(&vec),
                      "vec.size == %zu, expected %zu", vec.size, vec.capacity);
    PHYTO_TEST_ASSERT(PHYTO_VEC_COMPACT(&vec), PHYTO_VEC_FREE(&vec),
                      "PHYTO_VEC_COMPACT(&vec) reported failure");
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_push_array) {
    int arr[] = {5, 6, 7, 8, 9};
    vec_double_t vec;
    PHYTO_VEC_INIT(&vec);
    PHYTO_VEC_PUSH(&vec, 1);
    PHYTO_VEC_PUSH(&vec, 2);
    PHYTO_VEC_PUSH_ARRAY(&vec, arr, 5);
    PHYTO_TEST_ASSERT(vec.data[0] == 1, PHYTO_VEC_FREE(&vec), "vec.data[0] == %f, expected 1",
                      vec.data[0]);
    PHYTO_TEST_ASSERT(vec.data[2] == 5, PHYTO_VEC_FREE(&vec), "vec.data[2] == %f, expected 5",
                      vec.data[2]);
    PHYTO_TEST_ASSERT(vec.data[6] == 9, PHYTO_VEC_FREE(&vec), "vec.data[6] == %f, expected 9",
                      vec.data[6]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_VEC_PUSH_ARRAY(&vec, arr, 5);
    PHYTO_TEST_ASSERT(vec.data[0] == 5, PHYTO_VEC_FREE(&vec), "vec.data[0] == %f, expected 5",
                      vec.data[0]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_push_vec) {
    vec_int_t v;
    PHYTO_VEC_INIT(&v);
    vec_int_t v2;
    PHYTO_VEC_INIT(&v2);
    PHYTO_VEC_PUSH(&v, 12);
    PHYTO_VEC_PUSH(&v, 34);
    PHYTO_VEC_PUSH(&v2, 56);
    PHYTO_VEC_PUSH(&v2, 78);
    PHYTO_VEC_EXTEND(&v, &v2);
    PHYTO_TEST_ASSERT(v.size == 4, PHYTO_VEC_FREE(&v), "v.size == %zu, expected 4", v.size);
    PHYTO_TEST_ASSERT(v.data[0] == 12 && v.data[1] == 34 && v.data[2] == 56 && v.data[3] == 78,
                      PHYTO_VEC_FREE(&v),
                      "v.data[0] == %d, expected 12, v.data[1] == %d, expected 34, "
                      "v.data[2] == %d, expected 56, v.data[3] == %d, expected 78",
                      v.data[0], v.data[1], v.data[2], v.data[3]);
    PHYTO_VEC_FREE(&v);
    PHYTO_VEC_FREE(&v2);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_find) {
    vec_int_t v;
    PHYTO_VEC_INIT(&v);
    for (int i = 0; i < 26; ++i) {
        PHYTO_VEC_PUSH(&v, 'a' + i);
    }
    size_t i;
    PHYTO_VEC_FIND(&v, 'a', &i);
    PHYTO_TEST_ASSERT(i == 0, PHYTO_VEC_FREE(&v), "i == %zu, expected 0", i);
    PHYTO_VEC_FIND(&v, 'z', &i);
    PHYTO_TEST_ASSERT(i == 25, PHYTO_VEC_FREE(&v), "i == %zu, expected 25", i);
    PHYTO_VEC_FIND(&v, 'd', &i);
    PHYTO_TEST_ASSERT(i == 3, PHYTO_VEC_FREE(&v), "i == %zu, expected 3", i);
    PHYTO_TEST_ASSERT(!PHYTO_VEC_FIND(&v, '_', &i), PHYTO_VEC_FREE(&v),
                      "PHYTO_VEC_FIND(&v, '_', &i) reported success");
    PHYTO_VEC_FREE(&v);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(vec_tests) {
    PHYTO_TEST_RUN(vec_push);
    PHYTO_TEST_RUN(vec_pop);
    PHYTO_TEST_RUN(vec_splice);
    PHYTO_TEST_RUN(vec_swap_splice);
    PHYTO_TEST_RUN(vec_insert);
    PHYTO_TEST_RUN(vec_sort);
    PHYTO_TEST_RUN(vec_swap);
    PHYTO_TEST_RUN(vec_truncate);
    PHYTO_TEST_RUN(vec_clear);
    PHYTO_TEST_RUN(vec_compact);
    PHYTO_TEST_RUN(vec_push_array);
    PHYTO_TEST_RUN(vec_push_vec);
    PHYTO_TEST_RUN(vec_find);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(vec_tests, &state);
    printf("%" PRIu64 " tests, %" PRIu64 " assertions, %" PRIu64 " failures\n",
           state.tests_passed + state.tests_failed, state.assert_count, state.tests_failed);
    return state.tests_failed != 0;
}
