// Reconstructed from libtss_sdk.so.1.0.394
// CU: src/VMCore/VM.cpp
//
// Byte-code VM "CVM" (encoder side).  Everything below is reconstructed from
// the original disassembly at the addresses quoted per function plus DWARF
// class/member/parameter data.  The VM arena is one malloc(0x10400) buffer:
//
//   m_lpMem + 0            code image copied by Load (m_ReserveReg.CBR)
//   m_lpMem + CodeLen      data area base      (m_ReserveReg.DP)
//   m_ReserveReg.DP + 0x8000..0xffff   VM stack used by push/pop
//
// The 16-bit absolute addresses used by data instructions are offsets from
// m_ReserveReg.DP, not from the arena start.
//
// Opcode map (94 entries, jump table at .rodata 0x240148, dispatch at
// 0x001955bc; opcode > 0x5d -> SetError(InvalidInstruction)):
//
//   01  byte  [imm16] = [imm16]                 05  word  [imm16] = reg
//   02  word  [imm16] = [imm16]                 06  dword [imm16] = reg
//   03  dword [imm16] = [imm16]                 07  byte  reg = [imm16]
//   04  byte  [imm16] = reg                     08  word  reg = [imm16]
//                                               09  dword reg = [imm16]
//   0a  reg = reg            0e  byte  [reg] = reg   12  push word
//   0b  byte  reg = [reg]    0f  word  [reg] = reg   13  push dword
//   0c  word  reg = [reg]    10  dword [reg] = reg   14  pop  byte
//   0d  dword reg = [reg]    11  push byte           15  pop  word
//                                                    16  pop  dword
//   17  reg = imm8    1a  jmp imm16          1e  jb   reg,reg,reg
//   18  reg = imm16   1b  jmp reg            1f  jbe  reg,reg,reg
//   19  reg = imm32   1c  je  reg,reg,reg    20  ja   reg,reg,reg
//                     1d  jne reg,reg,reg    21  jae  reg,reg,reg
//   22  return TRUE   23  nop               24  loop
//
//   25  reg += reg    27  reg -= reg    29  reg *= reg    2b  div reg,reg
//   26  reg += imm32  28  reg -= imm32  2a  reg *= imm32  2c  div reg,imm32
//   2d  reg &= reg    2f  reg |= reg    31  reg ^= reg    33  not reg
//   2e  reg &= imm32  30  reg |= imm32  32  reg ^= imm32
//   34  rol reg,imm8  36  ror reg,imm8  38  shl reg,imm8  3a  shr reg,imm8
//   35  rol reg,reg4  37  ror reg,reg4  39  shl reg,reg4  3b  shr reg,reg4
//
//   3c  call imm16    3d  ret
//   3e  CallDecrypt(imm16, DP, reg2, DP+reg3, reg4, true)
//   3f  CallDecrypt(reg[imm8], ...)
//   40  reg = *(u32*)imm32       42  *(u32*)imm32 = reg
//   41  reg = *(u32*)reg         43  *(u32*)reg = reg
//   44  xora(DP, reg2, reg)      46  ora(DP, reg2, reg)
//   45  anda(DP, reg2, reg)      47  nota(DP, reg2)
//   48  reg6 = KLP[imm8]         4b  reg6 = KMP[reg[imm8]]
//   49  reg6 = KLP[reg[imm8]]    4c  reg6 = KTP[imm8]
//   4a  reg6 = KMP[imm8]         4d  reg6 = KTP[reg[imm8]]
//   4e  KLP[reg5] = reg6  4f  KMP[reg5] = reg6  50  KTP[reg5] = reg6
//   5c  CallCryptCode(imm16, DP, reg2, DP+reg3, reg4, reg5)
//   5d  CallCryptCode(reg[imm8], ...)
//
// Every failing data instruction sets RunTimeError (5); a missing key table
// sets KeyLinkError/KeyMatrixError/KeyTreeError (6/7/8) before the bounds
// test; the encrypt/decrypt wrapper calls set BinCodeError (11) when the
// callee returns FALSE, and an escaping exception is reported as
// RunTimeError.  Conditional jumps also require regA <= CL (an original
// quirk) and, like JMP/LD, accept only targets <= CL.  The original shares a
// small number of out-of-line failure tails between handlers; the labels at
// the bottom of Run reproduce that layout.
#include "src/VMCore/vm.h"

