#pragma once

#include <stdbool.h>
#include <string/string.h>

typedef struct {
    bool had_error;
} lox_t;

int run_file(lox_t* lox, string_t filename);
int run_prompt(lox_t* lox);
