#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sysexits.h>

#define READ_ERROR(File)                                                       \
	do {                                                                   \
		fprintf(stderr, "[ERR] Could not read from '%s'\n", File);     \
		return EX_IOERR;                                               \
	} while (0)

#define WRITE_ERROR(File)                                                      \
	do {                                                                   \
		fprintf(stderr, "[ERR] Could not write to '" File "'\n");      \
		return EX_IOERR;                                               \
	} while (0)

struct ast_subclass {
	char *name;
	char **members;
	size_t num_members;
};

struct ast_visitor {
	char *name;
	char *return_type;
};

struct ast_root {
	char *name;
	char **headers;
	size_t num_headers;
	struct ast_visitor *visitors;
	size_t num_visitors;
	struct ast_subclass *subclasses;
	size_t num_subclasses;
};

struct ast_definition {
	struct ast_root *roots;
	size_t num_roots;
};

static void ast_definition_deinit(struct ast_definition *ast_definition)
{
	for (size_t i = 0; i < ast_definition->num_roots; ++i) {
		struct ast_root *root = &ast_definition->roots[i];
		for (size_t j = 0; j < root->num_subclasses; ++j) {
			struct ast_subclass *subclass = &root->subclasses[j];
			for (size_t k = 0; k < subclass->num_members; ++k) {
				free(subclass->members[k]);
			}
			free(subclass->members);
			free(subclass->name);
		}
		free(root->subclasses);
		for (size_t j = 0; j < root->num_headers; ++j) {
			free(root->headers[j]);
		}
		free(root->headers);
		for (size_t j = 0; j < root->num_visitors; ++j) {
			free(root->visitors[j].name);
			free(root->visitors[j].return_type);
		}
		free(root->visitors);
		free(root->name);
	}
	free(ast_definition->roots);
}

struct scanner {
	const char *input;
	size_t input_length;
	size_t token_start;
	size_t position;
};

enum scanner_mode {
	scanner_mode_initial,
	scanner_mode_base_type,
	scanner_mode_lparen,
	scanner_mode_rparen,
	scanner_mode_header_name,
	scanner_mode_v,
	scanner_mode_visitor_name,
	scanner_mode_equals,
	scanner_mode_visitor_return_type,
	scanner_mode_subtype_name,
	scanner_mode_colon,
	scanner_mode_subtype_member,
	scanner_mode_comma,
	scanner_mode_comma2,
	scanner_mode_comma3,
	scanner_mode_period,
	scanner_mode_error,
	scanner_mode_final,
};

static void scanner_init(struct scanner *scanner, const char *input,
			 size_t input_length)
{
	scanner->input = input;
	scanner->input_length = input_length;
	scanner->token_start = 0;
	scanner->position = 0;
}

static void scanner_skip_whitespace(struct scanner *scanner)
{
	while (scanner->position < scanner->input_length &&
	       isspace(scanner->input[scanner->position])) {
		++scanner->position;
	}
}

