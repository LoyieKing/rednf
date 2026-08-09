/* mbc.cpp -- MBC 块密码（自定义 SetKey 扩展，与二进制逐字节核对）
   SetKey 由反汇编机械翻译：MMB 风格派生 a..g → 8 组 iv_tbl 扩展
   （每组 4 个 32 位值，rotl 5/9/13/17）→ 低/高字节表置换 key_exp[0..31]。 */

#include "inc/mbc.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned int mbc_ivtbl[64] = {
#include "mbc_ivtbl.inc"
};

#define ROTL(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// mangled: _ZN4CMbc6SetKeyEPj
void CMbc::SetKey(unsigned int *in_key) {
    unsigned int blk[4];
    unsigned int fbk;
    unsigned int t;
    unsigned int key_exp[32];
    unsigned int x;
    unsigned int i;
    unsigned int a, b, c, d, e, f, g, idx, iv[28];
    unsigned int S0, S1, S2, S3, V0, V1, V2, V3;
    unsigned char buf[256];
    unsigned int tmp;
    int j;

    a = in_key[0] - 0x33cfc738;
    b = ROTL(a, 5) + in_key[1];
    c = ROTL(b, 7) + in_key[2];
    d = ROTL(c, 11) + in_key[3];
    idx = ROTL(d, 1) & 0x3f;
    e = ROTL(d, 2) + iv_tbl[idx];
    e ^= 0x19860719;
    e = ROTL(e, 1) ^ a;
    f = ROTL(e, 2) ^ b;
    g = ROTL(f, 3) ^ c;
    idx = (e + idx) & 0x3f;   /* 二次索引 */
    d ^= ROTL(g, 4);          /* d 被修改后才用于扩展 */
    for (i = 0; i < 28; i++)
        iv[i] = iv_tbl[(idx + i) & 0x3f];

    S0 = e + iv[0] + iv[1] + 2 * iv[2];
    S1 = f + iv[0] + iv[1] + 2 * iv[2];
    S2 = g + 2 * iv[0] + 2 * iv[1];
    S3 = d + 4 * iv[3];

    /* group 0: order (S0,S1,S2,S3), no new iv */
    key_exp[0] = ROTL(S0, 5);
    key_exp[1] = ROTL(S1, 9);
    key_exp[2] = ROTL(S2, 13);
    key_exp[3] = ROTL(S3, 17);
    /* group 1: order (S3,S0,S1,S2) + iv0..3 */
    V0 = S3 + iv[0]; V1 = S0 + iv[1]; V2 = S1 + iv[2]; V3 = S2 + iv[3];
    key_exp[4] = ROTL(V0, 5); key_exp[5] = ROTL(V1, 9);
    key_exp[6] = ROTL(V2, 13); key_exp[7] = ROTL(V3, 17);
    /* group 2: order (V0,V1,V3,V2) + iv4..7 */
    key_exp[8]  = ROTL(V0 + iv[4], 5);
    key_exp[9]  = ROTL(V1 + iv[5], 9);
    key_exp[10] = ROTL(V3 + iv[6], 13);
    key_exp[11] = ROTL(V2 + iv[7], 17);
    /* group 3: order (V0',V2',V1',V3') + iv8..11 */
    key_exp[12] = ROTL(V0 + iv[4] + iv[8], 5);
    key_exp[13] = ROTL(V2 + iv[7] + iv[9], 9);
    key_exp[14] = ROTL(V1 + iv[5] + iv[10], 13);
    key_exp[15] = ROTL(V3 + iv[6] + iv[11], 17);
    /* group 4: order (S0''',S2''',S3''',S1''') + iv12..15 */
    key_exp[16] = ROTL(V0 + iv[4] + iv[8] + iv[12], 5);
    key_exp[17] = ROTL(V2 + iv[7] + iv[9] + iv[13], 9);
    key_exp[18] = ROTL(V3 + iv[6] + iv[11] + iv[14], 13);
    key_exp[19] = ROTL(V1 + iv[5] + iv[10] + iv[15], 17);
    /* group 5: order (S0'''',S3'''',S1'''',S2'''') + iv16..19 */
    key_exp[20] = ROTL(V0 + iv[4] + iv[8] + iv[12] + iv[16], 5);
    key_exp[21] = ROTL(V3 + iv[6] + iv[11] + iv[14] + iv[17], 9);
    key_exp[22] = ROTL(V1 + iv[5] + iv[10] + iv[15] + iv[18], 13);
    key_exp[23] = ROTL(V2 + iv[7] + iv[9] + iv[13] + iv[19], 17);
    /* group 6: order (S0''''',S3''''',S2''''',S1''''') + iv20..23 */
    key_exp[24] = ROTL(V0 + iv[4] + iv[8] + iv[12] + iv[16] + iv[20], 5);
    key_exp[25] = ROTL(V3 + iv[6] + iv[11] + iv[14] + iv[17] + iv[21], 9);
    key_exp[26] = ROTL(V2 + iv[7] + iv[9] + iv[13] + iv[19] + iv[22], 13);
    key_exp[27] = ROTL(V1 + iv[5] + iv[10] + iv[15] + iv[18] + iv[23], 17);
    /* group 7: order (S2'''''',S1'''''',S3'''''',S0'''''') + iv24..27 */
    key_exp[28] = ROTL(V2 + iv[7] + iv[9] + iv[13] + iv[19] + iv[22] + iv[24], 5);
    key_exp[29] = ROTL(V1 + iv[5] + iv[10] + iv[15] + iv[18] + iv[23] + iv[25], 9);
    key_exp[30] = ROTL(V3 + iv[6] + iv[11] + iv[14] + iv[17] + iv[21] + iv[26], 13);
    key_exp[31] = ROTL(V0 + iv[4] + iv[8] + iv[12] + iv[16] + iv[20] + iv[27], 17);

    /* 低/高字节表 + 置换 */
    for (i = 0; i < 32; i++) {
        buf[i] = (unsigned char)(key_exp[i] & 0xff);
        buf[0x20 + i] = (unsigned char)((key_exp[i] >> 8) & 0xff);
    }
    for (i = 0; i < 64; i++) {
        j = i & 0x1f;
        t = key_exp[buf[i] & 0x1f];
        key_exp[buf[i] & 0x1f] = key_exp[j];
        key_exp[j] = t;
    }
    memcpy(key, key_exp, 32 * sizeof(unsigned int));
}

