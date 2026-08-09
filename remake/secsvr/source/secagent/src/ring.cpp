// Auto-generated stub from DWARF info
// Original source: src/ring.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/ring.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 8
void CRing::SetRingKey(unsigned char *key, unsigned char *ringkey, unsigned char *gene) {
    // local: int i;
    // local: int l;
    // local: unsigned int pos;
    // local: unsigned char kps;
    // local: unsigned int holdrand;
}

// line 38
void CRing::RingEncrypt(unsigned char *ringkey, unsigned char *buf) {
    // local: unsigned char Alpha[];
    // local: unsigned char bk;
    // local: int i;
}

// line 54
void CRing::RingDecrypt(unsigned char *ringkey, unsigned char *buf) {
    // local: unsigned char Alpha[];
    // local: unsigned char bk;
    // local: int i;
}

// line 140
void CRing::UnInit() {
}

// line 111
void CRing::Init(void *pSys, void *pLocal) {
    // local: unsigned char _Ringbbit[];
    // local: unsigned char _RingIP[];
}

// line 70
void CRing::Ring(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned char ringkey[];
    // local: unsigned char gene[];
    // local: unsigned int i;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

