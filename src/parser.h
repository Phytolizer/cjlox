#pragma once

#include "stmt.h"
#include "token.h"

typedef struct Parser
{
    Token *tokens;
    int current;
} Parser;

Parser new_parser(Token *tokens);
Stmt **parser_parse(Parser *parser);
