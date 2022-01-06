#include "phyto/string/string.h"

#include <nonstd/ctype.h>
#include <phyto/string_view/string_view.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

phyto_string_t phyto_string_new(void) {
    phyto_string_t result;
    PHYTO_VEC_INIT(&result);
    return result;
}

phyto_string_t phyto_string_own(phyto_string_view_t view) {
    phyto_string_t str = phyto_string_new();
    PHYTO_VEC_PUSH_ARRAY(&str, view.begin, view.size);
    return str;
}

phyto_string_t phyto_string_from_c(const char* str) {
    return phyto_string_own(phyto_string_view_from_c(str));
}

phyto_string_t phyto_string_from_sprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);
    phyto_string_t str = phyto_string_new();
    phyto_string_reserve(&str, (size_t)length + 1);
    va_start(args, format);
    vsnprintf(str.data, (size_t)length + 1, format, args);
    va_end(args);
    str.size = length;
    return str;
}

bool phyto_string_reserve(phyto_string_t* string, size_t capacity) {
    return PHYTO_VEC_RESERVE(string, capacity);
}

bool phyto_string_append_fill(phyto_string_t* string, size_t count, char fill) {
    for (size_t i = 0; i < count; ++i) {
        if (!PHYTO_VEC_PUSH(string, fill)) {
            return false;
        }
    }
    return true;
}

phyto_string_view_t phyto_string_view(phyto_string_t string) {
    if (string.size == 0) {
        return phyto_string_view_empty();
    }
    return phyto_string_view_from_ptr_length(string.data, string.size);
}

void phyto_string_append_view(phyto_string_t* string, phyto_string_view_t view) {
    PHYTO_VEC_PUSH_ARRAY(string, view.begin, view.size);
}

void phyto_string_append_c(phyto_string_t* string, const char* cstr) {
    phyto_string_append_view(string, phyto_string_view_from_c(cstr));
}

phyto_string_t phyto_string_capitalize(phyto_string_view_t view) {
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, view.size);
    for (size_t i = 0; i < view.size; ++i) {
        char c = view.begin[i];
        if (i == 0) {
            c = nonstd_toupper(c);
        } else {
            c = nonstd_tolower(c);
        }
        PHYTO_VEC_PUSH(&result, c);
    }
    return result;
}

phyto_string_t phyto_string_center(phyto_string_view_t view, size_t width, char fill) {
    phyto_string_t result = phyto_string_new();
    if (view.size >= width) {
        return phyto_string_own(view);
    }
    size_t left_padding = (width - view.size) / 2;
    size_t right_padding = width - view.size - left_padding;
    for (size_t i = 0; i < left_padding; ++i) {
        PHYTO_VEC_PUSH(&result, fill);
    }
    PHYTO_VEC_PUSH_ARRAY(&result, view.begin, view.size);
    for (size_t i = 0; i < right_padding; ++i) {
        PHYTO_VEC_PUSH(&result, fill);
    }
    return result;
}

size_t phyto_string_count(phyto_string_view_t view, phyto_string_view_t sub) {
    return phyto_string_count_in_range(view, sub, 0, view.size);
}

size_t phyto_string_count_in_range(phyto_string_view_t view,
                                   phyto_string_view_t sub,
                                   size_t start,
                                   size_t end) {
    if (sub.size == 0) {
        return 0;
    }
    if (start > end) {
        return 0;
    }
    if (end - start < sub.size) {
        return 0;
    }
    size_t count = 0;
    size_t sub_size = sub.size;
    for (size_t i = start; i <= end - sub_size; ++i) {
        if (memcmp(view.begin + i, sub.begin, sub_size) == 0) {
            ++count;
        }
    }
    return count;
}

bool phyto_string_ends_with(phyto_string_view_t view, phyto_string_view_t sub) {
    if (view.size < sub.size) {
        return false;
    }
    if (sub.size == 0) {
        return false;
    }
    return memcmp(view.begin + view.size - sub.size, sub.begin, sub.size) == 0;
}

