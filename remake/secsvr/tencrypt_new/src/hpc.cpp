/* hpc.cpp -- Hasty Pudding Cipher（AC 源码集 HPC/hpc.c 公版结构）
   Init：固定分配 l_key(0x8f0)/spice(0x40)/p119/e19/r220(各 8 字节)，
   p119/e19/r220 填 AC 常量；setKey/encrypt/decrypt 与 AC set_key/encrypt/decrypt
   一致（io_swap=字节交换，BYTE_SWAP 路径）。
   Hpc 包装层：setKey(key, 0x20) + 16 字节块循环；余数交
   CreateTsLocal()->Seattos(key, benc, buf+processed, remaining)。
   验证：uni_call 调二进制 setKey/encrypt/decrypt 逐字节一致（随机多组）。 */

#include "inc/hpc.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned int u4byte;
typedef unsigned char u1byte;
typedef u4byte u8byte[2];

static inline u4byte io_swap(u4byte x) {
    return ((x & 0xff) << 24) | ((x & 0xff00) << 8) |
           ((x >> 8) & 0xff00) | ((x >> 24) & 0xff);
}

static inline void xor_eq64(u4byte *x, const u4byte *y) {
    x[0] ^= y[0];
    x[1] ^= y[1];
}
static inline void and_eq64(u4byte *x, const u4byte *y) {
    x[0] &= y[0];
    x[1] &= y[1];
}
static inline void or_eq64(u4byte *x, const u4byte *y) {
    x[0] |= y[0];
    x[1] |= y[1];
}
static inline void add_eq(u4byte *x, const u4byte *y) {
    x[1] += y[1] + (((x[0] += y[0]) < y[0]) ? 1 : 0);
}
static inline void sub_eq(u4byte *x, const u4byte *y) {
    u4byte xs = x[0];
    x[1] -= y[1] + (((x[0] -= y[0]) > xs) ? 1 : 0);
}
static inline void lsh_eq(u4byte *x, u4byte n) {
    if (n > 31) {
        x[1] = x[0] << (n & 31);
        x[0] = 0;
    } else if (n > 0) {
        x[1] = (x[1] << n) | (x[0] >> (-(int)n & 31));
        x[0] = x[0] << n;
    }
}
static inline void rsh_eq(u4byte *x, u4byte n) {
    if (n > 31) {
        x[0] = x[1] >> (n & 31);
        x[1] = 0;
    } else if (n > 0) {
        x[0] = (x[0] >> n) | (x[1] << (-(int)n & 31));
        x[1] = x[1] >> n;
    }
}

#define lo(x) ((x) & 0x0000ffff)
#define hi(x) ((x) >> 16)

// mangled: _ZN4CHpc7mult_64EPjPKjS2_
void CHpc::mult_64(u4byte *r, const u4byte *x, const u4byte *y) {
    u4byte x0, x1, x2, x3, y0, y1, y2, y3, t0, t1, t2, t3, c;

    x0 = lo(x[0]); x1 = hi(x[0]); x2 = lo(x[1]); x3 = hi(x[1]);
    y0 = lo(y[0]); y1 = hi(y[0]); y2 = lo(y[1]); y3 = hi(y[1]);

    t0 = x0 * y0; r[0] = lo(t0); c = hi(t0);

    t0 = x0 * y1; t1 = x1 * y0; c += lo(t0) + lo(t1);
    r[0] += (c << 16); c = hi(c) + hi(t0) + hi(t1);

    t0 = x0 * y2; t1 = x1 * y1; t2 = x2 * y0;
    c += lo(t0) + lo(t1) + lo(t2); r[1] = lo(c);
    c = hi(c) + hi(t0) + hi(t1) + hi(t2);

    t0 = x0 * y3; t1 = x1 * y2; t2 = x2 * y1; t3 = x3 * y0;
    c += lo(t0) + lo(t1) + lo(t2) + lo(t3); r[1] += (c << 16);
}

