#include "phyto/string_view/string_view.h"

#include <string.h>

phyto_string_view_t phyto_string_view_from_c(const char* str) {
    return (phyto_string_view_t)PHYTO_SPAN_NEW(str, str + strlen(str));
}

phyto_string_view_t phyto_string_view_new(const char* begin, const char* end) {
    return (phyto_string_view_t)PHYTO_SPAN_NEW(begin, end);
}

phyto_string_view_t phyto_string_view_from_ptr_length(const char* ptr, size_t length) {
    return (phyto_string_view_t)PHYTO_SPAN_NEW(ptr, ptr + length);
}

phyto_string_view_t phyto_string_view_empty(void) {
    return (phyto_string_view_t){0};
}

phyto_string_view_t phyto_string_view_substr(phyto_string_view_t view, size_t begin, size_t end) {
    if (end > view.size) {
        end = view.size;
    }
    if (begin > end) {
        begin = end;
    }
    return (phyto_string_view_t)PHYTO_SPAN_NEW(view.begin + begin, view.begin + end);
}

bool phyto_string_view_is_empty(phyto_string_view_t view) {
    return view.size == 0;
}

bool phyto_string_view_equal(phyto_string_view_t view, phyto_string_view_t other) {
    return view.size == other.size && memcmp(view.begin, other.begin, view.size) == 0;
}

void phyto_string_view_print_to(phyto_string_view_t view, FILE* fp) {
    fwrite(view.begin, 1, view.size, fp);
}
