#include "arena.h"
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <sys/mman.h>

Arena* lu_arena_init(u64 size) {
    void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == MAP_FAILED) {
        assert(false && "arena allocation failed!");
    }

    Arena *arena = (Arena*)base;
    arena->size = size;
    arena->base = base;
    arena->used = sizeof(Arena);
    return arena;
}

void* lu_arena_alloc(Arena *arena, u64 size) {
    if (arena->used + size > arena->size) {
        printf("%lu + %lu = %lu | %lu\n", arena->used, size, arena->used + size, arena->size);
        assert(false && "TODO: handle reallocation of the arena");
    }

    void *mem = (u8*)arena->base + arena->used;
    arena->used += size;
    return mem;
}

void lu_arena_free(Arena *arena) {
    munmap(arena, arena->size);
}
