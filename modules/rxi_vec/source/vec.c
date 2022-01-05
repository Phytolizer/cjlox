#include "rxi/vec/vec.h"

#include <stddef.h>

int zvec_expand(char** data, const int* length, int* capacity, int memsz) {
    if (*length + 1 > *capacity) {
        void* ptr;
        int n = (*capacity == 0) ? 1 : *capacity << 1;
        ptr = realloc(*data, (size_t)n * memsz);
        if (ptr == NULL) {
            return -1;
        }
        *data = ptr;
        *capacity = n;
    }
    return 0;
}

int zvec_reserve(char** data, const int* length, int* capacity, int memsz, int n) {
    (void)length;
    if (n > *capacity) {
        void* ptr = realloc(*data, (size_t)n * memsz);
        if (ptr == NULL) {
            return -1;
        }
        *data = ptr;
        *capacity = n;
    }
    return 0;
}

int zvec_reserve_po2(char** data, int* length, int* capacity, int memsz, int n) {
    int n2 = 1;
    if (n == 0) {
        return 0;
    }
    while (n2 < n) {
        n2 <<= 1;
    }
    return zvec_reserve(data, length, capacity, memsz, n2);
}

int zvec_compact(char** data, const int* length, int* capacity, int memsz) {
    if (*length == 0) {
        free(*data);
        *data = NULL;
        *capacity = 0;
        return 0;
    }
    void* ptr;
    int n = *length;
    ptr = realloc(*data, (size_t)n * memsz);
    if (ptr == NULL) {
        return -1;
    }
    *capacity = n;
    *data = ptr;
    return 0;
}

int zvec_insert(char** data, int* length, int* capacity, int memsz, int idx) {
    int err = zvec_expand(data, length, capacity, memsz);
    if (err) {
        return err;
    }
    memmove(*data + (ptrdiff_t)((idx + 1) * memsz), *data + (ptrdiff_t)(idx * memsz),
            (size_t)(*length - idx) * memsz);
    return 0;
}

void zvec_splice(char** data,
                 const int* length,
                 const int* capacity,
                 int memsz,
                 int start,
                 int count) {
    (void)capacity;
    memmove(*data + (ptrdiff_t)(start * memsz), *data + (size_t)(start + count) * memsz,
            (size_t)(*length - start - count) * memsz);
}

void zvec_swapsplice(char** data,
                     const int* length,
                     const int* capacity,
                     int memsz,
                     int start,
                     int count) {
    (void)capacity;
    memmove(*data + (ptrdiff_t)(start * memsz), *data + (size_t)(*length - count) * memsz,
            (size_t)count * memsz);
}

void zvec_swap(char** data, const int* length, const int* capacity, int memsz, int idx1, int idx2) {
    unsigned char* a;
    unsigned char* b;
    unsigned char tmp;
    int count;
    (void)length;
    (void)capacity;
    if (idx1 == idx2) {
        return;
    }
    a = (unsigned char*)*data + (ptrdiff_t)idx1 * memsz;
    b = (unsigned char*)*data + (ptrdiff_t)idx2 * memsz;
    count = memsz;
    while (count--) {
        tmp = *a;
        *a = *b;
        *b = tmp;
        a++, b++;
    }
}