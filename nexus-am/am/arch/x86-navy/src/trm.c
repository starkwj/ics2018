#include <am.h>
#include <stdio.h>

extern int main();

void _trm_init() {
    int ret = main();
    printf("test\n");
    _halt(ret);
}

void _putc(char ch) {
    putchar(ch);
    putchar('x');
}

void _halt(int code) {
    while (1);
}

_Area _heap;
