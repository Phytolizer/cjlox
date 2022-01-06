#ifndef PHYTO_SPAN_SPAN_H_
#define PHYTO_SPAN_SPAN_H_

#include <stddef.h>

#define PHYTO_SPAN_TYPE(T) \
    struct {               \
        const T* begin;    \
        const T* end;      \
        size_t size;       \
    }

#define PHYTO_SPAN_NEW(Begin, End) \
    { .begin = (Begin), .end = (End), .size = (End) - (Begin), }

#define PHYTO_SPAN_INIT(Span, Begin, End) \
    do {                                  \
        (Span)->begin = (Begin);          \
        (Span)->end = (End);              \
        (Span)->size = (End) - (Begin);   \
    } while (0)

#define PHYTO_SPAN_ITER(Span, Iter) \
    for (*(Iter) = (Span)->begin; *(Iter) != (Span)->end; (*(Iter))++)

#endif  // PHYTO_SPAN_SPAN_H_
