/* gosthash.cpp -- GOST R 34.11-94（Markku-Juhani Saarinen 参考实现）
   按 DWARF 类结构封装：CGostHash（4 个 256 项 S 盒表 + 上下文）。 */

#include "inc/gosthash.h"

#include <string.h>

/* ORIG 0x1db730：4 个 256 项 S 盒表是 .rodata 常量
   CGostHash::InitBoxData()::C.0/C.2/C.4/C.6
   @0x258b80/0x258780/0x258380/0x257f80（各 1024 字节）；每个 m_psbox_N
   为空时按 ORIG 的 0x410 字节分配后整表拷入。表字节取自 ORIG 镜像
   （gosthash_sbox{1..4}.inc）。最后一行的 asm 标注把这三个常量的 ELF 名
   固定成 ORIG 的名字：GCC 自己只会把它编号成 C.0/C.1/C.2/C.3。 */
void CGostHash::InitBoxData() {
    static const unsigned int gost_sbox_1[256]
        __asm__("_ZZN9CGostHash11InitBoxDataEvE3C.0") = {
#include "gosthash_sbox1.inc"
    };
    static const unsigned int gost_sbox_2[256]
        __asm__("_ZZN9CGostHash11InitBoxDataEvE3C.2") = {
#include "gosthash_sbox2.inc"
    };
    static const unsigned int gost_sbox_3[256]
        __asm__("_ZZN9CGostHash11InitBoxDataEvE3C.4") = {
#include "gosthash_sbox3.inc"
    };
    static const unsigned int gost_sbox_4[256]
        __asm__("_ZZN9CGostHash11InitBoxDataEvE3C.6") = {
#include "gosthash_sbox4.inc"
    };

    if (!m_psbox_1) {
        m_psbox_1 = new unsigned int[0x104];
        memset(m_psbox_1, 0, 0x410);
    }
    memcpy(m_psbox_1, gost_sbox_1, sizeof(gost_sbox_1));
    if (!m_psbox_2) {
        m_psbox_2 = new unsigned int[0x104];
        memset(m_psbox_2, 0, 0x410);
    }
    memcpy(m_psbox_2, gost_sbox_2, sizeof(gost_sbox_2));
    if (!m_psbox_3) {
        m_psbox_3 = new unsigned int[0x104];
        memset(m_psbox_3, 0, 0x410);
    }
    memcpy(m_psbox_3, gost_sbox_3, sizeof(gost_sbox_3));
    if (!m_psbox_4) {
        m_psbox_4 = new unsigned int[0x104];
        memset(m_psbox_4, 0, 0x410);
    }
    memcpy(m_psbox_4, gost_sbox_4, sizeof(gost_sbox_4));
}

#define GOST_ENCRYPT_ROUND(k1, k2) \
    t = (k1) + r; \
    l ^= m_psbox_1[t & 0xff] ^ m_psbox_2[(t >> 8) & 0xff] ^ \
         m_psbox_3[(t >> 16) & 0xff] ^ m_psbox_4[t >> 24]; \
    t = (k2) + l; \
    r ^= m_psbox_1[t & 0xff] ^ m_psbox_2[(t >> 8) & 0xff] ^ \
         m_psbox_3[(t >> 16) & 0xff] ^ m_psbox_4[t >> 24];

#define GOST_ENCRYPT(key) \
    GOST_ENCRYPT_ROUND(key[0], key[1]) \
    GOST_ENCRYPT_ROUND(key[2], key[3]) \
    GOST_ENCRYPT_ROUND(key[4], key[5]) \
    GOST_ENCRYPT_ROUND(key[6], key[7]) \
    GOST_ENCRYPT_ROUND(key[0], key[1]) \
    GOST_ENCRYPT_ROUND(key[2], key[3]) \
    GOST_ENCRYPT_ROUND(key[4], key[5]) \
    GOST_ENCRYPT_ROUND(key[6], key[7]) \
    GOST_ENCRYPT_ROUND(key[0], key[1]) \
    GOST_ENCRYPT_ROUND(key[2], key[3]) \
    GOST_ENCRYPT_ROUND(key[4], key[5]) \
    GOST_ENCRYPT_ROUND(key[6], key[7]) \
    GOST_ENCRYPT_ROUND(key[7], key[6]) \
    GOST_ENCRYPT_ROUND(key[5], key[4]) \
    GOST_ENCRYPT_ROUND(key[3], key[2]) \
    GOST_ENCRYPT_ROUND(key[1], key[0]) \
    t = r; \
    r = l; \
    l = t;