bool phyto_string_starts_with(phyto_string_view_t view, phyto_string_view_t sub) {
    if (view.size < sub.size) {
        return false;
    }
    if (sub.size == 0) {
        return false;
    }
    return memcmp(view.begin, sub.begin, sub.size) == 0;
}

phyto_string_t phyto_string_expand_tabs(phyto_string_view_t view, size_t tab_width) {
    if (tab_width == 0) {
        return phyto_string_replace(view, phyto_string_view_from_c("\t"),
                                    phyto_string_view_empty());
    }
    phyto_string_t result = phyto_string_new();
    // if no tabs, it will be the same length
    // if there are tabs, it will be longer but the vec will just resize
    phyto_string_reserve(&result, view.size);
    for (size_t i = 0; i < view.size; ++i) {
        char c = view.begin[i];
        if (c == '\t') {
            size_t num_spaces = tab_width - (result.size % tab_width);
            for (size_t j = 0; j < num_spaces; ++j) {
                PHYTO_VEC_PUSH(&result, ' ');
            }
        } else {
            PHYTO_VEC_PUSH(&result, c);
        }
    }
    return result;
}

bool phyto_string_find(phyto_string_view_t view, phyto_string_view_t sub, size_t* out_index) {
    return phyto_string_find_in_range(view, sub, 0, view.size, out_index);
}

bool phyto_string_find_in_range(phyto_string_view_t view,
                                phyto_string_view_t sub,
                                size_t start,
                                size_t end,
                                size_t* out_index) {
    size_t sub_size = sub.size;
    for (size_t i = start; i < end - sub_size; ++i) {
        if (memcmp(view.begin + i, sub.begin, sub_size) == 0) {
            if (out_index != NULL) {
                *out_index = i;
            }
            return true;
        }
    }
    return false;
}

