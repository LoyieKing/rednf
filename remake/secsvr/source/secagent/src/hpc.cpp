// Auto-generated stub from DWARF info
// Original source: src/hpc.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/hpc.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 50
void CHpc::mult_64(unsigned int *r, const unsigned int *x, const unsigned int *y) {
    // local: unsigned int x0;
    // local: unsigned int x1;
    // local: unsigned int x2;
    // local: unsigned int x3;
    // local: unsigned int y0;
    // local: unsigned int y1;
    // local: unsigned int y2;
    // local: unsigned int y3;
    // local: unsigned int t0;
    // local: unsigned int t1;
    // local: unsigned int t2;
    // local: unsigned int t3;
    // local: unsigned int c;
}

// line 70
void CHpc::setKey(const unsigned int *in_key, unsigned int key_len) {
    // local: unsigned int s[][];
    // local: unsigned int t[];
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned int xs;
}

// line 142
void CHpc::encrypt(unsigned int *in_blk) {
    // local: unsigned int s0[];
    // local: unsigned int s1[];
    // local: unsigned int k[];
    // local: unsigned int kk[];
    // local: unsigned int t[];
    // local: unsigned int tt;
    // local: unsigned int xs;
    // local: int i;
}

// line 192
void CHpc::decrypt(unsigned int *in_blk) {
    // local: unsigned int s0[];
    // local: unsigned int s1[];
    // local: unsigned int k[];
    // local: unsigned int kk[];
    // local: unsigned int t[];
    // local: unsigned int tt;
    // local: unsigned int xs;
    // local: int i;
}

// line 298
void CHpc::UnInit() {
}

// line 262
void CHpc::Init(void *pSys, void *pLocal) {
    // local: unsigned int _spice[][];
    // local: unsigned int _p119[];
    // local: unsigned int _e19[];
    // local: unsigned int _r220[];
}

// line 242
void CHpc::Hpc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

