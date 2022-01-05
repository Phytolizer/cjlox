#ifndef PHYTO_STRING_VIEW_STRING_VIEW_H_
#define PHYTO_STRING_VIEW_STRING_VIEW_H_

#include <phyto/span/span.h>
#include <stdbool.h>

typedef PHYTO_SPAN_TYPE(char) phyto_string_view_t;

phyto_string_view_t phyto_string_view_from_c(const char* str);
phyto_string_view_t phyto_string_view_new(const char* begin, const char* end);
phyto_string_view_t phyto_string_view_from_ptr_length(const char* ptr, size_t length);
phyto_string_view_t phyto_string_view_empty(void);
bool phyto_string_view_is_empty(phyto_string_view_t view);

#endif  // PHYTO_STRING_VIEW_STRING_VIEW_H_