bool phyto_string_is_alphanumeric(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (!nonstd_isalnum(view.begin[i])) {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_alphabetic(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (!nonstd_isalpha(view.begin[i])) {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_decimal(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (!nonstd_isdigit(view.begin[i])) {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_identifier(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    if (!nonstd_isalpha(view.begin[0]) && view.begin[0] != '_') {
        return false;
    }
    for (size_t i = 1; i < view.size; ++i) {
        if (!nonstd_isalnum(view.begin[i]) && view.begin[i] != '_') {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_lowercase(phyto_string_view_t view) {
    bool found = false;
    for (size_t i = 0; i < view.size; ++i) {
        if (nonstd_isupper(view.begin[i])) {
            return false;
        }
        if (nonstd_islower(view.begin[i])) {
            found = true;
        }
    }
    return found;
}

bool phyto_string_is_printable(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (!nonstd_isprint(view.begin[i])) {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_space(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (!nonstd_isspace(view.begin[i])) {
            return false;
        }
    }
    return true;
}

bool phyto_string_is_titlecase(phyto_string_view_t view) {
    if (view.size == 0) {
        return false;
    }
    bool was_upper = false;
    bool was_lower = false;
    for (size_t i = 0; i < view.size; ++i) {
        if (nonstd_isupper(view.begin[i])) {
            if (was_upper || was_lower) {
                return false;
            }
            was_upper = true;
            was_lower = false;
        } else if (nonstd_islower(view.begin[i])) {
            was_lower = true;
            was_upper = false;
        } else {
            was_upper = false;
            was_lower = false;
        }
    }
    return true;
}

bool phyto_string_is_uppercase(phyto_string_view_t view) {
    bool found = false;
    if (view.size == 0) {
        return false;
    }
    for (size_t i = 0; i < view.size; ++i) {
        if (nonstd_islower(view.begin[i])) {
            return false;
        }
        if (nonstd_isupper(view.begin[i])) {
            found = true;
        }
    }
    return found;
}

phyto_string_t phyto_string_join(size_t count, phyto_string_view_t sep, ...) {
    va_list args;
    va_start(args, sep);
    phyto_string_t result = phyto_string_join_va(count, sep, args);
    va_end(args);
    return result;
}

// NOLINTNEXTLINE(readability-non-const-parameter)
phyto_string_t phyto_string_join_va(size_t count, phyto_string_view_t sep, va_list args) {
    phyto_string_t result = phyto_string_new();
    for (size_t i = 0; i < count; ++i) {
        phyto_string_view_t view = va_arg(args, phyto_string_view_t);
        phyto_string_append_view(&result, view);
        if (i < count - 1) {
            phyto_string_append_view(&result, sep);
        }
    }
    return result;
}

phyto_string_t phyto_string_left_justify(phyto_string_view_t view, size_t width, char fill) {
    phyto_string_t result = phyto_string_new();
    size_t size = view.size;
    if (size >= width) {
        return phyto_string_own(view);
    }
    phyto_string_reserve(&result, width);
    phyto_string_append_view(&result, view);
    phyto_string_append_fill(&result, width - size, fill);
    return result;
}

phyto_string_vec_t phyto_string_partition(phyto_string_view_t view, phyto_string_view_t sep) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    if (!phyto_string_find(view, sep, &sep_pos)) {
        // return (view, "", "")
        PHYTO_VEC_PUSH(&result, phyto_string_own(view));
        PHYTO_VEC_PUSH(&result, phyto_string_new());
        PHYTO_VEC_PUSH(&result, phyto_string_new());
        return result;
    }
    // return (view[:sep_pos], sep, view[sep_pos + sep.size:])
    PHYTO_VEC_PUSH(&result, phyto_string_own(phyto_string_view_substr(view, 0, sep_pos)));
    PHYTO_VEC_PUSH(&result, phyto_string_own(sep));
    PHYTO_VEC_PUSH(&result,
                   phyto_string_own(phyto_string_view_substr(view, sep_pos + sep.size, view.size)));
    return result;
}

phyto_string_t phyto_string_remove_prefix(phyto_string_view_t view, phyto_string_view_t prefix) {
    if (!phyto_string_starts_with(view, prefix)) {
        return phyto_string_own(view);
    }
    return phyto_string_own(phyto_string_view_substr(view, prefix.size, view.size));
}

phyto_string_t phyto_string_remove_suffix(phyto_string_view_t view, phyto_string_view_t suffix) {
    if (!phyto_string_ends_with(view, suffix)) {
        return phyto_string_own(view);
    }
    return phyto_string_own(phyto_string_view_substr(view, 0, view.size - suffix.size));
}

phyto_string_t phyto_string_replace(phyto_string_view_t view,
                                    phyto_string_view_t old,
                                    phyto_string_view_t new) {
    phyto_string_t result = phyto_string_new();
    size_t pos = 0;
    while (true) {
        size_t next_pos;
        if (!phyto_string_find_in_range(view, old, pos, view.size, &next_pos)) {
            phyto_string_append_view(&result, phyto_string_view_substr(view, pos, view.size));
            return result;
        }
        phyto_string_append_view(&result, phyto_string_view_substr(view, pos, next_pos));
        phyto_string_append_view(&result, new);
        pos = next_pos + old.size;
    }
}

bool phyto_string_find_last(phyto_string_view_t view, phyto_string_view_t sub, size_t* out_index) {
    return phyto_string_find_last_in_range(view, sub, 0, view.size, out_index);
}

bool phyto_string_find_last_in_range(phyto_string_view_t view,
                                     phyto_string_view_t sub,
                                     size_t start,
                                     size_t end,
                                     size_t* out_index) {
    size_t index = end;
    while (true) {
        size_t pos;
        if (!phyto_string_find_in_range(view, sub, index - sub.size, index, &pos)) {
            return false;
        }
        index = pos;
        if (index == start) {
            return false;
        }
        index -= sub.size;
        if (phyto_string_view_equal(view, phyto_string_view_substr(view, index, sub.size))) {
            *out_index = index;
            return true;
        }
    }
}

phyto_string_t phyto_string_right_justify(phyto_string_view_t view, size_t width, char fill) {
    phyto_string_t result = phyto_string_new();
    size_t size = view.size;
    if (size >= width) {
        return phyto_string_own(view);
    }
    phyto_string_reserve(&result, width);
    phyto_string_append_fill(&result, width - size, fill);
    phyto_string_append_view(&result, view);
    return result;
}

phyto_string_vec_t phyto_string_partition_last(phyto_string_view_t view, phyto_string_view_t sep) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    if (!phyto_string_find_last(view, sep, &sep_pos)) {
        // return (view, "", "")
        PHYTO_VEC_PUSH(&result, phyto_string_own(view));
        PHYTO_VEC_PUSH(&result, phyto_string_new());
        PHYTO_VEC_PUSH(&result, phyto_string_new());
        return result;
    }
    // return (view[:sep_pos], sep, view[sep_pos + sep.size:])
    PHYTO_VEC_PUSH(&result, phyto_string_own(phyto_string_view_substr(view, 0, sep_pos)));
    PHYTO_VEC_PUSH(&result, phyto_string_own(sep));
    PHYTO_VEC_PUSH(&result,
                   phyto_string_own(phyto_string_view_substr(view, sep_pos + sep.size, view.size)));
    return result;
}

phyto_string_vec_t phyto_string_split(phyto_string_view_t view, phyto_string_view_t sep) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    while (true) {
        if (!phyto_string_find(view, sep, &sep_pos)) {
            PHYTO_VEC_PUSH(&result, phyto_string_own(view));
            return result;
        }
        PHYTO_VEC_PUSH(&result, phyto_string_own(phyto_string_view_substr(view, 0, sep_pos)));
        view = phyto_string_view_substr(view, sep_pos + sep.size, view.size);
    }
}

phyto_string_vec_t phyto_string_split_count(phyto_string_view_t view,
                                            phyto_string_view_t sep,
                                            size_t count) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    while (count > 0) {
        if (!phyto_string_find(view, sep, &sep_pos)) {
            break;
        }
        PHYTO_VEC_PUSH(&result, phyto_string_own(phyto_string_view_substr(view, 0, sep_pos)));
        view = phyto_string_view_substr(view, sep_pos + sep.size, view.size);
        count--;
    }
    PHYTO_VEC_PUSH(&result, phyto_string_own(view));
    return result;
}

phyto_string_vec_t phyto_string_split_reverse(phyto_string_view_t view, phyto_string_view_t sep) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    while (true) {
        if (!phyto_string_find_last(view, sep, &sep_pos)) {
            PHYTO_VEC_PUSH(&result, phyto_string_own(view));
            return result;
        }
        PHYTO_VEC_PUSH(&result, phyto_string_own(
                                    phyto_string_view_substr(view, sep_pos + sep.size, view.size)));
        view = phyto_string_view_substr(view, 0, sep_pos);
    }
}

phyto_string_vec_t phyto_string_split_reverse_count(phyto_string_view_t view,
                                                    phyto_string_view_t sep,
                                                    size_t count) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t sep_pos;
    while (count > 0) {
        if (!phyto_string_find_last(view, sep, &sep_pos)) {
            break;
        }
        PHYTO_VEC_PUSH(&result, phyto_string_own(
                                    phyto_string_view_substr(view, sep_pos + sep.size, view.size)));
        view = phyto_string_view_substr(view, 0, sep_pos);
        count--;
    }
    PHYTO_VEC_PUSH(&result, phyto_string_own(view));
    return result;
}

phyto_string_vec_t phyto_string_split_whitespace(phyto_string_view_t view) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    while (true) {
        phyto_string_view_t sep = phyto_string_find_whitespace(view);
        if (sep.size == 0) {
            PHYTO_VEC_PUSH(&result, phyto_string_own(view));
            return result;
        }
        PHYTO_VEC_PUSH(&result,
                       phyto_string_own(phyto_string_view_substr(view, 0, sep.begin - view.begin)));
        view = phyto_string_view_substr(view, sep.end - view.begin, view.size);
    }
}

phyto_string_view_t phyto_string_find_whitespace(phyto_string_view_t view) {
    size_t i;
    for (i = 0; i < view.size; i++) {
        if (nonstd_isspace(view.begin[i])) {
            size_t len = 1;
            while (i + len < view.size && nonstd_isspace(view.begin[i + len])) {
                len++;
            }
            return phyto_string_view_substr(view, i, len);
        }
    }
    return phyto_string_view_empty();
}

phyto_string_vec_t phyto_string_split_lines(phyto_string_view_t view) {
    phyto_string_vec_t result;
    PHYTO_VEC_INIT(&result);
    size_t newline_pos;
    while (true) {
        if (!phyto_string_find(view, phyto_string_view_from_c("\r\n"), &newline_pos) &&
            !phyto_string_find(view, phyto_string_view_from_c("\n"), &newline_pos)) {
            PHYTO_VEC_PUSH(&result, phyto_string_own(view));
            return result;
        }
        PHYTO_VEC_PUSH(&result, phyto_string_own(phyto_string_view_substr(view, 0, newline_pos)));
        view = phyto_string_view_substr(view, newline_pos + 1, view.size);
    }
}

phyto_string_t phyto_string_strip(phyto_string_view_t view) {
    size_t i;
    for (i = 0; i < view.size; i++) {
        if (!nonstd_isspace(view.begin[i])) {
            break;
        }
    }
    size_t j;
    for (j = view.size - 1; j > i; j--) {
        if (!nonstd_isspace(view.begin[j])) {
            break;
        }
    }
    return phyto_string_own(phyto_string_view_substr(view, i, j + 1));
}

phyto_string_t phyto_string_strip_chars(phyto_string_view_t view, phyto_string_view_t chars) {
    size_t i;
    for (i = 0; i < view.size; i++) {
        if (!phyto_string_contains(chars, view.begin[i])) {
            break;
        }
    }
    size_t j;
    for (j = view.size - 1; j > i; j--) {
        if (!phyto_string_contains(chars, view.begin[j])) {
            break;
        }
    }
    return phyto_string_own(phyto_string_view_substr(view, i, j + 1));
}

bool phyto_string_contains(phyto_string_view_t view, char c) {
    for (size_t i = 0; i < view.size; i++) {
        if (view.begin[i] == c) {
            return true;
        }
    }
    return false;
}

phyto_string_t phyto_string_swap_case(phyto_string_view_t view) {
    phyto_string_t result = phyto_string_new();
    for (size_t i = 0; i < view.size; i++) {
        char c = view.begin[i];
        if (nonstd_isupper(c)) {
            c = nonstd_tolower(c);
        } else if (nonstd_islower(c)) {
            c = nonstd_toupper(c);
        }
        phyto_string_push(&result, c);
    }
    return result;
}

bool phyto_string_push(phyto_string_t* string, char c) {
    return PHYTO_VEC_PUSH(string, c);
}

phyto_string_t phyto_string_title(phyto_string_view_t view) {
    phyto_string_t result = phyto_string_new();
    bool was_upper = false;
    for (size_t i = 0; i < view.size; ++i) {
        char c = view.begin[i];
        if (was_upper) {
            c = nonstd_tolower(c);
            was_upper = false;
        } else if (nonstd_isalpha(c)) {
            c = nonstd_toupper(c);
            was_upper = true;
        } else {
            was_upper = false;
        }
        phyto_string_push(&result, c);
    }
    return result;
}

phyto_string_t phyto_string_upper(phyto_string_view_t view) {
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, view.size);
    for (size_t i = 0; i < view.size; ++i) {
        phyto_string_push(&result, nonstd_toupper(view.begin[i]));
    }
    return result;
}

phyto_string_t phyto_string_zero_fill(phyto_string_view_t view, size_t width) {
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, width);
    if (view.size > 0 && (view.begin[0] == '+' || view.begin[0] == '-')) {
        phyto_string_push(&result, view.begin[0]);
        view = phyto_string_view_substr(view, 1, view.size);
    }
    phyto_string_append_fill(&result, width - view.size, '0');
    phyto_string_append_view(&result, view);
    return result;
}

void phyto_string_free(phyto_string_t* str) {
    PHYTO_VEC_FREE(str);
}

void phyto_string_vec_free(phyto_string_vec_t* vec) {
    for (size_t i = 0; i < vec->size; i++) {
        phyto_string_free(&vec->data[i]);
    }
    PHYTO_VEC_FREE(vec);
}
