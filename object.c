#include "object.h"

#include <stdio.h>
#include <stdlib.h>

void object_init_null(struct object *object)
{
	object->type = object_null;
}

void object_init_float64(struct object *object, double value)
{
	object->type = object_float64;
	object->as.float64 = value;
}

void object_init_string(struct object *object, char *value)
{
	object->type = object_string;
	object->as.string = value;
}

void object_deinit(struct object *object)
{
	switch (object->type) {
	case object_null:
	case object_float64:
		break;
	case object_string:
		free(object->as.string);
	}
}

void object_print(struct object *object)
{
	switch (object->type) {
	case object_null:
		break;
	case object_float64:
		printf("%lf", object->as.float64);
		break;
	case object_string:
		printf("%s", object->as.string);
		break;
	}
}
