#include <stdint.h>

/**
 * generate_patch
 * Given two byte buffers, generate the patch required to turn `old` into `new` and save it in `patch`. 
 * Only operate on the first `size` bytes of the buffers. Returns the length of the patch data. `patch` 
 * has a worst-case size of `5.5 * length of old`, see Caveats in the README as to why.
 **/
uint32_t generate_patch(uint8_t* old, uint8_t* new, uint8_t* patch, uint32_t size);

/**
 * apply_patch
 * Given a patch and its length, apply it to `dest`. Returns 0 on success and an error code if something 
 * went wrong. There is only one possible error code at the moment:
 *  - -1 - An invalid opcode was encountered. This could mean the patch was corrupted somehow. Patching 
 *         stops at an invalid opcode to prevent further damage.
 **/
uint32_t apply_patch(uint8_t* patch, uint8_t* dest, uint32_t patch_length);
