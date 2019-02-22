#include "proc.h"
#include "fs.h"

// #define DEFAULT_ENTRY 0x4000000
#define DEFAULT_ENTRY 0x8048000

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  // ramdisk_read((void *)DEFAULT_ENTRY, 0, get_ramdisk_size());
  // int fd = fs_open(filename, 0, 0);
  // if (fd >= 0) {
  //   fs_read(fd, (void *)DEFAULT_ENTRY, fs_filesz(fd));
  //   fs_close(fd);
  // }
  // return DEFAULT_ENTRY;

  int fd = fs_open(filename, 0, 0);
  if (fd >= 0) {
    uint32_t sz = fs_filesz(fd);
    if (sz > 0) {
      int pg_num = (sz - 1) / PGSIZE + 1;
      void *v = (void *)DEFAULT_ENTRY;
      void *p;
      int i;
      for (i = 0; i < pg_num - 1; i++, v += PGSIZE) {
        p = new_page(1);
        // if (i == 0) {
        //   printf("load %s: 1st page = %x\n", filename, p);
        // }
        _map(&pcb->as, v, p, 0);
        fs_read(fd, p, PGSIZE);
      }
      p = new_page(1);
      _map(&pcb->as, v, p, 0);
      fs_read(fd, p, sz & PGMASK);
    }
    pcb->cur_brk = pcb->max_brk = DEFAULT_ENTRY + sz;
    fs_close(fd);
  }
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  pcb->as.ptr = NULL;
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
