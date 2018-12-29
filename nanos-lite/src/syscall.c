#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

extern int mm_brk(uintptr_t new_brk);

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
  else {
    i = fs_write(fd, buf, count);
  }
  return i;
}

int sys_execve(const char *filename, char *const argv[], char *const envp[]) {
  // naive_uload(NULL, filename);
  printf("sys_execve:%s  %x\n", filename, filename);
  reset_page();
  context_uload(current, filename);
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit:
      _halt(a[1]);
      // sys_execve("/bin/init", NULL, NULL);
      c->GPRx = 0;
      break;
    case SYS_yield:
      _yield();
      c->GPRx = 0;
      break;
    case SYS_open:
      c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
      break;
    case SYS_write:
      c->GPRx = fs_write(a[1], (const void *)a[2], a[3]);
      break;
    case SYS_close:
      c->GPRx = fs_close(a[1]);
    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk:
      c->GPRx = mm_brk(a[1]);
      break;
    case SYS_execve:
      sys_execve((const char *)a[1], (char *const *)a[2], (char *const *)a[3]);
      // _yield();
      printf("test\n");
      c->GPRx = 0;
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
