#include "token_type.h"

const char *token_type_name(enum token_type token_type)
{
	switch (token_type) {
	case token_left_paren:
		return "left_paren";
	case token_right_paren:
		return "right_paren";
	case token_left_brace:
		return "left_brace";
	case token_right_brace:
		return "right_brace";
	case token_comma:
		return "comma";
	case token_dot:
		return "dot";
	case token_minus:
		return "minus";
	case token_plus:
		return "plus";
	case token_semicolon:
		return "semicolon";
	case token_slash:
		return "slash";
	case token_star:
		return "star";
	case token_bang:
		return "bang";
	case token_bang_equal:
		return "bang_equal";
	case token_equal:
		return "equal";
	case token_equal_equal:
		return "equal_equal";
	case token_greater:
		return "greater";
	case token_greater_equal:
		return "greater_equal";
	case token_less:
		return "less";
	case token_less_equal:
		return "less_equal";
	case token_identifier:
		return "identifier";
	case token_string:
		return "string";
	case token_number:
		return "number";
	case token_and_kw:
		return "and_kw";
	case token_class_kw:
		return "class_kw";
	case token_else_kw:
		return "else_kw";
	case token_false_kw:
		return "false_kw";
	case token_for_kw:
		return "for_kw";
	case token_fun_kw:
		return "fun_kw";
	case token_if_kw:
		return "if_kw";
	case token_nil_kw:
		return "nil_kw";
	case token_or_kw:
		return "or_kw";
	case token_print_kw:
		return "print_kw";
	case token_return_kw:
		return "return_kw";
	case token_super_kw:
		return "super_kw";
	case token_this_kw:
		return "this_kw";
	case token_true_kw:
		return "true_kw";
	case token_var_kw:
		return "var_kw";
	case token_while_kw:
		return "while_kw";
	case token_eof:
		return "eof";
	default:
		return "<INVALID TOKEN>";
	}
}
