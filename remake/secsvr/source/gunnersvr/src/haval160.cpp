/* haval160.cpp -- HAVAL V.1，3 轮、160 位（Yuliang Zheng 参考实现，BSD 许可）
   按 DWARF 类结构封装：CHaval160::havalInit/Update/Final/Transform3。 */

#include "inc/haval160.h"

#include <string.h>

#define HAVAL_VERSION 1
#define PASS 3
#define FPTLEN 160

#define rotate_right(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

/* f 函数族（参考实现原样） */
#define f_1(x6, x5, x4, x3, x2, x1, x0) \
    ((x1) & ((x0) ^ (x4)) ^ (x2) & (x5) ^ (x3) & (x6) ^ (x0))
#define f_2(x6, x5, x4, x3, x2, x1, x0) \
    ((x2) & ((x1) & ~(x3) ^ (x4) & (x5) ^ (x6) ^ (x0)) ^ \
     (x4) & ((x1) ^ (x5)) ^ (x3) & (x5) ^ (x0))
#define f_3(x6, x5, x4, x3, x2, x1, x0) \
    ((x3) & ((x1) & (x2) ^ (x6) ^ (x0)) ^ (x1) & (x4) ^ (x2) & (x5) ^ (x0))

/* PASS=3 的 phi 置换 */
#define Fphi_1(x6, x5, x4, x3, x2, x1, x0) f_1(x1, x0, x3, x5, x6, x2, x4)
#define Fphi_2(x6, x5, x4, x3, x2, x1, x0) f_2(x4, x2, x1, x0, x5, x3, x6)
#define Fphi_3(x6, x5, x4, x3, x2, x1, x0) f_3(x6, x1, x2, x3, x4, x5, x0)

#define FF_1(x7, x6, x5, x4, x3, x2, x1, x0, w) { \
    unsigned int temp = Fphi_1(x6, x5, x4, x3, x2, x1, x0); \
    (x7) = rotate_right(temp, 7) + rotate_right((x7), 11) + (w); \
}
#define FF_2(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
    unsigned int temp = Fphi_2(x6, x5, x4, x3, x2, x1, x0); \
    (x7) = rotate_right(temp, 7) + rotate_right((x7), 11) + (w) + (c); \
}
#define FF_3(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
    unsigned int temp = Fphi_3(x6, x5, x4, x3, x2, x1, x0); \
    (x7) = rotate_right(temp, 7) + rotate_right((x7), 11) + (w) + (c); \
}

