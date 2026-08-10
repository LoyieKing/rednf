/* fastdes.cpp -- CFastDes：Eric Young 老版 libdes “fast” DES（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）：

   fsetkey：8 个密钥字节先去掉校验位（>>1），按 (byte&7)+i*8 查 FD_wC_K3/FD_wD_K3、
   i*16+(byte>>3) 查 FD_wC_K4/FD_wD_K4，OR 出 28 位 C/D；16 轮按 FD_PRESHIFT[i-1]
   （1/2）对 28 位 C/D 循环左移，再按 C/D 的 7 个 4 位组查 FD_hKS_C4/FD_lKS_D4
   （偏移 0x00/0x40/0x80/0xc0/0x100/0x140/0x180）合成每轮 h/l 子钥。
   fencrypt：IP 用 FD_wL_I8（8 字节逆序，byte&0x55 与 (byte>>1)&0x55 各查一次，
   L/R 逐位左移）；16 轮 D_ENCRYPT：tR=ror(R,15)，子钥 h 组与 l 组各 4 个
   6 位索引查 FD_wPS[8][64]，f=(h组|0) ^ (l组|0) ^ L；轮末 L=R_old、R=f；
   FP 用 FD_wO_L4（(R_i<<1)|L_i 的 8 个 nibble 对，i=0..7），大端写出。
   FastDes 包装：fsetkey → 8 字节块循环 fencrypt(decrypt=benc?0:1) →
   TsLocal 余数。全部表从 secagent .rodata 提取。 */

#include "inc/fastdes.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char FD_PRESHIFT[16] = {
#include "fucking_preshift.inc"
};

static const unsigned int FD_hKS_C4[7][16] = {
#include "fucking_hks_c4.inc"
};

static const unsigned int FD_lKS_D4[7][16] = {
#include "fucking_lks_d4.inc"
};

static const unsigned int FD_wL_I8[86] = {
#include "fucking_wl_i8.inc"
};

static const unsigned int FD_wO_L4[16] = {
#include "fucking_wo_l4.inc"
};

static const unsigned int FD_wPS[8][64] = {
#include "fucking_wps.inc"
};

static const unsigned int FD_wC_K3[64] = {
#include "fucking_wc_k3.inc"
};

static const unsigned int FD_wC_K4[128] = {
#include "fucking_wc_k4.inc"
};

static const unsigned int FD_wD_K3[64] = {
#include "fucking_wd_k3.inc"
};

static const unsigned int FD_wD_K4[128] = {
#include "fucking_wd_k4.inc"
};

static inline unsigned int ror32(unsigned int v, unsigned int n) {
    return (v >> n) | (v << (32 - n));
}

// mangled: _ZN8CFastDes7fsetkeyEPhPNS_8keyschedE
void CFastDes::fsetkey(unsigned char *key, CFastDes::keysched *ks) {
    int i;
    unsigned int C;
    unsigned int D;
    int v;
    const unsigned int *ap;

    C = 0;
    D = 0;
    (void)ap;
    for (i = 0; i < 8; i++) {
        v = key[i] >> 1;
        C |= FD_wC_K3[(v & 7) + i * 8] | FD_wC_K4[i * 16 + (v >> 3)];
        D |= FD_wD_K3[(v & 7) + i * 8] | FD_wD_K4[i * 16 + (v >> 3)];
    }
    for (i = 1; i <= 16; i++) {
        unsigned int h, l;
        v = FD_PRESHIFT[i - 1];
        C = ((C << v) & 0x0fffffff) | (((C << v) >> 28) & 3);
        D = ((D << v) & 0x0fffffff) | (((D << v) >> 28) & 3);

        h = FD_hKS_C4[0][(C >> 0x18)]
          | FD_hKS_C4[1][(C >> 0x14) & 0xf]
          | FD_hKS_C4[2][(C >> 0x10) & 0xf]
          | FD_hKS_C4[3][(C >> 0xc) & 0xf]
          | FD_hKS_C4[4][(C >> 8) & 0xf]
          | FD_hKS_C4[5][(C >> 4) & 0xf]
          | FD_hKS_C4[6][C & 0xf];
        l = FD_lKS_D4[0][(D >> 0x18)]
          | FD_lKS_D4[1][(D >> 0x14) & 0xf]
          | FD_lKS_D4[2][(D >> 0x10) & 0xf]
          | FD_lKS_D4[3][(D >> 0xc) & 0xf]
          | FD_lKS_D4[4][(D >> 8) & 0xf]
          | FD_lKS_D4[5][(D >> 4) & 0xf]
          | FD_lKS_D4[6][D & 0xf];
        ks->KS[i - 1].h = h;
        ks->KS[i - 1].l = l;
    }
}

