// Auto-generated stub from DWARF info
// Original source: src/idea.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/idea.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 3
short unsigned int CIdea::inv(short unsigned int x) {
    // local: short unsigned int t0;
    // local: short unsigned int t1;
    // local: short unsigned int q;
    // local: short unsigned int y;
}

// line 32
void CIdea::en_key_idea(short unsigned int *userkey, short unsigned int *Z) {
    // local: int i;
    // local: int j;
}

// line 48
void CIdea::de_key_idea(short unsigned int *Z, short unsigned int *DK) {
    // local: int j;
    // local: short unsigned int t1;
    // local: short unsigned int t2;
    // local: short unsigned int t3;
    // local: short unsigned int T[];
    // local: short unsigned int *p;
}

// line 116
short unsigned int CIdea::mul(short unsigned int a, short unsigned int b) {
    // local: unsigned int p;
}

// line 140
void CIdea::cipher_idea(short unsigned int *in, short unsigned int *out, short unsigned int *Z) {
    // local: short unsigned int x1;
    // local: short unsigned int x2;
    // local: short unsigned int x3;
    // local: short unsigned int x4;
    // local: short unsigned int t1;
    // local: short unsigned int t2;
    // local: int r;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: unsigned int tmp_p;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: unsigned int tmp_p;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: unsigned int tmp_p;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: unsigned int tmp_p;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: short unsigned int tmp_a;
    // local: short unsigned int tmp_b;
    // local: unsigned int tmp_p;
    // local: unsigned int tmp_p;
}

// line 185
unsigned int CIdea::cipher_file(unsigned char *in, unsigned int len, short unsigned int *key) {
    // local: short unsigned int input[];
    // local: short unsigned int output[];
    // local: short unsigned int Z[];
    // local: unsigned char *pch;
    // local: unsigned int nlen;
    // local: unsigned int x;
}

// line 219
unsigned int CIdea::decipher_file(unsigned char *in, unsigned int len, short unsigned int *key) {
    // local: short unsigned int input[];
    // local: short unsigned int output[];
    // local: short unsigned int Z[];
    // local: short unsigned int DK[];
    // local: unsigned char *pch;
    // local: unsigned int nlen;
    // local: unsigned int x;
}

// line 273
void CIdea::Init(void *pSys, void *pLocal) {
}

// line 278
void CIdea::UnInit() {
}

// line 254
void CIdea::Idea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

