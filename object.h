#pragma once

#include <stdint.h>

enum object_type {
	object_null,
	object_int32,
};

struct object {
	enum object_type type;
	union {
		int32_t int32;
	} as;
};

void object_init_null(struct object *object);
void object_init_int32(struct object *object, int32_t value);

void object_print(struct object *object);
