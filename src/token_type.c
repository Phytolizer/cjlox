#include "token_type.h"

void print_token_type(TokenType type, FILE *stream)
{
    switch (type)
    {
#define X(x)                                                                                                           \
    case TOKEN_##x:                                                                                                    \
        fprintf(stream, #x);                                                                                           \
        break;
        TOKEN_TYPES_X
#undef X
    default:
        break;
    }
}
