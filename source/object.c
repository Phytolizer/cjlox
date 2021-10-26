#include "lox/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	case object_boolean:
		break;
	case object_string:
		free(object->as.string);
	}
}

void object_init_boolean(struct object *object, bool value)
{
	object->type = object_boolean;
	object->as.boolean = value;
}

void object_init_object(struct object *object, struct object *value)
{
	switch (value->type) {
	case object_null:
		object->type = object_null;
		break;
	case object_float64:
		object->type = object_float64;
		object->as.float64 = value->as.float64;
		break;
	case object_string:
		object->type = object_string;
		object->as.string = malloc(strlen(value->as.string) + 1);
		strcpy(object->as.string, value->as.string);
		break;
	case object_boolean:
		object->type = object_boolean;
		object->as.boolean = value->as.boolean;
		break;
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
	case object_boolean:
		printf("%s", object->as.boolean ? "true" : "false");
		break;
	}
}

size_t object_query_string_length(struct object *object)
{
	switch (object->type) {
	case object_null:
		return sizeof("null") - 1;
	case object_float64:
		return snprintf(NULL, 0, "%lf", object->as.float64);
	case object_string:
		return strlen(object->as.string);
	case object_boolean:
		return object->as.boolean ? 4 : 5;
	}
	return 0;
}

void object_print_to_string(char *str, struct object *object)
{
	switch (object->type) {
	case object_null:
		sprintf(str, "null");
		break;
	case object_float64:
		sprintf(str, "%lf", object->as.float64);
		break;
	case object_string:
		strcpy(str, object->as.string);
		break;
	case object_boolean:
		strcpy(str, object->as.boolean ? "true" : "false");
		break;
	}
}
