// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
// Original translation unit: src/VMCore/codeengine.cpp
// Original header:           inc/VMCore/codeengine.h
//
// The unit is the little-endian <-> network-order codec of the VM transport:
// every Encode*/Decode* pair swaps the byte order of its operand (2 bytes for
// the short/char forms, 4 bytes for the int forms, or the full buffer for the
// Mem forms) and returns the number of bytes it consumed/produced, or 0 when
// the cursor, the destination or the length is unusable.
//
// Argument order is always (cursor, data, size); the cursor is advanced in
// place and never left pointing past what was actually processed.
//
// ORIG ranges (half-open, sizes from DWARF DW_AT_high_pc):
//   EncodeChar(char**, unsigned char)                0x00196fe0 0x24
//   DecodeChar(char**, unsigned char*)               0x00197010 0x2a
//   DecodeMemInt(char**, char*, int)                 0x00197040 0x61
//   EncodeMemInt(char**, char*, int)                 0x001970b0 0x61
//   DecodeMem(char**, char*, short)                  0x00197120 0x5f
//   EncodeMem(char**, char*, short)                  0x00197180 0x5f
//   DecodeString(char**, char*, short)               0x001971e0 0x94
//   DecodeInt(char**, unsigned int*)                 0x00197280 0x3a
//   EncodeInt(char**, unsigned int)                  0x001972c0 0x38
//   DecodeShort(char**, unsigned short*)             0x00197300 0x36
//   EncodeShort(char**, unsigned short)              0x00197340 0x34
//   EncodeString(char**, char*, short)               0x00197380 0x9d

#include <string.h>

#include "src/VMCore/codeengine.h"

// Little-endian <-> big-endian reversal notes.  The ORIG (GCC 4.1) expands the
// 32-bit byte reversal to the i386 sequence "ror 8,%ax / ror 16,%eax / ror
// 8,%ax" (0x001972a2, 0x001972a6, 0x001972a9, mirrored in EncodeInt at
// 0x001972da) - i.e. it keeps the value in 16-bit halves - and the 16-bit
// reversal to a single 16-bit rotate by 8 (0x00197324 / 0x0019735b).
// The reconstructions below express the 32-bit reversal as two 16-bit byte
// swaps combined with a 16-bit position swap, which is the same function.

// ---------------------------------------------------------------------------
// Single byte.
// ---------------------------------------------------------------------------

int EncodeChar(char **pstrEncode, unsigned char ucSrc) {
    if (pstrEncode == NULL) return 0;               // 0x00196fe9
    if (*pstrEncode == NULL) return 0;              // 0x00196fef
    **pstrEncode = (char)ucSrc;                     // 0x00196ff3
    (*pstrEncode)++;                                // 0x00196ffa
    return 1;
}

int DecodeChar(char **pstrDecode, unsigned char *pucDest) {
    if (pstrDecode == NULL) return 0;               // 0x00197019
    if (*pstrDecode == NULL) return 0;              // 0x0019701f
    if (pucDest == NULL) return 0;                  // 0x00197023
    *pucDest = (unsigned char)**pstrDecode;         // 0x00197027
    (*pstrDecode)++;                                // 0x00197031
    return 1;
}

// ---------------------------------------------------------------------------
// Raw blocks.  sMemSize <= 0 is rejected before memcpy, so a negative size
// never reaches the copy; the returned value is the size as an int.
// ---------------------------------------------------------------------------

int DecodeMemInt(char **pstrDecode, char *pcDest, int sMemSize) {
    if (pstrDecode == NULL) return 0;               // 0x00197063
    if (*pstrDecode == NULL) return 0;              // 0x00197069
    if (pcDest == NULL) return 0;                   // 0x0019706d
    if (sMemSize <= 0) return 0;                    // 0x00197073
    memcpy(pcDest, *pstrDecode, sMemSize);          // 0x00197080
    *pstrDecode += sMemSize;                        // 0x00197085
    return sMemSize;
}

