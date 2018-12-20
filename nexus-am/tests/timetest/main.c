#include <am.h>
#include <amdev.h>
#include <klib.h>

int main(){
  _RTCReg rtc;
  _ioe_init();
  int sec = 1;
  printf("Test:\n%06dx\n%+6dx\n%6xx\n%6Xx\n", -123, 245, 5, 5);
  while (1) {
    while(uptime() < 1000 * sec) ;
    get_timeofday(&rtc);
    printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    if (sec == 1) {
      printf("%d second).\n", sec);
    } else {
      printf("%d seconds).\n", sec);
    }
    sec ++;
  }
  return 0;
}
