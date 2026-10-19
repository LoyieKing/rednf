// hex_to_str -- zen_string_util.h:481 of the TSS SDK build
// (libtss_sdk.so.1.0.394).
//
// ORIG symbol: _Z10hex_to_strPKhjPcj, weak function at 0x000a2be0, 0x82 bytes,
// plus its function-local static _ZZ10hex_to_strPKhjPcjE3tag at 0x00297960
// (4 bytes, weak object, .data -- a pointer, statically initialised to the
// string "0123456789ABCDEF" in .rodata).
//
// The weak binding is the one a header-defined `inline` function gets: every CU
// that includes zen_string_util.h and calls hex_to_str carries its own copy, and
// the linker keeps one.  The body below is therefore written as an `inline`
// definition (which is what makes both the function and its function-local
// static vague-linkage symbols, W and V, exactly as in the ORIG) with
// __attribute__((used)) so that the vendor toolchain still emits the out-of-line
// copy even though this TU never calls it.
//
//   edx = buf_len; eax = 2*buf_len + 1; edx = -1
//   cmp eax, dest_len ; ja -> return -1          (unsigned, capacity check)
//   esi = dest; if (buf_len == 0) goto term
//   loop:  ecx = buf[i]; hi = c >> 4; lo = c & 0xf
//          dest[2i] = tag[hi]; dest[2i+1] = tag[lo]; ++i; i != buf_len -> loop
//   term:  *esi = 0; return 0
//
// tag is re-loaded from its slot on every iteration in the ORIG (the store
// through `dest` may alias the pointer), which is why the source below indexes
// the static directly rather than caching it in a local.
//
// Declaration: the generated DWARF stub of zen_string_util.h carries
//   extern int hex_to_str(const unsigned char *buf, size_t buf_len,
//                         char *dest, size_t dest_len);   // line 481
// which reproduces the ORIG mangling _Z10hex_to_strPKhjPcj (size_t is unsigned
// int in the 32-bit ABI); the stub is not included here because its class
// skeletons (zen_char_traits<char> etc.) are not part of this emission.

#include <stddef.h>

inline __attribute__((used)) int hex_to_str(const unsigned char *buf,
                                            size_t buf_len, char *dest,
                                            size_t dest_len) {
    static const char *tag = "0123456789ABCDEF";

    if (buf_len * 2 + 1 > dest_len) {
        return -1;
    }
    for (size_t i = 0; i < buf_len; ++i) {
        dest[i * 2] = tag[buf[i] >> 4];
        dest[i * 2 + 1] = tag[buf[i] & 0x0f];
    }
    dest[buf_len * 2] = '\0';
    return 0;
}
