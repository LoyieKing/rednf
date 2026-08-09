// Auto-generated stub from DWARF info
// Original source: src/ripemd160.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/ripemd160.h"

// line 117
void CRipeMD160::Reset() {
}

// line 345
unsigned int CRipeMD160::ROL(unsigned int x, unsigned int n) {
}

// line 350
unsigned int CRipeMD160::F(unsigned int x, unsigned int y, unsigned int z) {
}

// line 355
unsigned int CRipeMD160::G(unsigned int x, unsigned int y, unsigned int z) {
}

// line 360
unsigned int CRipeMD160::H(unsigned int x, unsigned int y, unsigned int z) {
}

// line 365
unsigned int CRipeMD160::I(unsigned int x, unsigned int y, unsigned int z) {
}

// line 370
unsigned int CRipeMD160::J(unsigned int x, unsigned int y, unsigned int z) {
}

// line 377
void CRipeMD160::FF160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 385
void CRipeMD160::GG160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 393
void CRipeMD160::HH160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 401
void CRipeMD160::II160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 409
void CRipeMD160::JJ160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 417
void CRipeMD160::FFF160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 425
void CRipeMD160::GGG160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 433
void CRipeMD160::HHH160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 441
void CRipeMD160::III160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 449
void CRipeMD160::JJJ160(unsigned int &ra, unsigned int b, unsigned int &rc, unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
}

// line 135
void CRipeMD160::Transform() {
    // local: unsigned int *puiIn;
    // local: unsigned int aa;
    // local: unsigned int bb;
    // local: unsigned int cc;
    // local: unsigned int dd;
    // local: unsigned int ee;
    // local: unsigned int aaa;
    // local: unsigned int bbb;
    // local: unsigned int ccc;
    // local: unsigned int ddd;
    // local: unsigned int eee;
}

// line 83
void CRipeMD160::FinalDigest(unsigned char *pcDigest) {
    // local: unsigned int uiCount;
    // local: unsigned char *puc;
}

// line 35
void CRipeMD160::AddData(unsigned char *pcData, unsigned int iDataLength) {
    // local: unsigned int uiT;
    // local: unsigned char *puc;
}

