// Auto-generated stub from DWARF info
// Original source: src/diamond.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/diamond.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 11
void CDiamond::BuildCRCTable() {
    // local: unsigned int value;
    // local: int i;
    // local: int j;
}

// line 31
unsigned int CDiamond::keyrand(unsigned int max_value) {
    // local: unsigned int prandvalue;
    // local: unsigned int i;
    // local: unsigned int mask;
}

// line 58
void CDiamond::makeonebox(unsigned int i, unsigned int j) {
    // local: int n;
    // local: unsigned int pos;
    // local: unsigned int m;
    // local: unsigned int p;
    // local: bool filled[];
}

// line 86
int CDiamond::set_diamond_key(unsigned char *external_key, unsigned int key_size, unsigned int rounds, bool invert, unsigned int block_size) {
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned int k;
}

// line 155
void CDiamond::substitute(unsigned int round, unsigned char *x, unsigned char *y) {
    // local: unsigned int i;
}

// line 164
void CDiamond::isubst(unsigned int round, unsigned char *x, unsigned char *y) {
    // local: unsigned int i;
}

// line 174
void CDiamond::permute(unsigned char *x, unsigned char *y) {
}

// line 226
void CDiamond::ipermute(unsigned char *x, unsigned char *y) {
}

// mangled: _Z12permute_litePhS_
// line 278
void permute_lite(unsigned char *a, unsigned char *b) {
}

// mangled: _Z13ipermute_litePhS_
// line 302
void ipermute_lite(unsigned char *b, unsigned char *a) {
}

// line 324
void CDiamond::diamond_encrypt_block(unsigned char *x, unsigned char *y) {
    // local: unsigned int round;
    // local: unsigned char z[];
}

// line 338
void CDiamond::diamond_decrypt_block(unsigned char *x, unsigned char *y) {
    // local: int round;
    // local: unsigned char z[];
}

// line 352
void CDiamond::lite_encrypt_block(unsigned char *x, unsigned char *y) {
    // local: unsigned int round;
    // local: unsigned char z[];
}

// line 366
void CDiamond::lite_decrypt_block(unsigned char *x, unsigned char *y) {
    // local: int round;
    // local: unsigned char z[];
}

// line 440
void CDiamond::UnInit() {
}

// line 425
void CDiamond::Init(void *pSys, void *pLocal) {
}

// line 380
void CDiamond::Diamond(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

