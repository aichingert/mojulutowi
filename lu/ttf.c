#include "ttf.h"
#include "arena.h"
#include "mem.h"
#include "array.h"

// References: 
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
// https://learn.microsoft.com/en-us/typography/opentype/spec/cmap
// https://stackoverflow.com/questions/20733790/truetype-fonts-glyph-are-made-of-quadratic-bezier-why-do-more-than-one-consecu/20772557#20772557

/*
 * shortFrac 	16-bit signed fraction
 * Fixed 	    16.16-bit signed fixed-point number
 * FWord 	    16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
 * uFWord 	    16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
 * F2Dot14 	    16-bit signed fixed number with the low 14 bits representing fraction.
 * longDateTime The long internal format of a date in seconds since 12:00 midnight, January 1, 1904. It is represented as a signed 64-bit integer.
*/

#define ON_CURVE                                0b00000001
#define X_SHORT_VECTOR                          0b00000010
#define Y_SHORT_VECTOR                          0b00000100
#define REPEAT_FLAG                             0b00001000
#define X_IS_SAME                               0b00010000
#define Y_IS_SAME                               0b00100000

#define ru16(buf, off) ((u16)buf[off] << 8) | ((u16)buf[off + 1])
#define ru32(buf, off) (((u32)ru16(buf, off)) << (u32)16) | ((u32)ru16(buf, off + 2))
#define ru64(buf, off) (((u64)ru32(buf, off)) << (u64)32) | ((u64)ru32(buf, off + 4))

#define CONSUME_FUNC(type) \
    static inline type consume_ ## type(u8 *buf, u64 *read_offset, u64 skip) { \
        type val = r ## type(buf, *read_offset); \
        *read_offset += sizeof(type) + skip; \
        return val; \
    } \

CONSUME_FUNC(u16)
CONSUME_FUNC(u32)
CONSUME_FUNC(u64)

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

// TODO: might want to use a macro for this when I know how
#define TABLE_SIZE 4
typedef enum TableDefinitions {
    HEAD = 0,
    LOCA = 1,
    CMAP = 2,
    GLYF = 3,
} TableDefinitions;
static const u32 TAGS[TABLE_SIZE] = { ru32("head", 0),ru32("loca", 0), ru32("cmap", 0), ru32("glyf", 0) };

// TODO: should make custom file functions
u8* lu_read_font(Arena *arena, String font) {
    FILE *font_file = fopen(font.value, "r");
    if (!font_file) {
        assert(false && "ERROR: unable to open file!");
    }

    fseek(font_file, 0, SEEK_END);
    long long file_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    size_t b_size = file_size / sizeof(u8);
    u8 *buf = (u8*)lu_arena_alloc(arena, b_size);
    fread(buf, sizeof(u8), b_size, font_file);
    fclose(font_file);

    return buf;
}

void read_table_directory(
        u8 *buf, 
        u64 *read_offset, 
        u16 num_tables, 
        TableDirectory *tables) 
{
    for (u16 i = 0; i < num_tables; i++) {
        TableDirectory table = {
            .tag        = consume_u32(buf, read_offset, 0),
            .checksum   = consume_u32(buf, read_offset, 0),
            .offset     = consume_u32(buf, read_offset, 0),
            .length     = consume_u32(buf, read_offset, 0),
        };

        if (table.tag != TAGS[HEAD]) {
            u32 sum = 0;
            u32 n_longs = (table.length + 3) / 4;

            for (u32 j = 0; j < n_longs; j++) {
                sum += ru32(buf, table.offset + 4 * j);
            }

            if (sum != table.checksum) assert(false && "checksum missmatch");
        }

        if        (table.tag == TAGS[HEAD]) {
            tables[HEAD] = table;
        } else if (table.tag == TAGS[LOCA]) {
            tables[LOCA] = table;
        } else if (table.tag == TAGS[CMAP]) {
            tables[CMAP] = table;
        } else if (table.tag == TAGS[GLYF]) {
            tables[GLYF] = table;
        }
    }
}

