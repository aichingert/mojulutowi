#include "lu_string.h"
#include "arena.h"

String str_copy(Arena *arena, String str) {
    char *value = (char*)lu_arena_alloc(arena, str.size);

    for (u64 i = 0; i < str.size; i++) {
        value[i] = str.value[i];
    }

    return (String) {
        .size = str.size,
        .value = value,
    };
}
