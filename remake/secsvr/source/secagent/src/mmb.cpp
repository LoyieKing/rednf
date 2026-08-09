// Auto-generated stub from DWARF info
// Original source: src/mmb.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/mmb.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 37
void CMmb::SetKey(unsigned int *in_key) {
    // local: unsigned int blk[];
    // local: unsigned int fbk;
    // local: unsigned int x;
}

// line 63
unsigned int CMmb::ModMult(unsigned int x, unsigned int y) {
    // local: unsigned int z0;
    // local: unsigned int z1;
    // local: unsigned int q;
}

// line 81
void CMmb::f(unsigned int *x) {
    // local: unsigned int t0;
    // local: unsigned int t1;
}

// line 105
void CMmb::g(unsigned int *x) {
    // local: unsigned int t0;
    // local: unsigned int t1;
}

// line 129
void CMmb::Encrypt(unsigned int *x) {
}

// line 146
void CMmb::Decrypt(unsigned int *x) {
}

// line 225
void CMmb::UnInit() {
}

// line 190
void CMmb::Init(void *pSys, void *pLocal) {
    // local: unsigned int _iv_tbl[];
}

// line 163
void CMmb::Mmb(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

