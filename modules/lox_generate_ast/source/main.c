#include <nonstd/ctype.h>
#include <phyto/hash/hash.h>
#include <phyto/io/io.h>
#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NS "lox"
#define NS_UPPER "LOX"

#define SSV(s) phyto_string_view(s)
#define SV(c) phyto_string_view_from_c(c)

#define SV_FMT PHYTO_STRING_FORMAT
#define STR_FMT PHYTO_STRING_FORMAT
#define SV_PRN PHYTO_STRING_VIEW_PRINTF_ARGS
#define STR_PRN PHYTO_STRING_PRINTF_ARGS

#define TOKS_X  \
    X(def)      \
    X(ident)    \
    X(includes) \
    X(lbrack)   \
    X(rbrack)   \
    X(lbrace)   \
    X(rbrace)   \
    X(colon)    \
    X(comma)    \
    X(end)

typedef enum {
#define X(x) ttype_##x,
    TOKS_X
#undef X
} ttype_t;

static const char* const ttype_names[] = {
#define X(x) #x,
    TOKS_X
#undef X
};

typedef struct {
    ttype_t type;
    phyto_string_t text;
} tok_t;

typedef PHYTO_VEC_TYPE(tok_t) toks_t;

#define TOKS_INIT PHYTO_VEC_INIT
#define TOKS_PUSH PHYTO_VEC_PUSH
#define TOKS_FREE PHYTO_VEC_FREE

PHYTO_HASH_DECL(kwmap, ttype_t);
PHYTO_HASH_IMPL(kwmap, ttype_t);

int32_t ttype_cmp(ttype_t a, ttype_t b) {
    return (int32_t)a - (int32_t)b;
}

static const kwmap_key_ops_t kwmap_key_ops = {
    .hash = phyto_hash_djb2,
};

static const kwmap_value_ops_t kwmap_value_ops = {
    .compare = ttype_cmp,
};

static bool is_ident_start(char c) {
    return nonstd_isalpha(c) || c == '_';
}

static bool is_ident_part(char c) {
    return is_ident_start(c) || c == '/' || c == '.' || nonstd_isdigit(c);
}

static void lex_file(toks_t* toks, kwmap_t* kwmap, phyto_string_view_t input) {
    const char* start = input.begin;
    const char* current = start;

    while (current < input.end) {
        start = current;
        switch (*current) {
            case '[':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_lbrack});
                break;
            case ']':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_rbrack});
                break;
            case '{':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_lbrace});
                break;
            case '}':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_rbrace});
                break;
            case ':':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_colon});
                break;
            case ',':
                current++;
                TOKS_PUSH(toks, (tok_t){.type = ttype_comma});
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                current++;
                break;

            default:
                if (is_ident_start(*current)) {
                    current++;
                    while (current < input.end && is_ident_part(*current)) {
                        current++;
                    }
                    phyto_string_view_t ident = phyto_string_view_new(start, current);
                    ttype_t* res = kwmap_get_ref(kwmap, ident);
                    ttype_t type = res ? *res : ttype_ident;
                    TOKS_PUSH(toks, ((tok_t){
                                        .type = type,
                                        .text = type == ttype_ident ? phyto_string_own(ident)
                                                                    : (phyto_string_t){0},
                                    }));
                } else {
                    current++;
                }
        }
    }
}

typedef struct {
    const toks_t* toks;
    size_t pos;
} parser_t;

typedef enum {
    stype_invalid,
    stype_includes,
    stype_node,
} stype_t;

typedef PHYTO_VEC_TYPE(phyto_string_view_t) sviews_t;

typedef struct {
    phyto_string_view_t name;
    phyto_string_t type;
} field_t;

typedef PHYTO_VEC_TYPE(field_t) fields_t;

typedef struct {
    phyto_string_view_t name;
    fields_t fields;
} node_t;

typedef PHYTO_VEC_TYPE(node_t) nodes_t;

