#include <phyto/test/test.h>
#include <phyto/vec/vec.h>

typedef PHYTO_VEC_WRAP(int) vec_int_t;

int compare_ints(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

PHYTO_TEST_FUNC(vec_push) {
    vec_int_t vec = PHYTO_VEC_INIT_DEFAULT(int, compare_ints);
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
    vec_int_t vec = PHYTO_VEC_INIT_DEFAULT(int, compare_ints);
    PHYTO_VEC_PUSH(&vec, 123);
    PHYTO_VEC_PUSH(&vec, 456);
    PHYTO_VEC_PUSH(&vec, 789);
    int value;
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
    vec_int_t vec = PHYTO_VEC_INIT_DEFAULT(int, compare_ints);
    for (int i = 0; i < 1000; ++i) {
        PHYTO_VEC_PUSH(&vec, i);
    }
    PHYTO_VEC_SPLICE(&vec, 0, 10);
    PHYTO_TEST_ASSERT(vec.data[0] == 10, PHYTO_VEC_FREE(&vec), "vec.data[0] == %d, expected 10",
                      vec.data[0]);
    PHYTO_VEC_SPLICE(&vec, 10, 10);
    PHYTO_TEST_ASSERT(vec.data[10] == 30, PHYTO_VEC_FREE(&vec), "vec.data[10] == %d, expected 30",
                      vec.data[10]);
    PHYTO_VEC_SPLICE(&vec, PHYTO_VEC_SIZE(&vec) - 50, 50);
    PHYTO_TEST_ASSERT(vec.data[PHYTO_VEC_SIZE(&vec) - 1] == 949, PHYTO_VEC_FREE(&vec),
                      "vec.data[PHYTO_VEC_SIZE(&vec) - 1] == %d, expected 949",
                      vec.data[PHYTO_VEC_SIZE(&vec) - 1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_FUNC(vec_swap_splice) {
    vec_int_t vec = PHYTO_VEC_INIT_DEFAULT(int, compare_ints);
    for (int i = 0; i < 10; ++i) {
        PHYTO_VEC_PUSH(&vec, i);
    }
    PHYTO_VEC_SWAP_SPLICE(&vec, 0, 3);
    PHYTO_TEST_ASSERT(vec.data[0] == 7 && vec.data[1] == 8 && vec.data[2] == 9,
                      PHYTO_VEC_FREE(&vec),
                      "vec.data[0] == %d, expected 7, vec.data[1] == %d, expected 8, vec.data[2] "
                      "== %d, expected 9",
                      vec.data[0], vec.data[1], vec.data[2]);
    PHYTO_VEC_SWAP_SPLICE(&vec, PHYTO_VEC_SIZE(&vec) - 1, 1);
    PHYTO_TEST_ASSERT(vec.data[PHYTO_VEC_SIZE(&vec) - 1] == 5, PHYTO_VEC_FREE(&vec),
                      "vec.data[PHYTO_VEC_SIZE(&vec) - 1] == %d, expected 5",
                      vec.data[PHYTO_VEC_SIZE(&vec) - 1]);
    PHYTO_VEC_FREE(&vec);
    PHYTO_TEST_PASS();
}

PHYTO_TEST_SUITE_FUNC(vec_tests) {
    PHYTO_TEST_RUN(vec_push);
    PHYTO_TEST_RUN(vec_pop);
    PHYTO_TEST_RUN(vec_splice);
    PHYTO_TEST_RUN(vec_swap_splice);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(vec_tests, &state);
    return state.tests_failed != 0;
}
