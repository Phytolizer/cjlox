#include "lox/ast_printer.h"

#include <phyto/string/string.h>

#include "lox/object.h"

LOX_EXPR_VISITOR_IMPL(lox, ast_printer, phyto_string_t);

phyto_string_t lox_print_ast(lox_expr_t* expr) {
    lox_ast_printer_t printer = {0};
    return lox_expr_accept_ast_printer(expr, &printer);
}

LOX_EXPR_VISITOR_VISIT_BINARY_FUNC(lox, ast_printer, phyto_string_t) {
    phyto_string_t left = lox_expr_accept_ast_printer(node->left, visitor);
    phyto_string_t right = lox_expr_accept_ast_printer(node->right, visitor);
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, left.size + right.size + node->op.lexeme.size + 4);
    phyto_string_push(&result, '(');
    phyto_string_append_view(&result, phyto_string_view(node->op.lexeme));
    phyto_string_push(&result, ' ');
    phyto_string_append_view(&result, phyto_string_view(left));
    phyto_string_free(&left);
    phyto_string_push(&result, ' ');
    phyto_string_append_view(&result, phyto_string_view(right));
    phyto_string_free(&right);
    phyto_string_push(&result, ')');
    return result;
}

LOX_EXPR_VISITOR_VISIT_GROUPING_FUNC(lox, ast_printer, phyto_string_t) {
    phyto_string_t expr = lox_expr_accept_ast_printer(node->expression, visitor);
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, expr.size + 8);
    phyto_string_append_c(&result, "(group ");
    phyto_string_append_view(&result, phyto_string_view(expr));
    phyto_string_free(&expr);
    phyto_string_push(&result, ')');
    return result;
}

LOX_EXPR_VISITOR_VISIT_LITERAL_FUNC(lox, ast_printer, phyto_string_t) {
    (void)visitor;
    return lox_object_to_string(node->value);
}

LOX_EXPR_VISITOR_VISIT_UNARY_FUNC(lox, ast_printer, phyto_string_t) {
    phyto_string_t expr = lox_expr_accept_ast_printer(node->right, visitor);
    phyto_string_t result = phyto_string_new();
    phyto_string_reserve(&result, expr.size + node->op.lexeme.size + 4);
    phyto_string_push(&result, '(');
    phyto_string_append_view(&result, phyto_string_view(node->op.lexeme));
    phyto_string_push(&result, ' ');
    phyto_string_append_view(&result, phyto_string_view(expr));
    phyto_string_free(&expr);
    phyto_string_push(&result, ')');
    return result;
}
