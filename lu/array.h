#ifndef LU_ARRAY_H
#define LU_ARRAY_H

#include <assert.h>

#include "arena.h"

// TODO: think about api design and implementation

#define lu_array_push(arena, array, obj) \
    do { \
        if ((array).used + 1 >= (array).size) { \
            void *base = lu_arena_alloc(arena, sizeof(obj) * (array).size * 2); \
            for (u64 i = 0; i < (arena).size; i++) { \
                base[i] = (arena).base[i]; \
            } \
            (arena).base = base; \
            (arena).size *= 2; \
        } \

        (array).base[(array).used] = obj; \
        (array).used += 1; \
    } while(0) \

#endif /* LU_ARRAY_H */
