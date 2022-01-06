#ifndef LOX_SCANNER_H_
#define LOX_SCANNER_H_

#include <phyto/string_view/string_view.h>
#include <stdint.h>

#include "lox/lox.h"
#include "lox/token.h"

typedef struct {
    lox_context_t* ctx;
    phyto_string_view_t source;
    lox_token_vec_t tokens;
    uint64_t start;
    uint64_t current;
    uint64_t line;
} lox_scanner_t;

lox_scanner_t lox_scanner_new(lox_context_t* ctx, phyto_string_view_t source);
lox_token_vec_t lox_scanner_scan_tokens(lox_scanner_t* scanner);
void lox_scanner_free(lox_scanner_t* scanner);

#endif  // LOX_SCANNER_H_
