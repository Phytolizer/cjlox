#include <lox/lox.h>
#include <stdio.h>
#include <sysexits/sysexits.h>

int main(int argc, char** argv) {
    if (argc > 2) {
        printf("Usage: %s [script]\n", argv[0]);
        return EX_USAGE;
    }
    lox_context_t ctx = {0};
    if (argc == 2) {
        return lox_run_file(&ctx, argv[1]);
    }
    lox_run_prompt(&ctx);
    return EX_OK;
}
