#ifndef LU_ARENA_H
#define LU_ARENA_H

#include "lu.h"

// TODO: in the future this could be a bigger virtual allocation
// using PROT_NONE and mprotect to actually bind this allocation
typedef struct Arena {
    u64 size;
    u64 used;
    void *base;
} Arena;

Arena*  lu_arena_init(u64);
void*   lu_arena_alloc(Arena *, u64);
void    lu_arena_free(Arena *);

#endif /* LU_ARENA_H */
