/* loki91.cpp -- LOKI91 块密码（AC 源码集 LOKI/LOKI91.C 参考）
   与 LOKI89 同 f/s 结构，但 setkey 生成 16 个轮子钥（ROL12/ROL13 交替），
   加解密直接用子钥轮（无首尾密钥混合）。P/sfn 表与 LOKI89 相同。 */

#include "inc/loki91.h"
#include "include/TenCrypt.h"

#include <string.h>

#define ROL12(b) b = ((b << 12) | (b >> 20))
#define ROL13(b) b = ((b << 13) | (b >> 19))
#define ROR12(b) b = ((b >> 12) | (b << 20))
#define ROR13(b) b = ((b >> 13) | (b << 19))
#define MASK12 0x0fff
#define MSB 0x80000000u

static const unsigned char loki_P[32] = {
    31,23,15,7,30,22,14,6,29,21,13,5,28,20,12,4,
    27,19,11,3,26,18,10,2,25,17,9,1,24,16,8,0
};

struct sfn_desc91 {
    unsigned short gen;
    unsigned short exp;
};

static const sfn_desc91 loki91_sfn[] = {
#include "loki89_sfn.inc"
};

// mangled: _ZN7CLoki915mult8Ettt
unsigned short CLoki91::mult8(unsigned short a, unsigned short b, unsigned short gen) {
    unsigned short product = 0;

    while (b != 0) {
        if (b & 01)
            product ^= a;
        a <<= 1;
        if (a >= 256)
            a ^= gen;
        b >>= 1;
    }
    return product;
}

// mangled: _ZN7CLoki915exp_8Ettt
unsigned short CLoki91::exp_8(unsigned short base, unsigned short exponent, unsigned short gen) {
    unsigned short accum = base;
    unsigned short result = 1;

    if (base == 0)
        return 0;
    while (exponent != 0) {
        if ((exponent & 0x0001) == 0x0001)
            result = mult8(result, accum, gen);
        exponent >>= 1;
        accum = mult8(accum, accum, gen);
    }
    return result;
}

// mangled: _ZN7CLoki917perm32EPjS0_Ph
void CLoki91::perm32(unsigned int *out, unsigned int *in, unsigned char *perm) {
    unsigned int mask = MSB;
    int i;
    int o;
    int b;
    unsigned char *p;

    *out = 0;
    p = perm;
    for (o = 0; o < 32; o++) {
        i = (int)*p++;
        b = (*in >> i) & 01;
        if (b)
            *out |= mask;
        mask >>= 1;
    }
}

// mangled: _ZN7CLoki911sEj
unsigned short CLoki91::s(unsigned int i) {
    unsigned short r;
    unsigned short c;
    unsigned short v;
    unsigned short t;

    r = (unsigned short)(((i >> 8) & 0xc) | (i & 0x3));
    c = (unsigned short)((i >> 2) & 0xff);
    t = (unsigned short)((c + ((r * 17) ^ 0xff)) & 0xff);
    v = exp_8(t, sfn[r].exp, sfn[r].gen);
    return v;
}

// mangled: _ZN7CLoki911fEjj
unsigned int CLoki91::f(unsigned int r, unsigned int k) {
    unsigned int a;
    unsigned int b;
    unsigned int c;

    a = r ^ k;
    b = ((unsigned int)s((a & MASK12))) |
        ((unsigned int)s(((a >> 8) & MASK12)) << 8) |
        ((unsigned int)s(((a >> 16) & MASK12)) << 16) |
        ((unsigned int)s((((a >> 24) | (a << 8)) & MASK12)) << 24);
    perm32(&c, &b, P);
    return c;
}

