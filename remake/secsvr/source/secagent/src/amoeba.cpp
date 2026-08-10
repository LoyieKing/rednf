/* amoeba.cpp -- CAmoeba：自定义 64 位位级密码（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）：

   Init：惰性分配三个成员表（amobbit[8]={0x81,0x40,0x22,0x10,0x0a,0x04,0x02,0x01}、
   amobb[8]={1,2,4,8,0x10,0x20,0x40,0x80}、amoIP[64] 置换表）并 memmove。
   amoKey：add[i>>3] |= (key[amoIP[i]>>3] & amobbit[amoIP[i]&7]) != 0 << (i&7)。
   amoEnCrypt/amoDeCrypt：IP 按 amobb 位提取 64 位状态 -> 8 轮（加密密钥
   1..8、位 6..0 再 7；解密密钥 8..1、位 0..6 再 7）。每轮按密钥字节位选择
   SET/CLEAR 块（字节旋转 + NOT 扩散 + 交叉），bit7 另有特殊块/普通块；
   FP 按位回写 8 字节。全部块语义经二进制内部状态逐块对拍确认。
   Amoeba 包装：amoKey 生成 Add 后按 8 字节块循环 amoEnCrypt/amoDeCrypt
   （用 Add 作块密钥），余数交 CreateTsLocal()->ProcessLastBytes(key,benc,
   buflen,8,buf)。TenAmoeba 为直通包装。 */

#include "inc/amoeba.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char AMOEBA_AMOBBIT[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};

static const unsigned char AMOEBA_AMOBB[8] = {
    1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80
};

static const unsigned char AMOEBA_AMOIP[64] = {
    0x3c,0x05,0x36,0x2e,0x01,0x1e,0x04,0x0e,0x22,0x1a,0x30,0x28,0x20,0x12,0x21,0x19,
    0x37,0x3f,0x1c,0x14,0x0c,0x16,0x1b,0x03,0x06,0x3d,0x35,0x2d,0x25,0x1d,0x15,0x34,
    0x2c,0x24,0x38,0x33,0x2b,0x23,0x0f,0x17,0x07,0x18,0x10,0x0b,0x27,0x39,0x31,0x29,
    0x3b,0x13,0x08,0x3e,0x0d,0x1f,0x00,0x2f,0x11,0x09,0x26,0x3a,0x32,0x2a,0x0a,0x02
};

// mangled: _ZN7CAmoeba4InitEPvS0_
void CAmoeba::Init(void *pSys, void *pLocal) {
    unsigned char _amobbit[8];
    unsigned char _amobb[8];
    unsigned char _amoIP[64];
    int i;

    amobbit = 0;
    amobb = 0;
    amoIP = 0;
    m_pSys = pSys;
    m_pLocal = pLocal;
    for (i = 0; i < 8; i++) {
        _amobbit[i] = AMOEBA_AMOBBIT[i];
        _amobb[i] = AMOEBA_AMOBB[i];
    }
    for (i = 0; i < 64; i++)
        _amoIP[i] = AMOEBA_AMOIP[i];
    if (!amobbit) {
        amobbit = new unsigned char[8];
        memset(amobbit, 0, 8);
    }
    memmove(amobbit, _amobbit, 8);
    if (!amobb) {
        amobb = new unsigned char[8];
        memset(amobb, 0, 8);
    }
    memmove(amobb, _amobb, 8);
    if (!amoIP) {
        amoIP = new unsigned char[64];
        memset(amoIP, 0, 64);
    }
    memmove(amoIP, _amoIP, 0x40);
}

// mangled: _ZN7CAmoeba6UnInitEv
void CAmoeba::UnInit() {
    if (amobbit) {
        delete[] amobbit;
        amobbit = 0;
    }
    if (amobb) {
        delete[] amobb;
        amobb = 0;
    }
    if (amoIP) {
        delete[] amoIP;
        amoIP = 0;
    }
}

// mangled: _ZN7CAmoebaC1Ev / _ZN7CAmoebaC2Ev
CAmoeba::CAmoeba() {
    Init(0, 0);
}

// mangled: _ZN7CAmoebaD1Ev / _ZN7CAmoebaD2Ev
CAmoeba::~CAmoeba() {
    UnInit();
}

// mangled: _ZN7CAmoeba6amoKeyEPhS0_
void CAmoeba::amoKey(unsigned char *key, unsigned char *add) {
    int i;
    int l;

    for (l = 0; l < 8; l++)
        add[l] = 0;
    for (i = 0; i < 64; i++) {
        l = i >> 3;
        if (key[amoIP[i] >> 3] & amobbit[amoIP[i] & 7])
            add[l] |= (unsigned char)(1 << (i & 7));
    }
}

