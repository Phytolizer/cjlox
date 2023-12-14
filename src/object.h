#pragma once

#include "sds.h"

#include <stdbool.h>
#include <stdio.h>

#define OBJECT_TYPES_X                                                                                                 \
    X(NUMBER)                                                                                                          \
    X(STRING)                                                                                                          \
    X(BOOLEAN)

typedef enum ObjectType
{
#define X(x) OBJECT_##x,
    OBJECT_TYPES_X
#undef X
} ObjectType;

typedef struct Object
{
    ObjectType type;
} Object;

typedef struct IntegerObject
{
    Object base;
    double value;
} NumberObject;

typedef struct StringObject
{
    Object base;
    sds value;
} StringObject;

typedef struct BooleanObject
{
    Object base;
    bool value;
} BooleanObject;

Object *new_number_object(double value);
Object *new_string_object(sds value);
Object *new_boolean_object(bool value);

void print_object(Object *object, FILE *stream);
sds object_to_string(Object *object);
