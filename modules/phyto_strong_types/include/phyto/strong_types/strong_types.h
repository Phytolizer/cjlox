#ifndef PHYTO_STRONG_TYPES_STRONG_TYPES_H_
#define PHYTO_STRONG_TYPES_STRONG_TYPES_H_

#include <stddef.h>
#include <stdint.h>

#define PHYTO_STRONG_T(T) \
    struct {              \
        T value;          \
    }

#endif  // PHYTO_STRONG_TYPES_STRONG_TYPES_H_
