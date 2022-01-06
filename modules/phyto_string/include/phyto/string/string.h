#ifndef PHYTO_STRING_STRING_H_
#define PHYTO_STRING_STRING_H_

#include <phyto/string_view/string_view.h>
#include <phyto/vec/vec.h>
#include <stdarg.h>
#include <stdbool.h>

typedef PHYTO_VEC_TYPE(char) phyto_string_t;
typedef PHYTO_VEC_TYPE(phyto_string_t) phyto_string_vec_t;

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
bool phyto_string_starts_with(phyto_string_view_t view, phyto_string_view_t sub);
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
phyto_string_vec_t phyto_string_partition(phyto_string_view_t view, phyto_string_view_t sep);
phyto_string_t phyto_string_remove_prefix(phyto_string_view_t view, phyto_string_view_t prefix);
phyto_string_t phyto_string_remove_suffix(phyto_string_view_t view, phyto_string_view_t suffix);
phyto_string_t phyto_string_replace(phyto_string_view_t view,
                                    phyto_string_view_t old,
                                    phyto_string_view_t new);
bool phyto_string_find_last(phyto_string_view_t view, phyto_string_view_t sub, size_t* out_index);
bool phyto_string_find_last_in_range(phyto_string_view_t view,
                                     phyto_string_view_t sub,
                                     size_t start,
                                     size_t end,
                                     size_t* out_index);
phyto_string_t phyto_string_right_justify(phyto_string_view_t view, size_t width, char fill);
phyto_string_vec_t phyto_string_partition_last(phyto_string_view_t view, phyto_string_view_t sep);
phyto_string_vec_t phyto_string_split(phyto_string_view_t view, phyto_string_view_t sep);
phyto_string_vec_t phyto_string_split_count(phyto_string_view_t view,
                                            phyto_string_view_t sep,
                                            size_t count);
phyto_string_vec_t phyto_string_split_reverse(phyto_string_view_t view, phyto_string_view_t sep);
phyto_string_vec_t phyto_string_split_reverse_count(phyto_string_view_t view,
                                                    phyto_string_view_t sep,
                                                    size_t count);
phyto_string_vec_t phyto_string_split_whitespace(phyto_string_view_t view);
phyto_string_view_t phyto_string_find_whitespace(phyto_string_view_t view);
phyto_string_vec_t phyto_string_split_lines(phyto_string_view_t view);
phyto_string_t phyto_string_strip(phyto_string_view_t view);
phyto_string_t phyto_string_strip_chars(phyto_string_view_t view, phyto_string_view_t chars);
bool phyto_string_contains(phyto_string_view_t view, char c);
phyto_string_t phyto_string_swap_case(phyto_string_view_t view);
bool phyto_string_push(phyto_string_t* string, char c);
phyto_string_t phyto_string_title(phyto_string_view_t view);
phyto_string_t phyto_string_upper(phyto_string_view_t view);
phyto_string_t phyto_string_zero_fill(phyto_string_view_t view, size_t width);

void phyto_string_free(phyto_string_t* str);
void phyto_string_vec_free(phyto_string_vec_t* vec);

#endif  // PHYTO_STRING_STRING_H_
