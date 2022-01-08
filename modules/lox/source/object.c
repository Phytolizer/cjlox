#include "lox/object.h"

#include <assert.h>
#include <inttypes.h>
#include <phyto/string/string.h>

static const char* const object_type_names[] = {
#define X(x, y) #x,
    LOX_OBJECT_TYPES_XY
#undef X
};

const lox_object_type_t lox_object_types[] = {
#define X(name, y) LOX_OBJECT_TYPE_##name,
    LOX_OBJECT_TYPES_XY
#undef X
};

static void free_nil_object(lox_object_t* obj) {
    (void)obj;
}

static void free_integer_object(lox_object_t* obj) {
    (void)obj;
}

static void free_boolean_object(lox_object_t* obj) {
    (void)obj;
}

static void free_string_object(lox_object_t* obj) {
    phyto_string_free(&obj->string_value);
}

static void free_double_object(lox_object_t* obj) {
    (void)obj;
}

static phyto_string_t stringify_nil_object(lox_object_t obj) {
    (void)obj;
    return phyto_string_from_c("nil");
}

static phyto_string_t stringify_integer_object(lox_object_t obj) {
    return phyto_string_from_sprintf("%" PRIu64, obj.integer_value);
}

static phyto_string_t stringify_boolean_object(lox_object_t obj) {
    return phyto_string_from_c(obj.boolean_value ? "true" : "false");
}

static phyto_string_t stringify_string_object(lox_object_t obj) {
    return phyto_string_copy(obj.string_value);
}

static phyto_string_t stringify_double_object(lox_object_t obj) {
    return phyto_string_from_sprintf("%g", obj.double_value);
}

phyto_string_span_t lox_object_type_name(lox_object_type_t type) {
    return phyto_string_span_from_c(object_type_names[type]);
}

lox_object_t lox_object_new_nil(void) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_NIL,
    };
    return obj;
}

lox_object_t lox_object_new_integer(int64_t value) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_INTEGER,
        .integer_value = value,
    };
    return obj;
}

lox_object_t lox_object_new_boolean(bool value) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_BOOLEAN,
        .boolean_value = value,
    };
    return obj;
}

lox_object_t lox_object_new_string(phyto_string_t value) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_STRING,
        .string_value = value,
    };
    return obj;
}

lox_object_t lox_object_new_double(double value) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_DOUBLE,
        .double_value = value,
    };
    return obj;
}

void lox_object_free(lox_object_t* obj) {
    switch (obj->type) {
#define X(x, y)                 \
    case LOX_OBJECT_TYPE_##x:   \
        free_##y##_object(obj); \
        break;
        LOX_OBJECT_TYPES_XY
#undef X
    }
}

phyto_string_t lox_object_to_string(lox_object_t obj) {
    switch (obj.type) {
#define X(x, y)               \
    case LOX_OBJECT_TYPE_##x: \
        return stringify_##y##_object(obj);
        LOX_OBJECT_TYPES_XY
#undef X
    }

    assert(false && "corrupt object type");
    return phyto_string_new();
}
