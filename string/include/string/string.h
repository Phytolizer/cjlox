#pragma once

#include <span/span.h>
#include <stdio.h>

typedef SPAN_TYPE(char) string_t;

#define STRING_C(constant) ((string_t)SPAN_NEW((constant), sizeof(constant) - 1))

void string_print(string_t str, FILE* stream);
