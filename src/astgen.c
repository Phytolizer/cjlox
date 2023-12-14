#include "println.h"

#include <assert.h>
#include <sds.h>
#include <stb_ds.h>
#include <stdlib.h>
#include <sysexits.h>

typedef struct ParsedType
{
    sds name;
    sds *fields;
} ParsedType;

static void proto_new(FILE *stream, char const *base_name, char const *base_lc, ParsedType type)
{
    sds name_lc = sdsnew(type.name);
    sdstolower(name_lc);
    fprintf(stream, "%s *new_%s_%s(", base_name, name_lc, base_lc);
    for (size_t i = 0; i < arrlen(type.fields); ++i)
    {
        if (i > 0)
        {
            fprintf(stream, ", ");
        }
        fprintf(stream, "%s", type.fields[i]);
    }
    fprintf(stream, ")");
}

static void proto_accept(FILE *stream, char const *base_name, char const *base_lc, ParsedType type)
{
    sds name_lc = sdsnew(type.name);
    sdstolower(name_lc);
    fprintf(stream, "void *%1$s_%2$s_accept(%3$s%4$s *%2$s, %4$sVisitor *visitor, void *data)", name_lc, base_lc,
            type.name, base_name);
}

static void proto_accept_base(FILE *stream, char const *base_name, char const *base_lc)
{
    fprintf(stream, "void *%1$s_accept(%2$s *%1$s, struct %2$sVisitor *visitor, void *data)", base_lc, base_name);
}

static void define_visitor(FILE *header, char const *base_name, char const *base_lc, ParsedType *types)
{
    fprintln(header);
    fprintfln(header, "typedef struct %sVisitor\n{", base_name);
    for (size_t i = 0; i < arrlen(types); ++i)
    {
        sds name_lc = sdsnew(types[i].name);
        sdstolower(name_lc);
        fprintfln(header,
                  "    void *(*visit_%1$s_%2$s)(struct %4$sVisitor *visitor, void *data, struct %3$s%4$s *%2$s);",
                  name_lc, base_lc, types[i].name, base_name);
    }
    fprintfln(header, "} %sVisitor;", base_name);
}

static void define_type(FILE *header, FILE *source, char const *base_name, char const *base_lc, char const *base_uc,
                        ParsedType type)
{
    fprintln(header);
    fprintfln(header, "typedef struct %s%s\n{", type.name, base_name);
    fprintfln(header, "    %s base;", base_name);
    for (size_t i = 0; i < arrlen(type.fields); ++i)
    {
        fprintfln(header, "    %s;", type.fields[i]);
    }
    fprintfln(header, "} %s%s;", type.name, base_name);
    fprintln(header);
    proto_new(header, base_name, base_lc, type);
    fprintfln(header, ";");
    proto_accept(header, base_name, base_lc, type);
    fprintfln(header, ";");

    fprintln(source);
    proto_new(source, base_name, base_lc, type);
    fprintfln(source, "\n{");
    fprintfln(source, "    %1$s%2$s *%3$s = malloc(sizeof(*%3$s));", type.name, base_name, base_lc);
    sds name_uc = sdsnew(type.name);
    sdstoupper(name_uc);
    fprintfln(source, "    %1$s->base.type = %2$s_%3$s;", base_lc, base_uc, name_uc);
    for (size_t i = 0; i < arrlen(type.fields); ++i)
    {
        char const *last_space = strrchr(type.fields[i], ' ');
        sds field_name = sdsnew(last_space + 1);
        fprintfln(source, "    %1$s->%2$s = %2$s;", base_lc, field_name);
    }
    fprintfln(source, "    return &%s->base;", base_lc);
    fprintfln(source, "}");

    fprintln(source);
    proto_accept(source, base_name, base_lc, type);
    fprintfln(source, "\n{");
    sds name_lc = sdsnew(type.name);
    sdstolower(name_lc);
    fprintfln(source, "    return visitor->visit_%1$s_%2$s(visitor, data, %3$s);", name_lc, base_lc, base_lc);
    fprintfln(source, "}");
}

