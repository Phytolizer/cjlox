#pragma once

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

struct lox_state {
	bool had_error;
};

void lox_init(struct lox_state *lox_state);
void lox_error(struct lox_state *lox_state, size_t line, const char *message);
void lox_error_at_token(struct lox_state *lox_state, struct token *token,
			const char *message);
