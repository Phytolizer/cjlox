#ifndef LOX_LOX_H_
#define LOX_LOX_H_

#include <phyto/string/string.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool had_error;
} lox_context_t;

int32_t lox_run_file(lox_context_t* ctx, const char* filename);
void lox_run_prompt(lox_context_t* ctx);
void lox_error(lox_context_t* ctx, uint64_t line, phyto_string_span_t message);

#endif  // LOX_LOX_H_
