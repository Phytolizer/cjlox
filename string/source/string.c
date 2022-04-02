#include "string/string.h"

#include <stdio.h>

void string_print(string_t str, FILE* stream) {
    fprintf(stream, "%.*s", (int)str.length, str.begin);
}

void string_println(string_t str, FILE* stream) {
    fprintf(stream, "%.*s\n", (int)str.length, str.begin);
}
