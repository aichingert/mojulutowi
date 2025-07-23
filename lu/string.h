#ifndef LU_STRING_H
#define LU_STRING_H

#include "lu.h"
#include "arena.h"

#define S(str) (String){ \
    .len = sizeof(str) - 1, \
    .value = str } \

typedef struct String {
    u64  len;
    char *value;
} String;

s8      lu_char_cmp(const char *, const char *);
s8      lu_str_cmp (String, String);
String  lu_str_copy(Arena *, String);

#endif /* LU_STRING_H */
