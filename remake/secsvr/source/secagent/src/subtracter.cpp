/* subtracter.cpp -- Subtracter 自定义流式密码（由二进制反汇编还原）
   SetSubKey：与 Adder 相同的 64 位比特置换（FUCKING_SUBIP/SUBBIT 表同 Adder）。
   Subtracter：逐字节 v = k8[i&7]；
   benc 时 cnt = v（v>=0x80 或 v==0 则跳过），否则 cnt = 0x100 - v；
   循环 cnt 次 buf[i] -= v（模 256）。
   验证：uni_call 调二进制 SetSubKey/Subtracter 逐字节一致。 */

#include "inc/subtracter.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned char CSubtracter::FUCKING_SUBBIT[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};
const unsigned char CSubtracter::FUCKING_SUBIP[64] = {
    0x07, 0x18, 0x10, 0x0b, 0x27, 0x39, 0x31, 0x29,
    0x3c, 0x05, 0x36, 0x2e, 0x01, 0x1e, 0x04, 0x0e,
    0x3b, 0x13, 0x08, 0x3e, 0x0d, 0x1f, 0x00, 0x2f,
    0x2c, 0x24, 0x38, 0x33, 0x2b, 0x23, 0x0f, 0x17,
    0x37, 0x3f, 0x1c, 0x14, 0x0c, 0x16, 0x1b, 0x03,
    0x06, 0x3d, 0x35, 0x2d, 0x25, 0x1d, 0x15, 0x34,
    0x22, 0x1a, 0x30, 0x28, 0x20, 0x12, 0x21, 0x19,
    0x11, 0x09, 0x26, 0x3a, 0x32, 0x2a, 0x0a, 0x02
};

// mangled: _ZN11CSubtracter9SetSubKeyEPhS0_
void CSubtracter::SetSubKey(unsigned char *key, unsigned char *out) {
    unsigned int i;

    memset(out, 0, 8);
    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_SUBIP[i];
        unsigned char bit = FUCKING_SUBBIT[ip & 7];
        unsigned char byte = key[ip >> 3];
        unsigned char v = (byte & bit) ? 1 : 0;

        out[i >> 3] |= (unsigned char)(v << (i & 7));
    }
}

// mangled: _ZN11CSubtracter10SubtracterEPhbS0_j
void CSubtracter::Subtracter(unsigned char *key, bool benc, unsigned char *buf,
                             unsigned int buflen) {
    unsigned char k8[8];
    unsigned int i;

    if (!buflen)
        return;

    SetSubKey(key, k8);

    for (i = 0; i < buflen; i++) {
        unsigned char v = k8[i & 7];
        int cnt = benc ? (int)v : (0x100 - v);

        if (cnt > 0) {
            unsigned int j;
            unsigned char al = buf[i];

            for (j = 0; j < (unsigned int)cnt; j++)
                al = (unsigned char)(al - v);
            buf[i] = al;
        }
    }
}

// mangled: _ZN11CSubtracter4InitEPvS0_
void CSubtracter::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN11CSubtracter6UnInitEv
void CSubtracter::UnInit() {
}

// mangled: _ZN11CSubtracterC1Ev / _ZN11CSubtracterC2Ev
CSubtracter::CSubtracter() {
    Init(0, 0);
}

// mangled: _ZN11CSubtracterD1Ev / _ZN11CSubtracterD2Ev
CSubtracter::~CSubtracter() {
    UnInit();
}

// mangled: _Z12TenSubtracterPhbS_j
void TenSubtracter(unsigned char *key, bool benc, unsigned char *buf,
                   unsigned int buflen) {
    CSubtracter subtracter;

    subtracter.Subtracter(key, benc, buf, buflen);
}