// mangled: _ZN4CMbc7EncryptEPj
void CMbc::Encrypt(unsigned int *blk) {
    unsigned int fbk;
    unsigned int x[4];
    unsigned int t;
    int i;

    x[0] = blk[0]; x[1] = blk[1]; x[2] = blk[2]; x[3] = blk[3];
    x[3] ^= key[16]; x[0] ^= key[19]; x[1] ^= key[21]; x[2] ^= key[23];

#define MB_ROUND(st, c0, c1, c2, k, wt0, wt1, wt2, wt3, w0, w1, w2, w3)   \
    do {                                                                \
        unsigned int ps = x[st];                                        \
        ps += key[k];                                                   \
        x[c0] += ROTL(ps, 1);                                           \
        x[c1] += ROTL(x[c0], 2);                                        \
        x[c2] += ROTL(x[c1], 3);                                        \
        t = ROTL(x[c2], 5) + key[k];                                    \
        t ^= key[k + 1];                                                \
        x[st] = ps ^ t;                                                 \
        x[c0] ^= ROTL(t, 1);                                            \
        x[c1] ^= ROTL(t, 2);                                            \
        x[c2] ^= ROTL(key[k], 1);                                       \
        x[wt0] ^= key[w0];                                              \
        x[wt1] ^= key[w1];                                              \
        x[wt2] ^= key[w2];                                              \
        x[wt3] ^= key[w3];                                              \
    } while (0)

#define MB_ROUND_NW(st, c0, c1, c2, k)                                  \
    do {                                                                \
        unsigned int ps = x[st];                                        \
        ps += key[k];                                                   \
        x[c0] += ROTL(ps, 1);                                           \
        x[c1] += ROTL(x[c0], 2);                                        \
        x[c2] += ROTL(x[c1], 3);                                        \
        t = ROTL(x[c2], 5) + key[k];                                    \
        t ^= key[k + 1];                                                \
        x[st] = ps ^ t;                                                 \
        x[c0] ^= ROTL(t, 1);                                            \
        x[c1] ^= ROTL(t, 2);                                            \
        x[c2] ^= ROTL(key[k], 1);                                       \
    } while (0)

    MB_ROUND(3, 0, 1, 2, 0, 3, 0, 2, 1, 17, 20, 22, 24);
    MB_ROUND(3, 0, 2, 1, 1, 3, 1, 0, 2, 18, 21, 23, 25);
    MB_ROUND(3, 1, 0, 2, 2, 3, 1, 2, 0, 19, 22, 24, 26);
    MB_ROUND(3, 1, 2, 0, 3, 3, 2, 0, 1, 20, 23, 25, 27);
    MB_ROUND(3, 2, 0, 1, 4, 3, 2, 1, 0, 21, 24, 26, 28);
    MB_ROUND(3, 2, 1, 0, 5, 1, 0, 2, 3, 22, 25, 27, 29);
    MB_ROUND(1, 0, 2, 3, 6, 1, 0, 3, 2, 23, 26, 28, 30);
    MB_ROUND(1, 0, 3, 2, 7, 1, 2, 0, 3, 24, 27, 29, 31);
    MB_ROUND(1, 2, 0, 3, 8, 1, 2, 3, 0, 25, 28, 30, 0);
    MB_ROUND(1, 2, 3, 0, 9, 0, 1, 2, 3, 26, 29, 31, 1);
    MB_ROUND(0, 1, 2, 3, 10, 0, 1, 3, 2, 27, 30, 0, 2);
    MB_ROUND(0, 1, 3, 2, 11, 0, 2, 1, 3, 28, 31, 1, 3);
    MB_ROUND(0, 2, 1, 3, 12, 0, 2, 3, 1, 29, 0, 2, 4);
    MB_ROUND(0, 2, 3, 1, 13, 0, 3, 1, 2, 30, 1, 3, 5);
    MB_ROUND(0, 3, 1, 2, 14, 0, 3, 2, 1, 31, 2, 4, 6);
    /* 最后一轮：无白化 */
    MB_ROUND_NW(0, 3, 2, 1, 15);
#undef MB_ROUND
#undef MB_ROUND_NW

    blk[0] = x[0]; blk[1] = x[1]; blk[2] = x[2]; blk[3] = x[3];
}

