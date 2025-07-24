#include "ttf.h"
#include "arena.h"
#include "mem.h"
#include "math.h"
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

    (void)maj_version;
    (void)min_version;
    (void)font_revision;
    (void)checksum;
    (void)global_x_min;
    (void)global_y_min;
    (void)global_y_max;
    (void)global_x_max;
    (void)modified;
    (void)created;
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
    u16 version     = consume_u16(buf, &read_offset, 0); (void)version;
    u16 subtables   = consume_u16(buf, &read_offset, 0);

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

            printf("%hu - %hu\n", end_codes[i], start_codes[i]);

            u16 delta = (code_point - start_codes[i]) * 2;

            id_range_offset_pos += i * 2;
            u16 pos = id_range_offset_pos + delta + id_range_offsets[i];

            if (buf[pos] == 0) {
                printf("MISSING GLYPH\n");
                return 0;
            }

            return buf[pos] + id_deltas[i];
        }
    }

    assert(false && "Error: not even missing glyph id found");
    return 0;
}

void read_simple_coordinates(
        u8  *buf, 
        u64 *glyph_offset, 
        s16 *coordinates, 
        u8  *flags, 
        u16 points, 
        u8  vec_flag, 
        u8  same_flag) 
{
    u8  pair = vec_flag | same_flag;

    for (u16 i = 0; i < points; i++) {
        s16 v = 0;

        if          ((flags[i] & pair) == pair) {
            v = buf[(*glyph_offset)++];
        } else if   ((flags[i] & vec_flag) == vec_flag) {
            v = -((s16)buf[(*glyph_offset)++]);
        } else if   ((flags[i] & same_flag) == 0) {
            v = consume_u16(buf, glyph_offset, 0);
        }

        v += i > 0 ? coordinates[i - 1] : 0;
        coordinates[i] = v;
    }
}

void outline_simple_glyph_contour(
        Arena *arena, 
        u8 *flags,
        u16 index,
        u16 contour_end_point,
        s16 *x_coordinates,
        s16 *y_coordinates,
        ArrayVec2 *vertices)
{
    // TODO: check that some elements got pushed
    u64 start = vertices->len != 0 ? vertices->len: 0;

    Vec2 previous_point  = index == 0 
        ? (Vec2){0} 
        : (Vec2){ .x = x_coordinates[index], .y = y_coordinates[index] };
    bool previous_on_curve = false;

    for (u16 i = index; i <= contour_end_point; i++) {
        Vec2 current = { .x = (f32)x_coordinates[i], .y = (f32)y_coordinates[i] };
        bool on_curve = (flags[i] & ON_CURVE) == ON_CURVE;

        if (i == index) {
            previous_point = current;
            previous_on_curve = on_curve;
            continue;
        }

        if (previous_on_curve && on_curve) {
            lu_array_push(arena, *vertices, previous_point);
            lu_array_push(arena, *vertices, current);
            previous_point = current;
        } else if (previous_on_curve) {
            for (u16 j = i + 1;; j++) {
                if ( j > contour_end_point) {
                    j = index;
                }

                bool next_on_curve = (flags[j] & ON_CURVE) == ON_CURVE;
                Vec2 next = { .x = x_coordinates[j], .y = y_coordinates[j] };

                Vec2 p_0 = previous_point;
                Vec2 c_0 = current;
                Vec2 p_1 = {0};

                if (next_on_curve) {
                    p_1 = next;
                } else {
                    p_1 = lerp_v2(current, next, 0.5);
                }

                lu_array_push(arena, *vertices, p_0);

                // Approximation for bezier
                // p(t) = (1-t)^2p0 + 2t(1-t)p1 + t^2p2
                for (f32 t = 0.001f; t < 1.0f; t += 0.001f) {
                    Vec2 p_t = {
                        .x = (1.0f - t) * (1.0f - t) * p_0.x + 2.0f * t * (1.0f - t) * c_0.x + t * t * p_1.x,
                        .y = (1.0f - t) * (1.0f - t) * p_0.y + 2.0f * t * (1.0f - t) * c_0.y + t * t * p_1.y,
                    };

                    lu_array_push(arena, *vertices, p_t);
                    lu_array_push(arena, *vertices, p_t);
                }

                lu_array_push(arena, *vertices, p_1);

                if (j == index) {
                    return;
                } else if (next_on_curve) {
                    i = j;
                    previous_point = next;
                    previous_on_curve = next_on_curve;
                    break;
                }

                previous_point = current;
                current = next;
            }
        } else {
            previous_point = lerp_v2(previous_point, current, 0.5);
            previous_on_curve = true;
        } 
    }

    lu_array_push(arena, *vertices, previous_point);
    lu_array_push(arena, *vertices, vertices->v[start]);
}

