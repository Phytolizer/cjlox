#include <args/args.h>
#include <lox/lox.h>
#include <stdio.h>
#include <string/string.h>

int main(int argc, char** argv) {
    args_t args = ARGS_NEW(argv, argc);

    if (args.length > 2) {
        fprintf(stderr, "Usage: %s [script]\n", args.begin[0]);
        return 1;
    }

    lox_t lox = {0};

    if (args.length == 2) {
        return run_file(&lox, string_from_c(args.begin[1]));
    }
    return run_prompt(&lox);
}
