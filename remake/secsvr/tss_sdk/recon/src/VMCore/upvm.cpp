// Reconstructed from libtss_sdk.so.1.0.394
// CU: src/VMCore/UPVM.cpp
//
// CUPVM is the decoder-side twin of CVM.  It shares CVM's layout (124 bytes)
// and CVM's memory arena convention (code image at the arena start, data area
// at m_ReserveReg.DP, VM stack at DP+0x8000).  Only Run(), Init() and
// Release() are overridden; the jump table lives at .rodata 0x23ffc8 with the
// dispatch at 0x0019400e.
//
// The instruction encodings are the same as CVM for the opcodes both VMs
// implement, but UPVM deliberately lacks the branch, call/ret, host-memory
// and key-table instruction groups.  Opcodes 0x00, 0x1a..0x21, 0x24, 0x3c,
// 0x3d, 0x40..0x43 and 0x48..0x5b therefore raise InvalidInstruction.
// Its two crypto call-outs pass bIsEncode = false (CVM passes true).
//
// Failure tails are shared out-of-line, as in the original.
#include "src/VMCore/vm.h"

// ---------------------------------------------------------------------------
// lifecycle
// ---------------------------------------------------------------------------
// ORIG 0x001950a0 (C1) / 0x001950e0 (C2), defined out of line by the
// original unit so that the vtable's destructor slots have a home.
CUPVM::CUPVM()
{
}

CUPVM::~CUPVM()
{
}

// ORIG 0x00193ef0: function-local CServerCryptCodeMgr named s_objMgr.
BOOL CUPVM::Init()
{
    static CServerCryptCodeMgr s_objMgr;
    m_pCodeMgr = &s_objMgr;
    return TRUE;
}

// ORIG 0x00193ed0: vtable slot +0x3c (deleting destructor).
void CUPVM::Release()
{
    if (this != 0) {
        delete this;
    }
}

// ---------------------------------------------------------------------------
// Run (ORIG 0x00193fc0 .. 0x00194ffb)
// ---------------------------------------------------------------------------
BOOL CUPVM::Run()
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
        // ---- copies ---------------------------------------------------
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

        // ---- stores to absolute addresses ------------------------------
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

        // ---- loads from absolute addresses ----------------------------
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

        // ---- stack ----------------------------------------------------
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

        // ---- control --------------------------------------------------
        case 0x22: // halt
            return TRUE;
        case 0x23: // nop
            m_Reg[0] = pc + 1;
            break;

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

        // ---- crypto call-outs (bIsEncode = false) --------------------
        case 0x3e: { // CallDecrypt(imm16, DP, reg2, DP+reg3, reg4, false)
            BOOL ok;
            try {
                ok = CallDecrypt((int)*(WORD *)(pCode + 1), pb, m_Reg[2],
                                 pb + m_Reg[3], m_Reg[4], false);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 3;
            break;
        }
        case 0x3f: { // CallDecrypt(imm16, ...) with a 2-byte advance
            BOOL ok;
            try {
                ok = CallDecrypt((int)*(WORD *)(pCode + 1), pb, m_Reg[2],
                                 pb + m_Reg[3], m_Reg[4], false);
            } catch (...) {
                goto runtime_error;
            }
            if (!ok) goto bincode_error;
            m_Reg[0] = pc + 2;
            break;
        }

        // ---- bulk transforms -----------------------------------------
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

        default: // opcode groups UPVM does not implement
            goto invalid_instruction;
        }
    }

invalid_instruction:
    SetError(InvalidInstruction);
    return FALSE;

runtime_error:
    SetError(RunTimeError);
    return FALSE;

bincode_error:
    SetError(BinCodeError);
    return FALSE;
}

// ---------------------------------------------------------------------------
// factory (ORIG 0x00195120)
// ---------------------------------------------------------------------------
CVMBase *GetUPVM()
{
    return new CUPVM();
}
