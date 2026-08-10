/* 3way.cpp -- 3-Way 块密码（Joan Daemen 原始 C 规范，类封装）
   参考：Applied Cryptography 源码集 3-WAY/3-way.c（函数名/局部变量与
   DWARF 桩一一对应）。块 12 字节，11 轮。 */

#include "inc/3way.h"
#include "include/TenCrypt.h"

#define STRT_E 0x0b0b
#define STRT_D 0xb1b1
#define NMBR   11

// mangled: _ZN9CThreeWay2muEPj
void CThreeWay::mu(unsigned int *a) {
    int i;
    unsigned int b[3];

    b[0] = b[1] = b[2] = 0;
    for (i = 0; i < 32; i++) {
        b[0] <<= 1;
        b[1] <<= 1;
        b[2] <<= 1;
        if (a[0] & 1) b[2] |= 1;
        if (a[1] & 1) b[1] |= 1;
        if (a[2] & 1) b[0] |= 1;
        a[0] >>= 1;
        a[1] >>= 1;
        a[2] >>= 1;
    }
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
}

// mangled: _ZN9CThreeWay5gammaEPj
void CThreeWay::gamma(unsigned int *a) {
    unsigned int b[3];

    b[0] = a[0] ^ (a[1] | (~a[2]));
    b[1] = a[1] ^ (a[2] | (~a[0]));
    b[2] = a[2] ^ (a[0] | (~a[1]));
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
}

// mangled: _ZN9CThreeWay5thetaEPj
void CThreeWay::theta(unsigned int *a) {
    unsigned int b[3];

    b[0] = a[0] ^ (a[0] >> 16) ^ (a[1] << 16) ^ (a[1] >> 16) ^ (a[2] << 16) ^
                  (a[1] >> 24) ^ (a[2] << 8)  ^ (a[2] >> 8)  ^ (a[0] << 24) ^
                  (a[2] >> 16) ^ (a[0] << 16) ^ (a[2] >> 24) ^ (a[0] << 8);
    b[1] = a[1] ^ (a[1] >> 16) ^ (a[2] << 16) ^ (a[2] >> 16) ^ (a[0] << 16) ^
                  (a[2] >> 24) ^ (a[0] << 8)  ^ (a[0] >> 8)  ^ (a[1] << 24) ^
                  (a[0] >> 16) ^ (a[1] << 16) ^ (a[0] >> 24) ^ (a[1] << 8);
    b[2] = a[2] ^ (a[2] >> 16) ^ (a[0] << 16) ^ (a[0] >> 16) ^ (a[1] << 16) ^
                  (a[0] >> 24) ^ (a[1] << 8)  ^ (a[1] >> 8)  ^ (a[2] << 24) ^
                  (a[1] >> 16) ^ (a[2] << 16) ^ (a[1] >> 24) ^ (a[2] << 8);
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
}

// mangled: _ZN9CThreeWay5pi_1EPj
void CThreeWay::pi_1(unsigned int *a) {
    a[0] = (a[0] >> 10) ^ (a[0] << 22);
    a[2] = (a[2] << 1) ^ (a[2] >> 31);
}

// mangled: _ZN9CThreeWay5pi_2EPj
void CThreeWay::pi_2(unsigned int *a) {
    a[0] = (a[0] << 1) ^ (a[0] >> 31);
    a[2] = (a[2] >> 10) ^ (a[2] << 22);
}

// mangled: _ZN9CThreeWay3rhoEPj
void CThreeWay::rho(unsigned int *a) {
    theta(a);
    pi_1(a);
    gamma(a);
    pi_2(a);
}

// mangled: _ZN9CThreeWay10rndcon_genEjPj
void CThreeWay::rndcon_gen(unsigned int strt, unsigned int *rtab) {
    int i;

    for (i = 0; i <= NMBR; i++) {
        rtab[i] = strt;
        strt <<= 1;
        if (strt & 0x10000)
            strt ^= 0x11011;
    }
}

// mangled: _ZN9CThreeWay7EncryptEPjS0_
void CThreeWay::Encrypt(unsigned int *k, unsigned int *a) {
    int i;
    unsigned int rcon[NMBR + 1];

    rndcon_gen(STRT_E, rcon);
    for (i = 0; i < NMBR; i++) {
        a[0] ^= k[0] ^ (rcon[i] << 16);
        a[1] ^= k[1];
        a[2] ^= k[2] ^ rcon[i];
        rho(a);
    }
    a[0] ^= k[0] ^ (rcon[NMBR] << 16);
    a[1] ^= k[1];
    a[2] ^= k[2] ^ rcon[NMBR];
    theta(a);
}

// mangled: _ZN9CThreeWay7DecryptEPjS0_
void CThreeWay::Decrypt(unsigned int *k, unsigned int *a) {
    int i;
    unsigned int ki[3];
    unsigned int rcon[NMBR + 1];

    ki[0] = k[0];
    ki[1] = k[1];
    ki[2] = k[2];
    theta(ki);
    mu(ki);
    rndcon_gen(STRT_D, rcon);
    mu(a);
    for (i = 0; i < NMBR; i++) {
        a[0] ^= ki[0] ^ (rcon[i] << 16);
        a[1] ^= ki[1];
        a[2] ^= ki[2] ^ rcon[i];
        rho(a);
    }
    a[0] ^= ki[0] ^ (rcon[NMBR] << 16);
    a[1] ^= ki[1];
    a[2] ^= ki[2] ^ rcon[NMBR];
    theta(a);
    mu(a);
}

// mangled: _ZN9CThreeWay8ThreeWayEPhbS0_j
void CThreeWay::ThreeWay(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        nLen = buflen / 12;
        for (i = 0; i < nLen; i++) {
            if (benc)
                Encrypt((unsigned int *)key, (unsigned int *)(buf + i * 12));
            else
                Decrypt((unsigned int *)key, (unsigned int *)(buf + i * 12));
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 12, buflen - nLen * 12);
    }
}

// mangled: _ZN9CThreeWay4InitEPvS0_
void CThreeWay::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN9CThreeWay6UnInitEv
void CThreeWay::UnInit() {
}

// mangled: _ZN9CThreeWayC1Ev / _ZN9CThreeWayC2Ev
CThreeWay::CThreeWay() {
    Init(0, 0);
}

// mangled: _ZN9CThreeWayD1Ev / _ZN9CThreeWayD2Ev
CThreeWay::~CThreeWay() {
    UnInit();
}

// mangled: _Z11TenThreeWayPhbS_j
void TenThreeWay(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    CThreeWay threeway;

    threeway.ThreeWay(key, benc, buf, buflen);
}
