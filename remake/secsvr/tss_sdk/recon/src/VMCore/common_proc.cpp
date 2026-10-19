// Semantic reconstruction of src/VMCore/CommonProc.cpp from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG ranges (half-open, sizes from DWARF DW_AT_high_pc):
//   rol(unsigned char*, int)                               0x00193190 0x47
//   ror(unsigned char*, int)                               0x001931e0 0x4c
//   xora(unsigned char*, int, unsigned int)                0x00193230 0x3b
//   anda(unsigned char*, int, unsigned int)                0x00193270 0x3b
//   ora(unsigned char*, int, unsigned int)                 0x001932b0 0x3b
//   nota(unsigned char*, int)                              0x001932f0 0x1f
//   GetLinkData(tagKEYLINKNODE*, int)                      0x00193310 0x31
//   GetTreeData(tagKEYTREENODE*, unsigned int&)            0x00193350 0x87
//   GetMatrixData(unsigned int**, int, int, int)           0x001933e0 0x3e
//   GenKey(int, unsigned int, unsigned char*&)             0x00193420 0x9d
//   Cipher(int, unsigned char*, unsigned int, unsigned int, bool)   0x001934c0 0x33a
//   CallDecrypt(int, unsigned char*, unsigned int, unsigned char*,
//               unsigned int, bool)                        0x00193800 0x2b1
//   UninitKeyMatrix(unsigned int**, int)                   0x00193ac0 0x4b
//   InitKeyMatrix(unsigned int*, int, int)                 0x00193b10 0xcc
//   UninitKeyTree(tagKEYTREENODE*)                         0x00193be0 0x68
//   UninitKeyLink(tagKEYLINKNODE*)                         0x00193c50 0x3c
//   InitKeyTree(unsigned int*, int, int&)                  0x00193c90 0x16b
//   InitKeyLink(unsigned int*, int)                        0x00193e00 0xc2
//
// The dispatcher jump tables were decoded from the ORIG image; see the
// comments at Cipher / CallDecrypt.  No symbol outside the ORIG's own import
// set and the TenCrypt boundary is referenced.

#include <string.h>

#include "src/VMCore/common_proc.h"
#include "include/TenCrypt.h"

// ---------------------------------------------------------------------------
// Byte rotate helpers.  Both index the buffer only after the NULL check and
// reduce the shift count modulo 8 with C's % on a signed int (the ORIG emits
// the sign-correcting sar/shr/add/and/sub sequence for `time % 8`).
// ---------------------------------------------------------------------------

void rol(unsigned char *data, int time) {
    if (data == NULL) return;
    int nShift = time % 8;                          // 0x001931a7..0x001931bc
    *data = (unsigned char)((*data >> (8 - nShift)) | (*data << nShift));
}

void ror(unsigned char *data, int time) {
    if (data == NULL) return;
    int nShift = time % 8;                          // 0x001931f6..0x00193208
    *data = (unsigned char)((*data << (8 - nShift)) | (*data >> nShift));
}

// The seed is consumed as four little-endian bytes: the ORIG computes the
// address of the by-value argument (`lea 0x10(%ebp),%edi`) and indexes it with
// i % 4.
void xora(unsigned char *start, int count, UINT32 seed) {
    if (start == NULL) return;
    if (count <= 0) return;
    unsigned char *s = (unsigned char *)&seed;
    for (int i = 0; i < count; i++) start[i] ^= s[i % 4];
}

void anda(unsigned char *start, int count, UINT32 seed) {
    if (start == NULL) return;
    if (count <= 0) return;
    unsigned char *s = (unsigned char *)&seed;
    for (int i = 0; i < count; i++) start[i] &= s[i % 4];
}

void ora(unsigned char *start, int count, UINT32 seed) {
    if (start == NULL) return;
    if (count <= 0) return;
    unsigned char *s = (unsigned char *)&seed;
    for (int i = 0; i < count; i++) start[i] |= s[i % 4];
}