static void define_ast(char const *output_dir, char const *base_name, char const *prelude, char const *types[])
{
    sds base_lc = sdsnew(base_name);
    sdstolower(base_lc);

    sds base_uc = sdsnew(base_name);
    sdstoupper(base_uc);

    sds header_path = sdscatprintf(sdsempty(), "%s/%s.h", output_dir, base_lc);
    sds source_path = sdscatprintf(sdsempty(), "%s/%s.c", output_dir, base_lc);

    ParsedType *parsed_types = NULL;
    for (char const **ptype = types; *ptype != NULL; ++ptype)
    {
        char const *type = *ptype;
        char const *colon_pos = strchr(type, ':');
        assert(colon_pos != NULL);
        sds name = sdstrim(sdsnewlen(type, colon_pos - type), " ");
        sds *fields = NULL;
        char const *it = colon_pos + 1;
        char const *comma_pos;
        while ((comma_pos = strchr(it, ',')) != NULL)
        {
            sds field = sdstrim(sdsnewlen(it, comma_pos - it), " ");
            if (sdslen(field) > 0)
            {
                arrput(fields, field);
            }
            it = comma_pos + 1;
        }
        arrput(fields, sdstrim(sdsnew(it), " "));
        ParsedType parsed = {.name = name, .fields = fields};
        arrput(parsed_types, parsed);
    }

    FILE *header = fopen(header_path, "w");
    if (header == NULL)
    {
        fprintfln(stderr, "Could not open file \"%s\".", header_path);
        exit(EX_CANTCREAT);
    }

    FILE *source = fopen(source_path, "w");
    if (source == NULL)
    {
        fprintfln(stderr, "Could not open file \"%s\".", source_path);
        exit(EX_CANTCREAT);
    }

    fprintfln(source, "#include \"%s.h\"", base_lc);

    fprintfln(header, "#pragma once");
    fprintln(header);
    fprintfln(header, "%s", prelude);
    fprintfln(header, "#include <stb_ds.h>");
    fprintln(header);
    fprintfln(header, "#define %s_X \\", base_uc);
    for (size_t i = 0; i < arrlen(parsed_types); ++i)
    {
        sds name = sdsnew(parsed_types[i].name);
        sdstoupper(name);
        fprintfln(header, "    X(%s) \\", name);
    }
    fprintln(header);
    fprintfln(header, "typedef enum %sType\n{", base_name);
    fprintfln(header, "#define X(x) %s_##x,", base_uc);
    fprintfln(header, "    %s_X", base_uc);
    fprintfln(header, "#undef X");
    fprintfln(header, "} %sType;", base_name);
    fprintln(header);
    fprintfln(header, "typedef struct %1$s\n{\n    %1$sType type;\n} %1$s;", base_name);

    fprintln(header);
    for (size_t i = 0; i < arrlen(parsed_types); ++i)
    {
        fprintfln(header, "struct %s%s;", parsed_types[i].name, base_name);
    }

    define_visitor(header, base_name, base_lc, parsed_types);
    fprintln(header);
    proto_accept_base(header, base_name, base_lc);
    fprintfln(header, ";");

    fprintln(source);
    proto_accept_base(source, base_name, base_lc);
    fprintfln(source, "\n{");
    fprintfln(source, "    switch (%s->type)", base_lc);
    fprintfln(source, "    {");
    for (size_t i = 0; i < arrlen(parsed_types); ++i)
    {
        sds name_uc = sdsnew(parsed_types[i].name);
        sdstoupper(name_uc);
        fprintfln(source, "    case %s_%s:", base_uc, name_uc);
        sds name_lc = sdsnew(parsed_types[i].name);
        sdstolower(name_lc);
        fprintfln(source, "        return %1$s_%2$s_accept((%3$s%4$s *)%2$s, visitor, data);", name_lc, base_lc,
                  parsed_types[i].name, base_name);
    }
    fprintfln(source, "    }");
    fprintfln(source, "    __builtin_unreachable();");
    fprintfln(source, "}");

    for (size_t i = 0; i < arrlen(parsed_types); ++i)
    {
        define_type(header, source, base_name, base_lc, base_uc, parsed_types[i]);
    }

    fclose(header);
    fclose(source);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintfln(stderr, "Usage: %s <output directory>", argv[0]);
        return EX_USAGE;
    }
    char const *output_dir = argv[1];

    define_ast(output_dir, "Expr", "#include \"token.h\"\n",
               (char const *[]){
                   "Assign   : Token name, Expr * value",
                   "Binary   : Expr * left, Token operator, Expr * right",
                   "Grouping : Expr * expression",
                   "Literal  : Object * value",
                   "Unary    : Token operator, Expr * right",
                   "Variable : Token name",
                   NULL,
               });

    define_ast(output_dir, "Stmt", "#include \"expr.h\"\n",
               (char const *[]){
                   "Expression : Expr * expression",
                   "Print      : Expr * expression",
                   "Var        : Token name, Expr * initializer",
                   NULL,
               });
}
