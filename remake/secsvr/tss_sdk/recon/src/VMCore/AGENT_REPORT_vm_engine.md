# AGENT_REPORT_vm_engine

Reconstruction of the anti-cheat VM core in `libtss_sdk.so.1.0.394`
(ELF32 i386, unstripped DWARF, vendor GCC 4.1.2):

| CU | reconstructed file | functions |
|---|---|---|
| `src/VMCore/VM.cpp` | `recon/src/VMCore/vm.cpp`, `recon/src/VMCore/vm.h` | CVM class (21 emitted symbols) |
| `src/VMCore/UPVM.cpp` | `recon/src/VMCore/upvm.cpp` | CUPVM class (10 emitted symbols) |
| `src/VMCore/ServerCryptCodeMgr.cpp` | `recon/src/VMCore/server_crypt_code_mgr.cpp` | 6 emitted symbols |

`recon/src/VMCore/cryptdata.cpp` and its header were **not** modified.
The CommonProc and codeengine helper declarations are taken from the canonical
headers owned by the `VmCodec` unit (`recon/src/VMCore/common_proc.h`,
`recon/src/VMCore/codeengine.h`); `vm.h` only declares the VM itself.

## 1. Evidence summary

| Check | Command | Result |
|---|---|---|
| build | `sh c5build.sh recon/src/VMCore/vm.cpp build/tmp/vm_engine/vm.o` | rc=0 |
| build | `sh c5build.sh recon/src/VMCore/upvm.cpp build/tmp/vm_engine/upvm.o` | rc=0 |
| build | `sh c5build.sh recon/src/VMCore/server_crypt_code_mgr.cpp build/tmp/vm_engine/server_crypt_code_mgr.o` | rc=0 |
| symbols | `nm -C --defined-only -D ORIG` vs `nm -C obj` (regex `CVM::`, `GetVM`, `CVM` vtable/typeinfo) | 0 ORIG-only; the only new names are the two function-local statics (`CVM::Init()::mgr`, `CUPVM::Init()::s_objMgr`) and their `__cxa_guard` variables, which also exist in the ORIG full symbol table |
| sizes | `nm -S` ORIG vs object, per mangled symbol (see `vm_engine_matrix.tsv`) | 41/49 symbols byte-size identical; all 8 remaining are 0.870..1.005 of the original (limit 0.85..1.15) |
| vtables | `readelf -rW` on `.rel.data.rel.ro._ZTV3CVM` / `_ZTV5CUPVM` | slot order identical to ORIG relocations; CUPVM overrides exactly Init/Release/Run |
| semantics | `build/tmp/vm_engine/vm_diff` (differential oracle, below) | **10,295 cases, 0 mismatches** |

Size matrix (full detail in `vm_engine_matrix.tsv`): the only non-identical
code sizes are `GetDecodeCode` 140/161, `GetEncodeCode` 92/103,
`CVM::Run` 5435/5581, `CVM::Load` 381/395, CVM ctors 211/210,
`CUPVM::Run` 3932/4156.  All are inside ±15%.  The shorter `Run` bodies are
the result of sharing the out-of-line failure tails; the remaining delta is
GCC 4.1 basic-block scheduling (the original's switch table lives in
`.rodata`, ours is emitted in `.text` by the same compiler).

## 2. Method

1. `cu_symbols.py`, `dwarf_full_extract.py`/`dump_cu` over the three CUs gave
   the exact function lists, signatures, pruned/enum/struct layouts and local
   variable names.
2. Each function was reconstructed from `objdump -d -Mintel` at the addresses
   below and cross-checked against the caller/callee conventions.
3. The 94-entry opcode jump tables were decoded from `.rodata`:
   CVM at `0x240148` (dispatch `0x1955bc`), CUPVM at `0x23ffc8` (dispatch
   `0x19400e`).  Every handler's length check, operand order, bounds constant,
   error code and pc advance was read individually.
4. A differential oracle (`recon/test_vm_engine.cpp`) `dlopen`s the ORIG,
   calls its exported CVM/CUPVM methods through `dlsym`, and compares against
   the reconstructed classes linked into the same process.  CommonProc /
   codeengine / CCryptData leaves are forwarded to the ORIG so that only the
   units under test can differ.

## 3. Object model (DWARF, confirmed by every access in disassembly)

