#include "parser.h"

#include "expr.h"
#include "hedley.h"
#include "main.h"
#include "token_type.h"

#include <stdbool.h>

typedef struct ExprResult
{
    Expr *expr;
    Token token;
    sds message;
} ExprResult;

#define EXPR_ERROR ((ExprResult){NULL})

typedef struct StmtResult
{
    Stmt *stmt;
    Token token;
    sds message;
} StmtResult;

#define STMT_ERROR ((StmtResult){NULL})

typedef struct ConsumeResult
{
    Token token;
    bool was_error;
} ConsumeResult;

static Stmt *declaration(Parser *parser);
static StmtResult var_declaration(Parser *parser);
static StmtResult statement(Parser *parser);
static StmtResult print_statement(Parser *parser);
static StmtResult expression_statement(Parser *parser);

static ExprResult expression(Parser *parser);
static ExprResult equality(Parser *parser);
static ExprResult comparison(Parser *parser);
static ExprResult term(Parser *parser);
static ExprResult factor(Parser *parser);
static ExprResult unary(Parser *parser);
static ExprResult primary(Parser *parser);

Parser new_parser(Token *tokens)
{
    Parser parser = {tokens, 0};
    return parser;
}

static Token peek(Parser *parser)
{
    return parser->tokens[parser->current];
}

static Token previous(Parser *parser)
{
    return parser->tokens[parser->current - 1];
}

static bool is_at_end(Parser *parser)
{
    return peek(parser).type == TOKEN_EOF;
}

static Token advance(Parser *parser)
{
    if (!is_at_end(parser))
    {
        ++parser->current;
    }
    return previous(parser);
}

static bool check(Parser *parser, TokenType type)
{
    if (is_at_end(parser))
    {
        return false;
    }
    return parser->tokens[parser->current].type == type;
}

