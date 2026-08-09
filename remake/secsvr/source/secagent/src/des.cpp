// Auto-generated stub from DWARF info
// Original source: src/des.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/des.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 4
void CDES::DesKey(unsigned char *key, bool benc, unsigned int *KnL) {
    // local: int i;
    // local: int j;
    // local: int l;
    // local: int m;
    // local: int n;
    // local: unsigned char pc1m[];
    // local: unsigned char pcr[];
    // local: unsigned int kn[];
    // local: unsigned char totrot[];
}

// line 151
void CDES::DesBase(unsigned int *KnL, unsigned char *inblock, unsigned char *outblock) {
    // local: unsigned int work[];
    // local: unsigned int fval;
    // local: unsigned int wtmp;
    // local: unsigned int right;
    // local: unsigned int leftt;
    // local: int round;
}

// line 453
void CDES::UnInit() {
}

// line 269
void CDES::Init(void *pSys, void *pLocal) {
    // local: unsigned int _SP1[];
    // local: unsigned int _SP2[];
    // local: unsigned int _SP3[];
    // local: unsigned int _SP4[];
    // local: unsigned int _SP5[];
    // local: unsigned int _SP6[];
    // local: unsigned int _SP7[];
    // local: unsigned int _SP8[];
}

// line 245
void CDES::Des(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int KnL[];
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

