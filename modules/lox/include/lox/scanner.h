#ifndef LOX_SCANNER_H_
#define LOX_SCANNER_H_

#include <phyto/hash/hash.h>
#include <phyto/string_view/string_view.h>
#include <stdint.h>

#include "lox/lox.h"
#include "lox/token.h"
#include "lox/token_type.h"

PHYTO_HASH_DECL(lox_scanner_keyword_map, lox_token_type_t);

typedef struct {
    lox_context_t* ctx;
    phyto_string_view_t source;
    lox_token_vec_t tokens;
    uint64_t start;
    uint64_t current;
    uint64_t line;
    lox_scanner_keyword_map_t* keywords;
} lox_scanner_t;

lox_scanner_t lox_scanner_new(lox_context_t* ctx, phyto_string_view_t source);
lox_token_vec_t lox_scanner_scan_tokens(lox_scanner_t* scanner);
void lox_scanner_free(lox_scanner_t* scanner);

#endif  // LOX_SCANNER_H_
