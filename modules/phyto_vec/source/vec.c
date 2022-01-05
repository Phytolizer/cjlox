#include "phyto/vec/vec.h"

#include <assert.h>
#include <string.h>

bool phyto_vec_resize(phyto_vec_t* vec, size_t new_size) {
    if (!phyto_vec_reserve(vec, new_size)) {
        return false;
    }
    vec->size = new_size;
    return true;
}

bool phyto_vec_push(phyto_vec_t* vec, const void* value) {
    if (vec->size == vec->capacity && !phyto_vec_reserve(vec, vec->capacity + 1)) {
        return false;
    }

    memcpy(vec->data + vec->size * vec->configuration.element_size, value,
           vec->configuration.element_size);
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
            if (vec->size * 0x100 / vec->capacity < vec->configuration.shrink_percentage) {
                phyto_vec_reserve(vec, vec->capacity / 2);
            }
            break;
        case phyto_vec_shrink_mode_empty:
            if (vec->size == 0) {
                phyto_vec_reserve(vec, 0);
            }
            break;
    }
    return vec->data + vec->size * vec->configuration.element_size;
}

bool phyto_vec_pop_to(phyto_vec_t* vec, void* value) {
    const void* popped = phyto_vec_pop(vec);
    if (popped == NULL) {
        return false;
    }
    memcpy(value, popped, vec->configuration.element_size);
    return true;
}

bool phyto_vec_splice(phyto_vec_t* vec, size_t index, size_t count) {
    if (index >= vec->size) {
        return false;
    }
    if (index + count > vec->size) {
        count = vec->size - index;
    }
    if (count == 0) {
        return true;
    }
    if (vec->size - count == index) {
        vec->size -= count;
        return true;
    }
    memmove(vec->data + index * vec->configuration.element_size,
            vec->data + (index + count) * vec->configuration.element_size,
            (vec->size - index - count) * vec->configuration.element_size);
    vec->size -= count;
    return true;
}

bool phyto_vec_swap_splice(phyto_vec_t* vec, size_t index, size_t count) {
    if (index >= vec->size) {
        return false;
    }
    if (index + count > vec->size) {
        count = vec->size - index;
    }
    if (count == 0) {
        return true;
    }
    memmove(vec->data + index * vec->configuration.element_size,
            vec->data + (vec->size - count) * vec->configuration.element_size,
            count * vec->configuration.element_size);
    vec->size -= count;
    return true;
}

bool phyto_vec_insert(phyto_vec_t* vec, size_t index, const void* value) {
    if (index > vec->size) {
        return false;
    }
    if (vec->size == vec->capacity && !phyto_vec_reserve(vec, vec->size + 1)) {
        return false;
    }
    memmove(vec->data + (index + 1) * vec->configuration.element_size, vec->data + index,
            (vec->size - index) * vec->configuration.element_size);
    memcpy(vec->data + index * vec->configuration.element_size, value,
           vec->configuration.element_size);
    vec->size++;
    return true;
}

bool phyto_vec_sort(phyto_vec_t* vec) {
    if (vec->size == 0) {
        return true;
    }
    qsort(vec->data, vec->size, vec->configuration.element_size, vec->configuration.compare);
    return true;
}

bool phyto_vec_swap(phyto_vec_t* vec, size_t index_a, size_t index_b) {
    if (index_a >= vec->size || index_b >= vec->size) {
        return false;
    }
    void* tmp = malloc(vec->configuration.element_size);
    memcpy(tmp, vec->data + index_a * vec->configuration.element_size,
           vec->configuration.element_size);
    memcpy(vec->data + index_a * vec->configuration.element_size,
           vec->data + index_b * vec->configuration.element_size, vec->configuration.element_size);
    memcpy(vec->data + index_b * vec->configuration.element_size, tmp,
           vec->configuration.element_size);
    free(tmp);
    return true;
}

bool phyto_vec_truncate(phyto_vec_t* vec, size_t new_size) {
    if (new_size > vec->size) {
        return false;
    }
    vec->size = new_size;
    return true;
}

void phyto_vec_clear(phyto_vec_t* vec) {
    vec->size = 0;
}

bool phyto_vec_reserve(phyto_vec_t* vec, size_t new_capacity) {
    if (new_capacity <= vec->capacity) {
        return true;
    }
    size_t actual_new_capacity;
    if (new_capacity > vec->capacity) {
        actual_new_capacity = vec->capacity * 2;
        if (actual_new_capacity == 0) {
            assert(vec->configuration.minimum_capacity > 0 &&
                   "attempting to push to an uninitialized vec");
            actual_new_capacity = vec->configuration.minimum_capacity;
        }
        while (new_capacity > actual_new_capacity) {
            actual_new_capacity *= 2;
        }
    } else if (new_capacity < vec->capacity) {
        actual_new_capacity = new_capacity;
    } else {
        return true;
    }
    uint8_t* new_data = realloc(vec->data, actual_new_capacity * vec->configuration.element_size);
    if (new_data == NULL) {
        return false;
    }
    vec->data = new_data;
    vec->capacity = actual_new_capacity;
    return true;
}

bool phyto_vec_compact(phyto_vec_t* vec) {
    if (vec->size == vec->capacity) {
        return true;
    }
    uint8_t* new_data = realloc(vec->data, vec->size * vec->configuration.element_size);
    if (new_data == NULL) {
        return false;
    }
    vec->data = new_data;
    vec->capacity = vec->size;
    return true;
}

bool phyto_vec_push_vec(phyto_vec_t* vec, const phyto_vec_t* other) {
    assert(other->configuration.element_size == vec->configuration.element_size &&
           "incompatible element sizes");
    if (other->size == 0) {
        return true;
    }
    if (vec->size + other->size > vec->capacity &&
        !phyto_vec_reserve(vec, vec->size + other->size)) {
        return false;
    }
    memcpy(vec->data + vec->size * vec->configuration.element_size, other->data,
           other->size * vec->configuration.element_size);
    vec->size += other->size;
    return true;
}

bool phyto_vec_push_array(phyto_vec_t* vec, const void* array, size_t count) {
    if (count == 0) {
        return true;
    }
    if (vec->size + count > vec->capacity && !phyto_vec_reserve(vec, vec->size + count)) {
        return false;
    }
    memcpy(vec->data + vec->size * vec->configuration.element_size, array,
           count * vec->configuration.element_size);
    vec->size += count;
    return true;
}

bool phyto_vec_find(phyto_vec_t* vec, const void* value, size_t* out_index) {
    if (vec->size == 0) {
        return false;
    }
    size_t i = 0;
    while (i < vec->size && vec->configuration.compare(
                                vec->data + i * vec->configuration.element_size, value) != 0) {
        i++;
    }
    if (i == vec->size) {
        return false;
    }
    *out_index = i;
    return true;
}

bool phyto_vec_remove(phyto_vec_t* vec, const void* value) {
    size_t index;
    if (!phyto_vec_find(vec, value, &index)) {
        return false;
    }
    return phyto_vec_splice(vec, index, 1);
}

bool phyto_vec_reverse(phyto_vec_t* vec) {
    if (vec->size == 0) {
        return true;
    }
    size_t i = 0;
    size_t j = vec->size - 1;
    while (i < j) {
        if (!phyto_vec_swap(vec, i, j)) {
            return false;
        }
        i++;
        j--;
    }
    return true;
}
