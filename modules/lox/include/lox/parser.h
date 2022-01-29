#ifndef LOX_PARSER_H_
#define LOX_PARSER_H_

#include "lox/lox.h"
#include "lox/token.h"
#include "lox/ast.h"

typedef struct {
    lox_context_t* ctx;
    lox_token_vec_t tokens;
    uint64_t current;
} lox_parser_t;

lox_parser_t lox_parser_new(lox_context_t* ctx, lox_token_vec_t tokens);
lox_expr_t* lox_parser_parse(lox_parser_t* parser);

#endif