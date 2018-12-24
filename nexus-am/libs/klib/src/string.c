#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s++ != '\0')
    len++;
  return len;
}

char *strcpy(char* dst,const char* src) {
  size_t i = 0;
  while (src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t dst_len = strlen(dst);
  size_t i = 0;
  while (src[i] != '\0') {
    dst[dst_len + i] = src[i];
    i++;
  }
  dst[dst_len + i] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1 != NULL && s2 != NULL);
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  int ret = *(unsigned char *)s1 - *(unsigned char *)s2;
  if (ret > 0)
    return 1;
  else if (ret < 0)
    return -1;
  else
    return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);
  if (n == 0)
    return 0;
  while (--n && *s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  int ret = *(unsigned char *)s1 - *(unsigned char *)s2;
  if (ret > 0)
    return 1;
  else if (ret < 0)
    return -1;
  else
    return 0;
}

// void* memset(void* v,int c,size_t n) {
//   assert(v != NULL);
//   size_t i;
//   for (i = 0; i < n; i++) {
//     *((uint8_t *)(v + i)) = c;
//   }
//   return v;
// }

void* memcpy(void* out, const void* in, size_t n) {
  assert(out != NULL && in != NULL);
  uint32_t *pout = out;
  const uint32_t *pin = in;
  while (n >= 4) {
    *pout++ = *pin++;
    n -= 4;
  }
  uint8_t *pout2 = (uint8_t *)pout;
  const uint8_t *pin2 = (uint8_t *)pin;
  while (n--) {
    *pout2++ = *pin2++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
  if (n == 0)
    return 0;
  while (--n && *(unsigned char *)s1 == *(unsigned char *)s2) {
    s1++;
    s2++;
  }
  int ret = *(unsigned char *)s1 - *(unsigned char *)s2;
  if (ret > 0)
    return 1;
  else if (ret < 0)
    return -1;
  else
    return 0;
}

#endif
