#ifndef LOX_OBJECT_H_
#define LOX_OBJECT_H_

#include <phyto/string/string.h>
#include <stdbool.h>
#include <stdint.h>

#define LOX_OBJECT_TYPES_XY \
    X(NIL, nil)             \
    X(INTEGER, integer)     \
    X(BOOLEAN, boolean)     \
    X(STRING, string)       \
    X(DOUBLE, double)

typedef enum {
#define X(name, y) LOX_OBJECT_TYPE_##name,
    LOX_OBJECT_TYPES_XY
#undef X
} lox_object_type_t;

typedef struct {
    lox_object_type_t type;
    union {
        int64_t integer_value;
        bool boolean_value;
        phyto_string_t string_value;
        double double_value;
    };
} lox_object_t;

phyto_string_span_t lox_object_type_name(lox_object_type_t type);

lox_object_t lox_object_new_nil(void);
lox_object_t lox_object_new_integer(int64_t value);
lox_object_t lox_object_new_boolean(bool value);
lox_object_t lox_object_new_string(phyto_string_t value);
lox_object_t lox_object_new_double(double value);
void lox_object_free(lox_object_t* obj);
phyto_string_t lox_object_to_string(lox_object_t obj);
void lox_object_print(lox_object_t obj);

extern const lox_object_type_t lox_object_types[];

#endif  // LOX_OBJECT_H_