int EncodeMemInt(char **pstrEncode, char *pcSrc, int sMemSize) {
    if (pstrEncode == NULL) return 0;               // 0x001970d3
    if (*pstrEncode == NULL) return 0;              // 0x001970d9
    if (pcSrc == NULL) return 0;                    // 0x001970dd
    if (sMemSize <= 0) return 0;                    // 0x001970e1
    memcpy(*pstrEncode, pcSrc, sMemSize);           // 0x001970f0
    *pstrEncode += sMemSize;                        // 0x001970f5
    return sMemSize;
}

int DecodeMem(char **pstrDecode, char *pcDest, short sMemSize) {
    if (pstrDecode == NULL) return 0;               // 0x00197144
    if (*pstrDecode == NULL) return 0;              // 0x0019714a
    if (pcDest == NULL) return 0;                   // 0x0019714e
    if (sMemSize <= 0) return 0;                    // 0x00197152
    memcpy(pcDest, *pstrDecode, sMemSize);          // 0x00197165
    *pstrDecode += sMemSize;                        // 0x0019716c
    return sMemSize;                                // 0x0019716a sign-extended
}

int EncodeMem(char **pstrEncode, char *pcSrc, short sMemSize) {
    if (pstrEncode == NULL) return 0;               // 0x001971a4
    if (*pstrEncode == NULL) return 0;              // 0x001971aa
    if (pcSrc == NULL) return 0;                    // 0x001971ae
    if (sMemSize <= 0) return 0;                    // 0x001971b2
    memcpy(*pstrEncode, pcSrc, sMemSize);           // 0x001971c5
    *pstrEncode += sMemSize;                        // 0x001971cc
    return sMemSize;                                // 0x001971ca sign-extended
}

// ---------------------------------------------------------------------------
// Length-prefixed string.  The prefix is a big-endian 16-bit length.
// ---------------------------------------------------------------------------

// Reads the 16-bit length, relocates the cursor past it, copies at most
// sMaxStrLength bytes, terminates the destination and returns
// usRealLength + 2.  The ORIG keeps the *unclamped* length for the cursor
// advance and for the return value (edi at 0x0019322f is never rewritten by
// the clamp at 0x00193270), while the copy count and the terminator index use
// the clamped one - reproduced below.
int DecodeString(char **pstrDecode, char *strDest, short sMaxStrLength) {
    unsigned short usTemp;
    unsigned short usTempLength;
    unsigned short usRealLength;

    if (pstrDecode == NULL) return 0;               // 0x001971fd
    if (*pstrDecode == NULL) return 0;              // 0x00197209
    if (strDest == NULL) return 0;                  // 0x00197211
    if (sMaxStrLength <= 0) return 0;               // 0x00197214

    usTemp = *(unsigned short *)(*pstrDecode);      // 0x00197219
    (*pstrDecode) += 2;                             // 0x0019721c
    usRealLength = (unsigned short)((usTemp >> 8) | (usTemp << 8));  // 0x00197229
    usTempLength = usRealLength;
    if ((int)usTempLength > (int)sMaxStrLength) usTempLength = sMaxStrLength;
    memcpy(strDest, *pstrDecode, usTempLength);     // 0x00197247
    (*pstrDecode) += usRealLength;                  // 0x0019724f
    strDest[usTempLength] = '\0';                   // 0x00197254
    return usRealLength + 2;                        // 0x00197258
}

