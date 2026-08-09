/* desnew.cpp -- DesNew 自定义 8 字节块密码（由二进制反汇编 + hook 提取）
   FUCKING_NF：256 字节 S 盒（.rodata 0x081ca940）。
   encrypt 9 轮（密钥索引 r = 0,7,14,6,13,5,12,4,11，即 +7 mod 15）：
   - out4 ^= NF[out0 ^ key[r] ^ si]；out5 ^= NF[out1 ^ key[(r+1)%15] ^ si]
     （eax 在轮检查处被覆写为 r+1，密钥索引=(r+1) mod 15）；out6..out0 用
     (r+2..r+6) mod 15
   - si 初始 0，r==14 时 si=key[7]，r+2==15 时 si=key[8]，r+3==15 时
     si=key[9]，r+4==15 时结束（共 9 轮）
   decrypt 为镜像：r 从 14 递减（-7 mod 15），si 初始 key[9]，r==0 时
   si=key[8]，r==1 时 si=key[7]，r==2 时 si=0，r==3 时结束；
   输出顺序与密钥索引完全镜像。
   Des_New 包装：8 字节块循环 + TsLocal->ProcessLastBytes 余数。
   验证：uni_call 调二进制 encrypt/decrypt/Des_New 逐字节一致。 */

#include "inc/desnew.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned char CDesNew::FUCKING_NF[256] = {
    0x20, 0x89, 0xef, 0xbc, 0x66, 0x7d, 0xdd, 0x48,
    0xd4, 0x44, 0x51, 0x25, 0x56, 0xed, 0x93, 0x95,
    0x46, 0xe5, 0x11, 0x7c, 0x73, 0xcf, 0x21, 0x14,
    0x7a, 0x8f, 0x19, 0xd7, 0x33, 0xb7, 0x8a, 0x8e,
    0x92, 0xd3, 0x6e, 0xad, 0x01, 0xe4, 0xbd, 0x0e,
    0x67, 0x4e, 0xa2, 0x24, 0xfd, 0xa7, 0x74, 0xff,
    0x9e, 0x2d, 0xb9, 0x32, 0x62, 0xa8, 0xfa, 0xeb,
    0x36, 0x8d, 0xc3, 0xf7, 0xf0, 0x3f, 0x94, 0x02,
    0xe0, 0xa9, 0xd6, 0xb4, 0x3e, 0x16, 0x75, 0x6c,
    0x13, 0xac, 0xa1, 0x9f, 0xa0, 0x2f, 0x2b, 0xab,
    0xc2, 0xaf, 0xb2, 0x38, 0xc4, 0x70, 0x17, 0xdc,
    0x59, 0x15, 0xa4, 0x82, 0x9d, 0x08, 0x55, 0xfb,
    0xd8, 0x2c, 0x5e, 0xb3, 0xe2, 0x26, 0x5a, 0x77,
    0x28, 0xca, 0x22, 0xce, 0x23, 0x45, 0xe7, 0xf6,
    0x1d, 0x6d, 0x4a, 0x47, 0xb0, 0x06, 0x3c, 0x91,
    0x41, 0x0d, 0x4d, 0x97, 0x0c, 0x7f, 0x5f, 0xc7,
    0x39, 0x65, 0x05, 0xe8, 0x96, 0xd2, 0x81, 0x18,
    0xb5, 0x0a, 0x79, 0xbb, 0x30, 0xc1, 0x8b, 0xfc,
    0xdb, 0x40, 0x58, 0xe9, 0x60, 0x80, 0x50, 0x35,
    0xbf, 0x90, 0xda, 0x0b, 0x6a, 0x84, 0x9b, 0x68,
    0x5b, 0x88, 0x1f, 0x2a, 0xf3, 0x42, 0x7e, 0x87,
    0x1e, 0x1a, 0x57, 0xba, 0xb6, 0x9a, 0xf2, 0x7b,
    0x52, 0xa6, 0xd0, 0x27, 0x98, 0xbe, 0x71, 0xcd,
    0x72, 0x69, 0xe1, 0x54, 0x49, 0xa3, 0x63, 0x6f,
    0xcc, 0x3d, 0xc8, 0xd9, 0xaa, 0x0f, 0xc6, 0x1c,
    0xc0, 0xfe, 0x86, 0xea, 0xde, 0x07, 0xec, 0xf8,
    0xc9, 0x29, 0xb1, 0x9c, 0x5c, 0x83, 0x43, 0xf9,
    0xf5, 0xb8, 0xcb, 0x09, 0xf1, 0x00, 0x1b, 0x2e,
    0x85, 0xae, 0x4b, 0x12, 0x5d, 0xd1, 0x64, 0x78,
    0x4c, 0xd5, 0x10, 0x53, 0x04, 0x6b, 0x8c, 0x34,
    0x3a, 0x37, 0x03, 0xf4, 0x61, 0xc5, 0xee, 0xe3,
    0x76, 0x31, 0x4f, 0xe6, 0xdf, 0xa5, 0x99, 0x3b
};