typedef struct {
    stype_t type;
    union {
        sviews_t includes;
        node_t node;
    };
} stmt_t;

static void fields_push(fields_t* fields, field_t field) {
    PHYTO_VEC_PUSH(fields, field);
}

void fields_free(fields_t* fields) {
    for (size_t i = 0; i < fields->size; i++) {
        field_t* field = &fields->data[i];
        phyto_string_free(&field->type);
    }
    PHYTO_VEC_FREE(fields);
}

#define SVIEWS_PUSH PHYTO_VEC_PUSH
#define SVIEWS_FREE PHYTO_VEC_FREE

void stmt_free(stmt_t* stmt) {
    if (stmt->type == stype_node) {
        fields_free(&stmt->node.fields);
    } else {
        SVIEWS_FREE(&stmt->includes);
    }
}

const tok_t* current(const parser_t* p) {
    if (p->pos >= p->toks->size) {
        return &p->toks->data[p->toks->size - 1];
    }
    return &p->toks->data[p->pos];
}

static stmt_t parse_stmt(parser_t* p) {
    stmt_t stmt = {0};
    switch (current(p)->type) {
        case ttype_includes:
            stmt.type = stype_includes;
            ++p->pos;
            if (current(p)->type != ttype_lbrack) {
                fprintf(stderr, "expected '['\n");
                return (stmt_t){0};
            }
            ++p->pos;
            while (current(p)->type == ttype_ident) {
                phyto_string_view_t include = SSV(current(p)->text);
                fprintf(stderr, "include: %" SV_FMT "\n", SV_PRN(include));
                SVIEWS_PUSH(&stmt.includes, include);
                ++p->pos;
            }
            if (current(p)->type != ttype_rbrack) {
                stmt_free(&stmt);
                fprintf(stderr, "expected ']'\n");
                return (stmt_t){0};
            }
            ++p->pos;
            return stmt;
        case ttype_ident:
            stmt.type = stype_node;
            stmt.node.name = SSV(current(p)->text);
            ++p->pos;
            if (current(p)->type != ttype_lbrace) {
                fprintf(stderr, "expected '{'\n");
                return (stmt_t){0};
            }
            ++p->pos;
            while (true) {
                field_t field = {.name = SSV(current(p)->text)};
                ++p->pos;
                if (current(p)->type != ttype_colon) {
                    stmt_free(&stmt);
                    fprintf(stderr, "expected ':'\n");
                    return (stmt_t){0};
                }
                ++p->pos;
                phyto_string_t type = phyto_string_new();
                while (current(p)->type == ttype_ident) {
                    phyto_string_append_view(&type, SSV(current(p)->text));
                    ++p->pos;
                }
                field.type = type;
                fields_push(&stmt.node.fields, field);
                if (current(p)->type == ttype_comma) {
                    ++p->pos;
                } else if (current(p)->type == ttype_rbrace) {
                    ++p->pos;
                    break;
                } else {
                    stmt_free(&stmt);
                    fprintf(stderr, "expected ',' or '}'\n");
                    return (stmt_t){0};
                }
            }
            return stmt;
        default:
            return (stmt_t){0};
    }
}

