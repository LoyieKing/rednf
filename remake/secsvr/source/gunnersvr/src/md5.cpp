/* md5.cpp -- L. Peter Deutsch 公版 MD5 实现（RFC 1321）
   由 DWARF 桩重建：签名/局部变量名与二进制一致（md5_init/append/finish/process）。 */

#include "inc/md5.h"

#include <string.h>

/*
 * 以下实现来自 L. Peter Deutsch 的公共领域 MD5 参考实现
 * (http://sourceforge.net/projects/libmd5-rfc/)，允许自由使用。
 */

/* MD5 变换常量表 */
static const md5_word_t S11 = 7;
static const md5_word_t S12 = 12;
static const md5_word_t S13 = 17;
static const md5_word_t S14 = 22;
static const md5_word_t S21 = 5;
static const md5_word_t S22 = 9;
static const md5_word_t S23 = 14;
static const md5_word_t S24 = 20;
static const md5_word_t S31 = 4;
static const md5_word_t S32 = 11;
static const md5_word_t S33 = 16;
static const md5_word_t S34 = 23;
static const md5_word_t S41 = 6;
static const md5_word_t S42 = 10;
static const md5_word_t S43 = 15;
static const md5_word_t S44 = 21;

static const md5_word_t T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

void md5_process(md5_state_t *pms, const md5_byte_t *data) {
    md5_word_t a, b, c, d;
    md5_word_t t;
    md5_word_t X[16];
    const md5_byte_t *xp;
    int i;

    a = pms->abcd[0];
    b = pms->abcd[1];
    c = pms->abcd[2];
    d = pms->abcd[3];

    for (i = 0, xp = data; i < 16; ++i, xp += 4)
        X[i] = (md5_word_t)(xp[0] | (xp[1] << 8) | (xp[2] << 16) | (xp[3] << 24));

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(f, a, b, c, d, k, s, Ti) \
    t = a + f(b, c, d) + X[k] + Ti; \
    a = ROTATE_LEFT(t, s) + b

    SET(F, a, b, c, d,  0, S11, T[0]);
    SET(F, d, a, b, c,  1, S12, T[1]);
    SET(F, c, d, a, b,  2, S13, T[2]);
    SET(F, b, c, d, a,  3, S14, T[3]);
    SET(F, a, b, c, d,  4, S11, T[4]);
    SET(F, d, a, b, c,  5, S12, T[5]);
    SET(F, c, d, a, b,  6, S13, T[6]);
    SET(F, b, c, d, a,  7, S14, T[7]);
    SET(F, a, b, c, d,  8, S11, T[8]);
    SET(F, d, a, b, c,  9, S12, T[9]);
    SET(F, c, d, a, b, 10, S13, T[10]);
    SET(F, b, c, d, a, 11, S14, T[11]);
    SET(F, a, b, c, d, 12, S11, T[12]);
    SET(F, d, a, b, c, 13, S12, T[13]);
    SET(F, c, d, a, b, 14, S13, T[14]);
    SET(F, b, c, d, a, 15, S14, T[15]);

    SET(G, a, b, c, d,  1, S21, T[16]);
    SET(G, d, a, b, c,  6, S22, T[17]);
    SET(G, c, d, a, b, 11, S23, T[18]);
    SET(G, b, c, d, a,  0, S24, T[19]);
    SET(G, a, b, c, d,  5, S21, T[20]);
    SET(G, d, a, b, c, 10, S22, T[21]);
    SET(G, c, d, a, b, 15, S23, T[22]);
    SET(G, b, c, d, a,  4, S24, T[23]);
    SET(G, a, b, c, d,  9, S21, T[24]);
    SET(G, d, a, b, c, 14, S22, T[25]);
    SET(G, c, d, a, b,  3, S23, T[26]);
    SET(G, b, c, d, a,  8, S24, T[27]);
    SET(G, a, b, c, d, 13, S21, T[28]);
    SET(G, d, a, b, c,  2, S22, T[29]);
    SET(G, c, d, a, b,  7, S23, T[30]);
    SET(G, b, c, d, a, 12, S24, T[31]);

    SET(H, a, b, c, d,  5, S31, T[32]);
    SET(H, d, a, b, c,  8, S32, T[33]);
    SET(H, c, d, a, b, 11, S33, T[34]);
    SET(H, b, c, d, a, 14, S34, T[35]);
    SET(H, a, b, c, d,  1, S31, T[36]);
    SET(H, d, a, b, c,  4, S32, T[37]);
    SET(H, c, d, a, b,  7, S33, T[38]);
    SET(H, b, c, d, a, 10, S34, T[39]);
    SET(H, a, b, c, d, 13, S31, T[40]);
    SET(H, d, a, b, c,  0, S32, T[41]);
    SET(H, c, d, a, b,  3, S33, T[42]);
    SET(H, b, c, d, a,  6, S34, T[43]);
    SET(H, a, b, c, d,  9, S31, T[44]);
    SET(H, d, a, b, c, 12, S32, T[45]);
    SET(H, c, d, a, b, 15, S33, T[46]);
    SET(H, b, c, d, a,  2, S34, T[47]);

    SET(I, a, b, c, d,  0, S41, T[48]);
    SET(I, d, a, b, c,  7, S42, T[49]);
    SET(I, c, d, a, b, 14, S43, T[50]);
    SET(I, b, c, d, a,  5, S44, T[51]);
    SET(I, a, b, c, d, 12, S41, T[52]);
    SET(I, d, a, b, c,  3, S42, T[53]);
    SET(I, c, d, a, b, 10, S43, T[54]);
    SET(I, b, c, d, a,  1, S44, T[55]);
    SET(I, a, b, c, d,  8, S41, T[56]);
    SET(I, d, a, b, c, 15, S42, T[57]);
    SET(I, c, d, a, b,  6, S43, T[58]);
    SET(I, b, c, d, a, 13, S44, T[59]);
    SET(I, a, b, c, d,  4, S41, T[60]);
    SET(I, d, a, b, c, 11, S42, T[61]);
    SET(I, c, d, a, b,  2, S43, T[62]);
    SET(I, b, c, d, a,  9, S44, T[63]);

    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

extern "C" void md5_init(md5_state_t *pms) {
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = 0xefcdab89;
    pms->abcd[2] = 0x98badcfe;
    pms->abcd[3] = 0x10325476;
}

extern "C" void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes) {
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (int)((pms->count[0] >> 3) & 63);
    md5_word_t nbits = (md5_word_t)(nbytes << 3);
    int copy;

    if (nbytes <= 0)
        return;
    pms->count[1] += (md5_word_t)(nbytes >> 29);
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
        pms->count[1]++;
    if (offset > 0) {
        copy = 64 - offset;
        if (copy > left)
            copy = left;
        memcpy(pms->buf + offset, p, (size_t)copy);
        if (offset + copy < 64)
            return;
        p += copy;
        left -= copy;
        md5_process(pms, pms->buf);
    }
    for (; left >= 64; p += 64, left -= 64)
        md5_process(pms, p);
    if (left > 0)
        memcpy(pms->buf, p, (size_t)left);
}

extern "C" void md5_finish(md5_state_t *pms, md5_byte_t *digest) {
    md5_byte_t data[8];
    int i;
    static const md5_byte_t pad[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    for (i = 0; i < 8; ++i)
        data[i] = (md5_byte_t)((pms->count[i >> 2] >> (8 * (i & 3))) & 255);
    md5_append(pms, pad, (int)((55 - (pms->count[0] >> 3)) & 63) + 1);
    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
        digest[i] = (md5_byte_t)((pms->abcd[i >> 2] >> (8 * (i & 3))) & 255);
}
