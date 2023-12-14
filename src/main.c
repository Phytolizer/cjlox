#include "main.h"

#include "interpreter.h"
#include "parser.h"
#include "println.h"
#include "scanner.h"
#include "stmt.h"
#include "sysexits.h"
#include "token.h"

#include <editline/history.h>
#include <editline/readline.h>
#include <sds.h>
#include <stb_ds.h>
#include <stdbool.h>
#include <stdlib.h>
#include <threads.h>

static thread_local bool hadError = false;
static thread_local bool hadRuntimeError = false;
static thread_local Interpreter interpreter;

static sds read_file(char const *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        printfln("Could not open file \"%s\".", path);
        exit(EX_NOINPUT);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    sds buffer = sdsnewlen(SDS_NOINIT, fileSize);
    if (buffer == NULL)
    {
        printfln("Not enough memory to read \"%s\".", path);
        exit(EX_OSERR);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        printfln("Could not read file \"%s\".", path);
        exit(EX_IOERR);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void run(sds source)
{
    Scanner scanner = new_scanner(source);
    Token *tokens = scan_tokens(&scanner);
    Parser parser = new_parser(tokens);
    Stmt **statements = parser_parse(&parser);

    if (hadError)
    {
        return;
    }
    if (!interpreter.init)
    {
        interpreter = new_interpreter();
    }

    interpret(&interpreter, statements);
}

static void run_file(char const *path)
{
    sds source = read_file(path);

    run(source);

    sdsfree(source);
    if (hadError)
    {
        exit(EX_DATAERR);
    }
    if (hadRuntimeError)
    {
        exit(EX_SOFTWARE);
    }
}

static void run_prompt(void)
{
    read_history(".lox-history");
    for (;;)
    {
        char *raw_line = readline("> ");
        if (raw_line == NULL)
        {
            fputc('\n', stderr);
            break;
        }
        add_history(raw_line);
        sds line = sdsnew(raw_line);
        free(raw_line);

        run(line);
        sdsfree(line);
        hadError = false;
        hadRuntimeError = false;
    }
    write_history(".lox-history");
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        printfln("Usage: %s [script]", argv[0]);
        return EX_USAGE;
    }

    if (argc == 2)
    {
        run_file(argv[1]);
    }
    else
    {
        run_prompt();
    }
}

void error(int line, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    verror(line, fmt, args);
    va_end(args);
}

void verror(int line, char const *fmt, va_list args)
{
    vreport(line, "", fmt, args);
}

void error_token(Token token, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    verror_token(token, fmt, args);
    va_end(args);
}

void verror_token(Token token, char const *fmt, va_list args)
{
    if (token.type == TOKEN_EOF)
    {
        vreport(token.line, " at end", fmt, args);
    }
    else
    {
        vreport(token.line, sdscatfmt(sdsempty(), " at '%s'", token.lexeme), fmt, args);
    }
}

void report(int line, char const *where, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vreport(line, where, fmt, args);
    va_end(args);
}

void vreport(int line, char const *where, char const *fmt, va_list args)
{
    fprintf(stderr, "[line %d] Error%s: ", line, where);
    vfprintfln(stderr, fmt, args);
    hadError = true;
}

void runtime_error(Token token, sds message)
{
    fprintfln(stderr, "%s\n[line %d]", message, token.line);
    hadRuntimeError = true;
}

// int main(void)
// {
//     Expr *expression = new_binary_expr(
//         new_unary_expr((Token){TOKEN_MINUS, "-", NULL, 1}, new_literal_expr(new_number_object(123))),
//         (Token){TOKEN_STAR, "*", NULL, 1}, new_grouping_expr(new_literal_expr(new_number_object(45.67))));
//     AstPrinter printer = ast_printer_new();
//     sds result = ast_printer_print(printer, expression);
//     printfln("%s", result);
// }
