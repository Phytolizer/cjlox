#ifndef PHYTO_COLLECTIONS_CALLBACKS_H_
#define PHYTO_COLLECTIONS_CALLBACKS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PHYTO_COLLECTIONS_FREE_CB(T) void (*free_cb)(T*)
#define PHYTO_COLLECTIONS_COPY_CB(T) T (*copy_cb)(T)
#define PHYTO_COLLECTIONS_COMPARE_CB(T) int32_t (*compare_cb)(T, T)
#define PHYTO_COLLECTIONS_PRINT_CB(T) void (*print_cb)(T, FILE*)

#endif  // PHYTO_COLLECTIONS_CALLBACKS_H_
