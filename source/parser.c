#include "lox/parser.h"

#include "lox/lox.h"
#include "lox/token_type.h"
#include "tool/ast.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct parse_error {
	struct token *token;
	char *message;
};

enum parse_result_type {
	parse_result_ok,
	parse_result_error,
};

struct parse_result {
	enum parse_result_type type;
	union {
		struct expr *expr;
		struct parse_error error;
	} as;
};

static void parse_error_deinit(struct parse_error *parse_error);

static struct parse_result parse_expression(struct parser *parser);
static struct parse_result parse_equality(struct parser *parser);
static struct parse_result parse_comparison(struct parser *parser);
static struct parse_result parse_term(struct parser *parser);
static struct parse_result parse_factor(struct parser *parser);
static struct parse_result parse_unary(struct parser *parser);
static struct parse_result parse_primary(struct parser *parser);
static bool match(struct parser *parser, size_t n, enum token_type type1, ...);
static bool check(struct parser *parser, enum token_type type);
static struct token *advance(struct parser *parser);
static bool is_at_end(struct parser *parser);
static struct token *peek(struct parser *parser);
static struct token *previous(struct parser *parser);
static struct parse_error consume(struct parser *parser, enum token_type type,
				  const char *message);
static void synchronize(struct parser *parser);

void parser_init(struct parser *parser, struct lox_state *lox_state,
		 struct token_list *tokens)
{
	parser->lox_state = lox_state;
	parser->tokens = tokens;
	parser->current = 0;
}

struct expr *parser_parse(struct parser *parser)
{
	struct parse_result expr_result = parse_expression(parser);
	if (expr_result.type == parse_result_error) {
		parse_error_deinit(&expr_result.as.error);
		return NULL;
	}
	return expr_result.as.expr;
}

static void parse_error_deinit(struct parse_error *parse_error)
{
	free(parse_error->message);
}

static struct parse_result parse_expression(struct parser *parser)
{
	return parse_equality(parser);
}

static struct parse_result parse_equality(struct parser *parser)
{
	struct parse_result expr_result = parse_comparison(parser);
	if (expr_result.type == parse_result_error) {
		return expr_result;
	}
	struct expr *expr = expr_result.as.expr;

	while (match(parser, 2, token_bang_equal, token_equal_equal)) {
		struct token *op = previous(parser);
		struct parse_result right_result = parse_comparison(parser);
		if (right_result.type == parse_result_error) {
			return right_result;
		}
		struct expr *right = right_result.as.expr;
		expr = (struct expr *)expr_new_binary(expr, *op, right);
	}
	return (struct parse_result){ .type = parse_result_ok,
				      .as = { .expr = expr } };
}

static struct parse_result parse_comparison(struct parser *parser)
{
	struct parse_result expr_result = parse_term(parser);
	if (expr_result.type == parse_result_error) {
		return expr_result;
	}
	struct expr *expr = expr_result.as.expr;

	while (match(parser, 4, token_greater, token_greater_equal, token_less,
		     token_less_equal)) {
		struct token *op = previous(parser);
		struct parse_result right_result = parse_term(parser);
		if (right_result.type == parse_result_error) {
			return right_result;
		}
		expr = (struct expr *)expr_new_binary(expr, *op,
						      right_result.as.expr);
	}

	return (struct parse_result){ .type = parse_result_ok,
				      .as = { .expr = expr } };
}

static struct parse_result parse_term(struct parser *parser)
{
	struct parse_result expr_result = parse_factor(parser);
	if (expr_result.type == parse_result_error) {
		return expr_result;
	}
	struct expr *expr = expr_result.as.expr;

	while (match(parser, 2, token_plus, token_minus)) {
		struct token *op = previous(parser);
		struct parse_result right_result = parse_factor(parser);
		if (right_result.type == parse_result_error) {
			return right_result;
		}
		expr = (struct expr *)expr_new_binary(expr, *op,
						      right_result.as.expr);
	}

	return (struct parse_result){ .type = parse_result_ok,
				      .as = { .expr = expr } };
}

static struct parse_result parse_factor(struct parser *parser)
{
	struct parse_result expr_result = parse_unary(parser);
	if (expr_result.type == parse_result_error) {
		return expr_result;
	}
	struct expr *expr = expr_result.as.expr;

