// Auto-generated stub from DWARF info
// Original source: src/rijndael.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/rijndael.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 326
unsigned char CRijndael::xtime(unsigned char a) {
    // local: unsigned char b;
}

// line 336
unsigned char CRijndael::mul(unsigned char a, unsigned char b) {
}

// line 344
void CRijndael::inv_mix_column(unsigned int *a, unsigned int *b) {
    // local: unsigned char c[][];
    // local: int i;
    // local: int j;
}

// line 364
void CRijndael::rijndael_setup(CRijndael::tagRIJNDAELContext *ctx, unsigned int keysize, const unsigned char *key) {
    // local: int nk;
    // local: int nr;
    // local: int i;
    // local: int lastkey;
    // local: unsigned int temp;
    // local: unsigned int rcon;
}

// line 409
void CRijndael::key_addition_8to32(const unsigned char *txt, unsigned int *keys, unsigned int *out) {
    // local: const unsigned char *ptr;
    // local: int i;
    // local: int j;
    // local: unsigned int val;
}

// line 424
void CRijndael::key_addition32(const unsigned int *txt, unsigned int *keys, unsigned int *out) {
    // local: int i;
}

// line 432
void CRijndael::key_addition32to8(const unsigned int *txt, unsigned int *keys, unsigned char *out) {
    // local: unsigned char *ptr;
    // local: int i;
    // local: int j;
    // local: unsigned int val;
}

// line 448
void CRijndael::rijndael_encrypt(CRijndael::tagRIJNDAELContext *ctx, const unsigned char *plaintext, unsigned char *ciphertext) {
    // local: int r;
    // local: int j;
    // local: unsigned int wtxt[];
    // local: unsigned int t[];
    // local: unsigned int e;
}

// line 480
void CRijndael::rijndael_decrypt(CRijndael::tagRIJNDAELContext *ctx, const unsigned char *ciphertext, unsigned char *plaintext) {
    // local: int r;
    // local: int j;
    // local: unsigned int wtxt[];
    // local: unsigned int t[];
    // local: unsigned int e;
}

// line 661
void CRijndael::RijnDael(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

// line 585
void CRijndael::block_decrypt(CRijndael::tagRIJNDAELContext *ctx, unsigned char *input, int inputlen, unsigned char *output, unsigned char *iv) {
    // local: int i;
    // local: int j;
    // local: int nblocks;
    // local: int carry_flg;
    // local: unsigned char block[];
    // local: unsigned char block2[];
}

// line 511
void CRijndael::block_encrypt(CRijndael::tagRIJNDAELContext *ctx, unsigned char *input, int inputlen, unsigned char *output, unsigned char *iv) {
    // local: int i;
    // local: int j;
    // local: int nblocks;
    // local: int carry_flg;
    // local: unsigned char block[];
    // local: unsigned char block2[];
}

// line 304
void CRijndael::UnInit() {
}

// line 18
void CRijndael::Initialize(int nrounds, int mode) {
}

// line 26
void CRijndael::Init(void *pSys, void *pLocal) {
    // local: unsigned int _dtbl[];
    // local: unsigned int _itbl[];
    // local: unsigned char _sbox[];
    // local: unsigned char _isbox[];
    // local: unsigned char _Logtable[];
    // local: unsigned char _Alogtable[];
    // local: unsigned int _idx[][];
    // local: unsigned int _iidx[][];
}

