#include "scanner.h"

#include "object.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>

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
	default:
		lox_error(lox_state, scanner->line, "Unexpected character.");
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
}

void scanner_deinit(struct scanner *scanner)
{
	token_list_deinit(&scanner->tokens);
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
