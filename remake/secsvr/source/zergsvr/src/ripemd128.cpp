// Auto-generated stub from DWARF info
// Original source: src/ripemd128.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/ripemd128.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 98
void CRipeMD128::Reset() {
}

// line 284
unsigned int CRipeMD128::ROL(unsigned int x, unsigned int n) {
}

// line 290
unsigned int CRipeMD128::F(unsigned int x, unsigned int y, unsigned int z) {
}

// line 295
unsigned int CRipeMD128::G(unsigned int x, unsigned int y, unsigned int z) {
}

// line 300
unsigned int CRipeMD128::H(unsigned int x, unsigned int y, unsigned int z) {
}

// line 305
unsigned int CRipeMD128::I(unsigned int x, unsigned int y, unsigned int z) {
}

// line 312
void CRipeMD128::FF128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 319
void CRipeMD128::GG128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 326
void CRipeMD128::HH128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 333
void CRipeMD128::II128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 340
void CRipeMD128::FFF128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 347
void CRipeMD128::GGG128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 354
void CRipeMD128::HHH128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 361
void CRipeMD128::III128(unsigned int &ra, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s) {
}

// line 114
void CRipeMD128::Transform() {
    // local: unsigned int *puiIn;
    // local: unsigned int aa;
    // local: unsigned int bb;
    // local: unsigned int cc;
    // local: unsigned int dd;
    // local: unsigned int aaa;
    // local: unsigned int bbb;
    // local: unsigned int ccc;
    // local: unsigned int ddd;
}

// line 64
void CRipeMD128::FinalDigest(unsigned char *pcDigest) {
    // local: unsigned int uiCount;
    // local: unsigned char *puc;
}

// line 20
void CRipeMD128::AddData(unsigned char *pcData, unsigned int iDataLength) {
    // local: unsigned int uiT;
    // local: unsigned char *puc;
}

