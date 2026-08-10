/* mars.cpp -- MARS 块密码（Gladman 公开实现 + 自定义 15 项 t_key 密钥扩展）
   mars_setkey 由二进制反汇编还原（与标准 MARS 差异明显）：
   - 密钥 dword 复制到 t_key[0..c-1]，t_key[c]=c，其余清零
   - 外层 4 轮（j=0..3）：
       t_key[i] = rotl3(t_key[IM2[i]] ^ t_key[IM7[i]]) ^ t_key[i] ^ (4*i+j)
       内层 k=0..3：t_key[i] = rotl9(t_key[i] + SBOX[t_key[IM1[i]] & 511])
       ll_key[10*j+k] = t_key[I4M[k]]（k=0..9，I4M 只用到前 10 项）
   - 密钥修正：i=0..15 作用于 ll_key[5+2i]：gen_mask 无 bit31 修补，
     w ^= rotl(B_TAB[w&3], ll_key[4+2i]&31) & m & ~3
   encrypt/decrypt：Gladman MARS 标准 f_mix/f_ktr/b_mix/r_ktr 宏。
   验证：uni_call 调二进制 mars_setkey/mars_enc/mars_dec 逐字节一致。 */

#include "inc/mars.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned int u4byte;

static inline u4byte rotl_fixed(u4byte x, unsigned int n) {
    return (x << n) | (x >> (32 - n));
}

static inline u4byte rotl_mod(u4byte x, unsigned int n) {
    n &= 31;
    return n ? ((x << n) | (x >> (32 - n))) : x;
}

static inline u4byte rotr_fixed(u4byte x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

static u4byte gen_mask(u4byte x) {
    u4byte m;

    m = (~x ^ (x >> 1)) & 0x7fffffff;
    m &= (m >> 1) & (m >> 2); m &= (m >> 3) & (m >> 6);

    if (!m)
        return 0;

    m <<= 1; m |= (m << 1); m |= (m << 2); m |= (m << 4);
    /* 二进制无 bit31 修补：m |= (m << 1) & ~x & 0x80000000 */

    return m & 0xfffffffc;
}

const unsigned int CMars::FUCKING_S_BOX[512] = {
#include "mars_sbox.inc"
};
const unsigned int CMars::FUCKING_IM1[15] = {
#include "mars_im1.inc"
};
const unsigned int CMars::FUCKING_IM2[15] = {
#include "mars_im2.inc"
};
const unsigned int CMars::FUCKING_IM7[15] = {
#include "mars_im7.inc"
};
const unsigned int CMars::FUCKING_I4M[15] = {
#include "mars_i4m.inc"
};
const unsigned int CMars::FUCKING_B_TAB[4] = {
#include "mars_btab.inc"
};

// mangled: _ZN5CMars4InitEPvS0_
void CMars::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN5CMars6UnInitEv
void CMars::UnInit() {
}

// mangled: _ZN5CMarsC1Ev / _ZN5CMarsC2Ev
CMars::CMars() {
    Init(0, 0);
}

// mangled: _ZN5CMarsD1Ev / _ZN5CMarsD2Ev
CMars::~CMars() {
    UnInit();
}

// mangled: _ZN5CMars11mars_setkeyEPhj
void CMars::mars_setkey(unsigned char *key, unsigned int keylen) {
    unsigned int c = keylen >> 5;
    unsigned int i;
    unsigned int j;
    unsigned int k;

    if (c == 0) {
        t_key[0] = 0;
        for (i = 1; i < 15; i++)
            t_key[i] = 0;
    } else {
        for (i = 0; i < c; i++)
            t_key[i] = ((u4byte *)key)[i];
        t_key[c] = c;
        for (i = c + 1; i < 15; i++)
            t_key[i] = 0;
    }

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 15; i++)
            t_key[i] = rotl_fixed(t_key[FUCKING_IM2[i]] ^
                                  t_key[FUCKING_IM7[i]], 3) ^
                       t_key[i] ^ (4 * i + j);

        for (k = 0; k < 4; k++)
            for (i = 0; i < 15; i++)
                t_key[i] = rotl_fixed(t_key[i] +
                    FUCKING_S_BOX[t_key[FUCKING_IM1[i]] & 511], 9);

        for (k = 0; k < 10; k++)
            ll_key[10 * j + k] = t_key[FUCKING_I4M[k]];
    }

    for (i = 0; i < 16; i++) {
        unsigned int w = ll_key[5 + 2 * i] | 3;
        unsigned int m = gen_mask(w);

        if (m)
            w ^= rotl_mod(FUCKING_B_TAB[ll_key[5 + 2 * i] & 3],
                          ll_key[4 + 2 * i]) & m;

        ll_key[5 + 2 * i] = w;
    }
}

