/* desmo.cpp -- CDesMo：Baldwin 式单函数 DES（tencrypt 闭源实现复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）：

   mo_crypt(benc, key, buf)：
   1) 64 位 PC1 置换：pcrn[i>>3] |= (key[MOIP[i]>>3] & MOBBIT[MOIP[i]&7])<<(i&7)；
      benc 时 pc1m 同法取 buf（MOBB 掩码），!benc 时 pc1m[0..7]=buf[0..7]。
   2) left/right = pc1m[0..3]/pc1m[4..7]（大端）。
   3) 8 轮：benc 按 pcrn[i] 低/高半字节 rol left/right，i&3==3 时取反；
      !benc 按 pcrn[7-i] 反向旋转（rol 0x20-半字节），i&3==3 先取反。
   4) 回写 pc1m：left/right 大端字节，pc1m[7]=dl（benc 时 pcrn[7]，
      !benc 时 pcrn[0]>>4）；benc 拷贝 8 字节到 buf，!benc 清 0 后做
      64 位最终散布：buf[MOIP[i]>>3] |= (pc1m[i>>3]>>(i&7))&1 ? MOBB[MOIP[i]&7] : 0。
   表从 secagent .rodata 提取（MOBBIT=PC1 掩码带奇偶位、MOBB=LSB 掩码、
   MOIP=IP 置换表）。Des_Mo 为 8 字节块 ECB + TsLocal 余数。 */

#include "inc/desmo.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char MOBBIT[8] = {
#include "fucking_mobbit.inc"
};

static const unsigned char MOBB[8] = {
#include "fucking_mobb.inc"
};

static const unsigned char MOIP[64] = {
#include "fucking_moip.inc"
};

static inline unsigned int rol32(unsigned int v, unsigned int n) {
    return (v << n) | (v >> (32 - n));
}

// mangled: _ZN6CDesMo8mo_cryptEbPhS0_
void CDesMo::mo_crypt(bool benc, unsigned char *key, unsigned char *buf) {
    int i, l;
    unsigned char pc1m[8];
    unsigned char pcrn[8];
    unsigned int left, right;
    unsigned char kyl, kyr;

    memset(pc1m, 0, sizeof(pc1m));
    memset(pcrn, 0, sizeof(pcrn));

    /* 64 位 PC1 置换：pcrn 由 key 派生；pc1m 由 buf 派生（benc）或直拷（!benc） */
    for (i = 0; i < 64; i++) {
        pcrn[i >> 3] |=
            ((key[MOIP[i] >> 3] & MOBBIT[MOIP[i] & 7]) != 0) << (i & 7);
        if (benc)
            pc1m[i >> 3] |=
                ((buf[MOIP[i] >> 3] & MOBB[MOIP[i] & 7]) != 0) << (i & 7);
        else if (i <= 7)
            pc1m[i] = buf[i];
    }

    left = ((unsigned int)pc1m[0] << 24) | ((unsigned int)pc1m[1] << 16)
         | ((unsigned int)pc1m[2] << 8) | pc1m[3];
    right = ((unsigned int)pc1m[4] << 24) | ((unsigned int)pc1m[5] << 16)
          | ((unsigned int)pc1m[6] << 8) | pc1m[7];

    /* 8 轮 Feistel 旋转 */
    for (i = 0; i < 8; i++) {
        if (benc) {
            kyl = pcrn[i];
            left = rol32(left, kyl & 0xf);
            kyr = kyl >> 4;
            right = rol32(right, kyr);
            if ((i & 3) == 3) {
                left = ~left;
                right = ~right;
            }
        } else {
            kyl = pcrn[7 - i];
            if ((i & 3) == 3) {
                left = ~left;
                right = ~right;
            }
            left = rol32(left, 0x20 - (kyl & 0xf));
            kyr = kyl >> 4;
            right = rol32(right, 0x20 - kyr);
        }
    }

    pc1m[0] = (unsigned char)(left >> 24);
    pc1m[1] = (unsigned char)(left >> 16);
    pc1m[2] = (unsigned char)(left >> 8);
    pc1m[3] = (unsigned char)left;
    pc1m[4] = (unsigned char)(right >> 24);
    pc1m[5] = (unsigned char)(right >> 16);
    pc1m[6] = (unsigned char)(right >> 8);
    pc1m[7] = (unsigned char)right;   /* mov edx, esi 后 dl=right 低字节 */

    if (benc) {
        for (l = 1; l < 9; l++)
            buf[l - 1] = pc1m[l - 1];
    } else {
        for (l = 1; l < 9; l++)
            buf[l - 1] = 0;
        /* 最终散布：!benc 特有 */
        for (i = 0; i < 64; i++) {
            unsigned char v = 0;
            if ((pc1m[i >> 3] >> (i & 7)) & 1)
                v = MOBB[MOIP[i] & 7];
            buf[MOIP[i] >> 3] |= v;
        }
    }
}

// mangled: _ZN6CDesMo4InitEPvS0_
void CDesMo::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN6CDesMo6UnInitEv
void CDesMo::UnInit() {
}

// mangled: _ZN6CDesMo6Des_MoEPhbS0_j
void CDesMo::Des_Mo(unsigned char *key, bool benc, unsigned char *buf,
                    unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        mo_crypt(benc, key, buf + i * 8);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN6CDesMoC1Ev / _ZN6CDesMoC2Ev
CDesMo::CDesMo() {
    Init(0, 0);
}

// mangled: _ZN6CDesMoD1Ev / _ZN6CDesMoD2Ev
CDesMo::~CDesMo() {
    UnInit();
}

// mangled: _Z8TenDesMoPhbS_j
void TenDesMo(unsigned char *key, bool benc, unsigned char *buf,
              unsigned int buflen) {
    CDesMo desmo;

    desmo.Des_Mo(key, benc, buf, buflen);
}
