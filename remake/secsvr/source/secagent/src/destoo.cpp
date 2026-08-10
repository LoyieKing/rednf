/* destoo.cpp -- CDesToo：4.3BSD-Reno des.c 类封装（DWORD SP 表版 DES）
   与二进制逐字节对照还原：
   - fsf = E 展开（ror(r,1) 后分 6 位段 ⊕ 子钥字节）→ 查静态 FUCKING_TOO_SP[8][64]
     → OR 合并；第 8 组用 rol(r,1) ⊕ sk[7]
   - round(num,block)：奇数轮 block[1]^=f(block[0])，偶数轮 block[0]^=f(block[1])
   - doencrypt/dodecrypt = IP → 16 轮（dec 前先交换、子钥逆序）→ FP
   - too_setkey：PC1→pc1m → TOTROT 累计轮转 pcr → PC2 打包
     too_kn[i][j/6] |= BYTEBIT[j%6]>>2；随后 too_perminit(iperm/fperm)
   - spinit 构建成员 too_sp（死代码，二进制从未调用；fsf 用静态表）
   - too_perminit/too_permute：Karn 式 nibble 查表（NIBBLEBIT=8,4,2,1）
   表全部从 secagent .rodata 提取。 */

#include "inc/destoo.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char TOO_PC1[56] = {
#include "destoo_pc1.inc"
};

static const unsigned int TOO_BYTEBIT[8] = {
#include "destoo_bytebit.inc"
};

static const unsigned char TOO_TOTROT[16] = {
#include "destoo_totrot.inc"
};

static const unsigned char TOO_PC2[48] = {
#include "destoo_pc2.inc"
};

static const unsigned char TOO_IP[64] = {
#include "destoo_ip.inc"
};

static const unsigned char TOO_FP[64] = {
#include "destoo_fp.inc"
};

static const unsigned int TOO_NIBBLEBIT[4] = {
#include "destoo_nibblebit.inc"
};

static const unsigned char TOO_P32I[32] = {
#include "destoo_p32i.inc"
};

static const unsigned char TOO_SI[8][64] = {
#include "destoo_si.inc"
};

static const unsigned int TOO_SP[8][64] = {
#include "destoo_sp.inc"
};

// mangled: _ZN7CDesToo3fsfEjPh
unsigned int CDesToo::fsf(unsigned int r, unsigned char *subkey) {
    unsigned int rval;
    unsigned int rt;

    rt = (r >> 1) | ((r & 1) ? 0x80000000 : 0);
    rval = TOO_SP[0][((rt >> 26) ^ subkey[0]) & 0x3f]
         | TOO_SP[1][((rt >> 22) ^ subkey[1]) & 0x3f]
         | TOO_SP[2][((rt >> 18) ^ subkey[2]) & 0x3f]
         | TOO_SP[3][((rt >> 14) ^ subkey[3]) & 0x3f]
         | TOO_SP[4][((rt >> 10) ^ subkey[4]) & 0x3f]
         | TOO_SP[5][((rt >> 6) ^ subkey[5]) & 0x3f]
         | TOO_SP[6][((rt >> 2) ^ subkey[6]) & 0x3f]
         | TOO_SP[7][(((r << 1) | (r >> 31)) ^ subkey[7]) & 0x3f];
    return rval;
}

// mangled: _ZN7CDesToo11too_permuteEPhPA16_A8_hS0_
void CDesToo::too_permute(unsigned char *inblock,
                          unsigned char (*perm)[16][8],
                          unsigned char *outblock) {
    int i;
    int j;
    unsigned char *ib;
    unsigned char *ob;
    unsigned char *p;
    unsigned char *q;

    for (i = 0; i < 8; i++)
        outblock[i] = 0;
    ib = inblock;
    ob = outblock;
    for (i = 0; i < 8; i++) {
        p = (*perm)[*ib >> 4];
        q = (*(perm + 1))[*ib & 0xf];
        for (j = 0; j < 8; j++)
            ob[j] |= (unsigned char)(p[j] | q[j]);
        ib++;
        perm += 2;
    }
}

// mangled: _ZN7CDesToo5roundEiPj
void CDesToo::round(int num, unsigned int *block) {
    if (num & 1)
        block[1] ^= fsf(block[0], too_kn[num]);
    else
        block[0] ^= fsf(block[1], too_kn[num]);
}

