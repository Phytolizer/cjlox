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

#define PHYTO_VEC_INIT_DEFAULT(T)                               \
    PHYTO_VEC_INIT_CONFIGURATED(                                \
        T, ((phyto_vec_configuration_t){                        \
               .element_size = sizeof(T),                       \
               .minimum_size = 8,                               \
               .shrink_mode = phyto_vec_shrink_mode_percentage, \
               .shrink_percentage = 0x40,                       \
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

#define PHYTO_VEC_RESIZE(Vec, NewSize) phyto_vec_resize(&(Vec)->base, NewSize)

#define PHYTO_VEC_POP(Vec, OutValue) phyto_vec_pop_to(&(Vec)->base, OutValue)

bool phyto_vec_resize(phyto_vec_t* vec, size_t new_size);
bool phyto_vec_push(phyto_vec_t* vec, const void* value);
const void* phyto_vec_pop(phyto_vec_t* vec);
bool phyto_vec_pop_to(phyto_vec_t* vec, void* value);

#endif  // PHYTO_VEC_VEC_H_
