#include <am.h>
#include <stdio.h>

extern int main();

void _trm_init() {
    printf("t\n");
    main();
    printf("test\n");
}

void _putc(char ch) {
    putchar(ch);
}

void _halt(int code) {
    while (1);
}

_Area _heap;
