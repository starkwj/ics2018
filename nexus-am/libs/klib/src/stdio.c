#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define LOWER 0x1

static inline int isdigit(int ch) {
  return (ch >= '0') && (ch <= '9');
}

char * num2a(char *str, int num, int base, int width, int precision, int flag) {
  static const char digits[2][16] = {"0123456789ABCDEF", "0123456789abcdef"};
  char tmp[100];
  int lower = flag & LOWER;

  int i = 0;
  if (num == 0) {
    tmp[i++] = 0;
  }
  while (num) {
    tmp[i++] = digits[lower][num % base];
    num /= base;
  }

  while (i--) {
    *str++ = tmp[i];
  }

  return str;
}


int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str = out;
  int flags;
  int field_width;
  int precision;
  // int length_modifier;
  // int conversion_specifiers;


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
        goto repeat;

      default:
        break;
    }

    field_width = -1;
    // width

    precision = -1;
    // precision

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

      case 'o':
        break;

      case 'u':
        break;

      case 'x':
        flags |= LOWER;
      case 'X':
        base = 16;
        break;
      
      case 'e':
      case 'E':
        break;
      
      case 'g':
      case 'G':
        break;
      
      case 'c':
        *str++ = (unsigned char)va_arg(ap, int);
        break;
      
      case 's': {
        char *s = va_arg(ap, char *);
        int len = strlen(s);
        if (precision > 0) {
          len = precision < len ? precision : len;
        }
        // left
        while (len--) {
          *str++ = *s++;
        }
        break;
      }
      
      case 'p':
        break;

      case '%':
        *str++ = '%';
        break;

    } // end of switch

    if (isnum) {
      int num = va_arg(ap, int);
      str = num2a(str, num, base, field_width, precision, flags);
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
