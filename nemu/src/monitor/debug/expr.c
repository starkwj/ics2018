#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUM
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"[0-9]+", TK_NUM},   // decimal number
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiple
  {"\\/", '/'},         // divide
  {"\\(", '('},         // left-bracket
  {"\\)", ')'},         // right-bracket
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

bool check_parentheses(int p, int q);
int eval(int p, int q);


/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

bool errexp;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          default:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            ++nr_token;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  errexp = false;

  /* TODO: Insert codes to evaluate the expression. */
  *success = !errexp;
  return eval(0, nr_token);
}

bool check_parentheses(int p, int q) {
  bool ret = true;
  if (tokens[p].type != '(' || tokens[q].type != ')')
    ret = false;

  int cnt = 0;  // un paired left-bracket
  int i;
  for (i = p; i <= q; i++) {
    if (tokens[p].type == '(')
      cnt++;
    else if (tokens[p].type == ')') {
      if (cnt == 1) { // example: (1 + 2) * (3 + 4)
        if (i != q)
          ret = false;
      }
      else if (cnt <= 0) {  // example: (1 + 2) ) - 3
        errexp = true;
        return false;
      }
      cnt--;
    }
  }
  if (cnt != 0) { // brackets not in pairs
    errexp = true;
    return false;
  }
  return ret;
}

int eval(int p, int q) {
  if (p > q || errexp) {
    errexp = true;
    printf("Bad Expression\n");
    return 0;
  }
  else if (p == q) {
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true && !errexp) {
    return eval(p + 1, q - 1);
  }
  else {
    int op = -1;
    int bracket_cnt = 0;
    int optype = TK_NOTYPE;
    int i;
    for (i = q; i > p; i--) {
      if (tokens[i].type == ')')
        bracket_cnt++;
      else if (tokens[i].type == '(')
        bracket_cnt--;
      else if (tokens[i].type == '+' || tokens[i].type == '-') {
        if (bracket_cnt == 0) {
          if (optype == TK_NOTYPE || optype == '*' || optype == '/') {
            optype = tokens[i].type;
            op = i;
            break;
          }
        }
      }
      else if (tokens[i].type == '*' || tokens[i].type == '/') {
        if (bracket_cnt == 0) {
          if (optype == TK_NOTYPE) {
            optype = tokens[i].type;
            op = i;
          }
        }
      }
    }
    if (op == -1) {
      errexp = true;
      return 0;
    }
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);
    printf("%d %d\n", val1, val2);
    if (errexp) {
      return 0;
    }

    switch (optype) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }

}
