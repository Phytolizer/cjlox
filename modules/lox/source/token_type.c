#include "lox/token_type.h"

static const char* const token_type_names[] = {
#define X(x) #x,
    LOX_TOKEN_TYPES_X
#undef X
};

const lox_token_type_t lox_token_types[] = {
#define X(x) LOX_TOKEN_TYPE_##x,
    LOX_TOKEN_TYPES_X
#undef X
};

phyto_string_view_t lox_token_type_name(lox_token_type_t type) {
    return phyto_string_view_from_c(token_type_names[type]);
}
