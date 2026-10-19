// Reconstructed from libtss_sdk.so.1.0.394
// CU: src/VMCore/ServerCryptCodeMgr.cpp
//
// ORIG 0x00197420 (C2), 0x00197440 (C1): vptr only, no members.
// ORIG 0x001974f0 (D0), 0x00197520 (D1), 0x00197540 (D2): vptr only.
// ORIG 0x00197460 CallCryptCode:
//
//     CCryptData objCryptData;                       // stack object at ebp-0x10
//     return objCryptData.EncryptData(wIndex, pbyKey, blIsEncode != 0,
//                                     pbyData, wDataLen);
//
// wKeyLen is part of the ABI but is not read by the original body; the
// encryption key length used by CCryptData comes from the algorithm itself.
// The original wraps the call in a landing pad that destroys objCryptData and
// resumes unwinding, which is what the compiler emits for this scope.
#include "src/VMCore/vm.h"

CServerCryptCodeMgr::CServerCryptCodeMgr()
{
}

CServerCryptCodeMgr::~CServerCryptCodeMgr()
{
}

BOOL CServerCryptCodeMgr::CallCryptCode(WORD wIndex, PBYTE pbyData,
                                        WORD wDataLen, PBYTE pbyKey,
                                        WORD wKeyLen, BOOL blIsEncode)
{
    CCryptData objCryptData;
    return objCryptData.EncryptData(wIndex, pbyKey, blIsEncode != 0,
                                    pbyData, wDataLen);
}
