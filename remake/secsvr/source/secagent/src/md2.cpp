// Auto-generated stub from DWARF info
// Original source: src/md2.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/md2.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 30
void CMd2::md2_update_digest_and_checksum(md2_t *ctx) {
    // local: int j;
    // local: int k;
    // local: unsigned char c;
    // local: unsigned char t;
}

// line 219
void CMd2::UnInit() {
}

// line 11
int CMd2::md2_init(md2_t *ctx) {
}

// line 152
void CMd2::Init() {
    // local: unsigned char _padding[][];
    // local: unsigned char _S[];
}

// line 74
int CMd2::md2_update_data(md2_t *ctx, unsigned char *buf, unsigned int buflen) {
    // local: unsigned int bufcount;
}

// line 114
void CMd2::md2_append_padding_and_checksum(md2_t *ctx) {
    // local: unsigned int padlen;
    // local: int padindex;
}

// line 127
int CMd2::md2_finish(md2_t *ctx, unsigned char *digest, unsigned int digestlen) {
}