// mangled: _ZN4CHpc6setKeyEPKjj
void CHpc::setKey(const u4byte *in_key, u4byte key_len) {
    u8byte s[8], t;
    u4byte i;
    u4byte j;

    l_key[0] = p119[0] + 3;
    l_key[1] = p119[1];
    t[0] = key_len;
    t[1] = 0;
    mult_64(l_key + 2, e19, t);
    l_key[4] = (r220[0] << 3) | (r220[1] >> 29);
    l_key[5] = (r220[1] << 3) | (r220[0] >> 29);

    for (i = 3; i < 256; ++i) {
        t[0] = l_key[2 * (i - 3)];
        t[1] = l_key[2 * (i - 3) + 1];
        lsh_eq(t, 41);
        l_key[2 * i] = l_key[2 * (i - 3)];
        l_key[2 * i + 1] = l_key[2 * (i - 3) + 1];
        rsh_eq(l_key + 2 * i, 23);
        or_eq64(l_key + 2 * i, t);
        xor_eq64(l_key + 2 * i, l_key + 2 * (i - 2));
        add_eq(l_key + 2 * i, l_key + 2 * (i - 1));
    }

    l_key[1] ^= io_swap(in_key[0]);
    l_key[0] ^= io_swap(in_key[1]);
    l_key[3] ^= io_swap(in_key[2]);
    l_key[2] ^= io_swap(in_key[3]);

    if (key_len > 128) {
        l_key[5] ^= io_swap(in_key[4]);
        l_key[4] ^= io_swap(in_key[5]);
    }

    if (key_len > 192) {
        l_key[7] ^= io_swap(in_key[6]);
        l_key[6] ^= io_swap(in_key[7]);
    }

    for (i = 0; i < 8; ++i) {
        s[i][0] = l_key[2 * (248 + i)];
        s[i][1] = l_key[2 * (248 + i) + 1];
    }

    for (j = 0; j < 3; ++j)
        for (i = 0; i < 256; ++i) {
            t[0] = l_key[2 * i];
            t[1] = l_key[2 * i + 1];
            xor_eq64(t, l_key + 2 * ((i + 83) & 255));
            add_eq(t, l_key + 2 * (s[0][0] & 255));
            xor_eq64(s[0], t);
            add_eq(s[1], s[0]);
            xor_eq64(s[3], s[2]);
            sub_eq(s[5], s[4]);
            xor_eq64(s[7], s[6]);
            t[0] = s[0][0];
            t[1] = s[0][1];
            rsh_eq(t, 13);
            add_eq(s[3], t);
            t[0] = s[1][0];
            t[1] = s[1][1];
            lsh_eq(t, 11);
            xor_eq64(s[4], t);
            t[0] = s[3][0];
            t[1] = s[3][1];
            lsh_eq(t, s[1][0] & 31);
            xor_eq64(s[5], t);
            t[0] = s[2][0];
            t[1] = s[2][1];
            rsh_eq(t, 17);
            add_eq(s[6], t);
            t[0] = s[3][0];
            t[1] = s[3][1];
            add_eq(t, s[4]);
            or_eq64(s[7], t);
            sub_eq(s[2], s[5]);
            t[0] = s[6][0] ^ i;
            t[1] = s[6][1];
            sub_eq(s[0], t);
            t[0] = s[5][0];
            t[1] = s[5][1];
            add_eq(t, p119);
            xor_eq64(s[1], t);
            t[0] = s[7][0];
            t[1] = s[7][1];
            rsh_eq(t, j);
            add_eq(s[2], t);
            xor_eq64(s[2], s[1]);
            sub_eq(s[4], s[3]);
            xor_eq64(s[6], s[5]);
            add_eq(s[0], s[7]);
            l_key[2 * i] = s[2][0];
            l_key[2 * i + 1] = s[2][1];
            add_eq(l_key + 2 * i, s[6]);
        }

    for (i = 0; i < 30; ++i) {
        l_key[2 * (256 + i)] = l_key[2 * i];
        l_key[2 * (256 + i) + 1] = l_key[2 * i + 1];
    }
}

