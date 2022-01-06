#include "lox/scanner.h"

#include <stdbool.h>
#include <stdlib.h>

#include "lox/lox.h"
#include "lox/object.h"
#include "lox/token_type.h"
#include "nonstd/ctype.h"
#include "phyto/string_view/string_view.h"

PHYTO_HASH_IMPL(lox_scanner_keyword_map, lox_token_type_t);

static const lox_scanner_keyword_map_key_ops_t key_ops = {
    .hash = phyto_hash_djb2,
};

static const lox_scanner_keyword_map_value_ops_t value_ops = {
    .compare = lox_token_type_cmp,
    .print = lox_token_type_print_to,
};

static bool is_at_end(lox_scanner_t* scanner) {
    return scanner->current >= scanner->source.size;
}

static char peek(lox_scanner_t* scanner) {
    if (is_at_end(scanner)) {
        return '\0';
    }
    return scanner->source.begin[scanner->current];
}

static char peek_next(lox_scanner_t* scanner) {
    if (scanner->current + 1 >= scanner->source.size) {
        return '\0';
    }
    return scanner->source.begin[scanner->current + 1];
}

static char advance(lox_scanner_t* scanner) {
    scanner->current++;
    return scanner->source.begin[scanner->current - 1];
}

static bool match(lox_scanner_t* scanner, char expected) {
    if (is_at_end(scanner)) {
        return false;
    }

    if (scanner->source.begin[scanner->current] != expected) {
        return false;
    }

    scanner->current++;
    return true;
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

static void string(lox_scanner_t* scanner) {
    while (peek(scanner) != '"' && !is_at_end(scanner)) {
        if (peek(scanner) == '\n') {
            scanner->line++;
        }
        advance(scanner);
    }

    if (is_at_end(scanner)) {
        lox_error(scanner->ctx, scanner->line, phyto_string_view_from_c("Unterminated string."));
        return;
    }

    advance(scanner);
    phyto_string_t value = phyto_string_own(
        phyto_string_view_substr(scanner->source, scanner->start + 1, scanner->current - 1));
    add_token_literal(scanner, LOX_TOKEN_TYPE_STRING, lox_object_new_string(value));
}

static void number(lox_scanner_t* scanner) {
    while (nonstd_isdigit(peek(scanner))) {
        advance(scanner);
    }

    if (peek(scanner) == '.' && nonstd_isdigit(peek_next(scanner))) {
        advance(scanner);

        while (nonstd_isdigit(peek(scanner))) {
            advance(scanner);
        }
    }

    add_token_literal(scanner, LOX_TOKEN_TYPE_NUMBER,
                      lox_object_new_double(strtod(scanner->source.begin + scanner->start, NULL)));
}

static void identifier(lox_scanner_t* scanner) {
    while (nonstd_isalnum(peek(scanner)) || peek(scanner) == '_') {
        advance(scanner);
    }

    phyto_string_view_t value =
        phyto_string_view_substr(scanner->source, scanner->start, scanner->current);
    lox_token_type_t* type = lox_scanner_keyword_map_get_ref(scanner->keywords, value);
    if (type != NULL) {
        add_token(scanner, *type);
    } else {
        add_token(scanner, LOX_TOKEN_TYPE_IDENTIFIER);
    }
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
        case '!':
            add_token(scanner,
                      match(scanner, '=') ? LOX_TOKEN_TYPE_BANG_EQUAL : LOX_TOKEN_TYPE_BANG);
            break;
        case '=':
            add_token(scanner,
                      match(scanner, '=') ? LOX_TOKEN_TYPE_EQUAL_EQUAL : LOX_TOKEN_TYPE_EQUAL);
            break;
        case '<':
            add_token(scanner,
                      match(scanner, '=') ? LOX_TOKEN_TYPE_LESS_EQUAL : LOX_TOKEN_TYPE_LESS);
            break;
        case '>':
            add_token(scanner,
                      match(scanner, '=') ? LOX_TOKEN_TYPE_GREATER_EQUAL : LOX_TOKEN_TYPE_GREATER);
            break;
        case '/':
            if (match(scanner, '/')) {
                while (peek(scanner) != '\n' && !is_at_end(scanner)) {
                    advance(scanner);
                }
            } else {
                add_token(scanner, LOX_TOKEN_TYPE_SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            scanner->line++;
            break;
        case '"':
            string(scanner);
            break;
        default:
            if (nonstd_isdigit(c)) {
                number(scanner);
            } else if (nonstd_isalpha(c) || c == '_') {
                identifier(scanner);
            } else {
                lox_error(scanner->ctx, scanner->line,
                          phyto_string_view_from_c("Unexpected character."));
            }
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
        .keywords = lox_scanner_keyword_map_new(20, phyto_hash_default_load, &key_ops, &value_ops),
    };
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("and"),
                                   LOX_TOKEN_TYPE_AND);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("class"),
                                   LOX_TOKEN_TYPE_CLASS);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("else"),
                                   LOX_TOKEN_TYPE_ELSE);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("false"),
                                   LOX_TOKEN_TYPE_FALSE);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("for"),
                                   LOX_TOKEN_TYPE_FOR);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("fun"),
                                   LOX_TOKEN_TYPE_FUN);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("if"),
                                   LOX_TOKEN_TYPE_IF);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("nil"),
                                   LOX_TOKEN_TYPE_NIL);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("or"),
                                   LOX_TOKEN_TYPE_OR);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("print"),
                                   LOX_TOKEN_TYPE_PRINT);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("return"),
                                   LOX_TOKEN_TYPE_RETURN);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("super"),
                                   LOX_TOKEN_TYPE_SUPER);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("this"),
                                   LOX_TOKEN_TYPE_THIS);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("true"),
                                   LOX_TOKEN_TYPE_TRUE);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("var"),
                                   LOX_TOKEN_TYPE_VAR);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_view_from_c("while"),
                                   LOX_TOKEN_TYPE_WHILE);
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
    lox_scanner_keyword_map_free(scanner->keywords);
    scanner->keywords = NULL;
}
