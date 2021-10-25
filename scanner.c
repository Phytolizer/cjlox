#include "scanner.h"

#include "object.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>

static bool is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

static bool is_alpha_numeric(char c)
{
	return is_alpha(c) || is_digit(c);
}

static bool scanner_is_at_end(struct scanner *scanner)
{
	return scanner->current >= scanner->source_length;
}

static char scanner_peek(struct scanner *scanner)
{
	if (scanner_is_at_end(scanner)) {
		return '\0';
	}
	return scanner->source[scanner->current];
}

static char scanner_peek_next(struct scanner *scanner)
{
	if (scanner->current + 1 >= scanner->source_length) {
		return '\0';
	}
	return scanner->source[scanner->current + 1];
}

static char scanner_advance(struct scanner *scanner)
{
	char advanced = scanner->source[scanner->current];
	++scanner->current;
	return advanced;
}

static bool scanner_match(struct scanner *scanner, char expected)
{
	if (scanner_is_at_end(scanner)) {
		return false;
	}
	if (scanner->source[scanner->current] != expected) {
		return false;
	}
	++scanner->current;
	return true;
}

static void scanner_add_token_with_literal(struct scanner *scanner,
					   enum token_type token_type,
					   struct object literal)
{
	size_t toklen = scanner->current - scanner->start;
	char *lexeme = malloc(toklen + 1);
	strncpy(lexeme, &scanner->source[scanner->start], toklen);
	lexeme[toklen] = '\0';
	struct token token;
	token.type = token_type;
	token.lexeme = lexeme;
	token.line = scanner->line;
	token.literal = literal;
	token_list_append(&scanner->tokens, token);
}

static void scanner_add_token(struct scanner *scanner,
			      enum token_type token_type)
{
	struct object null_obj;
	object_init_null(&null_obj);
	scanner_add_token_with_literal(scanner, token_type, null_obj);
}

static void scanner_scan_string(struct scanner *scanner,
				struct lox_state *lox_state)
{
	while (scanner_peek(scanner) != '"' && !scanner_is_at_end(scanner)) {
		if (scanner_peek(scanner) == '\n') {
			++scanner->line;
		}
		scanner_advance(scanner);
	}

	if (scanner_is_at_end(scanner)) {
		lox_error(lox_state, scanner->line, "Unterminated string.");
		return;
	}

	scanner_advance(scanner);

	size_t valuelen = scanner->current - scanner->start - 2;
	char *value = malloc(valuelen + 1);
	strncpy(value, &scanner->source[scanner->start + 1], valuelen);
	value[valuelen] = '\0';
	struct object string_obj;
	object_init_string(&string_obj, value);
	scanner_add_token_with_literal(scanner, token_string, string_obj);
}

static void scanner_scan_number(struct scanner *scanner)
{
	while (is_digit(scanner_peek(scanner))) {
		scanner_advance(scanner);
	}
	if (scanner_peek(scanner) == '.' &&
	    is_digit(scanner_peek_next(scanner))) {
		scanner_advance(scanner);

		while (is_digit(scanner_peek(scanner))) {
			scanner_advance(scanner);
		}
	}

	struct object value_obj;
	object_init_float64(&value_obj,
			    strtod(&scanner->source[scanner->start], NULL));
	scanner_add_token_with_literal(scanner, token_number, value_obj);
}

static void scanner_scan_identifier(struct scanner *scanner)
{
	while (is_alpha_numeric(scanner_peek(scanner))) {
		scanner_advance(scanner);
	}

	size_t textlen = scanner->current - scanner->start;
	char *text = malloc(textlen + 1);
	strncpy(text, &scanner->source[scanner->start], textlen);
	text[textlen] = '\0';
	enum token_type type =
		keyword_table_look_up(&scanner->keyword_table, text);
	free(text);
	scanner_add_token(scanner, type);
}

