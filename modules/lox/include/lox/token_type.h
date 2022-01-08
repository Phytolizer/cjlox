#ifndef LOX_TOKEN_TYPE_H_
#define LOX_TOKEN_TYPE_H_

#include <phyto/string/string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LOX_TOKEN_TYPES_X \
    X(left_paren)         \
    X(right_paren)        \
    X(left_brace)         \
    X(right_brace)        \
    X(comma)              \
    X(dot)                \
    X(minus)              \
    X(plus)               \
    X(semicolon)          \
    X(slash)              \
    X(star)               \
                          \
    X(bang)               \
    X(bang_equal)         \
    X(equal)              \
    X(equal_equal)        \
    X(greater)            \
    X(greater_equal)      \
    X(less)               \
    X(less_equal)         \
                          \
    X(identifier)         \
    X(string)             \
    X(number)             \
                          \
    X(kw_and)             \
    X(kw_class)           \
    X(kw_else)            \
    X(kw_false)           \
    X(kw_for)             \
    X(kw_fun)             \
    X(kw_if)              \
    X(kw_nil)             \
    X(kw_or)              \
    X(kw_print)           \
    X(kw_return)          \
    X(kw_super)           \
    X(kw_this)            \
    X(kw_true)            \
    X(kw_var)             \
    X(kw_while)           \
                          \
    X(eof)

typedef enum {
#define X(x) lox_token_type_##x,
    LOX_TOKEN_TYPES_X
#undef X
} lox_token_type_t;

phyto_string_span_t lox_token_type_name(lox_token_type_t type);
void lox_token_type_print_to(FILE* fp, lox_token_type_t type);
int32_t lox_token_type_cmp(lox_token_type_t a, lox_token_type_t b);

extern const lox_token_type_t lox_token_types[];

#endif  // LOX_TOKEN_TYPE_H_
