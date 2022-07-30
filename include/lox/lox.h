#pragma once

typedef struct {
} Lox;

void lox_init(Lox* lox);
void lox_free(Lox* lox);
int lox_run_file(Lox* lox, const char* path);
int lox_run_prompt(Lox* lox);
