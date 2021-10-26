project(
  'cjlox',
  'c',
  version : '0.1.0',
  default_options : [
    'c_std=c17',
    'warning_level=3',
  ],
)

subdir('tool')

incdir = include_directories('include')

ast_lib = static_library(
  'lox-ast',
  generated_ast,
  include_directories : incdir,
)

executable(
  'lox',
  'source' / 'ast_printer.c',
  'source' / 'expr_destructors.c',
  'source' / 'keyword_table.c',
  'source' / 'lox.c',
  'source' / 'object.c',
  'source' / 'parser.c',
  'source' / 'scanner.c',
  'source' / 'token.c',
  'source' / 'token_type.c',
  link_with : ast_lib,
  include_directories : incdir,
)
