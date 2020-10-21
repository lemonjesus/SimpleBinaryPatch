#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_bin_patch.h"

// this is the file I use to test generate and apply patch

unsigned char old[25] = {
    0x34, 0x23, 0x8D, 0x2B, 0xFF, 0x25, 0xEA, 0xC0, 0xBF, 0x16, 
    0x0D, 0x93, 0x80, 0x9F, 
    0x08, 0xDD, 0x21, 
    0x2B, 
    0xCD, 0xA3, 0xD0, 0x17, 0xA5, 0x79, 0x66
};

unsigned char new[25] = {
    0x34, 0x23, 0x8D, 0x2B, 0xFF, 0x25, 0xEA, 0xC0, 0xBF, 0x16, // 10 same
    0x0E, 0x9E, 0x8E, 0x9E,                                     // 4 diff
    0x08, 0xDD, 0x21,                                           // 3 same
    0x2E,                                                       // 1 diff
    0xCD, 0xA3, 0xD0, 0x17, 0xA5, 0x79, 0x66                    // 7 same
};

int main() {
    byte* patch_buf = (byte*)calloc(25 * 6, sizeof(byte));

    long len = generate_patch(old, new, patch_buf, 25);
    printf("len = %d\n", len);
    for(int i = 0; i < (len / 8) + 1; i++) {
        printf("%.4x | %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", i, patch_buf[i*8+0], patch_buf[i*8+1], patch_buf[i*8+2], patch_buf[i*8+3], patch_buf[i*8+4], patch_buf[i*8+5], patch_buf[i*8+6], patch_buf[i*8+7]);
    }

    int result = apply_patch(patch_buf, old, len);

    printf("apply_patch returned %d, and memcmp returned %d\n", result, memcmp(old, new, 25));

    free(patch_buf);
}