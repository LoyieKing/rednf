/* despc.cpp -- CDesPC：Phil Karn 公版 DES 位操作实现
   与二进制逐字节对照还原：
   - pc_perminit：由 1 基位号表（IP/FP）构建 perm[l][i][k] 查表：
     perm[l][i][j>>3] |= BBIT[j&7]，条件 (p[j]-1)>>2 == l 且 i 含位 (p[j]-1)&3
   - pc_permute：8 输入字节 × 2 半字节 → 8 输出字节（perm[2k][高半][j] | perm[2k+1][低半][j]）
   - kinit：PC1 提取 pc_pc1m[56] → 每轮 pc_pcr[i]=pc_pc1m[((i+TTT[r])%28)+(i>=28)*28]
     → PC2 压缩成 pc_kn[r][j>>3] |= BBIT[j&7]
   - expand：标准 E 展开（右半 4 字节 → 6 字节）
   - contract：s18[4][4096] 打包查表（s18[l][j]=(S[2l][g(j>>6)]<<4)|S[2l+1][g(j&63)]）
   - perm32：pc_p32[4][256][4] 查表（P 置换）
   - pc_f = expand ⊕ pc_kn[r] → contract → perm32；iter = 一轮 Feistel
   - endes/dedes = IP → 16 轮 → 交换 → FP；DesPC = 8 字节块 + TsLocal 余数
   DesPC 内联 sinit/p32init（表初始化）。全部表从 secagent .rodata 提取。 */

#include "inc/despc.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned int PC_NIBB[4] = {
#include "despc_nibb.inc"
};

static const unsigned int PC_BBIT[8] = {
#include "despc_bbit.inc"
};

static const unsigned char PC_IP[64] = {
#include "despc_ip.inc"
};

static const unsigned char PC_FP[64] = {
#include "despc_fp.inc"
};

static const unsigned char PC_PC1[64] = {
#include "despc_pc1.inc"
};

static const unsigned char PC_PC2[48] = {
#include "despc_pc2.inc"
};

static const unsigned char PC_P32I[32] = {
#include "despc_p32i.inc"
};

static const unsigned char PC_SI[8][64] = {
#include "despc_si.inc"
};

static const unsigned char PC_TTT[16] = {
#include "despc_ttt.inc"
};

// mangled: _ZN6CDesPC5kinitEPh
void CDesPC::kinit(unsigned char *key) {
    int i;
    int j;
    int l;
    int m;

    for (i = 0; i < 56; i++)
        pc_pc1m[i] = (unsigned char)
            ((key[(PC_PC1[i] - 1) >> 3] & PC_BBIT[(PC_PC1[i] - 1) & 7]) != 0);
    memset(pc_kn, 0, sizeof(pc_kn));
    for (l = 0; l < 16; l++) {
        for (i = 0; i < 56; i++) {
            m = (i + PC_TTT[l]) % 28 + (i >= 28 ? 28 : 0);
            pc_pcr[i] = pc_pc1m[m];
        }
        for (j = 0; j < 48; j++)
            if (pc_pcr[PC_PC2[j] - 1])
                pc_kn[l][j >> 3] |= (unsigned char)PC_BBIT[j & 7];
    }
}

// mangled: _ZN6CDesPC11pc_perminitEPA16_A8_hPKh
void CDesPC::pc_perminit(unsigned char (*perm)[16][8],
                         const unsigned char *p) {
    int l;
    int j;
    int k;
    int i;
    int m;

    memset(perm, 0, 0x800);
    for (l = 0; l < 16; l++)
        for (i = 0; i < 16; i++)
            for (j = 0; j < 64; j++)
                if (((p[j] - 1) >> 2) == l && (i & PC_NIBB[(p[j] - 1) & 3]))
                    perm[l][i][j >> 3] |= (unsigned char)PC_BBIT[j & 7];
}

