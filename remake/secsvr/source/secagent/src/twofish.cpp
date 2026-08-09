// Auto-generated stub from DWARF info
// Original source: src/twofish.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/twofish.h"
#include "inc/publish/TeaMacro.h"
#include "include/TenCrypt.h"
#include "yunfeiyang/dev/branch/src/common/tencrypt_new/<built-in>"
#include <bits/types.h>
#include <libio.h>
#include <stddef.h>
#include <stdio.h>

// line 340
int CTwofish::setRounds(int keyLen, int nRounds) {
}

// mangled: _Z13ParseHexDwordiPKhPjPh
// line 378
int ParseHexDword(int bits, const unsigned char *srcTxt, DWORD *d, unsigned char *dstTxt) {
    // local: int i;
    // local: unsigned char c;
}

// mangled: _Z13RS_MDS_Encodejj
// line 423
DWORD RS_MDS_Encode(DWORD k0, DWORD k1) {
    // local: int i;
    // local: int j;
    // local: DWORD r;
    // local: DWORD b;
    // local: DWORD g2;
    // local: DWORD g3;
}

// line 454
void CTwofish::BuildMDS() {
    // local: int i;
    // local: DWORD d;
    // local: BYTE m1[];
    // local: BYTE mX[];
    // local: BYTE mY[];
}

// line 516
void CTwofish::ReverseRoundSubkeys(._30 *key, BYTE newDir) {
    // local: DWORD t0;
    // local: DWORD t1;
    // local: DWORD *r0;
    // local: DWORD *r1;
}

// mangled: _Z6Xor256PvS_h
// line 557
void Xor256(void *dst, void *src, BYTE b) {
    // local: DWORD x;
    // local: DWORD *d;
    // local: DWORD *s;
}

// line 589
int CTwofish::reKey(._30 *key) {
    // local: int i;
    // local: int j;
    // local: int k64Cnt;
    // local: int keyLen;
    // local: int subkeyCnt;
    // local: DWORD A;
    // local: DWORD B;
    // local: DWORD q;
    // local: DWORD sKey[];
    // local: DWORD k32e[];
    // local: DWORD k32o[];
    // local: BYTE L0[];
    // local: BYTE L1[];
    // local: DWORD t;
    // local: DWORD t;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k1;
    // local: DWORD k0;
    // local: DWORD k0;
    // local: DWORD k0;
    // local: DWORD k0;
}

// line 771
int CTwofish::cipherInit(CFrog::._31 *cipher, BYTE mode, unsigned char *IV) {
    // local: int i;
}

// line 821
int CTwofish::blockEncrypt(CFrog::._31 *cipher, ._30 *key, BYTE *input, int inputLen, BYTE *outBuffer) {
    // local: int i;
    // local: int n;
    // local: DWORD x[];
    // local: DWORD t0;
    // local: DWORD t1;
    // local: int rounds;
    // local: BYTE bit;
    // local: BYTE ctBit;
    // local: BYTE carry;
    // local: int mode;
    // local: DWORD sk[];
    // local: DWORD IV[];
}

// line 965
int CTwofish::blockDecrypt(CFrog::._31 *cipher, ._30 *key, BYTE *input, int inputLen, BYTE *outBuffer) {
    // local: int i;
    // local: int n;
    // local: DWORD x[];
    // local: DWORD t0;
    // local: DWORD t1;
    // local: int rounds;
    // local: BYTE bit;
    // local: BYTE ctBit;
    // local: BYTE carry;
    // local: int mode;
    // local: DWORD sk[];
    // local: DWORD IV[];
}

// line 727
int CTwofish::makeKey(._30 *key, BYTE direction, int keyLen, unsigned char *keyMaterial) {
}

// line 1176
void CTwofish::Twofish32(unsigned char *key, bool benc, unsigned char *buf, long unsigned int buflen) {
    // local: unsigned char hexIV[];
    // local: ._30 keyIns;
    // local: CFrog::._31 twofish_cipher;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 1140
void CTwofish::Twofish24(unsigned char *key, bool benc, unsigned char *buf, long unsigned int buflen) {
    // local: unsigned char hexIV[];
    // local: ._30 keyIns;
    // local: CFrog::._31 twofish_cipher;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 1096
void CTwofish::Twofish16(unsigned char *key, bool benc, unsigned char *buf, long unsigned int buflen) {
    // local: unsigned char hexIV[];
    // local: ._30 keyIns;
    // local: CFrog::._31 twofish_cipher;
    // local: unsigned int nLen;
    // local: ITsLocal *pLocal;
}

// line 315
void CTwofish::UnInit() {
}

// line 218
void CTwofish::Init(void *pSys, void *pLocal) {
    // local: unsigned char _numRounds[];
    // local: long unsigned int i;
    // local: unsigned char _P8x8[];
    // local: unsigned char _P8x8[];
}

