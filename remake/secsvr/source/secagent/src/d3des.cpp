// Auto-generated stub from DWARF info
// Original source: src/d3des.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/d3des.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 270
void CD3des::cpkey(unsigned int *into) {
    // local: unsigned int *from;
    // local: unsigned int *endp;
}

// line 278
void CD3des::usekey(unsigned int *from) {
    // local: unsigned int *to;
    // local: unsigned int *endp;
}

// line 211
void CD3des::deskey(const unsigned char *key, short int edf) {
    // local: int i;
    // local: int j;
    // local: int l;
    // local: int m;
    // local: int n;
    // local: unsigned char pc1m[];
    // local: unsigned char pcr[];
    // local: unsigned int kn[];
    // local: unsigned int *raw1;
    // local: unsigned int *cook;
    // local: unsigned int *raw0;
    // local: unsigned int dough[];
}

// line 295
void CD3des::scrunch(unsigned char *outof, unsigned int *into) {
}

// line 307
void CD3des::unscrun(unsigned int *outof, unsigned char *into) {
}

// line 319
void CD3des::desfunc(unsigned int *block, unsigned int *keys) {
    // local: unsigned int fval;
    // local: unsigned int work;
    // local: unsigned int right;
    // local: unsigned int leftt;
    // local: int round;
}

// line 286
void CD3des::des(unsigned char *inblock, unsigned char *outblock) {
    // local: unsigned int work[];
}

// line 395
void CD3des::des2key(const unsigned char *hexkey, short int mode) {
    // local: short int revmod;
}

// line 406
void CD3des::Ddes(unsigned char *from, unsigned char *into) {
    // local: unsigned int work[];
}

// line 417
void CD3des::D2des(unsigned char *from, unsigned char *into) {
    // local: unsigned int *right;
    // local: unsigned int *l1;
    // local: unsigned int swap;
    // local: unsigned int leftt[];
    // local: unsigned int bufR[];
}

// line 486
void CD3des::des3key(const unsigned char *hexkey, short int mode) {
    // local: const unsigned char *first;
    // local: const unsigned char *third;
    // local: short int revmod;
}

// line 508
void CD3des::cp3key(unsigned int *into) {
    // local: unsigned int *from;
    // local: unsigned int *endp;
}

// line 520
void CD3des::use3key(unsigned int *from) {
    // local: unsigned int *to;
    // local: unsigned int *endp;
}

// line 464
void CD3des::make2key(char *aptr, unsigned char *kptr) {
    // local: unsigned char *store;
    // local: int first;
    // local: int i;
    // local: unsigned int savek[];
}

// line 442
void CD3des::makekey(char *aptr, unsigned char *kptr) {
    // local: unsigned char *store;
    // local: int first;
    // local: int i;
    // local: unsigned int savek[];
}

// line 532
void CD3des::D3des(unsigned char *from, unsigned char *into) {
    // local: unsigned int swap;
    // local: unsigned int leftt[];
    // local: unsigned int middl[];
    // local: unsigned int right[];
}

// line 565
void CD3des::make3key(char *aptr, unsigned char *kptr) {
    // local: unsigned char *store;
    // local: int first;
    // local: int i;
    // local: unsigned int savek[];
}

// line 676
void CD3des::Init(void *pSys, void *pLocal) {
}

// line 683
void CD3des::UnInit() {
}

// line 620
void CD3des::D3des161(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

// line 604
void CD3des::D3des162(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

// line 588
void CD3des::D3des24(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

