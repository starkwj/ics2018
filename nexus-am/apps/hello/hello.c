#include <am.h>
#include <klib.h>
void print(const char *s) {
  for (; *s; s ++) {
    _putc(*s);
  }
}
int main() {
  for (int i = 0; i < 10; i ++) {
    _putc('t');
    printf("Hello World!\n");
  }
  return 0;
}
