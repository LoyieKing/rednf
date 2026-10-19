// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_base64.cpp
// Address range 0x00198300-0x00198643.
//
// .rodata layout of the ORIG (nm -S on the CU-local symbols):
//   s_decode_index      0x240380  128 B  inverse table (input bytes 0x00..0x7f)
//   s_encode_index      0x240400   64 B  encoder alphabet
//   (following 64 B)    0x240440        literals the ORIG decoder reads past
//                                       the 128-byte table for input >= 0x80
// Its decode loop uses movzbl on the input byte and indexes the 128-byte
// table, so bytes >= 0x80 read the following .rodata.  b64_decode_value()
// below reproduces those bytes (the previous 256-byte recon table held the
// identical byte sequence 0x240380..0x2404ff); behaviour is unchanged for
// every input byte value, without depending on object order in the section.
//
// Padding behaviour (matches ORIG):
//   src_len % 3 == 1 -> two '=' ; src_len % 3 == 2 -> one '='
//   the pad computation reads src[src_len] (the NUL of a C string); the
//   original does the same (phase formula uses p[-1] after p advanced).
#include "src/commlib/zenlib/zen_base64.h"

// ORIG zen_base64.cpp static-init block: iostream only, no statistics table.
#include "src/ioinit_block.h"

// ORIG .rodata 0x00240380: inverse table (indices 0x00..0x7f).
const unsigned char s_decode_index[128] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x3e, 0x40, 0x40, 0x40, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x40, 0x40, 0x40, 0x40, 0x40
};

// ORIG .rodata 0x00240400: encoder alphabet.
const unsigned char s_encode_index[64] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2b, 0x2f
};

// ORIG .rodata 0x00240440: the 64 bytes the ORIG decoder reads past the
// 128-byte table for input bytes 0xc0..0xff (the literals "RECV_PIPE",
// "SEND_PIPE", "--", "POSIXLY_CORRECT", "::", "[zenlib]" with their padding).
static const unsigned char s_decode_index_tail[64] = {
    0x52, 0x45, 0x43, 0x56, 0x5f, 0x50, 0x49, 0x50, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x53, 0x45, 0x4e, 0x44, 0x5f, 0x50, 0x49, 0x50, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2d, 0x2d, 0x00, 0x50, 0x4f, 0x53, 0x49, 0x58, 0x4c, 0x59, 0x5f, 0x43, 0x4f, 0x52, 0x52, 0x45,
    0x43, 0x54, 0x00, 0x3a, 0x3a, 0x00, 0x00, 0x00, 0x5b, 0x7a, 0x65, 0x6e, 0x6c, 0x69, 0x62, 0x5d
};

// The ORIG decode loop does movzbl on the input byte and indexes the 128-byte
// table, so bytes >= 0x80 read the following .rodata.  Same value, defined
// behaviour, no dependence on object order in the section.
static inline unsigned char b64_decode_value(unsigned char c) {
    if (c < 0x80) return s_decode_index[c];
    if (c < 0xC0) return s_encode_index[c - 0x80];
    return s_decode_index_tail[c - 0xC0];
}

namespace {

const unsigned char BASE64_PAD = 0x3d;

} // namespace

unsigned int get_encode_len(unsigned int src_len) {
    unsigned int remainder = src_len % 3;
    unsigned int padding = (remainder == 0) ? 0 : 3 - remainder;
    return ((src_len + padding) / 3) * 4 + 1;
}

unsigned int get_decode_len(unsigned int src_len) {
    return (src_len / 4) * 3 + 1;
}

int base64_encode(const unsigned char *src, unsigned int src_len,
                  char *dst, unsigned int *dst_len) {
    if (*dst_len < get_encode_len(src_len)) {
        return -1;
    }
    if (src_len == 0) {
        return -1;
    }

    const unsigned char *p = src;
    char *out = dst;
    unsigned int phase = 0;
    unsigned int consumed = 0;
    ++p;
    for (;;) {
        if (phase == 1) {
            *out = (char)s_encode_index[((p[-2] & 0x03) << 4) |
                                        (p[-1] >> 4)];
        } else if (phase == 0) {
            *out = (char)s_encode_index[p[-1] >> 2];
        } else {
            *out = (char)s_encode_index[((p[-2] & 0x0f) << 2) |
                                        (p[-1] >> 6)];
            ++out;
            *out = (char)s_encode_index[p[-1] & 0x3f];
            phase = 0;
            ++consumed;
            ++out;
            ++p;
            if (consumed == src_len) {
                break;
            }
            continue;
        }
        ++phase;
        ++consumed;
        ++out;
        ++p;
        if (consumed == src_len) {
            break;
        }
    }

    if (phase == 1) {
        out[0] = (char)s_encode_index[((p[-2] & 0x03) << 4) |
                                      (p[-1] >> 4)];
        out[1] = (char)BASE64_PAD;
        out[2] = (char)BASE64_PAD;
        out += 3;
    } else if (phase == 2) {
        out[0] = (char)s_encode_index[((p[-2] & 0x0f) << 2) |
                                      (p[-1] >> 6)];
        out[1] = (char)BASE64_PAD;
        out += 2;
    }

    *dst_len = (unsigned int)(out - dst);
    return 0;
}

int base64_decode(const char *src, unsigned int src_len,
                  unsigned char *dst, unsigned int *dst_len) {
    if (*dst_len < get_decode_len(src_len)) {
        return -1;
    }
    if (src_len == 0) {
        return -1;
    }

    const unsigned char *p = (const unsigned char *)src;
    unsigned char *out = dst;
    unsigned int groups = src_len >> 2;
    unsigned int full_groups = groups - 1;
    unsigned int i;
    for (i = 0; i < full_groups; ++i) {
        out[0] = (unsigned char)((b64_decode_value(p[0]) << 2) |
                                 ((b64_decode_value(p[1]) & 0x30) >> 4));
        out[1] = (unsigned char)((b64_decode_value(p[1]) << 4) |
                                 ((b64_decode_value(p[2]) & 0x3c) >> 2));
        out[2] = (unsigned char)((b64_decode_value(p[2]) << 6) |
                                 (b64_decode_value(p[3]) & 0x3f));
        out += 3;
        p += 4;
    }

    out[0] = (unsigned char)((b64_decode_value(p[0]) << 2) |
                             ((b64_decode_value(p[1]) & 0x30) >> 4));
    unsigned char *next = out + 1;
    if (p[2] != BASE64_PAD) {
        out[1] = (unsigned char)((b64_decode_value(p[1]) << 4) |
                                 ((b64_decode_value(p[2]) & 0x3c) >> 2));
        next = out + 2;
        if (p[3] != BASE64_PAD) {
            out[2] = (unsigned char)((b64_decode_value(p[2]) << 6) |
                                     (b64_decode_value(p[3]) & 0x3f));
            next = out + 3;
        }
    }
    *dst_len = (unsigned int)(next - dst);
    return 0;
}