static enum scanner_mode scanner_scan(struct scanner *scanner,
				      enum scanner_mode scanner_mode)
{
	scanner->token_start = scanner->position;
	switch (scanner_mode) {
	case scanner_mode_initial:
		if (scanner->position == scanner->input_length) {
			return scanner_mode_final;
		}
		if (scanner->input[scanner->position] != '*') {
			fprintf(stderr,
				"[LEX] expected '*' before base type name\n");
			return scanner_mode_error;
		}
		++scanner->position;
		return scanner_mode_base_type;
	case scanner_mode_base_type:
		if (scanner->position == scanner->input_length ||
		    !isalnum(scanner->input[scanner->position])) {
			fprintf(stderr, "[LEX] expected base type name\n");
			return scanner_mode_error;
		}
		while (scanner->position < scanner->input_length &&
		       isalnum(scanner->input[scanner->position])) {
			++scanner->position;
		}
		return scanner_mode_lparen;
	case scanner_mode_lparen:
		if (scanner->position == scanner->input_length ||
		    scanner->input[scanner->position] != '(') {
			fprintf(stderr,
				"[LEX] expected include list after base type\n");
			return scanner_mode_error;
		}
		++scanner->position;
		return scanner_mode_header_name;
	case scanner_mode_header_name:
		if (scanner->position == scanner->input_length) {
			fprintf(stderr,
				"[LEX] expected include list after '('\n");
			return scanner_mode_error;
		}
		while (scanner->position < scanner->input_length &&
		       scanner->input[scanner->position] != ',' &&
		       scanner->input[scanner->position] != ')') {
			++scanner->position;
		}
		if (scanner->position == scanner->input_length) {
			fprintf(stderr,
				"[LEX] expected ')' after include list\n");
			return scanner_mode_error;
		}
		if (scanner->input[scanner->position] == ',') {
			return scanner_mode_comma2;
		}
		return scanner_mode_rparen;
	case scanner_mode_comma2:
		++scanner->position;
		return scanner_mode_header_name;
	case scanner_mode_rparen:
		++scanner->position;
		return scanner_mode_v;
	case scanner_mode_v:
		if (scanner->position == scanner->input_length ||
		    scanner->input[scanner->position] != 'V') {
			fprintf(stderr,
				"[LEX] expected 'V' after include list\n");
			return scanner_mode_error;
		}
		++scanner->position;
		return scanner_mode_visitor_name;
	case scanner_mode_visitor_name:
		if (scanner->position == scanner->input_length) {
			fprintf(stderr,
				"[LEX] expected visitor name after 'V'\n");
			return scanner_mode_error;
		}
		while (scanner->position < scanner->input_length &&
		       (isalnum(scanner->input[scanner->position]) ||
			scanner->input[scanner->position] == '_')) {
			++scanner->position;
		}
		return scanner_mode_equals;
	case scanner_mode_equals:
		++scanner->position;
		return scanner_mode_visitor_return_type;
	case scanner_mode_visitor_return_type:
		if (scanner->position == scanner->input_length) {
			fprintf(stderr,
				"[LEX] expected visitor return type after '='\n");
			return scanner_mode_error;
		}
		while (scanner->position < scanner->input_length &&
		       scanner->input[scanner->position] != ',' &&
		       scanner->input[scanner->position] != '.') {
			++scanner->position;
		}
		if (scanner->position == scanner->input_length) {
			fprintf(stderr,
				"[LEX] expected ',' or '.' after visitor return type\n");
			return scanner_mode_error;
		}
		if (scanner->input[scanner->position] == ',') {
			return scanner_mode_comma3;
		}
		return scanner_mode_period;
	case scanner_mode_comma3:
		++scanner->position;
		return scanner_mode_visitor_name;
	case scanner_mode_subtype_name:
		if (scanner->position == scanner->input_length) {
			return scanner_mode_final;
		}
		if (scanner->input[scanner->position] == '*') {
			++scanner->position;
			return scanner_mode_base_type;
		}
		while (scanner->position < scanner->input_length &&
		       isalnum(scanner->input[scanner->position])) {
			++scanner->position;
		}
		return scanner_mode_colon;
	case scanner_mode_colon:
		if (scanner->position == scanner->input_length ||
		    scanner->input[scanner->position] != ':') {
			fprintf(stderr,
				"[LEX] expected ':' after subtype name\n");
			return scanner_mode_error;
		}
		++scanner->position;
		return scanner_mode_subtype_member;
	case scanner_mode_subtype_member:
		if (scanner->position == scanner->input_length ||
		    !isalnum(scanner->input[scanner->position])) {
			fprintf(stderr, "[LEX] expected subtype member\n");
			return scanner_mode_error;
		}
		while (scanner->position < scanner->input_length &&
		       scanner->input[scanner->position] != '\n' &&
		       scanner->input[scanner->position] != ',' &&
		       scanner->input[scanner->position] != '.') {
			++scanner->position;
		}
		if (scanner->position == scanner->input_length ||
		    scanner->input[scanner->position] == '\n') {
			return scanner_mode_subtype_name;
		}
		if (scanner->input[scanner->position] == '.') {
			return scanner_mode_period;
		}
		return scanner_mode_comma;
	case scanner_mode_period:
		++scanner->position;
		return scanner_mode_subtype_name;
	case scanner_mode_comma:
		++scanner->position;
		return scanner_mode_subtype_member;
	default:
		assert(0 && "unhandled code path in lexer");
	}
}

