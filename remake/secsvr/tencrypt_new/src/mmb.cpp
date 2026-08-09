/* mmb.cpp -- MMB 块密码（标准 MMB + 自定义 SetKey 派生，与二进制逐字节核对）
   参考：AC 源码集 MMB/Mmb.c（modmult/f/g/Encrypt/Decrypt）。
   SetKey 由反汇编还原（常数 0x33cfc738/0x19860719 + iv_tbl[64] 查表）。 */

#include "inc/mmb.h"
#include "include/TenCrypt.h"

#include <string.h>

#define gamma0 0x025F1CDBUL
#define gamma1 0x04BE39B6UL
#define gamma2 0x12F8E6D8UL
#define gamma3 0x2F8E6D81UL

#define ammag0 0x0DAD4694UL
#define ammag1 0x06D6A34AUL
#define ammag2 0x81B5A8D2UL
#define ammag3 0x281B5A8DUL

#define delta 0x2AAAAAAAUL

#define LO(z) ((z) & 0xFFFFUL)
#define HI(z) ((z) >> 16)

static const unsigned int mmb_ivtbl[64] = {
#include "mmb_ivtbl.inc"
};

// mangled: _ZN4CMmb8ModMultEjj
unsigned int CMmb::ModMult(unsigned int x, unsigned int y) {
    unsigned int z0;
    unsigned int z1;
    unsigned int q;

    z0 = LO(x) * LO(y);
    z1 = HI(x) * LO(y) + HI(z0);
    q = LO(z0) + (LO(z1) << 16);
    z0 = LO(q) + HI(z1);
    z1 = LO(x) * HI(y) + HI(q) + HI(z0);
    q = LO(z0) + (LO(z1) << 16);
    z0 = HI(x) * HI(y) + LO(q) + HI(z1);
    z1 = HI(q) + HI(z0);
    return LO(z0) + (LO(z1) << 16) + HI(z1);
}

// mangled: _ZN4CMmb1fEPj
void CMmb::f(unsigned int *x) {
    unsigned int t0;
    unsigned int t1;

    x[0] = ModMult(x[0], gamma0);
    x[1] = ModMult(x[1], gamma1);
    x[2] = ModMult(x[2], gamma2);
    x[3] = ModMult(x[3], gamma3);
    if (x[0] & 1)
        x[0] ^= delta;
    t0 = x[0] ^ x[2];
    t1 = x[1] ^ x[3];
    x[0] ^= t1;
    x[1] ^= t0;
    x[2] ^= t1;
    x[3] ^= t0;
}

// mangled: _ZN4CMmb1gEPj
void CMmb::g(unsigned int *x) {
    unsigned int t0;
    unsigned int t1;

    t0 = x[0] ^ x[2];
    t1 = x[1] ^ x[3];
    x[0] ^= t1;
    x[1] ^= t0;
    x[2] ^= t1;
    x[3] ^= t0;
    if (x[0] & 1)
        x[0] ^= delta;
    x[0] = ModMult(x[0], ammag0);
    x[1] = ModMult(x[1], ammag1);
    x[2] = ModMult(x[2], ammag2);
    x[3] = ModMult(x[3], ammag3);
}

// mangled: _ZN4CMmb6SetKeyEPj
void CMmb::SetKey(unsigned int *in_key) {
    unsigned int blk[4];
    unsigned int fbk;
    unsigned int x;
    unsigned int a, b, c, d, e, f, g;

    a = in_key[0] - 0x33cfc738;
    b = ((a << 5) | (a >> 27)) + in_key[1];
    c = ((b << 7) | (b >> 25)) + in_key[2];
    d = ((c << 11) | (c >> 21)) + in_key[3];
    e = ((((d << 2) | (d >> 30)) + iv_tbl[(((d << 1) | (d >> 31)) & 0x3f)]) ^ 0x19860719);
    e = ((e << 1) | (e >> 31)) ^ a;
    f = ((e << 2) | (e >> 30)) ^ b;
    g = ((f << 3) | (f >> 29)) ^ c;
    key[0] = e;
    key[1] = e ^ f;
    key[2] = (e ^ f) ^ g;
    key[3] = d ^ ((g << 4) | (g >> 28)) ^ key[2];
}

