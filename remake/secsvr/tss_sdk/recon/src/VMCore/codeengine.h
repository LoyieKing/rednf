// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
// Original translation unit: src/VMCore/codeengine.cpp
// Original header:           inc/VMCore/codeengine.h
//
// Codec boundary of the VM transport.  Every function takes a cursor
// (`char **`) that is advanced in place, plus the data pointer and the byte
// count; all of them return the number of bytes transferred, or 0 on a NULL
// cursor / NULL data pointer / non-positive length.  The returned int is
// always usable as "bytes consumed" by the matching caller.
//
// The ORIG symbols are exported globals (T) with these exact signatures:
//   _Z10EncodeCharPPch     _Z10DecodeCharPPcPh    _Z12DecodeMemIntPPcS_i
//   _Z12EncodeMemIntPPcS_i _Z9DecodeMemPPcS_s     _Z9EncodeMemPPcS_s
//   _Z12DecodeStringPPcS_s _Z9DecodeIntPPcPj      _Z9EncodeIntPPcj
//   _Z11DecodeShortPPcPt   _Z11EncodeShortPPct    _Z12EncodeStringPPcS_s
#ifndef SECSVR_INC_VMCORE_CODEENGINE_H_
#define SECSVR_INC_VMCORE_CODEENGINE_H_

int EncodeChar(char **pstrEncode, unsigned char ucSrc);          // 0x00196fe0
int DecodeChar(char **pstrDecode, unsigned char *pucDest);       // 0x00197010
int DecodeMemInt(char **pstrDecode, char *pcDest, int sMemSize); // 0x00197040
int EncodeMemInt(char **pstrEncode, char *pcSrc, int sMemSize);  // 0x001970b0
int DecodeMem(char **pstrDecode, char *pcDest, short sMemSize);  // 0x00197120
int EncodeMem(char **pstrEncode, char *pcSrc, short sMemSize);   // 0x00197180
int DecodeString(char **pstrDecode, char *strDest, short sMaxStrLength);  // 0x001971e0
int DecodeInt(char **pstrDecode, unsigned int *puiDest);         // 0x00197280
int EncodeInt(char **pstrEncode, unsigned int uiSrc);            // 0x001972c0
int DecodeShort(char **pstrDecode, unsigned short *pusDest);     // 0x00197300
int EncodeShort(char **pstrEncode, unsigned short usSrc);        // 0x00197340
int EncodeString(char **pstrEncode, char *strSrc, short sMaxStrLength);   // 0x00197380

#endif // SECSVR_INC_VMCORE_CODEENGINE_H_
