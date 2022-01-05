#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rxi/vec/vec.h"

typedef vec_t(int) vec_int_t;
typedef vec_t(double) vec_double_t;

#define TEST_SECTION(desc)        \
    do {                          \
        printf("--- %s\n", desc); \
    } while (0)

#define TEST_ASSERT(cond)                                                     \
    do {                                                                      \
        int pass__ = cond;                                                    \
        printf("[%s] %s:%d: ", pass__ ? "PASS" : "FAIL", __FILE__, __LINE__); \
        printf((strlen(#cond) > 50 ? "%.47s...\n" : "%s\n"), #cond);          \
        if (pass__) {                                                         \
            pass_count++;                                                     \
        } else {                                                              \
            fail_count++;                                                     \
        }                                                                     \
    } while (0)

#define TEST_PRINT_RES()                                                          \
    do {                                                                          \
        printf("------------------------------------------------------------\n"); \
        printf("-- Results:   %3d Total    %3d Passed    %3d Failed       --\n",  \
               pass_count + fail_count, pass_count, fail_count);                  \
        printf("------------------------------------------------------------\n"); \
    } while (0)

int intptrcmp(const void* a_in, const void* b_in) {
    const int* a = a_in;
    const int* b = b_in;
    return *a < *b ? -1 : *a > *b;
}

int main(void) {
    int pass_count = 0;
    int fail_count = 0;
    {
        TEST_SECTION("vec_push");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 1000; i++) {
            vec_push(&v, i * 2);
        }
        TEST_ASSERT(v.data[1] == 2);
        TEST_ASSERT(v.data[999] == 999 * 2);
        TEST_ASSERT(vec_push(&v, 10) == 0);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_pop");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 123);
        vec_push(&v, 456);
        vec_push(&v, 789);
        TEST_ASSERT(vec_pop(&v) == 789);
        TEST_ASSERT(vec_pop(&v) == 456);
        TEST_ASSERT(vec_pop(&v) == 123);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_splice");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 1000; i++) {
            vec_push(&v, i);
        }
        vec_splice(&v, 0, 10);
        TEST_ASSERT(v.data[0] == 10);
        vec_splice(&v, 10, 10);
        TEST_ASSERT(v.data[10] == 30);
        vec_splice(&v, v.length - 50, 50);
        TEST_ASSERT(v.data[v.length - 1] == 949);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_swapsplice");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 10; i++) {
            vec_push(&v, i);
        }
        vec_swapsplice(&v, 0, 3);
        TEST_ASSERT(v.data[0] == 7 && v.data[1] == 8 && v.data[2] == 9);
        vec_swapsplice(&v, v.length - 1, 1);
        TEST_ASSERT(v.data[v.length - 1] == 5);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_insert");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 1000; i++) {
            vec_insert(&v, 0, i);
        }
        TEST_ASSERT(v.data[0] == 999);
        TEST_ASSERT(v.data[v.length - 1] == 0);
        vec_insert(&v, 10, 123);
        TEST_ASSERT(v.data[10] == 123);
        TEST_ASSERT(v.length == 1001);
        vec_insert(&v, v.length - 2, 678);
        TEST_ASSERT(v.data[999] == 678);
        TEST_ASSERT(vec_insert(&v, 10, 123) == 0);
        vec_insert(&v, v.length, 789);
        TEST_ASSERT(v.data[v.length - 1] == 789);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_sort");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 3);
        vec_push(&v, -1);
        vec_push(&v, 0);
        vec_sort(&v, intptrcmp);
        TEST_ASSERT(v.data[0] == -1);
        TEST_ASSERT(v.data[1] == 0);
        TEST_ASSERT(v.data[2] == 3);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_swap");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 'a');
        vec_push(&v, 'b');
        vec_push(&v, 'c');
        vec_swap(&v, 0, 2);
        TEST_ASSERT(v.data[0] == 'c' && v.data[2] == 'a');
        vec_swap(&v, 0, 1);
        TEST_ASSERT(v.data[0] == 'b' && v.data[1] == 'c');
        vec_swap(&v, 1, 2);
        TEST_ASSERT(v.data[1] == 'a' && v.data[2] == 'c');
        vec_swap(&v, 1, 1);
        TEST_ASSERT(v.data[1] == 'a');
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_truncate");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 1000; i++) {
            vec_push(&v, 0);
        }
        vec_truncate(&v, 10000);
        TEST_ASSERT(v.length == 1000);
        vec_truncate(&v, 900);
        TEST_ASSERT(v.length == 900);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_clear");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 1);
        vec_push(&v, 2);
        vec_clear(&v);
        TEST_ASSERT(v.length == 0);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_first");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 0xf00d);
        vec_push(&v, 0);
        TEST_ASSERT(vec_first(&v) == 0xf00d);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_last");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 0);
        vec_push(&v, 0xf00d);
        TEST_ASSERT(vec_last(&v) == 0xf00d);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_reserve");
        vec_int_t v;
        vec_init(&v);
        vec_reserve(&v, 100);
        TEST_ASSERT(v.capacity == 100);
        vec_reserve(&v, 50);
        TEST_ASSERT(v.capacity == 100);
        vec_deinit(&v);
        vec_init(&v);
        vec_push(&v, 123);
        vec_push(&v, 456);
        vec_reserve(&v, 200);
        TEST_ASSERT(v.capacity == 200);
        TEST_ASSERT(vec_reserve(&v, 300) == 0);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_compact");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 1000; i++) {
            vec_push(&v, 0);
        }
        vec_truncate(&v, 3);
        vec_compact(&v);
        TEST_ASSERT(v.length == v.capacity);
        TEST_ASSERT(vec_compact(&v) == 0);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_pusharr");
        int a[5] = {5, 6, 7, 8, 9};
        vec_double_t v;
        vec_init(&v);
        vec_push(&v, 1);
        vec_push(&v, 2);
        vec_pusharr(&v, a, 5);
        TEST_ASSERT(v.data[0] == 1);
        TEST_ASSERT(v.data[2] == 5);
        TEST_ASSERT(v.data[6] == 9);
        vec_deinit(&v);
        vec_init(&v);
        vec_pusharr(&v, a, 5);
        TEST_ASSERT(v.data[0] == 5);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_extend");
        vec_int_t v;
        vec_int_t v2;
        vec_init(&v);
        vec_init(&v2);
        vec_push(&v, 12);
        vec_push(&v, 34);
        vec_push(&v2, 56);
        vec_push(&v2, 78);
        vec_extend(&v, &v2);
        TEST_ASSERT(v.data[0] == 12 && v.data[1] == 34 && v.data[2] == 56 && v.data[3] == 78);
        TEST_ASSERT(v.length == 4);
        vec_deinit(&v);
        vec_deinit(&v2);
    }

    {
        TEST_SECTION("vec_find");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 26; i++) {
            vec_push(&v, 'a' + i);
        }
        vec_find(&v, 'a', i);
        TEST_ASSERT(i == 0);
        vec_find(&v, 'z', i);
        TEST_ASSERT(i == 25);
        vec_find(&v, 'd', i);
        TEST_ASSERT(i == 3);
        vec_find(&v, '_', i);
        TEST_ASSERT(i == -1);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_remove");
        vec_int_t v;
        vec_init(&v);
        int i;
        for (i = 0; i < 26; i++) {
            vec_push(&v, 'a' + i);
        }
        vec_remove(&v, '_');
        TEST_ASSERT(v.length == 26);
        vec_remove(&v, 'c');
        TEST_ASSERT(v.data[0] == 'a' && v.data[1] == 'b' && v.data[2] == 'd' && v.data[3] == 'e');
        TEST_ASSERT(v.length == 25);
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_reverse");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 'a');
        vec_push(&v, 'b');
        vec_push(&v, 'c');
        vec_push(&v, 'd');
        vec_reverse(&v);
        TEST_ASSERT(v.length == 4);
        TEST_ASSERT(v.data[0] == 'd' && v.data[1] == 'c' && v.data[2] == 'b' && v.data[3] == 'a');
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_foreach");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 19);
        vec_push(&v, 31);
        vec_push(&v, 47);
        int i;
        int x;
        int count = 0;
        int acc = 1;
        vec_foreach(&v, x, i) {
            acc *= (x + count);
            count++;
        }
        TEST_ASSERT(acc == (19 + 0) * (31 + 1) * (47 + 2));
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_foreach_rev");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 19);
        vec_push(&v, 31);
        vec_push(&v, 47);
        int i;
        int x;
        int count = 0;
        int acc = 1;
        vec_foreach_rev(&v, x, i) {
            acc *= (x + count);
            count++;
        }
        TEST_ASSERT(acc == (19 + 2) * (31 + 1) * (47 + 0));
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_foreach_ptr");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 19);
        vec_push(&v, 31);
        vec_push(&v, 47);
        int i;
        int* x;
        int count = 0;
        int acc = 1;
        vec_foreach_ptr(&v, x, i) {
            acc *= (*x + count);
            count++;
        }
        TEST_ASSERT(acc == (19 + 0) * (31 + 1) * (47 + 2));
        vec_deinit(&v);
    }

    {
        TEST_SECTION("vec_foreach_ptr_rev");
        vec_int_t v;
        vec_init(&v);
        vec_push(&v, 19);
        vec_push(&v, 31);
        vec_push(&v, 47);
        int i;
        int* x;
        int count = 0;
        int acc = 1;
        vec_foreach_ptr_rev(&v, x, i) {
            acc *= (*x + count);
            count++;
        }
        TEST_ASSERT(acc == (19 + 2) * (31 + 1) * (47 + 0));
        vec_deinit(&v);
    }

    TEST_PRINT_RES();

    return fail_count != 0;
}