static void parse_def(parser_t* p, FILE* output) {
    ++p->pos;
    const tok_t* ident = current(p);
    if (ident->type != ttype_ident) {
        fprintf(stderr, "expected ident after def\n");
        return;
    }
    phyto_string_view_t tree_name = SSV(ident->text);
    ++p->pos;
    nodes_t nodes;
    PHYTO_VEC_INIT(&nodes);
    while (current(p)->type != ttype_end) {
        fprintf(stderr, "%s\n", ttype_names[current(p)->type]);
        stmt_t stmt = parse_stmt(p);
        if (stmt.type == stype_invalid) {
            ++p->pos;
            continue;
        }
        if (stmt.type == stype_includes) {
            for (size_t i = 0; i < stmt.includes.size; ++i) {
                // includes always come first
                fprintf(output, "#include <%" SV_FMT ">\n", SV_PRN(stmt.includes.data[i]));
            }
            SVIEWS_FREE(&stmt.includes);
        } else {
            PHYTO_VEC_PUSH(&nodes, stmt.node);
        }
    }
    // consume 'end'
    ++p->pos;

    // begin output phase
    fprintf(output, "typedef enum {\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        const node_t* node = &nodes.data[i];
        fprintf(output, "    " NS "_%" SV_FMT "_type_%" SV_FMT ",\n", SV_PRN(tree_name),
                SV_PRN(node->name));
    }
    fprintf(output, "} " NS "_%" SV_FMT "_type_t;\n", SV_PRN(tree_name));
    fprintf(output, "typedef struct {\n");
    fprintf(output, "    " NS "_%" SV_FMT "_type_t type;\n", SV_PRN(tree_name));
    fprintf(output, "} " NS "_%" SV_FMT "_t;\n", SV_PRN(tree_name));
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output, "typedef struct {\n");
        fprintf(output, "    " NS "_%" SV_FMT "_t base;\n", SV_PRN(tree_name));
        for (size_t j = 0; j < nodes.data[i].fields.size; ++j) {
            const field_t* field = &nodes.data[i].fields.data[j];
            if (phyto_string_view_equal(SSV(field->type), tree_name)) {
                fprintf(output, "    " NS "_%" SV_FMT "_t* %" SV_FMT ";\n", SV_PRN(tree_name),
                        SV_PRN(field->name));
            } else {
                fprintf(output, "    %" SV_FMT " %" SV_FMT ";\n", STR_PRN(field->type),
                        SV_PRN(field->name));
            }
        }
        fprintf(output, "} " NS "_%" SV_FMT "_%" SV_FMT "_t;\n", SV_PRN(nodes.data[i].name),
                SV_PRN(tree_name));
    }
    phyto_string_t tree_name_upper = phyto_string_upper(tree_name);
    for (size_t i = 0; i < nodes.size; ++i) {
        phyto_string_t node_name_upper = phyto_string_upper(nodes.data[i].name);
        fprintf(output,
                "#define " NS_UPPER "_%" SV_FMT "_VISITOR_VISIT_%" SV_FMT "_FUNC(Name, T) \\\n",
                STR_PRN(tree_name_upper), STR_PRN(node_name_upper));
        phyto_string_free(&node_name_upper);
        fprintf(output,
                "    T Name##_visit_%" SV_FMT "_%" SV_FMT "(Name##_t* visitor, " NS "_%" SV_FMT
                "_%" SV_FMT "_t* node)\n",
                SV_PRN(nodes.data[i].name), SV_PRN(tree_name), SV_PRN(nodes.data[i].name),
                SV_PRN(tree_name));
    }
    fprintf(output, "#define " NS_UPPER "_%" SV_FMT "_VISITOR_DECL(Name, T) \\\n",
            STR_PRN(tree_name_upper));
    fprintf(output,
            "    T " NS "_%" SV_FMT "_accept_##Name(" NS "_%" SV_FMT
            "_t* node, Name##_t* visitor); \\\n",
            SV_PRN(tree_name), SV_PRN(tree_name));
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output,
                "    T " NS "_%" SV_FMT "_%" SV_FMT "_accept_##Name(" NS "_%" SV_FMT "_%" SV_FMT
                "_t* self, Name##_t* visitor); \\\n",
                SV_PRN(nodes.data[i].name), SV_PRN(tree_name), SV_PRN(nodes.data[i].name),
                SV_PRN(tree_name));
        phyto_string_t node_name_upper = phyto_string_upper(nodes.data[i].name);
        fprintf(output, "    " NS_UPPER "_%" SV_FMT "_VISITOR_VISIT_%" SV_FMT "_FUNC(Name, T)",
                STR_PRN(tree_name_upper), STR_PRN(node_name_upper));
        phyto_string_free(&node_name_upper);
        if (i < nodes.size - 1) {
            fprintf(output, "; \\");
        }
        fprintf(output, "\n");
    }
    fprintf(output, "#define " NS_UPPER "_%" SV_FMT "_VISITOR_IMPL(Name, T) \\\n",
            STR_PRN(tree_name_upper));
    phyto_string_free(&tree_name_upper);
    fprintf(output,
            "    T " NS "_%" SV_FMT "_accept_##Name(" NS "_%" SV_FMT
            "_t* node, Name##_t* visitor) { \\\n",
            SV_PRN(tree_name), SV_PRN(tree_name));
    fprintf(output, "        switch (node->type) { \\\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output, "            case " NS "_%" SV_FMT "_type_%" SV_FMT ": \\\n",
                SV_PRN(tree_name), SV_PRN(nodes.data[i].name));
        fprintf(output,
                "                return " NS "_%" SV_FMT "_%" SV_FMT "_accept_##Name((" NS
                "_%" SV_FMT "_%" SV_FMT
                "_t*)node, "
                "visitor); \\\n",
                SV_PRN(nodes.data[i].name), SV_PRN(tree_name), SV_PRN(nodes.data[i].name),
                SV_PRN(tree_name));
    }
    fprintf(output, "        } \\\n");
    fprintf(output, "        return (T){0}; \\\n");
    fprintf(output, "    } \\\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output,
                "    T " NS "_%" SV_FMT "_%" SV_FMT "_accept_##Name(" NS "_%" SV_FMT "_%" SV_FMT
                "_t* self, Name##_t* visitor) { \\\n",
                SV_PRN(nodes.data[i].name), SV_PRN(tree_name), SV_PRN(nodes.data[i].name),
                SV_PRN(tree_name));
        fprintf(output, "        return Name##_visit_%" SV_FMT "_%" SV_FMT "(visitor, self); \\\n",
                SV_PRN(nodes.data[i].name), SV_PRN(tree_name));
        if (i < nodes.size - 1) {
            fprintf(output, "    } \\\n");
        } else {
            fprintf(output, "    }\n");
        }
    }

    // free
    for (size_t i = 0; i < nodes.size; ++i) {
        fields_free(&nodes.data[i].fields);
    }
    PHYTO_VEC_FREE(&nodes);
}

