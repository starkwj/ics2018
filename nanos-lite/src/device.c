#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  // return 0;
  if (buf == NULL) {
    return -1;
  }
  int i = 0;
  for (; i < len; i++) {
    _putc(*((const char *)buf + i));
  }
  return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  if (key == _KEY_NONE) {
    sprintf(buf, "t %d\n", uptime());
  }
  else {
    sprintf(buf, "k%c %s\n", (key & KEYDOWN_MASK) == 0 ? 'u' : 'd', keyname[key & (~KEYDOWN_MASK)]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if (offset + len > 128) {
    len = 128 - offset;
  }
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset >>= 2;
  len >>= 2;
  int rsz = 0;
  int sw = screen_width();
  int sh = screen_height();
  int x = offset % sw;
  int y = offset / sw;
  int draw = len;
  
  if (x + len > sw) {
    draw = sw - x;
  }
  draw_rect((uint32_t *)(buf + rsz), x, y, draw, 1);
  rsz += draw << 2;
  len -= draw;
  y++;
  if (len > sw) {
    int h = len / sw;
    if (y + h > sh) {
      h = sh - y;
      len = sw * h;
    }
    draw_rect((uint32_t *)(buf + rsz), 0, y, sw, h);
    rsz += (sw * h) << 2;
    len -= sw * h;
    y += h;
  }
  if (len > 0) {
    draw_rect((uint32_t *)(buf + rsz), 0, y, len, 1);
    rsz += len << 2;
  }
  return rsz;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
