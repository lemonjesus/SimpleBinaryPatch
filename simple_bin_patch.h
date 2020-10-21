typedef unsigned char byte;

long generate_patch(byte* old, byte* new, byte* patch, long size);
int apply_patch(byte* patch, byte* dest, long patch_length);

// private methods
int _read_int_as_bytes(byte* src);
void _write_int_as_bytes(int x, byte* dest);