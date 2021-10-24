#include "token.h"

#include <stdio.h>
#include <stdlib.h>

void token_print(struct token *token)
{
	printf("%s %s", token_type_name(token->type), token->lexeme);
	if (token->literal.type != object_null) {
		putchar(' ');
		object_print(&token->literal);
	}
}

void token_println(struct token *token)
{
	token_print(token);
	putchar('\n');
}

void token_list_init(struct token_list *token_list)
{
	token_list->data = NULL;
	token_list->count = 0;
	token_list->capacity = 0;
}

void token_list_deinit(struct token_list *token_list)
{
	for (size_t i = 0; i < token_list->count; ++i) {
		free(token_list->data[i].lexeme);
		object_deinit(&token_list->data[i].literal);
	}
	free(token_list->data);
}

void token_list_append(struct token_list *token_list, struct token token)
{
	token_list->data =
		realloc(token_list->data,
			(token_list->count + 1) * sizeof(struct token));
	token_list->data[token_list->count] = token;
	++token_list->count;
}
