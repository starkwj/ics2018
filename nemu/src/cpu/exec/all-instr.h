#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(add);
make_EHelper(or);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(cmp);
make_EHelper(ret);

make_EHelper(lea);
make_EHelper(nop);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(jcc);
make_EHelper(test);
make_EHelper(leave);

make_EHelper(rol);
// make_EHelper(ror);
// make_EHelper(rcl);
// make_EHelper(rcr);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(sar);

make_EHelper(inc);
make_EHelper(dec);

make_EHelper(not);
make_EHelper(neg);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(cwtl);
make_EHelper(cltd);
make_EHelper(jmp);
make_EHelper(movsx);
make_EHelper(jmp_rm);
make_EHelper(call_rm);

make_EHelper(in);
make_EHelper(out);

make_EHelper(lidt);
make_EHelper(int);
make_EHelper(iret);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(mov_r2cr);
make_EHelper(mov_cr2r);

make_EHelper(stos);
