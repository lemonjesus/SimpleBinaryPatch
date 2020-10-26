#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bin_patch.h"

int main(int argc, char** argv) {
	if (argc < 3) {
		printf("usage: %s [patch] [old_file] [new_file]\n\n", argv[0]);
		printf("applies a patch to old_file and writes it to new_file.\n");
		printf("use generate_patch to make a compatible patch file.\n\n");
		return 1;
	}

	FILE *patch = fopen(argv[1], "rb");
	FILE *old   = fopen(argv[2], "rb");
	if (!patch || !old) {
		printf("opening patch or old file failed\n");
		return 2;
	}

	fseek(patch, 0L, SEEK_END);
	fseek(old,   0L, SEEK_END);
	uint32_t patch_len = ftell(patch);
	uint32_t old_len   = ftell(old);
	rewind(patch);
	rewind(old);

	uint8_t *patch_buf = (uint8_t *) malloc(patch_len);
	uint8_t *old_buf   = (uint8_t *) malloc(old_len);

	if (!patch_buf || !old_buf) {
		printf("patch or old buffer could not be allocated\n");
		fclose(patch);
		fclose(old);
		return 3;
	}

	fread(patch_buf, sizeof(uint8_t), patch_len, patch);
	fread(old_buf,   sizeof(uint8_t), old_len,   old);
	fclose(patch);
	fclose(old);

	len_t diff_len = *((len_t *) patch_buf);
	struct binary_patch bp = {
		.diff_len   = diff_len,
		.diff_start = ((len_t *) patch_buf + 1),
		.diff_delta = ((len_t *) patch_buf + diff_len + 1),
		.heap       = (uint8_t *) ((len_t *) patch_buf + diff_len * 2 + 1)
	};
	apply_patch(old_buf, &bp);

	FILE *new = fopen(argv[3], "wb");
	if (!new) {
		printf("new file could not be opened\n");
		free(patch_buf);
		free(old_buf);
		return 4;
	}
	fwrite(old_buf, sizeof(uint8_t), old_len, new);
	fclose(new);

	free(patch_buf);
	free(old_buf);

	return 0;
}
