#ifndef PHYTO_IO_IO_H_
#define PHYTO_IO_IO_H_

#include <stdint.h>

typedef enum {
    phyto_io_vec_shrink_never,
    phyto_io_vec_shrink_empty,
    phyto_io_vec_shrink_percentage,
} phyto_io_vec_shrink_t;

typedef struct {
    uint8_t* data;
    uint64_t size;
    uint64_t capacity;
    uint64_t min_capacity;
    phyto_io_vec_shrink_t shrink;
    // proportion of 0x100
    uint8_t shrink_percentage;
} phyto_io_vec_t;

typedef union {
    char* data;
    phyto_io_vec_t base;
} phyto_io_string_t;

#define PHYTO_IO_STRING_INIT                          \
    (phyto_io_string_t) {                             \
        .base = {                                     \
            .data = NULL,                             \
            .size = 0,                                \
            .capacity = 0,                            \
            .min_capacity = 64,                       \
            .shrink = phyto_io_vec_shrink_percentage, \
            .shrink_percentage = 0x40,                \
        }                                             \
    }

#endif  // PHYTO_IO_IO_H_
