#include "token.h"

void print_token(Token token, FILE *stream)
{
    print_token_type(token.type, stream);
    fprintf(stream, " %s ", token.lexeme);
    print_object(token.literal, stream);
}
