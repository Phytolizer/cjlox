ast_generator_exe = executable(
  'lox-ast-gen',
  'main.c',
)

generated_ast = custom_target(
  'gen-ast',
  output : [
    'ast.h',
    'ast.c',
  ],
  input : [
    'lox.ast',
  ],
  command : [
    ast_generator_exe,
    '@INPUT@',
  ],
)