// TODO: figure out what to do with the data
u16 read_head_table(u8 *buf, TableDirectory head) {
    u64 read_offset = head.offset;

    u16 maj_version         = consume_u16(buf, &read_offset, 0);
    u16 min_version         = consume_u16(buf, &read_offset, 0);
    u32 font_revision       = consume_u32(buf, &read_offset, 0);
    u32 checksum            = consume_u32(buf, &read_offset, 0);
    assert(0x5F0F3CF5 ==      consume_u32(buf, &read_offset, 0) && "Error: magic number in header is incorrect");
    u16 header_flags        = consume_u16(buf, &read_offset, 0);
    u16 uints_per_em        = consume_u16(buf, &read_offset, 0);
    u64 created             = consume_u64(buf, &read_offset, 0);
    u64 modified            = consume_u64(buf, &read_offset, 0);

    s16 global_x_min        = consume_u16(buf, &read_offset, 0);
    s16 global_y_min        = consume_u16(buf, &read_offset, 0);
    s16 global_x_max        = consume_u16(buf, &read_offset, 0);
    s16 global_y_max        = consume_u16(buf, &read_offset, 0);
    u16 mac_style           = consume_u16(buf, &read_offset, 0);
    u16 lowest_rec_ppem     = consume_u16(buf, &read_offset, 0);
    s16 font_direction_hint = consume_u16(buf, &read_offset, 0);
    s16 index_to_loc_format = consume_u16(buf, &read_offset, 0);
    s16 glyph_data_format   = consume_u16(buf, &read_offset, 0);

    printf("Font version: %hu.%hu\n", maj_version, min_version);
    printf("created: %lu - modified %lu\n", created, modified);
    printf("global bounds: \n");
    printf("    x: min(\t%hd)\tmax(\t%hd)\n", global_x_min, global_x_max);
    printf("    y: min(\t%hd)\tmax(\t%hd)\n", global_y_min, global_y_max);

    (void)font_revision;
    (void)checksum;
    (void)lowest_rec_ppem;
    (void)mac_style;
    (void)uints_per_em;
    (void)header_flags;
    (void)font_direction_hint;
    (void)glyph_data_format;

    return index_to_loc_format == 0 ? sizeof(u16) : sizeof(u32);
}

u16 read_glyph_id_from_cmap_table(u8 *buf, TableDirectory cmap, u16 code_point) {
    u64 read_offset = cmap.offset;
    u16 version     = consume_u16(buf, &read_offset, 0);
    u16 subtables   = consume_u16(buf, &read_offset, 0);

    printf("CMAP-Version: %hu - num_subtables: %hu\n", version, subtables);
    u64 format_4_offset = 0;

    for (u16 i = 0; i < subtables; i++) {
        u16 platform_id             = consume_u16(buf, &read_offset, 0);
        u16 platform_specific_id    = consume_u16(buf, &read_offset, 0);
        u32 subtable_offset         = consume_u32(buf, &read_offset, 8);
        (void)platform_id;
        (void)platform_specific_id;
        (void)subtable_offset;

        u64 format_offset = cmap.offset + subtable_offset;
        u16 format = consume_u16(buf, &format_offset, 0);

        if (format == 4) {
            format_4_offset = format_offset;
            break;
        }
    }

    assert(format_4_offset != 0 && "Error: font does not contain a format 4 encoding");

    u16 length      = consume_u16(buf, &format_4_offset, 0); (void)length;
    u16 language    = consume_u16(buf, &format_4_offset, 0);
    assert(language == 0 && "Error: mac language is not zero");

    u16 seg_count_x2 = consume_u16(buf, &format_4_offset, 0);
    u16 seg_count = seg_count_x2 / 2;

    u16 a = consume_u16(buf, &format_4_offset, 0); (void)a;
    u16 b = consume_u16(buf, &format_4_offset, 0); (void)b;
    u16 c = consume_u16(buf, &format_4_offset, 0); (void)c;

    u16 end_codes[seg_count]        = {};
    u16 start_codes[seg_count]      = {};
    u16 id_deltas[seg_count]        = {};
    u16 id_range_offsets[seg_count] = {};

    for (u16 i = 0; i < seg_count; i++) {
        end_codes[i] = consume_u16(buf, &format_4_offset, 0);
    }
    assert(end_codes[seg_count - 1] == 0xFFFF);

    u16 padding = consume_u16(buf, &format_4_offset, 0);
    assert(padding == 0 && "Error: padding is not zero in CMAP");

    for (u16 i = 0; i < seg_count; i++) {
        start_codes[i] = consume_u16(buf, &format_4_offset, 0);
    }
    for (u16 i = 0; i < seg_count; i++) {
        id_deltas[i] = consume_u16(buf, &format_4_offset, 0);
    }

    u16 id_range_offset_pos = format_4_offset;
    for (u16 i = 0; i < seg_count; i++) {
        id_range_offsets[i] = consume_u16(buf, &format_4_offset, 0);
    }

    for (u16 i = 0; i < seg_count; i++) {
        if (end_codes[i] >= code_point && start_codes[i] < code_point) {
            if        (id_range_offsets[i] == 0) {
                return code_point + id_deltas[i];
            } else if (id_range_offsets[i] == 0xFFFF) {
                assert(false && "ERROR: malformed font");
            }

            u16 delta = (code_point - start_codes[i]) * 2;

            id_range_offset_pos += i * 2;
            u16 pos = id_range_offset_pos + delta + id_range_offsets[i];

            if (buf[pos] == 0) {
                for (u8 j = 0; j < 200; j++) {
                    printf("MISSING GLYPH\n");
                }
                return 0;
            }

            return buf[pos] + id_deltas[i];
        }
    }

    assert(false && "Error: not even missing glyph id found");
    return 0;
}

