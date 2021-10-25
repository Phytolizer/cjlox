#pragma once

#include "token_type.h"
#include <stddef.h>

struct keyword_table_bucket {
	char *key;
	enum token_type value;
};

struct keyword_table {
	struct keyword_table_bucket *buckets;
	size_t bucket_count;
	size_t element_count;
};

void keyword_table_init(struct keyword_table *keyword_table);
void keyword_table_deinit(struct keyword_table *keyword_table);
void keyword_table_insert(struct keyword_table *keyword_table, char *key,
			  enum token_type value);
enum token_type keyword_table_look_up(struct keyword_table *keyword_table,
				      const char *key);
