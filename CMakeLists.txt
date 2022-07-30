cmake_minimum_required(VERSION 3.23)

project(cjlox LANGUAGES C)

add_library(lox src/lox/lox.c src/lox/run.c)
target_include_directories(lox PUBLIC include)

add_executable(cjlox src/cjlox/main.c)
target_link_libraries(cjlox PRIVATE lox)
