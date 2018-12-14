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
  TK_NEQ, TK_AND,
  TK_DEC, TK_HEX, TK_NEG, TK_DEREF
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0[Xx][0-9]+", TK_HEX},  // hexadecimal number
  {"[0-9]+", TK_DEC},   // decimal number
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiple
  {"\\/", '/'},         // divide
  {"\\(", '('},         // left-bracket
  {"\\)", ')'},         // right-bracket
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
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
  int i;
  for (i = 0; i < nr_token; i++) {
    if (tokens[i].type == '-' && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type != TK_HEX)))
    {
      tokens[i].type = TK_NEG;
    }
    else if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type != TK_HEX)))
    {
      tokens[i].type = TK_DEREF;
    }
  }

  errexp = false;

  /* TODO: Insert codes to evaluate the expression. */
  int ret = eval(0, nr_token - 1);
  *success = !errexp;
  return ret;
}

bool check_parentheses(int p, int q) {
  bool ret = true;
  if (tokens[p].type != '(' || tokens[q].type != ')')
    ret = false;

  int cnt = 0;  // un paired left-bracket
  int i;
  for (i = p; i <= q; i++) {
    if (tokens[i].type == '(')
      cnt++;
    else if (tokens[i].type == ')') {
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
    return 0;
  }
  else if (p == q) {
    return strtol(tokens[p].str, NULL, 0);
  }
  else if (check_parentheses(p, q) == true && !errexp) {
    return eval(p + 1, q - 1);
  }
  else if (!errexp) {
    int op = -1;
    int bracket_cnt = 0;
    int optype = TK_NOTYPE;
    int i;
    for (i = q; i > p; i--) {
      switch (tokens[i].type) {
        case ')':
          bracket_cnt++;
          break;
        case '(':
          bracket_cnt--;
          break;
        
        case TK_AND:
          if (bracket_cnt == 0 && optype != TK_AND) {
            optype = tokens[i].type;
            op = i;
          }
          break;
        
        case TK_EQ:
        case TK_NEQ:
          if (bracket_cnt == 0) {
            if (optype != TK_AND && optype != TK_EQ && optype != TK_NEQ) {
              optype = tokens[i].type;
              op = i;
            }
          }
          break;
        
        case '+':
        case '-':
          if (bracket_cnt == 0) {
            if (optype != TK_AND && optype != TK_EQ && optype != TK_NEQ && optype != '+' && optype != '-') {
              optype = tokens[i].type;
              op = i;
            }
          }
          break;
        
        case '*':
        case '/':
          if (bracket_cnt == 0) {
            if (optype != TK_AND && optype != TK_EQ && optype != TK_NEQ && optype != '+' && optype != '-' && optype != '*' && optype != '/') {
              optype = tokens[i].type;
              op = i;
            }
          }
          break;
        
        case TK_NEG:
        case TK_DEREF:
          if (bracket_cnt == 0) {
            if (optype != TK_AND && optype != TK_EQ && optype != TK_NEQ && optype != '+' && optype != '-' && optype != '*' && optype != '/') {
              optype = tokens[i].type;
              op = i;
            }
          }
          break;
        default: break;
      }
    }
    if (op == -1) {
      errexp = true;
      return 0;
    }
    int val1 = 0;
    printf("op: %d type: %d\n", op, optype);
    if (op != p) {
      val1 = eval(p, op - 1);
    }
    int val2 = eval(op + 1, q);

    if (errexp) {
      return 0;
    }
    switch (optype) {
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_NEG: return -val2;
      case TK_DEREF: return vaddr_read(val2, 4);
      default: assert(0);
    }
  }
  return 0;
}
