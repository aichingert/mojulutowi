#include "ttf.h"

// References: 
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html

#define ru16(buf, off) buf[off] << 8 | buf[off + 1]
#define ru32(buf, off) buf[off] << 24 | buf[off + 1] << 16 | buf[off + 2] << 8 | buf[off + 3]

void lu_parse_ttf(const char *font) {
    // TODO: should make custom file functions
    FILE *file = fopen(font, "r");
    if (!file) {
        printf("ERROR: file is null!\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    size_t size = file_size / sizeof(u8);
    u8 *buf = (u8*)malloc(file_size);
    fread(buf, sizeof(u32), size, file);
    fclose(file);

    // offset table - table
    u32 scaler = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
    u32 skip = sizeof(u16) * 4 + sizeof(u32);

    if (scaler == 0x00010000) {
        printf("adobe\n");
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

    u16 loca_format = ru16(buf, head_off + 2 * 2 + 4 * 3 + 2 * 2 + 2 * 8 + 8 * 2);
    u16 loca_offset_size = 2 + 2 * loca_format;

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
        printf("ERROR: font has no format4 encoding\n");
        return;
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

    u16 *codes = (u16*)malloc(sizeof(u16) * seg_count * 4);
    u16 *end_codes          = codes + seg_count * 0;
    u16 *start_codes        = codes + seg_count * 1;
    s16 *id_deltas          = (s16*)codes + seg_count * 2;
    u16 *id_range_offsets   = codes + seg_count * 3;

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

    u16 code_point = 0xC4;
    u16 glyph_id   = 0;

    for (u16 i = 0; i < seg_count; i++) {
        if (end_codes[i] >= code_point && start_codes[i] < code_point) {
            u16 id_range_off = id_range_offsets[i];
            u16 id_delta     = id_deltas[i];

            if (id_range_off == 0) {
                glyph_id = code_point + id_delta;
                break;
            } else if (id_range_off == 0xFFFF) {
                printf("ERROR: malformed font");
                break;
            }

            u16 delta = (code_point - start_codes[i]) * 2;
            u16 id_range_offset_pos = format4_off - 2 * seg_count + i;
            u16 pos = id_range_offset_pos + delta + id_range_off;

            // TODO: 0 in buf is a missing glyph
            printf("%hu\n", pos);

            // TODO: glyph_id = buf[pos] + id_delta
            printf("TODO\n");
            break;
        }
    }

    // loca - table
    // used to identify offset into the glyf table
    u16 glyph_id_offset = ru16(buf, loca_off + glyph_id * loca_offset_size);
    printf("glyph_id: %hu - offset: %hu\n", glyph_id, glyph_id_offset);

    // glyf - table

    free(codes);
    free(buf);
}

/* 
 * table directory
 * ===============
 * Type 	Name 	    Description
 * uint32 	tag 	    4-byte identifier
 * uint32 	checkSum 	checksum for this table
 * uint32 	offset 	    offset from beginning of sfnt
 * uint32 	length 	    length of this table in byte (actual length not padded length)
*/ 
void lu_parse_table_directory() {

}
