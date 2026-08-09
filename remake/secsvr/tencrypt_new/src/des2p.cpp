/* des2p.cpp -- CDes2p：libdes 位操作版 DES（Eric Young 早期 des.c 结构）
   与二进制逐字节对照还原：
   - pkbit/unbit：64 位比特数组 <-> 8 字节打包
   - perm：按位号表置换（op[i]-1 下标，1 起始）
   - rot：字节数组左旋 bits 次（每次整体左移 1 字节，首字节补到末尾）
   - add2：逐字节 XOR
   - subs：6 比特输入 → ST[s*64+idx] 查表 → 4 比特输出
     idx = (i[0]<<5)|(i[5]<<4)|(i[1]<<3)|(i[3]<<1)|(i[2]<<2)|(i[4])
   - des_set_key：unbit 展开密钥 → PC1 置换（64 项表）→ 16 轮
     rot(C,28,LS[i]) + rot(D,28,LS[i]) → PC2 压缩 → ks[i][0..47]
   - des_func：IP 置换 → 16 轮 Feistel（rsto 即 lr+32，E 扩展 ⊕ 子钥 → 8×S
     → P 置换 ⊕ 左半 → 左右交换；enc 时子钥正序、dec 逆序）→ 最终交换 → FP
   - des_ecb_encrypt：unbit → des_func → pkbit，返回 8
   - Des2p：8 字节块循环 ECB + TsLocal->ProcessLastBytes(key, benc, buflen, 8, buf)
   所有表（LS/PC1/PC2/IP/E/ST/P/FP）从 secagent .rodata 提取，与标准 DES 表一致。 */

#include "inc/des2p.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char P2_LS[16] = {
#include "des2p_ls.inc"
};

static const unsigned char P2_PC1[64] = {
#include "des2p_pc1.inc"
};

static const unsigned char P2_PC2[48] = {
#include "des2p_pc2.inc"
};

static const unsigned char P2_IP[64] = {
#include "des2p_ip.inc"
};

static const unsigned char P2_E[48] = {
#include "des2p_e.inc"
};

static const unsigned char P2_ST[8][64] = {
#include "des2p_st.inc"
};

static const unsigned char P2_P[32] = {
#include "des2p_p.inc"
};

static const unsigned char P2_FP[64] = {
#include "des2p_fp.inc"
};

// mangled: _ZN6CDes2p5pkbitEPhS0_i
void CDes2p::pkbit(unsigned char *to, unsigned char *from, int len) {
    int j;
    int t;
    int i;

    for (i = 0; i < len; i++) {
        t = 0;
        for (j = 0; j < 8; j++)
            t |= from[j] << (7 - j);
        *to++ = (unsigned char)t;
        from += 8;
    }
}

// mangled: _ZN6CDes2p4permEPhPKhS0_i
void CDes2p::perm(unsigned char *to, const unsigned char *op,
                  unsigned char *from, int bits) {
    int i;

    for (i = 0; i < bits; i++)
        to[i] = from[op[i] - 1];
}

// mangled: _ZN6CDes2p3rotEPhii
void CDes2p::rot(unsigned char *r, int len, int bits) {
    int i;
    int j;
    int t;

    if (len <= 1 || bits <= 0)
        return;
    for (i = 0; i < bits; i++) {
        t = r[0];
        for (j = 1; j < len; j++)
            r[j - 1] = r[j];
        r[len - 1] = (unsigned char)t;
    }
}

// mangled: _ZN6CDes2p4add2EPhS0_i
void CDes2p::add2(unsigned char *to, unsigned char *from, int bits) {
    int i;

    for (i = 0; i < bits; i++)
        to[i] ^= from[i];
}

// mangled: _ZN6CDes2p4subsEPhS0_i
void CDes2p::subs(unsigned char *o, unsigned char *i, int s) {
    int t;

    t = (i[0] << 5) | (i[5] << 4) | (i[1] << 3) | (i[3] << 1) |
        (i[2] << 2) | i[4];
    t = P2_ST[s][t];
    o[0] = (unsigned char)((t >> 3) & 1);
    o[1] = (unsigned char)((t >> 2) & 1);
    o[2] = (unsigned char)((t >> 1) & 1);
    o[3] = (unsigned char)(t & 1);
}