```
CVMBase (8)    vptr +0, unnamed-enum VMError +4  (never read/written in these CUs)
CVM (124)      +0x08 m_iErrCode        +0x0c m_pCodeMgr
               +0x10 m_dwTreeLength    +0x14 m_dwLinkLength
               +0x18 m_pTree           +0x1c m_pMatrix      +0x20 m_pLink
               +0x24 m_MatrixX         +0x28 m_MatrixY
               +0x2c m_Reg[13]         (reg0 = PC, reg1 = SP, reg2 = data length,
                                        reg3 = loop counter / quotient,
                                        reg4 = remainder / shift count,
                                        reg5/6 = key index/value)
               +0x60 RESERVEREGSTERS {CBR, DP, KMP, KLP, KTP, CL}
               +0x78 m_lpMem
CUPVM (124)    identical layout, overrides Init/Release/Run only
CServerCryptCodeMgr (4)  vptr only
```

Arena: `m_lpMem = malloc(0x10400)`; `Load` copies the code image to
`m_lpMem[0..CodeLen)`, sets `CBR = m_lpMem`, `DP = m_lpMem + CodeLen` and
`m_Reg[1] = 0x8000`.  All bytecode data addresses are 16-bit offsets from
`DP`; the VM stack grows up from `DP+0x8000`.

## 4. CVM (`vm.cpp`)

| Address | Function | Notes |
|---|---|---|
| 0x195180/90 | `GetLastError`/`SetError` | `m_iErrCode` |
| 0x1951a0/f0/240 | `SetKeyLink/Matrix/Tree` | store pointer + count; `SetError(NoError)`; NULL → `NullPointer(3)`, FALSE |
| 0x195290 | `GetDataBuffer` | `*pData = m_ReserveReg.DP`, `*dwLen = m_Reg[2]` |
| 0x1952b0 | `Release` | `if (this) delete this;` → vtable slot +0x3c (deleting dtor) |
| 0x1952d0 | `Init` | function-local `static CServerCryptCodeMgr mgr`; `m_pCodeMgr = &mgr`; TRUE |
| 0x1953a0/520 | `GetHeader`/`SetHeader` | three big-endian 16-bit fields at +0/+2/+4; NULL header/out-params accepted |
| 0x195400 | `GetDecodeCode` | `GetHeader(buf+0xb,NULL,&wLen,NULL)`; `*pbyDecodeCode = buf + 0x11 + wLen`; `GetHeader(*pbyDecodeCode,...)`; `wCodeLen = wLen + 6` |
| 0x1954b0 | `GetEncodeCode` | `*pbyEncodeCode = buf + 0xb`; `wCodeLen = innerCodeLen + 6` |
| 0x195520 | `SetHeader` | inverse of `GetHeader` |
| 0x195570 | `Run` | interpreter, table below |
| 0x196b40 | `Load` | see quirks |
| 0x196cd0/d20/d70 | `~CVM` D0/D1/D2 | `if (m_lpMem) { free; m_lpMem = 0; }` |
| 0x196dc0/6f00 | `CVM` C1/C2 | zero state, `malloc(0x10400)`, `SetError(1)` on failure |
| 0x196ea0 | `GetVM` | `return new CVM();` (returns `CVMBase*`) |

`Load` order, exactly as the original: NULL args → error 3; clear error, all 13
registers and the six reserve registers; `DecodeShort` ×3 (Len, CodeLen,
DataLen — Len unused); unsigned `CodeLen > CodeBufferSize - 6` check (a size
below 6 wraps); copy code and `DataSize` bytes of data; set `CBR`; *then*
reject negative `(short)CodeLen`/`DataLen` with error 4; set `CL`, `SP=0x8000`,
`DP`, `reg2=DataSize`, `KLP/KMP/KTP` from the key pointers.

### CVM instruction set (94 entries; opcode > 0x5d or table-default → error 2)

Length checks are against `CL`; every operand read uses the dispatch-time
`CBR+pc`.  `IMM16`/`IMM32` are little-endian.  Register numbers > 0x0c → error 5.
Data addresses: byte ≤ 0xffff, word ≠ 0xffff, dword ≤ 0xfffc.
Stack: push byte/word/dword require `SP` in `[0x8000,0xffff|0xfffe|0xfffc]`,
pop byte/word/dword in `[0x8001|0x8002|0x8004, 0xffff]`.

