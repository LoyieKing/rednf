// Auto-generated stub from DWARF info
// Original source: src/feal8.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/feal8.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 24
unsigned int CFeal8::MakeH1(unsigned char *B) {
    // local: ._30 RetVal;
}

// line 59
unsigned char CFeal8::Rot2(unsigned char X) {
    // local: int First;
    // local: unsigned char RetVal[];
    // local: int i;
    // local: int High;
    // local: int Low;
}

// line 82
unsigned int CFeal8::f(unsigned int AA, unsigned int BB) {
    // local: unsigned char f1;
    // local: unsigned char f2;
    // local: CFrog::._31 RetVal;
    // local: CFrog::._31 A;
    // local: CFrog::._32 B;
}

// line 107
unsigned int CFeal8::FK(unsigned int AA, unsigned int BB) {
    // local: unsigned char FK1;
    // local: unsigned char FK2;
    // local: ._33 RetVal;
    // local: ._33 A;
    // local: ._33 B;
}

// line 128
void CFeal8::DissQ1(unsigned int Q, unsigned char *B) {
    // local: CLoki97::._34 QQ;
}

// line 140
unsigned int CFeal8::MakeH2(unsigned int *Q) {
    // local: unsigned char B[];
}

// line 148
void CFeal8::DissH1(unsigned int H, unsigned char *D) {
    // local: CLoki97::._35 T;
}

// line 162
void CFeal8::SetKey(unsigned char *KP) {
    // local: CLoki89::._36 A;
    // local: CLoki89::._36 B;
    // local: CLoki89::._36 D;
    // local: CLoki89::._36 NewB;
    // local: CLoki91::._37 Q;
    // local: int i;
    // local: unsigned int *Out;
}

// line 204
void CFeal8::Encrypt(unsigned char *Plain, unsigned char *Cipher) {
    // local: unsigned int L;
    // local: unsigned int R;
    // local: unsigned int NewR;
    // local: int r;
}

// line 236
void CFeal8::Decrypt(unsigned char *Cipher, unsigned char *Plain) {
    // local: unsigned int L;
    // local: unsigned int R;
    // local: unsigned int NewL;
    // local: int r;
}

// line 286
void CFeal8::Init(void *pSys, void *pLocal) {
}

// line 294
void CFeal8::UnInit() {
}

// line 262
void CFeal8::Feal8(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

