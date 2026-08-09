// Auto-generated stub from DWARF info
// Original source: src/nsea.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/nsea.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 15
void CNsea::setInitialSBox(unsigned char *sBox) {
    // local: int i;
    // local: int j;
    // local: int index;
}

// line 24
void CNsea::permuteSBox(unsigned char *sBox, int tempKeyIndex) {
    // local: int columnTable[];
    // local: int srcIndex;
    // local: int destIndex;
    // local: unsigned char temp;
}

// line 40
void CNsea::encrypt(unsigned char *inData, unsigned char *outData) {
    // local: unsigned int lLeft;
    // local: unsigned int rLeft;
    // local: unsigned int lRight;
    // local: unsigned int rRight;
}

// line 113
void CNsea::initIV(unsigned int salt) {
    // local: unsigned char iv[];
    // local: int i;
}

// line 133
void CNsea::initTempKey(unsigned char *key, int keyLength, short unsigned int salt, unsigned char *tempKeyPtr) {
    // local: int i;
    // local: unsigned int holdrand;
}

// line 151
void CNsea::initSBoxes(unsigned char *key, int keyLength, unsigned int salt) {
    // local: unsigned char *sBoxByte1;
    // local: unsigned char *sBoxByte2;
    // local: int i;
    // local: unsigned int j;
}

// line 186
void CNsea::decrypt(unsigned char *inData, unsigned char *outData) {
    // local: unsigned int lLeft;
    // local: unsigned int rLeft;
    // local: unsigned int lRight;
    // local: unsigned int rRight;
}

// line 297
void CNsea::UnInit() {
}

// line 284
void CNsea::Init(void *pSys, void *pLocal) {
}

// line 259
void CNsea::Nsea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

