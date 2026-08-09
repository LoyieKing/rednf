/* des.cpp -- DES 块密码（Applied Cryptography 附录 B 版本，与二进制逐字节对照）
   结构：DesKey（PC1 → 循环左移 → PC2 压缩（bigbyte 24 位）→ cookey 变换，
   输出 32 个“熟化”子钥 KnL）、DesBase（合并 IP/FP 的 desfunc + 大端读写）。
   SP 表从二进制 .rodata 0x081be0e0 提取（DES_SP[8][64]，行序为 SP8..SP1，
   即 DES_SP[0]=SP8、DES_SP[7]=SP1；Init 按此映射到成员）。 */

#include "inc/des.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char DES_PC1[56] = {
    56,48,40,32,24,16, 8, 0,57,49,41,33,25,17,
     9, 1,58,50,42,34,26,18,10, 2,59,51,43,35,
    62,54,46,38,30,22,14, 6,61,53,45,37,29,21,
    13, 5,60,52,44,36,28,20,12, 4,27,19,11, 3,
};

static const unsigned char DES_PC2[48] = {
    13,16,10,23, 0, 4, 2,27,14, 5,20, 9,
    22,18,11, 3,25, 7,15, 6,26,19,12, 1,
    40,51,30,36,46,54,29,39,50,44,32,47,
    43,48,38,55,33,52,45,41,49,35,28,31,
};

static const unsigned char DES_TOTROT[16] = {
    1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28,
};