// mangled: _ZN4CMbc7DecryptEPj
void CMbc::Decrypt(unsigned int *blk) {
    unsigned int fbk;
    unsigned int x[4];
    unsigned int t;

    x[0] = blk[0]; x[1] = blk[1]; x[2] = blk[2]; x[3] = blk[3];

#define MB_IROUND(st, c0, c1, c2, k, wt0, wt1, wt2, wt3, w0, w1, w2, w3) \
    do {                                                               \
        x[wt0] ^= key[w0];                                             \
        x[wt1] ^= key[w1];                                             \
        x[wt2] ^= key[w2];                                             \
        x[wt3] ^= key[w3];                                             \
        x[c2] ^= ROTL(key[k], 1);                                      \
        t = ROTL(x[c2], 5) + key[k];                                   \
        t ^= key[k + 1];                                               \
        x[st] ^= t;                                                    \
        x[c0] ^= ROTL(t, 1);                                           \
        x[c1] ^= ROTL(t, 2);                                           \
        x[c2] -= ROTL(x[c1], 3);                                       \
        x[c1] -= ROTL(x[c0], 2);                                       \
        x[c0] -= ROTL(x[st], 1);                                       \
        x[st] -= key[k];                                               \
    } while (0)

#define MB_IROUND_NW(st, c0, c1, c2, k)                                 \
    do {                                                                \
        x[c2] ^= ROTL(key[k], 1);                                      \
        t = ROTL(x[c2], 5) + key[k];                                   \
        t ^= key[k + 1];                                                \
        x[st] ^= t;                                                    \
        x[c0] ^= ROTL(t, 1);                                           \
        x[c1] ^= ROTL(t, 2);                                           \
        x[c2] -= ROTL(x[c1], 3);                                       \
        x[c1] -= ROTL(x[c0], 2);                                       \
        x[c0] -= ROTL(x[st], 1);                                       \
        x[st] -= key[k];                                               \
    } while (0)

    MB_IROUND_NW(0, 3, 2, 1, 15);
    MB_IROUND(0, 3, 1, 2, 14, 0, 3, 2, 1, 31, 2, 4, 6);
    MB_IROUND(0, 2, 3, 1, 13, 0, 3, 1, 2, 30, 1, 3, 5);
    MB_IROUND(0, 2, 1, 3, 12, 0, 2, 3, 1, 29, 0, 2, 4);
    MB_IROUND(0, 1, 3, 2, 11, 0, 2, 1, 3, 28, 31, 1, 3);
    MB_IROUND(0, 1, 2, 3, 10, 0, 1, 3, 2, 27, 30, 0, 2);
    MB_IROUND(1, 2, 3, 0, 9, 0, 1, 2, 3, 26, 29, 31, 1);
    MB_IROUND(1, 2, 0, 3, 8, 1, 2, 3, 0, 25, 28, 30, 0);
    MB_IROUND(1, 0, 3, 2, 7, 1, 2, 0, 3, 24, 27, 29, 31);
    MB_IROUND(1, 0, 2, 3, 6, 1, 0, 3, 2, 23, 26, 28, 30);
    MB_IROUND(3, 2, 1, 0, 5, 1, 0, 2, 3, 22, 25, 27, 29);
    MB_IROUND(3, 2, 0, 1, 4, 3, 2, 1, 0, 21, 24, 26, 28);
    MB_IROUND(3, 1, 2, 0, 3, 3, 2, 0, 1, 20, 23, 25, 27);
    MB_IROUND(3, 1, 0, 2, 2, 3, 1, 2, 0, 19, 22, 24, 26);
    MB_IROUND(3, 0, 2, 1, 1, 3, 1, 0, 2, 18, 21, 23, 25);
    MB_IROUND(3, 0, 1, 2, 0, 3, 0, 2, 1, 17, 20, 22, 24);
#undef MB_IROUND
#undef MB_IROUND_NW

    x[3] ^= key[16]; x[0] ^= key[19]; x[1] ^= key[21]; x[2] ^= key[23];
    blk[0] = x[0]; blk[1] = x[1]; blk[2] = x[2]; blk[3] = x[3];
}

