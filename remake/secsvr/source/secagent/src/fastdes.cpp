// Auto-generated stub from DWARF info
// Original source: src/fastdes.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/fastdes.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 430
void CFastDes::fsetkey(unsigned char *key, CFastDes::keysched *ks) {
    // local: int i;
    // local: unsigned int C;
    // local: unsigned int D;
    // local: int v;
    // local: const unsigned int *ap;
}

// line 487
void CFastDes::fencrypt(unsigned char *block, int decrypt, CFastDes::keysched *ks) {
    // local: int i;
    // local: unsigned int L;
    // local: unsigned int R;
    // local: CFastDes::keystage *ksp;
    // local: const unsigned int *ap;
    // local: int v;
    // local: unsigned int k;
    // local: unsigned int tR;
    // local: unsigned int t;
    // local: unsigned int t;
    // local: unsigned char *bp;
}

// line 607
void CFastDes::Init(void *pSys, void *pLocal) {
}

// line 626
void CFastDes::UnInit() {
}

// line 582
void CFastDes::FastDes(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: CFastDes::keysched KS;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

