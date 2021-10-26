#include "lox/expr_destructors.h"

#include "lox/object.h"
#include "tool/ast.h"
#include <stdlib.h>

void expr_free(struct expr *expr)
{
	switch (expr->type) {
	case expr_type_binary: {
		struct binary_expr *b = (struct binary_expr *)expr;
		expr_free(b->left);
		expr_free(b->right);
		break;
	}
	case expr_type_grouping: {
		struct grouping_expr *g = (struct grouping_expr *)expr;
		expr_free(g->expression);
		break;
	}
	case expr_type_literal: {
		struct literal_expr *l = (struct literal_expr *)expr;
		object_deinit(&l->value);
		break;
	}
	case expr_type_unary: {
		struct unary_expr *u = (struct unary_expr *)expr;
		expr_free(u->right);
		break;
	}
	}
	free(expr);
}
