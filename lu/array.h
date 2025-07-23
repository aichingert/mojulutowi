#ifndef LU_ARRAY_H
#define LU_ARRAY_H

#include <assert.h>

#include "arena.h"
#include "math.h"

// TODO: think about api design and implementation

#define _ArrayHeader_ struct { u64 len; u64 cap; }

#define ArrayHeaderCast(a)  ((ArrayHeader *)&a)
#define ArrayItemSize(a)    (sizeof(*(a).v))

#define lu_array_push(arena, a, value)                                                              \
    (*((void **)&(a).v) = lu_array_grow((arena), ArrayHeaderCast(a), (a).v, ArrayItemSize(a), 1),   \
    (a).v[(a).len++] = (value))                                                                     \

#define lu_array_pop(a, value)                                                                      \
    do {                                                                                            \
        assert((a).len != 0 && "Error: tried to pop from empty array!");                            \
        (a).len -= 1;                                                                               \
    } while (0)                                                                                     \

typedef struct ArrayHeader {
    u64 len;
    u64 cap;
} ArrayHeader;

typedef struct ArrayVec2 {
    _ArrayHeader_;
    Vec2 *v;
} ArrayVec2;

void* lu_array_grow(Arena *arena, ArrayHeader *header, void *array, u64 item_size, u64 count);

#endif /* LU_ARRAY_H */
