// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
// Original translation unit: src/VMCore/CommonProc.cpp
// Original header:           inc/VMCore/CommonProc.h
//
// The key container types live in inc/common/VMDefines.h of the original tree
// (lines 311..327).  They are reproduced here unchanged because this unit is
// their only reconstructed consumer; every offset below is asserted by the
// ORIG disassembly of GetLinkData / GetTreeData / GetMatrixData /
// InitKeyTree / InitKeyLink (documented per function in the .cpp).
//
//   struct tagKEYLINKNODE  (VMDefines.h:311, sizeof 8)   next +0, key +4
//   struct tagKEYTREENODE  (VMDefines.h:317, sizeof 12)  left +0, right +4, key +8
//   typedef UINT32 **PKEYMATRIX (VMDefines.h:327)
#ifndef SECSVR_INC_VMCORE_COMMONPROC_H_
#define SECSVR_INC_VMCORE_COMMONPROC_H_

#include "inc/common/Types.h"

struct tagKEYLINKNODE {         // 0x00193310 GetLinkData walks *pHeader at +0
    tagKEYLINKNODE *next;       // +0
    UINT32 key;                 // +4
};
typedef tagKEYLINKNODE *PKEYLINKNODE;

struct tagKEYTREENODE {         // 0x00193c90 InitKeyTree stores 3 words
    tagKEYTREENODE *left;       // +0
    tagKEYTREENODE *right;      // +4
    UINT32 key;                 // +8
};
typedef tagKEYTREENODE *PKEYTREENODE;

typedef UINT32 **PKEYMATRIX;

// TenCrypt boundary used by GenKey.  include/TenCrypt.h declares every Ten*
// entry point this unit calls except this one; the ORIG symbol is the
// C++-mangled _Z6TenMd5PhjS_ (0x001b8960), called at 0x00193499.
// ORIG returns 16 (the MD5 digest size) and takes a non-const source pointer.
extern unsigned int TenMd5(unsigned char *pbySrc, UINT32 dwSrcLen,
                           unsigned char *pbyDest);

// Bit / buffer helpers.  All of them are plain globals in the ORIG (exported,
// no namespace) and are no-ops that leave the buffer untouched when the
// pointer is NULL; the count/length forms additionally do nothing when
// count <= 0.
void rol(unsigned char *data, int time);                        // 0x00193190
void ror(unsigned char *data, int time);                        // 0x001931e0
void xora(unsigned char *start, int count, UINT32 seed);        // 0x00193230
void anda(unsigned char *start, int count, UINT32 seed);        // 0x00193270
void ora(unsigned char *start, int count, UINT32 seed);         // 0x001932b0
void nota(unsigned char *start, int count);                     // 0x001932f0

// Key container accessors and lifetime helpers.
PKEYLINKNODE GetLinkData(PKEYLINKNODE pHeader, int count);          // 0x00193310
PKEYTREENODE GetTreeData(PKEYTREENODE pHeader, UINT32 &count);      // 0x00193350
PUINT32 GetMatrixData(PKEYMATRIX pHeader, int count, int x, int y); // 0x001933e0
BOOL GenKey(int nType, UINT32 dwKeySeed, PBYTE &pbyKey);            // 0x00193420
BOOL Cipher(int nType, PBYTE pbyBuffer, UINT32 dwBufLen,
            UINT32 dwKeySeed, bool bCipherFlag);                    // 0x001934c0
BOOL CallDecrypt(int nCode, PBYTE pbyBuffer, UINT32 dwBufLen, PBYTE pbyKey,
                 UINT32 dwKeyLen, bool bIsEncode);                  // 0x00193800
BOOL UninitKeyMatrix(PKEYMATRIX pHeader, int y);                    // 0x00193ac0
PKEYMATRIX InitKeyMatrix(UINT32 *arrKey, int x, int y);             // 0x00193b10
BOOL UninitKeyTree(PKEYTREENODE pHeader);                           // 0x00193be0
BOOL UninitKeyLink(PKEYLINKNODE pHeader);                           // 0x00193c50
PKEYTREENODE InitKeyTree(UINT32 *arrKey, int count, int &result);   // 0x00193c90
PKEYLINKNODE InitKeyLink(UINT32 *arrKey, int count);                // 0x00193e00

#endif // SECSVR_INC_VMCORE_COMMONPROC_H_
