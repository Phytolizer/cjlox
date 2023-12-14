#pragma once

#include "hedley.h"
#include "token.h"

#include <stdarg.h>

void error(int line, char const *fmt, ...) HEDLEY_PRINTF_FORMAT(2, 3);
void verror(int line, char const *fmt, va_list args) HEDLEY_PRINTF_FORMAT(2, 0);
void error_token(Token token, char const *fmt, ...) HEDLEY_PRINTF_FORMAT(2, 3);
void verror_token(Token token, char const *fmt, va_list args) HEDLEY_PRINTF_FORMAT(2, 0);
void report(int line, char const *where, char const *fmt, ...) HEDLEY_PRINTF_FORMAT(3, 4);
void vreport(int line, char const *where, char const *fmt, va_list args) HEDLEY_PRINTF_FORMAT(3, 0);

void runtime_error(Token token, sds message);
