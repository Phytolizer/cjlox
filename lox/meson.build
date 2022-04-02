subdir('source')

lox_inc = include_directories('include')

lox_lib = static_library(
    'lox',
    lox_src,
    include_directories : lox_inc,
    dependencies : [span_dep, string_dep],
)

lox_dep = declare_dependency(
    link_with : lox_lib,
    include_directories : lox_inc,
    dependencies : [span_dep, string_dep],
)