static void scanner_scan_token(struct scanner *scanner,
			       struct lox_state *lox_state)
{
	char c = scanner_advance(scanner);
	switch (c) {
	case '(':
		scanner_add_token(scanner, token_left_paren);
		break;
	case ')':
		scanner_add_token(scanner, token_right_paren);
		break;
	case '{':
		scanner_add_token(scanner, token_left_brace);
		break;
	case '}':
		scanner_add_token(scanner, token_right_brace);
		break;
	case ',':
		scanner_add_token(scanner, token_comma);
		break;
	case '.':
		scanner_add_token(scanner, token_dot);
		break;
	case '-':
		scanner_add_token(scanner, token_minus);
		break;
	case '+':
		scanner_add_token(scanner, token_plus);
		break;
	case ';':
		scanner_add_token(scanner, token_semicolon);
		break;
	case '*':
		scanner_add_token(scanner, token_star);
		break;
	case '!':
		if (scanner_match(scanner, '=')) {
			scanner_add_token(scanner, token_bang_equal);
		} else {
			scanner_add_token(scanner, token_bang);
		}
		break;
	case '=':
		if (scanner_match(scanner, '=')) {
			scanner_add_token(scanner, token_equal_equal);
		} else {
			scanner_add_token(scanner, token_equal);
		}
		break;
	case '<':
		if (scanner_match(scanner, '=')) {
			scanner_add_token(scanner, token_less_equal);
		} else {
			scanner_add_token(scanner, token_less);
		}
		break;
	case '>':
		if (scanner_match(scanner, '=')) {
			scanner_add_token(scanner, token_greater_equal);
		} else {
			scanner_add_token(scanner, token_greater);
		}
		break;
	case '/':
		if (scanner_match(scanner, '/')) {
			while (scanner_peek(scanner) != '\n' &&
			       !scanner_is_at_end(scanner)) {
				scanner_advance(scanner);
			}
		} else {
			scanner_add_token(scanner, token_slash);
		}
		break;
	case ' ':
	case '\r':
	case '\t':
		break;
	case '\n':
		++scanner->line;
		break;
	case '"':
		scanner_scan_string(scanner, lox_state);
		break;
	default:
		if (is_digit(c)) {
			scanner_scan_number(scanner);
		} else if (is_alpha(c)) {
			scanner_scan_identifier(scanner);
		} else {
			lox_error(lox_state, scanner->line,
				  "Unexpected character.");
		}
		break;
	}
}

void scanner_init(struct scanner *scanner, const char *source)
{
	scanner->source = source;
	scanner->source_length = strlen(source);
	token_list_init(&scanner->tokens);
	scanner->start = 0;
	scanner->current = 0;
	scanner->line = 1;
	keyword_table_init(&scanner->keyword_table);
	keyword_table_insert(&scanner->keyword_table, "and", token_and_kw);
	keyword_table_insert(&scanner->keyword_table, "class", token_class_kw);
	keyword_table_insert(&scanner->keyword_table, "else", token_else_kw);
	keyword_table_insert(&scanner->keyword_table, "false", token_false_kw);
	keyword_table_insert(&scanner->keyword_table, "for", token_for_kw);
	keyword_table_insert(&scanner->keyword_table, "fun", token_fun_kw);
	keyword_table_insert(&scanner->keyword_table, "if", token_if_kw);
	keyword_table_insert(&scanner->keyword_table, "nil", token_nil_kw);
	keyword_table_insert(&scanner->keyword_table, "or", token_or_kw);
	keyword_table_insert(&scanner->keyword_table, "print", token_print_kw);
	keyword_table_insert(&scanner->keyword_table, "return",
			     token_return_kw);
	keyword_table_insert(&scanner->keyword_table, "super", token_super_kw);
	keyword_table_insert(&scanner->keyword_table, "this", token_this_kw);
	keyword_table_insert(&scanner->keyword_table, "true", token_true_kw);
	keyword_table_insert(&scanner->keyword_table, "var", token_var_kw);
	keyword_table_insert(&scanner->keyword_table, "while", token_while_kw);
}

void scanner_deinit(struct scanner *scanner)
{
	token_list_deinit(&scanner->tokens);
	keyword_table_deinit(&scanner->keyword_table);
}

struct token_list *scanner_scan_tokens(struct scanner *scanner,
				       struct lox_state *lox_state)
{
	while (!scanner_is_at_end(scanner)) {
		scanner->start = scanner->current;
		scanner_scan_token(scanner, lox_state);
	}

	struct token eof_token;
	eof_token.type = token_eof;
	eof_token.lexeme = malloc(1);
	eof_token.lexeme[0] = '\0';
	eof_token.line = scanner->line;
	object_init_null(&eof_token.literal);
	token_list_append(&scanner->tokens, eof_token);
	return &scanner->tokens;
}