void CHaval160::havalTransform3(unsigned int *E, unsigned char *D, unsigned int *T) {
    unsigned int *W = T;
    unsigned int t0 = E[0], t1 = E[1], t2 = E[2], t3 = E[3];
    unsigned int t4 = E[4], t5 = E[5], t6 = E[6], t7 = E[7];
    int i;

    for (i = 0; i < 32; i++)
        W[i] = (unsigned int)D[i * 4] |
               ((unsigned int)D[i * 4 + 1] << 8) |
               ((unsigned int)D[i * 4 + 2] << 16) |
               ((unsigned int)D[i * 4 + 3] << 24);

    /* Pass 1 */
    FF_1(t7, t6, t5, t4, t3, t2, t1, t0, W[0]);
    FF_1(t6, t5, t4, t3, t2, t1, t0, t7, W[1]);
    FF_1(t5, t4, t3, t2, t1, t0, t7, t6, W[2]);
    FF_1(t4, t3, t2, t1, t0, t7, t6, t5, W[3]);
    FF_1(t3, t2, t1, t0, t7, t6, t5, t4, W[4]);
    FF_1(t2, t1, t0, t7, t6, t5, t4, t3, W[5]);
    FF_1(t1, t0, t7, t6, t5, t4, t3, t2, W[6]);
    FF_1(t0, t7, t6, t5, t4, t3, t2, t1, W[7]);
    FF_1(t7, t6, t5, t4, t3, t2, t1, t0, W[8]);
    FF_1(t6, t5, t4, t3, t2, t1, t0, t7, W[9]);
    FF_1(t5, t4, t3, t2, t1, t0, t7, t6, W[10]);
    FF_1(t4, t3, t2, t1, t0, t7, t6, t5, W[11]);
    FF_1(t3, t2, t1, t0, t7, t6, t5, t4, W[12]);
    FF_1(t2, t1, t0, t7, t6, t5, t4, t3, W[13]);
    FF_1(t1, t0, t7, t6, t5, t4, t3, t2, W[14]);
    FF_1(t0, t7, t6, t5, t4, t3, t2, t1, W[15]);
    FF_1(t7, t6, t5, t4, t3, t2, t1, t0, W[16]);
    FF_1(t6, t5, t4, t3, t2, t1, t0, t7, W[17]);
    FF_1(t5, t4, t3, t2, t1, t0, t7, t6, W[18]);
    FF_1(t4, t3, t2, t1, t0, t7, t6, t5, W[19]);
    FF_1(t3, t2, t1, t0, t7, t6, t5, t4, W[20]);
    FF_1(t2, t1, t0, t7, t6, t5, t4, t3, W[21]);
    FF_1(t1, t0, t7, t6, t5, t4, t3, t2, W[22]);
    FF_1(t0, t7, t6, t5, t4, t3, t2, t1, W[23]);
    FF_1(t7, t6, t5, t4, t3, t2, t1, t0, W[24]);
    FF_1(t6, t5, t4, t3, t2, t1, t0, t7, W[25]);
    FF_1(t5, t4, t3, t2, t1, t0, t7, t6, W[26]);
    FF_1(t4, t3, t2, t1, t0, t7, t6, t5, W[27]);
    FF_1(t3, t2, t1, t0, t7, t6, t5, t4, W[28]);
    FF_1(t2, t1, t0, t7, t6, t5, t4, t3, W[29]);
    FF_1(t1, t0, t7, t6, t5, t4, t3, t2, W[30]);
    FF_1(t0, t7, t6, t5, t4, t3, t2, t1, W[31]);

    /* Pass 2 */
    FF_2(t7, t6, t5, t4, t3, t2, t1, t0, W[5],  0x452821E6);
    FF_2(t6, t5, t4, t3, t2, t1, t0, t7, W[14], 0x38D01377);
    FF_2(t5, t4, t3, t2, t1, t0, t7, t6, W[26], 0xBE5466CF);
    FF_2(t4, t3, t2, t1, t0, t7, t6, t5, W[18], 0x34E90C6C);
    FF_2(t3, t2, t1, t0, t7, t6, t5, t4, W[11], 0xC0AC29B7);
    FF_2(t2, t1, t0, t7, t6, t5, t4, t3, W[28], 0xC97C50DD);
    FF_2(t1, t0, t7, t6, t5, t4, t3, t2, W[7],  0x3F84D5B5);
    FF_2(t0, t7, t6, t5, t4, t3, t2, t1, W[16], 0xB5470917);
    FF_2(t7, t6, t5, t4, t3, t2, t1, t0, W[0],  0x9216D5D9);
    FF_2(t6, t5, t4, t3, t2, t1, t0, t7, W[23], 0x8979FB1B);
    FF_2(t5, t4, t3, t2, t1, t0, t7, t6, W[20], 0xD1310BA6);
    FF_2(t4, t3, t2, t1, t0, t7, t6, t5, W[22], 0x98DFB5AC);
    FF_2(t3, t2, t1, t0, t7, t6, t5, t4, W[1],  0x2FFD72DB);
    FF_2(t2, t1, t0, t7, t6, t5, t4, t3, W[10], 0xD01ADFB7);
    FF_2(t1, t0, t7, t6, t5, t4, t3, t2, W[4],  0xB8E1AFED);
    FF_2(t0, t7, t6, t5, t4, t3, t2, t1, W[8],  0x6A267E96);
    FF_2(t7, t6, t5, t4, t3, t2, t1, t0, W[30], 0xBA7C9045);
    FF_2(t6, t5, t4, t3, t2, t1, t0, t7, W[3],  0xF12C7F99);
    FF_2(t5, t4, t3, t2, t1, t0, t7, t6, W[21], 0x24A19947);
    FF_2(t4, t3, t2, t1, t0, t7, t6, t5, W[9],  0xB3916CF7);
    FF_2(t3, t2, t1, t0, t7, t6, t5, t4, W[17], 0x0801F2E2);
    FF_2(t2, t1, t0, t7, t6, t5, t4, t3, W[24], 0x858EFC16);
    FF_2(t1, t0, t7, t6, t5, t4, t3, t2, W[29], 0x636920D8);
    FF_2(t0, t7, t6, t5, t4, t3, t2, t1, W[6],  0x71574E69);
    FF_2(t7, t6, t5, t4, t3, t2, t1, t0, W[19], 0xA458FEA3);
    FF_2(t6, t5, t4, t3, t2, t1, t0, t7, W[12], 0xF4933D7E);
    FF_2(t5, t4, t3, t2, t1, t0, t7, t6, W[15], 0x0D95748F);
    FF_2(t4, t3, t2, t1, t0, t7, t6, t5, W[13], 0x728EB658);
    FF_2(t3, t2, t1, t0, t7, t6, t5, t4, W[2],  0x718BCD58);
    FF_2(t2, t1, t0, t7, t6, t5, t4, t3, W[25], 0x82154AEE);
    FF_2(t1, t0, t7, t6, t5, t4, t3, t2, W[31], 0x7B54A41D);
    FF_2(t0, t7, t6, t5, t4, t3, t2, t1, W[27], 0xC25A59B5);

    /* Pass 3 */
    FF_3(t7, t6, t5, t4, t3, t2, t1, t0, W[19], 0x9C30D539);
    FF_3(t6, t5, t4, t3, t2, t1, t0, t7, W[9],  0x2AF26013);
    FF_3(t5, t4, t3, t2, t1, t0, t7, t6, W[4],  0xC5D1B023);
    FF_3(t4, t3, t2, t1, t0, t7, t6, t5, W[20], 0x286085F0);
    FF_3(t3, t2, t1, t0, t7, t6, t5, t4, W[28], 0xCA417918);
    FF_3(t2, t1, t0, t7, t6, t5, t4, t3, W[17], 0xB8DB38EF);
    FF_3(t1, t0, t7, t6, t5, t4, t3, t2, W[8],  0x8E79DCB0);
    FF_3(t0, t7, t6, t5, t4, t3, t2, t1, W[22], 0x603A180E);
    FF_3(t7, t6, t5, t4, t3, t2, t1, t0, W[29], 0x6C9E0E8B);
    FF_3(t6, t5, t4, t3, t2, t1, t0, t7, W[14], 0xB01E8A3E);
    FF_3(t5, t4, t3, t2, t1, t0, t7, t6, W[25], 0xD71577C1);
    FF_3(t4, t3, t2, t1, t0, t7, t6, t5, W[12], 0xBD314B27);
    FF_3(t3, t2, t1, t0, t7, t6, t5, t4, W[24], 0x78AF2FDA);
    FF_3(t2, t1, t0, t7, t6, t5, t4, t3, W[30], 0x55605C60);
    FF_3(t1, t0, t7, t6, t5, t4, t3, t2, W[16], 0xE65525F3);
    FF_3(t0, t7, t6, t5, t4, t3, t2, t1, W[26], 0xAA55AB94);
    FF_3(t7, t6, t5, t4, t3, t2, t1, t0, W[31], 0x57489862);
    FF_3(t6, t5, t4, t3, t2, t1, t0, t7, W[15], 0x63E81440);
    FF_3(t5, t4, t3, t2, t1, t0, t7, t6, W[7],  0x55CA396A);
    FF_3(t4, t3, t2, t1, t0, t7, t6, t5, W[3],  0x2AAB10B6);
    FF_3(t3, t2, t1, t0, t7, t6, t5, t4, W[1],  0xB4CC5C34);
    FF_3(t2, t1, t0, t7, t6, t5, t4, t3, W[0],  0x1141E8CE);
    FF_3(t1, t0, t7, t6, t5, t4, t3, t2, W[18], 0xA15486AF);
    FF_3(t0, t7, t6, t5, t4, t3, t2, t1, W[27], 0x7C72E993);
    FF_3(t7, t6, t5, t4, t3, t2, t1, t0, W[13], 0xB3EE1411);
    FF_3(t6, t5, t4, t3, t2, t1, t0, t7, W[6],  0x636FBC2A);
    FF_3(t5, t4, t3, t2, t1, t0, t7, t6, W[21], 0x2BA9C55D);
    FF_3(t4, t3, t2, t1, t0, t7, t6, t5, W[10], 0x741831F6);
    FF_3(t3, t2, t1, t0, t7, t6, t5, t4, W[23], 0xCE5C3E16);
    FF_3(t2, t1, t0, t7, t6, t5, t4, t3, W[11], 0x9B87931E);
    FF_3(t1, t0, t7, t6, t5, t4, t3, t2, W[5],  0xAFD6BA33);
    FF_3(t0, t7, t6, t5, t4, t3, t2, t1, W[2],  0x6C24CF5C);

    E[0] += t0;
    E[1] += t1;
    E[2] += t2;
    E[3] += t3;
    E[4] += t4;
    E[5] += t5;
    E[6] += t6;
    E[7] += t7;
}

