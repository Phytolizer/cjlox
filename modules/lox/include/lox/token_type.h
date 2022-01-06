#ifndef LOX_TOKEN_TYPE_H_
#define LOX_TOKEN_TYPE_H_

#include <phyto/string_view/string_view.h>

#define LOX_TOKEN_TYPES_X \
    X(LEFT_PAREN)         \
    X(RIGHT_PAREN)        \
    X(LEFT_BRACE)         \
    X(RIGHT_BRACE)        \
    X(COMMA)              \
    X(DOT)                \
    X(MINUS)              \
    X(PLUS)               \
    X(SEMICOLON)          \
    X(SLASH)              \
    X(STAR)               \
                          \
    X(BANG)               \
    X(BANG_EQUAL)         \
    X(EQUAL)              \
    X(EQUAL_EQUAL)        \
    X(GREATER)            \
    X(GREATER_EQUAL)      \
    X(LESS)               \
    X(LESS_EQUAL)         \
                          \
    X(IDENTIFIER)         \
    X(STRING)             \
    X(NUMBER)             \
                          \
    X(AND)                \
    X(CLASS)              \
    X(ELSE)               \
    X(FALSE)              \
    X(FOR)                \
    X(FUN)                \
    X(IF)                 \
    X(NIL)                \
    X(OR)                 \
    X(PRINT)              \
    X(RETURN)             \
    X(SUPER)              \
    X(THIS)               \
    X(TRUE)               \
    X(VAR)                \
    X(WHILE)              \
                          \
    X(EOF)

typedef enum {
#define X(x) LOX_TOKEN_TYPE_##x,
    LOX_TOKEN_TYPES_X
#undef X
} lox_token_type_t;

phyto_string_view_t lox_token_type_name(lox_token_type_t type);

extern const lox_token_type_t lox_token_types[];

#endif  // LOX_TOKEN_TYPE_H_
