#include "lox/lox.h"

#include "lox/ast_printer.h"
#include "lox/parser.h"
#include "lox/scanner.h"
#include "lox/token.h"
#include "tool/ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

static char *read_line(void)
{
	char *line = malloc(sizeof(char));
	size_t len = 0;
	while (true) {
		int c = fgetc(stdin);
		if (c == EOF && len == 0) {
			free(line);
			return NULL;
		}
		if (c == EOF || c == '\n') {
			line[len] = '\0';
			return line;
		}
		line[len] = (char)c;
		++len;
		line = realloc(line, (len + 1) * sizeof(char));
	}
}

static void run(struct lox_state *lox_state, const char *source)
{
	struct scanner scanner;
	scanner_init(&scanner, source);
	struct token_list *tokens = scanner_scan_tokens(&scanner, lox_state);
	struct parser parser;
	parser_init(&parser, lox_state, tokens);
	struct expr *expr = parser_parse(&parser);
	if (expr) {
		struct ast_printer ast_printer;
		expr_accept_ast_printer(expr, &ast_printer);
	}
	scanner_deinit(&scanner);
}

static int run_file(struct lox_state *lox_state, const char *path)
{
	FILE *fp = fopen(path, "rbe");
	if (fseek(fp, 0, SEEK_END) == -1) {
		perror("fseek");
		return EX_IOERR;
	}
	size_t filelen = ftell(fp);
	if (fseek(fp, 0, SEEK_SET) == -1) {
		perror("fseek");
		return EX_IOERR;
	}
	char *buf = malloc(filelen + 1);
	fread(buf, sizeof(char), filelen, fp);
	if (ferror(fp)) {
		perror("fread");
		return EX_IOERR;
	}
	buf[filelen] = '\0';
	run(lox_state, buf);
	free(buf);
	if (lox_state->had_error) {
		return EX_DATAERR;
	}
	return 0;
}

static void run_prompt(struct lox_state *lox_state)
{
	while (true) {
		printf("> ");
		fflush(stdout);
		char *line = read_line();
		if (line == NULL) {
			printf("\n");
			break;
		}
		run(lox_state, line);
		free(line);
		lox_state->had_error = false;
	}
}

static void report(struct lox_state *lox_state, size_t line, const char *where,
		   const char *message)
{
	printf("[line %zu] Error%s: %s\n", line, where, message);
	lox_state->had_error = true;
}

void lox_init(struct lox_state *lox_state)
{
	lox_state->had_error = false;
}

void lox_error(struct lox_state *lox_state, size_t line, const char *message)
{
	report(lox_state, line, "", message);
}

void lox_error_at_token(struct lox_state *lox_state, struct token *token,
			const char *message)
{
	if (token->type == token_eof) {
		report(lox_state, token->line, " at end", message);
	} else {
		char *where = malloc(strlen(token->lexeme) + sizeof(" at ''"));
		sprintf(where, " at '%s'", token->lexeme);
		report(lox_state, token->line, where, message);
		free(where);
	}
}

int main(int argc, char *argv[])
{
	struct lox_state lox_state;
	lox_init(&lox_state);
	if (argc > 2) {
		printf("Usage: %s [script]\n", argv[0]);
		return EX_USAGE;
	}
	if (argc == 2) {
		return run_file(&lox_state, argv[1]);
	}
	run_prompt(&lox_state);

	return 0;
}