static struct ast_definition parse_ast_definition(FILE *stream)
{
	if (fseek(stream, 0, SEEK_END)) {
		perror("fseek");
		return (struct ast_definition){ 0 };
	}
	long file_length = ftell(stream);
	if (file_length == -1) {
		perror("ftell");
		return (struct ast_definition){ 0 };
	}
	char *filebuf = malloc(file_length + 1);
	if (fseek(stream, 0, SEEK_SET)) {
		perror("fseek");
		return (struct ast_definition){ 0 };
	}
	if (fread(filebuf, sizeof(char), file_length, stream) !=
	    (unsigned long)file_length) {
		perror("fread");
		return (struct ast_definition){ 0 };
	}
	filebuf[file_length] = '\0';

	struct ast_definition ast_definition = { 0 };

	struct scanner scanner;
	scanner_init(&scanner, filebuf, file_length);
	enum scanner_mode next = scanner_mode_initial;
	while (1) {
		scanner_skip_whitespace(&scanner);
		enum scanner_mode current = next;
		next = scanner_scan(&scanner, current);
		if (next == scanner_mode_error) {
			ast_definition_deinit(&ast_definition);
			return (struct ast_definition){ 0 };
		}
		if (next == scanner_mode_final) {
			break;
		}
		switch (current) {
		case scanner_mode_initial:
		case scanner_mode_colon:
		case scanner_mode_comma:
		case scanner_mode_lparen:
		case scanner_mode_comma2:
		case scanner_mode_rparen:
		case scanner_mode_period:
		case scanner_mode_v:
		case scanner_mode_comma3:
		case scanner_mode_equals:
			break;
		case scanner_mode_base_type: {
			++ast_definition.num_roots;
			ast_definition.roots =
				realloc(ast_definition.roots,
					(ast_definition.num_roots) *
						sizeof(struct ast_root));
			size_t base_type_name_len =
				scanner.position - scanner.token_start;
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			last_root->name = malloc(base_type_name_len + 1);
			strncpy(last_root->name,
				&scanner.input[scanner.token_start],
				base_type_name_len);
			last_root->name[base_type_name_len] = '\0';
			fprintf(stderr, "root: %s\n", last_root->name);
			last_root->num_subclasses = 0;
			last_root->subclasses = NULL;
			last_root->num_headers = 0;
			last_root->headers = NULL;
			last_root->num_visitors = 0;
			last_root->visitors = NULL;
			break;
		}
		case scanner_mode_header_name: {
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			++last_root->num_headers;
			last_root->headers = realloc(last_root->headers,
						     last_root->num_headers *
							     sizeof(char *));
			char **last_header =
				&last_root->headers[last_root->num_headers - 1];
			size_t header_len =
				scanner.position - scanner.token_start;
			*last_header = malloc(header_len + 1);
			strncpy(*last_header,
				&scanner.input[scanner.token_start],
				header_len);
			(*last_header)[header_len] = '\0';
			break;
		}
		case scanner_mode_visitor_name: {
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			++last_root->num_visitors;
			last_root->visitors =
				realloc(last_root->visitors,
					last_root->num_visitors *
						sizeof(struct ast_visitor));
			struct ast_visitor *last_visitor =
				&last_root->visitors[last_root->num_visitors -
						     1];
			size_t visitor_len =
				scanner.position - scanner.token_start;
			last_visitor->name = malloc(visitor_len + 1);
			strncpy(last_visitor->name,
				&scanner.input[scanner.token_start],
				visitor_len);
			last_visitor->name[visitor_len] = '\0';
			last_visitor->return_type = NULL;
			break;
		}
		case scanner_mode_visitor_return_type: {
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			struct ast_visitor *last_visitor =
				&last_root->visitors[last_root->num_visitors -
						     1];
			size_t return_type_len =
				scanner.position - scanner.token_start;
			last_visitor->return_type = malloc(return_type_len + 1);
			strncpy(last_visitor->return_type,
				&scanner.input[scanner.token_start],
				return_type_len);
			last_visitor->return_type[return_type_len] = '\0';
			break;
		}
		case scanner_mode_subtype_name: {
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			++last_root->num_subclasses;
			last_root->subclasses =
				realloc(last_root->subclasses,
					last_root->num_subclasses *
						sizeof(struct ast_subclass));
			struct ast_subclass *last_subclass =
				&last_root->subclasses[last_root->num_subclasses -
						       1];
			size_t subclass_name_len =
				scanner.position - scanner.token_start;
			last_subclass->name = malloc(subclass_name_len + 1);
			strncpy(last_subclass->name,
				&scanner.input[scanner.token_start],
				subclass_name_len);
			last_subclass->name[subclass_name_len] = '\0';
			fprintf(stderr, " subclass: %s\n", last_subclass->name);
			last_subclass->members = NULL;
			last_subclass->num_members = 0;
			break;
		}
		case scanner_mode_subtype_member: {
			struct ast_root *last_root =
				&ast_definition
					 .roots[ast_definition.num_roots - 1];
			struct ast_subclass *last_subclass =
				&last_root->subclasses[last_root->num_subclasses -
						       1];
			++last_subclass->num_members;
			last_subclass->members = realloc(
				last_subclass->members,
				last_subclass->num_members * sizeof(char *));
			char **last_member =
				&last_subclass
					 ->members[last_subclass->num_members -
						   1];
			size_t member_len =
				scanner.position - scanner.token_start;
			*last_member = malloc(member_len + 1);
			strncpy(*last_member,
				&scanner.input[scanner.token_start],
				member_len);
			(*last_member)[member_len] = '\0';
			fprintf(stderr, "  member: %s\n", *last_member);
			break;
		}
		default:
			fprintf(stderr, "uncovered code path : %d\n", current);
			assert(0 && "uncovered code path in parser");
		}
	}
	free(filebuf);
	return ast_definition;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ast definition>\n", argv[0]);
		return EX_USAGE;
	}
	FILE *input = fopen(argv[1], "re");
	if (input == NULL) {
		READ_ERROR(argv[1]);
	}

	struct ast_definition ast_definition = parse_ast_definition(input);
	if (ast_definition.num_roots == 0) {
		return EX_DATAERR;
	}
	fclose(input);

	FILE *header_output = fopen("include/tool/ast.h", "we");
	if (header_output == NULL) {
		WRITE_ERROR("ast.h");
	}
	if (fprintf(header_output, "#pragma once\n") < 0) {
		perror("fprintf");
		return EX_IOERR;
	}
	FILE *source_output = fopen("ast.c", "we");
	if (source_output == NULL) {
		WRITE_ERROR("tool/ast.c");
	}
	if (fprintf(source_output, "#include \"tool/ast.h\"\n\n") < 0) {
		perror("fprintf");
		return EX_IOERR;
	}
	fprintf(source_output, "#include <assert.h>\n");
	fprintf(source_output, "#include <stdlib.h>\n");
	fprintf(source_output, "\n");

	for (size_t i = 0; i < ast_definition.num_roots; ++i) {
		struct ast_root *root = &ast_definition.roots[i];
		for (size_t j = 0; j < root->num_headers; ++j) {
			fprintf(header_output, "#include <lox/%s>\n",
				root->headers[j]);
		}
		for (size_t j = 0; j < root->num_visitors; ++j) {
			fprintf(source_output, "#include <lox/%s.h>\n",
				root->visitors[j].name);
		}
		fprintf(header_output, "enum %s_type {\n", root->name);
		for (size_t j = 0; j < root->num_subclasses; ++j) {
			fprintf(header_output, "\t%s_type_%s,\n", root->name,
				root->subclasses[j].name);
		}
		fprintf(header_output, "};\n");
		fprintf(header_output, "struct %s {\n", root->name);
		fprintf(header_output, "\tenum %s_type type;\n", root->name);
		fprintf(header_output, "};\n");
		for (size_t j = 0; j < root->num_subclasses; ++j) {
			struct ast_subclass *subclass = &root->subclasses[j];
			fprintf(header_output, "struct %s_%s {\n",
				subclass->name, root->name);
			fprintf(header_output, "\tstruct %s base;\n",
				root->name);
			for (size_t k = 0; k < subclass->num_members; ++k) {
				fprintf(header_output, "\t%s;\n",
					subclass->members[k]);
			}
			fprintf(header_output, "};\n");
		}
		for (size_t j = 0; j < root->num_subclasses; ++j) {
			struct ast_subclass *subclass = &root->subclasses[j];
			fprintf(header_output, "struct %s_%s *%s_new_%s(",
				subclass->name, root->name, root->name,
				subclass->name);
			for (size_t k = 0; k < subclass->num_members; ++k) {
				fprintf(header_output, "%s",
					subclass->members[k]);
				if (k < subclass->num_members - 1) {
					fprintf(header_output, ", ");
				}
			}
			fprintf(header_output, ");\n");

			fprintf(source_output, "struct %s_%s *%s_new_%s(",
				subclass->name, root->name, root->name,
				subclass->name);
			for (size_t k = 0; k < subclass->num_members; ++k) {
				fprintf(source_output, "%s",
					subclass->members[k]);
				if (k < subclass->num_members - 1) {
					fprintf(source_output, ", ");
				}
			}
			fprintf(source_output, ")\n{\n");
			fprintf(source_output,
				"\tstruct %s_%s *result = malloc(sizeof(struct %s_%s));\n",
				subclass->name, root->name, subclass->name,
				root->name);
			fprintf(source_output,
				"\tresult->base.type = %s_type_%s;\n",
				root->name, subclass->name);
			for (size_t k = 0; k < subclass->num_members; ++k) {
				const char *arg_name =
					strrchr(subclass->members[k], ' ');
				if (arg_name == NULL || arg_name[1] == '\0') {
					fprintf(stderr,
						"[PARSE] member is not a valid C declaration: %s\n",
						subclass->members[k]);
					return EX_DATAERR;
				}
				// after the space is the name
				arg_name = &arg_name[1];
				while (arg_name[0] != '\0' &&
				       !isalpha(arg_name[0])) {
					++arg_name;
				}
				if (arg_name[0] == '\0') {
					fprintf(stderr,
						"[PARSE] member has no name: %s\n",
						subclass->members[k]);
					return EX_DATAERR;
				}
				fprintf(source_output, "\tresult->%s = %s;\n",
					arg_name, arg_name);
			}
			fprintf(source_output, "\treturn result;\n");
			fprintf(source_output, "}\n");
		}
		for (size_t j = 0; j < root->num_visitors; ++j) {
			struct ast_visitor *visitor = &root->visitors[j];
			fprintf(header_output, "struct %s;\n", visitor->name);
			fprintf(header_output,
				"%s %s_accept_%s(struct %s *self, struct %s *visitor);\n",
				visitor->return_type, root->name, visitor->name,
				root->name, visitor->name);
			fprintf(source_output,
				"%s %s_accept_%s(struct %s *self, struct %s *visitor)\n",
				visitor->return_type, root->name, visitor->name,
				root->name, visitor->name);
			fprintf(source_output, "{\n");
			fprintf(source_output, "\tswitch (self->type) {\n");
			for (size_t k = 0; k < root->num_subclasses; ++k) {
				struct ast_subclass *subclass =
					&root->subclasses[k];
				fprintf(source_output, "\tcase %s_type_%s:\n",
					root->name, subclass->name);
				fprintf(source_output,
					"\t\treturn %s_visit_%s_%s(visitor, (struct %s_%s *)self);\n",
					visitor->name, subclass->name,
					root->name, subclass->name, root->name);
			}
			fprintf(source_output, "\t}\n");
			fprintf(source_output,
				"\tassert(0 && \"invalid %s type\");\n",
				root->name);
			fprintf(source_output, "}\n");
		}
	}

	fclose(source_output);
	fclose(header_output);

	ast_definition_deinit(&ast_definition);
	return 0;
}
