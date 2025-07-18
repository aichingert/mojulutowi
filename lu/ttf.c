#include "ttf.h"

// References: 
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
// https://learn.microsoft.com/en-us/typography/opentype/spec/cmap

/*
 * shortFrac 	16-bit signed fraction
 * Fixed 	    16.16-bit signed fixed-point number
 * FWord 	    16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
 * uFWord 	    16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
 * F2Dot14 	    16-bit signed fixed number with the low 14 bits representing fraction.
 * longDateTime The long internal format of a date in seconds since 12:00 midnight, January 1, 1904. It is represented as a signed 64-bit integer.
*/

#define ru16(buf, off) ((u16)buf[off] << 8) | ((u16)buf[off + 1])
#define ru32(buf, off) ((u32)buf[off] << 24) | ((u32)buf[off + 1] << 16) | ((u32)buf[off + 2] << 8) | ((u32)buf[off + 3])
#define ru64(buf, off) (((u64)ru32(buf, off)) << (u64)32) | ((u64)ru32(buf, off + 4))

#define REPEAT_FLAG                             0b00001000
#define X_SHORT_VECTOR                          0b00000010
#define Y_SHORT_VECTOR                          0b00000100
#define X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR    0b00010000
#define Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR    0b00100000

Vertex *lu_extract_glyph_from_font(const char *font, u16 code_point, size_t *size) {
    // TODO: should make custom file functions
    FILE *file = fopen(font, "r");
    if (!file) {
        assert(false && "ERROR: file is null!");
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    size_t b_size = file_size / sizeof(u8);
    u8 *buf = (u8*)malloc(file_size);
    fread(buf, sizeof(u32), b_size, file);
    fclose(file);

    // offset table - table
    u32 scaler = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
    u32 skip = sizeof(u16) * 4 + sizeof(u32);

    if (scaler == 0x00010000) {
        printf("adobe - microsoft\n");
    }

    // table directory - table
    u32 cmap_tag = ((u32)'c') << 24 | ((u32)'m') << 16 | ((u32)'a') << 8 | ((u32)'p');
    u32 cmap_off = 0;
    u32 head_tag = ((u32)'h') << 24 | ((u32)'e') << 16 | ((u32)'a') << 8 | ((u32)'d');
    u32 head_off = 0;
    u32 loca_tag = ((u32)'l') << 24 | ((u32)'o') << 16 | ((u32)'c') << 8 | ((u32)'a');
    u32 loca_off = 0;
    u32 glyf_tag = ((u32)'g') << 24 | ((u32)'l') << 16 | ((u32)'y') << 8 | ((u32)'f');
    u32 glyf_off = 0;

    u16 tables = buf[4] << 8 | buf[5];

    for (u16 i = 0; i < tables; i++) {
        u32 tag = ru32(buf, skip); skip += 4;
        skip += 4; // TODO: check checksum

        if        (tag == cmap_tag) {
            cmap_off = ru32(buf, skip);
        } else if (tag == head_tag) {
            head_off = ru32(buf, skip);
        } else if (tag == loca_tag) {
            loca_off = ru32(buf, skip);
        } else if (tag == glyf_tag) {
            glyf_off = ru32(buf, skip);
        }

        skip += 8; // len and offset skip
    }

    // head - table
    u16 maj_version = ru16(buf, head_off); head_off += 2; 
    u16 min_version = ru16(buf, head_off); head_off += 2; 
    u32 font_revision = ru32(buf, head_off); head_off += 4;
    u32 check_sum = ru32(buf, head_off); head_off += 4;
    u32 magic_num = ru32(buf, head_off); head_off += 4;
    assert(magic_num == 0x5F0F3CF5);
    u16 header_flags = ru16(buf, head_off); head_off += 2;
    u16 units_per_em = ru16(buf, head_off); head_off += 2;
    u64 created = ru64(buf, head_off); head_off += 8;
    u64 modified = ru64(buf, head_off); head_off += 8;
    s16 header_x_min = ru16(buf, head_off); head_off += 2;
    s16 header_y_min = ru16(buf, head_off); head_off += 2;
    s16 header_x_max = ru16(buf, head_off); head_off += 2;
    s16 header_y_max = ru16(buf, head_off); head_off += 2;
    u16 mac_style = ru16(buf, head_off); head_off += 2;
    u16 lowest_rec_ppem = ru16(buf, head_off); head_off += 2;
    s16 font_direction_hint = ru16(buf, head_off); head_off += 2;
    s16 index_to_loc_format = ru16(buf, head_off); head_off += 2;
    s16 glyph_data_format = ru16(buf, head_off); head_off += 2;

    printf("HEADER \n");
    printf("VERSION: %hu.%hu\n", maj_version, min_version);
    printf("ignored: %u\n", font_revision & check_sum & header_flags & units_per_em
            & mac_style & lowest_rec_ppem & font_direction_hint & glyph_data_format);
    printf("created: %lu - modified %lu\n", created, modified);
    printf("index to loc format: %hd\n", index_to_loc_format);
    printf("coords min: x [\t%hd ] | y [\t%hd ]\n", header_x_min, header_y_min);
    printf("coords max: x [\t%hd ] | y [\t%hd ]\n", header_x_max, header_y_max);

    assert(index_to_loc_format == 1 && "TODO: change read 32 and 16 byte function for loca lookup");
    u16 loca_offset_size = 4;

    // cmap - table 

    u32 offset = cmap_off;
    u32 format4_off = 0;

    u16 version = ru16(buf, offset); offset += 2;
    u16 subtables = ru16(buf, offset); offset += 2;

    printf("version: %hu - subtables: %hu\n", version, subtables);

    // TODO: ugly but for now search for format4 in cmaps
    for (u16 i = 0; i < subtables; i++) {
        u16 plat_id = ru16(buf, offset);
        u16 plat_spec_id = ru16(buf, offset + 2);
        u32 sub_offset = ru32(buf, offset + 4);
        offset += 8;

        u32 off = cmap_off + sub_offset;
        u16 format = ru16(buf, off);

        printf("PLAT_ID: %hu \t| PLAT_SPEC_ID: %hu \t| OFFSET: %u\n", plat_id, plat_spec_id, sub_offset);
        if (format == 4) {
            format4_off = off;
        }
    }

    if (format4_off == 0) {
        assert(false && "ERROR: font has no format4 encoding");
    }

    u16 format = ru16(buf, format4_off); format4_off += 2;
    assert(format == 4);
    ru16(buf, format4_off); format4_off += 2; // length

    u16 lang = ru16(buf, format4_off); format4_off += 2; // mac specific
    assert(lang == 0);
    u16 seg_count_x2 = ru16(buf, format4_off); format4_off += 2;
    u16 seg_count = seg_count_x2 / 2;

    ru16(buf, format4_off); format4_off += 2;
    ru16(buf, format4_off); format4_off += 2;
    ru16(buf, format4_off); format4_off += 2;

    u16 end_codes[seg_count] = {};
    u16 start_codes[seg_count] = {};
    u16 id_deltas[seg_count] = {};
    u16 id_range_offsets[seg_count] = {};

    for (u16 i = 0; i < seg_count; i++) {
        end_codes[i] = ru16(buf, format4_off); format4_off += 2;
    }
    assert(end_codes[seg_count - 1] == 0xFFFF);

    u16 pad = ru16(buf, format4_off); format4_off += 2; // reserve pad
    assert(pad == 0);

    for (u16 i = 0; i < seg_count; i++) {
        start_codes[i] = ru16(buf, format4_off); format4_off += 2;
    }
    for (u16 i = 0; i < seg_count; i++) {
        id_deltas[i] = ru16(buf, format4_off); format4_off += 2;
    }
    for (u16 i = 0; i < seg_count; i++) {
        id_range_offsets[i] = ru16(buf, format4_off); format4_off += 2;
    }

    u16 glyph_id   = 0;

    for (u16 i = 0; i < seg_count; i++) {
        if (end_codes[i] >= code_point && start_codes[i] < code_point) {
            u16 id_range_off = id_range_offsets[i];
            u16 id_delta     = id_deltas[i];

            if (id_range_off == 0) {
                glyph_id = code_point + id_delta;
                break;
            } else if (id_range_off == 0xFFFF) {
                free(buf);
                assert(false && "ERROR: malformed font");
            }

            u16 delta = (code_point - start_codes[i]) * 2;
            u16 id_range_offset_pos = format4_off - 2 * seg_count + i;
            u16 pos = id_range_offset_pos + delta + id_range_off;

            // TODO: 0 in buf is a missing glyph
            printf("%hu\n", pos);

            // TODO: glyph_id = buf[pos] + id_delta
            free(buf);
            assert(false && "TODO");
        }
    }

    // loca - table
    // used to identify offset into the glyf table
    printf("%u - %hu - %hu\n", loca_off, glyph_id, loca_offset_size);

    u32 glyph_id_offset = ru32(buf, loca_off + glyph_id * loca_offset_size);
    printf("glyph_id: %hu - offset: %hu\n", glyph_id, glyph_id_offset);

    // glyf - table
    glyf_off += (u32)glyph_id_offset;

    s16 num_of_contours = ru16(buf, glyf_off); glyf_off += 2;
    s16 x_min           = ru16(buf, glyf_off); glyf_off += 2;
    s16 y_min           = ru16(buf, glyf_off); glyf_off += 2;
    s16 x_max           = ru16(buf, glyf_off); glyf_off += 2;
    s16 y_max           = ru16(buf, glyf_off); glyf_off += 2;
    s16 x_err = 0;
    s16 y_err = 0;

    if (x_min < 0) {
        x_err = -x_min;
        x_max += x_err;
        x_min = 0;
    } else if (x_min > 0) {
        x_min = 0;
    }
    if (y_min < 0) {
        y_err = -y_min;
        y_max += y_err;
        y_min = 0;
    } else if (y_min > 0) {
        y_min = 0;
    }

    printf("contours: %hd\n", num_of_contours);
    printf("y: %hd - %hd | x: %hd - %hd\n", y_min, y_max, x_min, x_max);

    if (num_of_contours < 0) {
        free(buf);
        assert(false && "TODO: compound glyhs are not yet supported!");
    }

    u16 end_pts_of_contours[num_of_contours] = {};
    for (s16 i = 0; i < num_of_contours; i++) {
        end_pts_of_contours[i] = ru16(buf, glyf_off); glyf_off += 2;
    }

    // TODO: ignoring instructions might never use them
    u16 instr_len = ru16(buf, glyf_off); glyf_off += 2;
    glyf_off += instr_len;

    u16 points = end_pts_of_contours[num_of_contours - 1] + 1;
    printf("LEN: %hu\n", points);
    u8 flags[points] = {};

    *size = points + 1;
    Vertex *vertices = (Vertex*)malloc(sizeof(Vertex) * (points + 1));
    vertices[0] = (Vertex){
        .x = 0.0,
        .y = 0.0,
    };

    // TODO: more size checks
    u16 i = 0;
    while (i < points) {
        flags[i] = buf[glyf_off++];

        if ((flags[i] & REPEAT_FLAG) == REPEAT_FLAG) {
            u8 times = buf[glyf_off++];

            printf("%hu - repeat: %hu - size: %hu\n", i, times, points);
            assert(i + times < points && "ERROR: flags repeat outside flags array");
            for (u16 j = i + 1; j <= i + times; j++) {
                flags[j] = flags[i];
            }

            i += times;
        }

        i += 1;
    }
    printf("flags done \n");
    s16 value = 0;

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

    for (u16 i = 0; i < points + 1; i++) {
        vertices[i].x = (vertices[i].x - x_min) / x_denom;
        vertices[i].y = (vertices[i].y - y_min) / y_denom;
        printf("x: %f - y: %f\n", vertices[i].x, vertices[i].y);
    }
    
    printf("%hu\n", end_pts_of_contours[num_of_contours - 1]);

    free(buf);
    return vertices;
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

