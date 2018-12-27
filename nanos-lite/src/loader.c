#include "proc.h"
#include "fs.h"

// #define DEFAULT_ENTRY 0x4000000
#define DEFAULT_ENTRY 0x8048000

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  // ramdisk_read((void *)DEFAULT_ENTRY, 0, get_ramdisk_size());
  int fd = fs_open(filename, 0, 0);
  void *base = NULL;
  if (fd >= 0) {
    uint32_t sz = fs_filesz(fd);
    if (sz > 0) {
      uint32_t pg_num = (sz - 1) / PGSIZE + 1;
      base = new_page(pg_num);
      void *p = base;
      // printf("loader():  base = %x, pg_num = %x, end = %x\n", base, pg_num, base + pg_num * PGSIZE);
      void *v = (void *)DEFAULT_ENTRY;
      int i;
      for (i = 0; i < pg_num - 1; i++, p += PGSIZE, v += PGSIZE) {
        _map(&pcb->as, v, p, 1);
        fs_read(fd, p, PGSIZE);
      }
      _map(&pcb->as, v, p, 1);
      fs_read(fd, p, sz & PGMASK);
    }
    fs_close(fd);
  }
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
