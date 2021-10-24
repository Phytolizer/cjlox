#pragma once

enum token_type {
	token_left_paren,
	token_right_paren,
	token_left_brace,
	token_right_brace,
	token_comma,
	token_dot,
	token_minus,
	token_plus,
	token_semicolon,
	token_slash,
	token_star,

	token_bang,
	token_bang_equal,
	token_equal,
	token_equal_equal,
	token_greater,
	token_greater_equal,
	token_less,
	token_less_equal,

	token_identifier,
	token_string,
	token_number,

	token_and_kw,
	token_class_kw,
	token_else_kw,
	token_false_kw,
	token_for_kw,
	token_fun_kw,
	token_if_kw,
	token_nil_kw,
	token_or_kw,
	token_print_kw,
	token_return_kw,
	token_super_kw,
	token_this_kw,
	token_true_kw,
	token_var_kw,
	token_while_kw,

	token_eof,
};

const char *token_type_name(enum token_type token_type);
