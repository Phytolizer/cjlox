#include "phyto/vec/vec.h"

#include <assert.h>
#include <string.h>

bool phyto_vec_resize(phyto_vec_t* vec, size_t new_size) {
    size_t new_capacity;
    if (new_size > vec->capacity) {
        new_capacity = vec->capacity * 2;
        if (new_capacity == 0) {
            assert(vec->configuration.minimum_capacity > 0 &&
                   "attempting to push to an uninitialized vec");
            new_capacity = vec->configuration.minimum_capacity;
        }
        while (new_size > new_capacity) {
            new_capacity *= 2;
        }
    } else if (new_size < vec->capacity) {
        new_capacity = new_size;
    } else {
        return true;
    }
    uint8_t* new_data = realloc(vec->data, new_capacity);
    if (new_data == NULL) {
        return false;
    }
    vec->data = new_data;
    vec->capacity = new_capacity;
    vec->size = new_size;
    return true;
}

bool phyto_vec_push(phyto_vec_t* vec, const void* value) {
    if (vec->size == vec->capacity &&
        !phyto_vec_resize(vec, vec->capacity + 1)) {
        return false;
    }

    memcpy(vec->data + vec->size, value, vec->configuration.element_size);
    vec->size++;

    return true;
}

const void* phyto_vec_pop(phyto_vec_t* vec) {
    if (vec->size == 0) {
        return NULL;
    }

    vec->size--;
    switch (vec->configuration.shrink_mode) {
        case phyto_vec_shrink_mode_never:
            break;
        case phyto_vec_shrink_mode_percentage:
            if (vec->size * 0x100 / vec->capacity <
                vec->configuration.shrink_percentage) {
                phyto_vec_resize(vec, vec->capacity / 2);
            }
            break;
        case phyto_vec_shrink_mode_empty:
            if (vec->size == 0) {
                phyto_vec_resize(vec, 0);
            }
            break;
    }
    return vec->data + vec->size;
}

bool phyto_vec_pop_to(phyto_vec_t* vec, void* value) {
    const void* popped = phyto_vec_pop(vec);
    if (popped == NULL) {
        return false;
    }
    memcpy(value, popped, vec->configuration.element_size);
    return true;
}
