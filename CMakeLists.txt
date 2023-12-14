cmake_minimum_required(VERSION 3.27)

project(cjlox LANGUAGES C)

add_library(sds STATIC sds/sds.c)
target_include_directories(sds PRIVATE sds SYSTEM PUBLIC sds)
add_library(sds::sds ALIAS sds)

add_library(stb STATIC stb/stb.c)
target_include_directories(stb PRIVATE stb SYSTEM PUBLIC stb)
add_library(stb::stb ALIAS stb)

add_executable(astgen src/astgen.c src/println.c)
target_link_libraries(astgen PRIVATE sds::sds stb::stb)
add_executable(astgen::astgen ALIAS astgen)

set(EXTRA_SRC ${CMAKE_CURRENT_BINARY_DIR}/src)

add_custom_command(
    OUTPUT ${EXTRA_SRC}/expr.c ${EXTRA_SRC}/expr.h ${EXTRA_SRC}/stmt.c ${EXTRA_SRC}/stmt.h
    COMMAND astgen::astgen ${EXTRA_SRC}
    DEPENDS astgen::astgen
)

add_executable(lox
    src/ast_printer.c
    src/environment.c
    src/interpreter.c
    src/object.c
    src/parser.c
    src/println.c
    src/scanner.c
    src/token.c
    src/token_type.c
    src/main.c
)
target_include_directories(lox PRIVATE src ${EXTRA_SRC})
target_link_libraries(lox PRIVATE sds::sds edit stb::stb)
target_sources(lox PRIVATE ${EXTRA_SRC}/expr.c ${EXTRA_SRC}/expr.h ${EXTRA_SRC}/stmt.c ${EXTRA_SRC}/stmt.h)