void nota(unsigned char *start, int count) {
    if (start == NULL) return;
    for (int i = 0; i < count; i++) start[i] = (unsigned char)~start[i];
}

// ---------------------------------------------------------------------------
// Key container accessors.
// ---------------------------------------------------------------------------

// Walk `count` links.  count <= 0 returns the head unchanged; a NULL input
// returns NULL.  On the last hop the ORIG returns whatever the load produced,
// so an exact-length walk returns NULL instead of aborting early.
PKEYLINKNODE GetLinkData(PKEYLINKNODE pHeader, int count) {
    PKEYLINKNODE pRecent;
    if (pHeader == NULL) return NULL;
    if (count <= 0) return pHeader;
    pRecent = pHeader;
    for (int i = 0; i < count; i++) {
        pRecent = pRecent->next;                    // 0x00193337
        if (pRecent == NULL) break;                 // 0x00193330
    }
    return pRecent;
}

// Depth-first search that consumes one cursor unit per visited node: `count`
// is the in/out cursor, decremented by one per level.  The left subtree is
// tried first and the right one only when the left returned NULL; the
// decremented cursor is written back to `count` on every path except the
// count == 0 early return, so a failed search still advances the caller's
// cursor by the number of levels visited.
PKEYTREENODE GetTreeData(PKEYTREENODE pHeader, UINT32 &count) {
    UINT32 c;
    PKEYTREENODE pTmp;
    if (count == 0) return pHeader;                 // 0x00193370..0x00193379
    c = count - 1;                                  // 0x0019337d
    pTmp = NULL;
    if (pHeader->left != NULL) pTmp = GetTreeData(pHeader->left, c);
    if (pTmp == NULL && pHeader->right != NULL) pTmp = GetTreeData(pHeader->right, c);
    count = c;                                      // 0x001933a1 / 0x001933d3
    return pTmp;
}

// Row-major element lookup: returns &pHeader[count / x][count % x], but only
// when the matrix is non-empty in both dimensions, the linear index is inside
// the x*y area, and the selected row pointer is non-NULL.  The size test is a
// signed compare (`jl`), reproduced by the int arithmetic below.
PUINT32 GetMatrixData(PKEYMATRIX pHeader, int count, int x, int y) {
    if (pHeader == NULL) return NULL;
    if (x == 0) return NULL;
    if (y == 0) return NULL;
    if (y * x < count) return NULL;                 // 0x00193404..0x00193409
    if (pHeader[count / x] == NULL) return NULL;    // 0x00193412..0x0019341a
    return &pHeader[count / x][count % x];
}

// ---------------------------------------------------------------------------
// Key generation / cipher dispatch.
// ---------------------------------------------------------------------------

// Two MD5 rounds over an 8-byte source block, filling the 32-byte output with
// 16 bytes each.  The block is { dwKeySeed, nType + round } in native (little
// endian) form; the 10-byte scratch buffer is zeroed first (its trailing two
// bytes are never read - only dwProcLen = 8 bytes are hashed).
BOOL GenKey(int nType, UINT32 dwKeySeed, PBYTE &pbyKey) {
    PBYTE pbyTmpKey = new BYTE[32];                 // 0x0019343c operator new[]
    UINT32 dwTmpKeyLen = 32;
    UINT32 dwProcLen = 8;
    BYTE byTmpRandBuf[10];                          // DWARF upper_bound = 9
    UINT32 len;

    memset(byTmpRandBuf, 0, sizeof(byTmpRandBuf));  // 0x0019344b..0x00193459

    for (len = 0; len < 2; len++) {
        *(UINT32 *)&byTmpRandBuf[0] = dwKeySeed;    // 0x00193474
        *(UINT32 *)&byTmpRandBuf[4] = (UINT32)(nType + len);  // 0x00193482
        TenMd5(byTmpRandBuf, dwProcLen, pbyTmpKey + len * (dwTmpKeyLen / 2));
    }

    pbyKey = pbyTmpKey;                             // 0x001934a3..0x001934a9
    return TRUE;
}

