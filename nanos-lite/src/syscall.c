#include "common.h"
#include "syscall.h"

long sys_write(int fd, const void *buf, size_t count) {
  if (buf == NULL) {
    return -1;
  }
  int i = 0;
  if (fd == 1 || fd == 2) {
    for (; i < count; i++) {
      _putc(*((const char *)buf + i));
    }
  }
  return i;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:
      _yield();
      c->GPRx = 0;
      break;
    case SYS_exit:
      _halt(a[1]);
      c->GPRx = 0;
      break;
    case SYS_write:
      c->GPRx = sys_write(a[1], (const void *)a[2], a[3]);
      break;
    case SYS_brk:

      c->GPRx = 0;
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
