// base64 helpers, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_base64.cpp
// Address range 0x00198300-0x00198643.
//
// The functions live in the global namespace in the ORIG (mangled names
// _Z14get_encode_lenj, _Z14get_decode_lenj, _Z13base64_encodePKhjPcPj,
// _Z13base64_decodePKcjPhPj).
#ifndef TSS_RECON_ZEN_BASE64_H_
#define TSS_RECON_ZEN_BASE64_H_

// Encoded length including the trailing NUL byte (ORIG adds +1).
unsigned int get_encode_len(unsigned int src_len);

// Decoded length capacity including the trailing NUL byte (ORIG adds +1).
unsigned int get_decode_len(unsigned int src_len);

// Returns 0 on success, -1 when *dst_len is too small (compared against
// get_encode_len(src_len)) or src_len == 0.
int base64_encode(const unsigned char *src, unsigned int src_len,
                  char *dst, unsigned int *dst_len);

// Returns 0 on success, -1 when *dst_len is too small (compared against
// get_decode_len(src_len)) or src_len == 0.
int base64_decode(const char *src, unsigned int src_len,
                  unsigned char *dst, unsigned int *dst_len);

#endif // TSS_RECON_ZEN_BASE64_H_
