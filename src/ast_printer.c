#include "ast_printer.h"

#include "expr.h"
#include "object.h"

static sds parenthesize(ExprVisitor *visitor, char const *name, ...)
{
    va_list args;
    va_start(args, name);

    sds result = sdsempty();
    result = sdscatfmt(result, "(%s", name);
    Expr *arg = va_arg(args, Expr *);
    while (arg != NULL)
    {
        result = sdscatfmt(result, " %s", expr_accept(arg, visitor, NULL));
        arg = va_arg(args, Expr *);
    }
    result = sdscat(result, ")");

    va_end(args);

    return result;
}

static void *visit_binary_expr(ExprVisitor *visitor, void *data, BinaryExpr *expr)
{
    return parenthesize(visitor, expr->operator.lexeme, expr->left, expr->right, NULL);
}

static void *visit_grouping_expr(ExprVisitor *visitor, void *data, GroupingExpr *expr)
{
    return parenthesize(visitor, "group", expr->expression, NULL);
}

static void *visit_literal_expr(ExprVisitor *visitor, void *data, LiteralExpr *expr)
{
    sds result = object_to_string(expr->value);
    if (result == NULL)
    {
        return sdsnew("nil");
    }
    return result;
}

static void *visit_unary_expr(ExprVisitor *visitor, void *data, UnaryExpr *expr)
{
    return parenthesize(visitor, expr->operator.lexeme, expr->right, NULL);
}

AstPrinter ast_printer_new(void)
{
    AstPrinter printer = {.visitor = {
                              .visit_binary_expr = visit_binary_expr,
                              .visit_grouping_expr = visit_grouping_expr,
                              .visit_literal_expr = visit_literal_expr,
                              .visit_unary_expr = visit_unary_expr,
                          }};
    return printer;
}

sds ast_printer_print(AstPrinter printer, Expr *expr)
{
    return expr_accept(expr, &printer.visitor, NULL);
}