// mangled: _ZN7CDesNew7encryptEPhS0_
void CDesNew::encrypt(unsigned char *key, unsigned char *buf) {
    unsigned char si = 0;
    unsigned int r = 0;
    unsigned char v;

    for (;;) {
        v = (unsigned char)(buf[0] ^ key[r] ^ si);
        buf[4] = (unsigned char)(buf[4] ^ FUCKING_NF[v]);
        if (r == 14)
            si = key[7];

        v = (unsigned char)(buf[1] ^ key[(r + 1) % 15] ^ si);
        buf[5] = (unsigned char)(buf[5] ^ FUCKING_NF[v]);
        if ((r + 2) == 15)
            si = key[8];

        v = (unsigned char)(buf[2] ^ key[(r + 2) % 15] ^ si);
        buf[6] = (unsigned char)(buf[6] ^ FUCKING_NF[v]);
        if ((r + 3) == 15)
            si = key[9];

        v = (unsigned char)(buf[3] ^ key[(r + 3) % 15] ^ si);
        buf[7] = (unsigned char)(buf[7] ^ FUCKING_NF[v]);
        if ((r + 4) == 15)
            return;

        v = (unsigned char)(buf[4] ^ key[(r + 4) % 15] ^ si);
        buf[1] = (unsigned char)(buf[1] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[4] ^ buf[5]);
        buf[2] = (unsigned char)(buf[2] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[6] ^ key[(r + 5) % 15] ^ si);
        buf[3] = (unsigned char)(buf[3] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[7] ^ key[(r + 6) % 15] ^ si);
        buf[0] = (unsigned char)(buf[0] ^ FUCKING_NF[v]);

        r = (r + 7) % 15;
    }
}

// mangled: _ZN7CDesNew7decryptEPhS0_
void CDesNew::decrypt(unsigned char *key, unsigned char *buf) {
    unsigned char si = key[9];
    unsigned int r = 14;
    unsigned char v;

    for (;;) {
        v = (unsigned char)(buf[3] ^ key[r] ^ si);
        buf[7] = (unsigned char)(buf[7] ^ FUCKING_NF[v]);
        if (r == 0)
            si = key[8];

        v = (unsigned char)(buf[2] ^ key[(r + 14) % 15] ^ si);
        buf[6] = (unsigned char)(buf[6] ^ FUCKING_NF[v]);
        if (r == 1)
            si = key[7];

        v = (unsigned char)(buf[1] ^ key[(r + 13) % 15] ^ si);
        buf[5] = (unsigned char)(buf[5] ^ FUCKING_NF[v]);
        if (r == 2)
            si = 0;

        v = (unsigned char)(buf[0] ^ key[(r + 12) % 15] ^ si);
        buf[4] = (unsigned char)(buf[4] ^ FUCKING_NF[v]);
        if (r == 3)
            return;

        v = (unsigned char)(buf[7] ^ key[(r + 11) % 15] ^ si);
        buf[0] = (unsigned char)(buf[0] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[6] ^ key[(r + 10) % 15] ^ si);
        buf[3] = (unsigned char)(buf[3] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[4] ^ buf[5]);
        buf[2] = (unsigned char)(buf[2] ^ FUCKING_NF[v]);
        v = (unsigned char)(buf[4] ^ key[(r + 9) % 15] ^ si);
        buf[1] = (unsigned char)(buf[1] ^ FUCKING_NF[v]);

        r = (r + 8) % 15;
    }
}

// mangled: _ZN7CDesNew4InitEPvS0_
void CDesNew::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN7CDesNew6UnInitEv
void CDesNew::UnInit() {
}

// mangled: _ZN7CDesNewC1Ev / _ZN7CDesNewC2Ev
CDesNew::CDesNew() {
    Init(0, 0);
}

// mangled: _ZN7CDesNewD1Ev / _ZN7CDesNewD2Ev
CDesNew::~CDesNew() {
    UnInit();
}

// mangled: _ZN7CDesNew7Des_NewEPhbS0_j
void CDesNew::Des_New(unsigned char *key, bool benc, unsigned char *buf,
                      unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;

    blocks = buflen >> 3;
    for (i = 0; i < blocks; i++) {
        if (benc)
            encrypt(key, buf + 8 * i);
        else
            decrypt(key, buf + 8 * i);
    }

    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 8,
                                      buf + blocks * 8);
}

// mangled: _Z10TenDesNewPhbS_j
void TenDesNew(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CDesNew desnew;

    desnew.Des_New(key, benc, buf, buflen);
}
