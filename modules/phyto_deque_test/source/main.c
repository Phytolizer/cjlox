#include <phyto/collections/deque.h>
#include <phyto/test/test.h>
#include <stdint.h>

PHYTO_COLLECTIONS_DEQUE_DECL(int_deque, int)
PHYTO_COLLECTIONS_DEQUE_IMPL(int_deque, int)

static int32_t intcmp(int a, int b) {
    return (a > b) - (a < b);
}

static void intprn(int a, FILE* f) {
    fprintf(f, "%d", a);
}

static const int_deque_callbacks_t int_deque_callbacks = {
    .compare_cb = intcmp,
    .print_cb = intprn,
};

static PHYTO_TEST_FUNC(push_pop) {
    int_deque_t deque = int_deque_new(10, &int_deque_callbacks);
    int_deque_push_back(&deque, 1);
    int_deque_push_back(&deque, 2);
    int_deque_push_back(&deque, 3);
    int_deque_push_front(&deque, 4);
    int_deque_push_front(&deque, 5);
    int_deque_push_front(&deque, 6);
    PHYTO_TEST_ASSERT(int_deque_count(&deque) == 6, int_deque_free(&deque), "size was wrong");
    PHYTO_TEST_ASSERT(int_deque_front(&deque) == 6, int_deque_free(&deque), "front was wrong");
    PHYTO_TEST_ASSERT(int_deque_back(&deque) == 3, int_deque_free(&deque), "back was wrong");
    int_deque_pop_back(&deque);
    int_deque_pop_front(&deque);
    PHYTO_TEST_ASSERT(int_deque_count(&deque) == 4, int_deque_free(&deque),
                      "size after pop was wrong");
    PHYTO_TEST_ASSERT(int_deque_front(&deque) == 5, int_deque_free(&deque),
                      "front after pop was wrong");
    PHYTO_TEST_ASSERT(int_deque_back(&deque) == 2, int_deque_free(&deque),
                      "back after pop was wrong");
    int_deque_free(&deque);
    PHYTO_TEST_PASS();
}

static PHYTO_TEST_SUITE_FUNC(deque) {
    PHYTO_TEST_RUN(push_pop);
}

int main(void) {
    phyto_test_state_t state = {0};
    PHYTO_TEST_RUN_SUITE(deque, &state);
}
