#ifndef RXI_VEC_H_
#define RXI_VEC_H_

#include <stdlib.h>
#include <string.h>

#define VEC_VERSION "0.2.1"

#define vec_unpack_(v) (char**)&(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data)

#define vec_t(T)              \
    struct {                  \
        T* data;              \
        int length, capacity; \
    }

#define vec_init(v) memset((v), 0, sizeof(*(v)))

#define vec_deinit(v) (free((v)->data), vec_init(v))

#define vec_push(v, val) \
    (zvec_expand(vec_unpack_(v)) ? -1 : ((v)->data[(v)->length++] = (val), 0), 0)

#define vec_pop(v) (v)->data[--(v)->length]

#define vec_splice(v, start, count) \
    (zvec_splice(vec_unpack_(v), start, count), (v)->length -= (count))

#define vec_swapsplice(v, start, count) \
    (zvec_swapsplice(vec_unpack_(v), start, count), (v)->length -= (count))

#define vec_insert(v, idx, val) \
    (zvec_insert(vec_unpack_(v), idx) ? -1 : ((v)->data[idx] = (val), 0), (v)->length++, 0)

#define vec_sort(v, fn) qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)

#define vec_swap(v, idx1, idx2) zvec_swap(vec_unpack_(v), idx1, idx2)

#define vec_truncate(v, len) ((v)->length = (len) < (v)->length ? (len) : (v)->length)

#define vec_clear(v) ((v)->length = 0)

#define vec_first(v) (v)->data[0]

#define vec_last(v) (v)->data[(v)->length - 1]

#define vec_reserve(v, n) zvec_reserve(vec_unpack_(v), n)

#define vec_compact(v) zvec_compact(vec_unpack_(v))

#define vec_pusharr(v, arr, count)                                   \
    do {                                                             \
        int i_;                                                      \
        int n_ = (count);                                            \
        if (zvec_reserve_po2(vec_unpack_(v), (v)->length + n_) != 0) \
            break;                                                   \
        for (i_ = 0; i_ < n_; i_++) {                                \
            (v)->data[(v)->length++] = (arr)[i_];                    \
        }                                                            \
    } while (0)

#define vec_extend(v, v2) vec_pusharr((v), (v2)->data, (v2)->length)

#define vec_find(v, val, idx)                           \
    do {                                                \
        for ((idx) = 0; (idx) < (v)->length; (idx)++) { \
            if ((v)->data[(idx)] == (val))              \
                break;                                  \
        }                                               \
        if ((idx) == (v)->length)                       \
            (idx) = -1;                                 \
    } while (0)

#define vec_remove(v, val)          \
    do {                            \
        int idx_;                   \
        vec_find(v, val, idx_);     \
        if (idx_ != -1)             \
            vec_splice(v, idx_, 1); \
    } while (0)

#define vec_reverse(v)                                 \
    do {                                               \
        int i_ = (v)->length / 2;                      \
        while (i_--) {                                 \
            vec_swap((v), i_, (v)->length - (i_ + 1)); \
        }                                              \
    } while (0)

#define vec_foreach(v, var, iter) \
    if ((v)->length > 0)          \
        for ((iter) = 0; (iter) < (v)->length && (((var) = (v)->data[(iter)]), 1); ++(iter))

#define vec_foreach_rev(v, var, iter) \
    if ((v)->length > 0)              \
        for ((iter) = (v)->length - 1; (iter) >= 0 && (((var) = (v)->data[(iter)]), 1); --(iter))

#define vec_foreach_ptr(v, var, iter) \
    if ((v)->length > 0)              \
        for ((iter) = 0; (iter) < (v)->length && (((var) = &(v)->data[(iter)]), 1); ++(iter))

#define vec_foreach_ptr_rev(v, var, iter) \
    if ((v)->length > 0)                  \
        for ((iter) = (v)->length - 1; (iter) >= 0 && (((var) = &(v)->data[(iter)]), 1); --(iter))

int zvec_expand(char** data, const int* length, int* capacity, int memsz);
int zvec_reserve(char** data, const int* length, int* capacity, int memsz, int n);
int zvec_reserve_po2(char** data, int* length, int* capacity, int memsz, int n);
int zvec_compact(char** data, const int* length, int* capacity, int memsz);
int zvec_insert(char** data, int* length, int* capacity, int memsz, int idx);
void zvec_splice(char** data,
                 const int* length,
                 const int* capacity,
                 int memsz,
                 int start,
                 int count);
void zvec_swapsplice(char** data,
                     const int* length,
                     const int* capacity,
                     int memsz,
                     int start,
                     int count);
void zvec_swap(char** data, const int* length, const int* capacity, int memsz, int idx1, int idx2);

#endif  // RXI_VEC_H_
