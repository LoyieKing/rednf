/* rc5.cpp -- RC5-32/12/16 块密码（类封装）
   与二进制逐字节对照：26 个子钥（12 轮）、16 字节密钥、块 8 字节。
   参考：Kelsey 公版 RC5（AC 源码集 RC5-KEL/RC5.C）。 */

#include "inc/rc5.h"
#include "include/TenCrypt.h"

#include <string.h>

#define ROTL32(X, C) (((X) << (C)) | ((X) >> (32 - (C))))
#define ROTR32(X, C) (((X) >> (C)) | ((X) << (32 - (C))))

// mangled: _ZN4CRc57set_keyEPh
void CRc5::set_key(unsigned char *K) {
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int u;
    unsigned int A;
    unsigned int B;
    unsigned int L[4];
    unsigned int P;
    unsigned int Q;

    /* 16 字节密钥 → 4 个 LE 字 */
    for (i = 0; i < 4; i++)
        L[i] = ((unsigned int)K[4 * i + 3] << 24) | ((unsigned int)K[4 * i + 2] << 16)
             | ((unsigned int)K[4 * i + 1] << 8) | (unsigned int)K[4 * i];

    S[0] = 0xb7e15163;
    for (i = 1; i < 26; i++)
        S[i] = S[i - 1] + 0x9e3779b9;

    A = B = 0;
    i = 0;
    j = 0;
    for (k = 0; k < 78; k++) {
        A = S[i] = ROTL32(S[i] + A + B, 3);
        B = L[j] = ROTL32(L[j] + A + B, (A + B) & 31);
        i = (i + 1) % 26;
        j = (j + 1) % 4;
    }
}

// mangled: _ZN4CRc57encryptEPjS0_
void CRc5::encrypt(unsigned int *pt, unsigned int *ct) {
    unsigned int i;
    unsigned int A;
    unsigned int B;

    A = pt[0] + S[0];
    B = pt[1] + S[1];
    for (i = 0; i < 12; i++) {
        A = ROTL32(A ^ B, B & 31) + S[2 * i + 2];
        B = ROTL32(B ^ A, A & 31) + S[2 * i + 3];
    }
    ct[0] = A;
    ct[1] = B;
}

// mangled: _ZN4CRc57decryptEPjS0_
void CRc5::decrypt(unsigned int *ct, unsigned int *pt) {
    unsigned int i;
    unsigned int B;
    unsigned int A;

    A = ct[0];
    B = ct[1];
    for (i = 12; i > 0; i--) {
        B = ROTR32(B - S[2 * i + 1], A & 31) ^ A;
        A = ROTR32(A - S[2 * i], B & 31) ^ B;
    }
    pt[0] = A - S[0];
    pt[1] = B - S[1];
}

// mangled: _ZN4CRc53Rc5EPhbS0_j
void CRc5::Rc5(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
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

// mangled: _ZN4CRc54InitEPvS0_
void CRc5::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
    S = new unsigned int[26];
    memset(S, 0, 26 * sizeof(unsigned int));
}

// mangled: _ZN4CRc56UnInitEv
void CRc5::UnInit() {
    if (S != 0) {
        delete[] S;
        S = 0;
    }
}

// mangled: _ZN4CRc5C1Ev / _ZN4CRc5C2Ev
CRc5::CRc5() {
    Init(0, 0);
}

// mangled: _ZN4CRc5D1Ev / _ZN4CRc5D2Ev
CRc5::~CRc5() {
    UnInit();
}
