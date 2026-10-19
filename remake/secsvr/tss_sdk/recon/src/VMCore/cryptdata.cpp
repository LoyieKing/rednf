// Reconstructed from libtss_sdk.so.1.0.394
// CU: src/VMCore/cryptdata.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
//
// EncryptData is a 27-entry dispatch observed at 0x001975a0:
//   0        -> TRUE (no-op)
//   1..25    -> Ten<Algorithm>(key, bEnCrypt, src, len); TRUE
//   26       -> TenTean(...); TRUE
//   default  -> FALSE
// The jump table at 0x002402e4 was decoded against EBX 0x00294534; every case
// passes (pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen) unchanged.
#include "inc/VMCore/cryptdata.h"

CCryptData::CCryptData() {}
CCryptData::~CCryptData() {}

BOOL CCryptData::EncryptData(unsigned short ushCryptID, unsigned char *pbyCryptKey,
                             bool bEnCrypt, unsigned char *pbySrcData,
                             unsigned int unSrcDataLen) {
    switch (ushCryptID) {
    case 0:
        return TRUE;
    case 1:
        TenAdder(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 2:
        TenBlowFish16(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 3:
        TenBlowFish24(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 4:
        TenBlowFish32(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 5:
        TenBlowFish56(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 6:
        TenBlowFish8(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 7:
        TenCast(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 8:
        TenD3des161(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 9:
        TenD3des162(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 10:
        TenDes2P(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 11:
        TenDesBig(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 12:
        TenDesMo(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 13:
        TenDesNew(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 14:
        TenDesPC(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 15:
        TenDesToo(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 16:
        TenFastDes(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 17:
        TenFeal8(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 18:
        TenGene(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 19:
        TenGost(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 20:
        TenGramary(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 21:
        TenIdea(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 22:
        TenMars(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 23:
        TenRc6(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 24:
        TenShift(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 25:
        TenSubtracter(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    case 26:
        TenTean(pbyCryptKey, bEnCrypt, pbySrcData, unSrcDataLen);
        return TRUE;
    default:
        return FALSE;
    }
}
