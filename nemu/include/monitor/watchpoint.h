#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char e[128];
  uint32_t value;

} WP;

WP* new_wp();
void free_wp(WP *wp);
void free_wp_no(int no);
bool check_wp();
void show_wp();

#endif
