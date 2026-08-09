// Auto-generated stub from DWARF info
// Original source: src/frog.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/frog.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 71
void CFrog::MakePerm(unsigned char *ip, unsigned int top) {
    // local: unsigned char ua[];
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned int ie;
    // local: unsigned int ne;
}

// line 96
void CFrog::MakeKey(CFrog::._31 *kp) {
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned int k;
    // local: unsigned int ix;
    // local: unsigned int ll;
    // local: unsigned char ua[];
}

// line 196
void CFrog::Decrypt(unsigned int *in_blk, unsigned int *out_blk) {
    // local: unsigned char i;
    // local: unsigned char blk[];
    // local: unsigned char ct;
    // local: unsigned char *xp;
    // local: unsigned char *sp;
    // local: unsigned char *pp;
}

// line 172
void CFrog::Encrypt(unsigned int *in_blk, unsigned int *out_blk) {
    // local: unsigned char i;
    // local: unsigned char k;
    // local: unsigned char blk[];
    // local: unsigned char *xp;
    // local: unsigned char *sp;
    // local: unsigned char *pp;
}

// line 149
void CFrog::SetKey(unsigned int *in_key, unsigned int key_len) {
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned int k;
    // local: unsigned char kb[];
}

// line 220
void CFrog::Frog(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

// line 66
void CFrog::UnInit() {
}

// line 27
void CFrog::Init(void *pSys, void *pLocal) {
    // local: unsigned char _seed[];
}