// mangled: _ZN6CDes2p5unbitEPhS0_i
void CDes2p::unbit(unsigned char *to, unsigned char *from, int len) {
    int j;
    int f;

    for (j = 0; j < len; j++) {
        f = *from++;
        to[0] = (unsigned char)((f >> 7) & 1);
        to[1] = (unsigned char)((f >> 6) & 1);
        to[2] = (unsigned char)((f >> 5) & 1);
        to[3] = (unsigned char)((f >> 4) & 1);
        to[4] = (unsigned char)((f >> 3) & 1);
        to[5] = (unsigned char)((f >> 2) & 1);
        to[6] = (unsigned char)((f >> 1) & 1);
        to[7] = (unsigned char)(f & 1);
        to += 8;
    }
}

// mangled: _ZN6CDes2p11des_set_keyEPhP16des_key_schedule
int CDes2p::des_set_key(unsigned char *key, des_key_schedule *ks) {
    int i;
    int j;
    unsigned char k[64];
    unsigned char cd[64];

    unbit(k, key, 8);
    for (i = 0; i < 64; i++)
        cd[i] = k[P2_PC1[i] - 1];
    for (i = 0; i < 16; i++) {
        rot(cd, 28, P2_LS[i]);
        rot(cd + 28, 28, P2_LS[i]);
        for (j = 0; j < 48; j++)
            ks->b[i][j] = cd[P2_PC2[j] - 1];
    }
    return 1;
}

// mangled: _ZN6CDes2p18des_set_odd_parityEPh
void CDes2p::des_set_odd_parity(unsigned char *key) {
    int i;
    unsigned char c;
    unsigned char t;
    unsigned char p;

    for (i = 0; i < 8; i++) {
        c = key[i];
        t = (unsigned char)(c & 0xfe);
        p = (unsigned char)((c >> 4) ^ (c & 0x0e));
        p = (unsigned char)((p >> 2) ^ (p & 3));
        p = (unsigned char)((p >> 1) ^ (p & 1) ^ 1);
        key[i] = (unsigned char)(t | p);
    }
}

// mangled: _ZN6CDes2p8des_funcEPhS0_PA48_hb
void CDes2p::des_func(unsigned char *in, unsigned char *out,
                      unsigned char (*b)[48], bool enc) {
    unsigned char lr[64];
    unsigned char *rsto = lr + 32;
    unsigned char si[48];
    unsigned char so[32];
    unsigned char temp[32];
    int i;
    int r;

    for (i = 0; i < 64; i++)
        lr[i] = in[P2_IP[i] - 1];
    for (r = 0; r < 16; r++) {
        memmove(temp, rsto, 32);
        for (i = 0; i < 48; i++)
            si[i] = rsto[P2_E[i] - 1];
        for (i = 0; i < 48; i++)
            si[i] ^= b[enc ? r : 15 - r][i];
        for (i = 0; i < 8; i++)
            subs(so + 4 * i, si + 6 * i, i);
        for (i = 0; i < 32; i++)
            rsto[i] = so[P2_P[i] - 1];
        for (i = 0; i < 32; i++)
            rsto[i] ^= lr[i];
        memmove(lr, temp, 32);
    }
    memmove(lr, rsto, 32);
    memmove(rsto, temp, 32);
    for (i = 0; i < 64; i++)
        out[i] = lr[P2_FP[i] - 1];
}

// mangled: _ZN6CDes2p15des_ecb_encryptEPhS0_P16des_key_scheduleb
int CDes2p::des_ecb_encrypt(unsigned char *in, unsigned char *out,
                            des_key_schedule *ks, bool enc) {
    unsigned char o[64];

    unbit(o, in, 8);
    des_func(o, o, ks->b, enc);
    pkbit(out, o, 8);
    return 8;
}

// mangled: _ZN6CDes2p4InitEPvS0_
void CDes2p::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN6CDes2p6UnInitEv
void CDes2p::UnInit() {
}

// mangled: _ZN6CDes2p5Des2pEPhbS0_j
void CDes2p::Des2p(unsigned char *key, bool benc, unsigned char *buf,
                   unsigned int buflen) {
    des_key_schedule ks;
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    des_set_key(key, &ks);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++)
        des_ecb_encrypt(buf + i * 8, buf + i * 8, &ks, benc);
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN6CDes2pC1Ev / _ZN6CDes2pC2Ev
CDes2p::CDes2p() {
    Init(0, 0);
}

// mangled: _ZN6CDes2pD1Ev / _ZN6CDes2pD2Ev
CDes2p::~CDes2p() {
    UnInit();
}

// mangled: _Z8TenDes2PPhbS_j
void TenDes2P(unsigned char *key, bool benc, unsigned char *buf,
              unsigned int buflen) {
    CDes2p des2p;

    des2p.Des2p(key, benc, buf, buflen);
}
