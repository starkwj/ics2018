#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);
  
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(rol) {
  t0 = id_dest->val;
  t1 = id_src->val;
  if (t1 != 0) {
    if (decoding.is_operand_size_16) {
      t1 = t1 % 16;
      t0 = ((t0 << t1) | (t0 >> (16 - t1))) & 0xffff;
    }
    else {
      t1 = t1 % 32;
      t0 = ((t0 << t1) | (t0 >> (32 - t1)));
    }
    operand_write(id_dest, &t0);
    t0 &= 0x1;
    rtl_set_CF(&t0);
  }

  print_asm_template2(rol);
}

make_EHelper(shl) {
  // TODO();
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  rtl_not(&t0, &id_dest->val);
  operand_write(id_dest, &t0);

  print_asm_template1(not);
}
