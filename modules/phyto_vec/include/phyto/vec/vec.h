#ifndef PHYTO_VEC_VEC_H_
#define PHYTO_VEC_VEC_H_

#include <phyto/strong_types/strong_types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    phyto_vec_shrink_mode_never,
    phyto_vec_shrink_mode_empty,
    phyto_vec_shrink_mode_percentage,
} phyto_vec_shrink_mode_t;

typedef struct {
    size_t element_size;
    size_t minimum_capacity;
    phyto_vec_shrink_mode_t shrink_mode;
    // proportion of 0x100
    uint32_t shrink_percentage;
    int (*compare)(const void*, const void*);
} phyto_vec_configuration_t;

typedef struct {
    uint8_t* data;
    size_t size;
    size_t capacity;
    phyto_vec_configuration_t configuration;
} phyto_vec_t;

#define PHYTO_VEC_WRAP(T) \
    union {               \
        T* data;          \
        phyto_vec_t base; \
    }

#define PHYTO_VEC_INIT_CONFIGURATED(T, Configuration) \
    {                                                 \
        .base = {                                     \
            .data = NULL,                             \
            .size = 0,                                \
            .capacity = 0,                            \
            .configuration = Configuration,           \
        }                                             \
    }

#define PHYTO_VEC_INIT_DEFAULT(T, Compare)                                              \
    PHYTO_VEC_INIT_CONFIGURATED(T, ((phyto_vec_configuration_t){                        \
                                       .element_size = sizeof(T),                       \
                                       .minimum_size = 8,                               \
                                       .shrink_mode = phyto_vec_shrink_mode_percentage, \
                                       .shrink_percentage = 0x40,                       \
                                       .compare = Compare,                              \
                                   }))

#define PHYTO_VEC_FREE(Vec)                                            \
    do {                                                               \
        free((Vec)->base.data);                                        \
        memset(&(Vec)->base, 0, offsetof(phyto_vec_t, configuration)); \
    } while (false)

#define PHYTO_VEC_PUSH(Vec, Value)          \
    ({                                      \
        typeof(Value) val = Value;          \
        phyto_vec_push(&(Vec)->base, &val); \
    })

#define PHYTO_VEC_SIZE(Vec) ((Vec)->base.size)

#define PHYTO_VEC_RESIZE(Vec, NewSize) phyto_vec_resize(&(Vec)->base, NewSize)

#define PHYTO_VEC_POP(Vec, OutValue) phyto_vec_pop_to(&(Vec)->base, OutValue)
#define PHYTO_VEC_SPLICE(Vec, Index, Count) phyto_vec_splice(&(Vec)->base, Index, Count)
#define PHYTO_VEC_SWAP_SPLICE(Vec, Index, Count) phyto_vec_swap_splice(&(Vec)->base, Index, Count)
#define PHYTO_VEC_INSERT(Vec, Index, Value)          \
    ({                                               \
        typeof(Value) val = Value;                   \
        phyto_vec_insert(&(Vec)->base, Index, &val); \
    })
#define PHYTO_VEC_SORT(Vec) phyto_vec_sort(&(Vec)->base)
#define PHYTO_VEC_SWAP(Vec, IndexA, IndexB) phyto_vec_swap(&(Vec)->base, IndexA, IndexB)
#define PHYTO_VEC_TRUNCATE(Vec, NewSize) phyto_vec_truncate(&(Vec)->base, NewSize)
#define PHYTO_VEC_CLEAR(Vec) phyto_vec_clear(&(Vec)->base)
#define PHYTO_VEC_RESERVE(Vec, NewCapacity) phyto_vec_reserve(&(Vec)->base, NewCapacity)
#define PHYTO_VEC_COMPACT(Vec) phyto_vec_compact(&(Vec)->base)
#define PHYTO_VEC_PUSH_VEC(Vec, Other) phyto_vec_push_vec(&(Vec)->base, &(Other)->base)
#define PHYTO_VEC_PUSH_ARRAY(Vec, Array, Count) phyto_vec_push_array(&(Vec)->base, Array, Count)
#define PHYTO_VEC_FIND(Vec, Value, OutIndex)          \
    ({                                                \
        typeof(Value) val = Value;                    \
        phyto_vec_find(&(Vec)->base, &val, OutIndex); \
    })
#define PHYTO_VEC_REMOVE(Vec, Value)          \
    ({                                        \
        typeof(Value) val = Value;            \
        phyto_vec_remove(&(Vec)->base, &val); \
    })
#define PHYTO_VEC_REVERSE(Vec) phyto_vec_reverse(&(Vec)->base)

bool phyto_vec_resize(phyto_vec_t* vec, size_t new_size);
bool phyto_vec_push(phyto_vec_t* vec, const void* value);
const void* phyto_vec_pop(phyto_vec_t* vec);
bool phyto_vec_pop_to(phyto_vec_t* vec, void* value);
bool phyto_vec_splice(phyto_vec_t* vec, size_t index, size_t count);
bool phyto_vec_swap_splice(phyto_vec_t* vec, size_t index, size_t count);
bool phyto_vec_insert(phyto_vec_t* vec, size_t index, const void* value);
bool phyto_vec_sort(phyto_vec_t* vec);
bool phyto_vec_swap(phyto_vec_t* vec, size_t index_a, size_t index_b);
bool phyto_vec_truncate(phyto_vec_t* vec, size_t new_size);
void phyto_vec_clear(phyto_vec_t* vec);
bool phyto_vec_reserve(phyto_vec_t* vec, size_t new_capacity);
bool phyto_vec_compact(phyto_vec_t* vec);
bool phyto_vec_push_vec(phyto_vec_t* vec, const phyto_vec_t* other);
bool phyto_vec_push_array(phyto_vec_t* vec, const void* array, size_t count);
bool phyto_vec_find(phyto_vec_t* vec, const void* value, size_t* out_index);
bool phyto_vec_remove(phyto_vec_t* vec, const void* value);
bool phyto_vec_reverse(phyto_vec_t* vec);

#endif  // PHYTO_VEC_VEC_H_
