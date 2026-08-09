// Auto-generated stub from DWARF info
// Original source: src/TenHash.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/sha1.h"
#include "inc/md2.h"
#include "inc/md4.h"
#include "inc/haval160.h"
#include "inc/haval128.h"
#include "inc/ripemd160.h"
#include "inc/ripemd128.h"
#include "inc/gosthash.h"
#include "inc/crc32.h"
#include "inc/adler32.h"
#include "inc/md5.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// mangled: _Z7TenSha1PhjS_
// line 178
unsigned int TenSha1(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    // local: SHA1_CTX ctx;
    // local: CSha1 objSha1;
}

// mangled: _Z6TenMd2PhjS_
// line 159
unsigned int TenMd2(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    // local: md2_t ctx;
    // local: CMd2 objMd2;
}

// mangled: _Z6TenMd4PhjS_
// line 140
unsigned int TenMd4(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    // local: ._33 ctx;
    // local: CMD4 objMD4;
    // local: unsigned char digest[];
}

// mangled: _Z11TenHaval160PhjS_
// line 123
unsigned int TenHaval160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    // local: CHaval160 objHaval160;
}

// mangled: _Z11TenHaval128PhjS_
// line 108
unsigned int TenHaval128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    // local: CHaval128 objHaval128;
}

// mangled: _Z12TenRipeMd160PhjS_
// line 93
unsigned int TenRipeMd160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    // local: CRipeMD160 objMd160;
}

// mangled: _Z12TenRipeMd128PhjS_
// line 78
unsigned int TenRipeMd128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    // local: CRipeMD128 objMd128;
}

// mangled: _Z11TenGostHashPhjS_
// line 63
unsigned int TenGostHash(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    // local: CGostHash objGost;
}

// mangled: _Z8TenCrc32Phj
// line 53
unsigned int TenCrc32(unsigned char *pbyBuf, unsigned int dwSize) {
    // local: CCrc32 objCrc32;
}

// mangled: _Z10TenAdler32jPhj
// line 43
unsigned int TenAdler32(unsigned int dwAdler, unsigned char *pbyBuf, unsigned int dwSize) {
    // local: CAdler32 objAdler32;
}

// mangled: _Z6TenMd5PhjS_
// line 25
unsigned int TenMd5(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    // local: md5_state_t state;
    // local: md5_byte_t digest[];
}

