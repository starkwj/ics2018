#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (free_ == NULL) {
    printf("No free watchpoint!\n");
    assert(0);
  }
  WP *p = free_;
  free_ = free_->next;
  if (head == NULL) {
    head = p;
    head->next = NULL;
  }
  else {
    p->next = head->next;
    head = p;
  }
  return head;
}

void free_wp(WP *wp) {
  if (wp == NULL) {
    printf("wp is NULL\n");
    assert(0);
  }
  wp->value = 0;
  if (wp == head) {
    head = head->next;
  }
  else {
    WP *p = head;
    while (p && p->next != wp) {
      p = p->next;
    }
    if (p == NULL) {
      printf("Cannot find wp!\n");
      assert(0);
    }
    p->next = wp->next;
  }
  wp->next = free_;
  free_ = wp;
}

void free_wp_no(int no) {
  WP *p = head;
  while (p != NULL && p->NO != no) {
    p = p->next;
  }
  if (!p) {
    printf("Watchpoint %d is not on!\n", no);
  }
  else {
    free_wp(p);
  }
}

bool check_wp() {
  WP *p = head;
  bool ret = false;
  while (p != NULL) {
    bool suc;
    uint32_t newvalue = expr(p->e, &suc);
    if (newvalue != p->value) {
      ret = true;
      printf("Value of watchpoint %d (%s) changed, from %u to %u\n", p->NO, p->e, p->value, newvalue);
      p->value = newvalue;
    }
    p = p->next;
  }
  return ret;
}