static bool match(Parser *parser, ...)
{
    va_list args;
    va_start(args, parser);
    while (true)
    {
        TokenType type = va_arg(args, TokenType);
        if (type == TOKEN_INVALID)
        {
            break;
        }
        if (check(parser, type))
        {
            advance(parser);
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}

#define match(parser, ...) match(parser, __VA_ARGS__, TOKEN_INVALID)
HEDLEY_PRINTF_FORMAT(2, 0)
static void vparse_error(Token token, char const *fmt, va_list args);

HEDLEY_PRINTF_FORMAT(2, 3)
static void parse_error(Token token, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vparse_error(token, fmt, args);
    va_end(args);
}

static void vparse_error(Token token, char const *fmt, va_list args)
{
    verror_token(token, fmt, args);
}

HEDLEY_PRINTF_FORMAT(3, 4)
static ConsumeResult consume(Parser *parser, TokenType type, char const *fmt, ...)
{
    if (check(parser, type))
    {
        return (ConsumeResult){advance(parser), NULL};
    }

    va_list args;
    va_start(args, fmt);
    vparse_error(peek(parser), fmt, args);
    va_end(args);
    return (ConsumeResult){peek(parser), true};
}

#define consume(err, parser, type, ...)                                                                                \
    ({                                                                                                                 \
        ConsumeResult result = consume(parser, type, __VA_ARGS__);                                                     \
        if (result.was_error)                                                                                          \
        {                                                                                                              \
            return err;                                                                                                \
        };                                                                                                             \
        result.token;                                                                                                  \
    })

#define check_result(call)                                                                                             \
    ({                                                                                                                 \
        StmtResult result = call;                                                                                      \
        if (result.stmt == NULL)                                                                                       \
        {                                                                                                              \
            return result;                                                                                             \
        }                                                                                                              \
        result.stmt;                                                                                                   \
    })

#define check_expr(call)                                                                                               \
    ({                                                                                                                 \
        ExprResult result = call;                                                                                      \
        if (result.expr == NULL)                                                                                       \
        {                                                                                                              \
            return (StmtResult){NULL, result.token, result.message};                                                   \
        }                                                                                                              \
        result.expr;                                                                                                   \
    })

#define ok(value)                                                                                                      \
    ({                                                                                                                 \
        StmtResult result = {value, peek(parser), NULL};                                                               \
        result;                                                                                                        \
    })

static void synchronize(Parser *parser)
{
    advance(parser);

    while (!is_at_end(parser))
    {
        if (previous(parser).type == TOKEN_SEMICOLON)
        {
            return;
        }

        switch (peek(parser).type)
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;
        default:;
        }

        advance(parser);
    }
}

static Stmt *declaration(Parser *parser)
{
    if (match(parser, TOKEN_VAR))
    {
        StmtResult result = var_declaration(parser);
        if (result.stmt == NULL)
        {
            synchronize(parser);
        }
        return result.stmt;
    }
    StmtResult result = statement(parser);
    if (result.stmt == NULL)
    {
        synchronize(parser);
    }
    return result.stmt;
}

static StmtResult var_declaration(Parser *parser)
{
    Token name = consume(STMT_ERROR, parser, TOKEN_IDENTIFIER, "Expect variable name.");

    Expr *initializer = NULL;
    if (match(parser, TOKEN_EQUAL))
    {
        initializer = check_expr(expression(parser));
    }

    consume(STMT_ERROR, parser, TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    return ok(new_var_stmt(name, initializer));
}

static StmtResult statement(Parser *parser)
{
    if (match(parser, TOKEN_PRINT))
    {
        return print_statement(parser);
    }
    return expression_statement(parser);
}

static StmtResult print_statement(Parser *parser)
{
    Expr *value = check_expr(expression(parser));
    consume(STMT_ERROR, parser, TOKEN_SEMICOLON, "Expect ';' after value.");
    return ok(new_print_stmt(value));
}

static StmtResult expression_statement(Parser *parser)
{
    Expr *expr = check_expr(expression(parser));
    consume(STMT_ERROR, parser, TOKEN_SEMICOLON, "Expect ';' after expression.");
    return ok(new_expression_stmt(expr));
}

#undef ok
#undef check_result

#define check_result(call)                                                                                             \
    ({                                                                                                                 \
        ExprResult result = call;                                                                                      \
        if (result.expr == NULL)                                                                                       \
        {                                                                                                              \
            return result;                                                                                             \
        }                                                                                                              \
        result.expr;                                                                                                   \
    })

#define ok(value)                                                                                                      \
    ({                                                                                                                 \
        ExprResult result = {value, peek(parser), NULL};                                                               \
        result;                                                                                                        \
    })

static ExprResult expression(Parser *parser)
{
    return equality(parser);
}

static ExprResult equality(Parser *parser)
{
    Expr *expr = check_result(comparison(parser));
    while (match(parser, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL))
    {
        Token op = previous(parser);
        Expr *right = check_result(comparison(parser));
        expr = new_binary_expr(expr, op, right);
    }

    return ok(expr);
}

static ExprResult comparison(Parser *parser)
{
    Expr *expr = check_result(term(parser));
    while (match(parser, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL))
    {
        Token op = previous(parser);
        Expr *right = check_result(term(parser));
        expr = new_binary_expr(expr, op, right);
    }

    return ok(expr);
}

static ExprResult term(Parser *parser)
{
    Expr *expr = check_result(factor(parser));
    while (match(parser, TOKEN_MINUS, TOKEN_PLUS))
    {
        Token op = previous(parser);
        Expr *right = check_result(factor(parser));
        expr = new_binary_expr(expr, op, right);
    }

    return ok(expr);
}

static ExprResult factor(Parser *parser)
{
    Expr *expr = check_result(unary(parser));
    while (match(parser, TOKEN_SLASH, TOKEN_STAR))
    {
        Token op = previous(parser);
        Expr *right = check_result(unary(parser));
        expr = new_binary_expr(expr, op, right);
    }

    return ok(expr);
}

static ExprResult unary(Parser *parser)
{
    if (match(parser, TOKEN_BANG, TOKEN_MINUS))
    {
        Token op = previous(parser);
        Expr *right = check_result(unary(parser));
        return ok(new_unary_expr(op, right));
    }

    return primary(parser);
}

static ExprResult primary(Parser *parser)
{
    if (match(parser, TOKEN_FALSE))
    {
        return ok(new_literal_expr(new_boolean_object(false)));
    }
    if (match(parser, TOKEN_TRUE))
    {
        return ok(new_literal_expr(new_boolean_object(true)));
    }
    if (match(parser, TOKEN_NIL))
    {
        return ok(new_literal_expr(NULL));
    }

    if (match(parser, TOKEN_NUMBER, TOKEN_STRING))
    {
        return ok(new_literal_expr(previous(parser).literal));
    }

    if (match(parser, TOKEN_IDENTIFIER))
    {
        return ok(new_variable_expr(previous(parser)));
    }

    if (match(parser, TOKEN_LEFT_PAREN))
    {
        Expr *expr = check_result(expression(parser));
        consume(EXPR_ERROR, parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
        return ok(new_grouping_expr(expr));
    }
    parse_error(peek(parser), "Expect expression.");
    return EXPR_ERROR;
}

Stmt **parser_parse(Parser *parser)
{
    Stmt **statements = NULL;
    while (!is_at_end(parser))
    {
        arrput(statements, declaration(parser));
    }

    return statements;
}
