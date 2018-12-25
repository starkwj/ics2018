#ifndef __MONITOR_H__
#define __MONITOR_H__

enum { NEMU_STOP, NEMU_RUNNING, NEMU_END, NEMU_ABORT };
extern int nemu_state;

#define ENTRY_START 0x100000
#define PMEM_SIZE (128 * 1024 * 1024)

#endif