#define f_mix(a, b, c, d)                           \
    do {                                            \
        u4byte r = rotr_fixed(a, 8);                \
        b ^= CMars::FUCKING_S_BOX[a & 255];         \
        b += CMars::FUCKING_S_BOX[(r & 255) + 256]; \
        r = rotr_fixed(a, 16);                      \
        a = rotr_fixed(a, 24);                      \
        c += CMars::FUCKING_S_BOX[r & 255];         \
        d ^= CMars::FUCKING_S_BOX[(a & 255) + 256]; \
    } while (0)

#define b_mix(a, b, c, d)                            \
    do {                                             \
        u4byte r = rotl_fixed(a, 8);                 \
        b ^= CMars::FUCKING_S_BOX[(a & 255) + 256];  \
        c -= CMars::FUCKING_S_BOX[r & 255];          \
        r = rotl_fixed(a, 16);                       \
        a = rotl_fixed(a, 24);                       \
        d -= CMars::FUCKING_S_BOX[(r & 255) + 256];  \
        d ^= CMars::FUCKING_S_BOX[a & 255];          \
    } while (0)

#define f_ktr(a, b, c, d, i)                         \
    do {                                             \
        u4byte m = a + ll_key[i];                    \
        u4byte l, r;                                 \
        a = rotl_fixed(a, 13);                       \
        r = a * ll_key[i + 1];                       \
        l = CMars::FUCKING_S_BOX[m & 511];           \
        r = rotl_fixed(r, 5);                        \
        l ^= r;                                      \
        c += rotl_mod(m, r);                         \
        r = rotl_fixed(r, 5);                        \
        l ^= r;                                      \
        d ^= r;                                      \
        b += rotl_mod(l, r);                         \
    } while (0)

#define r_ktr(a, b, c, d, i)                         \
    do {                                             \
        u4byte r = a * ll_key[i + 1];                \
        u4byte l, m;                                 \
        a = rotr_fixed(a, 13);                       \
        m = a + ll_key[i];                           \
        l = CMars::FUCKING_S_BOX[m & 511];           \
        r = rotl_fixed(r, 5);                        \
        l ^= r;                                      \
        c -= rotl_mod(m, r);                         \
        r = rotl_fixed(r, 5);                        \
        l ^= r;                                      \
        d ^= r;                                      \
        b -= rotl_mod(l, r);                         \
    } while (0)

