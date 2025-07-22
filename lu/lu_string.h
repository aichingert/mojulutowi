#ifndef LU_STRING_H
#define LU_STRING_H

#include "lu.h"
#include "arena.h"

#define S(str) (String){ \
    .size = sizeof(str) - 1, \
    .value = str } \

typedef struct String {
    u64  size;
    char *value;
} String;

String str_copy(Arena *, String);

int strcmp(const char *, const char *);
//void* memcpy(void *, void *, size_t);

#endif /* LU_STRING_H */