// mangled: _ZN7CLoki916setkeyEPh
void CLoki91::setkey(unsigned char *key) {
    unsigned int i;
    unsigned int KL;
    unsigned int KR;
    unsigned char c;

    c = key[0]; key[0] = key[3]; key[3] = c;
    c = key[1]; key[1] = key[2]; key[2] = c;
    c = key[4]; key[4] = key[7]; key[7] = c;
    c = key[5]; key[5] = key[6]; key[6] = c;
    lokikey[0] = ((unsigned int *)key)[0];
    lokikey[1] = ((unsigned int *)key)[1];
    KL = lokikey[0];
    KR = lokikey[1];
    for (i = 0; i < 16; i += 4) {
        subkeys[i] = KL;
        ROL12(KL);
        subkeys[i + 1] = KL;
        ROL13(KL);
        subkeys[i + 2] = KR;
        ROL12(KR);
        subkeys[i + 3] = KR;
        ROL13(KR);
    }
    c = key[0]; key[0] = key[3]; key[3] = c;
    c = key[1]; key[1] = key[2]; key[2] = c;
    c = key[4]; key[4] = key[7]; key[7] = c;
    c = key[5]; key[5] = key[6]; key[6] = c;
}

// mangled: _ZN7CLoki9112blockEncryptEPh
void CLoki91::blockEncrypt(unsigned char *b) {
    unsigned int i;
    unsigned int L;
    unsigned int R;
    unsigned char c;

    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
    L = ((unsigned int *)b)[0];
    R = ((unsigned int *)b)[1];
    for (i = 0; i < 16; i += 2) {
        L ^= f(R, subkeys[i]);
        R ^= f(L, subkeys[i + 1]);
    }
    ((unsigned int *)b)[0] = R;
    ((unsigned int *)b)[1] = L;
    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
}

// mangled: _ZN7CLoki9112blockDecryptEPh
void CLoki91::blockDecrypt(unsigned char *b) {
    unsigned int i;
    unsigned int L;
    unsigned int R;
    unsigned char c;

    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
    L = ((unsigned int *)b)[0];
    R = ((unsigned int *)b)[1];
    for (i = 16; i > 0; i -= 2) {
        L ^= f(R, subkeys[i - 1]);
        R ^= f(L, subkeys[i - 2]);
    }
    ((unsigned int *)b)[0] = R;
    ((unsigned int *)b)[1] = L;
    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
}

// mangled: _ZN7CLoki914InitEPvS0_
void CLoki91::Init(void *pSys, void *pLocal) {
    unsigned char _P[32];
    sfn_desc91 _sfn[17];

    m_pSys = pSys;
    m_pLocal = pLocal;
    lokikey[0] = 0;
    lokikey[1] = 0;
    P = 0;
    subkeys = 0;
    sfn = 0;
    if (P == 0)
        P = new unsigned char[32];
    if (subkeys == 0)
        subkeys = new unsigned int[16];
    if (sfn == 0)
        sfn = new sfn_desc91[17];
    memcpy(_P, loki_P, 32);
    memmove(P, _P, 32);
    memcpy(_sfn, loki91_sfn, 17 * sizeof(sfn_desc91));
    memmove(sfn, _sfn, 17 * sizeof(sfn_desc91));
}

// mangled: _ZN7CLoki916UnInitEv
void CLoki91::UnInit() {
    if (P != 0) {
        delete[] P;
        P = 0;
    }
    if (subkeys != 0) {
        delete[] subkeys;
        subkeys = 0;
    }
    if (sfn != 0) {
        delete[] sfn;
        sfn = 0;
    }
}

// mangled: _ZN7CLoki915Loki91EPhbS0_j
void CLoki91::Loki91(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        setkey(key);
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++) {
            if (benc)
                blockEncrypt(buf + i * 8);
            else
                blockDecrypt(buf + i * 8);
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 8, buflen - nLen * 8);
    }
}

// mangled: _ZN7CLoki91C1Ev / _ZN7CLoki91C2Ev
CLoki91::CLoki91() {
    Init(0, 0);
}

// mangled: _ZN7CLoki91D1Ev / _ZN7CLoki91D2Ev
CLoki91::~CLoki91() {
    UnInit();
}
