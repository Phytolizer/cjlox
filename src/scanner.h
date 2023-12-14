#pragma once

#include "token.h"

#include <sds.h>

typedef struct Scanner
{
    sds source;
    Token *tokens;
    int start;
    int current;
    int line;
} Scanner;

Scanner new_scanner(sds source);
Token *scan_tokens(Scanner *scanner);
