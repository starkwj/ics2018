#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    cpu_exec(1);
  }
  else {
    int n = atoi(arg);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("info: missing operand\n");
  }
  else {
    if (!strcmp(arg, "r")) {
      int i;
      for (i = R_EAX; i <= R_EDI; i ++) {
        printf("%-12s0x%08x\n", regsl[i], cpu.gpr[i]._32);
      }
      printf("%-12s0x%08x\n", "eip", cpu.eip);
    }
    else if (!strcmp(arg, "w")) {
      show_wp();
    }
    else {
      printf("Unknown arg '%s'\n", arg);
    }
  }
  return 0;
}

static int cmd_p(char *args) {
  bool suc = true;
  uint32_t ret = expr(args, &suc);
  if (suc) {
    printf("value = %u  (0x%x)\n", ret, ret);
  }
  else {
    printf("Illegal expression: '%s'\n", args);
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("x: missing operand\n");
    return 0;
  }
  int n = atoi(arg);
  arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("x: missing operand\n");
    return 0;
  }
  int addr = strtol(arg, NULL, 0);
  int i;
  for (i = 0; i < n; i++) {
    if (i % 4 == 0)
      printf("0x%08x: ", addr);
    printf("%08x  ", vaddr_read(addr, 4));
    addr += 4;
    if (i % 4 == 3 || i == n - 1)
      printf("\n");
  }
  return 0;
}

static int cmd_w(char *args) {
  bool suc = true;
  uint32_t ret = expr(args, &suc);
  if (suc) {
    WP *wp = new_wp();
    strcpy(wp->e, args);
    wp->value = ret;
    printf("Watchpoint %d is set: %s\n", wp->NO, wp->e);
  }
  else {
    printf("Illegal expression: '%s'\n", args);
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (!arg) {
    printf("d: missing operand\n");
    return 0;
  }
  free_wp_no(atoi(arg));
  return 0;
}

// for DiffTest
extern bool detached;
extern void difftest_sync();

static int cmd_detach(char *args) {
  detached = true;
  printf("DiffTest is closed.\n");
  return 0;
}

static int cmd_attach(char *args) {
  detached = false;
  difftest_sync();
  printf("DiffTest is opened.\n");
  return 0;
}

static int cmd_save(char *args) {
  FILE *f = fopen(args, "wb");
  if (f == NULL) {
    printf("Can't open file '%s'.\n", args);
    return -1;
  }
  if (fwrite(&cpu, sizeof(cpu), 1, f) != 1)
    printf("save regs failed.\n");
  int res = 0;
  if ((res = fwrite(guest_to_host(0), 1, PMEM_SIZE + ENTRY_START, f)) != PMEM_SIZE + ENTRY_START)
    printf("save mem failed. %d bytes\n", res);
  fclose(f);
  return 0;
}

static int cmd_load(char *args) {
  FILE *f = fopen(args, "rb");
  if (f == NULL) {
    printf("Can't open file '%s'.\n", args);
    return -1;
  }
  if (fread(&cpu, sizeof(cpu), 1, f) != 1)
    printf("load regs failed.\n");
  if (fwrite(guest_to_host(0), 1, PMEM_SIZE + ENTRY_START, f) != PMEM_SIZE + ENTRY_START)
    printf("load mem failed.\n");
  fclose(f);
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  // wj 12.13
  { "si", "Execute one or N machine instruction(s)", cmd_si },
  { "info", "List program status", cmd_info },
  { "p", "Print contents of a variable", cmd_p },
  { "x", "Examining memory", cmd_x },
  { "w", "Set watchpoint to EXPR", cmd_w },
  { "d", "Delete watchpoint", cmd_d },
  { "detach", "Quit DiffTest mode", cmd_detach },
  { "attach", "Enter DiffTest mode", cmd_attach },
  { "save", "save NEMU snapshot to path", cmd_save },
  { "load", "load NEMU snapshot from path", cmd_load },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
