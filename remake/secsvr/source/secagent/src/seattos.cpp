/* seattos.cpp -- Seattos 自定义流式密码（由二进制反汇编还原）
   Init：分配 seattosbbit[8]={0x81,0x40,0x22,0x10,0x0a,0x04,0x02,0x01}、
   seattosIP[64]（IP 位置换表）。
   SetseattosKey(key, out)：对 64 位，out 第 i 位 = key 第 IP[i] 位
   （out[i>>3] 的 (i&7) 位 ← key[IP[i]>>3] 的 (IP[i]&7) 位）。
   Seattos：逐字节 v1 = buf[i]^t16[i&15]^key[i&7]；
   benc 时 v2 = v1 + t16，否则 v2 = v1 - t16；
   buf[i] = (t16 ^ v2) ^ key[i&7]。
   t16 = {0x37,0x92,0x44,0x68,0xa5,0x3d,0xcc,0x7f,0xbb,0x0f,0xd9,0x88,
          0xee,0x9a,0xe9,0x5a}。
   验证：uni_call 调二进制 SetseattosKey/Seattos 逐字节一致。 */

#include "inc/seattos.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char seattos_bbit[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};

static const unsigned char seattos_ip[64] = {
    0x07, 0x18, 0x10, 0x27, 0x39, 0x31, 0x29, 0x0f,
    0x3c, 0x05, 0x36, 0x01, 0x1e, 0x04, 0x1c, 0x0e,
    0x3b, 0x13, 0x08, 0x3e, 0x0d, 0x1f, 0x0b, 0x00,
    0x2c, 0x24, 0x38, 0x33, 0x2b, 0x25, 0x23, 0x17,
    0x37, 0x3f, 0x14, 0x0c, 0x16, 0x0a, 0x1b, 0x03,
    0x06, 0x3d, 0x35, 0x2d, 0x3a, 0x1d, 0x15, 0x34,
    0x22, 0x1a, 0x30, 0x20, 0x09, 0x12, 0x21, 0x19,
    0x11, 0x2e, 0x26, 0x32, 0x2a, 0x02, 0x28, 0x2f
};

static const unsigned char seattos_t16[16] = {
    0x37, 0x92, 0x44, 0x68, 0xa5, 0x3d, 0xcc, 0x7f,
    0xbb, 0x0f, 0xd9, 0x88, 0xee, 0x9a, 0xe9, 0x5a
};

// mangled: _ZN8CSeattos4InitEPvS0_
void CSeattos::Init(void *arg0, void *arg1) {
    seattosbbit = new unsigned char[8];
    memmove(seattosbbit, seattos_bbit, 8);
    seattosIP = new unsigned char[0x40];
    memset(seattosIP, 0, 0x40);
    memmove(seattosIP, seattos_ip, 0x40);
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN8CSeattos6UnInitEv
void CSeattos::UnInit() {
    if (seattosbbit) {
        delete[] seattosbbit;
        seattosbbit = 0;
    }
    if (seattosIP) {
        delete[] seattosIP;
        seattosIP = 0;
    }
}

// mangled: _ZN8CSeattosC1Ev / _ZN8CSeattosC2Ev
CSeattos::CSeattos() {
    Init(0, 0);
}

// mangled: _ZN8CSeattosD1Ev / _ZN8CSeattosD2Ev
CSeattos::~CSeattos() {
    UnInit();
}

// mangled: _ZN8CSeattos13SetseattosKeyEPhS0_
void CSeattos::SetseattosKey(unsigned char *key, unsigned char *out) {
    unsigned int i;

    memset(out, 0, 8);
    for (i = 0; i < 64; i++) {
        unsigned int ip = seattosIP[i];
        unsigned char bit = seattosbbit[ip & 7];
        unsigned char byte = key[ip >> 3];
        unsigned char v = (byte & bit) ? 1 : 0;

        out[i >> 3] |= (unsigned char)(v << (i & 7));
    }
}

// mangled: _ZN8CSeattos7SeattosEPhbS0_j
void CSeattos::Seattos(unsigned char *key, bool benc, unsigned char *buf,
                       unsigned int buflen) {
    unsigned char k8[8];
    unsigned int i;

    if (!buflen)
        return;

    SetseattosKey(key, k8);

    for (i = 0; i < buflen; i++) {
        unsigned char t = seattos_t16[i & 0xf];
        unsigned char v = (unsigned char)(buf[i] ^ t ^ key[i & 7]);

        if (benc)
            v = (unsigned char)(v + t);
        else
            v = (unsigned char)(v - t);

        buf[i] = (unsigned char)((t ^ v) ^ key[i & 7]);
    }
}

// mangled: _Z10TenSeattosPhbS_j
void TenSeattos(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CSeattos seattos;

    seattos.Seattos(key, benc, buf, buflen);
}
