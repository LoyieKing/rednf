# AGENT_REPORT_vm_codec

Reconstruction of two VMCore translation units of
`libtss_sdk.so.1.0.394`
(ELF32 i386, GCC 4.1.x, `-O2 -fPIC`, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`):

* `src/VMCore/CommonProc.cpp` — 18 functions, `0x00193190..0x00193ec2`
* `src/VMCore/codeengine.cpp` — 12 functions, `0x00196fe0..0x0019741d`

Written files:

| file | content |
|---|---|
| `recon/src/VMCore/common_proc.h` | key-container types (`tagKEYLINKNODE`, `tagKEYTREENODE`, `PKEYMATRIX`), `TenMd5` boundary, 18 prototypes |
| `recon/src/VMCore/common_proc.cpp` | 18 function bodies |
| `recon/src/VMCore/codeengine.h` | 12 prototypes |
| `recon/src/VMCore/codeengine.cpp` | 12 function bodies |
| `recon/src/VMCore/vm_codec_matrix.tsv` | per-symbol size matrix with status/notes |

`recon/src/VMCore/cryptdata.cpp` and its header were not touched. The types
this unit needs from `inc/common/VMDefines.h` are reproduced in
`common_proc.h`; `TenCrypt.h`
is included read-only and supplies every `Ten*` declaration except `TenMd5`,
which is declared in `common_proc.h` (ORIG `_Z6TenMd5PhjS_`, `0x001b8960`).

Integration note: `recon/src/VMCore/vm.h` (a different unit, written by a peer
after these headers) originally redefined `tagKEYLINKNODE` / `tagKEYTREENODE`
and declared `DecodeShort`. C++ rejects duplicate struct definitions, so no
translation unit may include both; the peer has since adopted these headers
(`vm.h` now includes `common_proc.h` and `codeengine.h` instead of keeping its
own copies) and reports its own 3-unit differential run clean. My files were
not changed for this.

## Verdict

All 30 DWARF-attributed symbols are emitted with the ORIG mangling and calling
convention; behaviour matches the ORIG on **76,771 differential oracle cases
with 0 mismatches**, covering every dispatcher arm, every NULL/empty guard,
both exception paths and both byte-order quirks.
28/30 sizes are inside ±15 % of the ORIG; the two that are not are caused by
measured GCC code-generation differences (see "Size deltas"). This is a
semantic reconstruction, not a claim of byte-identical machine code.

## Method

* `cu_symbols.py` for the CU symbol list, address and `DW_AT_high_pc` size.
* A private DWARF dumper (`build/tmp/vmcodec/dw.py`) resolved GCC 4.1
  CU-relative `DW_AT_type` / `DW_AT_specification` references and produced the
  exact parameter/local names, order and types used below (`rol`'s second
  parameter really is called `time`, `GetTreeData`'s cursor is `count`,
  `GenKey`'s scratch array is `BYTE byTmpRandBuf[upper_bound 9]`, etc.).
* `objdump -d` of each function, decoded against the two dispatcher jump
  tables (`Cipher` table at `0x0023ff40`, 20 entries; `CallDecrypt` table at
  `0x0023ff90`, 14 entries; both indexed from EBX `0x00294534`).
* Differential oracle (`build/tmp/vmcodec/oracle.cpp`): the two reconstructed
  objects are linked in with all 30 symbols renamed `rc_*` via
  `objcopy --redefine-sym`, the ORIG is `dlopen`ed `RTLD_GLOBAL` and the
  originals resolved with `dlsym`. Because the ORIG exports its own `Ten*`
  implementations, both sides run on the *same* crypto code, and both allocate
  through the same `operator new/delete`, so the comparison isolates the
  reconstructed logic. The harness also compares cursor advances, written-back
  out-parameters, key bytes, tree/list shape and matrix contents.

## Layouts (DWARF, asserted by the ORIG code)

```text
tagKEYLINKNODE  = next +0, key +4                     sizeof 8   (VMDefines.h:311)
tagKEYTREENODE  = left +0, right +4, key +8           sizeof 12  (VMDefines.h:317)
PKEYMATRIX      = UINT32 **                                      (VMDefines.h:327)
```

`GetLinkData` walks `+0`; `GetTreeData` recurses on `+0` then `+4`;
`InitKeyTree` allocates 12 bytes and stores `left/right/key`; `InitKeyLink`
allocates 8 bytes and stores `next/key`; `InitKeyMatrix` allocates
`y * sizeof(UINT32 *)` plus `y` rows of `x * sizeof(UINT32)`.
`InitKeyTree`'s `result` is a non-const `int &`: the ORIG mangling is `Ri`, and
all three reference parameters of this unit mangle non-const (`Ri` for
`int&`, `Rj` for `unsigned int&`, `RPh` for `unsigned char*&`; a `const`
reference carries `K`, e.g. `RKi`). DWARF nevertheless shows a
`DW_TAG_const_type` inside each `DW_TAG_reference_type`; the same artifact
appears when the vendor GCC 4.1.2 compiles a plain `int&`, so it is compiler
encoding, not source constness - and the ORIG writes through all three.

## Semantics that the disassembly pins down

`CommonProc.cpp`

* `rol/ror` — `int nShift = time % 8;` (signed remainder, so negative `time`
  yields a negative shift count exactly like the ORIG's `sar/shr/add/and/sub`
  sequence), then one 8-bit rotate of `*data`. NULL is a no-op.
* `xora/anda/ora` — the by-value `seed` is byte-indexed with `i % 4`
  (`lea 0x10(%ebp)` in the ORIG); `count <= 0` and NULL are no-ops.
* `nota` — byte-wise complement over `count` bytes.
* `GetLinkData` — NULL → NULL, `count <= 0` → head, else `count` hops. On the
  final hop the ORIG returns the loaded pointer without a NULL test, so an
  exact-length walk returns NULL rather than the last node.
* `GetTreeData` — `count == 0` returns the node; otherwise `c = count - 1` is
  threaded through the left recursion first and the right recursion only if the
  left returned NULL, and `c` is written back to `count` on every path that
  passed the first test (including failure).
* `GetMatrixData` — `pHeader`, `x`, `y` must be non-zero and
  `y * x >= count` (signed compare); returns `&pHeader[count / x][count % x]`
  or NULL when that row pointer is NULL. `count == x*y` is accepted and indexes
  one row past the end, exactly like the ORIG.
* `GenKey` — allocates 32 bytes, zeroes a 10-byte stack scratch buffer
  (`memset`), then two rounds of
  `TenMd5(buf, 8, out + 16*i)` with `buf = { dwKeySeed, nType + i }` in native
  little-endian order; returns `TRUE` and hands the buffer back through
  `pbyKey`. `dwTmpKeyLen` is set to 32 but folds into the `16`-byte stride, so
  it has no location in the ORIG's DWARF - reproduced as written.
* `Cipher` — `dwBufLen > 0xFFFF` returns `FALSE` before any key work; else
  `GenKey(nType, dwKeySeed, pbyKey)` then the 19-arm `Ten*` dispatch
  (table mapping recorded in the source comment), each arm returning `TRUE`;
  `nType == 0` and `nType > 19` return `FALSE`; `delete[] pbyKey` at the end.
  The `catch (...)` landing pad at `0x001937e6` returns `TRUE`, and it covers
  the `GenKey` call too - verified by making the next `new[]` throw through an
  interposed `operator new[]`: ORIG `{threw=0, ret=1}`, reconstruction
  `{threw=0, ret=1}` (before moving `GenKey` inside the `try` the
  reconstruction propagated the exception).
* `CallDecrypt` — `dwBufLen > 0xFFFF` or `dwKeyLen > 0xFFFF` → `FALSE`; 13 arms
  with a per-arm key-length check (`8/16/24/16/16/16/16/16/32/16/8/16/32`),
  `nCode == 0` or `> 13` → `FALSE`; the ORIG has no catch handler here (the function ends straight after the last arm).
* `UninitKeyMatrix` — frees and NULLs `y` row pointers, never the header array,
  no NULL check on `pHeader`; returns `TRUE`.
* `InitKeyMatrix` — NULL `arrKey` → NULL; `new UINT32 *[y]`, `pHeader[0] = NULL`,
  then `y` rows of `x` words copied from the flat key array, returning the
  header. Allocation failure releases the rows, clears the header and returns
  NULL (the ORIG leaves rows `i..y-1` uninitialised on that path - a latent
  bug in the original, preserved).
* `UninitKeyTree` — post-order recursive `delete`; NULL returns `TRUE`; the
  `catch (...)` landing pad at `0x00193c37` returns `FALSE`.
* `UninitKeyLink` — iterative `next`-first release; returns `TRUE`.
* `InitKeyTree` — balanced build: `count == 0` → NULL, `count == 1` → leaf with
  `arrKey[0]`, otherwise `left = InitKeyTree(arrKey, count/2)`,
  `key = arrKey[count/2]`, `right = InitKeyTree(arrKey + count/2 + 1,
  count - count/2 - 1)`; every allocation failure sets `result = FALSE`, frees
  what was built (the left subtree in the right-subtree failure path) and
  returns NULL.
* `InitKeyLink` — NULL key array or `count == 0` → NULL; head node from
  `arrKey[0]`, then one node per remaining key, appended in order; allocation
  failure releases the chain, clears the head and returns NULL.

`codeengine.cpp`

* Every function rejects a NULL cursor, a NULL `*cursor`, a NULL data pointer
  and (where present) a non-positive length with return `0`; the success return
  is the byte count (`1 / 2 / 4 / length`, and `length + 2` for the strings).
* `EncodeChar`/`DecodeChar` move one byte.
* `*Mem*` are raw `memcpy` blocks (`sMemSize <= 0` rejected before the copy;
  the `short` forms return the sign-extended size).
* `DecodeString` — reads a big-endian 16-bit length, advances the cursor by 2,
  then **copies `min(length, sMaxStrLength)` bytes and terminates at that
  index, but advances the cursor by and returns the *unclamped* length + 2**
  (ORIG `edi` at `0x0019722f` is never rewritten by the clamp at
  `0x00197270`). Preserved.
* `EncodeString` — `n = (unsigned short)strlen(strSrc)` truncated to 16 bits,
  clamped to `sMaxStrLength`; writes `swap16(n)` then `n` bytes; no terminator
  is written; the cursor advances by `n + 2`, which is also the return value.
* `DecodeInt`/`EncodeInt`/`DecodeShort`/`EncodeShort` — full byte reversal of
  the 32-/16-bit operand (the ORIG's 32-bit reversal is the 16-bit rotate
  triple `ror 8,%ax / ror 16,%eax / ror 8,%ax`; the 16-bit one is a single
  `ror 8`).

## Oracle results

```
$ bash build/tmp/vmcodec/build_oracle.sh
built build/tmp/vmcodec/oracle
renamed symbols: 18 + 12
$ LD_LIBRARY_PATH=/opt/dnf1031/home/template/neople/game ./build/tmp/vmcodec/oracle
Cipher/GenKey throw probe: orig{threw=0 ret=1} mine{threw=0 ret=1}
ALL MATCH  cases=76771 failures=0
```

Coverage per surface:

| surface | inputs |
|---|---|
| `rol`,`ror` | all 256 byte values × shift counts −17..17, NULL buffer |
| `xora`,`anda`,`ora`,`nota` | counts −3..20, seeds {0, 1, 0xdeadbeef, 0xffffffff, 0x00ff00ff}, NULL buffer, plus 4,000 deterministic fuzz rounds |
| `GetLinkData` | 6-node list, count −3..9, NULL head |
| `GetTreeData` | trees of 0..12 keys, cursor 0..count+1 (node pointer + written-back cursor compared) |
| `GetMatrixData` | 6 (x,y) shapes incl. 0 dimensions, every valid linear index, over-range index, NULL row, NULL header |
| `GenKey` | nType 0..25 × seeds {0, 1, 0x12345678, 0xffffffff, 0xa5a5a5a5} - 32 key bytes compared |
| `Cipher` | nType 0..30 × lengths {0,1,15,16,17,32,64,0xffff,0x10000,0x10001} × 2 seeds × both flags - whole buffer compared |
| `CallDecrypt` | nCode 0..15 × 7 buffer lengths × 11 key lengths × both flags - whole buffer compared |
| `InitKey*`/`UninitKey*` | 6 matrix shapes, tree/list lengths 0..16, NULL arguments, structural dump comparison, plus free-path return values |
| codeengine | chars: all 256 values; blocks: sizes −5..16; strings: lengths 0..20 × max −2..22 on both sides of the clamp; scalars: 13 boundary values + 4,000 fuzz rounds |

Bugs the oracle caught during development (both in my first draft, not in the
ORIG): the 32-bit byte reversal had its two halves swapped
(`swap16(hi) << 16 | swap16(lo)` instead of `swap16(lo) << 16 | swap16(hi)`),
and `Cipher`'s `try` did not cover `GenKey`. Both are fixed and re-verified.

## Size deltas

Full matrix with per-symbol status: `vm_codec_matrix.tsv`. Summary:
18/18 `CommonProc.cpp` and 10/12 `codeengine.cpp` symbols are within ±15 %.
Eleven `CommonProc.cpp` symbols are an exact size match (`rol`, `ror`,
`xora`, `anda`, `ora`, `nota`, `GenKey`, `InitKeyMatrix`, `UninitKeyMatrix`,
`UninitKeyTree`, `UninitKeyLink`), as are `EncodeChar` and `DecodeChar`;
`InitKeyTree` is +0.6 %, `DecodeShort` -1.9 %.

Two documented exceptions (same machine code semantics, different GCC
version):

1. `DecodeInt` 69 vs 58 bytes (+19 %). The ORIG build folds the 32-bit byte
   reversal into `ror $8,%ax / ror $0x10,%eax / ror $8,%ax` (11 bytes). The
   vendor GCC 4.1.2 in `c5root` never produces that triple: it expands every
   spelling of the reversal I measured (4-term mask/shift chain, two 32-bit
   rotates, two `unsigned short` halves) into shift/mask code. The form used
   here (`shr $0x10 / rol $8,%ax / rol $8,%dx / shl $0x10 / movzwl / or`) is
   21 bytes, so the object is 11 bytes larger with identical behaviour.
2. `EncodeShort` 43 vs 52 bytes (−17 %). The ORIG spills `usTemp` to its frame
   (`mov %dx,-0x2(%ebp)`) inside a 16-byte frame; this build keeps it in a
   register. 9 bytes smaller, identical behaviour.

These are compiler-generation artifacts, not logic differences; no attempt was
made to defeat the register allocator to match them.

## Symbols emitted

The two objects define exactly the 30 CU symbols plus compiler artifacts
(`__i686.get_pc_thunk.bx` local, `DW.ref.__gxx_personality_v0` weak data).
Undefined references are only the intended boundaries: 28 `Ten*` entry points
(27 from `TenCrypt.h` + `TenMd5`), `operator new/new[]/delete/delete[]`,
`memcpy`, `strlen`, and the EH runtime (`__cxa_begin_catch`,
`__cxa_end_catch`, `_Unwind_Resume`, `__gxx_personality_v0`) used by the two
`catch (...)` paths that exist in the ORIG. No helper, wrapper or asm bridge
was introduced.

## Reproduce

```sh
# objects (vendor GCC 4.1.2, -m32 -O2 -fPIC)
sh c5build.sh recon/src/VMCore/common_proc.cpp /tmp/vm_cp.o -I recon/include
sh c5build.sh recon/src/VMCore/codeengine.cpp /tmp/vm_ce.o -I recon/include

# size matrix (needs the two objects above)
python3 build/tmp/vmcodec/mktsv.py

# differential oracle
bash build/tmp/vmcodec/build_oracle.sh
LD_LIBRARY_PATH=/opt/dnf1031/home/template/neople/game ./build/tmp/vmcodec/oracle
```

## Limits

* Not byte-identical; two size deltas explained above.
* The ORIG's LSDA could not be decoded by address arithmetic (the FDE's
  `pcrel` LSDA pointer for `Cipher` lands outside `.gcc_except_table` in this
  image), so the `try` region boundary was established empirically with the
  allocation-failure probe instead of from the exception table.
* The allocation-failure branches of `InitKeyMatrix`/`InitKeyLink` are
  unreachable with a throwing `operator new` (the ORIG tests the result for
  NULL, which `new` never returns), so they are reconstructed from the
  disassembly only and are not exercised by the oracle.
* `Cipher`/`CallDecrypt` arms that pass a buffer length the underlying Ten*
  routine does not expect are outside the tested domain; only lengths whose
  guards accept them were compared.
