#include "lox/keyword_table.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KEYWORD_TABLE_INITIAL_BUCKET_COUNT 8
#define KEYWORD_TABLE_MAXIMUM_LOAD 0.75

#define KEYWORD_TABLE_FNV_OFFSET_BASIS 0xCBF29CE484222325
#define KEYWORD_TABLE_FNV_PRIME 0x00000100000001B3

static uint64_t keyword_table_hash_string(const char *key)
{
	uint64_t hashval = KEYWORD_TABLE_FNV_OFFSET_BASIS;
	for (size_t i = 0; key[i] != '\0'; ++i) {
		hashval ^= (unsigned char)key[i];
		hashval *= KEYWORD_TABLE_FNV_PRIME;
	}
	return hashval;
}

static struct keyword_table_bucket *
keyword_table_locate_bucket(struct keyword_table_bucket *buckets,
			    size_t bucket_count, const char *key)
{
	uint64_t hashval = keyword_table_hash_string(key) % bucket_count;
	while (buckets[hashval].key != NULL &&
	       strcmp(buckets[hashval].key, key) != 0) {
		hashval = (hashval + 1) % bucket_count;
	}
	return &buckets[hashval];
}

static void keyword_table_rehash(struct keyword_table *keyword_table)
{
	size_t old_bucket_count = keyword_table->bucket_count;
	keyword_table->bucket_count *= 2;
	struct keyword_table_bucket *new_buckets =
		malloc(keyword_table->bucket_count *
		       sizeof(struct keyword_table_bucket));
	for (size_t i = 0; i < keyword_table->bucket_count; ++i) {
		new_buckets[i].key = NULL;
	}
	for (size_t i = 0; i < old_bucket_count; ++i) {
		if (keyword_table->buckets[i].key == NULL) {
			continue;
		}

		struct keyword_table_bucket *new_bucket =
			keyword_table_locate_bucket(
				new_buckets, keyword_table->bucket_count,
				keyword_table->buckets[i].key);
		new_bucket->key = keyword_table->buckets[i].key;
		new_bucket->value = keyword_table->buckets[i].value;
	}

	free(keyword_table->buckets);
	keyword_table->buckets = new_buckets;
}

void keyword_table_init(struct keyword_table *keyword_table)
{
	keyword_table->buckets = malloc(KEYWORD_TABLE_INITIAL_BUCKET_COUNT *
					sizeof(struct keyword_table_bucket));
	for (size_t i = 0; i < KEYWORD_TABLE_INITIAL_BUCKET_COUNT; ++i) {
		keyword_table->buckets[i].key = NULL;
	}
	keyword_table->bucket_count = KEYWORD_TABLE_INITIAL_BUCKET_COUNT;
	keyword_table->element_count = 0;
}

void keyword_table_deinit(struct keyword_table *keyword_table)
{
	free(keyword_table->buckets);
}

void keyword_table_insert(struct keyword_table *keyword_table, char *key,
			  enum token_type value)
{
	if ((double)keyword_table->element_count /
		    (double)keyword_table->bucket_count >
	    KEYWORD_TABLE_MAXIMUM_LOAD) {
		keyword_table_rehash(keyword_table);
	}
	struct keyword_table_bucket *bucket = keyword_table_locate_bucket(
		keyword_table->buckets, keyword_table->bucket_count, key);
	bucket->key = key;
	bucket->value = value;
	++keyword_table->element_count;
}

enum token_type keyword_table_look_up(struct keyword_table *keyword_table,
				      const char *key)
{
	struct keyword_table_bucket *bucket = keyword_table_locate_bucket(
		keyword_table->buckets, keyword_table->bucket_count, key);
	if (bucket->key == NULL) {
		return token_identifier;
	}
	return bucket->value;
}
