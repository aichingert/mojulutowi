#include "ttf.h"

// References: 
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html

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

    u32 scaler = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
    u32 skip = sizeof(u16) * 4 + sizeof(u32);

    if (scaler == 0x00010000) {
        printf("adobe\n");
    }

    u16 tables = buf[4] << 8 | buf[5];
    printf("%d\n", skip);

    for (u16 i = 0; i < tables; i++) {
        printf("TABLE: ");
        for (u32 j = skip; j < skip + 4; j++) {
            printf("%c", buf[j]);
        }

        skip += 12;
        u32 len = (u8)buf[skip] << 24 | buf[skip + 1] << 16 | buf[skip + 2] << 8 | buf[skip + 3];
        printf(" - size: %d\n", len);
        skip += 4;
    }
}

void lu_parse_table_directory() {
}
