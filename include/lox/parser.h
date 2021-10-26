#pragma once

#include "lox/token.h"
struct parser {
	struct lox_state *lox_state;
	struct token_list *tokens;
	size_t current;
};

void parser_init(struct parser *parser, struct lox_state *lox_state,
		 struct token_list *tokens);
struct expr *parser_parse(struct parser *parser);
