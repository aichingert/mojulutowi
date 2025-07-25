#ifndef LU_TTF_H
#define LU_TTF_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "lu.h"
#include "string.h"
#include "array.h"
#include "window.h"

typedef struct OffsetSubtable {
    u32 scaler_type;
    u16 num_tables;
    u16 search_range;
    u16 entry_selector;
    u16 range_shift;
} OffsetSubtable;

typedef struct TableDirectory {
    u32 tag;
    u32 checksum;
    u32 offset;
    u32 length;
} TableDirectory;

ArrayVec2 lu_extract_glyph_from_font(Arena *, String, u16);
ArrayVec2 read_glyph_table(Arena *, u8 *, u64 *, TableDirectory *, u16);

#endif /* LU_TTF_H */