	while (match(parser, 2, token_slash, token_star)) {
		struct token *op = previous(parser);
		struct parse_result right_result = parse_unary(parser);
		if (right_result.type == parse_result_error) {
			return right_result;
		}
		expr = (struct expr *)expr_new_binary(expr, *op,
						      right_result.as.expr);
	}

	return (struct parse_result){ .type = parse_result_ok,
				      .as = { .expr = expr } };
}

static struct parse_result parse_unary(struct parser *parser)
{
	if (match(parser, 2, token_bang, token_minus)) {
		struct token *op = previous(parser);
		struct parse_result right_result = parse_unary(parser);
		if (right_result.type == parse_result_error) {
			return right_result;
		}
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_unary(
					*op, right_result.as.expr) }
		};
	}

	return parse_primary(parser);
}

static struct parse_result parse_primary(struct parser *parser)
{
	if (match(parser, 1, token_false_kw)) {
		struct object false_obj;
		object_init_boolean(&false_obj, false);
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_literal(
					false_obj) }
		};
	}
	if (match(parser, 1, token_true_kw)) {
		struct object true_obj;
		object_init_boolean(&true_obj, true);
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_literal(
					true_obj) }
		};
	}
	if (match(parser, 1, token_nil_kw)) {
		struct object null_obj;
		object_init_null(&null_obj);
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_literal(
					null_obj) }
		};
	}
	if (match(parser, 2, token_number, token_string)) {
		struct object lit;
		object_init_object(&lit, &previous(parser)->literal);
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_literal(lit) }
		};
	}
	if (match(parser, 1, token_left_paren)) {
		struct parse_result expr_result = parse_expression(parser);
		if (expr_result.type == parse_result_error) {
			return expr_result;
		}
		struct parse_error error =
			consume(parser, token_right_paren,
				"Expect ')' after expression.");
		if (error.message != NULL) {
			return (struct parse_result){
				.type = parse_result_error,
				.as = { .error = error }
			};
		}
		return (struct parse_result){
			.type = parse_result_ok,
			.as = { .expr = (struct expr *)expr_new_grouping(
					expr_result.as.expr) }
		};
	}

	lox_error_at_token(parser->lox_state, peek(parser),
			   "Expect expression.");
	char *message = malloc(sizeof("Expect expression."));
	strcpy(message, "Expect expression.");
	return (struct parse_result){ .type = parse_result_error,
				      .as = { .error = (struct parse_error){
						      .token = peek(parser),
						      .message = message } } };
}

static bool match(struct parser *parser, size_t n, enum token_type type1, ...)
{
	va_list args;
	va_start(args, type1);
	for (size_t i = 0; i < n; ++i) {
		if ((i == 0 && check(parser, type1)) ||
		    (i > 0 && check(parser, va_arg(args, enum token_type)))) {
			advance(parser);
			return true;
		}
	}
	va_end(args);

	return false;
}

static bool check(struct parser *parser, enum token_type type)
{
	if (is_at_end(parser)) {
		return false;
	}
	return peek(parser)->type == type;
}

static struct token *advance(struct parser *parser)
{
	if (!is_at_end(parser)) {
		++parser->current;
	}
	return previous(parser);
}

static bool is_at_end(struct parser *parser)
{
	return peek(parser)->type == token_eof;
}

static struct token *peek(struct parser *parser)
{
	return &parser->tokens->data[parser->current];
}

static struct token *previous(struct parser *parser)
{
	return &parser->tokens->data[parser->current - 1];
}

static struct parse_error consume(struct parser *parser, enum token_type type,
				  const char *message)
{
	if (check(parser, type)) {
		return (struct parse_error){ 0 };
	}

	lox_error_at_token(parser->lox_state, peek(parser), message);
	char *message_out = malloc(strlen(message) + 1);
	strcpy(message_out, message);
	message_out[strlen(message)] = '\0';
	return (struct parse_error){ .token = peek(parser),
				     .message = message_out };
}

static void synchronize(struct parser *parser)
{
	advance(parser);
	while (!is_at_end(parser)) {
		if (previous(parser)->type == token_semicolon) {
			return;
		}

		switch (peek(parser)->type) {
		case token_class_kw:
		case token_fun_kw:
		case token_var_kw:
		case token_for_kw:
		case token_if_kw:
		case token_while_kw:
		case token_print_kw:
		case token_return_kw:
			return;
		default:
			break;
		}

		advance(parser);
	}
}
