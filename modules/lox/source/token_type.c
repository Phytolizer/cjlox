#include "lox/token_type.h"

static const char* const token_type_names[] = {
#define X(x) #x,
    LOX_TOKEN_TYPES_X
#undef X
};

const lox_token_type_t lox_token_types[] = {
#define X(x) lox_token_type_##x,
    LOX_TOKEN_TYPES_X
#undef X
};

phyto_string_span_t lox_token_type_name(lox_token_type_t type) {
    return phyto_string_span_from_c(token_type_names[type]);
}

bool lox_token_type_print_to(FILE* fp, lox_token_type_t type) {
    phyto_string_span_t name = lox_token_type_name(type);
    phyto_string_span_print_to(name, fp);
    return true;
}

int32_t lox_token_type_cmp(lox_token_type_t a, lox_token_type_t b) {
    return (int32_t)a - (int32_t)b;
}
