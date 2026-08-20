// df_game_r Game/ MD5 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 算法正确即可，不逐指令对齐。
#include "md5.h"
#include <cstring>

static unsigned int md5_rotl(unsigned int x, int c)
{
    return (x << c) | (x >> (32 - c));
}

void md5_starts(md5_context* ctx)
{
    md5_ctx* c = (md5_ctx*)ctx;
    c->total[0] = 0;
    c->total[1] = 0;
    c->state[0] = 0x67452301;
    c->state[1] = 0xefcdab89;
    c->state[2] = 0x98badcfe;
    c->state[3] = 0x10325476;
}

void md5_update(md5_context* ctx, unsigned char* data, int len)
{
    md5_ctx* c = (md5_ctx*)ctx;
    unsigned int used = (c->total[0] >> 3) & 0x3f;
    c->total[0] += (unsigned int)len << 3;
    if (c->total[0] < ((unsigned int)len << 3))
    {
        ++c->total[1];
    }
    c->total[1] += (unsigned int)len >> 29;
    if (used != 0)
    {
        unsigned int fill = 64 - used;
        if (len < (int)fill)
        {
            memcpy(c->buffer + used, data, len);
            return;
        }
        memcpy(c->buffer + used, data, fill);
        data += fill;
        len -= (int)fill;
        used = 0;
        md5_ctx* p = (md5_ctx*)ctx;
        unsigned int* x = (unsigned int*)p->buffer;
        unsigned int a = p->state[0], b = p->state[1];
        unsigned int cc = p->state[2], d = p->state[3];
#define MD5_F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~(z)))
#define MD5_STEP(f, a, b, c, d, x, t, s) \
    (a) += f((b), (c), (d)) + (x) + (t); \
    (a) = md5_rotl((a), (s));             \
    (a) += (b);
        MD5_STEP(MD5_F, a, b, cc, d, x[0], 0xd76aa478, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[1], 0xe8c7b756, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[2], 0x242070db, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[3], 0xc1bdceee, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[4], 0xf57c0faf, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[5], 0x4787c62a, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[6], 0xa8304613, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[7], 0xfd469501, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[8], 0x698098d8, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[9], 0x8b44f7af, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[10], 0xffff5bb1, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[11], 0x895cd7be, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[12], 0x6b901122, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[13], 0xfd987193, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[14], 0xa679438e, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[15], 0x49b40821, 22);
        MD5_STEP(MD5_G, a, b, cc, d, x[1], 0xf61e2562, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[6], 0xc040b340, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[11], 0x265e5a51, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[0], 0xe9b6c7aa, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[5], 0xd62f105d, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[10], 0x02441453, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[15], 0xd8a1e681, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[4], 0xe7d3fbc8, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[9], 0x21e1cde6, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[14], 0xc33707d6, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[3], 0xf4d50d87, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[8], 0x455a14ed, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[13], 0xa9e3e905, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[2], 0xfcefa3f8, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[7], 0x676f02d9, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[12], 0x8d2a4c8a, 20);
        MD5_STEP(MD5_H, a, b, cc, d, x[5], 0xfffa3942, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[8], 0x8771f681, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[11], 0x6d9d6122, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[14], 0xfde5380c, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[1], 0xa4beea44, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[4], 0x4bdecfa9, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[7], 0xf6bb4b60, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[10], 0xbebfbc70, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[13], 0x289b7ec6, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[0], 0xeaa127fa, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[3], 0xd4ef3085, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[6], 0x04881d05, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[9], 0xd9d4d039, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[12], 0xe6db99e5, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[15], 0x1fa27cf8, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[2], 0xc4ac5665, 23);
        MD5_STEP(MD5_I, a, b, cc, d, x[0], 0xf4292244, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[7], 0x432aff97, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[14], 0xab9423a7, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[5], 0xfc93a039, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[12], 0x655b59c3, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[3], 0x8f0ccc92, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[10], 0xffeff47d, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[1], 0x85845dd1, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[8], 0x6fa87e4f, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[15], 0xfe2ce6e0, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[6], 0xa3014314, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[13], 0x4e0811a1, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[4], 0xf7537e82, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[11], 0xbd3af235, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[2], 0x2ad7d2bb, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[9], 0xeb86d391, 21);
#undef MD5_F
#undef MD5_G
#undef MD5_H
#undef MD5_I
#undef MD5_STEP
        p->state[0] += a;
        p->state[1] += b;
        p->state[2] += cc;
        p->state[3] += d;
    }
    while (len >= 64)
    {
        memcpy(c->buffer, data, 64);
        md5_ctx* p = (md5_ctx*)ctx;
        unsigned int* x = (unsigned int*)p->buffer;
        unsigned int a = p->state[0], b = p->state[1];
        unsigned int cc = p->state[2], d = p->state[3];
#define MD5_F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~(z)))
#define MD5_STEP(f, a, b, c, d, x, t, s) \
    (a) += f((b), (c), (d)) + (x) + (t); \
    (a) = md5_rotl((a), (s));             \
    (a) += (b);
        MD5_STEP(MD5_F, a, b, cc, d, x[0], 0xd76aa478, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[1], 0xe8c7b756, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[2], 0x242070db, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[3], 0xc1bdceee, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[4], 0xf57c0faf, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[5], 0x4787c62a, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[6], 0xa8304613, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[7], 0xfd469501, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[8], 0x698098d8, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[9], 0x8b44f7af, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[10], 0xffff5bb1, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[11], 0x895cd7be, 22);
        MD5_STEP(MD5_F, a, b, cc, d, x[12], 0x6b901122, 7);
        MD5_STEP(MD5_F, d, a, b, cc, x[13], 0xfd987193, 12);
        MD5_STEP(MD5_F, cc, d, a, b, x[14], 0xa679438e, 17);
        MD5_STEP(MD5_F, b, cc, d, a, x[15], 0x49b40821, 22);
        MD5_STEP(MD5_G, a, b, cc, d, x[1], 0xf61e2562, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[6], 0xc040b340, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[11], 0x265e5a51, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[0], 0xe9b6c7aa, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[5], 0xd62f105d, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[10], 0x02441453, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[15], 0xd8a1e681, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[4], 0xe7d3fbc8, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[9], 0x21e1cde6, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[14], 0xc33707d6, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[3], 0xf4d50d87, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[8], 0x455a14ed, 20);
        MD5_STEP(MD5_G, a, b, cc, d, x[13], 0xa9e3e905, 5);
        MD5_STEP(MD5_G, d, a, b, cc, x[2], 0xfcefa3f8, 9);
        MD5_STEP(MD5_G, cc, d, a, b, x[7], 0x676f02d9, 14);
        MD5_STEP(MD5_G, b, cc, d, a, x[12], 0x8d2a4c8a, 20);
        MD5_STEP(MD5_H, a, b, cc, d, x[5], 0xfffa3942, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[8], 0x8771f681, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[11], 0x6d9d6122, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[14], 0xfde5380c, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[1], 0xa4beea44, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[4], 0x4bdecfa9, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[7], 0xf6bb4b60, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[10], 0xbebfbc70, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[13], 0x289b7ec6, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[0], 0xeaa127fa, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[3], 0xd4ef3085, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[6], 0x04881d05, 23);
        MD5_STEP(MD5_H, a, b, cc, d, x[9], 0xd9d4d039, 4);
        MD5_STEP(MD5_H, d, a, b, cc, x[12], 0xe6db99e5, 11);
        MD5_STEP(MD5_H, cc, d, a, b, x[15], 0x1fa27cf8, 16);
        MD5_STEP(MD5_H, b, cc, d, a, x[2], 0xc4ac5665, 23);
        MD5_STEP(MD5_I, a, b, cc, d, x[0], 0xf4292244, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[7], 0x432aff97, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[14], 0xab9423a7, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[5], 0xfc93a039, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[12], 0x655b59c3, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[3], 0x8f0ccc92, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[10], 0xffeff47d, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[1], 0x85845dd1, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[8], 0x6fa87e4f, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[15], 0xfe2ce6e0, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[6], 0xa3014314, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[13], 0x4e0811a1, 21);
        MD5_STEP(MD5_I, a, b, cc, d, x[4], 0xf7537e82, 6);
        MD5_STEP(MD5_I, d, a, b, cc, x[11], 0xbd3af235, 10);
        MD5_STEP(MD5_I, cc, d, a, b, x[2], 0x2ad7d2bb, 15);
        MD5_STEP(MD5_I, b, cc, d, a, x[9], 0xeb86d391, 21);
#undef MD5_F
#undef MD5_G
#undef MD5_H
#undef MD5_I
#undef MD5_STEP
        p->state[0] += a;
        p->state[1] += b;
        p->state[2] += cc;
        p->state[3] += d;
        data += 64;
        len -= 64;
    }
    memcpy(c->buffer, data, len);
}

void md5_finish(md5_context* ctx, unsigned char* out)
{
    md5_ctx* c = (md5_ctx*)ctx;
    unsigned int used = (c->total[0] >> 3) & 0x3f;
    unsigned char pad[64] = {0x80};
    unsigned char lenbytes[8];
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        lenbytes[i] = (unsigned char)(c->total[0] >> (i * 8));
        lenbytes[4 + i] = (unsigned char)(c->total[1] >> (i * 8));
    }
    md5_update(ctx, pad, used < 56 ? 56 - used : 120 - used);
    md5_update(ctx, lenbytes, 8);
    for (i = 0; i < 4; ++i)
    {
        out[i * 4] = (unsigned char)(c->state[i]);
        out[i * 4 + 1] = (unsigned char)(c->state[i] >> 8);
        out[i * 4 + 2] = (unsigned char)(c->state[i] >> 16);
        out[i * 4 + 3] = (unsigned char)(c->state[i] >> 24);
    }
}
