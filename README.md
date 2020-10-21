# Simple Binary Patch

Simple Binary Patch is a pair of functions that can create a diff between two binary files of the same length and and later patch the original file to become the new one.

## Motivation

I was working on patching firmware on an embedded device. My code would load the original firmware from memory and then attempt to patch it with modified code I had written elsewhere. The binary images were sparsely different, and I needed an easy way to patch the original firmware since copying the whole modified firmware file was impractical. Since this was on an embedded device, I couldn't use the features of other binary diffs since they used compression, `malloc`, and other complications. I made this to be as simple as possible so it would work on pretty much everything.

 ## API

 `long generate_patch(byte* old, byte* new, byte* patch, long size)`
 
 Given two byte buffers, generate the patch required to turn `old` into `new` and save it in `patch`. Only operate on the first `size` bytes of the buffers. Returns the length of the patch data. `patch` has a worst-case size of `5.5 * length of old`, see Caveats as to why.

 `int apply_patch(byte* patch, byte* dest, long patch_length)`

 Given a patch and its length, apply it to `dest`. Returns 0 on success and an error code if something went wrong. There is only one possible error code at the moment:
 
  - `-1`: An invalid opcode was encountered. This could mean the patch was corrupted somehow. Patching stops at an invalid opcode to prevent further damage.

 ## Caveats

This algorithm works best on sparsely different files, or with files that have long chunks of similarity. The worst case for this algorithm is a diff that is 5.5 times the original size. You can do this by diffing two buffers where every other byte is different since at every byte a full instruction would be written (1 byte for opcode, 4 bytes for length, 1 byte for diff half of the time).

 ## Included Tools

 I also included command line tools that allow you to generate and apply diffs to binary files. I also wrote a short example file that I use to test the algorithm, although it's not very thorough.

 Build with:

 ```
 make           # makes generate_patch and apply_patch
 make example   # makes the example code
 make clean     # deletes the binaries
 ```

 ## Future Work
  - Better compression (don't use an int for the length of a segment if a byte will suffice)
  - Automated tests.
  - Checksum checking, so we don't clobber a target with data we know to be corrupted.