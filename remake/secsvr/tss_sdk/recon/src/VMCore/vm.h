// Reconstructed from libtss_sdk.so.1.0.394
// CU: src/VMCore/VM.cpp, src/VMCore/UPVM.cpp,
//     src/VMCore/ServerCryptCodeMgr.cpp
//
// Original headers for these units were inc/VMCore/VM.h, VMBase.h, UPVM.h,
// ServerCryptCodeMgr.h and VMDefines.h.  Per this assignment the declarations
// are consolidated here (one header for the VMCore CUs).
//
// DWARF layouts (byte-for-byte from the original):
//   CVMBase    8   vptr + unnamed-enum VMError
//   CVM      124   see class below
//   CUPVM    124   identical to CVM (no additional members)
//   CServerCryptCodeMgr 4  vptr only
//   CCryptData         4  vptr only (defined in cryptdata.cpp)
//
// CommonProc helpers (rol/ror/xora/anda/ora/nota/CallDecrypt) and the
// codeengine cursor helpers (DecodeShort) are owned by their own
// reconstruction units.  Their canonical headers are included here instead of
// duplicating the declarations, so a translation unit may include both
// without redefinition conflicts.
#ifndef TSS_SDK_RECON_SRC_VMCORE_VM_H_
#define TSS_SDK_RECON_SRC_VMCORE_VM_H_

#include "inc/common/Types.h"
#include "src/VMCore/common_proc.h"
#include "src/VMCore/codeengine.h"

// ---------------------------------------------------------------------------
// VMDefines.h types (VM.h:5 in DWARF).
// ---------------------------------------------------------------------------
struct tagRESERVEREGSTERS {      // sizeof 24
    PBYTE CBR;                   // +0x00  code base register
    PBYTE DP;                    // +0x04  data pointer register
    PVOID KMP;                   // +0x08  key matrix pointer
    PVOID KLP;                   // +0x0c  key link pointer
    PVOID KTP;                   // +0x10  key tree pointer
    UINT32 CL;                   // +0x14  code length
};
typedef tagRESERVEREGSTERS RESERVEREGSTERS;

struct tagDATAHEADER {           // sizeof 6
    WORD Len;                    // +0
    WORD CodeLen;                // +2
    WORD DataLen;                // +4
};
typedef tagDATAHEADER DATAHEADER;



// ---------------------------------------------------------------------------
// CCryptData (src/VMCore/cryptdata.cpp; DWARF decl_file 2 = cryptdata.h:20).
// Only the vptr-sized layout and the two symbols used by ServerCryptCodeMgr
// are needed here.  The definition of the members lives in cryptdata.cpp.
// ---------------------------------------------------------------------------
class CCryptData {
public:
    CCryptData();
    virtual ~CCryptData();
    BOOL EncryptData(unsigned short ushCryptID, unsigned char *pbyCryptKey,
                     bool bEnCrypt, unsigned char *pbySrcData,
                     unsigned int unSrcDataLen);
};

// ---------------------------------------------------------------------------
// CServerCryptCodeMgr (src/VMCore/ServerCryptCodeMgr.cpp; sizeof 4).
// ---------------------------------------------------------------------------
class CServerCryptCodeMgr {
public:
    CServerCryptCodeMgr();
    virtual ~CServerCryptCodeMgr();
    BOOL CallCryptCode(WORD wIndex, PBYTE pbyData, WORD wDataLen,
                       PBYTE pbyKey, WORD wKeyLen, BOOL blIsEncode);
};

// ---------------------------------------------------------------------------
// CVMBase (VMBase.h; sizeof 8).  The VM error enum is unnamed in DWARF
// ("enum ._1"); its enumerators are class-scope names.
// ---------------------------------------------------------------------------
class CVMBase {
public:
    enum {
        NoError            = 0,
        MallocMemoryErr    = 1,
        InvalidInstruction = 2,
        NullPointer        = 3,
        InvalidHeader      = 4,
        RunTimeError       = 5,
        KeyLinkError       = 6,
        KeyMatrixError     = 7,
        KeyTreeError       = 8,
        PackageCheckErr    = 9,
        NoByteCode         = 10,
        BinCodeError       = 11
    } VMError;