```
01 byte [imm16]=[imm16]      05 word [imm16]=reg       09 dword reg=[imm16]    0d dword reg=[reg]
02 word [imm16]=[imm16]      06 dword[imm16]=reg       0a reg=reg              0e byte  [reg]=reg
03 dword[imm16]=[imm16]      07 byte reg=[imm16]       0b byte reg=[reg]       0f word  [reg]=reg
04 byte [imm16]=reg          08 word reg=[imm16]       0c word reg=[reg]       10 dword [reg]=reg
11 push byte   12 push word   13 push dword   14 pop byte   15 pop word   16 pop dword
17 reg=imm8    18 reg=imm16   19 reg=imm32
1a jmp imm16   1b jmp reg     1c je regA,regB,regT     1d jne   1e jb   1f jbe   20 ja   21 jae
22 return TRUE 23 nop         24 loop: if (reg3) { --reg3; pc=imm16 } else pc+=3
25 reg+=reg    26 reg+=imm32  27 reg-=reg   28 reg-=imm32  29 reg*=reg  2a reg*=imm32
2b reg3=regA/regB, reg4=regA%regB          2c same with imm32 divisor
2d reg&=reg    2e reg&=imm32  2f reg|=reg   30 reg|=imm32  31 reg^=reg  32 reg^=imm32
33 not reg     34 rol reg,imm8 35 rol reg,reg4 36 ror reg,imm8 37 ror reg,reg4
38 shl imm8    39 shl reg4     3a shr imm8     3b shr reg4
3c call imm16 (pushes pc+3)    3d ret (target ≤ CL, pops word)
3e CallDecrypt(imm16, DP, reg2, DP+reg3, reg4, true)     3f same, id = reg[imm8]
40 reg=*(u32*)imm32  41 reg=*(u32*)reg  42 *(u32*)imm32=reg  43 *(u32*)reg=reg
44 xora(DP,reg2,reg)  45 anda(...)  46 ora(...)  47 nota(DP,reg2)
48 reg6=KLP[imm8]  49 reg6=KLP[reg[imm8]]  4a reg6=KMP[imm8]  4b reg6=KMP[reg[imm8]]
4c reg6=KTP[imm8]  4d reg6=KTP[reg[imm8]]  4e KLP[reg5]=reg6  4f KMP[reg5]=reg6  50 KTP[reg5]=reg6
5c CallCryptCode(imm16, DP, reg2, DP+reg3, reg4, reg5)
5d CallCryptCode(reg[imm8], ...)           (reg5 = encode flag, 16-bit fields truncated)
```

Original quirks reproduced deliberately:

* conditional jumps validate **regA's value** against `CL` (`CL < regA` → error 5)
  before comparing; regB and regT are not range-checked;
* `call` and `loop` do not bounds-check their targets; the next dispatch
  returns FALSE **without** setting an error when `pc > CL`;
* `ret` validates its popped target;
* key-table loads/stores do not validate the instruction length (the opcode
  dispatch already guarantees `pc <= CL`); the null-pointer check precedes the
  index bound and yields 6/7/8 instead of 5;
* with a zero-length key table the bound `len-1` wraps and every index passes;
* the byte/word/dword store/load opcodes check **different** limits, as listed;
* division by zero is not guarded (hardware `div` trap), and the quotient/
  remainder are always written to reg3/reg4;
* `CallDecrypt`/`CallCryptCode` failures set BinCodeError(11); a C++ exception
  escaping either call is caught and reported as RunTimeError(5);
* an invalid opcode (> 0x5d or table default 0x00/0x51..0x5b) sets error 2;
* `CL == 0` returns TRUE immediately.

Error codes used: 1 MallocMemoryErr, 2 InvalidInstruction, 3 NullPointer,
4 InvalidHeader, 5 RunTimeError, 6 KeyLinkError, 7 KeyMatrixError,
8 KeyTreeError, 11 BinCodeError.

## 5. CUPVM (`upvm.cpp`)

Same layout/arena.  `Init` uses a distinct function-local static
`CServerCryptCodeMgr s_objMgr`; `Release` is `delete this`; `GetUPVM` returns
`new CUPVM()`.  Its jump table (`.rodata 0x23ffc8`) implements:

* `0x01..0x19` — the full copy/load/store/immediate group, identical encodings
  to CVM;
* `0x22` halt, `0x23` nop, `0x25..0x3b` arithmetic/bit operations, `0x44..0x47`
  bulk transforms on `DP` — identical encodings to CVM;
* `0x3e`/`0x3f` — `CallDecrypt(..., false)`.  Both read a 16-bit algorithm id,
  but `0x3e` advances pc by 3 and `0x3f` by **2** (original quirk, verified);
* `0x5c`/`0x5d` — `CallCryptCode(..., encode = reg5)`;
* **not implemented** (→ error 2): `0x00`, `0x1a..0x21`, `0x24`, `0x3c`,
  `0x3d`, `0x40..0x43`, `0x48..0x5b`.
* Stack length checks are `CL >= pc+1` (not `pc+2`) for the 2-byte push/pop
  forms — this exact difference was found by the differential fuzz stage and
  fixed in both VMs.

## 6. CServerCryptCodeMgr (`server_crypt_code_mgr.cpp`)

`CallCryptCode(wIndex, pbyData, wDataLen, pbyKey, wKeyLen, blIsEncode)`:

```cpp
CCryptData objCryptData;
return objCryptData.EncryptData(wIndex, pbyKey, blIsEncode != 0,
                                pbyData, wDataLen);
```