// mangled: _ZN4CHpc7encryptEPj
void CHpc::encrypt(u4byte *in_blk) {
    u8byte s0, s1, k, kk, t;
    u4byte tt;
    int i;

    s0[1] = io_swap(in_blk[0]);
    s0[0] = io_swap(in_blk[1]);
    s1[1] = io_swap(in_blk[2]);
    s1[0] = io_swap(in_blk[3]);

    add_eq(s0, l_key + 2 * 128);
    add_eq(s1, l_key + 2 * 129);

    for (i = 0; i < 8; ++i) {
        tt = s0[0] & 255;
        k[0] = l_key[2 * tt];
        k[1] = l_key[2 * tt + 1];
        add_eq(s1, k);
        lsh_eq(k, 8);
        xor_eq64(s0, k);
        xor_eq64(s1, s0);
        t[0] = s1[0];
        t[1] = s1[1];
        rsh_eq(t, 11);
        sub_eq(s0, t);
        t[0] = s1[0];
        t[1] = s1[1];
        lsh_eq(t, 2);
        xor_eq64(s0, t);
        sub_eq(s0, spice + 2 * (i ^ 4));
        t[0] = s0[0];
        t[1] = s0[1];
        lsh_eq(t, 32);
        kk[0] = p119[0] + 128;
        kk[1] = p119[1];
        xor_eq64(t, kk);
        add_eq(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 17);
        xor_eq64(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 34);
        xor_eq64(s0, t);
        t[0] = spice[2 * i];
        t[1] = spice[2 * i + 1];
        xor_eq64(s0, t);
        lsh_eq(t, 5);
        add_eq(s0, t);
        t[0] = spice[2 * i];
        t[1] = spice[2 * i + 1];
        rsh_eq(t, 4);
        add_eq(s1, t);
        xor_eq64(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        lsh_eq(t, 22 + (s0[0] & 31));
        add_eq(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 23);
        xor_eq64(s0, t);
        sub_eq(s0, spice + 2 * (i ^ 7));
        tt = s0[0] & 255;
        k[0] = l_key[2 * tt];
        k[1] = l_key[2 * tt + 1];
        tt += 3 * i + 1;
        kk[0] = l_key[2 * tt];
        kk[1] = l_key[2 * tt + 1];
        xor_eq64(s1, k);
        t[0] = kk[0];
        t[1] = kk[1];
        lsh_eq(t, 8);
        xor_eq64(s0, t);
        xor_eq64(kk, k);
        t[0] = kk[0];
        t[1] = kk[1];
        rsh_eq(t, 5);
        add_eq(s1, t);
        t[0] = kk[0];
        t[1] = kk[1];
        lsh_eq(t, 12);
        sub_eq(s0, t);
        kk[0] &= ~255;
        xor_eq64(s0, kk);
        add_eq(s1, s0);
        t[0] = s1[0];
        t[1] = s1[1];
        lsh_eq(t, 3);
        add_eq(s0, t);
        xor_eq64(s0, spice + 2 * (i ^ 2));
        add_eq(s0, l_key + 2 * (144 + i));
        t[0] = s0[0];
        t[1] = s0[1];
        lsh_eq(t, 22);
        add_eq(s0, t);
        t[0] = s1[0];
        t[1] = s1[1];
        rsh_eq(t, 4);
        xor_eq64(s0, t);
        add_eq(s0, spice + 2 * (i ^ 1));
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 33 + i);
        xor_eq64(s0, t);
    }

    add_eq(s0, l_key + 2 * 136);
    add_eq(s1, l_key + 2 * 137);

    in_blk[0] = io_swap(s0[1]);
    in_blk[1] = io_swap(s0[0]);
    in_blk[2] = io_swap(s1[1]);
    in_blk[3] = io_swap(s1[0]);
}

