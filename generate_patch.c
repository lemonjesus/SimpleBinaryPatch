#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bin_patch.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: %s [old_file] [new_file] [patch_file]\n\n", argv[0]);
        printf("generates the patch required for apply_patch to change old_file into new_file.\n");
        printf("works best when the differences are sparse, otherwise the patch will be massive.\n\n");
        return 1;
    }

    FILE *old = fopen(argv[1], "rb");
    FILE *new = fopen(argv[2], "rb");
    if (!old || !new) {
        printf("failed to open new or old file\n");
        return 2;
    }

    fseek(old, 0L, SEEK_END);
    fseek(new, 0L, SEEK_END);
    uint32_t bin_len = ftell(old);
    if (bin_len != ftell(new)) {
        printf("files do not match in length. simple_bin_patch does not support shifting\n"
               "reigons of memory.\n\n");
        fclose(old);
        fclose(new);
        return 3;
    }
    rewind(old);
    rewind(new);

    char *old_buf = (char *) malloc(bin_len);
    char *new_buf = (char *) malloc(bin_len);
    if (!old_buf || !new_buf) {
        printf("failed to allocate file buffers\n");
        fclose(old);
        fclose(new);
        return 4;
    }

    fread(old_buf, sizeof(char), bin_len, old);
    fread(new_buf, sizeof(char), bin_len, new);
    fclose(old);
    fclose(new);

    // Allocating for the worst case
    struct binary_patch patch = {
        .diff_len   = len_to_index(bin_len),
        .diff_start = malloc((bin_len + 1) * sizeof(len_t)),
        .diff_delta = malloc((bin_len + 1) * sizeof(len_t)),
        .heap_len   = len_to_index(bin_len / 2 + 1),
        .heap       = malloc(bin_len / 2 + 1),
    };
    if (!patch.diff_start || !patch.diff_delta || !patch.heap) {
        printf("Failed to allocate patch memory\n");
        free(old_buf);
        free(new_buf);
        return 5;
    }

    enum bp_ret_code ret = gen_patch(old_buf, new_buf, bin_len, &patch);
    free(old_buf);
    free(new_buf);

    if (ret) {
        printf("generating patch failed because %d\n", ret);
        free(patch.diff_start);
        free(patch.diff_delta);
        free(patch.heap);
        return 6;
    }

    FILE *patch_out = fopen(argv[3], "wb");
    if (!patch_out) {
        printf("failed to open output file\n");
        free(patch.diff_start);
        free(patch.diff_delta);
        free(patch.heap);
        return 7;
    }
    fwrite(&patch.diff_len,   sizeof(index_t), 1,                            patch_out);
    fwrite( patch.diff_start, sizeof(index_t), index_to_len(patch.diff_len), patch_out);
    fwrite( patch.diff_delta, sizeof(index_t), index_to_len(patch.diff_len), patch_out);
    fwrite( patch.heap,       sizeof(char),    index_to_len(patch.heap_len), patch_out);

    fclose(patch_out);
    free(patch.diff_start);
    free(patch.diff_delta);
    free(patch.heap);

    return 0;
}
