/* rc6.cpp -- RC6 块密码（Gladman 参考结构 + 二进制逐字节核对）
   注意二进制差异：set_key 的 L[0..7] 只装入密钥前 8 个字节（每字节一个字），
   后 8 字节被忽略（已实测验证）；混合 132 轮。 */

#include "inc/rc6.h"
#include "include/TenCrypt.h"

#define ROTL32(X, C) (((X) << (C)) | ((X) >> (32 - (C))))
#define ROTR32(X, C) (((X) >> (C)) | ((X) << (32 - (C))))

#define f_rnd(i, a, b, c, d)                                 \
    u = ROTL32(d * (d + d + 1), 5);                          \
    t = ROTL32(b * (b + b + 1), 5);                          \
    a = ROTL32(a ^ t, u & 31) + l_key[i];                    \
    c = ROTL32(c ^ u, t & 31) + l_key[i + 1]

#define i_rnd(i, a, b, c, d)                                 \
    u = ROTL32(d * (d + d + 1), 5);                          \
    t = ROTL32(b * (b + b + 1), 5);                          \
    c = ROTR32(c - l_key[i + 1], t & 31) ^ u;                \
    a = ROTR32(a - l_key[i], u & 31) ^ t

// mangled: _ZN4CRc67set_keyEPh
unsigned int *CRc6::set_key(unsigned char *key_blk) {
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int a;
    unsigned int b;
    unsigned int l[8];

    /* 二进制确认：仅使用密钥前 8 字节，逐字节装入 l[0..7] */
    for (i = 0; i < 8; i++)
        l[i] = key_blk[i];

    l_key[0] = 0xb7e15163;
    for (k = 1; k < 44; k++)
        l_key[k] = l_key[k - 1] + 0x9e3779b9;

    a = b = i = j = 0;
    for (k = 0; k < 132; k++) {
        a = l_key[i] = ROTL32(l_key[i] + a + b, 3);
        b = l[j] = ROTL32(l[j] + a + b, (a + b) & 31);
        i = (i + 1) % 44;
        j = (j + 1) % 8;
    }
    return l_key;
}

// mangled: _ZN4CRc67encryptEPjS0_
void CRc6::encrypt(unsigned int *in_blk, unsigned int *out_blk) {
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int t;
    unsigned int u;

    a = in_blk[0];
    b = in_blk[1] + l_key[0];
    c = in_blk[2];
    d = in_blk[3] + l_key[1];

    f_rnd(2, a, b, c, d);
    f_rnd(4, b, c, d, a);
    f_rnd(6, c, d, a, b);
    f_rnd(8, d, a, b, c);
    f_rnd(10, a, b, c, d);
    f_rnd(12, b, c, d, a);
    f_rnd(14, c, d, a, b);
    f_rnd(16, d, a, b, c);
    f_rnd(18, a, b, c, d);
    f_rnd(20, b, c, d, a);
    f_rnd(22, c, d, a, b);
    f_rnd(24, d, a, b, c);
    f_rnd(26, a, b, c, d);
    f_rnd(28, b, c, d, a);
    f_rnd(30, c, d, a, b);
    f_rnd(32, d, a, b, c);
    f_rnd(34, a, b, c, d);
    f_rnd(36, b, c, d, a);
    f_rnd(38, c, d, a, b);
    f_rnd(40, d, a, b, c);

    out_blk[0] = a + l_key[42];
    out_blk[1] = b;
    out_blk[2] = c + l_key[43];
    out_blk[3] = d;
}

// mangled: _ZN4CRc67decryptEPjS0_
void CRc6::decrypt(unsigned int *in_blk, unsigned int *out_blk) {
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int t;
    unsigned int u;

    d = in_blk[3];
    c = in_blk[2] - l_key[43];
    b = in_blk[1];
    a = in_blk[0] - l_key[42];

    i_rnd(40, d, a, b, c);
    i_rnd(38, c, d, a, b);
    i_rnd(36, b, c, d, a);
    i_rnd(34, a, b, c, d);
    i_rnd(32, d, a, b, c);
    i_rnd(30, c, d, a, b);
    i_rnd(28, b, c, d, a);
    i_rnd(26, a, b, c, d);
    i_rnd(24, d, a, b, c);
    i_rnd(22, c, d, a, b);
    i_rnd(20, b, c, d, a);
    i_rnd(18, a, b, c, d);
    i_rnd(16, d, a, b, c);
    i_rnd(14, c, d, a, b);
    i_rnd(12, b, c, d, a);
    i_rnd(10, a, b, c, d);
    i_rnd(8, d, a, b, c);
    i_rnd(6, c, d, a, b);
    i_rnd(4, b, c, d, a);
    i_rnd(2, a, b, c, d);

    /* 二进制核对：加密侧 b=in[1]+l_key[0]、d=in[3]+l_key[1]，
       故解密输出为 a、(b-l_key[0])、c、(d-l_key[1]) */
    out_blk[0] = a;
    out_blk[1] = b - l_key[0];
    out_blk[2] = c;
    out_blk[3] = d - l_key[1];
}

// mangled: _ZN4CRc63Rc6EPhbS0_j
void CRc6::Rc6(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        set_key(key);
        nLen = buflen / 16;
        for (i = 0; i < nLen; i++) {
            if (benc)
                encrypt((unsigned int *)(buf + i * 16), (unsigned int *)(buf + i * 16));
            else
                decrypt((unsigned int *)(buf + i * 16), (unsigned int *)(buf + i * 16));
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
    }
}

// mangled: _ZN4CRc64InitEPvS0_
void CRc6::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN4CRc66UnInitEv
void CRc6::UnInit() {
}

// mangled: _ZN4CRc6C1Ev / _ZN4CRc6C2Ev
CRc6::CRc6() {
    Init(0, 0);
}

// mangled: _ZN4CRc6D1Ev / _ZN4CRc6D2Ev
CRc6::~CRc6() {
    UnInit();
}

// mangled: _Z6TenRc6PhbS_j
void TenRc6(unsigned char *key, bool benc, unsigned char *buf,
            unsigned int buflen) {
    CRc6 rc6;

    rc6.Rc6(key, benc, buf, buflen);
}
