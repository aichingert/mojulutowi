#include "math.h"
#include "string.h"
#include "arena.h"

String lu_str_copy(Arena *arena, String str) {
    char *value = (char*)lu_arena_alloc(arena, str.len);

    for (u64 i = 0; i < str.len; i++) {
        value[i] = str.value[i];
    }

    return (String) {
        .len   = str.len,
        .value = value,
    };
}

s8 lu_str_cmp(String a, String b) {
    for (u64 i = 0; i < MIN(a.len, b.len); i++) {
        if (a.value[i] > b.value[i]) {
            return 1;
        } else if (a.value[i] < b.value[i]) {
            return -1;
        }
    }

    if (a.len == b.len) {
        return 0;
    }

    return a.len < b.len ? -1 : 1;
}

s8 lu_char_cmp(const char *a, const char *b) {
    for (; *a != '\0' && *b != '\0'; a += 1, b += 1) {
        if (*a > *b) {
            return 1;
        } else if (*b < *a) {
            return -1;
        }
    }

    if (*a == '\0' && *b == '\0') {
        return 0;
    }

    return *a == '\0' ? -1 : 1;
}


