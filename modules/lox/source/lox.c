#include "lox/lox.h"

#include <inttypes.h>
#include <phyto/io/io.h>
#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits/sysexits.h>

#include "lox/scanner.h"
#include "lox/token.h"

static void run(lox_context_t* ctx, phyto_string_view_t source);
static void report(lox_context_t* ctx,
                   uint64_t line,
                   phyto_string_view_t where,
                   phyto_string_view_t message);

int32_t lox_run_file(lox_context_t* ctx, const char* filename) {
    phyto_string_t source = phyto_io_read_file(filename);
    if (source.size == 0) {
        fprintf(stderr, "Could not read file: %s\n", filename);
        return EX_NOINPUT;
    }
    run(ctx, phyto_string_view(source));
    if (ctx->had_error) {
        return EX_DATAERR;
    }
    return EX_OK;
}

void lox_run_prompt(lox_context_t* ctx) {
    while (true) {
        printf("> ");
        phyto_string_t source = phyto_io_read_line(stdin);
        if (source.size == 0) {
            break;
        }
        run(ctx, phyto_string_view(source));
        ctx->had_error = false;
    }
}

void lox_error(lox_context_t* ctx, uint64_t line, phyto_string_view_t message) {
    report(ctx, line, phyto_string_view_empty(), message);
}

void run(lox_context_t* ctx, phyto_string_view_t source) {
    lox_scanner_t scanner = lox_scanner_new(ctx, source);
    lox_token_vec_t tokens = lox_scanner_scan_tokens(&scanner);

    for (size_t i = 0; i < tokens.size; ++i) {
        printf("%" PHYTO_STRING_FORMAT "\n", PHYTO_STRING_PRINTF_ARGS(tokens.data[i].lexeme));
    }
    PHYTO_VEC_FREE(&tokens);
    lox_scanner_free(&scanner);
}

void report(lox_context_t* ctx,
            uint64_t line,
            phyto_string_view_t where,
            phyto_string_view_t message) {
    fprintf(stderr, "[line %" PRIu64 "] Error %" PHYTO_STRING_FORMAT ": %" PHYTO_STRING_FORMAT "\n",
            line, PHYTO_STRING_VIEW_PRINTF_ARGS(where), PHYTO_STRING_VIEW_PRINTF_ARGS(message));
    ctx->had_error = true;
}