void CGostHash::gosthash_compress(unsigned int *h, unsigned int *m) {
    int i;
    unsigned int l, r, t, key[8], u[8], v[8], w[8], s[8];

    memcpy(u, h, sizeof(u));
    memcpy(v, m, sizeof(u));

    for (i = 0; i < 8; i += 2) {
        w[0] = u[0] ^ v[0];
        w[1] = u[1] ^ v[1];
        w[2] = u[2] ^ v[2];
        w[3] = u[3] ^ v[3];
        w[4] = u[4] ^ v[4];
        w[5] = u[5] ^ v[5];
        w[6] = u[6] ^ v[6];
        w[7] = u[7] ^ v[7];

        key[0] = (w[0]  & 0x000000ff) | ((w[2] & 0x000000ff) << 8) |
                 ((w[4] & 0x000000ff) << 16) | ((w[6] & 0x000000ff) << 24);
        key[1] = ((w[0] & 0x0000ff00) >> 8)  | (w[2]  & 0x0000ff00) |
                 ((w[4] & 0x0000ff00) << 8) | ((w[6] & 0x0000ff00) << 16);
        key[2] = ((w[0] & 0x00ff0000) >> 16) | ((w[2] & 0x00ff0000) >> 8) |
                 (w[4] & 0x00ff0000) | ((w[6] & 0x00ff0000) << 8);
        key[3] = ((w[0] & 0xff000000) >> 24) | ((w[2] & 0xff000000) >> 16) |
                 ((w[4] & 0xff000000) >> 8) | (w[6] & 0xff000000);
        key[4] = (w[1] & 0x000000ff) | ((w[3] & 0x000000ff) << 8) |
                 ((w[5] & 0x000000ff) << 16) | ((w[7] & 0x000000ff) << 24);
        key[5] = ((w[1] & 0x0000ff00) >> 8) | (w[3]  & 0x0000ff00) |
                 ((w[5] & 0x0000ff00) << 8) | ((w[7] & 0x0000ff00) << 16);
        key[6] = ((w[1] & 0x00ff0000) >> 16) | ((w[3] & 0x00ff0000) >> 8) |
                 (w[5] & 0x00ff0000) | ((w[7] & 0x00ff0000) << 8);
        key[7] = ((w[1] & 0xff000000) >> 24) | ((w[3] & 0xff000000) >> 16) |
                 ((w[5] & 0xff000000) >> 8) | (w[7] & 0xff000000);

        r = h[i];
        l = h[i + 1];
        GOST_ENCRYPT(key);

        s[i] = r;
        s[i + 1] = l;

        if (i == 6)
            break;

        l = u[0] ^ u[2];
        r = u[1] ^ u[3];
        u[0] = u[2];
        u[1] = u[3];
        u[2] = u[4];
        u[3] = u[5];
        u[4] = u[6];
        u[5] = u[7];
        u[6] = l;
        u[7] = r;

        if (i == 2) {
            u[0] ^= 0xff00ff00;
            u[1] ^= 0xff00ff00;
            u[2] ^= 0x00ff00ff;
            u[3] ^= 0x00ff00ff;
            u[4] ^= 0x00ffff00;
            u[5] ^= 0xff0000ff;
            u[6] ^= 0x000000ff;
            u[7] ^= 0xff00ffff;
        }

        l = v[0];
        r = v[2];
        v[0] = v[4];
        v[2] = v[6];
        v[4] = l ^ r;
        v[6] = v[0] ^ r;
        l = v[1];
        r = v[3];
        v[1] = v[5];
        v[3] = v[7];
        v[5] = l ^ r;
        v[7] = v[1] ^ r;
    }

    u[0] = m[0] ^ s[6];
    u[1] = m[1] ^ s[7];
    u[2] = m[2] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff) ^
           (s[1] & 0xffff) ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[6] ^ (s[6] << 16) ^
           (s[7] & 0xffff0000) ^ (s[7] >> 16);
    u[3] = m[3] ^ (s[0] & 0xffff) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^
           (s[1] << 16) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^
           (s[3] << 16) ^ s[6] ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^
           (s[7] << 16) ^ (s[7] >> 16);
    u[4] = m[4] ^
           (s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[0] >> 16) ^
           (s[1] & 0xffff0000) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^
           (s[3] << 16) ^ (s[3] >> 16) ^ (s[4] << 16) ^ (s[6] << 16) ^
           (s[6] >> 16) ^ (s[7] & 0xffff) ^ (s[7] << 16) ^ (s[7] >> 16);
    u[5] = m[5] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff0000) ^
           (s[1] & 0xffff) ^ s[2] ^ (s[2] >> 16) ^ (s[3] << 16) ^ (s[3] >> 16) ^
           (s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[6] << 16) ^
           (s[6] >> 16) ^ (s[7] & 0xffff0000) ^ (s[7] << 16) ^ (s[7] >> 16);
    u[6] = m[6] ^ s[0] ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[3] ^ (s[3] >> 16) ^
           (s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[5] >> 16) ^ s[6] ^
           (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] << 16);
    u[7] = m[7] ^ (s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^
           (s[1] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ s[4] ^ (s[4] >> 16) ^
           (s[5] << 16) ^ (s[5] >> 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^
           (s[7] << 16) ^ (s[7] >> 16);

    v[0] = h[0] ^ (u[1] << 16) ^ (u[0] >> 16);
    v[1] = h[1] ^ (u[2] << 16) ^ (u[1] >> 16);
    v[2] = h[2] ^ (u[3] << 16) ^ (u[2] >> 16);
    v[3] = h[3] ^ (u[4] << 16) ^ (u[3] >> 16);
    v[4] = h[4] ^ (u[5] << 16) ^ (u[4] >> 16);
    v[5] = h[5] ^ (u[6] << 16) ^ (u[5] >> 16);
    v[6] = h[6] ^ (u[7] << 16) ^ (u[6] >> 16);
    v[7] = h[7] ^ (u[0] & 0xffff0000) ^ (u[0] << 16) ^ (u[7] >> 16) ^
           (u[1] & 0xffff0000) ^ (u[1] << 16) ^ (u[6] << 16) ^ (u[7] & 0xffff0000);

    h[0] = (v[0] & 0xffff0000) ^ (v[0] << 16) ^ (v[0] >> 16) ^ (v[1] >> 16) ^
           (v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ (v[4] << 16) ^
           (v[5] >> 16) ^ v[5] ^ (v[6] >> 16) ^ (v[7] << 16) ^ (v[7] >> 16) ^
           (v[7] & 0xffff);
    h[1] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^ (v[1] & 0xffff) ^
           v[2] ^ (v[2] >> 16) ^ (v[3] << 16) ^ (v[4] >> 16) ^ (v[5] << 16) ^
           (v[6] << 16) ^ v[6] ^ (v[7] & 0xffff0000) ^ (v[7] >> 16);
    h[2] = (v[0] & 0xffff) ^ (v[0] << 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^
           (v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^
           (v[5] >> 16) ^ v[6] ^ (v[6] >> 16) ^ (v[7] & 0xffff) ^ (v[7] << 16) ^
           (v[7] >> 16);
    h[3] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^
           (v[1] & 0xffff0000) ^ (v[1] >> 16) ^ (v[2] << 16) ^ (v[2] >> 16) ^ v[2] ^
           (v[3] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^
           (v[7] & 0xffff) ^ (v[7] >> 16);
    h[4] = (v[0] >> 16) ^ (v[1] << 16) ^ v[1] ^ (v[2] >> 16) ^ v[2] ^
           (v[3] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ (v[5] >> 16) ^
           v[5] ^ (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16);
    h[5] = (v[0] << 16) ^ (v[0] & 0xffff0000) ^ (v[1] << 16) ^ (v[1] >> 16) ^
           (v[1] & 0xffff0000) ^ (v[2] << 16) ^ v[2] ^ (v[3] >> 16) ^ v[3] ^
           (v[4] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^
           (v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ (v[7] >> 16) ^ (v[7] & 0xffff0000);
    h[6] = v[0] ^ v[2] ^ (v[2] >> 16) ^ v[3] ^ (v[3] << 16) ^ v[4] ^
           (v[4] >> 16) ^ (v[5] << 16) ^ (v[5] >> 16) ^ v[5] ^ (v[6] << 16) ^
           (v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ v[7];
    h[7] = v[0] ^ (v[0] >> 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^ (v[2] << 16) ^
           (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ v[4] ^ (v[5] >> 16) ^ v[5] ^
           (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16) ^ v[7];
}

void CGostHash::gosthash_reset() {
    memset(m_pctx->sum, 0, 32);
    memset(m_pctx->hash, 0, 32);
    memset(m_pctx->len, 0, 32);
    memset(m_pctx->partial, 0, 32);
    m_pctx->partial_bytes = 0;
}

void CGostHash::gosthash_bytes(unsigned char *buf, unsigned int bits) {
    int i, j;
    unsigned int a, b, c, m[8];

    j = 0;
    c = 0;
    for (i = 0; i < 8; i++) {
        a = ((unsigned int)buf[j]) |
            (((unsigned int)buf[j + 1]) << 8) |
            (((unsigned int)buf[j + 2]) << 16) |
            (((unsigned int)buf[j + 3]) << 24);
        j += 4;
        m[i] = a;
        b = m_pctx->sum[i];
        c = a + c + m_pctx->sum[i];
        m_pctx->sum[i] = c;
        c = ((c < a) || (c < b)) ? 1 : 0;
    }

    gosthash_compress(m_pctx->hash, m);

    m_pctx->len[0] += bits;
    if (m_pctx->len[0] < bits)
        m_pctx->len[1]++;
}

void CGostHash::gosthash_update(unsigned char *buf, unsigned int len) {
    unsigned int i, j;

    i = m_pctx->partial_bytes;
    j = 0;
    while (i < 32 && j < len)
        m_pctx->partial[i++] = buf[j++];

    if (i < 32) {
        m_pctx->partial_bytes = i;
        return;
    }
    gosthash_bytes(m_pctx->partial, 256);

    while ((j + 32) < len) {
        gosthash_bytes(&buf[j], 256);
        j += 32;
    }

    i = 0;
    while (j < len)
        m_pctx->partial[i++] = buf[j++];
    m_pctx->partial_bytes = i;
}

void CGostHash::gosthash_final(unsigned char *digest) {
    int i, j;
    unsigned int a;

    if (m_pctx->partial_bytes > 0) {
        memset(&m_pctx->partial[m_pctx->partial_bytes], 0,
               32 - m_pctx->partial_bytes);
        gosthash_bytes(m_pctx->partial, m_pctx->partial_bytes << 3);
    }

    gosthash_compress(m_pctx->hash, m_pctx->len);
    gosthash_compress(m_pctx->hash, m_pctx->sum);

    j = 0;
    for (i = 0; i < 8; i++) {
        a = m_pctx->hash[i];
        digest[j] = (unsigned char)a;
        digest[j + 1] = (unsigned char)(a >> 8);
        digest[j + 2] = (unsigned char)(a >> 16);
        digest[j + 3] = (unsigned char)(a >> 24);
        j += 4;
    }
}

CGostHash::CGostHash() {
    m_psbox_1 = new unsigned int[256];
    m_psbox_2 = new unsigned int[256];
    m_psbox_3 = new unsigned int[256];
    m_psbox_4 = new unsigned int[256];
    m_pctx = new gost_ctx;
    InitBoxData();
    gosthash_reset();
}

CGostHash::~CGostHash() {
    if (m_psbox_1) { delete[] m_psbox_1; m_psbox_1 = 0; }
    if (m_psbox_2) { delete[] m_psbox_2; m_psbox_2 = 0; }
    if (m_psbox_3) { delete[] m_psbox_3; m_psbox_3 = 0; }
    if (m_psbox_4) { delete[] m_psbox_4; m_psbox_4 = 0; }
    if (m_pctx) { delete m_pctx; m_pctx = 0; }
}
