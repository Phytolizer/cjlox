#pragma once

#include <tool/ast.h>

struct ast_printer {
	// the ast_printer has no state, but
	// empty structs are an extension
	char nothing;
};

char *ast_printer_visit_binary_expr(struct ast_printer *ast_printer,
				    struct binary_expr *expr);
char *ast_printer_visit_grouping_expr(struct ast_printer *ast_printer,
				      struct grouping_expr *expr);
char *ast_printer_visit_literal_expr(struct ast_printer *ast_printer,
				     struct literal_expr *expr);
char *ast_printer_visit_unary_expr(struct ast_printer *ast_printer,
				   struct unary_expr *expr);
