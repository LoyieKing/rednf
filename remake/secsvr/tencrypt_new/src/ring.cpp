/* ring.cpp -- Ring 自定义块密码（由二进制反汇编还原）
   Init：分配 Ringbbit[8]={0x81,0x40,0x22,0x10,0x0a,0x04,0x02,0x01}、
   RingIP[64]（位置换表）。
   SetRingKey：out 按 RingIP 对密钥 64 位比特置换；table[i]=i 后以 key 前
   8 字节组成 32 位种子（dword0^dword1），
   MSVC rand LCG 迭代 256 次交换 table[i-1] 与 table[(seed>>16)&0xff]。
   RingEncrypt/Decrypt：8 字节块，逐字节 buf[i] ^= rotr(k8[i],i) ^ T[i]
   （T={0x51,0xf3,0x4c,0xd7,0x89,0xab,0x20,0x6e}，加解密互逆）。
   Ring 包装：加密前/解密后 buf[i] ^= table[i]（i<blocks*8）+ TsLocal 余数。
   验证：uni_call 调二进制 SetRingKey/RingEncrypt/RingDecrypt/Ring 逐字节一致。 */

#include "inc/ring.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char ring_bbit[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};
static const unsigned char ring_ip[64] = {
    0x07, 0x18, 0x10, 0x0b, 0x2b, 0x23, 0x0f, 0x17,
    0x3c, 0x05, 0x36, 0x2e, 0x01, 0x1e, 0x04, 0x0e,
    0x3b, 0x30, 0x1f, 0x0c, 0x16, 0x1b, 0x03, 0x00,
    0x2c, 0x24, 0x38, 0x33, 0x27, 0x39, 0x31, 0x29,
    0x37, 0x3f, 0x1c, 0x13, 0x3d, 0x08, 0x3e, 0x0d,
    0x06, 0x35, 0x2d, 0x25, 0x26, 0x1d, 0x15, 0x34,
    0x22, 0x1a, 0x28, 0x20, 0x12, 0x21, 0x19, 0x2f,
    0x11, 0x09, 0x3a, 0x32, 0x2a, 0x0a, 0x02, 0x14
};

static const unsigned char ring_t[8] = {
    0x51, 0xf3, 0x4c, 0xd7, 0x89, 0xab, 0x20, 0x6e
};

// mangled: _ZN5CRing4InitEPvS0_
void CRing::Init(void *arg0, void *arg1) {
    Ringbbit = new unsigned char[8];
    memmove(Ringbbit, ring_bbit, 8);
    RingIP = new unsigned char[0x40];
    memset(RingIP, 0, 0x40);
    memmove(RingIP, ring_ip, 0x40);
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN5CRing6UnInitEv
void CRing::UnInit() {
    if (Ringbbit) {
        delete[] Ringbbit;
        Ringbbit = 0;
    }
    if (RingIP) {
        delete[] RingIP;
        RingIP = 0;
    }
}

// mangled: _ZN5CRingC1Ev / _ZN5CRingC2Ev
CRing::CRing() {
    Init(0, 0);
}

// mangled: _ZN5CRingD1Ev / _ZN5CRingD2Ev
CRing::~CRing() {
    UnInit();
}

// mangled: _ZN5CRing10SetRingKeyEPhS0_S0_
void CRing::SetRingKey(unsigned char *key, unsigned char *out,
                       unsigned char *table) {
    unsigned int seed;
    unsigned int i;

    memset(out, 0, 8);
    for (i = 0; i < 64; i++) {
        unsigned int ip = RingIP[i];
        unsigned char bit = Ringbbit[ip & 7];
        unsigned char byte = key[ip >> 3];
        unsigned char v = (byte & bit) ? 1 : 0;

        out[i >> 3] |= (unsigned char)(v << (i & 7));
    }

    for (i = 0; i < 256; i++)
        table[i] = (unsigned char)i;

    seed = (((unsigned int)key[0] << 24) | ((unsigned int)key[1] << 16) |
            ((unsigned int)key[2] << 8) | key[3]) ^
           (((unsigned int)key[4] << 24) | ((unsigned int)key[5] << 16) |
            ((unsigned int)key[6] << 8) | key[7]);

    for (i = 1; i <= 256; i++) {
        unsigned char temp;
        unsigned int idx;

        seed = seed * 0x343fd + 0x269ec3;
        idx = (seed >> 16) & 0xff;
        temp = table[i - 1];
        table[i - 1] = table[idx];
        table[idx] = temp;
    }
}

// mangled: _ZN5CRing11RingEncryptEPhS0_
void CRing::RingEncrypt(unsigned char *key, unsigned char *buf) {
    unsigned int i;

    for (i = 0; i < 8; i++) {
        unsigned char v = key[i];
        unsigned char r = (unsigned char)((v >> i) | (v << (8 - i)));

        buf[i] = (unsigned char)((r ^ buf[i]) ^ ring_t[i]);
    }
}

// mangled: _ZN5CRing11RingDecryptEPhS0_
void CRing::RingDecrypt(unsigned char *key, unsigned char *buf) {
    int i;

    for (i = 7; i >= 0; i--) {
        unsigned char v = key[i];
        unsigned char r = (unsigned char)((v >> i) | (v << (8 - i)));

        buf[i] = (unsigned char)((buf[i] ^ ring_t[i]) ^ r);
    }
}

// mangled: _ZN5CRing4RingEPhbS0_j
void CRing::Ring(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    unsigned char k8[8];
    unsigned char table[256];
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;

    SetRingKey(key, k8, table);

    blocks = buflen >> 3;
    if (benc) {
        for (i = 0; i < blocks * 8; i++)
            buf[i] = (unsigned char)(buf[i] ^ table[i]);
    }

    for (i = 0; i < blocks; i++) {
        if (benc)
            RingEncrypt(k8, buf + 8 * i);
        else
            RingDecrypt(k8, buf + 8 * i);
    }

    if (!benc) {
        for (i = 0; i < blocks * 8; i++)
            buf[i] = (unsigned char)(buf[i] ^ table[i]);
    }

    CreateTsLocal()->Seattos(key, benc, buf + blocks * 8,
                             buflen - blocks * 8);
}

// mangled: _Z7TenRingPhbS_j
void TenRing(unsigned char *buf, bool benc, unsigned char *key,
             unsigned int buflen) {
    CRing ring;

    ring.Ring(key, benc, buf, buflen);
}