// mangled: _ZN4CMbc3MbcEPhbS0_j
void CMbc::Mbc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        SetKey((unsigned int *)key);
        nLen = buflen / 16;
        for (i = 0; i < nLen; i++) {
            if (benc)
                Encrypt((unsigned int *)(buf + i * 16));
            else
                Decrypt((unsigned int *)(buf + i * 16));
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
    }
}

// mangled: _ZN4CMbc4InitEPvS0_
void CMbc::Init(void *pSys, void *pLocal) {
    unsigned int _iv_tbl[64];

    m_pSys = pSys;
    m_pLocal = pLocal;
    iv_tbl = 0;
    key = 0;
    if (iv_tbl == 0)
        iv_tbl = new unsigned int[64];
    if (key == 0)
        key = new unsigned int[32];
    memcpy(_iv_tbl, mbc_ivtbl, 64 * sizeof(unsigned int));
    memmove(iv_tbl, _iv_tbl, 64 * sizeof(unsigned int));
}

// mangled: _ZN4CMbc6UnInitEv
void CMbc::UnInit() {
    if (iv_tbl != 0) {
        delete[] iv_tbl;
        iv_tbl = 0;
    }
    if (key != 0) {
        delete[] key;
        key = 0;
    }
}

// mangled: _ZN4CMbcC1Ev / _ZN4CMbcC2Ev
CMbc::CMbc() {
    Init(0, 0);
}

// mangled: _ZN4CMbcD1Ev / _ZN4CMbcD2Ev
CMbc::~CMbc() {
    UnInit();
}
