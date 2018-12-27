#include "common.h"

extern _Context* do_syscall(_Context *c);
extern _Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
  _Context *r = NULL;
  switch (e.event) {
    case _EVENT_SYSCALL: r = do_syscall(c); break;
    case _EVENT_YIELD: r = schedule(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return r;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
