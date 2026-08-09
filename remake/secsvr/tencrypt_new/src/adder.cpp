/* adder.cpp -- Adder 自定义流式密码（由二进制反汇编还原）
   SetAdderKey：按 FUCKING_M_ADDERIP[64] 对密钥 64 位比特置换：
   out[i>>3] 的 (i&7) 位 ← (key[IP[i]>>3] & BBIT[IP[i]&7]) != 0。
   Adder：逐字节 v = k8[i&7]；
   benc 时 cnt = v（v>=0x80 或 v==0 则跳过），否则 cnt = 0x100 - v；
   循环 cnt 次 buf[i] += v（模 256）。
   （enc 加 v² 次、dec 加 v*(256-v) 次，互补还原。）
   验证：uni_call 调二进制 SetAdderKey/Adder 逐字节一致。 */

#include "inc/adder.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned char CAdder::FUCKING_M_ADDERBBIT[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};
const unsigned char CAdder::FUCKING_M_ADDERIP[64] = {
    0x07, 0x18, 0x10, 0x0b, 0x27, 0x39, 0x31, 0x29,
    0x3c, 0x05, 0x36, 0x2e, 0x01, 0x1e, 0x04, 0x0e,
    0x3b, 0x13, 0x08, 0x3e, 0x0d, 0x1f, 0x00, 0x2f,
    0x2c, 0x24, 0x38, 0x33, 0x2b, 0x23, 0x0f, 0x17,
    0x37, 0x3f, 0x1c, 0x14, 0x0c, 0x16, 0x1b, 0x03,
    0x06, 0x3d, 0x35, 0x2d, 0x25, 0x1d, 0x15, 0x34,
    0x22, 0x1a, 0x30, 0x28, 0x20, 0x12, 0x21, 0x19,
    0x11, 0x09, 0x26, 0x3a, 0x32, 0x2a, 0x0a, 0x02
};

// mangled: _ZN6CAdder11SetAdderKeyEPhS0_
void CAdder::SetAdderKey(unsigned char *key, unsigned char *out) {
    unsigned int i;

    memset(out, 0, 8);
    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_M_ADDERIP[i];
        unsigned char bit = FUCKING_M_ADDERBBIT[ip & 7];
        unsigned char byte = key[ip >> 3];
        unsigned char v = (byte & bit) ? 1 : 0;

        out[i >> 3] |= (unsigned char)(v << (i & 7));
    }
}

// mangled: _ZN6CAdder5AdderEPhbS0_j
void CAdder::Adder(unsigned char *key, bool benc, unsigned char *buf,
                   unsigned int buflen) {
    unsigned char k8[8];
    unsigned int i;

    if (!buflen)
        return;

    SetAdderKey(key, k8);

    for (i = 0; i < buflen; i++) {
        unsigned char v = k8[i & 7];
        int cnt = benc ? (int)v : (0x100 - v);

        if (cnt > 0) {
            unsigned int j;
            unsigned char al = buf[i];

            for (j = 0; j < (unsigned int)cnt; j++)
                al = (unsigned char)(al + v);
            buf[i] = al;
        }
    }
}

// mangled: _ZN6CAdder4InitEPvS0_
void CAdder::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN6CAdder6UnInitEv
void CAdder::UnInit() {
}

// mangled: _ZN6CAdderC1Ev / _ZN6CAdderC2Ev
CAdder::CAdder() {
    Init(0, 0);
}

// mangled: _ZN6CAdderD1Ev / _ZN6CAdderD2Ev
CAdder::~CAdder() {
    UnInit();
}

// mangled: _Z8TenAdderPhbS_j
void TenAdder(unsigned char *buf, bool benc, unsigned char *key,
              unsigned int buflen) {
    CAdder adder;

    adder.Adder(key, benc, buf, buflen);
}
