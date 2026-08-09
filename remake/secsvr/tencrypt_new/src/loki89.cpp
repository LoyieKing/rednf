/* loki89.cpp -- LOKI89 块密码（AC 源码集 LOKI/LOKI89.C 参考）
   P 表 Init 内联（标准值）；sfn 表从二进制 .rodata 0x081ccfe0 提取。 */

#include "inc/loki89.h"
#include "include/TenCrypt.h"

#include <string.h>

#define ROL12(b) b = ((b << 12) | (b >> 20))
#define ROR12(b) b = ((b >> 12) | (b << 20))
#define MASK12 0x0fff
#define MSB 0x80000000u

static const unsigned char loki89_P[32] = {
    31,23,15,7,30,22,14,6,29,21,13,5,28,20,12,4,
    27,19,11,3,26,18,10,2,25,17,9,1,24,16,8,0
};

struct sfn_desc {
    unsigned short gen;
    unsigned short exp;
};

static const sfn_desc loki89_sfn[] = {
#include "loki89_sfn.inc"
};

// mangled: _ZN7CLoki895mult8Et tt
unsigned short CLoki89::mult8(unsigned short a, unsigned short b, unsigned short gen) {
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

// mangled: _ZN7CLoki895exp_8Ettt
unsigned short CLoki89::exp_8(unsigned short base, unsigned short exponent, unsigned short gen) {
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

// mangled: _ZN7CLoki897perm32EPjS0_Ph
void CLoki89::perm32(unsigned int *out, unsigned int *in, unsigned char *perm) {
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

// mangled: _ZN7CLoki891sEj
unsigned short CLoki89::s(unsigned int i) {
    unsigned short r;
    unsigned short c;
    unsigned short v;
    unsigned short t;

    r = (unsigned short)(((i >> 8) & 0xc) | (i & 0x3));
    c = (unsigned short)((i >> 2) & 0xff);
    t = (unsigned short)(c ^ r);
    v = exp_8(t, sfn[r].exp, sfn[r].gen);
    return v;
}

// mangled: _ZN7CLoki891fEjj
unsigned int CLoki89::f(unsigned int r, unsigned int k) {
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

// mangled: _ZN7CLoki896setkeyEPh
void CLoki89::setkey(unsigned char *key) {
    unsigned char c;
    unsigned char c2;

    c = key[0]; key[0] = key[3]; key[3] = c;
    c = key[1]; key[1] = key[2]; key[2] = c;
    c = key[4]; key[4] = key[7]; key[7] = c;
    c = key[5]; key[5] = key[6]; key[6] = c;
    lokikey[0] = ((unsigned int *)key)[0];
    lokikey[1] = ((unsigned int *)key)[1];
    c = key[0]; key[0] = key[3]; key[3] = c;
    c = key[1]; key[1] = key[2]; key[2] = c;
    c = key[4]; key[4] = key[7]; key[7] = c;
    c = key[5]; key[5] = key[6]; key[6] = c;
}

// mangled: _ZN7CLoki8912blockEncryptEPh
void CLoki89::blockEncrypt(unsigned char *b) {
    unsigned int L;
    unsigned int R;
    unsigned int KL;
    unsigned int KR;
    unsigned char c;
    int i;

    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
    KL = lokikey[0];
    KR = lokikey[1];
    L = ((unsigned int *)b)[0] ^ KL;
    R = ((unsigned int *)b)[1] ^ KR;
    for (i = 0; i < 8; i++) {
        L ^= f(R, KL); ROL12(KL);
        R ^= f(L, KR); ROL12(KR);
    }
    ((unsigned int *)b)[0] = R ^ KR;
    ((unsigned int *)b)[1] = L ^ KL;
    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
}

// mangled: _ZN7CLoki8912blockDecryptEPh
void CLoki89::blockDecrypt(unsigned char *b) {
    unsigned int L;
    unsigned int R;
    unsigned int KL;
    unsigned int KR;
    unsigned char c;
    int i;

    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
    KL = lokikey[0];
    KR = lokikey[1];
    L = ((unsigned int *)b)[0] ^ KR;
    R = ((unsigned int *)b)[1] ^ KL;
    for (i = 0; i < 8; i++) {
        ROR12(KR); L ^= f(R, KR);
        ROR12(KL); R ^= f(L, KL);
    }
    ((unsigned int *)b)[0] = R ^ KL;
    ((unsigned int *)b)[1] = L ^ KR;
    c = b[0]; b[0] = b[3]; b[3] = c;
    c = b[1]; b[1] = b[2]; b[2] = c;
    c = b[4]; b[4] = b[7]; b[7] = c;
    c = b[5]; b[5] = b[6]; b[6] = c;
}

// mangled: _ZN7CLoki894InitEPvS0_
void CLoki89::Init(void *pSys, void *pLocal) {
    unsigned char _P[32];
    sfn_desc _sfn[17];

    m_pSys = pSys;
    m_pLocal = pLocal;
    lokikey[0] = 0;
    lokikey[1] = 0;
    P = 0;
    sfn = 0;
    if (P == 0)
        P = new unsigned char[32];
    if (sfn == 0)
        sfn = new sfn_desc[17];
    memcpy(_P, loki89_P, 32);
    memmove(P, _P, 32);
    memcpy(_sfn, loki89_sfn, 17 * sizeof(sfn_desc));
    memmove(sfn, _sfn, 17 * sizeof(sfn_desc));
}

// mangled: _ZN7CLoki896UnInitEv
void CLoki89::UnInit() {
    if (P != 0) {
        delete[] P;
        P = 0;
    }
    if (sfn != 0) {
        delete[] sfn;
        sfn = 0;
    }
}

// mangled: _ZN7CLoki895Loki89EPhbS0_j
void CLoki89::Loki89(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
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

// mangled: _ZN7CLoki89C1Ev / _ZN7CLoki89C2Ev
CLoki89::CLoki89() {
    Init(0, 0);
}

// mangled: _ZN7CLoki89D1Ev / _ZN7CLoki89D2Ev
CLoki89::~CLoki89() {
    UnInit();
}
