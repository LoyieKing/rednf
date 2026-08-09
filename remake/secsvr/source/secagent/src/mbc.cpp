// Auto-generated stub from DWARF info
// Original source: src/mbc.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/mbc.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 55
void CMbc::SetKey(unsigned int *in_key) {
    // local: unsigned int blk[];
    // local: unsigned int fbk;
    // local: unsigned int t;
    // local: unsigned int key_exp[];
    // local: unsigned int x;
    // local: unsigned int i;
}

// line 106
void CMbc::Encrypt(unsigned int *blk) {
    // local: unsigned int fbk;
}

// line 120
void CMbc::Decrypt(unsigned int *blk) {
    // local: unsigned int fbk;
}

// line 195
void CMbc::UnInit() {
}

// line 161
void CMbc::Init(void *pSys, void *pLocal) {
    // local: unsigned int _iv_tbl[];
}

// line 134
void CMbc::Mbc(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
    // local: unsigned int i;
}

