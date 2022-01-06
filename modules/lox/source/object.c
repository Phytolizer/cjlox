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

static phyto_string_t stringify_nil_object(lox_object_t obj) {
    (void)obj;
    return phyto_string_from_c("nil");
}

static phyto_string_t stringify_integer_object(lox_object_t obj) {
    return phyto_string_from_sprintf("%" PRIu64, obj.integer);
}

static phyto_string_t stringify_boolean_object(lox_object_t obj) {
    return phyto_string_from_c(obj.boolean ? "true" : "false");
}

phyto_string_view_t lox_object_type_name(lox_object_type_t type) {
    return phyto_string_view_from_c(object_type_names[type]);
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
        .integer = value,
    };
    return obj;
}

lox_object_t lox_object_new_boolean(bool value) {
    lox_object_t obj = {
        .type = LOX_OBJECT_TYPE_BOOLEAN,
        .boolean = value,
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
