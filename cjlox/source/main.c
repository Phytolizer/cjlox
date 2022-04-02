#include <stdio.h>
#include <string/string.h>

int main(void) {
    string_t hello = STRING_C("hello world");
    string_print(hello, stdout);
}