// mangled: _ZN5CMars8mars_encEPhS0_
void CMars::mars_enc(unsigned char *inData, unsigned char *outData) {
    u4byte a, b, c, d;

    a = ((u4byte *)inData)[0] + ll_key[0];
    b = ((u4byte *)inData)[1] + ll_key[1];
    c = ((u4byte *)inData)[2] + ll_key[2];
    d = ((u4byte *)inData)[3] + ll_key[3];

    f_mix(a, b, c, d); a += d;
    f_mix(b, c, d, a); b += c;
    f_mix(c, d, a, b);
    f_mix(d, a, b, c);
    f_mix(a, b, c, d); a += d;
    f_mix(b, c, d, a); b += c;
    f_mix(c, d, a, b);
    f_mix(d, a, b, c);

    f_ktr(a, b, c, d, 4); f_ktr(b, c, d, a, 6);
    f_ktr(c, d, a, b, 8); f_ktr(d, a, b, c, 10);
    f_ktr(a, b, c, d, 12); f_ktr(b, c, d, a, 14);
    f_ktr(c, d, a, b, 16); f_ktr(d, a, b, c, 18);
    f_ktr(a, d, c, b, 20); f_ktr(b, a, d, c, 22);
    f_ktr(c, b, a, d, 24); f_ktr(d, c, b, a, 26);
    f_ktr(a, d, c, b, 28); f_ktr(b, a, d, c, 30);
    f_ktr(c, b, a, d, 32); f_ktr(d, c, b, a, 34);

    b_mix(a, b, c, d);
    b_mix(b, c, d, a); c -= b;
    b_mix(c, d, a, b); d -= a;
    b_mix(d, a, b, c);
    b_mix(a, b, c, d);
    b_mix(b, c, d, a); c -= b;
    b_mix(c, d, a, b); d -= a;
    b_mix(d, a, b, c);

    ((u4byte *)outData)[0] = a - ll_key[36];
    ((u4byte *)outData)[1] = b - ll_key[37];
    ((u4byte *)outData)[2] = c - ll_key[38];
    ((u4byte *)outData)[3] = d - ll_key[39];
}

// mangled: _ZN5CMars8mars_decEPhS0_
void CMars::mars_dec(unsigned char *inData, unsigned char *outData) {
    u4byte a, b, c, d;

    d = ((u4byte *)inData)[0] + ll_key[36];
    c = ((u4byte *)inData)[1] + ll_key[37];
    b = ((u4byte *)inData)[2] + ll_key[38];
    a = ((u4byte *)inData)[3] + ll_key[39];

    f_mix(a, b, c, d); a += d;
    f_mix(b, c, d, a); b += c;
    f_mix(c, d, a, b);
    f_mix(d, a, b, c);
    f_mix(a, b, c, d); a += d;
    f_mix(b, c, d, a); b += c;
    f_mix(c, d, a, b);
    f_mix(d, a, b, c);

    r_ktr(a, b, c, d, 34); r_ktr(b, c, d, a, 32);
    r_ktr(c, d, a, b, 30); r_ktr(d, a, b, c, 28);
    r_ktr(a, b, c, d, 26); r_ktr(b, c, d, a, 24);
    r_ktr(c, d, a, b, 22); r_ktr(d, a, b, c, 20);
    r_ktr(a, d, c, b, 18); r_ktr(b, a, d, c, 16);
    r_ktr(c, b, a, d, 14); r_ktr(d, c, b, a, 12);
    r_ktr(a, d, c, b, 10); r_ktr(b, a, d, c, 8);
    r_ktr(c, b, a, d, 6); r_ktr(d, c, b, a, 4);

    b_mix(a, b, c, d);
    b_mix(b, c, d, a); c -= b;
    b_mix(c, d, a, b); d -= a;
    b_mix(d, a, b, c);
    b_mix(a, b, c, d);
    b_mix(b, c, d, a); c -= b;
    b_mix(c, d, a, b); d -= a;
    b_mix(d, a, b, c);

    d -= ll_key[0];
    c -= ll_key[1];
    b -= ll_key[2];
    a -= ll_key[3];

    ((u4byte *)outData)[0] = d;
    ((u4byte *)outData)[1] = c;
    ((u4byte *)outData)[2] = b;
    ((u4byte *)outData)[3] = a;
}

// mangled: _ZN5CMars4MarsEPhbS0_j
void CMars::Mars(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;

    mars_setkey(key, 0x80);

    blocks = buflen >> 4;
    for (i = 0; i < blocks; i++) {
        if (benc)
            mars_enc(buf + 16 * i, buf + 16 * i);
        else
            mars_dec(buf + 16 * i, buf + 16 * i);
    }
}

// mangled: _Z7TenMarsPhbS_j
void TenMars(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    CMars mars;

    mars.Mars(key, benc, buf, buflen);
}
