// Auto-generated stub from DWARF info
// Original source: src/loki91.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/loki91.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 38
short unsigned int CLoki91::mult8(short unsigned int a, short unsigned int b, short unsigned int gen) {
    // local: short unsigned int product;
}

// line 60
short unsigned int CLoki91::exp_8(short unsigned int base, short unsigned int exponent, short unsigned int gen) {
    // local: short unsigned int accum;
    // local: short unsigned int result;
}

// line 90
void CLoki91::perm32(unsigned int *out, unsigned int *in, unsigned char *perm) {
    // local: unsigned int mask;
    // local: int i;
    // local: int o;
    // local: int b;
    // local: unsigned char *p;
}

// line 109
short unsigned int CLoki91::s(unsigned int i) {
    // local: short unsigned int r;
    // local: short unsigned int c;
    // local: short unsigned int v;
    // local: short unsigned int t;
}

// line 144
unsigned int CLoki91::f(unsigned int r, unsigned int k) {
    // local: unsigned int a;
    // local: unsigned int b;
    // local: unsigned int c;
}

// line 167
void CLoki91::setkey(unsigned char *key) {
    // local: unsigned int i;
    // local: unsigned int KL;
    // local: unsigned int KR;
    // local: unsigned char c;
    // local: unsigned char c;
}

// line 219
void CLoki91::blockEncrypt(unsigned char *b) {
    // local: unsigned int i;
    // local: unsigned int L;
    // local: unsigned int R;
    // local: unsigned char c;
    // local: unsigned char c;
}

// line 246
void CLoki91::blockDecrypt(unsigned char *b) {
    // local: unsigned int i;
    // local: unsigned int L;
    // local: unsigned int R;
    // local: unsigned char c;
    // local: unsigned char c;
}

// line 348
void CLoki91::UnInit() {
}

// line 299
void CLoki91::Init(void *pSys, void *pLocal) {
    // local: unsigned char _P[];
    // local: ._30 _sfn[];
}

// line 271
void CLoki91::Loki91(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

