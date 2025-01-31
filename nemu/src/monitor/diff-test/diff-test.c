#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

bool detached;
static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() {
  if (detached)
    return;
  is_skip_ref = true;
}
void difftest_skip_dut() {
  if (detached)
    return;
  is_skip_dut = true;
}

void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
  detached = true;
}

void difftest_step(uint32_t eip) {
  if (detached)
    return;

  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);

  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
  // TODO();
  int i;
  for (i = 0; i < 8; i++) {
    if (ref_r.gpr[i]._32 != cpu.gpr[i]._32) {
      printf("Difference at $%s: ref: %x dul: %x\n", reg_name(i, 4), ref_r.gpr[i]._32, cpu.gpr[i]._32);
      nemu_state = NEMU_ABORT;
    }
  }
  if (ref_r.eip != cpu.eip) {
    printf("Difference at $eip: ref: %x dul: %x\n", ref_r.eip, cpu.eip);
    nemu_state = NEMU_ABORT;
  }
}

void difftest_sync() {
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), PMEM_SIZE - ENTRY_START);
  CPU_state ref_r;
  ref_difftest_getregs(&ref_r);
  ref_r.eax = 0x7e00;
  ref_r.eip = 0x7e40;
  ref_difftest_exec(1);
  // printf("eax:%d edx:%d eip:%d esp:%d\n", cpu.eax, cpu.edx, cpu.eip, cpu.esp);
  ref_difftest_setregs(&cpu);
}
