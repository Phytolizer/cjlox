#include "lox/token.h"

#include <phyto/string/string.h>
#include <stdio.h>

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(lox_token_vec, lox_token_t);

lox_token_t lox_token_new(lox_token_type_t type,
                          phyto_string_span_t lexeme,
                          lox_object_t literal,
                          uint64_t line) {
    lox_token_t token = {
        .type = type,
        .lexeme = phyto_string_own(lexeme),
        .literal = literal,
        .line = line,
    };
    return token;
}

void lox_token_free(lox_token_t* token) {
    lox_object_free(&token->literal);
    phyto_string_free(&token->lexeme);
}

phyto_string_t lox_token_to_string(lox_token_t token) {
    phyto_string_t str = phyto_string_new();
    phyto_string_extend(&str, lox_token_type_name(token.type));
    phyto_string_append(&str, ' ');
    phyto_string_extend(&str, phyto_string_as_span(token.lexeme));
    phyto_string_append(&str, ' ');
    phyto_string_t literal = lox_object_to_string(token.literal);
    phyto_string_extend(&str, phyto_string_as_span(literal));
    phyto_string_free(&literal);
    return str;
}

void lox_token_print(lox_token_t token, FILE* stream) {
    phyto_string_t literal = lox_object_to_string(token.literal);
    fprintf(stream, "%" PHYTO_STRING_FORMAT " %" PHYTO_STRING_FORMAT " %" PHYTO_STRING_FORMAT,
            PHYTO_STRING_VIEW_PRINTF_ARGS(lox_token_type_name(token.type)),
            PHYTO_STRING_PRINTF_ARGS(token.lexeme), PHYTO_STRING_PRINTF_ARGS(literal));
    phyto_string_free(&literal);
}