// mangled: _ZN7CAmoeba10amoEnCryptEPhS0_
void CAmoeba::amoEnCrypt(unsigned char *key, unsigned char *buf) {
    unsigned char pcrn[64];
    unsigned char bak;
    int j;
    int n;

    for (j = 0; j < 64; j++)
        pcrn[j] = (buf[j >> 3] & amobb[j & 7]) ? 1 : 0;
    for (n = 1; n <= 8; n++) {
        bak = key[n - 1];
        if (bak & 0x80) {
            unsigned char c0 = pcrn[0];
            int eax;
            for (eax = 8; eax < 64; eax += 8)
                pcrn[eax - 8] = (unsigned char)!pcrn[eax];
            pcrn[56] = (unsigned char)!c0;
            c0 = pcrn[63];
            for (eax = 7; eax >= 1; eax--)
                pcrn[56 + eax] = pcrn[55 + eax];
            pcrn[56] = c0;
        } else {
            unsigned char c0 = pcrn[56];
            int kk;
            for (kk = 7; kk >= 1; kk--)
                pcrn[kk * 8] = pcrn[(kk - 1) * 8];
            pcrn[0] = c0;
            c0 = pcrn[56];
            for (kk = 1; kk < 8; kk++)
                pcrn[55 + kk] = (unsigned char)!pcrn[56 + kk];
            pcrn[63] = (unsigned char)!c0;
        }
        for (j = 6; j >= 0; j--) {
            if (bak & (1 << j)) {
                unsigned char cross = pcrn[7 - j];
                unsigned char tmp;
                int B = 8 * j;
                int eax;
                int i;
                for (eax = 8; eax < 64; eax += 8)
                    pcrn[eax - (j + 1)] = (unsigned char)!pcrn[eax + (7 - j)];
                pcrn[63 - j] = (unsigned char)!cross;
                tmp = pcrn[B + 7];
                for (i = B + 6; i >= B; i--)
                    pcrn[i + 1] = pcrn[i];
                pcrn[B] = tmp;
            } else {
                unsigned char tmp = pcrn[(7 - j) + 56];
                unsigned char cross;
                int B = 8 * j;
                int G = 7 - j;
                int kk;
                int i;
                for (kk = 7; kk >= 1; kk--)
                    pcrn[G + 8 * kk] = pcrn[G + 8 * (kk - 1)];
                pcrn[G] = tmp;
                cross = pcrn[B];
                for (i = B; i < B + 7; i++)
                    pcrn[i] = (unsigned char)!pcrn[i + 1];
                pcrn[B + 7] = (unsigned char)!cross;
            }
        }
    }
    for (j = 0; j < 8; j++)
        buf[j] = 0;
    for (j = 0; j < 64; j++) {
        if (pcrn[j])
            buf[j >> 3] |= (unsigned char)(1 << (j & 7));
    }
}

// mangled: _ZN7CAmoeba10amoDeCryptEPhS0_
void CAmoeba::amoDeCrypt(unsigned char *key, unsigned char *buf) {
    unsigned char pcrn[64];
    unsigned char bak;
    int j;
    int n;

    for (j = 0; j < 64; j++)
        pcrn[j] = (buf[j >> 3] & amobb[j & 7]) ? 1 : 0;
    for (n = 8; n >= 1; n--) {
        bak = key[n - 1];
        for (j = 0; j < 7; j++) {
            int B = 8 * j;
            if (bak & (1 << j)) {
                unsigned char tmp = pcrn[B];
                unsigned char cross;
                int i;
                int eax;
                for (i = B; i < B + 7; i++)
                    pcrn[i] = pcrn[i + 1];
                pcrn[B + 7] = tmp;
                cross = pcrn[63 - j];
                for (eax = 0x30; eax >= 0; eax -= 8)
                    pcrn[eax + 15 - j] = (unsigned char)!pcrn[eax + 7 - j];
                pcrn[7 - j] = (unsigned char)!cross;
            } else {
                unsigned char cross = pcrn[B + 7];
                unsigned char tmp;
                int G = 7 - j;
                int i;
                int kk;
                for (i = B + 7; i > B; i--)
                    pcrn[i] = (unsigned char)!pcrn[i - 1];
                pcrn[B] = (unsigned char)!cross;
                tmp = pcrn[G];
                for (kk = 0; kk < 7; kk++)
                    pcrn[G + 8 * kk] = pcrn[G + 8 * (kk + 1)];
                pcrn[G + 56] = tmp;
            }
        }
        if (bak & 0x80) {
            unsigned char c0 = pcrn[56];
            int i;
            int eax;
            for (i = 56; i < 63; i++)
                pcrn[i] = pcrn[i + 1];
            pcrn[63] = c0;
            c0 = pcrn[56];
            for (eax = 0x30; eax >= 0; eax -= 8)
                pcrn[eax + 8] = (unsigned char)!pcrn[eax];
            pcrn[0] = (unsigned char)!c0;
        } else {
            unsigned char cross = pcrn[63];
            unsigned char tmp;
            int i;
            int kk;
            for (i = 63; i > 56; i--)
                pcrn[i] = (unsigned char)!pcrn[i - 1];
            pcrn[56] = (unsigned char)!cross;
            tmp = pcrn[0];
            for (kk = 0; kk < 7; kk++)
                pcrn[kk * 8] = pcrn[(kk + 1) * 8];
            pcrn[56] = tmp;
        }
    }
    for (j = 0; j < 8; j++)
        buf[j] = 0;
    for (j = 0; j < 64; j++) {
        if (pcrn[j])
            buf[j >> 3] |= (unsigned char)(1 << (j & 7));
    }
}

// mangled: _ZN7CAmoeba6AmoebaEPhbS0_j
void CAmoeba::Amoeba(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned char Add[8];
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    amoKey(key, Add);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++) {
        if (benc)
            amoEnCrypt(Add, buf + i * 8);
        else
            amoDeCrypt(Add, buf + i * 8);
    }
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _Z9TenAmoebaPhbS_j
void TenAmoeba(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CAmoeba amoeba;

    amoeba.Amoeba(key, benc, buf, buflen);
}
