#include "object.h"

#include <stdlib.h>

Object *new_number_object(double value)
{
    NumberObject *object = malloc(sizeof(NumberObject));
    object->base.type = OBJECT_NUMBER;
    object->value = value;
    return (Object *)object;
}

Object *new_string_object(sds value)
{
    StringObject *object = malloc(sizeof(StringObject));
    object->base.type = OBJECT_STRING;
    object->value = value;
    return (Object *)object;
}

Object *new_boolean_object(bool value)
{
    BooleanObject *object = malloc(sizeof(BooleanObject));
    object->base.type = OBJECT_BOOLEAN;
    object->value = value;
    return (Object *)object;
}

void print_object(Object *object, FILE *stream)
{
    if (object != NULL)
    {
        switch (object->type)
        {
        case OBJECT_NUMBER:
            fprintf(stream, "%g", ((NumberObject *)object)->value);
            break;
        case OBJECT_STRING:
            fprintf(stream, "%s", ((StringObject *)object)->value);
            break;
        case OBJECT_BOOLEAN:
            fprintf(stream, "%s", ((BooleanObject *)object)->value ? "true" : "false");
            break;
        default:
            break;
        }
    }
}

sds object_to_string(Object *object)
{
    if (object != NULL)
    {
        switch (object->type)
        {
        case OBJECT_NUMBER:
            return sdscatprintf(sdsempty(), "%g", ((NumberObject *)object)->value);
        case OBJECT_STRING:
            return sdsnew(((StringObject *)object)->value);
        case OBJECT_BOOLEAN:
            return sdsnew(((BooleanObject *)object)->value ? "true" : "false");
        default:
            break;
        }
    }
    return NULL;
}