`wKeyLen` is present in the ABI but not read by the original body (confirmed
by disassembly; CCryptData derives the key size from the algorithm id).  The
compiler-generated landing pads destroy `objCryptData` and resume unwinding.

## 7. Differential oracle

Source: `recon/test_vm_engine.cpp` (follows the existing `recon/test_*.cpp`
convention).  Build and run:

```bash
sh c5build.sh recon/src/VMCore/vm.cpp build/tmp/vm_engine/vm.o
sh c5build.sh recon/src/VMCore/upvm.cpp build/tmp/vm_engine/upvm.o
sh c5build.sh recon/src/VMCore/server_crypt_code_mgr.cpp build/tmp/vm_engine/server_crypt_code_mgr.o
g++ -m32 -O2 -fno-stack-protector -I recon -o build/tmp/vm_engine/vm_diff \
    recon/test_vm_engine.cpp build/tmp/vm_engine/vm.o build/tmp/vm_engine/upvm.o \
    build/tmp/vm_engine/server_crypt_code_mgr.o -ldl
./build/tmp/vm_engine/vm_diff
# PASSED: 10295 checks, 0 failures
```

Coverage of the 10,295 differential cases:

* every CVM opcode in its valid form, plus invalid opcodes `0x00`, `0x51`,
  `0x58`, `0x5b`, `0x5e`, `0xff`; every UPVM opcode plus its unimplemented
  group `0x1a/0x24/0x3c/0x40/0x48/0x51`;
* exact boundaries: word `0xffff` / `0xfffe`, dword `0xfffc` / `0xfffd`,
  byte `0xffff` / `0x10000`, register index `0x0c` / `0x0d`, stack edges
  `0x7fff/0x8000/0x8001/0x8002/0x8003/0x8004/0xfffc/0xfffd/0xfffe/0xffff`,
  key-table last-valid/last+1 indices, conditional-jump `regA == CL` vs
  `CL+1`, unchecked `loop`/`call` targets, jump target exactly at `CL`;
* `Load`: 600 random header/size combinations (including `CodeBufferSize < 6`,
  positive `CodeLen` with negative signed value, negative `DataLen`, null
  code/data, and the unsigned wrap path), comparing return value, error and
  every member;
* `GetHeader`/`SetHeader` (400 random buffers incl. null forms),
  `GetDecodeCode`/`GetEncodeCode` (400 random nested headers, pointer offsets
  and lengths), `GetDataBuffer`, `GetLastError`/`SetError`, all key setters;
* vtable layout: all 16 slots checked for the exact inherited/overridden
  pattern; slots 0,1,7,8,9,10,11,12,13 invoked through the vtable including
  `Release(nullptr)`; equality proven by vtable-relocation order
  (`.rel.data.rel.ro._ZTV3CVM` / `_ZTV5CUPVM`), which matches the ORIG order
  Init, Release, GetDecodeCode, GetEncodeCode, GetHeader, SetHeader,
  GetDataBuffer, SetKeyTree, SetKeyMatrix, SetKeyLink, Run, Load,
  GetLastError, SetError, D1, D0;
* crypto call-outs: `CallDecrypt` ids 0..0xe and oversized lengths,
  `CallCryptCode` ids 0/26/27, absent manager, and the encode flag;
* random fuzz: 10,000 terminating programs over the non-host, non-crypto,
  non-dividing instruction set with random data, registers and stack pointers.
  Each case compares Run's return value, `m_iErrCode`, all 13 registers,
  every reserve register, and the whole 0x10400-byte arena.

Two divergences were found and fixed during this work: the stack
length-check constant (`pc+1`, not `pc+2`) and a missing `SetError` sharing
structure (no semantic change, only code size).

## 8. Limitations / unreachable paths

* `MallocMemoryErr (1)` is implemented exactly (`if (m_lpMem == 0) SetError(1)`)
  but cannot be triggered without intercepting `malloc`; not exercised.
* Division by zero (`0x2b`/`0x2c`) traps in hardware in both implementations;
  the harness avoids it because it would terminate the process.  The unsigned
  `div` semantics are reproduced.
* `0x40..0x43` dereference host pointers by design; they are tested with
  controlled addresses but excluded from random fuzzing.
* The leaf helpers (`rol`, `ror`, `xora`, `anda`, `ora`, `nota`, `CallDecrypt`,
  `DecodeShort`) and `CCryptData` belong to other CUs; the oracle forwards them
  to the ORIG, so equal results isolate the three units above.  Their real
  implementations are being reconstructed in `common_proc.cpp`, `codeengine.cpp`
  and `cryptdata.cpp`; this report makes no claim about them.
* Byte identity is not claimed.  `Run` sizes are 5435/5581 (CVM) and
  3932/4156 (CUPVM); everything else is byte-identical in size except five
  functions within 13%.
