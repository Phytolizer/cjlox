#include <lox/ast_printer.h>
#include <lox/object.h>
#include <lox/token.h>
#include <lox/token_type.h>
#include <phyto/string_view/string_view.h>
#include <stdlib.h>

int main(void) {
    lox_expr_t* expr = (lox_expr_t*)lox_expr_new_binary(
        (lox_expr_t*)lox_expr_new_unary(
            lox_token_new(lox_token_type_minus, phyto_string_view_from_c("-"), lox_object_new_nil(),
                          1),
            (lox_expr_t*)lox_expr_new_literal(lox_object_new_double(123))),
        lox_token_new(lox_token_type_star, phyto_string_view_from_c("*"), lox_object_new_nil(), 1),
        (lox_expr_t*)lox_expr_new_grouping(
            (lox_expr_t*)lox_expr_new_literal(lox_object_new_double(45.67))));
    phyto_string_t str = lox_print_ast(expr);
    phyto_string_view_print_to(phyto_string_view(str), stdout);
    printf("\n");
    phyto_string_free(&str);
    lox_expr_free(expr);
    return 0;
}
