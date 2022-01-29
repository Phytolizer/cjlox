#include "lox/parser.h"

#include "lox/ast.h"

typedef struct {
    bool success;
    union {
        lox_expr_t* expression;
        struct {
            lox_token_t token;
            const char* message;
        };
    };
} parse_result_t;

static parse_result_t parse_success(lox_expr_t* expression);
static parse_result_t parse_failure(lox_token_t token, const char* message);

static parse_result_t expression(lox_parser_t* parser);
static parse_result_t equality(lox_parser_t* parser);
static parse_result_t comparison(lox_parser_t* parser);
static parse_result_t term(lox_parser_t* parser);
static parse_result_t factor(lox_parser_t* parser);
static parse_result_t unary(lox_parser_t* parser);
static parse_result_t primary(lox_parser_t* parser);

static bool match(lox_parser_t* parser, ...);
static bool check(lox_parser_t* parser, lox_token_type_t type);
static lox_token_t advance(lox_parser_t* parser);
static lox_token_t* peek(lox_parser_t* parser);
static lox_token_t previous(lox_parser_t* parser);
static bool is_at_end(lox_parser_t* parser);
static parse_result_t consume(lox_parser_t* parser, lox_token_type_t type, const char* message);
static parse_result_t error(lox_parser_t* parser, lox_token_t token, const char* message);
static void synchronize(lox_parser_t* parser);

#define MATCH(parser, ...) match(parser, __VA_ARGS__, lox_token_type_eof)

lox_parser_t lox_parser_new(lox_context_t* ctx, lox_token_vec_t tokens) {
    return (lox_parser_t){
        .ctx = ctx,
        .tokens = tokens,
        .current = 0,
    };
}

lox_expr_t* lox_parser_parse(lox_parser_t* parser) {
    parse_result_t result = expression(parser);
    if (result.success) {
        return result.expression;
    }
    return NULL;
}

parse_result_t parse_success(lox_expr_t* expression) {
    return (parse_result_t){
        .success = true,
        .expression = expression,
    };
}
parse_result_t parse_failure(lox_token_t token, const char* message) {
    return (parse_result_t){
        .success = false,
        .token = token,
        .message = message,
    };
}

parse_result_t expression(lox_parser_t* parser) {
    return equality(parser);
}

parse_result_t equality(lox_parser_t* parser) {
    parse_result_t left_result = comparison(parser);
    if (!left_result.success) {
        return left_result;
    }
    lox_expr_t* left = left_result.expression;

    while (MATCH(parser, lox_token_type_bang_equal, lox_token_type_equal_equal)) {
        lox_token_t oper = previous(parser);
        parse_result_t right_result = comparison(parser);
        if (!right_result.success) {
            lox_expr_free(left);
            return right_result;
        }
        left = (lox_expr_t*)lox_expr_new_binary(left, oper, right_result.expression);
    }

    return parse_success(left);
}

parse_result_t comparison(lox_parser_t* parser) {
    parse_result_t left_result = term(parser);
    if (!left_result.success) {
        return left_result;
    }
    lox_expr_t* left = left_result.expression;

    while (MATCH(parser, lox_token_type_greater, lox_token_type_greater_equal, lox_token_type_less,
                 lox_token_type_less_equal)) {
        lox_token_t oper = previous(parser);
        parse_result_t right_result = term(parser);
        if (!right_result.success) {
            lox_expr_free(left);
            return right_result;
        }
        left = (lox_expr_t*)lox_expr_new_binary(left, oper, right_result.expression);
    }

    return parse_success(left);
}

parse_result_t term(lox_parser_t* parser) {
    parse_result_t left_result = factor(parser);
    if (!left_result.success) {
        return left_result;
    }
    lox_expr_t* left = left_result.expression;

    while (MATCH(parser, lox_token_type_minus, lox_token_type_plus)) {
        lox_token_t oper = previous(parser);
        parse_result_t right_result = factor(parser);
        if (!right_result.success) {
            lox_expr_free(left);
            return right_result;
        }
        left = (lox_expr_t*)lox_expr_new_binary(left, oper, right_result.expression);
    }

    return parse_success(left);
}