static const unsigned char DES_BYTEBIT[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

static const unsigned int DES_BIGBYTE[24] = {
    0x800000,0x400000,0x200000,0x100000,0x080000,0x040000,
    0x020000,0x010000,0x008000,0x004000,0x002000,0x001000,
    0x000800,0x000400,0x000200,0x000100,0x000080,0x000040,
    0x000020,0x000010,0x000008,0x000004,0x000002,0x000001,
};

// 从 secagent .rodata 0x081be0e0 提取（8×64 DWORD，行序 SP8..SP1）
static const unsigned int DES_SP[8][64] = {
#include "des_sp.inc"
};

// mangled: _ZN4CDES7DesKeyEPhbPj
void CDES::DesKey(unsigned char *key, bool benc, unsigned int *KnL) {
    int i;
    int j;
    int l;
    int m;
    int n;
    unsigned char pc1m[64];
    unsigned char pcr[56];
    unsigned int kn[32];
    unsigned char totrot[16];
    unsigned int dough[32];

    memcpy(totrot, DES_TOTROT, 16);
    for (j = 0; j < 56; j++) {
        l = DES_PC1[j];
        m = l & 7;
        pc1m[j] = ((key[l >> 3] & DES_BYTEBIT[m]) != 0) ? 1 : 0;
    }
    for (i = 0; i < 16; i++) {
        /* 二进制反汇编确认：benc!=0 时子钥逆序（m=(15-i)<<1），benc==0 正序 */
        if (benc)
            m = (15 - i) << 1;
        else
            m = i << 1;
        n = m + 1;
        kn[m] = 0;
        kn[n] = 0;
        for (j = 0; j < 28; j++) {
            l = j + totrot[i];
            if (l < 28)
                pcr[j] = pc1m[l];
            else
                pcr[j] = pc1m[l - 28];
        }
        for (j = 28; j < 56; j++) {
            l = j + totrot[i];
            if (l < 56)
                pcr[j] = pc1m[l];
            else
                pcr[j] = pc1m[l - 28];
        }
        for (j = 0; j < 24; j++) {
            if (pcr[DES_PC2[j]] != 0)
                kn[m] |= DES_BIGBYTE[j];
            if (pcr[DES_PC2[j + 24]] != 0)
                kn[n] |= DES_BIGBYTE[j];
        }
    }
    /* cookey：把 16 对 24 位子钥变换成 32 个 32 位“熟化”子钥 */
    for (i = 0; i < 16; i++) {
        unsigned int raw0 = kn[2 * i];
        unsigned int raw1 = kn[2 * i + 1];
        dough[2 * i] = ((raw0 & 0x00fc0000) << 6)
                     | ((raw0 & 0x00000fc0) << 10)
                     | ((raw1 & 0x00fc0000) >> 10)
                     | ((raw1 & 0x00000fc0) >> 6);
        dough[2 * i + 1] = ((raw0 & 0x0003f000) << 12)
                         | ((raw0 & 0x0000003f) << 16)
                         | ((raw1 & 0x0003f000) >> 4)
                         | (raw1 & 0x0000003f);
    }
    for (i = 0; i < 32; i++)
        KnL[i] = dough[i];
}

// mangled: _ZN4CDES7DesBaseEPjPhS1_
void CDES::DesBase(unsigned int *KnL, unsigned char *inblock, unsigned char *outblock) {
    unsigned int work[2];
    unsigned int fval;
    unsigned int wtmp;
    unsigned int right;
    unsigned int leftt;
    int round;

    /* scrunch：大端装入 */
    work[0] = ((unsigned int)inblock[0] << 24) | ((unsigned int)inblock[1] << 16)
            | ((unsigned int)inblock[2] << 8) | (unsigned int)inblock[3];
    work[1] = ((unsigned int)inblock[4] << 24) | ((unsigned int)inblock[5] << 16)
            | ((unsigned int)inblock[6] << 8) | (unsigned int)inblock[7];
    leftt = work[0];
    right = work[1];

    /* IP（合并置换） */
    wtmp = ((leftt >> 4) ^ right) & 0x0f0f0f0f;
    right ^= wtmp;
    leftt ^= (wtmp << 4);
    wtmp = ((leftt >> 16) ^ right) & 0x0000ffff;
    right ^= wtmp;
    leftt ^= (wtmp << 16);
    wtmp = ((right >> 2) ^ leftt) & 0x33333333;
    leftt ^= wtmp;
    right ^= (wtmp << 2);
    wtmp = ((right >> 8) ^ leftt) & 0x00ff00ff;
    leftt ^= wtmp;
    right ^= (wtmp << 8);
    right = (right << 1) | (right >> 31);
    wtmp = (leftt ^ right) & 0xaaaaaaaa;
    leftt ^= wtmp;
    right ^= wtmp;
    leftt = (leftt << 1) | (leftt >> 31);

    /* 8 轮 × 4 子钥；成员表序与 AC 一致：SP7,SP5,SP3,SP1 | SP8,SP6,SP4,SP2 */
    for (round = 0; round < 8; round++) {
        wtmp = (right << 28) | (right >> 4);
        wtmp ^= KnL[4 * round];
        fval = SP7[wtmp & 0x3f];
        fval |= SP5[(wtmp >> 8) & 0x3f];
        fval |= SP3[(wtmp >> 16) & 0x3f];
        fval |= SP1[(wtmp >> 24) & 0x3f];
        wtmp = right ^ KnL[4 * round + 1];
        fval |= SP8[wtmp & 0x3f];
        fval |= SP6[(wtmp >> 8) & 0x3f];
        fval |= SP4[(wtmp >> 16) & 0x3f];
        fval |= SP2[(wtmp >> 24) & 0x3f];
        leftt ^= fval;
        wtmp = (leftt << 28) | (leftt >> 4);
        wtmp ^= KnL[4 * round + 2];
        fval = SP7[wtmp & 0x3f];
        fval |= SP5[(wtmp >> 8) & 0x3f];
        fval |= SP3[(wtmp >> 16) & 0x3f];
        fval |= SP1[(wtmp >> 24) & 0x3f];
        wtmp = leftt ^ KnL[4 * round + 3];
        fval |= SP8[wtmp & 0x3f];
        fval |= SP6[(wtmp >> 8) & 0x3f];
        fval |= SP4[(wtmp >> 16) & 0x3f];
        fval |= SP2[(wtmp >> 24) & 0x3f];
        right ^= fval;
    }

    /* FP（合并置换） */
    right = (right << 31) | (right >> 1);
    wtmp = (leftt ^ right) & 0xaaaaaaaa;
    leftt ^= wtmp;
    right ^= wtmp;
    leftt = (leftt << 31) | (leftt >> 1);
    wtmp = ((leftt >> 8) ^ right) & 0x00ff00ff;
    right ^= wtmp;
    leftt ^= (wtmp << 8);
    wtmp = ((leftt >> 2) ^ right) & 0x33333333;
    right ^= wtmp;
    leftt ^= (wtmp << 2);
    wtmp = ((right >> 16) ^ leftt) & 0x0000ffff;
    leftt ^= wtmp;
    right ^= (wtmp << 16);
    wtmp = ((right >> 4) ^ leftt) & 0x0f0f0f0f;
    leftt ^= wtmp;
    right ^= (wtmp << 4);
    work[0] = right;
    work[1] = leftt;

    /* unscrun：大端写出 */
    outblock[0] = (unsigned char)((work[0] >> 24) & 0xff);
    outblock[1] = (unsigned char)((work[0] >> 16) & 0xff);
    outblock[2] = (unsigned char)((work[0] >> 8) & 0xff);
    outblock[3] = (unsigned char)(work[0] & 0xff);
    outblock[4] = (unsigned char)((work[1] >> 24) & 0xff);
    outblock[5] = (unsigned char)((work[1] >> 16) & 0xff);
    outblock[6] = (unsigned char)((work[1] >> 8) & 0xff);
    outblock[7] = (unsigned char)(work[1] & 0xff);
}

// mangled: _ZN4CDES6UnInitEv
void CDES::UnInit() {
    if (SP1 != 0) {
        delete[] SP1;
        SP1 = 0;
    }
    if (SP2 != 0) {
        delete[] SP2;
        SP2 = 0;
    }
    if (SP3 != 0) {
        delete[] SP3;
        SP3 = 0;
    }
    if (SP4 != 0) {
        delete[] SP4;
        SP4 = 0;
    }
    if (SP5 != 0) {
        delete[] SP5;
        SP5 = 0;
    }
    if (SP6 != 0) {
        delete[] SP6;
        SP6 = 0;
    }
    if (SP7 != 0) {
        delete[] SP7;
        SP7 = 0;
    }
    if (SP8 != 0) {
        delete[] SP8;
        SP8 = 0;
    }
}

// mangled: _ZN4CDES4InitEPvS0_
void CDES::Init(void *pSys, void *pLocal) {
    unsigned int _SP1[64];
    unsigned int _SP2[64];
    unsigned int _SP3[64];
    unsigned int _SP4[64];
    unsigned int _SP5[64];
    unsigned int _SP6[64];
    unsigned int _SP7[64];
    unsigned int _SP8[64];

    m_pSys = pSys;
    m_pLocal = pLocal;
    /* 二进制反汇编确认：Init 先把 8 个 SP 成员清零，再逐个惰性分配 */
    SP1 = 0;
    SP2 = 0;
    SP3 = 0;
    SP4 = 0;
    SP5 = 0;
    SP6 = 0;
    SP7 = 0;
    SP8 = 0;
    if (SP1 == 0)
        SP1 = new unsigned int[64];
    if (SP2 == 0)
        SP2 = new unsigned int[64];
    if (SP3 == 0)
        SP3 = new unsigned int[64];
    if (SP4 == 0)
        SP4 = new unsigned int[64];
    if (SP5 == 0)
        SP5 = new unsigned int[64];
    if (SP6 == 0)
        SP6 = new unsigned int[64];
    if (SP7 == 0)
        SP7 = new unsigned int[64];
    if (SP8 == 0)
        SP8 = new unsigned int[64];

    /* 静态表行序：DES_SP[0]=SP8 ... DES_SP[7]=SP1 */
    memcpy(_SP1, DES_SP[7], 64 * sizeof(unsigned int));
    memmove(SP1, _SP1, 64 * sizeof(unsigned int));
    memcpy(_SP2, DES_SP[6], 64 * sizeof(unsigned int));
    memmove(SP2, _SP2, 64 * sizeof(unsigned int));
    memcpy(_SP3, DES_SP[5], 64 * sizeof(unsigned int));
    memmove(SP3, _SP3, 64 * sizeof(unsigned int));
    memcpy(_SP4, DES_SP[4], 64 * sizeof(unsigned int));
    memmove(SP4, _SP4, 64 * sizeof(unsigned int));
    memcpy(_SP5, DES_SP[3], 64 * sizeof(unsigned int));
    memmove(SP5, _SP5, 64 * sizeof(unsigned int));
    memcpy(_SP6, DES_SP[2], 64 * sizeof(unsigned int));
    memmove(SP6, _SP6, 64 * sizeof(unsigned int));
    memcpy(_SP7, DES_SP[1], 64 * sizeof(unsigned int));
    memmove(SP7, _SP7, 64 * sizeof(unsigned int));
    memcpy(_SP8, DES_SP[0], 64 * sizeof(unsigned int));
    memmove(SP8, _SP8, 64 * sizeof(unsigned int));
}

// mangled: _ZN4CDES3DesEPhbS0_j
void CDES::Des(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int KnL[32];
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        DesKey(key, benc, KnL);
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++)
            DesBase(KnL, buf + i * 8, buf + i * 8);
        pLocal = CreateTsLocal();
        pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
    }
}

// mangled: _ZN4CDESC1Ev / _ZN4CDESC2Ev
CDES::CDES() {
    Init(0, 0);
}

// mangled: _ZN4CDESD1Ev / _ZN4CDESD2Ev
CDES::~CDES() {
    UnInit();
}
