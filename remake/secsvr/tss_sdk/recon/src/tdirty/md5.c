/* Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/md5.c).
   Classic RFC1321 MD5.  Encode/MD5_memcpy/MD5_memset/MD5Transform in ORIG use
   regparm(3) calling convention (verified: MD5Transform reads state in eax and
   block in edx, Encode reads output in eax / input edx / len ecx); the exported
   entry points use normal cdecl. */
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int UINT4;

typedef struct {
    UINT4 state[4];
    UINT4 count[2];
    unsigned char buffer[64];
} MD5_CTX;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

void __attribute__((regparm(3)))
Encode(unsigned char *output, UINT4 *input, unsigned int len) {
    unsigned int i;
    unsigned int j;
    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void __attribute__((regparm(3)))
MD5_memcpy(unsigned char *output, unsigned char *input, unsigned int len) {
    for (unsigned int i = 0; i < len; i++)
        output[i] = input[i];
}

void __attribute__((regparm(3)))
MD5_memset(unsigned char *output, int value, unsigned int len) {
    for (unsigned int i = 0; i < len; i++)
        ((char *)output)[i] = (char)value;
}

static UINT4 GET_LE32(const unsigned char *p) {
    return ((UINT4)p[0]) | (((UINT4)p[1]) << 8) | (((UINT4)p[2]) << 16) |
           (((UINT4)p[3]) << 24);
}

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define FF(a, b, c, d, x, s, ac)                                               \
    {                                                                          \
        (a) += F((b), (c), (d)) + (x) + (UINT4)(ac);                           \
        (a) = ROTATE_LEFT((a), (s));                                           \
        (a) += (b);                                                            \
    }
#define GG(a, b, c, d, x, s, ac)                                               \
    {                                                                          \
        (a) += G((b), (c), (d)) + (x) + (UINT4)(ac);                           \
        (a) = ROTATE_LEFT((a), (s));                                           \
        (a) += (b);                                                            \
    }
#define HH(a, b, c, d, x, s, ac)                                               \
    {                                                                          \
        (a) += H((b), (c), (d)) + (x) + (UINT4)(ac);                           \
        (a) = ROTATE_LEFT((a), (s));                                           \
        (a) += (b);                                                            \
    }
#define II(a, b, c, d, x, s, ac)                                               \
    {                                                                          \
        (a) += I((b), (c), (d)) + (x) + (UINT4)(ac);                           \
        (a) = ROTATE_LEFT((a), (s));                                           \
        (a) += (b);                                                            \
    }

void __attribute__((regparm(3)))
MD5Transform(UINT4 state[4], unsigned char block[64]) {
    UINT4 a = state[0], b = state[1], c = state[2], d = state[3];
    UINT4 x[16];
    unsigned int i;

    for (i = 0; i < 16; i++)
        x[i] = GET_LE32(block + i * 4);

    FF(a, b, c, d, x[0], S11, 0xd76aa478);
    FF(d, a, b, c, x[1], S12, 0xe8c7b756);
    FF(c, d, a, b, x[2], S13, 0x242070db);
    FF(b, c, d, a, x[3], S14, 0xc1bdceee);
    FF(a, b, c, d, x[4], S11, 0xf57c0faf);
    FF(d, a, b, c, x[5], S12, 0x4787c62a);
    FF(c, d, a, b, x[6], S13, 0xa8304613);
    FF(b, c, d, a, x[7], S14, 0xfd469501);
    FF(a, b, c, d, x[8], S11, 0x698098d8);
    FF(d, a, b, c, x[9], S12, 0x8b44f7af);
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    FF(b, c, d, a, x[11], S14, 0x895cd7be);
    FF(a, b, c, d, x[12], S11, 0x6b901122);
    FF(d, a, b, c, x[13], S12, 0xfd987193);
    FF(c, d, a, b, x[14], S13, 0xa679438e);
    FF(b, c, d, a, x[15], S14, 0x49b40821);

    GG(a, b, c, d, x[1], S21, 0xf61e2562);
    GG(d, a, b, c, x[6], S22, 0xc040b340);
    GG(c, d, a, b, x[11], S23, 0x265e5a51);
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
    GG(a, b, c, d, x[5], S21, 0xd62f105d);
    GG(d, a, b, c, x[10], S22, 0x02441453);
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
    GG(a, b, c, d, x[9], S21, 0x21e1cde6);
    GG(d, a, b, c, x[14], S22, 0xc33707d6);
    GG(c, d, a, b, x[3], S23, 0xf4d50d87);
    GG(b, c, d, a, x[8], S24, 0x455a14ed);
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
    GG(c, d, a, b, x[7], S23, 0x676f02d9);
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

    HH(a, b, c, d, x[5], S31, 0xfffa3942);
    HH(d, a, b, c, x[8], S32, 0x8771f681);
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    HH(b, c, d, a, x[14], S34, 0xfde5380c);
    HH(a, b, c, d, x[1], S31, 0xa4beea44);
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    HH(d, a, b, c, x[0], S32, 0xeaa127fa);
    HH(c, d, a, b, x[3], S33, 0xd4ef3085);
    HH(b, c, d, a, x[6], S34, 0x04881d05);
    HH(a, b, c, d, x[9], S31, 0xd9d4d039);
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, x[2], S34, 0xc4ac5665);

    II(a, b, c, d, x[0], S41, 0xf4292244);
    II(d, a, b, c, x[7], S42, 0x432aff97);
    II(c, d, a, b, x[14], S43, 0xab9423a7);
    II(b, c, d, a, x[5], S44, 0xfc93a039);
    II(a, b, c, d, x[12], S41, 0x655b59c3);
    II(d, a, b, c, x[3], S42, 0x8f0ccc92);
    II(c, d, a, b, x[10], S43, 0xffeff47d);
    II(b, c, d, a, x[1], S44, 0x85845dd1);
    II(a, b, c, d, x[8], S41, 0x6fa87e4f);
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, x[6], S43, 0xa3014314);
    II(b, c, d, a, x[13], S44, 0x4e0811a1);
    II(a, b, c, d, x[4], S41, 0xf7537e82);
    II(d, a, b, c, x[11], S42, 0xbd3af235);
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
    II(b, c, d, a, x[9], S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    MD5_memset((unsigned char *)x, 0, sizeof(x));
}