// mangled: _ZN7CDesToo9doencryptEPh
void CDesToo::doencrypt(unsigned char *block) {
    int i;
    unsigned int work[2];
    unsigned int tmp;

    too_permute(block, too_iperm, (unsigned char *)work);
    for (i = 0; i < 16; i++)
        round(i, work);
    tmp = work[0];
    work[0] = work[1];
    work[1] = tmp;
    too_permute((unsigned char *)work, too_fperm, block);
}

// mangled: _ZN7CDesToo9dodecryptEPh
void CDesToo::dodecrypt(unsigned char *block) {
    int i;
    unsigned int work[2];
    unsigned int tmp;

    too_permute(block, too_iperm, (unsigned char *)work);
    tmp = work[0];
    work[0] = work[1];
    work[1] = tmp;
    for (i = 15; i >= 0; i--)
        round(i, work);
    too_permute((unsigned char *)work, too_fperm, block);
}

// mangled: _ZN7CDesToo12too_perminitEPA16_A8_hPKh
void CDesToo::too_perminit(unsigned char (*perm)[16][8],
                           const unsigned char *p) {
    int l;
    int j;
    int k;
    int i;
    int m;

    memset(perm, 0, 16 * 16 * 8);
    for (l = 0; l < 16; l++)
        for (i = 0; i < 16; i++)
            for (j = 0; j < 64; j++)
                if (((p[j] - 1) >> 2) == l &&
                    (i & TOO_NIBBLEBIT[(p[j] - 1) & 3]))
                    perm[l][i][j >> 3] |= (unsigned char)TOO_BYTEBIT[j & 7];
}

// mangled: _ZN7CDesToo10too_setkeyEPh
void CDesToo::too_setkey(unsigned char *key) {
    unsigned char pc1m[56];
    unsigned char pcr[56];
    int i;
    int j;
    int l;
    int m;

    memset(too_kn, 0, sizeof(too_kn));
    for (j = 0; j < 56; j++)
        pc1m[j] = (unsigned char)
            ((key[(TOO_PC1[j] - 1) >> 3] & TOO_BYTEBIT[(TOO_PC1[j] - 1) & 7])
             != 0);
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 56; j++) {
            l = (j + TOO_TOTROT[i]) % 28 + (j >= 28 ? 28 : 0);
            pcr[j] = pc1m[l];
        }
        for (j = 0; j < 48; j++)
            if (pcr[TOO_PC2[j] - 1])
                too_kn[i][j / 6] |= (unsigned char)(TOO_BYTEBIT[j % 6] >> 2);
    }
    too_perminit(too_iperm, TOO_IP);
    too_perminit(too_fperm, TOO_FP);
}

// mangled: _ZN7CDesToo6spinitEv
void CDesToo::spinit() {
    unsigned char pbox[32];
    int p;
    int i;
    int s;
    int j;
    unsigned int val;
    unsigned int rowcol;

    for (p = 0; p < 32; p++)
        for (i = 0; i < 32; i++)
            if (TOO_P32I[i] - 1 == p) {
                pbox[p] = (unsigned char)i;
                break;
            }
    for (s = 0; s < 8; s++)
        for (i = 0; i < 64; i++) {
            val = 0;
            rowcol = (i & 0x20) | ((i & 1) ? 0x10 : 0) | ((i >> 1) & 0xf);
            for (j = 0; j < 4; j++)
                if (TOO_SI[s][rowcol] & (8 >> j))
                    val |= 1u << (31 - pbox[4 * s + j]);
            too_sp[s][i] = val;
        }
}

// mangled: _ZN7CDesToo4InitEPvS0_
void CDesToo::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN7CDesToo6UnInitEv
void CDesToo::UnInit() {
}

// mangled: _ZN7CDesToo6DesTooEPhbS0_j
void CDesToo::DesToo(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    too_setkey(key);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        if (benc)
            doencrypt(buf + i * 8);
        else
            dodecrypt(buf + i * 8);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN7CDesTooC1Ev / _ZN7CDesTooC2Ev
CDesToo::CDesToo() {
    Init(0, 0);
}

// mangled: _ZN7CDesTooD1Ev / _ZN7CDesTooD2Ev
CDesToo::~CDesToo() {
    UnInit();
}

// mangled: _Z9TenDesTooPhbS_j
void TenDesToo(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CDesToo destoo;

    destoo.DesToo(key, benc, buf, buflen);
}
