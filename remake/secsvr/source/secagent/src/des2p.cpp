// Auto-generated stub from DWARF info
// Original source: src/des2p.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/des2p.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 111
void CDes2p::pkbit(unsigned char *to, unsigned char *from, int len) {
    // local: int j;
    // local: int t;
}

// line 132
void CDes2p::perm(unsigned char *to, const unsigned char *op, unsigned char *from, int bits) {
    // local: int i;
}

// line 140
void CDes2p::rot(unsigned char *r, int len, int bits) {
    // local: int i;
    // local: int j;
    // local: int t;
}

// line 163
void CDes2p::add2(unsigned char *to, unsigned char *from, int bits) {
    // local: int i;
}

// line 185
void CDes2p::subs(unsigned char *o, unsigned char *i, int s) {
    // local: int t;
}

// line 201
void CDes2p::unbit(unsigned char *to, unsigned char *from, int len) {
    // local: int j;
    // local: int f;
}

// line 215
int CDes2p::des_set_key(unsigned char *key, des_key_schedule *ks) {
    // local: int i;
    // local: unsigned char k[];
    // local: unsigned char cd[];
    // local: int __i;
    // local: int __i;
}

// line 293
void CDes2p::des_set_odd_parity(unsigned char *key) {
    // local: int i;
    // local: unsigned char c;
}

// line 334
void CDes2p::Init(void *pSys, void *pLocal) {
}

// line 342
void CDes2p::UnInit() {
}

// line 237
void CDes2p::des_func(unsigned char *in, unsigned char *out, unsigned char *b[], bool enc) {
    // local: unsigned char lr[];
    // local: unsigned char rsto[];
    // local: unsigned char si[];
    // local: unsigned char so[];
    // local: int __i;
    // local: int i;
    // local: int __i;
    // local: int __i;
    // local: int j;
    // local: int ___t;
    // local: int __i;
    // local: int __i;
    // local: int __i;
}

// line 283
int CDes2p::des_ecb_encrypt(unsigned char *in, unsigned char *out, des_key_schedule *ks, bool enc) {
    // local: unsigned char o[];
}

// line 308
void CDes2p::Des2p(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: des_key_schedule ks;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

