#include <am.h>
#include <klib.h>
void print(const char *s) {
  for (; *s; s ++) {
    _putc(*s);
  }
}
int main() {
  for (int i = 0; i < 10; i ++) {
    // print("Hello World!\n");
    // printf("Hello World!\n");
    printf("%s %d\n", "Hello World!", 1234);
  }
  return 0;
}
