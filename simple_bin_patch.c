#include <string.h>
#include "simple_bin_patch.h"

long generate_patch(byte* old, byte* new, byte* patch, long size) {
    long src_ptr = 0;
    long patch_ptr = 0;
    int length = 0;  // length argument to function

    while(src_ptr < size) {
        length = 0;
        if(*(old + src_ptr) == *(new + src_ptr)) {
            while(*(old + src_ptr) == *(new + src_ptr) && src_ptr < size) {
                length++; // find the end of the similarity
                src_ptr++;
            }
            patch[patch_ptr] = 0; // 0 = skip
            _write_int_as_bytes(length, patch + patch_ptr + 1);
            patch_ptr += sizeof(int) + 1;
            continue;
        }
        if(*(old + src_ptr) != *(new + src_ptr)) {
            while(*(old + src_ptr) != *(new + src_ptr) && src_ptr < size) {
                length++; // find the end of the disimilarity
                src_ptr++;
            }
            patch[patch_ptr] = 1; // 1 = write
            _write_int_as_bytes(length, patch + patch_ptr + 1);
            patch_ptr += sizeof(int) + 1;
            memcpy(patch + patch_ptr, new + src_ptr - length, length);
            patch_ptr += length;
            continue;
        }
    }
    return patch_ptr;
}

int apply_patch(byte* patch, byte* dest, long patch_length) {
    long patch_ptr = 0;
    long dest_ptr = 0;
    byte opcode = 0;
    int length = 0;

    while(patch_ptr < patch_length) {
        opcode = patch[patch_ptr++];
        length = _read_int_as_bytes(patch + patch_ptr);
        patch_ptr += sizeof(int);
        switch(opcode) {
            case 0:
            dest_ptr += length;
            break;

            case 1:
            memcpy(dest + dest_ptr, patch + patch_ptr, length);
            dest_ptr += length;
            patch_ptr += length;
            break;

            default:
                // inform of illegal opcode, it could mean the patch was corrupted
                return -1;
        }
    }
    return 0;
}

int _read_int_as_bytes(byte* src) {
    return ((int) src[3]) << 24 | ((int) src[2]) << 16 | ((int) src[1]) << 8 | (int) src[0];
}

void _write_int_as_bytes(int x, byte* dest) {
    dest[0] = x;
    dest[1] = x >> 8;
    dest[2] = x >> 16;
    dest[3] = x >> 24;
}