#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  printf("new_page : p=%x  pf=%x\n", p, pf);
  return p;
}

void free_page(void *p) {
  panic("not implement yet or no need to implement");
  pf -= PGSIZE;
}

void reset_page() {
  size_t sz = pf - (void *)PGROUNDUP((uintptr_t)_heap.start);
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  memset(pf, 0, sz);
  printf("reset_page : sz = %x, pf = %x\n", sz, pf);
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
  if (new_brk > current->max_brk) {
    uintptr_t va = (current->max_brk & ~0xfff) + PGSIZE;
    uintptr_t newpf = new_brk & ~0xfff;
    // printf("mm_brk: va=%x new_brk=%x\n", va, newpf);
    while (va <= newpf) {
      void *pa = new_page(1);
      // printf("mm_brk va -> pa : %x -> %x\n", va, pa);
      _map(&current->as, (void *)va, pa, 0);
      va += PGSIZE;
    }
    current->max_brk = new_brk;
  }
  current->cur_brk = new_brk;
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
