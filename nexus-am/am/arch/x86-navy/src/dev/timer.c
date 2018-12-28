#include <am.h>
#include <amdev.h>
#include <ndl.h>
#include <stdio.h>

static _UptimeReg boot_time;

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      NDL_Event e;
      do {
        NDL_WaitEvent(&e);
       } while (e.type != NDL_EVENT_TIMER);
      _UptimeReg *uptime = (_UptimeReg *)buf;
      uptime->hi = 0;
      uptime->lo = e.data - boot_time.lo;
      printf("time :%d\n", uptime->lo);
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
  NDL_Event e;
  printf("timer_init begin\n");
  do {
    NDL_WaitEvent(&e);
  } while (e.type != NDL_EVENT_TIMER);
  boot_time.hi = 0;
  boot_time.lo = e.data;
  printf("timer_init over\n");
}

