#ifndef LU_ARRAY_H
#define LU_ARRAY_H

#include <assert.h>

#include "arena.h"

// TODO: think about api design and implementation

#define _ArrayHeader_ struct { u64 len; u64 cap; }

#define ArrayHeaderCast(a)  ((ArrayHeader *)&a)
#define ArrayItemSize(a)    (sizeof(*(a).v))

#define lu_array_push(arena, a, value)                                                            \
    (*((void **)&(a).v) = lu_array_grow((arena), ArrayHeaderCast(a), (a).v, ArrayItemSize(a), 1), \
    (a).v[(a).count++] = (value))                                                                 \

void* lu_array_grow(Arena *arena, ArrayHeader *header, void *array, u64 item_size, u64 count);

#endif /* LU_ARRAY_H */
