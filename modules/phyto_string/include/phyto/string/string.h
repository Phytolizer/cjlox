#ifndef PHYTO_STRING_STRING_H_
#define PHYTO_STRING_STRING_H_

#include <phyto/string_view/string_view.h>
#include <phyto/vec/vec.h>
#include <stdarg.h>
#include <stdbool.h>

typedef PHYTO_VEC_TYPE(char) phyto_string_t;

phyto_string_t phyto_string_new(void);
phyto_string_t phyto_string_own(phyto_string_view_t view);
phyto_string_t phyto_string_from_c(const char* str);
bool phyto_string_reserve(phyto_string_t* string, size_t capacity);
bool phyto_string_append_fill(phyto_string_t* string, size_t count, char fill);

phyto_string_view_t phyto_string_view(phyto_string_t string);

void phyto_string_append_view(phyto_string_t* string, phyto_string_view_t view);

phyto_string_t phyto_string_capitalize(phyto_string_view_t view);
phyto_string_t phyto_string_center(phyto_string_view_t view, size_t width, char fill);
size_t phyto_string_count(phyto_string_view_t view, phyto_string_view_t sub);
size_t phyto_string_count_in_range(phyto_string_view_t view,
                                   phyto_string_view_t sub,
                                   size_t start,
                                   size_t end);
bool phyto_string_ends_with(phyto_string_view_t view, phyto_string_view_t sub);
phyto_string_t phyto_string_expand_tabs(phyto_string_view_t view, size_t tab_width);
bool phyto_string_find(phyto_string_view_t view, phyto_string_view_t sub, size_t* out_index);
bool phyto_string_find_in_range(phyto_string_view_t view,
                                phyto_string_view_t sub,
                                size_t start,
                                size_t end,
                                size_t* out_index);
bool phyto_string_is_alphanumeric(phyto_string_view_t view);
bool phyto_string_is_alphabetic(phyto_string_view_t view);
bool phyto_string_is_decimal(phyto_string_view_t view);
bool phyto_string_is_identifier(phyto_string_view_t view);
bool phyto_string_is_lowercase(phyto_string_view_t view);
bool phyto_string_is_printable(phyto_string_view_t view);
bool phyto_string_is_space(phyto_string_view_t view);
bool phyto_string_is_titlecase(phyto_string_view_t view);
bool phyto_string_is_uppercase(phyto_string_view_t view);
phyto_string_t phyto_string_join(size_t count, phyto_string_view_t sep, ...);
phyto_string_t phyto_string_join_va(size_t count, phyto_string_view_t sep, va_list args);
phyto_string_t phyto_string_left_justify(phyto_string_view_t view, size_t width, char fill);

void phyto_string_free(phyto_string_t* str);

#endif  // PHYTO_STRING_STRING_H_
