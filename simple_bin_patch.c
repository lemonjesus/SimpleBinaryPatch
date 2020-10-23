#include "simple_bin_patch.h"

// private methods
void _memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
uint32_t _read_int_as_uint8_ts(uint8_t* src);
void _write_int_as_bytes(uint32_t x, uint8_t* dest);

uint32_t generate_patch(uint8_t* old, uint8_t* new, uint8_t* patch, uint32_t size) {
    uint32_t src_offset = 0;
    uint32_t patch_offset = 0;
    uint32_t length = 0;  // length argument to function

    while(src_offset < size) {
        length = 0;
        if(*(old + src_offset) == *(new + src_offset)) {
            while(*(old + src_offset) == *(new + src_offset) && src_offset < size) {
                length++; // find the end of the similarity
                src_offset++;
            }
            *(patch + patch_offset) = 0; // 0 = skip
            _write_int_as_bytes(length, patch + patch_offset + 1);
            patch_offset += sizeof(uint32_t) + 1;
        } else {
            while(*(old + src_offset) != *(new + src_offset) && src_offset < size) {
                length++; // find the end of the disimilarity
                src_offset++;
            }
            *(patch + patch_offset) = 1; // 1 = write
            _write_int_as_bytes(length, patch + patch_offset + 1);
            patch_offset += sizeof(uint32_t) + 1;
            _memcpy(patch + patch_offset, new + src_offset - length, length);
            patch_offset += length;
        }
    }
    return patch_offset;
}

uint32_t apply_patch(uint8_t* patch, uint8_t* dest, uint32_t patch_length) {
    uint32_t patch_offset = 0;
    uint32_t dest_offset = 0;
    uint8_t opcode = 0;
    uint32_t length = 0;

    while(patch_offset < patch_length) {
        opcode = *(patch + patch_offset++);
        length = _read_int_as_uint8_ts(patch + patch_offset);
        patch_offset += sizeof(uint32_t);
        switch(opcode) {
        case 0:
            dest_offset += length;
            break;

        case 1:
            _memcpy(dest + dest_offset, patch + patch_offset, length);
            dest_offset += length;
            patch_offset += length;
            break;

        default:
            // inform of illegal opcode, it could mean the patch was corrupted
            return -1;
        }
    }
    return 0;
}

void _memcpy(uint8_t* dest, const uint8_t* src, uint32_t len) {
  while (len--) *dest++ = *src++;
}

uint32_t _read_int_as_uint8_ts(uint8_t* src) {
    return ((uint32_t) src[3]) << 24 | ((uint32_t) src[2]) << 16 | ((uint32_t) src[1]) << 8 | (uint32_t) src[0];
}

void _write_int_as_bytes(uint32_t x, uint8_t* dest) {
    dest[0] = x;
    dest[1] = x >> 8;
    dest[2] = x >> 16;
    dest[3] = x >> 24;
}