// Auto-generated stub from DWARF info
// Original source: src/blowfish.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/blowfish.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 357
void CBlowFish::UnInit() {
}

// line 361
void CBlowFish::Encrypt(CBlowFish::SBlock &block) {
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
}

// line 387
void CBlowFish::Decrypt(CBlowFish::SBlock &block) {
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
}

// line 590
unsigned int CBlowFish::F(unsigned int ui) {
}

// line 595
void CBlowFish::BytesToBlock(unsigned char *p, CBlowFish::SBlock &b) {
    // local: unsigned int y;
}

// line 626
void CBlowFish::BlockToBytes(CBlowFish::SBlock &b, unsigned char *p) {
    // local: unsigned int y;
}

// line 413
void CBlowFish::BlowFish(unsigned char *ucKey, unsigned int keysize) {
    // local: unsigned char aucLocalKey[];
    // local: unsigned int i;
    // local: unsigned int j;
    // local: const unsigned char *p;
    // local: unsigned int x;
    // local: unsigned int iCount;
    // local: CBlowFish::SBlock block;
    // local: int n;
    // local: int iVal;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
    // local: int k;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
}

// line 347
void CBlowFish::Init(void *pSys, void *pLocal) {
}

// line 479
void CBlowFish::Encrypt(unsigned char *in, unsigned char *out, unsigned int n, int iMode) {
    // local: CBlowFish::SBlock work;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
    // local: CBlowFish::SBlock chain;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
    // local: CBlowFish::SBlock chain;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
}

// line 534
void CBlowFish::Decrypt(unsigned char *in, unsigned char *out, unsigned int n, int iMode) {
    // local: CBlowFish::SBlock work;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
    // local: CBlowFish::SBlock crypt;
    // local: CBlowFish::SBlock chain;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
    // local: CBlowFish::SBlock crypt;
    // local: CBlowFish::SBlock chain;
    // local: unsigned int uiLeft;
    // local: unsigned int uiRight;
}

// line 723
void CBlowFish::BlowFish56(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 705
void CBlowFish::BlowFish32(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 687
void CBlowFish::BlowFish24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 669
void CBlowFish::BlowFish16(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 651
void CBlowFish::BlowFish8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

