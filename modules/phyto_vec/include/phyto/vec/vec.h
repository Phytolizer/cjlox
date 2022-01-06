#ifndef PHYTO_VEC_V2_VEC_H_
#define PHYTO_VEC_V2_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t** p_data;
    size_t* p_size;
    size_t* p_capacity;
    size_t element_size;
} phyto_vec_unpacked_t;

#define PHYTO_VEC_UNPACK(Vec)                 \
    ((phyto_vec_unpacked_t){                  \
        .p_data = (uint8_t**)&(Vec)->data,    \
        .p_size = &(Vec)->size,               \
        .p_capacity = &(Vec)->capacity,       \
        .element_size = sizeof(*(Vec)->data), \
    })

#define PHYTO_VEC_TYPE(T) \
    struct {              \
        T* data;          \
        size_t size;      \
        size_t capacity;  \
    }

#define PHYTO_VEC_INIT(V) memset((V), 0, sizeof(*(V)))
#define PHYTO_VEC_FREE(V)  \
    do {                   \
        free((V)->data);   \
        PHYTO_VEC_INIT(V); \
    } while (0)

#define PHYTO_VEC_PUSH(V, Val)                             \
    ({                                                     \
        bool ret = phyto_vec_expand_(PHYTO_VEC_UNPACK(V)); \
        if (ret) {                                         \
            (V)->data[(V)->size++] = (Val);                \
        }                                                  \
        ret;                                               \
    })

#define PHYTO_VEC_POP(V, Val)                  \
    ({                                         \
        bool ret = (V)->size > 0;              \
        if (ret) {                             \
            *(Val) = ((V)->data[--(V)->size]); \
        }                                      \
        ret;                                   \
    })

#define PHYTO_VEC_SPLICE(V, Start, Count)                     \
    do {                                                      \
        phyto_vec_splice_(PHYTO_VEC_UNPACK(V), Start, Count); \
        (V)->size -= (Count);                                 \
    } while (0)

#define PHYTO_VEC_SWAPSPLICE(V, Start, Count)                     \
    do {                                                          \
        phyto_vec_swapsplice_(PHYTO_VEC_UNPACK(V), Start, Count); \
        (V)->size -= (Count);                                     \
    } while (0)

#define PHYTO_VEC_INSERT(V, Idx, Val)                           \
    ({                                                          \
        bool ret = phyto_vec_insert_(PHYTO_VEC_UNPACK(V), Idx); \
        if (ret) {                                              \
            (V)->data[Idx] = (Val);                             \
            ++(V)->size;                                        \
        }                                                       \
        ret;                                                    \
    })

#define PHYTO_VEC_SORT(V, Fn) qsort((V)->data, (V)->size, sizeof(*(V)->data), Fn)

#define PHYTO_VEC_SWAP(V, IndexA, IndexB) phyto_vec_swap_(PHYTO_VEC_UNPACK(V), IndexA, IndexB)

#define PHYTO_VEC_TRUNCATE(V, Len) ((V)->size = (Len) < (V)->size ? (Len) : (V)->size)

#define PHYTO_VEC_CLEAR(V) ((V)->size = 0)

#define PHYTO_VEC_FIRST(V) ((V)->data[0])

#define PHYTO_VEC_LAST(V) ((V)->data[(V)->size - 1])

#define PHYTO_VEC_RESERVE(V, N) phyto_vec_reserve_(PHYTO_VEC_UNPACK(V), N)

#define PHYTO_VEC_COMPACT(V) phyto_vec_compact_(PHYTO_VEC_UNPACK(V))

#define PHYTO_VEC_PUSH_ARRAY(V, Arr, Count)                             \
    do {                                                                \
        size_t n_ = (Count);                                            \
        if (!phyto_vec_reserve_(PHYTO_VEC_UNPACK(V), (V)->size + n_)) { \
            break;                                                      \
        }                                                               \
        for (size_t i_ = 0; i_ < n_; ++i_) {                            \
            (V)->data[(V)->size++] = (Arr)[i_];                         \
        }                                                               \
    } while (0)

#define PHYTO_VEC_EXTEND(V, V2) PHYTO_VEC_PUSH_ARRAY((V), (V2).data, (V2).size)

#define PHYTO_VEC_FIND(V, Val, Idx)                        \
    ({                                                     \
        for (*(Idx) = 0; *(Idx) < (V)->size; ++(*(Idx))) { \
            if ((V)->data[*(Idx)] == (Val)) {              \
                break;                                     \
            }                                              \
        }                                                  \
        *(Idx) != (V)->size;                               \
    })

#define PHYTO_VEC_REMOVE(V, Val)            \
    do {                                    \
        size_t idx_;                        \
        if (PHYTO_VEC_FIND(V, Val, idx_)) { \
            PHYTO_VEC_SPLICE(V, idx_, 1);   \
        }                                   \
    } while (0)

#define PHYTO_VEC_REVERSE(V)                           \
    do {                                               \
        size_t i_ = (V)->size / 2;                     \
        while (i_--) {                                 \
            PHYTO_VEC_SWAP(V, i_, (V)->size - i_ - 1); \
        }                                              \
    } while (0)

bool phyto_vec_expand_(phyto_vec_unpacked_t vec);
bool phyto_vec_reserve_(phyto_vec_unpacked_t vec, size_t n);
bool phyto_vec_reserve_po2_(phyto_vec_unpacked_t vec, size_t n);
bool phyto_vec_compact_(phyto_vec_unpacked_t vec);
bool phyto_vec_insert_(phyto_vec_unpacked_t vec, size_t idx);
void phyto_vec_splice_(phyto_vec_unpacked_t vec, size_t start, size_t count);
void phyto_vec_swapsplice_(phyto_vec_unpacked_t vec, size_t start, size_t count);
void phyto_vec_swap_(phyto_vec_unpacked_t vec, size_t idx1, size_t idx2);

#endif  // PHYTO_VEC_V2_VEC_H_