// mangled: _ZN4CHpc7decryptEPj
void CHpc::decrypt(u4byte *in_blk) {
    u8byte s0, s1, k, kk, t;
    u4byte tt;
    int i;

    s0[1] = io_swap(in_blk[0]);
    s0[0] = io_swap(in_blk[1]);
    s1[1] = io_swap(in_blk[2]);
    s1[0] = io_swap(in_blk[3]);

    sub_eq(s0, l_key + 2 * 136);
    sub_eq(s1, l_key + 2 * 137);

    for (i = 7; i >= 0; --i) {
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 33 + i);
        xor_eq64(s0, t);
        sub_eq(s0, spice + 2 * (i ^ 1));
        t[0] = s1[0];
        t[1] = s1[1];
        rsh_eq(t, 4);
        xor_eq64(s0, t);
        k[0] = s0[0];
        k[1] = s0[1];
        lsh_eq(k, 22);
        t[0] = s0[0];
        t[1] = s0[1];
        sub_eq(t, k);
        lsh_eq(t, 22);
        sub_eq(s0, t);
        sub_eq(s0, l_key + 2 * (144 + i));
        xor_eq64(s0, spice + 2 * (i ^ 2));
        t[0] = s1[0];
        t[1] = s1[1];
        lsh_eq(t, 3);
        sub_eq(s0, t);
        sub_eq(s1, s0);
        tt = s0[0] & 255;
        k[0] = l_key[2 * tt];
        k[1] = l_key[2 * tt + 1];
        tt += 3 * i + 1;
        kk[0] = l_key[2 * tt];
        kk[1] = l_key[2 * tt + 1];
        xor_eq64(kk, k);
        t[0] = kk[0] & ~255;
        t[1] = kk[1];
        xor_eq64(s0, t);
        t[0] = kk[0];
        t[1] = kk[1];
        lsh_eq(t, 12);
        add_eq(s0, t);
        t[0] = kk[0];
        t[1] = kk[1];
        rsh_eq(t, 5);
        sub_eq(s1, t);
        kk[0] = l_key[2 * tt];
        kk[1] = l_key[2 * tt + 1];
        lsh_eq(kk, 8);
        xor_eq64(s0, kk);
        xor_eq64(s1, k);
        add_eq(s0, spice + 2 * (i ^ 7));
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 23);
        xor_eq64(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 46);
        xor_eq64(s0, t);
        tt = 22 + (s0[0] & 31);
        t[0] = s0[0];
        t[1] = s0[1];
        lsh_eq(t, tt);
        kk[0] = s0[0];
        kk[1] = s0[1];
        sub_eq(kk, t);
        lsh_eq(kk, tt);
        sub_eq(s0, kk);
        t[0] = kk[0] = spice[2 * i];
        t[1] = kk[1] = spice[2 * i + 1];
        rsh_eq(kk, 4);
        xor_eq64(s0, kk);
        sub_eq(s1, kk);
        k[0] = t[0];
        k[1] = t[1];
        lsh_eq(k, 5);
        sub_eq(s0, k);
        xor_eq64(s0, t);
        t[0] = s0[0];
        t[1] = s0[1];
        rsh_eq(t, 17);
        xor_eq64(s0, t);
        t[0] = p119[0] + 128;
        t[1] = p119[1];
        k[0] = s0[0];
        k[1] = s0[1];
        sub_eq(k, t);
        lsh_eq(k, 32);
        xor_eq64(t, k);
        sub_eq(s0, t);
        add_eq(s0, spice + 2 * (i ^ 4));
        t[0] = s1[0];
        t[1] = s1[1];
        lsh_eq(t, 2);
        xor_eq64(s0, t);
        t[0] = s1[0];
        t[1] = s1[1];
        rsh_eq(t, 11);
        add_eq(s0, t);
        xor_eq64(s1, s0);
        tt = s0[0] & 255;
        k[0] = l_key[2 * tt];
        k[1] = l_key[2 * tt + 1];
        t[0] = k[0];
        t[1] = k[1];
        lsh_eq(t, 8);
        xor_eq64(s0, t);
        sub_eq(s1, k);
    }

    sub_eq(s0, l_key + 2 * 128);
    sub_eq(s1, l_key + 2 * 129);

    in_blk[0] = io_swap(s0[1]);
    in_blk[1] = io_swap(s0[0]);
    in_blk[2] = io_swap(s1[1]);
    in_blk[3] = io_swap(s1[0]);
}

// mangled: _ZN4CHpc4InitEPvS0_
void CHpc::Init(void *arg0, void *arg1) {
    l_key = new u4byte[0x8f0 / 4];
    memset(l_key, 0, 0x8f0);
    spice = new u4byte[0x40 / 4];
    memset(spice, 0, 0x40);
    p119 = new u4byte[2];
    p119[0] = 0xa23249d6;
    p119[1] = 0x2b992ddf;
    e19 = new u4byte[2];
    e19[0] = 0xc0b36173;
    e19[1] = 0x25b946eb;
    r220 = new u4byte[2];
    r220[0] = 0xe9e17158;
    r220[1] = 0xc442f56b;
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN4CHpc6UnInitEv
void CHpc::UnInit() {
    if (l_key) {
        delete[] l_key;
        l_key = 0;
    }
    if (spice) {
        delete[] spice;
        spice = 0;
    }
    if (p119) {
        delete[] p119;
        p119 = 0;
    }
    if (e19) {
        delete[] e19;
        e19 = 0;
    }
    if (r220) {
        delete[] r220;
        r220 = 0;
    }
}

// mangled: _ZN4CHpcC1Ev / _ZN4CHpcC2Ev
CHpc::CHpc() {
    Init(0, 0);
}

// mangled: _ZN4CHpcD1Ev / _ZN4CHpcD2Ev
CHpc::~CHpc() {
    UnInit();
}

// mangled: _ZN4CHpc3HpcEPhbS0_j
void CHpc::Hpc(u1byte *key, bool benc, u1byte *buf, u4byte buflen) {
    u4byte blocks;
    u4byte i;

    if (!buflen)
        return;

    setKey((const u4byte *)key, 0x20);

    blocks = buflen >> 4;
    for (i = 0; i < blocks; i++) {
        if (benc)
            encrypt((u4byte *)(buf + 16 * i));
        else
            decrypt((u4byte *)(buf + 16 * i));
    }

    CreateTsLocal()->Seattos(key, benc, buf + blocks * 16,
                             buflen - blocks * 16);
}

// mangled: _Z7TenHpcPhbS_j
void TenHpc(unsigned char *key, bool benc, unsigned char *buf,
            unsigned int buflen) {
    CHpc hpc;

    hpc.Hpc(key, benc, buf, buflen);
}
