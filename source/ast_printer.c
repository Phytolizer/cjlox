#include "lox/ast_printer.h"

#include "lox/object.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *parenthesize(struct ast_printer *ast_printer, const char *name,
			  size_t children, struct expr *arg, ...);

char *ast_printer_visit_binary_expr(struct ast_printer *ast_printer,
				    struct binary_expr *expr)
{
	return parenthesize(ast_printer, expr->operator.lexeme, 2, expr->left,
			    expr->right);
}

char *ast_printer_visit_grouping_expr(struct ast_printer *ast_printer,
				      struct grouping_expr *expr)
{
	return parenthesize(ast_printer, "group", 1, expr->expression);
}

char *ast_printer_visit_literal_expr(struct ast_printer *ast_printer,
				     struct literal_expr *expr)
{
	(void)ast_printer;
	size_t len = object_query_string_length(&expr->value);
	char *result = malloc(len + 1);
	object_print_to_string(result, &expr->value);
	result[len] = '\0';
	return result;
}

char *ast_printer_visit_unary_expr(struct ast_printer *ast_printer,
				   struct unary_expr *expr)
{
	return parenthesize(ast_printer, expr->operator.lexeme, 1, expr->right);
}

static char *parenthesize(struct ast_printer *ast_printer, const char *name,
			  size_t children, struct expr *arg, ...)
{
	char **args_visited = malloc(children * sizeof(char *));
	va_list args;
	va_start(args, arg);
	size_t args_len = 0;
	for (size_t i = 0; i < children; ++i) {
		struct expr *child = i == 0 ? arg : va_arg(args, struct expr *);
		args_visited[i] = expr_accept_ast_printer(child, ast_printer);
		args_len += strlen(args_visited[i]);
		if (i < children - 1) {
			// for ' '
			++args_len;
		}
	}
	//                '('  name          ' '  args      ')'
	size_t total_len = 1 + strlen(name) + 1 + args_len + 1;
	va_end(args);
	char *result = malloc(total_len + 1);
	sprintf(result, "(%s", name);
	char *cursor = &result[1 + strlen(name)];
	for (size_t i = 0; i < children; ++i) {
		cursor += sprintf(cursor, " ");
		strcpy(cursor, args_visited[i]);
		cursor += strlen(args_visited[i]);
	}
	sprintf(cursor, ")");
	cursor[1] = '\0';
	for (size_t i = 0; i < children; ++i) {
		free(args_visited[i]);
	}
	free(args_visited);
	return result;
}