// mangled: _ZN8CFastDes8fencryptEPhiPNS_8keyschedE
void CFastDes::fencrypt(unsigned char *block, int decrypt,
                        CFastDes::keysched *ks) {
    int i;
    unsigned int L;
    unsigned int R;
    CFastDes::keystage *ksp;
    const unsigned int *ap;
    int v;
    unsigned int k;
    unsigned int tR;
    unsigned int t;
    unsigned int t2;
    unsigned char *bp;

    /* ---- IP：FD_wL_I8 字节查表逐位装入 ---- */
    L = 0;
    R = 0;
    for (i = 7; i >= 0; i--) {
        v = block[i];
        L = (L << 1) | FD_wL_I8[v & 0x55];
        R = (R << 1) | FD_wL_I8[(v >> 1) & 0x55];
    }
    (void)ap;
    (void)bp;
    (void)t;

    ksp = ks->KS;
    if (decrypt)
        ksp = (CFastDes::keystage *)((char *)ks + 0x78);

    /* ---- 16 轮 D_ENCRYPT ---- */
    for (i = 16; i > 0; i--) {
        k = ksp->h;
        tR = ror32(R, 0xf);
        t2 = FD_wPS[0][((tR >> 0xc) ^ (k >> 0x18)) & 0x3f]
           | FD_wPS[1][((tR >> 8) ^ (k >> 0x10)) & 0x3f]
           | FD_wPS[2][((tR >> 4) ^ (k >> 8)) & 0x3f]
           | FD_wPS[3][(k ^ tR) & 0x3f];
        t2 ^= L;

        k = ksp->l;
        t2 ^= FD_wPS[5][((R >> 7) ^ (k >> 0x10)) & 0x3f]
            | FD_wPS[4][((R >> 0xb) ^ (k >> 0x18)) & 0x3f]
            | FD_wPS[6][((R >> 3) ^ (k >> 8)) & 0x3f]
            | FD_wPS[7][(k ^ (tR >> 0x10)) & 0x3f];

        if (decrypt)
            ksp -= 1;
        else
            ksp += 1;
        L = R;
        R = t2;                 /* 末轮后 R 不再回写（FP 直接取 L/R） */
    }

    /* ---- FP：FD_wO_L4 nibble 组合（输入 = (L16, R16)） ---- */
    {
        /* 二进制 FP 取 [ebp-0x20]=L16（末轮前的 R）与 [ebp-0x14]=R16 */
        unsigned int hi, mid;
        hi = ((FD_wO_L4[L & 0xf] << 1) | FD_wO_L4[R & 0xf])
           | (((FD_wO_L4[(L >> 8) & 0xf] << 1) | FD_wO_L4[(R >> 8) & 0xf]) << 2)
           | (((FD_wO_L4[(L >> 0x10) & 0xf] << 1) | FD_wO_L4[(R >> 0x10) & 0xf]) << 4)
           | (((FD_wO_L4[(L >> 0x18) & 0xf] << 1) | FD_wO_L4[(R >> 0x18) & 0xf]) << 6);
        mid = ((FD_wO_L4[(L >> 4) & 0xf] << 1) | FD_wO_L4[(R >> 4) & 0xf])
            | (((FD_wO_L4[(L >> 0xc) & 0xf] << 1) | FD_wO_L4[(R >> 0xc) & 0xf]) << 2)
            | (((FD_wO_L4[(L >> 0x14) & 0xf] << 1) | FD_wO_L4[(R >> 0x14) & 0xf]) << 4)
            | (((FD_wO_L4[(L >> 0x1c) & 0xf] << 1) | FD_wO_L4[(R >> 0x1c) & 0xf]) << 6);

        block[7] = (unsigned char)mid;
        block[6] = (unsigned char)(mid >> 8);
        block[5] = (unsigned char)(mid >> 0x10);
        block[4] = (unsigned char)(mid >> 0x18);
        block[3] = (unsigned char)hi;
        block[2] = (unsigned char)(hi >> 8);
        block[1] = (unsigned char)(hi >> 0x10);
        block[0] = (unsigned char)(hi >> 0x18);
    }
}

// mangled: _ZN8CFastDes4InitEPvS0_
void CFastDes::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN8CFastDes6UnInitEv
void CFastDes::UnInit() {
}

// mangled: _ZN8CFastDes7FastDesEPhbS0_j
void CFastDes::FastDes(unsigned char *key, bool benc, unsigned char *buf,
                       unsigned int buflen) {
    CFastDes::keysched KS;
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    fsetkey(key, &KS);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        fencrypt(buf + i * 8, benc ? 0 : 1, &KS);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN8CFastDesC1Ev / _ZN8CFastDesC2Ev
CFastDes::CFastDes() {
    Init(0, 0);
}

// mangled: _ZN8CFastDesD1Ev / _ZN8CFastDesD2Ev
CFastDes::~CFastDes() {
    UnInit();
}

// mangled: _Z10TenFastDesPhbS_j
void TenFastDes(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CFastDes fastdes;

    fastdes.FastDes(key, benc, buf, buflen);
}
