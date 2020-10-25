#include "bin_patch.h"


static void blockcpy(uint8_t *dest, uint8_t *src, len_t len)
{
	for (; len--; *dest++ = *src++);
}


enum bp_ret_code gen_patch(uint8_t *old, uint8_t *new, struct binary_patch *patch)
{
	len_t curr_diff = 0;
	len_t curr_heap = 0;
	len_t i         = 0;
	for (; curr_diff < patch->diff_len; curr_diff++) {
		for (; i < patch->bin_len && old[i] == new[i]; i++);
		if (i >= patch->bin_len) {
			patch->diff_len = curr_diff;
			patch->heap_len = curr_heap;
			return SUCCESS;
		}
		patch->diff_start[curr_diff] = i;

		for (; i < patch->bin_len && old[i] != new[i]; i++);
		if (i - patch->diff_start[curr_diff] > patch->heap_len - curr_heap)
			return OUT_OF_HEAP;
		patch->diff_stop[curr_diff] = i;

		blockcpy(&patch->heap[curr_heap],
		         &new[patch->diff_start[curr_diff]],
		         patch->diff_stop[curr_diff] - patch->diff_start[curr_diff]);
		curr_heap += patch->diff_stop[curr_diff] - patch->diff_start[curr_diff];
	}

	return OUT_OF_DIFF;
}


enum bp_ret_code apply_patch(uint8_t *dest, struct binary_patch *patch)
{
	return SUCCESS;
}
