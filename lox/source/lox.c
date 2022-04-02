#include "lox/lox.h"

#include <stdlib.h>

int run_file(lox_t* lox, string_t filename) {
    (void)lox;
    free(filename.begin);
    return 0;
}

int run_prompt(lox_t* lox) {
    (void)lox;
    return 0;
}
