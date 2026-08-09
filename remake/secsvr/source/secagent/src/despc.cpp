// Auto-generated stub from DWARF info
// Original source: src/despc.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/despc.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 175
void CDesPC::kinit(unsigned char *key) {
    // local: int i;
    // local: int j;
    // local: int l;
    // local: int m;
}

// line 208
unsigned char CDesPC::getcomp(unsigned int k, unsigned int v) {
}

// line 284
void CDesPC::pc_permute(unsigned char *inblock, unsigned char *perm[][], unsigned char *outblock) {
    // local: int i;
    // local: int j;
    // local: unsigned char *ib;
    // local: unsigned char *ob;
    // local: unsigned char *p;
    // local: unsigned char *q;
}

// line 302
void CDesPC::expand(unsigned char *right, unsigned char *bigright) {
    // local: unsigned char *bb;
    // local: unsigned char *r;
    // local: unsigned char r0;
    // local: unsigned char r1;
    // local: unsigned char r2;
    // local: unsigned char r3;
}

// line 330
void CDesPC::contract(unsigned char *in48, unsigned char *out32) {
    // local: unsigned char *c;
    // local: unsigned char *i;
    // local: unsigned int i0;
    // local: unsigned int i1;
    // local: unsigned int i2;
    // local: unsigned int i3;
    // local: unsigned int i4;
    // local: unsigned int i5;
}

// line 345
void CDesPC::perm32(unsigned char *inblock, unsigned char *outblock) {
    // local: int j;
    // local: unsigned char *ib;
    // local: unsigned char *ob;
    // local: unsigned char *q;
}

// line 365
void CDesPC::pc_f(unsigned char *right, int num, unsigned char *fret) {
    // local: unsigned char *kb;
    // local: unsigned char *rb;
    // local: unsigned char *bb;
    // local: unsigned char bigright[];
    // local: unsigned char result[];
    // local: unsigned char preout[];
}

// line 385
void CDesPC::iter(int num, unsigned char *inblock, unsigned char *outblock) {
    // local: unsigned char fret[];
    // local: unsigned char *ib;
    // local: unsigned char *ob;
    // local: unsigned char *fb;
}

// line 403
void CDesPC::endes(unsigned char *inblock, unsigned char *outblock) {
    // local: unsigned char iters[][];
    // local: unsigned char swap[];
    // local: int i;
    // local: unsigned char *s;
    // local: unsigned char *t;
}

// line 425
void CDesPC::dedes(unsigned char *inblock, unsigned char *outblock) {
    // local: unsigned char iters[][];
    // local: unsigned char swap[];
    // local: int i;
    // local: unsigned char *s;
    // local: unsigned char *t;
}

// line 467
void CDesPC::Init(void *pSys, void *pLocal) {
}

// line 472
void CDesPC::UnInit() {
}

// line 136
void CDesPC::pc_perminit(unsigned char *perm[][], const unsigned char *p) {
    // local: int l;
    // local: int j;
    // local: int k;
    // local: int i;
    // local: int m;
}

// line 443
void CDesPC::DesPC(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

