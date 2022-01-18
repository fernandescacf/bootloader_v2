#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void *memset(void *s, int c, uint32_t n);

void *memcpy(void *dst, const void *src, uint32_t len);

int32_t memcmp(const void *s1, const void *s2, uint32_t n);

uint32_t strlen(const void *str);

int32_t strcmp(const void *s1, const void *s2);

char *strcpy(char *dst, const char *src);

#endif