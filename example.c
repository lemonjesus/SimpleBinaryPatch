#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bin_patch.h"

// this is the file I use to test generate and apply patch
#define BUF_LEN    25
#define DIFF_COUNT 3
#define DIFF_SIZE  5

uint8_t old[BUF_LEN] = {
    0x34, 0x23, 0x8D, 0x2B, 0xFF, 0x25, 0xEA, 0xC0, 0xBF, 0x16,
    0x0D, 0x93, 0x80, 0x9F,
    0x08, 0xDD, 0x21,
    0x2B,
    0xCD, 0xA3, 0xD0, 0x17, 0xA5, 0x79, 0x66
};

uint8_t new[BUF_LEN] = {
    0x34, 0x23, 0x8D, 0x2B, 0xFF, 0x25, 0xEA, 0xC0, 0xBF, 0x16, // 10 same
    0x0E, 0x9E, 0x8E, 0x9E,                                     // 4 diff
    0x08, 0xDD, 0x21,                                           // 3 same
    0x2E,                                                       // 1 diff
    0xCD, 0xA3, 0xD0, 0x17, 0xA5, 0x79, 0x66                    // 7 same
};

int main() {
    struct binary_patch patch = {
        .diff_len   = DIFF_COUNT,
        .diff_start = malloc(DIFF_COUNT * sizeof(len_t)),
        .diff_delta = malloc(DIFF_COUNT * sizeof(len_t)),
        .heap_len   = DIFF_SIZE,
        .heap       = malloc(DIFF_SIZE),
    };

    if (!patch.diff_start || !patch.diff_delta || !patch.heap)
        return 1;

    switch (gen_patch(old, new, BIN_LEN, &patch)) {
        case SUCCESS:                                       break;
        case OUT_OF_DIFF: puts("ran out of diff space\n");  goto free_and_die;
        case OUT_OF_HEAP: puts("ran out of heap space\n");  goto free_and_die;
    }

    printf("heap used: %d\n", patch.heap_len);
    printf("diff used: %d\n", patch.diff_len);
    printf("\ti = (start,len)\n");
    for (len_t i = 0; i < patch.diff_len; i++)
        printf("\t%d = (%d,%d)\n", i, patch.diff_start[i], patch.diff_delta[i]);


    apply_patch(old, &patch);
    printf("after applying, memcmp returned %d\n", memcmp(old, new, BUF_LEN));

free_and_die:
    free(patch.diff_start);
    free(patch.diff_delta);
    free(patch.heap);
    return 0;
}