parse_result_t factor(lox_parser_t* parser) {
    parse_result_t left_result = unary(parser);
    if (!left_result.success) {
        return left_result;
    }
    lox_expr_t* left = left_result.expression;

    while (MATCH(parser, lox_token_type_slash, lox_token_type_star)) {
        lox_token_t oper = previous(parser);
        parse_result_t right_result = unary(parser);
        if (!right_result.success) {
            lox_expr_free(left);
            return right_result;
        }
        left = (lox_expr_t*)lox_expr_new_binary(left, oper, right_result.expression);
    }

    return parse_success(left);
}

parse_result_t unary(lox_parser_t* parser) {
    if (MATCH(parser, lox_token_type_bang, lox_token_type_minus)) {
        lox_token_t oper = previous(parser);
        parse_result_t right_result = unary(parser);
        if (!right_result.success) {
            return right_result;
        }
        return parse_success((lox_expr_t*)lox_expr_new_unary(oper, right_result.expression));
    }

    return primary(parser);
}

parse_result_t primary(lox_parser_t* parser) {
    if (MATCH(parser, lox_token_type_kw_false)) {
        return parse_success((lox_expr_t*)lox_expr_new_literal(lox_object_new_boolean(false)));
    }
    if (MATCH(parser, lox_token_type_kw_true)) {
        return parse_success((lox_expr_t*)lox_expr_new_literal(lox_object_new_boolean(true)));
    }
    if (MATCH(parser, lox_token_type_kw_nil)) {
        return parse_success((lox_expr_t*)lox_expr_new_literal(lox_object_new_nil()));
    }
    if (MATCH(parser, lox_token_type_number, lox_token_type_string)) {
        return parse_success((lox_expr_t*)lox_expr_new_literal(previous(parser).literal));
    }
    if (MATCH(parser, lox_token_type_left_paren)) {
        parse_result_t expr_result = expression(parser);
        if (!expr_result.success) {
            return expr_result;
        }
        parse_result_t consume_result =
            consume(parser, lox_token_type_right_paren, "Expect ')' after expression.");
        if (!consume_result.success) {
            return consume_result;
        }
        return parse_success((lox_expr_t*)lox_expr_new_grouping(expr_result.expression));
    }

    return error(parser, *peek(parser), "Expect expression.");
}

bool match(lox_parser_t* parser, ...) {
    va_list args;
    bool result = false;
    va_start(args, parser);
    for (lox_token_type_t type = va_arg(args, lox_token_type_t); type != lox_token_type_eof;
         type = va_arg(args, lox_token_type_t)) {
        if (check(parser, type)) {
            advance(parser);
            result = true;
        }
    }
    va_end(args);
    return result;
}
bool check(lox_parser_t* parser, lox_token_type_t type) {
    if (is_at_end(parser)) {
        return false;
    }
    return peek(parser)->type == type;
}

lox_token_t advance(lox_parser_t* parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
    return previous(parser);
}

lox_token_t* peek(lox_parser_t* parser) {
    return &parser->tokens.data[parser->current];
}

lox_token_t previous(lox_parser_t* parser) {
    return parser->tokens.data[parser->current - 1];
}
bool is_at_end(lox_parser_t* parser) {
    return peek(parser)->type == lox_token_type_eof;
}
parse_result_t consume(lox_parser_t* parser, lox_token_type_t type, const char* message) {
    if (check(parser, type)) {
        advance(parser);
        return parse_success(NULL);
    }

    return error(parser, *peek(parser), message);
}

parse_result_t error(lox_parser_t* parser, lox_token_t token, const char* message) {
    if (token.type == lox_token_type_eof) {
        lox_report(parser->ctx, token.line, phyto_string_span_from_c(" at end"),
                   phyto_string_span_from_c(message));
    } else {
        phyto_string_t where =
            phyto_string_from_sprintf("at '%" PHYTO_STRING_FORMAT "'", token.lexeme);
        lox_report(parser->ctx, token.line, phyto_string_as_span(where),
                   phyto_string_span_from_c(message));
        phyto_string_free(&where);
    }
    return parse_failure(token, message);
}

void synchronize(lox_parser_t* parser) {
    advance(parser);

    while (!is_at_end(parser)) {
        if (previous(parser).type == lox_token_type_semicolon) {
            return;
        }

        switch (peek(parser)->type) {
            case lox_token_type_kw_class:
            case lox_token_type_kw_fun:
            case lox_token_type_kw_var:
            case lox_token_type_kw_for:
            case lox_token_type_kw_if:
            case lox_token_type_kw_while:
            case lox_token_type_kw_print:
            case lox_token_type_kw_return:
                return;
            default:
                break;
        }

        advance(parser);
    }
}
