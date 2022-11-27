#include <lox/lox.h>
#include <stdio.h>
#include <sysexits/sysexits.h>

int main(int argc, char** argv) {
  Lox lox;
  lox_init(&lox);
  switch (argc) {
    case 1: {
      int code = lox_run_prompt(&lox);
      lox_free(&lox);
      return code;
    }
    case 2: {
      int code = lox_run_file(&lox, argv[1]);
      lox_free(&lox);
      return code;
    }
    default: {
      (void)fprintf(stderr, "Usage: lox [script]\n");
      return EX_USAGE;
    }
  }
}
