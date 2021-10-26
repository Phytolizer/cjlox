#pragma once

#include <stddef.h>
#include <stdint.h>

enum object_type {
	object_null,
	object_float64,
	object_string,
};

struct object {
	enum object_type type;
	union {
		double float64;
		char *string;
	} as;
};

void object_init_null(struct object *object);
void object_init_float64(struct object *object, double value);
void object_init_string(struct object *object, char *value);
void object_deinit(struct object *object);

void object_print(struct object *object);
size_t object_query_string_length(struct object *object);
void object_print_to_string(char *str, struct object *object);
