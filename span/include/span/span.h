#pragma once

#include <stddef.h>

#define SPAN_TYPE(T) \
    struct { \
        T* begin; \
        T* end; \
        size_t length; \
    }

#define SPAN_NEW(span_begin, span_length) \
    { .begin = (span_begin), .end = (span_begin) + (span_length), .length = (span_length) }

#define SPAN_FROM_BOUNDS(span_begin, span_end) \
    { .begin = (span_begin), .end = (span_end), .length = (span_end) - (span_begin) }

#define SPAN_EMPTY \
    { 0 }
