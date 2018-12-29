#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
uint32_t fg_pcb;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  // naive_uload(NULL, "/bin/init");
  // context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  // context_uload(&pcb[2], "/bin/slider-am");
  // context_uload(&pcb[3], "/bin/pal");
  // context_uload(&pcb[0], "/bin/dummy");
  fg_pcb = 1;
  switch_boot_pcb();
}

static uint8_t count = 0;
_Context* schedule(_Context *prev) {
  current->cp = prev;
  // current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[fg_pcb] : &pcb[0]);
  current = count == 0 ? &pcb[0] : &pcb[fg_pcb];
  count++;
  return current->cp;
}
