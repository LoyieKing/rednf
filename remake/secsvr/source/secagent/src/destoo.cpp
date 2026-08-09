// Auto-generated stub from DWARF info
// Original source: src/destoo.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/destoo.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 226
unsigned int CDesToo::fsf(unsigned int r, unsigned char *subkey) {
    // local: unsigned int rval;
    // local: unsigned int rt;
}

// line 261
void CDesToo::too_permute(unsigned char *inblock, unsigned char *perm[][], unsigned char *outblock) {
    // local: int i;
    // local: int j;
    // local: unsigned char *ib;
    // local: unsigned char *ob;
    // local: unsigned char *p;
    // local: unsigned char *q;
}

// line 288
void CDesToo::round(int num, unsigned int *block) {
}

// line 304
void CDesToo::doencrypt(unsigned char *block) {
    // local: int i;
    // local: unsigned int work[];
    // local: unsigned int tmp;
}

// line 333
void CDesToo::dodecrypt(unsigned char *block) {
    // local: int i;
    // local: unsigned int work[];
    // local: unsigned int tmp;
}

// line 408
void CDesToo::spinit() {
    // local: unsigned char pbox[];
    // local: int p;
    // local: int i;
    // local: int s;
    // local: int j;
    // local: unsigned int val;
    // local: unsigned int rowcol;
}

// line 521
void CDesToo::Init(void *pSys, void *pLocal) {
}

// line 527
void CDesToo::UnInit() {
}

// line 364
void CDesToo::too_perminit(unsigned char *perm[][], const unsigned char *p) {
    // local: int l;
    // local: int j;
    // local: int k;
    // local: int i;
    // local: int m;
}

// line 451
void CDesToo::too_setkey(unsigned char *key) {
    // local: unsigned char pc1m[];
    // local: unsigned char pcr[];
    // local: int i;
    // local: int j;
    // local: int l;
    // local: int m;
}

// line 498
void CDesToo::DesToo(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

