subdir('source')

string_inc = include_directories('include')

string_lib = static_library(
    'string',
    string_src,
    include_directories : string_inc,
    dependencies : [span_dep],
)

string_dep = declare_dependency(
    link_with : string_lib,
    include_directories : string_inc,
    dependencies : [span_dep],
)
