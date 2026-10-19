/* md2.cpp -- RFC 1319 MD2（公版参考实现）的 tencrypt_new 类封装
   由 DWARF 桩重建：CMd2::md2_init/update_data/finish 等，上下文布局
   __md2 { magic, l, mlen, x[48], c[16], m[16] }（sizeof 92 验证）。 */

#include "inc/md2.h"

#include <string.h>

/* RFC 1319 附录 PI 置换表（公版） */
static const unsigned char PI_SUBST[256] = {
    41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
    19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
    76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
    138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
    245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
    148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
    39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
    181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
    150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
    112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
    96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
    85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
    234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
    129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
    8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
    203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
    166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
    31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

/* ORIG 0x1f1940：状态混合与校验和都读 this->S（Init() 装入的 PI 表）；
   校验和的运行值存放在 ctx->l，跨块保持。 */
void CMd2::md2_update_digest_and_checksum(md2_t *ctx) {
    unsigned int i, j, t;

    /* 加密表：x[16..31]=block, x[32..47]=state^block */
    memcpy(&ctx->x[16], ctx->m, 16);
    for (i = 0; i < 16; i++)
        ctx->x[i + 32] = (unsigned char)(ctx->x[i] ^ ctx->x[i + 16]);
    t = 0;
    for (i = 0; i < 18; i++) {
        for (j = 0; j < 48; j++)
            t = ctx->x[j] = (unsigned char)(ctx->x[j] ^ this->S[t]);
        t = (t + i) & 0xff;
    }
    t = ctx->l;
    for (i = 0; i < 16; i++) {
        t = ctx->c[i] = (unsigned char)(ctx->c[i] ^ this->S[ctx->m[i] ^ t]);
        ctx->l = (unsigned char)t;
    }
}

/* ORIG 0x1f2060：填充行 = padding[padlen-1]（padlen 个 padlen），
   随后把 ctx->c 的 16 字节校验和作为最后一块送入 update_data。 */
void CMd2::md2_append_padding_and_checksum(md2_t *ctx) {
    unsigned int padlen = 16 - ctx->mlen;

    md2_update_data(ctx, this->padding[padlen - 1], padlen);
    md2_update_data(ctx, ctx->c, 16);
}

/* ORIG 0x1f1a90：写魔数 0xf00fd00d；ctx 为空返回 -1 */
int CMd2::md2_init(md2_t *ctx) {
    if (!ctx)
        return -1;
    ctx->magic = 0xf00fd00d;
    ctx->l = 0;
    ctx->mlen = 0;
    memset(ctx->x, 0, sizeof(ctx->x));
    memset(ctx->c, 0, sizeof(ctx->c));
    memset(ctx->m, 0, sizeof(ctx->m));
    return 0;
}

/* ORIG 0x1f1f20：ctx->mlen 是 ctx->m 内已缓冲的字节数（0..15），
   不是消息总长；成功返回 len，参数非法返回 -1。 */
int CMd2::md2_update_data(md2_t *ctx, unsigned char *data, unsigned int len) {
    unsigned int i, partLen;

    if (!ctx)
        return -1;
    if (ctx->magic != 0xf00fd00d)
        return -1;
    if (!data)
        return -1;
    if (!len)
        return 0;
    if (ctx->mlen + len <= 15) {
        memmove(&ctx->m[ctx->mlen], data, len);
        ctx->mlen += len;
        return (int)len;
    }
    partLen = 16 - ctx->mlen;
    memmove(&ctx->m[ctx->mlen], data, partLen);
    md2_update_digest_and_checksum(ctx);
    i = partLen;
    while (len - i > 15) {
        memmove(ctx->m, data + i, 16);
        md2_update_digest_and_checksum(ctx);
        i += 16;
    }
    ctx->mlen = len - i;
    if (ctx->mlen)
        memmove(ctx->m, data + i, ctx->mlen);
    return (int)len;
}

/* ORIG 0x1f20d0：成功返回 16（摘要长度），参数非法返回 -1；
   digest 缓冲必须 >= 16 字节。 */
int CMd2::md2_finish(md2_t *ctx, unsigned char *digest, unsigned int len) {
    if (!ctx || ctx->magic != 0xf00fd00d)
        return -1;
    if (!digest)
        return -1;
    if (len <= 15)
        return -1;
    md2_append_padding_and_checksum(ctx);
    memmove(digest, ctx->x, 16);
    return 16;
}

/* ORIG 0x1f1ec0/0x1f1a30：构造即 Init()，析构即 UnInit() */
CMd2::CMd2() {
    Init();
}

CMd2::~CMd2() {
    UnInit();
}

/* ORIG 0x1f1b10：建立 padding（16x16，第 n-1 行 = n 个 n）与 S（PI 表）
   两个 256 字节堆缓冲；成员在入口先清零，缓冲为空时才 new[]。 */
void CMd2::Init() {
    unsigned char pad[256];
    int i, j;

    this->padding = 0;
    this->S = 0;
    memset(pad, 0, sizeof(pad));
    for (i = 0; i < 16; i++)
        for (j = 0; j <= i; j++)
            pad[i * 16 + j] = (unsigned char)(i + 1);
    if (!this->padding) {
        this->padding = (unsigned char (*)[16]) new unsigned char[256];
        memset(this->padding, 0, 256);
    }
    memmove(this->padding, pad, 256);
    if (!this->S) {
        this->S = new unsigned char[256];
        memset(this->S, 0, 256);
    }
    memmove(this->S, PI_SUBST, 256);
}

/* ORIG 0x1f19e0 */
void CMd2::UnInit() {
    if (this->padding) {
        delete [] this->padding;
        this->padding = 0;
    }
    if (this->S) {
        delete [] this->S;
        this->S = 0;
    }
}
