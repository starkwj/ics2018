#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // TODO();
  rtl_push(&cpu.eflags._32);
  // cpu.eflags.IF = 0;
  // cpu.eflags.TF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  
  vaddr_t base = cpu.IDTR.BASE + sizeof(GateDesc) * NO;
  vaddr_t low = vaddr_read(base, 2);
  vaddr_t high = vaddr_read(base + 4, 4) & 0xffff0000;
  rtl_j(high | low);

}

void dev_raise_intr() {
}
