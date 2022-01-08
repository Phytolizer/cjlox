#include "lox/scanner.h"

#include <nonstd/ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lox/lox.h"
#include "lox/object.h"
#include "lox/token.h"
#include "lox/token_type.h"
#include "phyto/string/string.h"

PHYTO_HASH_IMPL(lox_scanner_keyword_map, lox_token_type_t);

static const lox_token_vec_callbacks_t lox_token_vec_callbacks = {
    .free_cb = lox_token_free,
};

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
        type, phyto_string_span_subspan(scanner->source, scanner->start, scanner->current), literal,
        scanner->line);
    lox_token_vec_append(&scanner->tokens, token);
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
        lox_error(scanner->ctx, scanner->line, phyto_string_span_from_c("Unterminated string."));
        return;
    }

    advance(scanner);
    phyto_string_t value = phyto_string_own(
        phyto_string_span_subspan(scanner->source, scanner->start + 1, scanner->current - 1));
    add_token_literal(scanner, lox_token_type_string, lox_object_new_string(value));
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

    add_token_literal(scanner, lox_token_type_number,
                      lox_object_new_double(strtod(scanner->source.begin + scanner->start, NULL)));
}

static void identifier(lox_scanner_t* scanner) {
    while (nonstd_isalnum(peek(scanner)) || peek(scanner) == '_') {
        advance(scanner);
    }

    phyto_string_span_t value =
        phyto_string_span_subspan(scanner->source, scanner->start, scanner->current);
    lox_token_type_t* type = lox_scanner_keyword_map_get_ref(scanner->keywords, value);
    if (type != NULL) {
        add_token(scanner, *type);
    } else {
        add_token(scanner, lox_token_type_identifier);
    }
}

static void scan_token(lox_scanner_t* scanner) {
    char c = advance(scanner);
    switch (c) {
        case '(':
            add_token(scanner, lox_token_type_left_paren);
            break;
        case ')':
            add_token(scanner, lox_token_type_right_paren);
            break;
        case '{':
            add_token(scanner, lox_token_type_left_brace);
            break;
        case '}':
            add_token(scanner, lox_token_type_right_brace);
            break;
        case ',':
            add_token(scanner, lox_token_type_comma);
            break;
        case '.':
            add_token(scanner, lox_token_type_dot);
            break;
        case '-':
            add_token(scanner, lox_token_type_minus);
            break;
        case '+':
            add_token(scanner, lox_token_type_plus);
            break;
        case ';':
            add_token(scanner, lox_token_type_semicolon);
            break;
        case '*':
            add_token(scanner, lox_token_type_star);
            break;
        case '!':
            add_token(scanner,
                      match(scanner, '=') ? lox_token_type_bang_equal : lox_token_type_bang);
            break;
        case '=':
            add_token(scanner,
                      match(scanner, '=') ? lox_token_type_equal_equal : lox_token_type_equal);
            break;
        case '<':
            add_token(scanner,
                      match(scanner, '=') ? lox_token_type_less_equal : lox_token_type_less);
            break;
        case '>':
            add_token(scanner,
                      match(scanner, '=') ? lox_token_type_greater_equal : lox_token_type_greater);
            break;
        case '/':
            if (match(scanner, '/')) {
                while (peek(scanner) != '\n' && !is_at_end(scanner)) {
                    advance(scanner);
                }
            } else {
                add_token(scanner, lox_token_type_slash);
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
                          phyto_string_span_from_c("Unexpected character."));
            }
            break;
    }
}

lox_scanner_t lox_scanner_new(lox_context_t* ctx, phyto_string_span_t source) {
    lox_scanner_t scanner = {
        .ctx = ctx,
        .source = source,
        .tokens = {0},
        .start = 0,
        .current = 0,
        .line = 1,
        .keywords = lox_scanner_keyword_map_new(20, phyto_hash_default_load, &key_ops, &value_ops),
    };
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("and"),
                                   lox_token_type_kw_and);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("class"),
                                   lox_token_type_kw_class);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("else"),
                                   lox_token_type_kw_else);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("false"),
                                   lox_token_type_kw_false);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("for"),
                                   lox_token_type_kw_for);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("fun"),
                                   lox_token_type_kw_fun);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("if"),
                                   lox_token_type_kw_if);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("nil"),
                                   lox_token_type_kw_nil);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("or"),
                                   lox_token_type_kw_or);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("print"),
                                   lox_token_type_kw_print);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("return"),
                                   lox_token_type_kw_return);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("super"),
                                   lox_token_type_kw_super);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("this"),
                                   lox_token_type_kw_this);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("true"),
                                   lox_token_type_kw_true);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("var"),
                                   lox_token_type_kw_var);
    lox_scanner_keyword_map_insert(scanner.keywords, phyto_string_span_from_c("while"),
                                   lox_token_type_kw_while);
    scanner.tokens = lox_token_vec_init(&lox_token_vec_callbacks);
    return scanner;
}

lox_token_vec_t lox_scanner_scan_tokens(lox_scanner_t* scanner) {
    while (!is_at_end(scanner)) {
        scanner->start = scanner->current;
        scan_token(scanner);
    }

    lox_token_vec_append(&scanner->tokens,
                         lox_token_new(lox_token_type_eof, phyto_string_span_empty(),
                                       lox_object_new_nil(), scanner->line));
    return scanner->tokens;
}

void lox_scanner_free(lox_scanner_t* scanner) {
    lox_token_vec_free(&scanner->tokens);
    lox_scanner_keyword_map_free(scanner->keywords);
    scanner->keywords = NULL;
}