static void parse_file(const toks_t* toks, FILE* output) {
    parser_t p = {.toks = toks, .pos = 0};

    while (current(&p)->type == ttype_def) {
        parse_def(&p, output);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }
    phyto_string_t input = phyto_io_read_file(argv[1]);
    if (input.size == 0) {
        fprintf(stderr, "Failed to read input file: %s\n", argv[1]);
        return 1;
    }
    FILE* output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "Failed to open output file: %s\n", argv[2]);
        return 1;
    }
    kwmap_t* kwmap = kwmap_new(20, phyto_hash_default_load, &kwmap_key_ops, &kwmap_value_ops);
    kwmap_insert(kwmap, SV("def"), ttype_def);
    kwmap_insert(kwmap, SV("includes"), ttype_includes);
    kwmap_insert(kwmap, SV("end"), ttype_end);

    toks_t toks;
    TOKS_INIT(&toks);

    lex_file(&toks, kwmap, phyto_string_view(input));

    if (toks.size == 0) {
        fprintf(stderr, "Failed to lex input file: %s (no tokens)\n", argv[1]);
        return 1;
    }

    parse_file(&toks, output);

    for (size_t i = 0; i < toks.size; ++i) {
        phyto_string_free(&toks.data[i].text);
    }
    TOKS_FREE(&toks);

    kwmap_free(kwmap);
    fclose(output);
    phyto_string_free(&input);
    return 0;
}
