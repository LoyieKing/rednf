// Auto-generated stub from DWARF info
// Original source: src/sha1.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/sha1.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 120
void CSha1::SHA1_Init(SHA1_CTX *context) {
}

// line 62
void CSha1::SHA1Transform(unsigned int *state, unsigned char *buffer) {
    // local: unsigned int a;
    // local: unsigned int b;
    // local: unsigned int c;
    // local: unsigned int d;
    // local: unsigned int e;
    // local: CHAR64LONG16 *block;
    // local: unsigned char workspace[];
}

// line 134
void CSha1::SHA1_Update(SHA1_CTX *context, unsigned char *data, unsigned int len) {
    // local: unsigned int i;
    // local: unsigned int j;
}

// line 171
void CSha1::SHA1_Final(unsigned char *digest, SHA1_CTX *context) {
    // local: unsigned int i;
    // local: unsigned int j;
    // local: unsigned char finalcount[];
}

