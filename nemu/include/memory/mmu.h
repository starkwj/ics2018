#ifndef __MMU_H__
#define __MMU_H__

#include <stdint.h>

/* 32bit x86 uses 4KB page size */
#define PAGE_SIZE					4096
#define NR_PDE						1024
#define NR_PTE						1024
#define PAGE_MASK					(4096 - 1)
#define PT_SIZE						((NR_PTE) * (PAGE_SIZE))

// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/\------ OFF(va) ------/
#define PTE_P     0x001     // Present
#define PTE_A     0x020     // Accessed
#define PTE_D     0x040     // Dirty
#define PGSHFT    12      // log2(PGSIZE)
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xfff)

/* the Control Register 0 */
typedef union CR0 {
  struct {
    uint32_t protect_enable      : 1;
    uint32_t dont_care           : 30;
    uint32_t paging              : 1;
  };
  uint32_t val;
} CR0;

/* the Control Register 3 (physical address of page directory) */
typedef union CR3 {
  struct {
    uint32_t pad0                : 3;
    uint32_t page_write_through  : 1;
    uint32_t page_cache_disable  : 1;
    uint32_t pad1                : 7;
    uint32_t page_directory_base : 20;
  };
  uint32_t val;
} CR3;


/* the 32bit Page Directory(first level page table) data structure */
typedef union PageDirectoryEntry {
  struct {
    uint32_t present             : 1;
    uint32_t read_write          : 1; 
    uint32_t user_supervisor     : 1;
    uint32_t page_write_through  : 1;
    uint32_t page_cache_disable  : 1;
    uint32_t accessed            : 1;
    uint32_t pad0                : 6;
    uint32_t page_frame          : 20;
  };
  uint32_t val;
} PDE;

/* the 32bit Page Table Entry(second level page table) data structure */
typedef union PageTableEntry {
  struct {
    uint32_t present             : 1;
    uint32_t read_write          : 1;
    uint32_t user_supervisor     : 1;
    uint32_t page_write_through  : 1;
    uint32_t page_cache_disable  : 1;
    uint32_t accessed            : 1;
    uint32_t dirty               : 1;
    uint32_t pad0                : 1;
    uint32_t global              : 1;
    uint32_t pad1                : 3;
    uint32_t page_frame          : 20;
  };
  uint32_t val;
} PTE;

typedef PTE (*PT) [NR_PTE];

typedef union GateDescriptor {
  struct {
    uint32_t offset_15_0      : 16;
    uint32_t dont_care0       : 16;
    uint32_t dont_care1       : 15;
    uint32_t present          : 1;
    uint32_t offset_31_16     : 16;
  };
  uint32_t val;
} GateDesc;

#endif