// mangled: _ZN6CDesPC7getcompEjj
unsigned char CDesPC::getcomp(unsigned int k, unsigned int v) {
    return PC_SI[k][((((v & 0x20) >> 4) | (v & 1)) << 4) + ((v >> 1) & 0xf)];
}

// mangled: _ZN6CDesPC10pc_permuteEPhPA16_A8_hS0_
void CDesPC::pc_permute(unsigned char *inblock, unsigned char (*perm)[16][8],
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

// mangled: _ZN6CDesPC6expandEPhS0_
void CDesPC::expand(unsigned char *right, unsigned char *bigright) {
    unsigned char *bb;
    unsigned char *r;
    unsigned char r0;
    unsigned char r1;
    unsigned char r2;
    unsigned char r3;

    r0 = right[0];
    r1 = right[1];
    r2 = right[2];
    r3 = right[3];
    bigright[0] = (unsigned char)(((r0 >> 1) & 0x7c) | ((r0 >> 3) & 0x03) |
                                  ((r3 & 1) << 7));
    bigright[1] = (unsigned char)(((r0 << 5) & 0xe0) | ((r0 & 1) << 3) |
                                  ((r1 & 0x80) >> 3) | (r1 >> 5));
    bigright[2] = (unsigned char)(((r1 & 0x18) << 3) | ((r1 & 0x1f) << 1) |
                                  (r2 >> 7));
    bigright[3] = (unsigned char)(((r1 & 1) << 7) | ((r2 >> 1) & 0x7c) |
                                  ((r2 >> 3) & 0x03));
    bigright[4] = (unsigned char)(((r2 << 5) & 0xe0) | ((r2 & 1) << 3) |
                                  ((r3 & 0x80) >> 3) | (r3 >> 5));
    bigright[5] = (unsigned char)(((r3 & 0x18) << 3) | ((r3 & 0x1f) << 1) |
                                  (r0 >> 7));
}

// mangled: _ZN6CDesPC8contractEPhS0_
void CDesPC::contract(unsigned char *in48, unsigned char *out32) {
    unsigned char *c;
    unsigned char *i;
    unsigned int i0;
    unsigned int i1;
    unsigned int i2;
    unsigned int i3;
    unsigned int i4;
    unsigned int i5;

    i0 = in48[0];
    i1 = in48[1];
    i2 = in48[2];
    i3 = in48[3];
    i4 = in48[4];
    i5 = in48[5];
    out32[0] = s18[0][(i0 << 4) | (i1 >> 4)];
    out32[1] = s18[1][((i1 << 8) | i2) & 0xfff];
    out32[2] = s18[2][(i3 << 4) | (i4 >> 4)];
    out32[3] = s18[3][((i4 << 8) | i5) & 0xfff];
}

// mangled: _ZN6CDesPC6perm32EPhS0_
void CDesPC::perm32(unsigned char *inblock, unsigned char *outblock) {
    int j;
    unsigned char *ib;
    unsigned char *ob;
    unsigned char *q;

    ob = outblock;
    ob[0] = 0;
    ob[1] = 0;
    ob[2] = 0;
    ob[3] = 0;
    ib = inblock;
    for (j = 0; j < 4; j++) {
        q = pc_p32[j][*ib++];
        ob[0] |= q[0];
        ob[1] |= q[1];
        ob[2] |= q[2];
        ob[3] |= q[3];
    }
}

// mangled: _ZN6CDesPC4pc_fEPhiS0_
void CDesPC::pc_f(unsigned char *right, int num, unsigned char *fret) {
    unsigned char *kb;
    unsigned char *rb;
    unsigned char *bb;
    unsigned char bigright[8];
    unsigned char result[8];
    unsigned char preout[8];
    int j;

    rb = right;
    bb = bigright;
    expand(rb, bb);
    kb = pc_kn[num];
    for (j = 0; j < 6; j++)
        bb[j] ^= kb[j];
    contract(bb, result);
    perm32(result, fret);
}

// mangled: _ZN6CDesPC4iterEiPhS0_
void CDesPC::iter(int num, unsigned char *inblock, unsigned char *outblock) {
    unsigned char fret[8];
    unsigned char *ib;
    unsigned char *ob;
    unsigned char *fb;
    int j;

    pc_f(inblock + 4, num, fret);
    ib = inblock;
    ob = outblock;
    fb = fret;
    for (j = 0; j < 4; j++)
        ob[j] = ib[j + 4];
    for (j = 0; j < 4; j++)
        ob[j + 4] = (unsigned char)(ib[j] ^ fb[j]);
}

// mangled: _ZN6CDesPC5endesEPhS0_
void CDesPC::endes(unsigned char *inblock, unsigned char *outblock) {
    unsigned char iters[17][8];
    unsigned char swap[8];
    int i;
    unsigned char *s;
    unsigned char *t;

    pc_permute(inblock, pc_iperm, iters[0]);
    for (i = 0; i < 16; i++)
        iter(i, iters[i], iters[i + 1]);
    s = iters[16];
    t = swap;
    for (i = 0; i < 4; i++)
        t[i] = s[i + 4];
    for (i = 0; i < 4; i++)
        t[i + 4] = s[i];
    pc_permute(swap, pc_fperm, outblock);
}

// mangled: _ZN6CDesPC5dedesEPhS0_
void CDesPC::dedes(unsigned char *inblock, unsigned char *outblock) {
    unsigned char iters[17][8];
    unsigned char swap[8];
    int i;
    unsigned char *s;
    unsigned char *t;

    pc_permute(inblock, pc_iperm, iters[0]);
    for (i = 0; i < 16; i++)
        iter(15 - i, iters[i], iters[i + 1]);
    s = iters[16];
    t = swap;
    for (i = 0; i < 4; i++)
        t[i] = s[i + 4];
    for (i = 0; i < 4; i++)
        t[i + 4] = s[i];
    pc_permute(swap, pc_fperm, outblock);
}

// mangled: _ZN6CDesPC4InitEPvS0_
void CDesPC::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN6CDesPC6UnInitEv
void CDesPC::UnInit() {
}

// mangled: _ZN6CDesPC5DesPCEPhbS0_j
void CDesPC::DesPC(unsigned char *key, bool benc, unsigned char *buf,
                   unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;
    int l;
    int j;
    int m;
    unsigned int n;

    if (!buflen)
        return;
    pc_perminit(pc_iperm, PC_IP);
    pc_perminit(pc_fperm, PC_FP);
    kinit(key);
    /* sinit（内联）：s18[l][j] = (S[2l](j>>6)<<4) | S[2l+1](j&63) */
    for (l = 0; l < 4; l++)
        for (j = 0; j < 4096; j++)
            s18[l][j] = (unsigned char)((getcomp(2 * l, j >> 6) << 4) |
                                        (getcomp(2 * l + 1, j & 0x3f) & 0xf));
    /* p32init（内联） */
    memset(pc_p32, 0, sizeof(pc_p32));
    for (l = 0; l < 4; l++)
        for (m = 0; m < 256; m++)
            for (j = 0; j < 32; j++)
                if (((PC_P32I[j] - 1) >> 3) == l &&
                    (m & PC_BBIT[(PC_P32I[j] - 1) & 7]))
                    pc_p32[l][m][j >> 3] |= (unsigned char)PC_BBIT[j & 7];
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        if (benc)
            endes(buf + i * 8, buf + i * 8);
        else
            dedes(buf + i * 8, buf + i * 8);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN6CDesPCC1Ev / _ZN6CDesPCC2Ev
CDesPC::CDesPC() {
    Init(0, 0);
}

// mangled: _ZN6CDesPCD1Ev / _ZN6CDesPCD2Ev
CDesPC::~CDesPC() {
    UnInit();
}

// mangled: _Z8TenDesPCPhbS_j
void TenDesPC(unsigned char *key, bool benc, unsigned char *buf,
              unsigned int buflen) {
    CDesPC despc;

    despc.DesPC(key, benc, buf, buflen);
}