#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// construction / destruction
// ---------------------------------------------------------------------------
// ORIG 0x00196dc0 (C1) / 0x00196f00 (C2)
CVM::CVM()
{
    m_iErrCode = 0;

    UINT32 i;
    for (i = 0; i < 13; ++i) {
        m_Reg[i] = 0;
    }
    m_ReserveReg.CBR = 0;
    m_ReserveReg.DP = 0;
    m_ReserveReg.KMP = 0;
    m_ReserveReg.KLP = 0;
    m_ReserveReg.KTP = 0;
    m_ReserveReg.CL = 0;

    m_pLink = 0;
    m_pMatrix = 0;
    m_pTree = 0;
    m_MatrixX = 0;
    m_MatrixY = 0;
    m_dwTreeLength = 0;
    m_dwLinkLength = 0;
    m_pCodeMgr = 0;

    m_lpMem = malloc(0x10400);
    if (m_lpMem == 0) {
        SetError(MallocMemoryErr);
    }
}

// ORIG 0x00196cd0 (D0), 0x00196d20 (D1), 0x00196d70 (D2)
CVM::~CVM()
{
    if (m_lpMem != 0) {
        free(m_lpMem);
        m_lpMem = 0;
    }
}

// ---------------------------------------------------------------------------
// error state
// ---------------------------------------------------------------------------
// ORIG 0x00195180
int CVM::GetLastError()
{
    return m_iErrCode;
}

// ORIG 0x00195190
void CVM::SetError(int nErrorCode)
{
    m_iErrCode = nErrorCode;
}

// ---------------------------------------------------------------------------
// key tables
// ---------------------------------------------------------------------------
// ORIG 0x001951a0
BOOL CVM::SetKeyLink(PVOID pLink, UINT32 dwSize)
{
    if (pLink != 0) {
        m_pLink = pLink;
        m_dwLinkLength = dwSize;
        SetError(NoError);
        return TRUE;
    }
    SetError(NullPointer);
    return FALSE;
}

// ORIG 0x001951f0
BOOL CVM::SetKeyMatrix(PVOID pMatrix, UINT32 X, UINT32 Y)
{
    if (pMatrix != 0) {
        m_pMatrix = pMatrix;
        m_MatrixX = X;
        m_MatrixY = Y;
        SetError(NoError);
        return TRUE;
    }
    SetError(NullPointer);
    return FALSE;
}

// ORIG 0x00195240
BOOL CVM::SetKeyTree(PVOID pTree, UINT32 dwSize)
{
    if (pTree != 0) {
        m_pTree = pTree;
        m_dwTreeLength = dwSize;
        SetError(NoError);
        return TRUE;
    }
    SetError(NullPointer);
    return FALSE;
}

// ORIG 0x00195290
void CVM::GetDataBuffer(PBYTE &pData, UINT32 &dwLen)
{
    pData = m_ReserveReg.DP;
    dwLen = m_Reg[2];
}

// ---------------------------------------------------------------------------
// lifecycle
// ---------------------------------------------------------------------------
// ORIG 0x001952b0: calls vtable slot +0x3c, i.e. the deleting destructor.
void CVM::Release()
{
    if (this != 0) {
        delete this;
    }
}

// ORIG 0x001952d0.  One function-local CServerCryptCodeMgr per translation
// unit; the original guard variable and __tcf_0 destructor registration are
// reproduced by the compiler for this static.
BOOL CVM::Init()
{
    static CServerCryptCodeMgr mgr;
    m_pCodeMgr = &mgr;
    return TRUE;
}

