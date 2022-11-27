cmake_minimum_required(VERSION 3.23)
project(
  sysexits
  LANGUAGES C
  DESCRIPTION "cross-platform sysexits.h"
)
add_library(sysexits INTERFACE)
target_include_directories(sysexits INTERFACE include)
add_library(sysexits::sysexits ALIAS sysexits)
