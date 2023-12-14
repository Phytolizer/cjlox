#pragma once

#include "hedley.h"

#include <stdarg.h>
#include <stdio.h>

int printfln(char const *fmt, ...) HEDLEY_PRINTF_FORMAT(1, 2);
int println(void);
int fprintfln(FILE *stream, char const *fmt, ...) HEDLEY_PRINTF_FORMAT(2, 3);
int fprintln(FILE *stream);
int vprintfln(char const *fmt, va_list args) HEDLEY_PRINTF_FORMAT(1, 0);
int vfprintfln(FILE *stream, char const *fmt, va_list args) HEDLEY_PRINTF_FORMAT(2, 0);
