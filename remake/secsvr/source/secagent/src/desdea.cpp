/* desdea.cpp -- DesDea / des_sched / des_dea3（3DES-EDE2，16 字节密钥）
   与 secagent 二进制逐字节对照还原（2026-08-09）：

   des_sched：8 字节密钥按 dea_pc1（8 行 x 256 项 x 8B）查表 OR 出两个 32 位
   状态 A/B（dword0/dword1 各 8 行）；16 轮按 dea_ls（1/1/2/2/2/2/2/2/1/2/2/2/
   2/2/2/1）对 28 位窗口右旋（bit0->bit27）；每轮按 A/B 的 8 个字节查 dea_pc2
   （行 0..7 对应字节 0..7），dword0 合成 DWORD1、dword1 合成 DWORD2，
   schedule 每轮 8 字节按小端写入（byte0 在前）。
   des_dea3：48 轮展开 = E_K1(D_K2(E_K1)) 单链，但段边界处 L/R 交换：
     E_K1（sched[0] 正序 16 轮）-> swap -> D_K2（sched[1] 逆序 16 轮）-> swap
     -> E_K1（sched[0] 正序 16 轮）；解密镜像（D_K1/E_K2/D_K1）。
   每轮：idx0=(R<<1)&0x3e（R bit31 置位时 |1）、idx_i=(R>>(4i-1))&0x3f
   （i=1..6）、idx7=(R>>27)&0x1f；idx ^= 轮子钥字节；F=OR s_and_p[i][idx_i]；
   newR = F ^ L；newL = oldR。IP 用 dea_ip（dword0->X、dword1->Y），
   FP 用 ip_inv（输入 = R LE ++ L LE，dword0->out 前 4 字节、dword1->后 4 字节）。
   DesDea 包装：des_sched(key) 与 des_sched(key+8) 组成 2x16 schedule，
   8 字节块循环 des_dea3 原地加解密，最后 CreateTsLocal()->ProcessLastBytes
   (key, benc, buflen, 0x10, buf) 处理余数；TenDesDea 为直通包装。 */

#include "include/TenCrypt.h"

#include <string.h>

struct des_ks_struct {
    unsigned int ks[2];
};

typedef des_ks_struct des_key_schedule[16];

static const unsigned int DEA_PC1[8][256][2] = {
#include "dea_pc1.inc"
};

static const unsigned int DEA_PC2[8][256][2] = {
#include "dea_pc2.inc"
};

static const unsigned char DEA_LS[16] = {
#include "dea_ls.inc"
};

static const unsigned int DEA_IP[8][256][2] = {
#include "dea_ip.inc"
};

static const unsigned int IP_INV[8][256][2] = {
#include "ip_inv.inc"
};

static const unsigned int S_AND_P[8][64] = {
#include "s_and_p.inc"
};

static inline unsigned int dea_rotr28(unsigned int v, unsigned int n) {
    v &= 0x0fffffffu;
    return ((v >> n) | ((v & ((1u << n) - 1u)) << (28 - n))) & 0x0fffffffu;
}

// mangled: _Z9des_schedPhP13des_ks_struct
int des_sched(unsigned char *key, des_ks_struct *schedule) {
    unsigned int c, d, n, tmp, tmp2;
    unsigned char *out = (unsigned char *)schedule;
    int r;

    c = 0;
    d = 0;
    for (r = 0; r < 8; r++) {
        c |= DEA_PC1[r][key[r]][0];
        d |= DEA_PC1[r][key[r]][1];
    }
    for (n = 0; n < 16; n++) {
        int i;

        c = dea_rotr28(c, DEA_LS[n]);
        d = dea_rotr28(d, DEA_LS[n]);
        const unsigned char b[8] = {
            (unsigned char)c, (unsigned char)(c >> 8),
            (unsigned char)(c >> 16), (unsigned char)(c >> 24),
            (unsigned char)d, (unsigned char)(d >> 8),
            (unsigned char)(d >> 16), (unsigned char)(d >> 24)
        };
        tmp = 0;
        tmp2 = 0;
        for (i = 0; i < 8; i++) {
            tmp |= DEA_PC2[i][b[i]][0];
            tmp2 |= DEA_PC2[i][b[i]][1];
        }
        out[n * 8 + 0] = (unsigned char)tmp;
        out[n * 8 + 1] = (unsigned char)(tmp >> 8);
        out[n * 8 + 2] = (unsigned char)(tmp >> 16);
        out[n * 8 + 3] = (unsigned char)(tmp >> 24);
        out[n * 8 + 4] = (unsigned char)tmp2;
        out[n * 8 + 5] = (unsigned char)(tmp2 >> 8);
        out[n * 8 + 6] = (unsigned char)(tmp2 >> 16);
        out[n * 8 + 7] = (unsigned char)(tmp2 >> 24);
    }
    return 0;
}