int CHaval160::havalInit() {
    if (!m_pctx)
        return -1;
    m_pctx->passes = PASS;
    m_pctx->hashLength = FPTLEN;
    m_pctx->digest[0] = 0x243F6A88;
    m_pctx->digest[1] = 0x85A308D3;
    m_pctx->digest[2] = 0x13198A2E;
    m_pctx->digest[3] = 0x03707344;
    m_pctx->digest[4] = 0xA4093822;
    m_pctx->digest[5] = 0x299F31D0;
    m_pctx->digest[6] = 0x082EFA98;
    m_pctx->digest[7] = 0xEC4E6C89;
    m_pctx->occupied = 0;
    m_pctx->bitCount[0] = m_pctx->bitCount[1] = 0;
    memset(m_pctx->block, 0, sizeof(m_pctx->block));
    return 0;
}

int CHaval160::havalUpdate(unsigned char *dataBuffer, unsigned int dataLength) {
    unsigned int i, rmd_len, fill_len;

    if (!m_pctx || !dataBuffer)
        return -1;
    rmd_len = (unsigned int)((m_pctx->bitCount[0] >> 3) & 0x7F);
    fill_len = 128 - rmd_len;

    if ((m_pctx->bitCount[0] += (dataLength << 3)) < (dataLength << 3))
        m_pctx->bitCount[1]++;
    m_pctx->bitCount[1] += (dataLength >> 29);

    if (rmd_len + dataLength >= 128) {
        unsigned int W[32];
        memcpy(m_pctx->block + rmd_len, dataBuffer, fill_len);
        havalTransform3(m_pctx->digest, m_pctx->block, W);
        for (i = fill_len; i + 127 < dataLength; i += 128) {
            memcpy(m_pctx->block, dataBuffer + i, 128);
            havalTransform3(m_pctx->digest, m_pctx->block, W);
        }
        rmd_len = 0;
    } else {
        i = 0;
    }
    memcpy(m_pctx->block + rmd_len, dataBuffer + i, dataLength - i);
    return 0;
}

