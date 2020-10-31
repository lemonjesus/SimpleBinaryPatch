#include "bin_patch.h"


static void blockcpy(uint8_t *dest, uint8_t *src, len_t len) {
    for (; len--; *dest++ = *src++);
}


len_t index_to_len(index_t n) {
    len_t ret = 0;
    for (uint8_t i = 0; i < sizeof(index_t); i++)
        ret |= n.n[i] << 8 * i;
    return ret;
}


index_t len_to_index(len_t n) {
    index_t ret;
    for (uint8_t i = 0; i < sizeof(index_t); i++)
        ret.n[i] = n >> 8 * i;
    return ret;
}


void apply_patch(uint8_t *dest, struct binary_patch *patch) {
    len_t curr_heap = 0;
    len_t len       = index_to_len(patch->diff_len);
    len_t delta;
    for (len_t i = 0; i < len; i++) {
        delta = index_to_len(patch->diff_delta[i]);
        blockcpy(&dest[index_to_len(patch->diff_start[i])],
                 &patch->heap[curr_heap],
                 delta);
        curr_heap += delta;
    }
}


enum bp_ret_code gen_patch(uint8_t *old, uint8_t *new, len_t bin_len, struct binary_patch *patch) {
    len_t len_diff  = index_to_len(patch->diff_len);
    len_t len_heap  = index_to_len(patch->heap_len);
    len_t curr_diff = 0;
    len_t curr_heap = 0;
    len_t i, j;
    for (i = 0; curr_diff < len_diff; curr_diff++) {
        for (; i < bin_len && old[i] == new[i]; i++);
        if (i >= bin_len) {
            patch->diff_len = len_to_index(curr_diff);
            patch->heap_len = len_to_index(curr_heap);
            return SUCCESS;
        }

        for (j = i; j < bin_len && old[j] != new[j]; j++);
        if (j - i > len_heap - curr_heap)
            return OUT_OF_HEAP;

        patch->diff_start[curr_diff] = len_to_index(i);
        patch->diff_delta[curr_diff] = len_to_index(j - i);
        blockcpy(&patch->heap[curr_heap], &new[i], j - i);
        curr_heap += j - i;
        i = j;
    }

    return OUT_OF_DIFF;
}
