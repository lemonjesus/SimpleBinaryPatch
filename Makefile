CC=gcc

all: generate_patch apply_patch

generate_patch: generate_patch.c bin_patch.c bin_patch.h
	${CC} -o generate_patch generate_patch.c bin_patch.c

apply_patch: apply_patch.c bin_patch.c bin_patch.h
	${CC} -o apply_patch apply_patch.c bin_patch.c

example: example.c bin_patch.c bin_patch.h
	${CC} -std=c99 -o example example.c bin_patch.c

clean:
	-@rm -rf example generate_patch apply_patch 2>/dev/null || true
