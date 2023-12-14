#include "println.h"

#include <stdarg.h>
#include <stdio.h>

int printfln(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vfprintfln(stdout, fmt, args);
    va_end(args);
    return ret;
}

int println(void)
{
    return fputs("\n", stdout);
}

int fprintfln(FILE *stream, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vfprintfln(stream, fmt, args);
    va_end(args);
    return ret;
}

int fprintln(FILE *stream)
{
    return fputs("\n", stream);
}

int vprintfln(char const *fmt, va_list args)
{
    return vfprintfln(stdout, fmt, args);
}

int vfprintfln(FILE *stream, char const *fmt, va_list args)
{
    int ret = vfprintf(stream, fmt, args);
    if (ret < 0)
    {
        return ret;
    }
    ret += fputs("\n", stream);
    return ret;
}