// Cipher dispatcher.  The ORIG jump table (EBX 0x00294534) is at 0x0023ff40,
// 20 entries, index = nType:
//   0  -> .L77 (default)     1 TenIdea        2 TenDesNew    3 TenDesBig
//   4  TenCast               5 TenTean        6 TenRc2        7 TenTwofish32
//   8  TenTwofish24          9 TenMmb        10 TenNsea      11 TenTwofish16
//  12  TenSquare            13 TenMars       14 TenRc4       15 TenSeattos
//  16  TenGene              17 TenRc6        18 TenRc5       19 TenGeneNew
// Every case passes (pbyKey, bCipherFlag, pbyBuffer, dwBufLen) and returns
// TRUE; the key buffer is released with delete[].  nType 0 and nType > 19 keep
// bRetCode == FALSE (the ORIG's default block at 0x00193533).  The exception
// landing pad at 0x001937e6 catches a throwing Ten* entry and returns TRUE.
BOOL Cipher(int nType, PBYTE pbyBuffer, UINT32 dwBufLen, UINT32 dwKeySeed,
            bool bCipherFlag) {
    BOOL bRetCode = FALSE;
    if (dwBufLen > 0xffff) return bRetCode;         // 0x001934de..0x001934f4
    PBYTE pbyKey = NULL;

    // The ORIG try region covers the GenKey call as well: an exception raised
    // by the key allocation reaches the landing pad and the function still
    // reports success (probed against the ORIG, see AGENT_REPORT_vm_codec.md).
    try {
        GenKey(nType, dwKeySeed, pbyKey);           // 0x0019351e
        switch (nType) {
        case 1:  TenIdea(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 2:  TenDesNew(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 3:  TenDesBig(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 4:  TenCast(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 5:  TenTean(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 6:  TenRc2(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 7:  TenTwofish32(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 8:  TenTwofish24(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 9:  TenMmb(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 10: TenNsea(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 11: TenTwofish16(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 12: TenSquare(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 13: TenMars(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 14: TenRc4(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 15: TenSeattos(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 16: TenGene(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 17: TenRc6(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 18: TenRc5(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        case 19: TenGeneNew(pbyKey, bCipherFlag, pbyBuffer, dwBufLen); bRetCode = TRUE; break;
        default: bRetCode = FALSE; break;           // 0x00193533, shared with case 0
        }
    } catch (...) {
        bRetCode = TRUE;                            // 0x001937e6 landing pad
    }

    if (pbyKey != NULL) delete[] pbyKey;            // 0x0019353a..0x00193549
    return bRetCode;
}

// CallDecrypt dispatcher.  The ORIG jump table (EBX 0x00294534) is at
// 0x0023ff90, 14 entries, index = nCode; case 0 and nCode > 13 return FALSE.
// Each arm validates the key length before calling the Ten* entry point:
//   1  TenDes       dwKeyLen == 8      2  TenIdea    == 16   3  TenD3des24 == 24
//   4  TenD3des162  == 16              5  TenD3des161 == 16   6  TenCast   == 16
//   7  TenTean      == 16              8  TenRc6      == 16   9  TenGost   == 32
//  10  TenMars      == 16             11  TenMagic8   == 8   12  TenMagic16 == 16
//  13  TenMagic32   == 32
BOOL CallDecrypt(int nCode, PBYTE pbyBuffer, UINT32 dwBufLen, PBYTE pbyKey,
                 UINT32 dwKeyLen, bool bIsEncode) {
    if (dwBufLen > 0xffff) return FALSE;            // 0x00193824..0x00193830
    if (dwKeyLen > 0xffff) return FALSE;            // 0x00193832..0x00193838

    switch (nCode) {
    case 1:
        if (dwKeyLen != 8) return FALSE;
        TenDes(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 2:
        if (dwKeyLen != 16) return FALSE;
        TenIdea(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 3:
        if (dwKeyLen != 24) return FALSE;
        TenD3des24(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 4:
        if (dwKeyLen != 16) return FALSE;
        TenD3des162(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 5:
        if (dwKeyLen != 16) return FALSE;
        TenD3des161(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 6:
        if (dwKeyLen != 16) return FALSE;
        TenCast(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 7:
        if (dwKeyLen != 16) return FALSE;
        TenTean(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 8:
        if (dwKeyLen != 16) return FALSE;
        TenRc6(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 9:
        if (dwKeyLen != 32) return FALSE;
        TenGost(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 10:
        if (dwKeyLen != 16) return FALSE;
        TenMars(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 11:
        if (dwKeyLen != 8) return FALSE;
        TenMagic8(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 12:
        if (dwKeyLen != 16) return FALSE;
        TenMagic16(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    case 13:
        if (dwKeyLen != 32) return FALSE;
        TenMagic32(pbyKey, bIsEncode, pbyBuffer, dwBufLen);
        return TRUE;
    default:
        return FALSE;
    }
}

// ---------------------------------------------------------------------------
// Key material lifetime helpers.  Every one of them returns TRUE except the
// UninitKeyTree catch-all, which returns FALSE (ORIG landing pad 0x00193c37).
// ---------------------------------------------------------------------------

// Frees the y row allocations and NULLs the slots; the row-pointer array
// itself is owned by the caller and is not freed here.  pHeader is not
// NULL-checked in the ORIG.
BOOL UninitKeyMatrix(PKEYMATRIX pHeader, int y) {
    for (int i = 0; i < y; i++) {                   // y <= 0 -> no work
        if (pHeader[i] != NULL) {
            delete pHeader[i];                      // 0x00193aea operator delete
            pHeader[i] = NULL;
        }
    }
    return TRUE;
}

// Builds y rows of x UINT32 copied from the flat arrKey.  On an allocation
// failure the already built rows are released, the header is set to NULL and
// NULL is returned.
PKEYMATRIX InitKeyMatrix(UINT32 *arrKey, int x, int y) {
    PKEYMATRIX pHeader = NULL;
    UINT32 *pLine;
    if (arrKey == NULL) return pHeader;             // 0x00193b31..0x00193b33
    pHeader = new UINT32 *[y];                      // 0x00193b3e, y * 4 bytes
    pHeader[0] = NULL;                              // 0x00193b46
    if (y <= 0) return pHeader;
    for (int i = 0; i < y; i++) {
        pLine = new UINT32[x];                      // 0x00193b6d, x * 4 bytes
        if (pLine == NULL) {                        // 0x00193bb8
            UninitKeyMatrix(pHeader, y);
            pHeader = NULL;
            return pHeader;
        }
        for (int j = 0; j < x; j++) pLine[j] = arrKey[i * x + j];
        pHeader[i] = pLine;
    }
    return pHeader;
}

// Recursive post-order release of the whole tree.  An empty tree is "already
// uninitialised" and returns TRUE.
BOOL UninitKeyTree(PKEYTREENODE pHeader) {
    try {
        if (pHeader == NULL) return TRUE;           // 0x00193c01 returns TRUE
        if (pHeader->left != NULL) UninitKeyTree(pHeader->left);
        if (pHeader->right != NULL) UninitKeyTree(pHeader->right);
        delete pHeader;                             // 0x00193c23 operator delete
        return TRUE;
    } catch (...) {
        return FALSE;                               // 0x00193c44 xor eax,eax
    }
}

// Iterative release of the whole singly linked list.
BOOL UninitKeyLink(PKEYLINKNODE pHeader) {
    PKEYLINKNODE pRecent;
    PKEYLINKNODE pTmp;
    if (pHeader != NULL) {
        pRecent = pHeader;
        do {
            pTmp = pRecent->next;                   // 0x00193c70
            delete pRecent;                         // 0x00193c75
            pRecent = pTmp;
        } while (pTmp != NULL);
    }
    return TRUE;
}

// Balanced tree build: for count >= 2 the root key is arrKey[count / 2], the
// left subtree is built from arrKey[0 .. count/2-1] and the right one from
// arrKey[count/2+1 .. count-1].  `result` is set to TRUE up front and to FALSE
// as soon as a sub-build fails; on failure the whole partial tree is released
// and NULL returned.  node = { left +0, right +4, key +8 }.
PKEYTREENODE InitKeyTree(UINT32 *arrKey, int count, int &result) {
    PKEYTREENODE pHeader = NULL;
    int r = 0;
    result = TRUE;                                  // 0x00193cb9
    if (count == 0) return pHeader;                 // 0x00193cbf
    if (count == 1) {                               // 0x00193cc5
        pHeader = new tagKEYTREENODE;               // 0x00193cd5, 0xc bytes
        if (pHeader == NULL) {                      // 0x00193de6
            result = FALSE;
            return NULL;
        }
        pHeader->left = NULL;                       // 0x00193d7b
        pHeader->right = NULL;                      // 0x00193d81
        pHeader->key = arrKey[0];                   // 0x00193d88
        return pHeader;
    }
    pHeader = new tagKEYTREENODE;                   // 0x00193cd5, 0xc bytes
    if (pHeader == NULL) {                          // 0x00193dd1
        result = FALSE;
        return NULL;
    }
    pHeader->left = InitKeyTree(arrKey, count / 2, r);          // 0x00193d03
    if (r == FALSE) {                                           // 0x00193d12
        result = FALSE;
        delete pHeader;                             // 0x00193dc0
        return NULL;
    }
    pHeader->key = arrKey[count / 2];               // 0x00193d28
    pHeader->right = InitKeyTree(arrKey + count / 2 + 1,
                                 count - count / 2 - 1, r);      // 0x00193d3c
    if (r == FALSE) {                                           // 0x00193d4c
        result = FALSE;
        UninitKeyTree(pHeader->left);               // 0x00193d9b
        delete pHeader;                             // 0x00193da6
        return NULL;
    }
    return pHeader;
}

// Builds a count-node singly linked list copying arrKey[] in order.  On an
// allocation failure the chain built so far is released, the head is cleared
// and NULL is returned.
PKEYLINKNODE InitKeyLink(UINT32 *arrKey, int count) {
    PKEYLINKNODE pHeader = NULL;
    PKEYLINKNODE pRecent = NULL;
    PKEYLINKNODE pTmp = NULL;

    if (arrKey == NULL) return pHeader;             // 0x00193e17..0x00193e19
    if (count == 0) return pHeader;                 // 0x00193e22..0x00193e24

    pTmp = new tagKEYLINKNODE;                      // 0x00193e31, 8 bytes
    if (pTmp == NULL) return pHeader;               // 0x00193e38 returns NULL
    pTmp->next = NULL;                              // 0x00193e4c
    pTmp->key = arrKey[0];                          // 0x00193e52
    pHeader = pRecent = pTmp;                       // 0x00193e40 / 0x00193e57

    for (int i = 1; i < count; i++) {
        pTmp = new tagKEYLINKNODE;                  // 0x00193e82, 8 bytes
        if (pTmp == NULL) {                         // 0x00193e8d
            UninitKeyLink(pHeader);
            pHeader = NULL;
            return pHeader;
        }
        pTmp->next = NULL;                          // 0x00193e63
        pRecent->next = pTmp;                       // 0x00193e69
        pRecent = pTmp;                             // 0x00193e6b
        pTmp->key = arrKey[i];                      // 0x00193e76
    }
    return pHeader;                                 // 0x00193eb7
}
