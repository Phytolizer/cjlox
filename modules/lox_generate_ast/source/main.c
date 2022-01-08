#include <nonstd/ctype.h>
#include <phyto/hash/hash.h>
#include <phyto/io/io.h>
#include <phyto/string/string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NS "lox"
#define NS_UPPER "LOX"

#define SSP(s) phyto_string_as_span(s)
#define SP(c) phyto_string_span_from_c(c)

#define SP_FMT PHYTO_STRING_FORMAT
#define STR_FMT PHYTO_STRING_FORMAT
#define SP_PRN PHYTO_STRING_VIEW_PRINTF_ARGS
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

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(toks, tok_t);
PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(toks, tok_t);

static void tok_free(tok_t* tok) {
    phyto_string_free(&tok->text);
}

static const toks_callbacks_t toks_callbacks = {
    .free_cb = tok_free,
};

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

static void lex_file(toks_t* toks, kwmap_t* kwmap, phyto_string_span_t input) {
    const char* start = input.begin;
    const char* current = start;

    while (current < input.end) {
        start = current;
        switch (*current) {
            case '[':
                current++;
                toks_append(toks, (tok_t){.type = ttype_lbrack});
                break;
            case ']':
                current++;
                toks_append(toks, (tok_t){.type = ttype_rbrack});
                break;
            case '{':
                current++;
                toks_append(toks, (tok_t){.type = ttype_lbrace});
                break;
            case '}':
                current++;
                toks_append(toks, (tok_t){.type = ttype_rbrace});
                break;
            case ':':
                current++;
                toks_append(toks, (tok_t){.type = ttype_colon});
                break;
            case ',':
                current++;
                toks_append(toks, (tok_t){.type = ttype_comma});
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
                    phyto_string_span_t ident = phyto_string_span_new(start, current);
                    ttype_t* res = kwmap_get_ref(kwmap, ident);
                    ttype_t type = res ? *res : ttype_ident;
                    toks_append(toks, ((tok_t){
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

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(sspans, phyto_string_span_t);
PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(sspans, phyto_string_span_t);

static const sspans_callbacks_t sspans_callbacks = {0};

typedef struct {
    phyto_string_span_t name;
    phyto_string_t type;
} field_t;

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(fields, field_t);
PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(fields, field_t);

static void field_free(field_t* f) {
    phyto_string_free(&f->type);
}

static const fields_callbacks_t fields_callbacks = {
    .free_cb = field_free,
};

typedef struct {
    phyto_string_span_t name;
    fields_t fields;
} node_t;

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(nodes, node_t);
PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(nodes, node_t);

static void node_free(node_t* node) {
    fields_free(&node->fields);
}

static const nodes_callbacks_t nodes_callbacks = {
    .free_cb = node_free,
};

typedef struct {
    stype_t type;
    union {
        sspans_t includes;
        node_t node;
    };
} stmt_t;

void stmt_free(stmt_t* stmt) {
    if (stmt->type == stype_node) {
        fields_free(&stmt->node.fields);
    } else {
        sspans_free(&stmt->includes);
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
            stmt.includes = sspans_init(&sspans_callbacks);
            ++p->pos;
            if (current(p)->type != ttype_lbrack) {
                fprintf(stderr, "expected '['\n");
                return (stmt_t){0};
            }
            ++p->pos;
            while (current(p)->type == ttype_ident) {
                phyto_string_span_t include = SSP(current(p)->text);
                fprintf(stderr, "include: %" SP_FMT "\n", SP_PRN(include));
                sspans_append(&stmt.includes, include);
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
            stmt.node.name = SSP(current(p)->text);
            stmt.node.fields = fields_init(&fields_callbacks);
            ++p->pos;
            if (current(p)->type != ttype_lbrace) {
                fprintf(stderr, "expected '{'\n");
                return (stmt_t){0};
            }
            ++p->pos;
            while (true) {
                field_t field = {.name = SSP(current(p)->text)};
                ++p->pos;
                if (current(p)->type != ttype_colon) {
                    stmt_free(&stmt);
                    fprintf(stderr, "expected ':'\n");
                    return (stmt_t){0};
                }
                ++p->pos;
                phyto_string_t type = phyto_string_new();
                phyto_string_vec_t parts = phyto_string_vec_init(&phyto_string_vec_callbacks);
                while (current(p)->type == ttype_ident) {
                    phyto_string_vec_append(&parts, phyto_string_copy(current(p)->text));
                    ++p->pos;
                }
                type = phyto_string_join(phyto_string_span_from_c(" "), parts);
                phyto_string_vec_free(&parts);
                field.type = type;
                fields_append(&stmt.node.fields, field);
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

static void print_discriminator_type_name(phyto_string_span_t name, FILE* output) {
    fprintf(output, NS "_%" SP_FMT "_type_t", SP_PRN(name));
}

static void print_base_class_name(phyto_string_span_t tree_name, FILE* output) {
    fprintf(output, NS "_%" SP_FMT "_t", SP_PRN(tree_name));
}

static void print_adjective_noun(phyto_string_span_t noun, phyto_string_span_t adj, FILE* output) {
    fprintf(output, "%" SP_FMT "_%" SP_FMT, SP_PRN(adj), SP_PRN(noun));
}

static void print_derived_type_name(phyto_string_span_t tree,
                                    phyto_string_span_t node,
                                    FILE* output) {
    fprintf(output, NS "_");
    print_adjective_noun(tree, node, output);
    fprintf(output, "_t");
}

static void dump_types(phyto_string_span_t tree_name, nodes_t nodes, FILE* output) {
    fprintf(output, "typedef enum {\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        const node_t* node = &nodes.data[i];
        fprintf(output, "    " NS "_%" SP_FMT "_type_%" SP_FMT ",\n", SP_PRN(tree_name),
                SP_PRN(node->name));
    }
    fprintf(output, "} ");
    print_discriminator_type_name(tree_name, output);
    fprintf(output, ";\n");
    fprintf(output, "typedef struct {\n");
    fprintf(output, "    ");
    print_discriminator_type_name(tree_name, output);
    fprintf(output, " type;\n");
    fprintf(output, "} ");
    print_base_class_name(tree_name, output);
    fprintf(output, ";\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output, "typedef struct {\n");
        fprintf(output, "    ");
        print_base_class_name(tree_name, output);
        fprintf(output, " base;\n");
        for (size_t j = 0; j < nodes.data[i].fields.size; ++j) {
            const field_t* field = &nodes.data[i].fields.data[j];
            if (phyto_string_span_equal(SSP(field->type), tree_name)) {
                fprintf(output, "    ");
                print_base_class_name(tree_name, output);
                fprintf(output, "* %" SP_FMT, SP_PRN(field->name));
                fprintf(output, ";\n");
            } else {
                fprintf(output, "    %" SP_FMT " %" SP_FMT ";\n", STR_PRN(field->type),
                        SP_PRN(field->name));
            }
        }
        fprintf(output, "} ");
        print_derived_type_name(tree_name, nodes.data[i].name, output);
        fprintf(output, ";\n");
    }
}

typedef struct {
    phyto_string_t signature;
} visitor_func_macro_t;

PHYTO_COLLECTIONS_DYNAMIC_ARRAY_DECL(visitor_func_macros, visitor_func_macro_t);
PHYTO_COLLECTIONS_DYNAMIC_ARRAY_IMPL(visitor_func_macros, visitor_func_macro_t);

static void visitor_func_macro_free(visitor_func_macro_t* mac) {
    phyto_string_free(&mac->signature);
}

static const visitor_func_macros_callbacks_t visitor_func_macro_callbacks = {
    .free_cb = visitor_func_macro_free,
};

static visitor_func_macros_t dump_visitor_func_macros(phyto_string_span_t tree_name,
                                                      nodes_t nodes,
                                                      FILE* output) {
    visitor_func_macros_t result = visitor_func_macros_init(&visitor_func_macro_callbacks);
    phyto_string_t tree_name_upper = phyto_string_upper(tree_name);
    for (size_t i = 0; i < nodes.size; ++i) {
        phyto_string_t node_name_upper = phyto_string_upper(nodes.data[i].name);
        phyto_string_t macro_signature = phyto_string_from_sprintf(
            NS_UPPER "_%" SP_FMT "_VISITOR_VISIT_%" SP_FMT "_FUNC(Ns, Name, T)",
            STR_PRN(tree_name_upper), STR_PRN(node_name_upper));
        phyto_string_free(&node_name_upper);
        fprintf(output, "#define %" STR_FMT " \\\n", STR_PRN(macro_signature));
        fprintf(output, "    T Name##_visit_");
        print_adjective_noun(tree_name, nodes.data[i].name, output);
        fprintf(output, "(Ns##_##Name##_t* visitor, ");
        print_derived_type_name(tree_name, nodes.data[i].name, output);
        fprintf(output, "* node)\n");
        visitor_func_macros_append(&result, (visitor_func_macro_t){macro_signature});
    }
    phyto_string_free(&tree_name_upper);
    return result;
}

static void dump_toplevel_header_macro(phyto_string_span_t tree_name,
                                       nodes_t nodes,
                                       visitor_func_macros_t visitor_func_macros,
                                       FILE* output) {
    phyto_string_t tree_name_upper = phyto_string_upper(tree_name);
    fprintf(output, "#define " NS_UPPER "_%" SP_FMT "_VISITOR_DECL(Ns, Name, T) \\\n",
            STR_PRN(tree_name_upper));
    fprintf(output, "    T " NS "_%" SP_FMT "_accept_##Name(", SP_PRN(tree_name));
    print_base_class_name(tree_name, output);
    fprintf(output, "* node, Ns##_##Name##_t* visitor); \\\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output,
                "    T " NS "_%" SP_FMT "_%" SP_FMT "_accept_##Name(" NS "_%" SP_FMT "_%" SP_FMT
                "_t* self, Ns##_##Name##_t* visitor); \\\n",
                SP_PRN(nodes.data[i].name), SP_PRN(tree_name), SP_PRN(nodes.data[i].name),
                SP_PRN(tree_name));
        fprintf(output, "    ");
        fprintf(output, "%" STR_FMT, STR_PRN(visitor_func_macros.data[i].signature));
        if (i < nodes.size - 1) {
            fprintf(output, "; \\");
        }
        fprintf(output, "\n");
    }
    phyto_string_free(&tree_name_upper);
}

static void dump_toplevel_source_macro(phyto_string_span_t tree_name, nodes_t nodes, FILE* output) {
    phyto_string_t tree_name_upper = phyto_string_upper(tree_name);
    fprintf(output, "#define " NS_UPPER "_%" SP_FMT "_VISITOR_IMPL(Ns, Name, T) \\\n",
            STR_PRN(tree_name_upper));
    fprintf(output,
            "    T " NS "_%" SP_FMT "_accept_##Name(" NS "_%" SP_FMT
            "_t* node, Ns##_##Name##_t* visitor) { \\\n",
            SP_PRN(tree_name), SP_PRN(tree_name));
    fprintf(output, "        switch (node->type) { \\\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output, "            case " NS "_%" SP_FMT "_type_%" SP_FMT ": \\\n",
                SP_PRN(tree_name), SP_PRN(nodes.data[i].name));
        fprintf(output,
                "                return " NS "_%" SP_FMT "_%" SP_FMT "_accept_##Name((" NS
                "_%" SP_FMT "_%" SP_FMT
                "_t*)node, "
                "visitor); \\\n",
                SP_PRN(nodes.data[i].name), SP_PRN(tree_name), SP_PRN(nodes.data[i].name),
                SP_PRN(tree_name));
    }
    fprintf(output, "        } \\\n");
    fprintf(output, "        return (T){0}; \\\n");
    fprintf(output, "    } \\\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        fprintf(output,
                "    T " NS "_%" SP_FMT "_%" SP_FMT "_accept_##Name(" NS "_%" SP_FMT "_%" SP_FMT
                "_t* self, Ns##_##Name##_t* visitor) { \\\n",
                SP_PRN(nodes.data[i].name), SP_PRN(tree_name), SP_PRN(nodes.data[i].name),
                SP_PRN(tree_name));
        fprintf(output, "        return Name##_visit_%" SP_FMT "_%" SP_FMT "(visitor, self); \\\n",
                SP_PRN(nodes.data[i].name), SP_PRN(tree_name));
        if (i < nodes.size - 1) {
            fprintf(output, "    } \\\n");
        } else {
            fprintf(output, "    }\n");
        }
    }
    phyto_string_free(&tree_name_upper);
}

static void print_constructor_signature(phyto_string_span_t tree_name, node_t* node, FILE* output) {
    print_derived_type_name(tree_name, node->name, output);
    fprintf(output, "* " NS "_%" SP_FMT "_new_%" SP_FMT "(", SP_PRN(tree_name), SP_PRN(node->name));
    for (size_t j = 0; j < node->fields.size; ++j) {
        phyto_string_t field_type =
            phyto_string_span_equal(phyto_string_as_span(node->fields.data[j].type), tree_name)
                ? phyto_string_from_sprintf(NS "_%" SP_FMT "_t*", SP_PRN(tree_name))
                : phyto_string_copy(node->fields.data[j].type);
        fprintf(output, "%" STR_FMT " %" SP_FMT, STR_PRN(field_type),
                SP_PRN(node->fields.data[j].name));
        phyto_string_free(&field_type);
        if (j < node->fields.size - 1) {
            fprintf(output, ", ");
        }
    }
    fprintf(output, ")");
}

static void print_free_fn_signature(phyto_string_span_t tree_name, FILE* output) {
    fprintf(output, "void " NS "_%" SP_FMT "_free(", SP_PRN(tree_name));
    print_base_class_name(tree_name, output);
    fprintf(output, "* node)");
}

static void dump_source_file_decls(phyto_string_span_t tree_name, nodes_t nodes, FILE* output) {
    print_free_fn_signature(tree_name, output);
    fprintf(output, ";\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        print_constructor_signature(tree_name, &nodes.data[i], output);
        fprintf(output, ";\n");
    }
}

static void dump_static_free_functions(phyto_string_span_t tree_name, nodes_t nodes, FILE* output) {
    for (size_t i = 0; i < nodes.size; ++i) {
        node_t* node = &nodes.data[i];
        fprintf(output, "static void free_");
        print_adjective_noun(tree_name, node->name, output);
        fprintf(output, "(");
        print_derived_type_name(tree_name, node->name, output);
        fprintf(output, "* node) {\n");
        fprintf(output, "    if (node == NULL) {\n        return;\n    }\n");
        for (size_t j = 0; j < node->fields.size; ++j) {
            field_t* field = &node->fields.data[j];
            bool recursive = phyto_string_span_equal(phyto_string_as_span(field->type), tree_name);
            phyto_string_t field_type =
                recursive ? phyto_string_from_sprintf(NS "_%" SP_FMT "_t*", SP_PRN(tree_name))
                          : phyto_string_copy(field->type);
            bool field_type_is_pointer = phyto_string_ends_with(phyto_string_as_span(field_type),
                                                                phyto_string_span_from_c("*"));
            bool field_type_is_const = phyto_string_starts_with(phyto_string_as_span(field_type),
                                                                phyto_string_span_from_c("const "));
            if (field_type_is_pointer) {
                if (!field_type_is_const) {
                    if (recursive) {
                        fprintf(output, "    ");
                        fprintf(output, "if (node->%" SP_FMT " != NULL) {\n", SP_PRN(field->name));
                        fprintf(output, "        ");
                        fprintf(output, NS "_%" SP_FMT "_free(node->%" SP_FMT ");\n",
                                SP_PRN(tree_name), SP_PRN(field->name));
                        fprintf(output, "    }\n");
                    } else {
                        phyto_string_t field_type_name = phyto_string_remove_suffix(
                            phyto_string_as_span(field_type), phyto_string_span_from_c("_t*"));
                        fprintf(output, "    ");
                        fprintf(output, "if (node->%" SP_FMT " != NULL) {\n", SP_PRN(field->name));
                        fprintf(output, "        ");
                        fprintf(output, "%" STR_FMT "_free(node->%" SP_FMT ");\n",
                                STR_PRN(field_type_name), SP_PRN(field->name));
                        phyto_string_free(&field_type_name);
                    }
                }
            } else {
                phyto_string_t field_type_name = phyto_string_remove_suffix(
                    phyto_string_as_span(field_type), phyto_string_span_from_c("_t"));
                fprintf(output, "    ");
                fprintf(output, "%" STR_FMT "_free(&node->%" SP_FMT ");\n",
                        STR_PRN(field_type_name), SP_PRN(field->name));
                phyto_string_free(&field_type_name);
            }
            phyto_string_free(&field_type);
        }
        fprintf(output, "}\n");
    }
}

static void dump_constructors(phyto_string_span_t tree_name, nodes_t nodes, FILE* output) {
    for (size_t i = 0; i < nodes.size; ++i) {
        node_t* node = &nodes.data[i];
        print_derived_type_name(tree_name, node->name, output);
        fprintf(output, "* " NS "_%" SP_FMT "_new_%" SP_FMT, SP_PRN(tree_name), SP_PRN(node->name));
        fprintf(output, "(");
        for (size_t j = 0; j < node->fields.size; ++j) {
            field_t* field = &node->fields.data[j];
            phyto_string_t field_type =
                phyto_string_span_equal(phyto_string_as_span(field->type), tree_name)
                    ? phyto_string_from_sprintf(NS "_%" SP_FMT "_t*", SP_PRN(tree_name))
                    : phyto_string_copy(field->type);
            fprintf(output, "%" STR_FMT " %" SP_FMT, STR_PRN(field_type), SP_PRN(field->name));
            phyto_string_free(&field_type);
            if (j < node->fields.size - 1) {
                fprintf(output, ", ");
            }
        }
        fprintf(output, ") {\n");
        fprintf(output, "    ");
        print_derived_type_name(tree_name, node->name, output);
        fprintf(output, "* node = calloc(1, sizeof(");
        print_derived_type_name(tree_name, node->name, output);
        fprintf(output, "));\n");
        fprintf(output, "    node->base.type = " NS "_%" SP_FMT "_type_%" SP_FMT ";\n",
                SP_PRN(tree_name), SP_PRN(node->name));
        for (size_t j = 0; j < node->fields.size; ++j) {
            field_t* field = &node->fields.data[j];
            fprintf(output, "    node->%" SP_FMT " = %" SP_FMT ";\n", SP_PRN(field->name),
                    SP_PRN(field->name));
        }
        fprintf(output, "    return node;\n");
        fprintf(output, "}\n");
    }
}

static void dump_source_file(phyto_string_span_t tree_name,
                             nodes_t nodes,
                             const char* output_path,
                             FILE* output) {
    fprintf(output, "#include \"%s\"\n", output_path);
    fprintf(output, "#include <stdlib.h>\n");

    dump_static_free_functions(tree_name, nodes, output);

    print_free_fn_signature(tree_name, output);
    fprintf(output, " {\n");
    fprintf(output, "    switch (node->type) {\n");
    for (size_t i = 0; i < nodes.size; ++i) {
        node_t* node = &nodes.data[i];
        fprintf(output, "        case " NS "_%" SP_FMT "_type_%" SP_FMT ":\n", SP_PRN(tree_name),
                SP_PRN(node->name));
        fprintf(output,
                "            free_%" SP_FMT "_%" SP_FMT "((" NS "_%" SP_FMT "_%" SP_FMT
                "_t*)node);\n",
                SP_PRN(node->name), SP_PRN(tree_name), SP_PRN(node->name), SP_PRN(tree_name));
        fprintf(output, "            break;\n");
    }
    fprintf(output, "    }\n");
    fprintf(output, "    free(node);\n");
    fprintf(output, "}\n");

    dump_constructors(tree_name, nodes, output);
}

static void parse_def(parser_t* p,
                      const char* header_path,
                      FILE* header_output,
                      FILE* source_output) {
    ++p->pos;
    const tok_t* ident = current(p);
    if (ident->type != ttype_ident) {
        fprintf(stderr, "expected ident after def\n");
        return;
    }
    phyto_string_span_t tree_name = SSP(ident->text);
    ++p->pos;

    fprintf(header_output, "#ifndef " NS_UPPER "_AST_H_\n");
    fprintf(header_output, "#define " NS_UPPER "_AST_H_\n");

    nodes_t nodes = nodes_init(&nodes_callbacks);
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
                fprintf(header_output, "#include <%" SP_FMT ">\n", SP_PRN(stmt.includes.data[i]));
            }
            sspans_free(&stmt.includes);
        } else {
            nodes_append(&nodes, stmt.node);
        }
    }
    // consume 'end'
    ++p->pos;

    dump_types(tree_name, nodes, header_output);
    visitor_func_macros_t visitor_func_macros =
        dump_visitor_func_macros(tree_name, nodes, header_output);
    dump_toplevel_header_macro(tree_name, nodes, visitor_func_macros, header_output);
    dump_toplevel_source_macro(tree_name, nodes, header_output);
    fprintf(header_output, "#endif\n");

    dump_source_file_decls(tree_name, nodes, header_output);

    dump_source_file(tree_name, nodes, header_path, source_output);

    // free
    visitor_func_macros_free(&visitor_func_macros);
    nodes_free(&nodes);
}

static void parse_file(const toks_t* toks,
                       const char* header_path,
                       FILE* header_output,
                       FILE* source_output) {
    parser_t p = {.toks = toks, .pos = 0};

    while (current(&p)->type == ttype_def) {
        parse_def(&p, header_path, header_output, source_output);
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input> <header_output> <source_output>\n", argv[0]);
        return 1;
    }
    phyto_string_t input = phyto_io_read_file(argv[1]);
    if (input.size == 0) {
        fprintf(stderr, "Failed to read input file: %s\n", argv[1]);
        return 1;
    }
    FILE* header_output = fopen(argv[2], "w");
    if (!header_output) {
        fprintf(stderr, "Failed to open header output file: %s\n", argv[2]);
        return 1;
    }
    FILE* source_output = fopen(argv[3], "w");
    if (!source_output) {
        fprintf(stderr, "Failed to open source output file: %s\n", argv[3]);
        return 1;
    }
    kwmap_t* kwmap = kwmap_new(20, phyto_hash_default_load, &kwmap_key_ops, &kwmap_value_ops);
    kwmap_insert(kwmap, SP("def"), ttype_def);
    kwmap_insert(kwmap, SP("includes"), ttype_includes);
    kwmap_insert(kwmap, SP("end"), ttype_end);

    toks_t toks = toks_init(&toks_callbacks);

    lex_file(&toks, kwmap, phyto_string_as_span(input));

    if (toks.size == 0) {
        fprintf(stderr, "Failed to lex input file: %s (no tokens)\n", argv[1]);
        return 1;
    }

    parse_file(&toks, argv[2], header_output, source_output);

    for (size_t i = 0; i < toks.size; ++i) {
        phyto_string_free(&toks.data[i].text);
    }
    toks_free(&toks);

    kwmap_free(kwmap);
    fclose(source_output);
    fclose(header_output);
    phyto_string_free(&input);
    return 0;
}
