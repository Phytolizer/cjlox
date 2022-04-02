#include "string/string.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

string_t string_from_c(const char* c) {
    size_t len = strlen(c);
    char* begin = malloc(len + 1);
    strncpy(begin, c, len + 1);
    return (string_t)SPAN_NEW(begin, len);
}

void string_print(string_t str, FILE* stream) {
    fprintf(stream, "%.*s", (int)str.length, str.begin);
}

void string_println(string_t str, FILE* stream) {
    fprintf(stream, "%.*s\n", (int)str.length, str.begin);
}