// ---------------------------------------------------------------------------
// data header (big-endian on the wire)
// ---------------------------------------------------------------------------
// ORIG 0x001953a0
void CVM::GetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                    PWORD pwDataLen)
{
    if (pbyHeader == 0) {
        return;
    }
    if (pwLen != 0) {
        WORD v = *(WORD *)pbyHeader;
        *pwLen = (WORD)((v >> 8) | (v << 8));
    }
    if (pwCodeLen != 0) {
        WORD v = *(WORD *)(pbyHeader + 2);
        *pwCodeLen = (WORD)((v >> 8) | (v << 8));
    }
    if (pwDataLen != 0) {
        WORD v = *(WORD *)(pbyHeader + 4);
        *pwDataLen = (WORD)((v >> 8) | (v << 8));
    }
}

// ORIG 0x00195520
void CVM::SetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                    PWORD pwDataLen)
{
    if (pbyHeader == 0) {
        return;
    }
    if (pwLen != 0) {
        WORD v = *pwLen;
        *(WORD *)pbyHeader = (WORD)((v >> 8) | (v << 8));
    }
    if (pwCodeLen != 0) {
        WORD v = *pwCodeLen;
        *(WORD *)(pbyHeader + 2) = (WORD)((v >> 8) | (v << 8));
    }
    if (pwDataLen != 0) {
        WORD v = *pwDataLen;
        *(WORD *)(pbyHeader + 4) = (WORD)((v >> 8) | (v << 8));
    }
}

// ORIG 0x00195400.
// The code image is a 6-byte header at +0, a blob at +0xb whose own 6-byte
// header is at +0xb and whose payload starts at +0x11; the decode program is
// the header that follows that payload.
BOOL CVM::GetDecodeCode(BYTE *pbyCodeBuff, PBYTE &pbyDecodeCode,
                        WORD &wCodeLen)
{
    if (pbyCodeBuff == 0) {
        return FALSE;
    }
    WORD wLen = 0;
    GetHeader(pbyCodeBuff + 0xb, 0, &wLen, 0);
    pbyDecodeCode = pbyCodeBuff + 0x11 + wLen;
    GetHeader(pbyDecodeCode, 0, &wLen, 0);
    wCodeLen = (WORD)(wLen + 6);
    return TRUE;
}