// Writes the big-endian 16-bit length, then the string.  The length is the
// clamp of strlen(strSrc) to sMaxStrLength, and it is used for the copy, the
// cursor advance and the return value; no terminator is written (the prefix
// carries the length).
int EncodeString(char **pstrEncode, char *strSrc, short sMaxStrLength) {
    unsigned short usTemp;
    unsigned short usTempLength;

    if (pstrEncode == NULL) return 0;               // 0x001973a9
    if (*pstrEncode == NULL) return 0;              // 0x001973ab
    if (strSrc == NULL) return 0;                   // 0x001973b1
    if (sMaxStrLength <= 0) return 0;               // 0x001973b8

    usTempLength = (unsigned short)strlen(strSrc);  // 0x001973c3
    if ((int)usTempLength > (int)sMaxStrLength) usTempLength = sMaxStrLength;
    usTemp = (unsigned short)((usTempLength >> 8) | (usTempLength << 8)); // 0x001973d9
    *(unsigned short *)(*pstrEncode) = usTemp;      // 0x001973e1
    *pstrEncode += 2;                               // 0x001973e9
    memcpy(*pstrEncode, strSrc, usTempLength);      // 0x001973f9
    *pstrEncode += usTempLength;                    // 0x00197401
    return usTempLength + 2;                        // 0x001973fe
}

// ---------------------------------------------------------------------------
// Scalar codecs.  Each returns the number of bytes moved (4 / 2 / 1).
// ---------------------------------------------------------------------------

int DecodeInt(char **pstrDecode, unsigned int *puiDest) {
    unsigned int uiTemp;
    if (pstrDecode == NULL) return 0;               // 0x0019728c
    if (*pstrDecode == NULL) return 0;              // 0x00197292
    if (puiDest == NULL) return 0;                  // 0x00197296
    uiTemp = *(unsigned int *)(*pstrDecode);        // 0x0019729a
    (*pstrDecode) += 4;                             // 0x0019729f
    // Full 32-bit byte reversal, built from two 16-bit byte swaps: the ORIG
    // keeps the reversal in 16-bit halves as well (it rotates each half with a
    // 16-bit `ror $8` and swaps the halves with `ror $0x10`).
    {
        unsigned short usLo = (unsigned short)uiTemp;
        unsigned short usHi = (unsigned short)(uiTemp >> 16);
        usLo = (unsigned short)((usLo >> 8) | (usLo << 8));
        usHi = (unsigned short)((usHi >> 8) | (usHi << 8));
        *puiDest = ((unsigned int)usLo << 16) | usHi;   // 0x001972a2..0x001972ad
    }
    return 4;
}

int EncodeInt(char **pstrEncode, unsigned int uiSrc) {
    unsigned int uiTemp;
    if (pstrEncode == NULL) return 0;               // 0x001972cc
    if (*pstrEncode == NULL) return 0;              // 0x001972d2
    {
        unsigned short usLo = (unsigned short)uiSrc;
        unsigned short usHi = (unsigned short)(uiSrc >> 16);
        usLo = (unsigned short)((usLo >> 8) | (usLo << 8));  // 0x001972da..0x001972e5
        usHi = (unsigned short)((usHi >> 8) | (usHi << 8));
        uiTemp = ((unsigned int)usLo << 16) | usHi;
    }
    *(unsigned int *)(*pstrEncode) = uiTemp;        // 0x001972e8
    (*pstrEncode) += 4;                             // 0x001972ef
    return 4;
}

int DecodeShort(char **pstrDecode, unsigned short *pusDest) {
    unsigned short usTemp;
    if (pstrDecode == NULL) return 0;               // 0x0019730c
    if (*pstrDecode == NULL) return 0;              // 0x00197312
    if (pusDest == NULL) return 0;                  // 0x00197316
    usTemp = *(unsigned short *)(*pstrDecode);      // 0x0019731a
    (*pstrDecode) += 2;                             // 0x00197321
    *pusDest = (unsigned short)((usTemp >> 8) | (usTemp << 8));      // 0x00197324
    return 2;
}

int EncodeShort(char **pstrEncode, unsigned short usSrc) {
    unsigned short usTemp;
    if (pstrEncode == NULL) return 0;               // 0x0019734d
    if (*pstrEncode == NULL) return 0;              // 0x00197353
    usTemp = (unsigned short)((usSrc >> 8) | (usSrc << 8));          // 0x0019735b
    *(unsigned short *)(*pstrEncode) = usTemp;      // 0x00197363
    (*pstrEncode) += 2;                             // 0x0019736b
    return 2;
}
