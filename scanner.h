#pragma once

#include "lox.h"
#include "token.h"

struct scanner {
	const char *source;
	size_t source_length;
	struct token_list tokens;
	size_t start;
	size_t current;
	size_t line;
};

void scanner_init(struct scanner *scanner, const char *source);
void scanner_deinit(struct scanner *scanner);
struct token_list *scanner_scan_tokens(struct scanner *scanner,
				       struct lox_state *lox_state);
