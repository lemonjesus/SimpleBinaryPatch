#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_bin_patch.h"

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("usage: %s [old_file] [new_file] [patch]\n\n", argv[0]);
        printf("generates the patch required for apply_patch to change old_file into new_file.\n");
        printf("works best when the differences are sparse, otherwise the patch will be massive.\n\n");
        return 1;
    }

    FILE* old_fp = fopen(argv[1], "rb");
    FILE* new_fp = fopen(argv[2], "rb");

    fseek(old_fp, 0L, SEEK_END);
    fseek(new_fp, 0L, SEEK_END);
    uint32_t old_len = ftell(old_fp);
    uint32_t new_len = ftell(new_fp);
    if(old_len != new_len) {
        printf("files do not match in length. simple_bin_patch does not support shifting reigons of memory.\n\n");
        fclose(old_fp); fclose(new_fp);
        return 2;
    }

    rewind(old_fp); rewind(new_fp);

    FILE* diff_fp = fopen(argv[3], "wb");

    uint8_t* old_buf = (uint8_t*)malloc(old_len);
    uint8_t* new_buf = (uint8_t*)malloc(new_len);

    fread(old_buf, sizeof(uint8_t), old_len, old_fp);
    fread(new_buf, sizeof(uint8_t), new_len, new_fp);

    // the worst case scenario is if every other byte is the same and
    // every other byte is different. 1 byte for the opcode, 4 for the
    // length, and one for the byte. hence the *6. in those cases, this
    // algorithm shouldn't be used.
    uint8_t* diff_buf = (uint8_t*)calloc(old_len * 6, sizeof(uint8_t));

    uint32_t diff_len = generate_patch(old_buf, new_buf, diff_buf, old_len);

    fwrite(diff_buf, sizeof(uint8_t), diff_len, diff_fp);

    fclose(old_fp); fclose(new_fp); fclose(diff_fp);
    free(old_buf);  free(new_buf);  free(diff_buf);
    return 0;
}