    virtual BOOL Init() = 0;
    virtual void Release() = 0;
    virtual BOOL GetDecodeCode(BYTE *pbyCodeBuff, PBYTE &pbyDecodeCode,
                               WORD &wCodeLen) = 0;
    virtual BOOL GetEncodeCode(BYTE *pbyCodeBuff, PBYTE &pbyEncodeCode,
                               WORD &wCodeLen) = 0;
    virtual void GetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen) = 0;
    virtual void SetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen) = 0;
    virtual void GetDataBuffer(PBYTE &pData, UINT32 &dwLen) = 0;
    virtual BOOL SetKeyTree(PVOID pTree, UINT32 dwSize) = 0;
    virtual BOOL SetKeyMatrix(PVOID pMatrix, UINT32 X, UINT32 Y) = 0;
    virtual BOOL SetKeyLink(PVOID pLink, UINT32 dwSize) = 0;
    virtual BOOL Run() = 0;
    virtual BOOL Load(PVOID pCodeBuffer, SIZE_T CodeBufferSize,
                      PVOID pData, SIZE_T DataSize) = 0;
    virtual int GetLastError() = 0;
    virtual void SetError(int nErrorCode) = 0;
};

// ---------------------------------------------------------------------------
// CVM (VM.h; sizeof 124)
// ---------------------------------------------------------------------------
class CVM : public CVMBase {
public:
    CVM();
    virtual ~CVM();

    virtual BOOL Init();
    virtual void Release();
    virtual BOOL GetDecodeCode(BYTE *pbyCodeBuff, PBYTE &pbyDecodeCode,
                               WORD &wCodeLen);
    virtual BOOL GetEncodeCode(BYTE *pbyCodeBuff, PBYTE &pbyEncodeCode,
                               WORD &wCodeLen);
    virtual void GetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen);
    virtual void SetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen);
    virtual void GetDataBuffer(PBYTE &pData, UINT32 &dwLen);
    virtual BOOL SetKeyTree(PVOID pTree, UINT32 dwSize);
    virtual BOOL SetKeyMatrix(PVOID pMatrix, UINT32 X, UINT32 Y);
    virtual BOOL SetKeyLink(PVOID pLink, UINT32 dwSize);
    virtual BOOL Run();
    virtual BOOL Load(PVOID pCodeBuffer, SIZE_T CodeBufferSize,
                      PVOID pData, SIZE_T DataSize);
    virtual int GetLastError();
    virtual void SetError(int nErrorCode);

    // --- members, DWARF order and offsets -------------------------------
    int m_iErrCode;                  // +0x08
    CServerCryptCodeMgr *m_pCodeMgr; // +0x0c
    UINT32 m_dwTreeLength;           // +0x10
    UINT32 m_dwLinkLength;           // +0x14
    PVOID m_pTree;                   // +0x18
    PVOID m_pMatrix;                 // +0x1c
    PVOID m_pLink;                   // +0x20
    UINT32 m_MatrixX;                // +0x24
    UINT32 m_MatrixY;                // +0x28
    // Register file: [0] = PC, [1] = SP, [2] = data length, [3] = loop
    // counter / quotient, [4] = remainder / shift count, [5], [6] = key
    // index/value, [7..12] general purpose.  Load() seeds [1] = 0x8000.
    UINT32 m_Reg[13];                // +0x2c
    RESERVEREGSTERS m_ReserveReg;    // +0x60
    PVOID m_lpMem;                   // +0x78  malloc(0x10400) scratch arena
};

// ---------------------------------------------------------------------------
// CUPVM (UPVM.h; sizeof 124, identical layout to CVM)
// ---------------------------------------------------------------------------
class CUPVM : public CVM {
public:
    CUPVM();
    virtual ~CUPVM();
    virtual BOOL Init();
    virtual void Release();
    virtual BOOL Run();
};

// Factories, ORIG 0x00196ea0 / 0x00195120.
CVMBase *GetVM();
CVMBase *GetUPVM();

#endif // TSS_SDK_RECON_SRC_VMCORE_VM_H_
