#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_no = is_mmio(addr);
  if (map_no == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr, len, map_no);
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int map_no = is_mmio(addr);
  if (map_no == -1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr, len, data, map_no);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (OFF(addr) + len > 0x1000) {
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (OFF(addr) + len > 0x1000) {
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, data, len);
  }
}

paddr_t page_translate(vaddr_t vaddr, bool write) {
  if (cpu.cr0.paging && cpu.cr0.protect_enable) {
    PDE *ppde = guest_to_host((cpu.cr3.val & ~0xfff) + PDX(vaddr) * sizeof(PDE));
    // PDE *ppde = (PDE *)(intptr_t)(cpu.cr3.val & ~0xfff);
    // PDE pde;
    // pde.val = paddr_read((intptr_t)&ppde[PDX(vaddr)], 4);
    assert(ppde->present);
    // pde.accessed = 1;
    ppde->accessed = 1;
    PTE *ppte = guest_to_host((ppde->val & ~0xfff) + PTX(vaddr) * sizeof(PTE));
    // PTE pte;
    // pte.val = paddr_read((intptr_t)&ppte[PTX(vaddr)], 4);
    assert(ppte->present);
    ppte->accessed = 1;
    if (write) {
      ppte->dirty = 1;
    }
    return PTE_ADDR(ppte->val) | OFF(vaddr);

  }
  else {
    return vaddr;
  }
}
