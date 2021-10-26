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

ast_lib = static_library(
  'lox-ast',
  generated_ast,
)

executable(
  'lox',
  'keyword_table.c',
  'lox.c',
  'object.c',
  'scanner.c',
  'token.c',
  'token_type.c',
  link_with : ast_lib,
)