// ORIG 0x001954b0
BOOL CVM::GetEncodeCode(BYTE *pbyCodeBuff, PBYTE &pbyEncodeCode,
                        WORD &wCodeLen)
{
    if (pbyCodeBuff == 0) {
        return FALSE;
    }
    pbyEncodeCode = pbyCodeBuff + 0xb;
    WORD wLen = 0;
    GetHeader(pbyEncodeCode, 0, &wLen, 0);
    wCodeLen = (WORD)(wLen + 6);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Load (ORIG 0x00196b40)
// ---------------------------------------------------------------------------
BOOL CVM::Load(PVOID pCodeBuffer, SIZE_T CodeBufferSize, PVOID pData,
               SIZE_T DataSize)
{
    if (pCodeBuffer == 0 || pData == 0) {
        SetError(NullPointer);
        return FALSE;
    }

    m_iErrCode = 0;
    UINT32 i;
    for (i = 0; i < 13; ++i) {
        m_Reg[i] = 0;
    }
    m_ReserveReg.CBR = 0;
    m_ReserveReg.DP = 0;
    m_ReserveReg.KMP = 0;
    m_ReserveReg.KLP = 0;
    m_ReserveReg.KTP = 0;
    m_ReserveReg.CL = 0;

    DATAHEADER stDataHeader;
    char *pbTmp = (char *)pCodeBuffer;
    DecodeShort(&pbTmp, &stDataHeader.Len);
    DecodeShort(&pbTmp, &stDataHeader.CodeLen);
    DecodeShort(&pbTmp, &stDataHeader.DataLen);

    // Unsigned comparison on (CodeBufferSize - 6); a size below 6 wraps and
    // therefore passes here, exactly like the original.
    if (stDataHeader.CodeLen > CodeBufferSize - 6) {
        return FALSE;
    }

    memcpy(m_lpMem, (char *)pCodeBuffer + 6, stDataHeader.CodeLen);
    memcpy((char *)m_lpMem + stDataHeader.CodeLen, pData, DataSize);
    m_ReserveReg.CBR = (PBYTE)m_lpMem;

    // The header words are checked as signed 16-bit values, after the copies.
    if ((short)stDataHeader.CodeLen < 0 || (short)stDataHeader.DataLen < 0) {
        SetError(InvalidHeader);
        return FALSE;
    }

    m_ReserveReg.CL = stDataHeader.CodeLen;
    m_Reg[1] = 0x8000;
    m_ReserveReg.DP = (PBYTE)m_lpMem + stDataHeader.CodeLen;
    m_Reg[2] = (UINT32)DataSize;
    m_ReserveReg.KLP = m_pLink;
    m_ReserveReg.KMP = m_pMatrix;
    m_ReserveReg.KTP = m_pTree;
    return TRUE;
}

// ---------------------------------------------------------------------------
// Run (ORIG 0x00195570 .. 0x00196b3d)
// ---------------------------------------------------------------------------
BOOL CVM::Run()
{
    PBYTE pb = m_ReserveReg.DP;
    const UINT32 dwCodeLen = m_ReserveReg.CL;

    if (dwCodeLen == 0) {
        return TRUE;
    }

    for (;;) {
        const UINT32 pc = m_Reg[0];
        if (pc > dwCodeLen) {
            return FALSE;
        }
        PBYTE pCode = m_ReserveReg.CBR + pc;

        switch (pCode[0]) {
        // ---- copies/loads/stores -------------------------------------
        case 0x01: { // byte [imm16] = [imm16]
            if (dwCodeLen < pc + 5) goto runtime_error;
            WORD dst = *(WORD *)(pCode + 1);
            WORD src = *(WORD *)(pCode + 3);
            pb[dst] = pb[src];
            m_Reg[0] = pc + 5;
            break;
        }
        case 0x02: { // word [imm16] = [imm16]
            if (dwCodeLen < pc + 5) goto runtime_error;
            WORD dst = *(WORD *)(pCode + 1);
            if (dst == 0xffff) goto runtime_error;
            WORD src = *(WORD *)(pCode + 3);
            if (src == 0xffff) goto runtime_error;
            *(WORD *)(pb + dst) = *(WORD *)(pb + src);
            m_Reg[0] = pc + 5;
            break;
        }
        case 0x03: { // dword [imm16] = [imm16]
            if (dwCodeLen < pc + 5) goto runtime_error;
            WORD dst = *(WORD *)(pCode + 1);
            if (dst > 0xfffc) goto runtime_error;
            WORD src = *(WORD *)(pCode + 3);
            if (src > 0xfffc) goto runtime_error;
            *(UINT32 *)(pb + dst) = *(UINT32 *)(pb + src);
            m_Reg[0] = pc + 5;
            break;
        }
        case 0x04: { // byte [imm16] = reg
            if (dwCodeLen < pc + 4) goto runtime_error;
            BYTE r = pCode[3];
            if (r > 0xc) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 1);
            pb[addr] = (BYTE)m_Reg[r];
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x05: { // word [imm16] = reg
            if (dwCodeLen < pc + 4) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 1);
            if (addr == 0xffff) goto runtime_error;
            BYTE r = pCode[3];
            if (r > 0xc) goto runtime_error;
            *(WORD *)(pb + addr) = (WORD)m_Reg[r];
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x06: { // dword [imm16] = reg
            if (dwCodeLen < pc + 4) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 1);
            if (addr > 0xfffc) goto runtime_error;
            BYTE r = pCode[3];
            if (r > 0xc) goto runtime_error;
            *(UINT32 *)(pb + addr) = m_Reg[r];
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x07: { // byte reg = [imm16]
            if (dwCodeLen < pc + 4) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 2);
            m_Reg[r] = pb[addr];
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x08: { // word reg = [imm16]
            if (dwCodeLen < pc + 4) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 2);
            if (addr == 0xffff) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = *(WORD *)(pb + addr);
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x09: { // dword reg = [imm16]
            if (dwCodeLen < pc + 4) goto runtime_error;
            WORD addr = *(WORD *)(pCode + 2);
            if (addr > 0xfffc) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = *(UINT32 *)(pb + addr);
            m_Reg[0] = pc + 4;
            break;
        }

        // ---- register/indirect moves ---------------------------------
        case 0x0a: { // reg = reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] = m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x0b: { // byte reg = [reg]
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[s];
            if (addr > 0xffff) goto runtime_error;
            m_Reg[d] = pb[addr];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x0c: { // word reg = [reg]
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[s];
            if (addr > 0xfffe) goto runtime_error;
            m_Reg[d] = *(WORD *)(pb + addr);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x0d: { // dword reg = [reg]
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[s];
            if (addr > 0xfffc) goto runtime_error;
            m_Reg[d] = *(UINT32 *)(pb + addr);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x0e: { // byte [reg] = reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[d];
            if (addr > 0xffff) goto runtime_error;
            pb[addr] = (BYTE)m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x0f: { // word [reg] = reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[d];
            if (addr > 0xfffe) goto runtime_error;
            *(WORD *)(pb + addr) = (WORD)m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x10: { // dword [reg] = reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            UINT32 addr = m_Reg[d];
            if (addr > 0xfffc) goto runtime_error;
            *(UINT32 *)(pb + addr) = m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }

        // ---- stack (m_Reg[1], grows up from 0x8000) ------------------
        case 0x11: { // push byte
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8000) > 0x7fff) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            pb[sp] = (BYTE)m_Reg[r];
            m_Reg[1] = sp + 1;
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x12: { // push word
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8000) > 0x7ffe) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            *(WORD *)(pb + sp) = (WORD)m_Reg[r];
            m_Reg[1] = sp + 2;
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x13: { // push dword
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8000) > 0x7ffc) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            *(UINT32 *)(pb + sp) = m_Reg[r];
            m_Reg[1] = sp + 4;
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x14: { // pop byte
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8001) > 0x7ffe) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            m_Reg[1] = sp - 1;
            m_Reg[r] = pb[sp - 1];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x15: { // pop word
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8002) > 0x7ffd) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            m_Reg[1] = sp - 2;
            m_Reg[r] = *(WORD *)(pb + sp - 2);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x16: { // pop dword
            UINT32 sp = m_Reg[1];
            if ((UINT32)(sp - 0x8004) > 0x7ffb) goto runtime_error;
            if (dwCodeLen < pc + 1) goto runtime_error;
            BYTE r = pCode[1];
            if ((int)r > 0xc) goto runtime_error;
            m_Reg[1] = sp - 4;
            m_Reg[r] = *(UINT32 *)(pb + sp - 4);
            m_Reg[0] = pc + 2;
            break;
        }

        // ---- immediates ----------------------------------------------
        case 0x17: { // reg = imm8
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = pCode[2];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x18: { // reg = imm16
            if (dwCodeLen < pc + 4) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = *(WORD *)(pCode + 2);
            m_Reg[0] = pc + 4;
            break;
        }
        case 0x19: { // reg = imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }

        // ---- control flow --------------------------------------------
        case 0x1a: { // jmp imm16
            if (dwCodeLen < pc + 3) goto runtime_error;
            UINT32 target = *(WORD *)(pCode + 1);
            if (dwCodeLen < target) goto runtime_error;
            m_Reg[0] = target;
            break;
        }
        case 0x1b: { // jmp reg
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            UINT32 target = m_Reg[r];
            if (dwCodeLen < target) goto runtime_error;
            m_Reg[0] = target;
            break;
        }
        case 0x1c:   // je  regA, regB, regTarget
        case 0x1d:   // jne
        case 0x1e:   // jb
        case 0x1f:   // jbe
        case 0x20:   // ja
        case 0x21: { // jae
            if (dwCodeLen < pc + 4) goto runtime_error;
            BYTE a = pCode[1];
            if (a > 0xc) goto runtime_error;
            UINT32 va = m_Reg[a];
            // Original quirk: the *value* of regA is bounds-checked against CL.
            if (dwCodeLen < va) goto runtime_error;
            UINT32 vb = m_Reg[pCode[2]];
            const BYTE op = pCode[0];
            BOOL take;
            if (op == 0x1c) {
                take = (va == vb);
            } else if (op == 0x1d) {
                take = (va != vb);
            } else if (op == 0x1e) {
                take = (va < vb);
            } else if (op == 0x1f) {
                take = (va <= vb);
            } else if (op == 0x20) {
                take = (va > vb);
            } else {
                take = (va >= vb);
            }
            m_Reg[0] = take ? m_Reg[pCode[3]] : pc + 4;
            break;
        }
        case 0x22: // halt
            return TRUE;
        case 0x23: // nop
            m_Reg[0] = pc + 1;
            break;
        case 0x24: { // loop imm16: decrement reg3, branch while non-zero
            if (dwCodeLen < pc + 3) goto runtime_error;
            if (m_Reg[3] == 0) {
                m_Reg[0] = pc + 3;
            } else {
                --m_Reg[3];
                m_Reg[0] = *(WORD *)(pCode + 1);
            }
            break;
        }

        // ---- arithmetic ----------------------------------------------
        case 0x25: { // reg += reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] += m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x26: { // reg += imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] += *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x27: { // reg -= reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] -= m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x28: { // reg -= imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] -= *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x29: { // reg *= reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] *= m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x2a: { // reg *= imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] *= *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x2b: { // reg3 = regA / regB, reg4 = regA % regB
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE a = pCode[1];
            if (a > 0xc) goto runtime_error;
            BYTE b = pCode[2];
            if (b > 0xc) goto runtime_error;
            UINT32 x = m_Reg[a];
            m_Reg[3] = x / m_Reg[b];
            m_Reg[4] = x % m_Reg[b];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x2c: { // reg3 = regA / imm32, reg4 = regA % imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE a = pCode[1];
            if (a > 0xc) goto runtime_error;
            UINT32 x = m_Reg[a];
            UINT32 d = *(UINT32 *)(pCode + 2);
            m_Reg[3] = x / d;
            m_Reg[4] = x % d;
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x2d: { // reg &= reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] &= m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x2e: { // reg &= imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] &= *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x2f: { // reg |= reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] |= m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x30: { // reg |= imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] |= *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x31: { // reg ^= reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] ^= m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x32: { // reg ^= imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            m_Reg[d] ^= *(UINT32 *)(pCode + 2);
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x33: { // not reg
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = ~m_Reg[r];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x34: { // rol reg, imm8
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            rol((unsigned char *)&m_Reg[r], pCode[2]);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x35: { // rol reg, reg4
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            rol((unsigned char *)&m_Reg[r], (int)m_Reg[4]);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x36: { // ror reg, imm8
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            ror((unsigned char *)&m_Reg[r], pCode[2]);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x37: { // ror reg, reg4
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            ror((unsigned char *)&m_Reg[r], (int)m_Reg[4]);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x38: { // shl reg, imm8
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] <<= (pCode[2] & 0x1f);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x39: { // shl reg, reg4
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] <<= (m_Reg[4] & 0x1f);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x3a: { // shr reg, imm8
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] >>= (pCode[2] & 0x1f);
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x3b: { // shr reg, reg4
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] >>= (m_Reg[4] & 0x1f);
            m_Reg[0] = pc + 2;
            break;
        }

        // ---- subroutines ---------------------------------------------
        case 0x3c: { // call imm16
            if (dwCodeLen < pc + 3) goto runtime_error;
            UINT32 sp = m_Reg[1];
            *(WORD *)(pb + sp) = (WORD)(pc + 3);
            m_Reg[1] = sp + 2;
            m_Reg[0] = *(WORD *)(pCode + 1);
            break;
        }
        case 0x3d: { // ret
            if (dwCodeLen < pc + 1) goto runtime_error;
            UINT32 sp = m_Reg[1];
            UINT32 target = *(WORD *)(pb + sp - 2);
            if (dwCodeLen < target) goto runtime_error;
            m_Reg[1] = sp - 2;
            m_Reg[0] = target;
            break;
        }

        // ---- crypto call-outs ----------------------------------------
        case 0x3e: { // CallDecrypt(imm16, DP, reg2, DP+reg3, reg4, true)
            BOOL ok;
            try {
                ok = CallDecrypt((int)*(WORD *)(pCode + 1), pb, m_Reg[2],
                                 pb + m_Reg[3], m_Reg[4], true);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x3f: { // CallDecrypt(reg[imm8], DP, reg2, DP+reg3, reg4, true)
            BOOL ok;
            try {
                ok = CallDecrypt((int)m_Reg[pCode[1]], pb, m_Reg[2],
                                 pb + m_Reg[3], m_Reg[4], true);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 2;
            break;
        }

        // ---- host memory access (deliberately native) ----------------
        case 0x40: { // reg = *(u32*)imm32
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            m_Reg[r] = *(UINT32 *)(*(UINT32 *)(pCode + 2));
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x41: { // reg = *(u32*)reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            m_Reg[d] = *(UINT32 *)m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x42: { // *(u32*)imm32 = reg
            if (dwCodeLen < pc + 6) goto runtime_error;
            BYTE r = pCode[5];
            if (r > 0xc) goto runtime_error;
            *(UINT32 *)(*(UINT32 *)(pCode + 1)) = m_Reg[r];
            m_Reg[0] = pc + 6;
            break;
        }
        case 0x43: { // *(u32*)reg = reg
            if (dwCodeLen < pc + 3) goto runtime_error;
            BYTE d = pCode[1];
            if (d > 0xc) goto runtime_error;
            BYTE s = pCode[2];
            if (s > 0xc) goto runtime_error;
            *(UINT32 *)m_Reg[d] = m_Reg[s];
            m_Reg[0] = pc + 3;
            break;
        }

        // ---- bulk transforms on the data area ------------------------
        case 0x44: { // xora(DP, reg2, reg)
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            xora(pb, (int)m_Reg[2], m_Reg[r]);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x45: { // anda(DP, reg2, reg)
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            anda(pb, (int)m_Reg[2], m_Reg[r]);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x46: { // ora(DP, reg2, reg)
            if (dwCodeLen < pc + 2) goto runtime_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            ora(pb, (int)m_Reg[2], m_Reg[r]);
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x47: { // nota(DP, reg2)
            if (dwCodeLen < pc + 1) goto runtime_error;
            nota(pb, (int)m_Reg[2]);
            m_Reg[0] = pc + 1;
            break;
        }

        // ---- key tables ----------------------------------------------
        // These handlers do not validate the instruction length, exactly
        // like the original (the opcode dispatch already guarantees pc <= CL).
        case 0x48: { // reg6 = KLP[imm8]
            if (m_pLink == 0) goto keylink_error;
            UINT32 idx = pCode[1];
            if (idx > m_dwLinkLength - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KLP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x49: { // reg6 = KLP[reg[imm8]]
            if (m_pLink == 0) goto keylink_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            UINT32 idx = m_Reg[r];
            if (idx > m_dwLinkLength - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KLP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x4a: { // reg6 = KMP[imm8]
            if (m_pMatrix == 0) goto keymatrix_error;
            UINT32 idx = pCode[1];
            if (idx > m_MatrixX * m_MatrixY - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KMP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x4b: { // reg6 = KMP[reg[imm8]]
            if (m_pMatrix == 0) goto keymatrix_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            UINT32 idx = m_Reg[r];
            if (idx > m_MatrixX * m_MatrixY - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KMP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x4c: { // reg6 = KTP[imm8]
            if (m_pTree == 0) goto keytree_error;
            UINT32 idx = pCode[1];
            if (idx > m_dwTreeLength - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KTP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x4d: { // reg6 = KTP[reg[imm8]]
            if (m_pTree == 0) goto keytree_error;
            BYTE r = pCode[1];
            if (r > 0xc) goto runtime_error;
            UINT32 idx = m_Reg[r];
            if (idx > m_dwTreeLength - 1) goto runtime_error;
            m_Reg[6] = ((UINT32 *)m_ReserveReg.KTP)[idx];
            m_Reg[0] = pc + 2;
            break;
        }
        case 0x4e: { // KLP[reg5] = reg6
            if (m_pLink == 0) goto keylink_error;
            UINT32 idx = m_Reg[5];
            if (idx > m_dwLinkLength - 1) goto runtime_error;
            ((UINT32 *)m_ReserveReg.KLP)[idx] = m_Reg[6];
            m_Reg[0] = pc + 1;
            break;
        }
        case 0x4f: { // KMP[reg5] = reg6
            if (m_pMatrix == 0) goto keymatrix_error;
            UINT32 idx = m_Reg[5];
            if (idx > m_MatrixX * m_MatrixY - 1) goto runtime_error;
            ((UINT32 *)m_ReserveReg.KMP)[idx] = m_Reg[6];
            m_Reg[0] = pc + 1;
            break;
        }
        case 0x50: { // KTP[reg5] = reg6
            if (m_pTree == 0) goto keytree_error;
            UINT32 idx = m_Reg[5];
            if (idx > m_dwTreeLength - 1) goto runtime_error;
            ((UINT32 *)m_ReserveReg.KTP)[idx] = m_Reg[6];
            m_Reg[0] = pc + 1;
            break;
        }

        // ---- block crypto --------------------------------------------
        case 0x5c: { // CallCryptCode(imm16, DP, reg2, DP+reg3, reg4, reg5)
            if (m_pCodeMgr == 0) goto runtime_error;
            BOOL ok;
            try {
                ok = m_pCodeMgr->CallCryptCode(
                    *(WORD *)(pCode + 1), pb, (WORD)m_Reg[2],
                    pb + m_Reg[3], (WORD)m_Reg[4], (BOOL)m_Reg[5]);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x5d: { // CallCryptCode(reg[imm8], DP, reg2, DP+reg3, reg4, reg5)
            if (m_pCodeMgr == 0) goto runtime_error;
            BOOL ok;
            try {
                ok = m_pCodeMgr->CallCryptCode(
                    (WORD)m_Reg[pCode[1]], pb, (WORD)m_Reg[2],
                    pb + m_Reg[3], (WORD)m_Reg[4], (BOOL)m_Reg[5]);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 2;
            break;
        }

        default: // 0x00, 0x51..0x5b and (via the pre-check) > 0x5d
            goto invalid_instruction;
        }
    }

invalid_instruction:
    SetError(InvalidInstruction);
    return FALSE;

runtime_error:
    SetError(RunTimeError);
    return FALSE;

keylink_error:
    SetError(KeyLinkError);
    return FALSE;

keymatrix_error:
    SetError(KeyMatrixError);
    return FALSE;

keytree_error:
    SetError(KeyTreeError);
    return FALSE;

bincode_error:
    SetError(BinCodeError);
    return FALSE;
}

// ---------------------------------------------------------------------------
// factory (ORIG 0x00196ea0)
// ---------------------------------------------------------------------------
CVMBase *GetVM()
{
    return new CVM();
}