ArrayVec2 read_simple_glyph(Arena *arena, u8 *buf, u64 *glyph_offset, s16 num_of_contours) {
    assert(num_of_contours > 0 && "Error: no contours for simple glyph");

    u16 contour_end_pts[num_of_contours] = {};
    for (u16 i = 0; i < num_of_contours; i++) {
        contour_end_pts[i] = consume_u16(buf, glyph_offset, 0);
        printf("CONTOUR ENDS WITH POINTS: %hu\n", contour_end_pts[i]);
    }

    // NOTE: ignoring instructions might never use them
    u16 instr_len = consume_u16(buf, glyph_offset, 0);
    *glyph_offset += instr_len;

    u16 points = contour_end_pts[num_of_contours - 1] + 1;
    s16 value = 0;

    u8 flags[points] = {};
    ArrayVec2 vertices = {
        .cap    = points * 3,
        .v      = lu_arena_alloc(arena, sizeof(Vec2) * points * 2),
    };

    printf("%hu\n", points);
    for (u16 i = 0; i < points; i++) {
        flags[i] = buf[(*glyph_offset)++];

        if ((flags[i] & REPEAT_FLAG) == REPEAT_FLAG) {
            u8 times = buf[(*glyph_offset)++];
            assert(i + times < points && "Error: repeat flag is too large");

            for (u16 j = i + 1; j <= i + times; j++) flags[j] = flags[i];
            i += times;
        }
    }

    u8 pair = X_SHORT_VECTOR | X_IS_SAME;

    for (u16 i = 0; i < points; i++) {
        Vec2 vector = {0};

        if          ((flags[i] & pair) == pair) {
            vector.x = buf[(*glyph_offset)++];
        } else if   ((flags[i] & X_SHORT_VECTOR) == X_SHORT_VECTOR) {
            vector.x = -((s16)buf[(*glyph_offset)++]);
        } else if   ((flags[i] & X_IS_SAME) == X_IS_SAME) {
            vector.x = i == 0 ? 0.0 : vertices.v[i - 1].x;
        } else {
            vector.x = consume_u16(buf, glyph_offset, 0);
        }

        printf("%hhu ", flags[i] & ON_CURVE);
        lu_array_push(arena, vertices, vector);
    }
    printf("\n");

    for (u16 i = 0; i < points; i++) {
        printf("%f\n", vertices.v[i].x);
    }

    /*
    for (u16 i = 0; i < points; i++) {
        if          ((flags[i] & pair) == pair) {
            vertices[i]
            lu_array_push(arena, vertices, (Vec2){ .x = buf[(*glyph_offset)++] });
        } else if   ((flags[i] & X_SHORT_VECTOR) == X_SHORT_VECTOR) {
            lu_array_push(arena, vertices, (Vec2){ .x = -((s16)buf[(*glyph_offset)++]) });
        } else if   ((flags[i] & X_IS_SAME) == X_IS_SAME) {
            lu_array_push(arena, vertices, (Vec2){ .x = i == 0 ? 0.0 : vertices[i - 1].x });
        } else {
            lu_array_push(arena, vertices, (Vec2){ .x = consume_u16(buf, glyph_offset, 0) });
        }
    }


    for (u16 i = 0; i < points; i++) {
        switch (flags[i] & (X_SHORT_VECTOR | X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)) {
            case X_SHORT_VECTOR:
                value = -((s16)buf[glyf_off++]) + x_err;
                vertices[i + 1].x = ((f32)value) + vertices[i].x;
                break;
            case X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR:
                vertices[i + 1].x = vertices[i].x;
                break;
            case X_SHORT_VECTOR | X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR:
                value = ((s16)buf[glyf_off++]) + x_err;
                vertices[i + 1].x = ((f32)value) + vertices[i].x;
                break;
            default:
                value = ((s16)ru16(buf, glyf_off)) + x_err; glyf_off += 2;
                vertices[i + 1].x = ((f32)value) + vertices[i].x;
                break;
        }
    }

    for (u16 i = 0; i < points; i++) {
        switch (flags[i] & (Y_SHORT_VECTOR | Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)) {
            case Y_SHORT_VECTOR:
                value = -((s16)buf[glyf_off++]) + x_err;
                vertices[i + 1].y = ((f32)value) + vertices[i].y;
                break;
            case Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR:
                vertices[i + 1].y = vertices[i].y;
                break;
            case Y_SHORT_VECTOR | Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR:
                value = ((s16)buf[glyf_off++]) + x_err;
                vertices[i + 1].y = ((f32)value) + vertices[i].y;
                break;
            default:
                value = ((s16)ru16(buf, glyf_off)) + x_err; glyf_off += 2;
                vertices[i + 1].y = ((f32)value) + vertices[i].y;
                break;
        }
    }

    f32 x_denom = (x_max - x_min);
    f32 y_denom = (y_max - y_min);
    printf("%f - %f\n", x_denom, y_denom);

    printf("I've got: %lu points\n", *size);
    *size -= 1;
    vertices = &vertices[1];

    // Should use this scaling
    // pointSize * resolution / (72 points per inch * units_per_em).

    for (u16 i = 0; i < *size; i++) {
        printf("x: %f - y: %f\n", vertices[i].x, vertices[i].y);
        vertices[i].x = vertices[i].x / 1000.f;
        vertices[i].y = vertices[i].y / 1000.f;
    }
    
    // TODO: prepare actual vertex array using approximations for the bezier lines
    printf("%hu\n", end_pts_of_contours[num_of_contours - 1]);
    */
    return vertices;
}

