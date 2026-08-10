/* md4.cpp -- RFC 1320 MD4（公版参考实现）的 tencrypt_new 类封装
   由 DWARF 桩重建：CMD4 成员签名/局部变量与 RFC 参考一致。 */

#include "inc/md4.h"

#include <string.h>

/* RFC 1320 附录常量（公版） */
#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15

static const unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s) { \
    (a) += F((b), (c), (d)) + (x); \
    (a) = ROTATE_LEFT((a), (s)); \
}
#define GG(a, b, c, d, x, s) { \
    (a) += G((b), (c), (d)) + (x) + (unsigned int)0x5a827999; \
    (a) = ROTATE_LEFT((a), (s)); \
}
#define HH(a, b, c, d, x, s) { \
    (a) += H((b), (c), (d)) + (x) + (unsigned int)0x6ed9eba1; \
    (a) = ROTATE_LEFT((a), (s)); \
}

void CMD4::Encode(unsigned char *output, unsigned int *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j]     = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void CMD4::Decode(unsigned int *output, unsigned char *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = (unsigned int)input[j] |
                    ((unsigned int)input[j + 1] << 8) |
                    ((unsigned int)input[j + 2] << 16) |
                    ((unsigned int)input[j + 3] << 24);
}

void CMD4::MD4Transform(unsigned int *state, unsigned char *block) {
    unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
    unsigned int x[16];

    Decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d, x[0],  S11);
    FF(d, a, b, c, x[1],  S12);
    FF(c, d, a, b, x[2],  S13);
    FF(b, c, d, a, x[3],  S14);
    FF(a, b, c, d, x[4],  S11);
    FF(d, a, b, c, x[5],  S12);
    FF(c, d, a, b, x[6],  S13);
    FF(b, c, d, a, x[7],  S14);
    FF(a, b, c, d, x[8],  S11);
    FF(d, a, b, c, x[9],  S12);
    FF(c, d, a, b, x[10], S13);
    FF(b, c, d, a, x[11], S14);
    FF(a, b, c, d, x[12], S11);
    FF(d, a, b, c, x[13], S12);
    FF(c, d, a, b, x[14], S13);
    FF(b, c, d, a, x[15], S14);

    /* Round 2 */
    GG(a, b, c, d, x[0],  S21);
    GG(d, a, b, c, x[4],  S22);
    GG(c, d, a, b, x[8],  S23);
    GG(b, c, d, a, x[12], S24);
    GG(a, b, c, d, x[1],  S21);
    GG(d, a, b, c, x[5],  S22);
    GG(c, d, a, b, x[9],  S23);
    GG(b, c, d, a, x[13], S24);
    GG(a, b, c, d, x[2],  S21);
    GG(d, a, b, c, x[6],  S22);
    GG(c, d, a, b, x[10], S23);
    GG(b, c, d, a, x[14], S24);
    GG(a, b, c, d, x[3],  S21);
    GG(d, a, b, c, x[7],  S22);
    GG(c, d, a, b, x[11], S23);
    GG(b, c, d, a, x[15], S24);

    /* Round 3 */
    HH(a, b, c, d, x[0],  S31);
    HH(d, a, b, c, x[8],  S32);
    HH(c, d, a, b, x[4],  S33);
    HH(b, c, d, a, x[12], S34);
    HH(a, b, c, d, x[2],  S31);
    HH(d, a, b, c, x[10], S32);
    HH(c, d, a, b, x[6],  S33);
    HH(b, c, d, a, x[14], S34);
    HH(a, b, c, d, x[1],  S31);
    HH(d, a, b, c, x[9],  S32);
    HH(c, d, a, b, x[5],  S33);
    HH(b, c, d, a, x[13], S34);
    HH(a, b, c, d, x[3],  S31);
    HH(d, a, b, c, x[11], S32);
    HH(c, d, a, b, x[7],  S33);
    HH(b, c, d, a, x[15], S34);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    memset((void *)x, 0, sizeof(x));
}

void CMD4::MD4Init(MD4_CTX *context) {
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void CMD4::MD4Update(MD4_CTX *context, unsigned char *input, unsigned int inputLen) {
    unsigned int i, index, partLen;

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    if ((context->count[0] += ((unsigned int)inputLen << 3)) <
        ((unsigned int)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((unsigned int)inputLen >> 29);

    partLen = 64 - index;
    if (inputLen >= partLen) {
        memcpy(&context->buffer[index], input, partLen);
        MD4Transform(context->state, context->buffer);
        for (i = partLen; i + 63 < inputLen; i += 64)
            MD4Transform(context->state, &input[i]);
        index = 0;
    } else
        i = 0;

    memcpy(&context->buffer[index], &input[i], inputLen - i);
}

void CMD4::MD4Final(unsigned char *digest, MD4_CTX *context) {
    unsigned char bits[8];
    unsigned int index, padLen;

    Encode(bits, context->count, 8);
    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD4Update(context, (unsigned char *)PADDING, padLen);
    MD4Update(context, bits, 8);
    Encode(digest, context->state, 16);

    memset((void *)context, 0, sizeof(*context));
}

void CMD4::Init() {
    unsigned char _padding[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    (void)_padding;
}

void CMD4::UnInit() {
}

CMD4::CMD4() {
}

CMD4::~CMD4() {
}
