#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define LOWER     0x1
#define LEFT      0x2
#define ZEROPAD   0x4
#define SPACE     0x8
#define SIGN      0x10


static inline int isdigit(int ch) {
  return (ch >= '0') && (ch <= '9');
}

int getNum(const char **fmt) {
  int n = 0;
  while (isdigit(**fmt)) {
    n = n * 10 + **fmt - '0';
    (*fmt)++;
  }
  return n;
}

char * num2a(char *str, int64_t num, int base, int width, int flags) {
  static const char digits[2][16] = {"0123456789ABCDEF", "0123456789abcdef"};
  char tmp[60];
  int lower = flags & LOWER;

  char sign = 0;
  if (base == 10) {
    if (num < 0) {
      sign = '-';
      width--;
    } else if (flags & SIGN) {
      sign = '+';
      width--;
    } else if (flags & SPACE) {
      sign = ' ';
      width--;
    }
  } else if (base == 16) {
    width -= 2;
  }

  if (flags & LEFT) {
    flags = flags & (~ZEROPAD);
  }

  int i = 0;
  if (num == 0) {
    tmp[i++] = '0';
  }

  // convert num from int to string
  if (num < 0)
    num = -num;
  while (num) {
    tmp[i++] = digits[lower][num % base];
    num /= base;
  }
  width -= i;

  // if not left and not zero-padding, spaces are padded
  if (!(flags & (LEFT | ZEROPAD))) {
    while (width-- > 0) {
      *str++ = ' ';
    }
  }
  // sign if
  if (sign != 0) {
    *str++ = sign;
  }
  // if base == 16, 0[xX] is printed
  if (base == 16) {
    *str++ = '0';
    *str++ = lower ? 'x' : 'X'; 
  }
  // if zeropad, '0' is padded
  if (flags & ZEROPAD) {
    while (width-- > 0) {
      *str++ = '0';
    }
  }
  
  while (i--) {
    *str++ = tmp[i];
  }

  // right padding
  while (width-- > 0) {
    *str++ = ' ';
  }

  return str;
}


int printf(const char *fmt, ...) {
  char tmp[256];
  va_list ap;
  int i;
  va_start(ap, fmt);
  i = vsprintf(tmp, fmt, ap);
  va_end(ap);
  int j;
  for (j = 0; j < i; j++) {
    _putc(tmp[j]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str = out;
  int flags;
  int field_width;
  // int length_modifier;
  // int conversion_specifiers;
  *str++ = '!';

  for (; *fmt; ++fmt) {
    if (*fmt != '%') {
      *str++ = *fmt;
      continue;
    }

    flags = 0;
repeat:
    ++fmt;
    // flags
    switch (*fmt) {
      case '0':
        flags |= ZEROPAD;
        goto repeat;
      case '-':
        flags |= LEFT;
        goto repeat;
      case ' ':
        flags |= SPACE;
        goto repeat;
      case '+':
        flags |= SIGN;
        goto repeat;

      default:
        break;
    }

    // width
    field_width = -1;
    if (isdigit(*fmt)) {
      field_width = getNum(&fmt);
    }

    // length_modifier = -1;

    // conversion_specifiers = -1;

    int isnum = 0;
    int base = 10;
    switch (*fmt) {
      case 'd':
      case 'i':
        isnum = 1;
        base = 10;
        break;

      // case 'o':
      //   break;

      // case 'u':
      //   break;
      case 'p':
      case 'x':
        flags |= LOWER;
      case 'X':
        isnum = 1;
        base = 16;
        break;
      
      // case 'e':
      // case 'E':
      //   break;
      
      // case 'g':
      // case 'G':
      //   break;
      
      case 'c':
        *str++ = (unsigned char)va_arg(ap, int);
        break;
      
      case 's': {
        char *s = va_arg(ap, char *);
        int len = strlen(s);
        field_width -= len;
        if (!(flags & LEFT)) {
          while (field_width-- > 0) {
            *str++ = ' ';
          }
        }
        while (len--) {
          *str++ = *s++;
        }
        while (field_width-- > 0) {
          *str++ = ' ';
        }
        

        break;
      }

      case '%':
        *str++ = '%';
        break;

      default:
        *str++ = '%';
        *str++ = *fmt;
        break;

    } // end of switch

    if (isnum) {
      int num = va_arg(ap, int);
      str = num2a(str, num, base, field_width, flags);
    }
  }

  *str = '\0';
  return str - out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  int i;
  va_start(ap, fmt);
  i = vsprintf(out, fmt, ap);
  va_end(ap);
  return i;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