void MD5Init(MD5_CTX *context) {
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, unsigned char *input,
               unsigned int inputLen) {
    unsigned int i;
    unsigned int index;
    unsigned int partLen;

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);

    if ((context->count[0] += ((UINT4)inputLen << 3)) <
        ((UINT4)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((UINT4)inputLen >> 29);

    partLen = 64 - index;

    if (inputLen >= partLen) {
        MD5_memcpy((unsigned char *)&context->buffer[index], input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context->state, &input[i]);

        index = 0;
    } else {
        i = 0;
    }

    MD5_memcpy((unsigned char *)&context->buffer[index], &input[i],
               inputLen - i);
}

/* ORIG md5.c CU 把 RFC 1321 填充块放在文件作用域（本地符号 PADDING，64 字节）；
   放在 MD5Final 内会变成 PADDING.<n> 形式的函数局部静态符号。 */
static const unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void MD5Final(unsigned char digest[16], MD5_CTX *context) {
    unsigned char bits[8];
    unsigned int index;
    unsigned int padLen;

    Encode(bits, context->count, 8);

    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, (unsigned char *)PADDING, padLen);

    MD5Update(context, bits, 8);

    Encode(digest, context->state, 16);

    MD5_memset((unsigned char *)context, 0, sizeof(*context));
}

void Md5HashBuffer(char *pszDigest, char *pszData, unsigned int uiLen) {
    MD5_CTX context;

    MD5Init(&context);
    MD5Update(&context, (unsigned char *)pszData, uiLen);
    MD5Final((unsigned char *)pszDigest, &context);
}

void Md5HexString(unsigned char *pszDigest, char *pszOut) {
    int i;
    for (i = 1; i < 17; i++)
        sprintf(pszOut + (i - 1) * 2, "%02x", pszDigest[i - 1]);
}

void Md5File(char *pszDigest, char *pszFile) {
    FILE *fp;
    MD5_CTX context;
    unsigned char buffer[1024];
    size_t uiRead;

    fp = fopen(pszFile, "rb");
    if (fp == 0) {
        printf("%s can't be opened.\n", pszFile);
        return;
    }

    MD5Init(&context);
    while ((uiRead = fread(buffer, 1, 1024, fp)) != 0)
        MD5Update(&context, buffer, (unsigned int)uiRead);
    MD5Final((unsigned char *)pszDigest, &context);
    fclose(fp);
}

#ifdef __cplusplus
}
#endif
