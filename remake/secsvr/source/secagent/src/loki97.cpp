// Auto-generated stub from DWARF info
// Original source: src/loki97.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/loki97.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 148
int CLoki97::mult(int a, int b, int g, int n) {
    // local: int p;
}

// line 128
int CLoki97::exp3(int b, int g, int n) {
    // local: int r;
}

// line 562
._30 CLoki97::f(._30 A, ._30 B) {
    // local: ._30 f;
    // local: ._30 d;
    // local: ._30 e;
    // local: ._30 f;
    // local: int s;
}

// line 617
._30 CLoki97::add64(._30 a, ._30 b) {
    // local: ._30 sum;
    // local: ._30 sum;
}

// line 504
int CLoki97::deCFB1(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int b;
    // local: unsigned char msgbit;
    // local: unsigned char prev;
    // local: unsigned char keybit;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 336
int CLoki97::enCFB1(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int b;
    // local: unsigned char msgbit;
    // local: unsigned char keybit;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 631
._30 CLoki97::sub64(._30 a, ._30 b) {
    // local: ._30 diff;
    // local: ._30 diff;
}

// line 644
._30 CLoki97::byteToULONG64(unsigned char *inp) {
    // local: ._30 I;
    // local: ._30 I;
}

// line 165
int CLoki97::makeKey(CFrog::._31 *key, unsigned char direction, int keyLen, unsigned char *keyMaterial) {
    // local: ._30 k4;
    // local: ._30 k3;
    // local: ._30 k2;
    // local: ._30 k1;
    // local: ._30 deltan;
    // local: ._30 t1;
    // local: ._30 t2;
    // local: ._30 f_out;
    // local: int i;
}

// line 660
unsigned char * CLoki97::ULONG64ToBYTE(unsigned char *buf, ._30 I) {
    // local: unsigned char *sav;
}

// line 455
int CLoki97::deCBC(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int blocks;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 newIVL;
    // local: ._30 newIVR;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 417
int CLoki97::deECB(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int blocks;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 394
int CLoki97::blockDecrypt(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
}

// line 290
int CLoki97::enCBC(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int blocks;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 251
int CLoki97::enECB(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int blocks;
    // local: ._30 L;
    // local: ._30 R;
    // local: ._30 nR;
    // local: ._30 f_out;
}

// line 227
int CLoki97::blockEncrypt(CFrog::._32 *cipher, CFrog::._31 *key, unsigned char *input, int inputLen, unsigned char *outBuffer) {
}

// line 722
void CLoki97::UnInit() {
}

// line 703
void CLoki97::Init(void *pSys, void *pLocal) {
}

// line 47
int CLoki97::cipherInit(CFrog::._32 *cipher, unsigned char mode, unsigned char *IV) {
    // local: int S1_MASK;
    // local: int S2_MASK;
    // local: int i;
    // local: int j;
    // local: int k;
    // local: int b;
    // local: int pval;
    // local: unsigned char *input;
}

// line 674
void CLoki97::Loki97(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned char hexIV[];
    // local: CFrog::._31 keyIns;
    // local: CFrog::._32 loki97_cipher;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

