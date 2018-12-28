#include <am.h>
#include <amdev.h>
#include <ndl.h>

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      NDL_Event e;
      do {
        NDL_WaitEvent(&e);
      } while (e.type != NDL_EVENT_KEYDOWN && e.type != NDL_EVENT_KEYUP);
      kbd->keydown = e.type == NDL_EVENT_KEYDOWN;
      kbd->keycode = e.data;
      return sizeof(_KbdReg);
    }
  }

  return 0;
}
