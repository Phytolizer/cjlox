#pragma once

#include <sds.h>
#include <stdio.h>

#define TOKEN_TYPES_X                                                                                                  \
    /* Single-character tokens. */                                                                                     \
    X(LEFT_PAREN)                                                                                                      \
    X(RIGHT_PAREN)                                                                                                     \
    X(LEFT_BRACE)                                                                                                      \
    X(RIGHT_BRACE)                                                                                                     \
    X(COMMA)                                                                                                           \
    X(DOT)                                                                                                             \
    X(MINUS)                                                                                                           \
    X(PLUS)                                                                                                            \
    X(SEMICOLON)                                                                                                       \
    X(SLASH)                                                                                                           \
    X(STAR)                                                                                                            \
    /* One or two character tokens. */                                                                                 \
    X(BANG)                                                                                                            \
    X(BANG_EQUAL)                                                                                                      \
    X(EQUAL)                                                                                                           \
    X(EQUAL_EQUAL)                                                                                                     \
    X(LESS)                                                                                                            \
    X(LESS_EQUAL)                                                                                                      \
    X(GREATER)                                                                                                         \
    X(GREATER_EQUAL)                                                                                                   \
    /* Literals. */                                                                                                    \
    X(IDENTIFIER)                                                                                                      \
    X(STRING)                                                                                                          \
    X(NUMBER)                                                                                                          \
    /* Keywords. */                                                                                                    \
    X(AND)                                                                                                             \
    X(CLASS)                                                                                                           \
    X(ELSE)                                                                                                            \
    X(FALSE)                                                                                                           \
    X(FOR)                                                                                                             \
    X(FUN)                                                                                                             \
    X(IF)                                                                                                              \
    X(NIL)                                                                                                             \
    X(OR)                                                                                                              \
    X(PRINT)                                                                                                           \
    X(RETURN)                                                                                                          \
    X(SUPER)                                                                                                           \
    X(THIS)                                                                                                            \
    X(TRUE)                                                                                                            \
    X(VAR)                                                                                                             \
    X(WHILE)                                                                                                           \
    X(EOF)

typedef enum TokenType
{
    TOKEN_INVALID = -1,
#define X(x) TOKEN_##x,
    TOKEN_TYPES_X
#undef X
} TokenType;

void print_token_type(TokenType type, FILE *stream);
