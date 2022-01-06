#include "lox/scanner.h"

#include <stdlib.h>

#include "lox/object.h"
#include "lox/token_type.h"
#include "phyto/string_view/string_view.h"

static bool is_at_end(lox_scanner_t* scanner) {
    return scanner->current >= scanner->source.size;
}

static char advance(lox_scanner_t* scanner) {
    scanner->current++;
    return scanner->source.begin[scanner->current - 1];
}

static void add_token_literal(lox_scanner_t* scanner, lox_token_type_t type, lox_object_t literal) {
    lox_token_t token = lox_token_new(
        type, phyto_string_view_substr(scanner->source, scanner->start, scanner->current), literal,
        scanner->line);
    PHYTO_VEC_PUSH(&scanner->tokens, token);
}

static void add_token(lox_scanner_t* scanner, lox_token_type_t type) {
    add_token_literal(scanner, type, lox_object_new_nil());
}

static void scan_token(lox_scanner_t* scanner) {
    char c = advance(scanner);
    switch (c) {
        case '(':
            add_token(scanner, LOX_TOKEN_TYPE_LEFT_PAREN);
            break;
        case ')':
            add_token(scanner, LOX_TOKEN_TYPE_RIGHT_PAREN);
            break;
        case '{':
            add_token(scanner, LOX_TOKEN_TYPE_LEFT_BRACE);
            break;
        case '}':
            add_token(scanner, LOX_TOKEN_TYPE_RIGHT_BRACE);
            break;
        case ',':
            add_token(scanner, LOX_TOKEN_TYPE_COMMA);
            break;
        case '.':
            add_token(scanner, LOX_TOKEN_TYPE_DOT);
            break;
        case '-':
            add_token(scanner, LOX_TOKEN_TYPE_MINUS);
            break;
        case '+':
            add_token(scanner, LOX_TOKEN_TYPE_PLUS);
            break;
        case ';':
            add_token(scanner, LOX_TOKEN_TYPE_SEMICOLON);
            break;
        case '*':
            add_token(scanner, LOX_TOKEN_TYPE_STAR);
            break;
    }
}

lox_scanner_t lox_scanner_new(lox_context_t* ctx, phyto_string_view_t source) {
    lox_scanner_t scanner = {
        .ctx = ctx,
        .source = source,
        .tokens = {0},
        .start = 0,
        .current = 0,
        .line = 1,
    };
    PHYTO_VEC_INIT(&scanner.tokens);
    return scanner;
}

lox_token_vec_t lox_scanner_scan_tokens(lox_scanner_t* scanner) {
    while (!is_at_end(scanner)) {
        scanner->start = scanner->current;
        scan_token(scanner);
    }

    PHYTO_VEC_PUSH(&scanner->tokens, lox_token_new(LOX_TOKEN_TYPE_EOF, phyto_string_view_empty(),
                                                   lox_object_new_nil(), scanner->line));
    return scanner->tokens;
}

void lox_scanner_free(lox_scanner_t* scanner) {
    for (size_t i = 0; i < scanner->tokens.size; i++) {
        lox_token_free(&scanner->tokens.data[i]);
    }
    PHYTO_VEC_FREE(&scanner->tokens);
}