int CHaval160::havalFinal(unsigned char *digest) {
    unsigned char tail[10];
    unsigned int w, rmd_len, pad_len;
    unsigned int W[32];
    int i;

    if (!m_pctx || !digest)
        return -1;
    tail[0] = (unsigned char)(((FPTLEN & 0x3) << 6) |
                              ((PASS & 0x7) << 3) | (HAVAL_VERSION & 0x7));
    tail[1] = (unsigned char)((FPTLEN >> 2) & 0xFF);
    w = m_pctx->bitCount[0];
    tail[2] = (unsigned char)(w & 0xFF);
    tail[3] = (unsigned char)((w >> 8) & 0xFF);
    tail[4] = (unsigned char)((w >> 16) & 0xFF);
    tail[5] = (unsigned char)((w >> 24) & 0xFF);
    w = m_pctx->bitCount[1];
    tail[6] = (unsigned char)(w & 0xFF);
    tail[7] = (unsigned char)((w >> 8) & 0xFF);
    tail[8] = (unsigned char)((w >> 16) & 0xFF);
    tail[9] = (unsigned char)((w >> 24) & 0xFF);

    rmd_len = (unsigned int)((m_pctx->bitCount[0] >> 3) & 0x7F);
    pad_len = (rmd_len < 118) ? (118 - rmd_len) : (246 - rmd_len);
    {
        static const unsigned char padding[128] = {0x01};
        unsigned int p;
        for (p = 0; p < pad_len; p += 128) {
            unsigned int chunk = (pad_len - p < 128) ? (pad_len - p) : 128;
            havalUpdate((unsigned char *)padding, chunk);
        }
    }
    havalUpdate(tail, 10);

    /* tailor：160 位折叠（参考实现 FPTLEN == 160） */
    {
        unsigned int temp;
        temp = (m_pctx->digest[7] & 0x3F) |
               (m_pctx->digest[6] & (0x7F << 25)) |
               (m_pctx->digest[5] & (0x3F << 19));
        m_pctx->digest[0] += rotate_right(temp, 19);
        temp = (m_pctx->digest[7] & (0x3F << 6)) |
               (m_pctx->digest[6] & 0x3F) |
               (m_pctx->digest[5] & (0x7F << 25));
        m_pctx->digest[1] += rotate_right(temp, 25);
        temp = (m_pctx->digest[7] & (0x7F << 12)) |
               (m_pctx->digest[6] & (0x3F << 6)) |
               (m_pctx->digest[5] & 0x3F);
        m_pctx->digest[2] += temp;
        temp = (m_pctx->digest[7] & (0x3F << 19)) |
               (m_pctx->digest[6] & (0x7F << 12)) |
               (m_pctx->digest[5] & (0x3F << 6));
        m_pctx->digest[3] += temp >> 6;
        temp = (m_pctx->digest[7] & (0x7F << 25)) |
               (m_pctx->digest[6] & (0x3F << 19)) |
               (m_pctx->digest[5] & (0x7F << 12));
        m_pctx->digest[4] += temp >> 12;
    }

    for (i = 0; i < FPTLEN / 32; i++) {
        digest[i * 4]     = (unsigned char)(m_pctx->digest[i] & 0xFF);
        digest[i * 4 + 1] = (unsigned char)((m_pctx->digest[i] >> 8) & 0xFF);
        digest[i * 4 + 2] = (unsigned char)((m_pctx->digest[i] >> 16) & 0xFF);
        digest[i * 4 + 3] = (unsigned char)((m_pctx->digest[i] >> 24) & 0xFF);
    }
    memset(m_pctx, 0, sizeof(*m_pctx));
    return 0;
}

CHaval160::CHaval160() {
    m_pctx = new haval_ctx;
    havalInit();
}

CHaval160::~CHaval160() {
    if (m_pctx) {
        delete m_pctx;
        m_pctx = 0;
    }
}
