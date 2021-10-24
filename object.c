#include "object.h"

#include <stdio.h>

void object_init_null(struct object *object)
{
	object->type = object_null;
}

void object_init_int32(struct object *object, int32_t value)
{
	object->type = object_int32;
	object->as.int32 = value;
}

void object_print(struct object *object)
{
	switch (object->type) {
	case object_null:
		break;
	case object_int32:
		printf("%d", object->as.int32);
	}
}
