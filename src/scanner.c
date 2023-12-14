#include "scanner.h"

#include "main.h"
#include "meta.h"
#include "object.h"
#include "token_type.h"

#include <stb_ds.h>
#include <stdbool.h>

Scanner new_scanner(sds source)
{
    Scanner scanner = {source, NULL, 0, 0, 1};
    return scanner;
}

static bool is_at_end(Scanner *scanner)
{
    return scanner->current >= sdslen(scanner->source);
}

static char advance(Scanner *scanner)
{
    char c = scanner->source[scanner->current];
    ++scanner->current;
    return c;
}

static char peek(Scanner *scanner)
{
    if (is_at_end(scanner))
    {
        return '\0';
    }
    return scanner->source[scanner->current];
}

static char peek_next(Scanner *scanner)
{
    if (scanner->current + 1 >= sdslen(scanner->source))
    {
        return '\0';
    }
    return scanner->source[scanner->current + 1];
}

static bool match(Scanner *scanner, char expected)
{
    if (is_at_end(scanner))
    {
        return false;
    }
    if (scanner->source[scanner->current] != expected)
    {
        return false;
    }

    ++scanner->current;
    return true;
}

static void add_token(Scanner *scanner, TokenType type, Object *literal)
{
    sds text = sdsnewlen(scanner->source + scanner->start, scanner->current - scanner->start);
    Token token = {type, text, literal, scanner->line};
    arrput(scanner->tokens, token);
}

#define ADD_TOKEN_3(scanner, type, literal) (add_token)(scanner, type, literal)
#define ADD_TOKEN_2(scanner, type) (add_token)(scanner, type, NULL)
#define add_token(...) CONCAT(ADD_TOKEN_, VARGS(__VA_ARGS__))(__VA_ARGS__)

static void string(Scanner *scanner)
{
    while (peek(scanner) != '"' && !is_at_end(scanner))
    {
        if (peek(scanner) == '\n')
        {
            ++scanner->line;
        }
        advance(scanner);
    }

    if (is_at_end(scanner))
    {
        error(scanner->line, "Unterminated string.");
        return;
    }

    advance(scanner);

    sds value = sdsnewlen(scanner->source + scanner->start + 1, scanner->current - scanner->start - 2);
    add_token(scanner, TOKEN_STRING, new_string_object(value));
}

static bool is_digit(char c)
{
    return '0' <= c && c <= '9';
}

static void number(Scanner *scanner)
{
    while (is_digit(peek(scanner)))
    {
        advance(scanner);
    }

    if (peek(scanner) == '.' && is_digit(peek_next(scanner)))
    {
        advance(scanner);
        while (is_digit(peek(scanner)))
        {
            advance(scanner);
        }
    }

    sds value = sdsnewlen(scanner->source + scanner->start, scanner->current - scanner->start);
    add_token(scanner, TOKEN_NUMBER, new_number_object(strtod(value, NULL)));
}

static bool is_alpha(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

static void identifier(Scanner *scanner)
{
    struct KeywordEntry
    {
        char const *key;
        TokenType value;
    };
    static struct KeywordEntry *keywords = NULL;

    if (keywords == NULL)
    {
        sh_new_arena(keywords);
        shput(keywords, "and", TOKEN_AND);
        shput(keywords, "class", TOKEN_CLASS);
        shput(keywords, "else", TOKEN_ELSE);
        shput(keywords, "false", TOKEN_FALSE);
        shput(keywords, "for", TOKEN_FOR);
        shput(keywords, "fun", TOKEN_FUN);
        shput(keywords, "if", TOKEN_IF);
        shput(keywords, "nil", TOKEN_NIL);
        shput(keywords, "or", TOKEN_OR);
        shput(keywords, "print", TOKEN_PRINT);
        shput(keywords, "return", TOKEN_RETURN);
        shput(keywords, "super", TOKEN_SUPER);
        shput(keywords, "this", TOKEN_THIS);
        shput(keywords, "true", TOKEN_TRUE);
        shput(keywords, "var", TOKEN_VAR);
        shput(keywords, "while", TOKEN_WHILE);
        shdefault(keywords, TOKEN_IDENTIFIER);
    }

    while (is_alpha_numeric(peek(scanner)))
    {
        advance(scanner);
    }

    sds text = sdsnewlen(scanner->source + scanner->start, scanner->current - scanner->start);
    TokenType type = shget(keywords, text);
    sdsfree(text);
    add_token(scanner, type, NULL);
}

static void scan_token(Scanner *scanner)
{
    char c = advance(scanner);
    switch (c)
    {
    case '(':
        add_token(scanner, TOKEN_LEFT_PAREN);
        break;
    case ')':
        add_token(scanner, TOKEN_RIGHT_PAREN);
        break;
    case '{':
        add_token(scanner, TOKEN_LEFT_BRACE);
        break;
    case '}':
        add_token(scanner, TOKEN_RIGHT_BRACE);
        break;
    case ',':
        add_token(scanner, TOKEN_COMMA);
        break;
    case '.':
        add_token(scanner, TOKEN_DOT);
        break;
    case '-':
        add_token(scanner, TOKEN_MINUS);
        break;
    case '+':
        add_token(scanner, TOKEN_PLUS);
        break;
    case ';':
        add_token(scanner, TOKEN_SEMICOLON);
        break;
    case '*':
        add_token(scanner, TOKEN_STAR);
        break;
    case '!':
        add_token(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        break;
    case '=':
        add_token(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        break;
    case '<':
        add_token(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        break;
    case '>':
        add_token(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        break;
    case '/':
        if (match(scanner, '/'))
        {
            while (peek(scanner) != '\n' && !is_at_end(scanner))
            {
                advance(scanner);
            }
        }
        else
        {
            add_token(scanner, TOKEN_SLASH);
        }
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        ++scanner->line;
        break;
    case '"':
        string(scanner);
        break;
    default:
        if (is_digit(c))
        {
            number(scanner);
        }
        else if (is_alpha(c))
        {
            identifier(scanner);
        }
        else
        {
            error(scanner->line, "Unexpected character.");
        }
        break;
    }
}

Token *scan_tokens(Scanner *scanner)
{
    while (!is_at_end(scanner))
    {
        scanner->start = scanner->current;
        scan_token(scanner);
    }

    Token eof_tok = {TOKEN_EOF, "", NULL, scanner->line};
    arrput(scanner->tokens, eof_tok);
    return scanner->tokens;
}