static inline void dea_ip_block(const unsigned char *in, unsigned int *X,
                                unsigned int *Y) {
    unsigned int x = 0, y = 0;
    int r;

    for (r = 0; r < 8; r++) {
        x |= DEA_IP[r][in[r]][0];
        y |= DEA_IP[r][in[r]][1];
    }
    *X = x;
    *Y = y;
}

static inline unsigned int dea_feistel(unsigned int L, unsigned int R,
                                       const unsigned char *key8) {
    unsigned int idx[8];
    unsigned int F = 0;
    int i;

    idx[0] = ((R << 1) & 0x3e) | (((R >> 27) & 0x10) ? 1u : 0u);
    idx[1] = (R >> 3) & 0x3f;
    idx[2] = (R >> 7) & 0x3f;
    idx[3] = (R >> 11) & 0x3f;
    idx[4] = (R >> 15) & 0x3f;
    idx[5] = (R >> 19) & 0x3f;
    idx[6] = (R >> 23) & 0x3f;
    idx[7] = (R >> 27) & 0x1f;
    for (i = 0; i < 8; i++) {
        F |= S_AND_P[i][(idx[i] ^ key8[i]) & 0x3f];
    }
    return F ^ L;
}

static inline void dea_fp_block(unsigned int R, unsigned int L,
                                unsigned char *out) {
    const unsigned char comb[8] = {
        (unsigned char)R, (unsigned char)(R >> 8),
        (unsigned char)(R >> 16), (unsigned char)(R >> 24),
        (unsigned char)L, (unsigned char)(L >> 8),
        (unsigned char)(L >> 16), (unsigned char)(L >> 24)
    };
    unsigned int x = 0, y = 0;
    int r;

    for (r = 0; r < 8; r++) {
        x |= IP_INV[r][comb[r]][0];
        y |= IP_INV[r][comb[r]][1];
    }
    out[0] = (unsigned char)x;
    out[1] = (unsigned char)(x >> 8);
    out[2] = (unsigned char)(x >> 16);
    out[3] = (unsigned char)(x >> 24);
    out[4] = (unsigned char)y;
    out[5] = (unsigned char)(y >> 8);
    out[6] = (unsigned char)(y >> 16);
    out[7] = (unsigned char)(y >> 24);
}

// mangled: _Z8des_dea3PhS_PA16_13des_ks_structb
int des_dea3(unsigned char *input, unsigned char *output,
             des_key_schedule *schedule, bool encrypt) {
    unsigned int l, r, tmp;
    int i;

    dea_ip_block(input, &l, &r);
    if (encrypt) {
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[0] + i * 8);
            l = r;
            r = tmp;
        }
        tmp = l;
        l = r;
        r = tmp;
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[1] + (15 - i) * 8);
            l = r;
            r = tmp;
        }
        tmp = l;
        l = r;
        r = tmp;
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[0] + i * 8);
            l = r;
            r = tmp;
        }
    } else {
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[0] + (15 - i) * 8);
            l = r;
            r = tmp;
        }
        tmp = l;
        l = r;
        r = tmp;
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[1] + i * 8);
            l = r;
            r = tmp;
        }
        tmp = l;
        l = r;
        r = tmp;
        for (i = 0; i < 16; i++) {
            tmp = dea_feistel(l, r, (const unsigned char *)schedule[0] + (15 - i) * 8);
            l = r;
            r = tmp;
        }
    }
    dea_fp_block(r, l, output);
    return 0;
}

// mangled: _Z6DesDeaPhbS_j
void DesDea(unsigned char *key, bool benc, unsigned char *buf,
            unsigned int buflen) {
    des_ks_struct schedule[2][16];
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    des_sched(key, schedule[0]);
    des_sched(key + 8, schedule[1]);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        des_dea3(buf + i * 8, buf + i * 8, schedule, benc);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 0x10, buf);
}

// mangled: _Z9TenDesDeaPhbS_j
void TenDesDea(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    DesDea(key, benc, buf, buflen);
}
