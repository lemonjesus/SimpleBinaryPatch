# Simple Binary Patch

Simple Binary Patch is a pair of functions that can create a diff between two binary files of the same length and and later patch the original file to become the new one.

## Motivation

I was working on patching firmware on an embedded device. My code would load the original firmware from memory and then attempt to patch it with modified code I had written elsewhere. The binary images were sparsely different, and I needed an easy way to patch the original firmware since copying the whole modified firmware file was impractical. Since this was on an embedded device, I couldn't use the features of other binary diffs since they used compression, `malloc`, and other complications. I made this to be as simple as possible so it would work on pretty much everything.

## Requirements
Simple Binary Patch requires fixed width data types. It includes `stdint.h`, but if that's not available on your platform you'll need to typedef your own.

## API

Please see [`bin_patch.h`](./bin_patch.h) for the documentation.  It is not here to reduce redundancy.

## Caveats

This algorithm works best on sparsely different files, or with files that have long chunks of similarity. There is overhead of 2x the size of what `len_t` is configured as.

## Included Tools

I also included command line tools that allow you to generate and apply diffs to binary files. I also wrote a short example file that I use to test the algorithm, although it's not very thorough.

Build with:

```
make           # makes generate_patch and apply_patch
make example   # makes the example code
make clean     # deletes the binaries
```

## Future Work
- Better compression
- Automated tests.
- Checksum checking, so we don't clobber a target with data we know to be corrupted.
