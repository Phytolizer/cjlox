#ifndef PHYTO_STRING_VIEW_STRING_VIEW_H_
#define PHYTO_STRING_VIEW_STRING_VIEW_H_

#include <phyto/span/span.h>
#include <stdbool.h>
#include <stdio.h>

typedef PHYTO_SPAN_TYPE(char) phyto_string_view_t;

phyto_string_view_t phyto_string_view_from_c(const char* str);
phyto_string_view_t phyto_string_view_new(const char* begin, const char* end);
phyto_string_view_t phyto_string_view_from_ptr_length(const char* ptr, size_t length);
phyto_string_view_t phyto_string_view_empty(void);
phyto_string_view_t phyto_string_view_substr(phyto_string_view_t view, size_t begin, size_t end);
bool phyto_string_view_is_empty(phyto_string_view_t view);
bool phyto_string_view_equal(phyto_string_view_t view, phyto_string_view_t other);
void phyto_string_view_print_to(phyto_string_view_t view, FILE* fp);

#endif  // PHYTO_STRING_VIEW_STRING_VIEW_H_