// mangled: _ZN4CMmb7EncryptEPj
void CMmb::Encrypt(unsigned int *x) {
    x[0] ^= key[0]; x[1] ^= key[1]; x[2] ^= key[2]; x[3] ^= key[3];
    f(x);
    x[0] ^= key[1]; x[1] ^= key[2]; x[2] ^= key[3]; x[3] ^= key[0];
    f(x);
    x[0] ^= key[2]; x[1] ^= key[3]; x[2] ^= key[0]; x[3] ^= key[1];
    f(x);
    x[0] ^= key[3]; x[1] ^= key[0]; x[2] ^= key[1]; x[3] ^= key[2];
    f(x);
    x[0] ^= key[0]; x[1] ^= key[1]; x[2] ^= key[2]; x[3] ^= key[3];
    f(x);
    x[0] ^= key[1]; x[1] ^= key[2]; x[2] ^= key[3]; x[3] ^= key[0];
    f(x);
    x[0] ^= key[2]; x[1] ^= key[3]; x[2] ^= key[0]; x[3] ^= key[1];
}

// mangled: _ZN4CMmb7DecryptEPj
void CMmb::Decrypt(unsigned int *x) {
    x[0] ^= key[2]; x[1] ^= key[3]; x[2] ^= key[0]; x[3] ^= key[1];
    g(x);
    x[0] ^= key[1]; x[1] ^= key[2]; x[2] ^= key[3]; x[3] ^= key[0];
    g(x);
    x[0] ^= key[0]; x[1] ^= key[1]; x[2] ^= key[2]; x[3] ^= key[3];
    g(x);
    x[0] ^= key[3]; x[1] ^= key[0]; x[2] ^= key[1]; x[3] ^= key[2];
    g(x);
    x[0] ^= key[2]; x[1] ^= key[3]; x[2] ^= key[0]; x[3] ^= key[1];
    g(x);
    x[0] ^= key[1]; x[1] ^= key[2]; x[2] ^= key[3]; x[3] ^= key[0];
    g(x);
    x[0] ^= key[0]; x[1] ^= key[1]; x[2] ^= key[2]; x[3] ^= key[3];
}

// mangled: _ZN4CMmb3MmbEPhbS0_j
void CMmb::Mmb(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        SetKey((unsigned int *)key);
        nLen = buflen / 16;
        for (i = 0; i < nLen; i++) {
            if (benc)
                Encrypt((unsigned int *)(buf + i * 16));
            else
                Decrypt((unsigned int *)(buf + i * 16));
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
    }
}

// mangled: _ZN4CMmb4InitEPvS0_
void CMmb::Init(void *pSys, void *pLocal) {
    unsigned int _iv_tbl[64];

    m_pSys = pSys;
    m_pLocal = pLocal;
    key = 0;
    iv_tbl = 0;
    if (key == 0)
        key = new unsigned int[4];
    if (iv_tbl == 0)
        iv_tbl = new unsigned int[64];
    memcpy(_iv_tbl, mmb_ivtbl, 64 * sizeof(unsigned int));
    memmove(iv_tbl, _iv_tbl, 64 * sizeof(unsigned int));
}

// mangled: _ZN4CMmb6UnInitEv
void CMmb::UnInit() {
    if (key != 0) {
        delete[] key;
        key = 0;
    }
    if (iv_tbl != 0) {
        delete[] iv_tbl;
        iv_tbl = 0;
    }
}

// mangled: _ZN4CMmbC1Ev / _ZN4CMmbC2Ev
CMmb::CMmb() {
    Init(0, 0);
}

// mangled: _ZN4CMmbD1Ev / _ZN4CMmbD2Ev
CMmb::~CMmb() {
    UnInit();
}