ArrayVec2 read_simple_glyph(Arena *arena, u8 *buf, u64 *glyph_offset, s16 num_of_contours) {
    assert(num_of_contours > 0 && "Error: no contours for simple glyph");

    u16 contour_end_pts[num_of_contours] = {};

    for (u16 i = 0; i < num_of_contours; i++) {
        contour_end_pts[i] = consume_u16(buf, glyph_offset, 0);
    }

    // NOTE: ignoring instructions might never use them
    u16 instr_len = consume_u16(buf, glyph_offset, 0);
    *glyph_offset += instr_len;

    u16 points = contour_end_pts[num_of_contours - 1] + 1;
    u8          flags[points] = {};
    s16         x_coordinates[points] = {};
    s16         y_coordinates[points] = {};
    ArrayVec2   vertices = {
        .cap    = points * 4,
        .v      = lu_arena_alloc(arena, sizeof(Vec2) * points * 2),
    };
    
    for (u16 i = 0; i < points; i++) {
        flags[i] = buf[(*glyph_offset)++];

        if ((flags[i] & REPEAT_FLAG) == REPEAT_FLAG) {
            u8 times = buf[(*glyph_offset)++];
            assert(i + times < points && "Error: repeat flag is too large");

            for (u16 j = i + 1; j <= i + times; j++) flags[j] = flags[i];
            i += times;
        }
    }

    read_simple_coordinates(buf, glyph_offset, x_coordinates, flags, points, X_SHORT_VECTOR, X_IS_SAME);
    read_simple_coordinates(buf, glyph_offset, y_coordinates, flags, points, Y_SHORT_VECTOR, Y_IS_SAME); 

    for (s16 i = 0;  i < num_of_contours; i++) {
        u64 index = i == 0 ? 0 : contour_end_pts[i - 1] + 1;
        outline_simple_glyph_contour(
                arena, 
                flags, 
                index, 
                contour_end_pts[i], 
                x_coordinates, 
                y_coordinates, 
                &vertices);
    }

    for (u64 i = 0; i < vertices.len; i++) {
        vertices.v[i].x /= 1000.0f;
        vertices.v[i].y /= -1000.0f;
    }

    return vertices;
}

// TODO: implement
void read_compound_glyph(u8 *buf, u64 *glyph_offset) {
    assert(false && "TODO: implement compound glyphs");
}

ArrayVec2 read_glyph_table(Arena *arena, u8 *buf, u64 *glyph_offset) {
    s16 num_of_contours = consume_u16(buf, glyph_offset, 0);
    s16 x_min           = consume_u16(buf, glyph_offset, 0);
    s16 y_min           = consume_u16(buf, glyph_offset, 0);
    s16 x_max           = consume_u16(buf, glyph_offset, 0);
    s16 y_max           = consume_u16(buf, glyph_offset, 0);

    (void)x_min; (void)y_min; (void)x_max; (void)y_max;
    
    if (num_of_contours < 0) {
        read_compound_glyph(buf, glyph_offset);
        return (ArrayVec2){0};
    }

    return read_simple_glyph(arena, buf, glyph_offset, num_of_contours);
}

ArrayVec2 lu_extract_glyph_from_font(Arena *arena, String font, u16 code_point) {
    u8 *buf = lu_read_font(arena, font);

    u64 read_offset = offsetof(OffsetSubtable, num_tables);
    u64 skip        = sizeof(OffsetSubtable) - read_offset - sizeof(u16);
    u16 num_tables  = consume_u16(buf, &read_offset, skip);

    TableDirectory tables[TABLE_SIZE] = {};
    read_table_directory(buf, &read_offset, num_tables, tables);

    u16 loca_index_format   = read_head_table(buf, tables[HEAD]);
    u16 glyph_id = read_glyph_id_from_cmap_table(buf, tables[CMAP], code_point);

    u64 glyph_offset        = tables[LOCA].offset + glyph_id * loca_index_format;
    u64 glyph_id_offset     = consume_u32(buf, &glyph_offset, 0) + tables[GLYF].offset;

    return read_glyph_table(arena, buf, &glyph_id_offset);
}

