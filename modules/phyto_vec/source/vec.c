#include "phyto/vec/vec.h"

#include <stdlib.h>

bool phyto_vec_expand_(phyto_vec_unpacked_t vec) {
    if (*vec.p_size + 1 > *vec.p_capacity) {
        size_t n = (*vec.p_capacity == 0) ? 1 : *vec.p_capacity << 1;
        void* ptr = realloc(*vec.p_data, n * vec.element_size);
        if (ptr == NULL) {
            return false;
        }
        *vec.p_data = ptr;
        *vec.p_capacity = n;
    }
    return true;
}
bool phyto_vec_reserve_(phyto_vec_unpacked_t vec, size_t n) {
    if (n > *vec.p_capacity) {
        void* ptr = realloc(*vec.p_data, n * vec.element_size);
        if (ptr == NULL) {
            return false;
        }
        *vec.p_data = ptr;
        *vec.p_capacity = n;
    }
    return true;
}
bool phyto_vec_reserve_po2_(phyto_vec_unpacked_t vec, size_t n) {
    size_t n2 = 1;
    if (n == 0) {
        return true;
    }
    while (n2 < n) {
        n2 <<= 1;
    }
    return phyto_vec_reserve_(vec, n2);
}
bool phyto_vec_compact_(phyto_vec_unpacked_t vec) {
    if (*vec.p_size == 0) {
        free(*vec.p_data);
        *vec.p_data = NULL;
        *vec.p_capacity = 0;
        return true;
    }
    void* ptr = realloc(*vec.p_data, *vec.p_size * vec.element_size);
    if (ptr == NULL) {
        return false;
    }
    *vec.p_capacity = *vec.p_size;
    *vec.p_data = ptr;
    return true;
}
bool phyto_vec_insert_(phyto_vec_unpacked_t vec, size_t idx) {
    if (!phyto_vec_expand_(vec)) {
        return false;
    }
    memmove(*vec.p_data + (idx + 1) * vec.element_size, *vec.p_data + idx * vec.element_size,
            (*vec.p_size - idx) * vec.element_size);
    return true;
}
void phyto_vec_splice_(phyto_vec_unpacked_t vec, size_t start, size_t count) {
    memmove(*vec.p_data + start * vec.element_size,
            *vec.p_data + (start + count) * vec.element_size,
            (*vec.p_size - start - count) * vec.element_size);
}
void phyto_vec_swapsplice_(phyto_vec_unpacked_t vec, size_t start, size_t count) {
    memmove(*vec.p_data + start * vec.element_size,
            *vec.p_data + (*vec.p_size - count) * vec.element_size, count * vec.element_size);
}
void phyto_vec_swap_(phyto_vec_unpacked_t vec, size_t idx1, size_t idx2) {
    if (idx1 == idx2) {
        return;
    }
    uint8_t* a = *vec.p_data + idx1 * vec.element_size;
    uint8_t* b = *vec.p_data + idx2 * vec.element_size;
    uint8_t tmp;
    for (size_t i = 0; i < vec.element_size; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}
