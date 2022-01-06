#ifndef LOX_AST_PRINTER_H_
#define LOX_AST_PRINTER_H_

#include <lox/ast.h>
#include <phyto/string/string.h>

typedef struct {
    char nothing;
} lox_ast_printer_t;

LOX_EXPR_VISITOR_DECL(lox_ast_printer, phyto_string_t);

phyto_string_t lox_print_ast(lox_expr_t* expr);

#endif  // LOX_AST_PRINTER_H_
