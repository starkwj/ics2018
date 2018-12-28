#include <am.h>
#include <amdev.h>
#include <stdio.h>
#include <ndl.h>

void timer_init();
void vga_init();

size_t input_read(uintptr_t reg, void *buf, size_t size);
size_t timer_read(uintptr_t reg, void *buf, size_t size);
size_t video_read(uintptr_t reg, void *buf, size_t size);

size_t video_write(uintptr_t reg, void *buf, size_t size);

static _Device devices[] = {
  {_DEV_INPUT,   "NDL Keyboard", input_read, NULL},
  {_DEV_TIMER,   "Native Timer", timer_read, NULL},
  {_DEV_VIDEO,   "NDL Graphics", video_read, video_write},
};

int _ioe_init() {
  NDL_OpenDisplay(400, 300);
  timer_init();
  vga_init();
  return 0;
}

_Device *_device(int n) {
  if (n >= 1 && n <= sizeof(devices) / sizeof(devices[0])) {
    return &devices[n - 1];
  } else {
    return NULL;
  }
}
