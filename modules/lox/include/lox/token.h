#ifndef LOX_TOKEN_H_
#define LOX_TOKEN_H_

#include <phyto/string/string.h>
#include <stdio.h>

#include "lox/object.h"
#include "lox/token_type.h"

typedef struct {
    lox_token_type_t type;
    phyto_string_t lexeme;
    lox_object_t literal;
    uint64_t line;
} lox_token_t;

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(lox_token_vec, lox_token_t);

lox_token_t lox_token_new(lox_token_type_t type,
                          phyto_string_span_t lexeme,
                          lox_object_t literal,
                          uint64_t line);
void lox_token_free(lox_token_t* token);
phyto_string_t lox_token_to_string(lox_token_t token);
void lox_token_print(lox_token_t token, FILE* stream);

#endif  // LOX_TOKEN_H_
