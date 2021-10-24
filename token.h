#pragma once

#include "object.h"
#include "token_type.h"
#include <stddef.h>

struct token {
	enum token_type type;
	char *lexeme;
	struct object literal;
	size_t line;
};

struct token_list {
	struct token *data;
	size_t count;
	size_t capacity;
};

void token_print(struct token *token);
void token_println(struct token *token);

void token_list_init(struct token_list *token_list);
void token_list_deinit(struct token_list *token_list);
void token_list_append(struct token_list *token_list, struct token token);
