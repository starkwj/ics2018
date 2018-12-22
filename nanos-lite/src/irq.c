#include "common.h"

// enum {
//   _EVENT_NULL = 0,
//   _EVENT_ERROR,
//   _EVENT_IRQ_TIMER,
//   _EVENT_IRQ_IODEV,
//   _EVENT_PAGEFAULT,
//   _EVENT_YIELD,
//   _EVENT_SYSCALL,
// };

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_YIELD: printf("Event Yield ID = %d\n", e.event); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
