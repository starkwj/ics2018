#include <am.h>
#include <amdev.h>
#include <ndl.h>
#include <klib.h>

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  printf("input_read:\n");
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      NDL_Event e;
      NDL_WaitEvent(&e);
      if (e.type != NDL_EVENT_KEYUP && e.type != NDL_EVENT_KEYDOWN) {
        kbd->keycode = _KEY_NONE;
        kbd->keydown = false;
      } else {
        kbd->keydown = e.type == NDL_EVENT_KEYDOWN;
        kbd->keycode = e.data;
      }
      return sizeof(_KbdReg);
    }
  }

  return 0;
}
