#include "array.h"
#include "mem.h"

// TODO: add more checks
void* lu_array_grow(Arena *arena, ArrayHeader *header, void *array, u64 item_size, u64 count) {
    if (header->len + count < header->cap) {
        return array;
    }

    header->cap = (header->len + count) * 2;
    void *new = lu_arena_alloc(arena, header->cap * item_size);

    lu_memcpy(new, array, header->len * item_size);
    header->len += count;

    return new;
}
