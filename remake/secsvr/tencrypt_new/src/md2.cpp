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

/* 一次变换：状态混合（18 轮）+ 校验和更新 */
void CMd2::md2_update_digest_and_checksum(md2_t *ctx) {
    unsigned int i, j, t;

    /* 加密表：x[0..15]=state, x[16..31]=block, x[32..47]=state^block */
    memcpy(&ctx->x[16], ctx->m, 16);
    for (i = 0; i < 16; i++)
        ctx->x[i + 32] = (unsigned char)(ctx->x[i] ^ ctx->x[i + 16]);
    t = 0;
    for (i = 0; i < 18; i++) {
        for (j = 0; j < 48; j++)
            t = ctx->x[j] ^= PI_SUBST[t];
        t = (t + i) & 0xff;
    }
    t = ctx->c[15];
    for (i = 0; i < 16; i++)
        t = ctx->c[i] ^= PI_SUBST[ctx->m[i] ^ t];
}

/* 填充并做最后一次变换 */
void CMd2::md2_append_padding_and_checksum(md2_t *ctx) {
    unsigned int padlen = (unsigned int)(16 - (ctx->mlen % 16));
    unsigned int i;
    for (i = 0; i < padlen; i++)
        ctx->m[ctx->mlen % 16 + i] = (unsigned char)padlen;
    ctx->mlen += padlen;
    md2_update_digest_and_checksum(ctx);
}

int CMd2::md2_init(md2_t *ctx) {
    memset(ctx, 0, sizeof(md2_t));
    ctx->magic = 0x4D44324F; /* "MD2O"；与二进制逐字节一致性待反汇编核对 */
    return 0;
}

int CMd2::md2_update_data(md2_t *ctx, unsigned char *data, unsigned int len) {
    unsigned int i;
    if (!ctx || !data)
        return -1;
    for (i = 0; i < len; i++) {
        ctx->m[ctx->mlen % 16] = data[i];
        ctx->mlen++;
        if (ctx->mlen % 16 == 0)
            md2_update_digest_and_checksum(ctx);
    }
    return 0;
}

int CMd2::md2_finish(md2_t *ctx, unsigned char *digest, unsigned int len) {
    unsigned int i;
    if (!ctx || !digest)
        return -1;
    md2_append_padding_and_checksum(ctx);
    /* 把校验和作为最后一块处理 */
    memcpy(ctx->m, ctx->c, 16);
    ctx->mlen += 16;
    md2_update_digest_and_checksum(ctx);
    if (len > 16)
        len = 16;
    for (i = 0; i < len; i++)
        digest[i] = ctx->x[i];
    return 0;
}

CMd2::CMd2() {
}

CMd2::~CMd2() {
}

void CMd2::Init() {
}

void CMd2::UnInit() {
}
