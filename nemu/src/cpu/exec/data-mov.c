#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // only 4 bytes
  assert(!decoding.is_operand_size_16);
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  // only 4 bytes
  assert(!decoding.is_operand_size_16);
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  // TODO();
  t0 = cpu.esp;
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
  rtl_push(&t0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));

  print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();
  rtl_pop(&reg_l(R_EDI));
  rtl_pop(&reg_l(R_ESI));
  rtl_pop(&reg_l(R_EBP));
  rtl_pop(&t0);
  rtl_pop(&reg_l(R_EBX));
  rtl_pop(&reg_l(R_EDX));
  rtl_pop(&reg_l(R_ECX));
  rtl_pop(&reg_l(R_EAX));

  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  assert(!decoding.is_operand_size_16);
  cpu.esp = cpu.ebp;
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_msb(&t0, &reg_l(R_AX), 2);
    t0 = -t0 & 0xff;
    rtl_sr(R_DX, &t0, 2);
  }
  else {
    // TODO();
    rtl_msb(&t0, &reg_l(R_EAX), 4);
    t0 = - t0;
    rtl_sr(R_EDX, &t0, 4);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_sext(&t0, &reg_l(R_AL), 1);
    rtl_sr(R_AX, &t0, 2);
  }
  else {
    // TODO();
    rtl_sext(&t0, &reg_l(R_AX), 2);
    rtl_sr(R_EAX, &t0, 4);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(stos) {
  operand_write(id_dest, &id_src->val);
  t0 = reg_l(R_EDI);
  t0 += id_src->width;
  rtl_sr(R_EDI, &t0, id_src->width);
}
