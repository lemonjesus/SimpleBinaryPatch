#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_bin_patch.h"

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("usage: %s [patch] [old_file] [new_file]\n\n", argv[0]);
        printf("applies a patch to old_file and writes it to new_file.\n");
        printf("use generate_patch to make a compatible patch file.\n\n");
        return 1;
    }

    FILE* patch_fp = fopen(argv[1], "rb");
    FILE* old_fp = fopen(argv[2], "rb");

    fseek(patch_fp, 0L, SEEK_END);
    fseek(old_fp, 0L, SEEK_END);
    uint32_t patch_len = ftell(patch_fp);
    uint32_t old_len = ftell(old_fp);

    rewind(patch_fp); rewind(old_fp);

    FILE* new_fp = fopen(argv[3], "wb");

    uint8_t* patch_buf = (uint8_t*)malloc(patch_len);
    uint8_t* old_buf = (uint8_t*)malloc(old_len);

    fread(patch_buf, sizeof(uint8_t), patch_len, patch_fp);
    fread(old_buf, sizeof(uint8_t), old_len, old_fp);

    uint32_t result = apply_patch(patch_buf, old_buf, patch_len);

    fwrite(old_buf, sizeof(uint8_t), old_len, new_fp);

    if(result != 0) {
        printf("this patch contains an unrecognized opcode. it was skipped and the\n");
        printf("patch was applied, but the output (and the patch) may be bad.\n");
    }

    fclose(patch_fp); fclose(old_fp); fclose(new_fp);
    free(patch_buf);  free(old_buf);
    return result;
}