#pragma once

#include "object.h"
#include "token_type.h"

#include <stdio.h>

typedef struct Token
{
    TokenType type;
    sds lexeme;
    Object *literal;
    int line;
} Token;

void print_token(Token token, FILE *stream);
