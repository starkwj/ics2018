#include "cpu/exec.h"
#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
  // TODO();
  if (decoding.is_operand_size_16) {
    cpu.IDTR.LIMIT = vaddr_read(id_dest->addr, 2);
    cpu.IDTR.BASE = vaddr_read(id_dest->addr + 2, 3);
  }
  else {
    cpu.IDTR.LIMIT = vaddr_read(id_dest->addr, 2);
    cpu.IDTR.BASE = vaddr_read(id_dest->addr + 2, 4);
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  // TODO();
  switch (id_dest->reg) {
    case 0: cpu.cr0.val = id_src->val; break;
    case 3: cpu.cr3.val = id_src->val; break;
    default: Assert(0, "only cr0 or cr3"); break;
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  // TODO();
  switch (id_src->reg) {
    case 0: t0 = cpu.cr0.val; break;
    case 3: t0 = cpu.cr3.val; break;
    default: Assert(0, "only cr0 or cr3"); break;
  }
  operand_write(id_dest, &t0);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  // TODO();
  t0 = id_dest->val & 0xff;
  raise_intr(t0, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  // TODO();
  rtl_pop(&t0);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags._32);
  rtl_jr(&t0);

  print_asm("iret");
}

make_EHelper(in) {
  // TODO();
  switch (id_dest->width) {
    case 4:
      t0 = pio_read_l(id_src->val);
      break;
    case 1:
      t0 = pio_read_b(id_src->val);
      break;
    case 2:
      t0 = pio_read_w(id_src->val);
      break;
    default:
      assert(0);
  }
  operand_write(id_dest, &t0);

  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  // TODO();
  switch (id_dest->width) {
    case 4:
      pio_write_l(id_dest->val, id_src->val);
      break;
    case 1:
      pio_write_b(id_dest->val, id_src->val);
      break;
    case 2:
      pio_write_w(id_dest->val, id_src->val);
      break;
    default:
      assert(0);
  }

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
