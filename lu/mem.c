#include "mem.h"

// TODO: might want to do some checks here
void* lu_memcpy(void *dst, void *src, u64 size) {
    void *start = dst;

    for (u64 i = 0; i < size; i++) {
        ((u8*)dst)[i] = ((u8*)src)[i];
    }

    return start;
}