void read_compound_glyph(u8 *buf, u64 *glyph_offset) {

}

ArrayVec2 read_glyph_table(Arena *arena, u8 *buf, u64 *glyph_offset) {
    s16 num_of_contours = consume_u16(buf, glyph_offset, 0);
    s16 x_min           = consume_u16(buf, glyph_offset, 0);
    s16 y_min           = consume_u16(buf, glyph_offset, 0);
    s16 x_max           = consume_u16(buf, glyph_offset, 0);
    s16 y_max           = consume_u16(buf, glyph_offset, 0);
    
    printf("-- Glyf Table --\n");
    printf("contours: %hd\n", num_of_contours);
    printf("y: %hd - %hd | x: %hd - %hd\n", y_min, y_max, x_min, x_max);

    if (num_of_contours < 0) {
        read_compound_glyph(buf, glyph_offset);
        return (ArrayVec2){0};
    }

    return read_simple_glyph(arena, buf, glyph_offset, num_of_contours);
}

ArrayVec2 lu_extract_glyph_from_font(Arena *arena, String font, u16 code_point, size_t *size) {
    u8 *buf = lu_read_font(arena, font);

    u64 read_offset = offsetof(OffsetSubtable, num_tables);
    u64 skip        = sizeof(OffsetSubtable) - read_offset - sizeof(u16);
    u16 num_tables  = consume_u16(buf, &read_offset, skip);

    TableDirectory tables[TABLE_SIZE] = {};
    read_table_directory(buf, &read_offset, num_tables, tables);

    u16 loca_index_format   = read_head_table(buf, tables[HEAD]);
    u16 glyph_id = read_glyph_id_from_cmap_table(buf, tables[CMAP], code_point);
    printf("GLYPH_ID: %hu\n", glyph_id);

    u64 glyph_offset        = tables[LOCA].offset + glyph_id * loca_index_format;
    u64 glyph_id_offset     = consume_u32(buf, &glyph_offset, 0) + tables[GLYF].offset;

    printf("%lu\n", glyph_id_offset);

    return read_glyph_table(arena, buf, &glyph_id_offset);
}

/* 
 * Q
 * table directory
 * ===============
 * Type 	Name 	    Description
 * uint32 	tag 	    4-byte identifier
 * uint32 	checkSum 	checksum for this table
 * uint32 	offset 	    offset from beginning of sfnt
 * uint32 	length 	    length of this table in byte (actual length not padded length)
void lu_parse_table_directory() {
}
*/ 

