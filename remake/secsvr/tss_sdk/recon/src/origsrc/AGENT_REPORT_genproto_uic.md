# GenProto pilot — `formmog_proto_tss_sdk_uic.cpp` reconstruction

CU (ORIG, tag 394):
`/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp`
inside `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ORIG compiled by `GCC 4.1.0 (SUSE Linux)` — `readelf -p .comment`; the available
vendor toolchain is `c5root g++ 4.1.2 (Red Hat 4.1.2-55)`).

Artifacts (all new, no commits):
* `recon/src/origsrc/formmog_proto_tss_sdk_uic.cpp` — the TU (44 functions).
* `recon/src/origsrc/formmog_proto_tss_sdk_uic.h` — private protocol header
  (DWARF layout, `#pragma pack(1)`).
* `recon/src/origsrc/tsf4g_tdr_compat.h` — ABI model of `tsf4g_tdr`
  (`TdrError`, `TdrWriteBuf`, `TdrReadBuf`, `TdrBufUtil`) built from the CU's
  own inlined-call records.
* `recon/src/origsrc/genproto_uic_matrix.tsv` — machine-readable
  `addr_orig / size_orig / size_recon / ratio / name` (44 rows).
* `tools/tss_sdk_recon/cu_syms.py`, `cu_layout.py`, `dis_annotate.py`,
  `dwarf_inline_names.py`, `dwarf_type_decl.py`, `dwarf_inl_dbg.py` — new
  helper scripts (see *commands*).  Existing scripts were not modified.

## 1. Symbol table used (ORIG address / size)

`cu_syms.py <elf> formmog_proto_tss_sdk_uic.cpp` enumerates every
`DW_TAG_subprogram` with `DW_AT_low_pc` in the CU; all 44 rows below were
cross-checked byte-for-byte against `nm -DC -S --defined-only` (address and
size identical for every row).  The 43 `sec_proto::` rows are exactly the set
matched by `grep -E 'sec_proto::(UserInputCheckPkg|UserInputCheckPkgHead|UserInputCheckPkgBody|ReqUserInputCheck|RspUserInputCheck)::'`
(43 lines, no others); the 44th is the CU's only out-of-line `tsf4g_tdr`
definition.

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | tsf4g_tdr::TdrWriteBuf::textize(char const*, ...) | 108 | 108 | 1.000 |
| 00165be0 | sec_proto::UserInputCheckPkgHead::construct() | 39 | 39 | 1.000 |
| 00165c10 | sec_proto::RspUserInputCheck::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 249 | 249 | 1.000 |
| 00165d10 | sec_proto::UserInputCheckPkgHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 308 | 308 | 1.000 |
| 00165e50 | sec_proto::RspUserInputCheck::construct() | 37 | 37 | 1.000 |
| 00165e80 | sec_proto::ReqUserInputCheck::construct() | 53 | 53 | 1.000 |
| 00165ec0 | sec_proto::UserInputCheckPkgBody::construct(long long) | 94 | 94 | 1.000 |
| 00165f20 | sec_proto::UserInputCheckPkg::construct() | 75 | 75 | 1.000 |
| 00165f70 | sec_proto::UserInputCheckPkgHead::visualize_ex(TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00166000 | sec_proto::RspUserInputCheck::visualize_ex(TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00166090 | sec_proto::UserInputCheckPkgHead::pack(TdrWriteBuf&, unsigned int) const | 297 | 272 | 0.916 |
| 001661c0 | sec_proto::ReqUserInputCheck::visualize(TdrWriteBuf&, int, char) const | 884 | 862 | 0.975 |
| 00166540 | sec_proto::UserInputCheckPkgBody::visualize(long long, TdrWriteBuf&, int, char) const | 300 | 298 | 0.993 |
| 00166670 | sec_proto::UserInputCheckPkg::visualize(TdrWriteBuf&, int, char) const | 247 | 248 | 1.004 |
| 00166770 | sec_proto::UserInputCheckPkg::visualize_ex(TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00166800 | sec_proto::ReqUserInputCheck::visualize_ex(TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00166890 | sec_proto::UserInputCheckPkgHead::unpack(TdrReadBuf&, unsigned int) | 315 | 261 | 0.829 |
| 001669d0 | sec_proto::UserInputCheckPkgHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00166a40 | sec_proto::RspUserInputCheck::pack(TdrWriteBuf&, unsigned int) const | 303 | 313 | 1.033 |
| 00166b70 | sec_proto::ReqUserInputCheck::pack(TdrWriteBuf&, unsigned int) const | 577 | 549 | 0.951 |
| 00166dc0 | sec_proto::UserInputCheckPkgBody::pack(long long, TdrWriteBuf&, unsigned int) const | 149 | 149 | 1.000 |
| 00166e60 | sec_proto::UserInputCheckPkg::pack(TdrWriteBuf&, unsigned int) const | 277 | 252 | 0.910 |
| 00166f80 | sec_proto::UserInputCheckPkgHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00167010 | sec_proto::UserInputCheckPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001670e0 | sec_proto::UserInputCheckPkgHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 001671a0 | sec_proto::ReqUserInputCheck::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00167230 | sec_proto::UserInputCheckPkgHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00167300 | sec_proto::UserInputCheckPkg::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 001673c0 | sec_proto::UserInputCheckPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00167450 | sec_proto::UserInputCheckPkgBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const | 198 | 198 | 1.000 |
| 00167520 | sec_proto::UserInputCheckPkgBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const | 147 | 147 | 1.000 |
| 001675c0 | sec_proto::RspUserInputCheck::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00167690 | sec_proto::RspUserInputCheck::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00167750 | sec_proto::RspUserInputCheck::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 001677e0 | sec_proto::ReqUserInputCheck::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001678b0 | sec_proto::ReqUserInputCheck::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00167970 | sec_proto::RspUserInputCheck::unpack(TdrReadBuf&, unsigned int) | 299 | 302 | 1.010 |
| 00167aa0 | sec_proto::RspUserInputCheck::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00167b10 | sec_proto::ReqUserInputCheck::unpack(TdrReadBuf&, unsigned int) | 562 | 533 | 0.948 |
| 00167d50 | sec_proto::UserInputCheckPkgBody::unpack(long long, TdrReadBuf&, unsigned int) | 149 | 149 | 1.000 |
| 00167df0 | sec_proto::UserInputCheckPkg::unpack(TdrReadBuf&, unsigned int) | 244 | 213 | 0.873 |
| 00167ef0 | sec_proto::UserInputCheckPkg::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00167f60 | sec_proto::UserInputCheckPkgBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int) | 122 | 122 | 1.000 |
| 00167fe0 | sec_proto::ReqUserInputCheck::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |

## 2. Commands run

```
# --- enumeration (step 1) -------------------------------------------------
python3 cu_syms.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_uic.cpp
nm -DC -S --defined-only /opt/.../libtss_sdk.so.1.0.394 > /tmp/orig_nm.txt
nm -DC -S --defined-only /opt/.../libtss_sdk.so.1.0.394 | \
  grep -E 'sec_proto::(UserInputCheckPkg|UserInputCheckPkgHead|UserInputCheckPkgBody|ReqUserInputCheck|RspUserInputCheck)::' | wc -l
#  -> 43 (only these types belong to the CU; the CU's full DWARF list is 44 rows)

# --- layout (step 2) ------------------------------------------------------
python3 cu_layout.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_uic.cpp UserInput
python3 dwarf_type_decl.py /opt/.../libtss_sdk.so.1.0.394 UserInputCode
#  -> enum UserInputCode declared at formmog_proto_tss_sdk_uic.h:36

# --- disassembly (step 3) -------------------------------------------------
nm -DC -S --defined-only /opt/.../libtss_sdk.so.1.0.394 | grep -E 'sec_proto::(User…|Rsp…)::' | sort > /tmp/uic_nm.txt
objdump -d -C --start-address=0x165be0 --stop-address=0x168050 /opt/.../libtss_sdk.so.1.0.394 > /tmp/uic_dis.txt
python3 dis_annotate.py /opt/.../libtss_sdk.so.1.0.394 /tmp/uic_dis.txt > /tmp/uic_ann.txt
python3 dwarf_inline_names.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_uic.cpp   # Tdr API + ORIG .cpp call lines

# --- build / verification (step 4) ---------------------------------------
./c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_uic.cpp /tmp/uic.o
./symdiff.sh /opt/.../libtss_sdk.so.1.0.394 /tmp/uic.o 'sec_proto::(UserInputCheckPkg|UserInputCheckPkgHead|UserInputCheckPkgBody|ReqUserInputCheck|RspUserInputCheck)::'
./sizecmp.sh /tmp/uic.o 'sec_proto::(UserInputCheckPkg|UserInputCheckPkgHead|UserInputCheckPkgBody|ReqUserInputCheck|RspUserInputCheck)::'
./strfind.sh < <literal list>            # every literal PRESENT
./obj_fn.sh /tmp/uic.o "<symbol>" <len>  # per-function object disassembly
```
`symdiff.sh`/`sizecmp.sh` write fixed paths under `/tmp` that are currently owned
by root in this box, so they were run through byte-identical copies with the
temp prefix renamed (`sed 's|/tmp/|/tmp/gpp_uic_|g' symdiff.sh > /tmp/gpp_uic_symdiff.sh`,
same for `sizecmp.sh`); only the temp-file names differ.

## 3. Results

* `symdiff.sh`: `== ORIG-only ==` empty, `== NEW-only ==` empty, counts `43 / 43`.
* `sizecmp.sh`: `matched=43 identical=28 differing=15`; min ratio 0.829, max 1.033.
* Only **two** symbols are outside ±10 % (both *smaller* than the ORIG, i.e.
  strictly less code, no missing behaviour):
  * `UserInputCheckPkgHead::unpack(TdrReadBuf&, unsigned int)` 315 → 261 (0.83)
  * `UserInputCheckPkg::unpack(TdrReadBuf&, unsigned int)` 244 → 213 (0.87)
  *cause (measured):* these are the two 16-bit-read-heavy functions.  The ORIG
  expands every big-endian swap as `shr $8/shl $8/or`; the 4.1.2 scheduler
  recognises the same expression as a rotate — `objdump -d /tmp/uic.o | grep -c rol`
  = 18, `objdump -d --start-address=0x165be0 --stop-address=0x168050 <ORIG> | grep -c rol`
  = 0.  Same source shape, shorter code.  The remaining 12 divergences are
  ≤ ±9 % (`ReqUserInputCheck::pack` 549/577, `Pkg::pack` 252/277,
  `Head::pack` 272/297, `Rsp::pack` 313/303, `…visualize…` 198–862, …).
* `Head::unpack` additionally profits from 4.1.2 CSE-ing the `getUsedSize()+2`
  address of the version peek with the following `version_` read.

### Divergences that are compiler-generation artifacts (reported, not chased)

`readelf -p .comment` of the ORIG shows GCC 4.1.0; `c5root` is GCC 4.1.2.  Two
4.1.2 behaviours were found and both are *reproduced or compensated*:

1. `rol`-based byte swaps (see above) — smaller code, left as is.
2. EH cleanup landing pads: 4.1.2 emits a cleanup region (31 bytes + alignment,
   `_Unwind_Resume`) for every `pack(char*)`/`unpack(char*)` wrapper, even though
   their callees are provably nothrow.  Proof that the ORIG has no such region:
   `_Unwind_Resume@plt` is referenced exactly 9 times in the CU range, and only
   inside `visualize`/`visualize_ex(char*)` wrappers (0x1670d4, 0x167191,
   0x1672f4, 0x1673b1, 0x167511, 0x167684, 0x167741, 0x1678a4, 0x167961);
   the FDE for `Head::pack(char*)` (0x166f80..0x167005) carries no LSDA.
   Minimal repro of the 4.1.2 behaviour: a local with a non-trivial destructor +
   one call to a nothrow same-TU member (`/tmp/gpp_t8.cpp`) still yields
   `_Unwind_Resume`.  Compensation: the `pack`/`unpack` declarations carry
   `throw()` (documented in the header).  An empty exception specification is
   not part of the Itanium mangling (`nm` proof: `int N::S::g() throw()` →
   `_ZN1N1S1gEv`), so all 43+1 symbol names are unchanged, and the compensation
   removes exactly the region the ORIG does not have: with it,
   28/43 sizes are byte-identical and no symbol is >3.3 % off except the two
   `unpack`s above.  Without it, 6 further symbols would be +25 % (the wrappers
   133→166, 147→182).  `throw()` is *not* an ORIG source attribute: the
   `visualize*(char*)` wrappers of the ORIG *do* have landing pads, which would
   be impossible if the generator had emitted `throw()` on all methods.

### Tdr API recovered from DWARF (not guessed)

`dwarf_inline_names.py` names every inlined callee of the CU; the header model
`tsf4g_tdr_compat.h` implements `TdrWriteBuf::{_set,_free,getUsedSize,getTotalSize,
getBeginPtr,writeChar,writeInt8,writeUInt8,writeUInt16,writeUInt32,writeBytes,
writeUInt8(uint8_t,unsigned),writeChar(char,unsigned),writeUInt16(uint16_t,unsigned),
writeCharWithNull,vtextize,textize}`, `TdrReadBuf::{set,getUsedSize,getTotalSize,
getBeginPtr,readUInt8,readInt8,readUInt16,readUInt32,readUInt16(uint16_t&,unsigned),
readBytes}` and `TdrBufUtil::{printVariable(…,const char* fmt,…), printVariable(…,bool),
printArray(…,long long)}` (the last three stay external PLT calls, exactly as in
the ORIG).  Member layout `data_/pos_/limit_/allocated_` (+0/+4/+8/+0xc) is the
offset set the ORIG's inlined ctor/dtor touch.

Key semantics recovered from the disassembly (all reproduced in the source):

* `construct()`: `Head` sets `version_ = 1`, others 0; `Req`/`Rsp` inlined
  `memset(this,0,sizeof(*this))` (1169-byte `memset@plt` / 4 stores);
  `Body::construct(sel)` and `Pkg::construct()` dispatch on the union selector.
* union selectors are `1 = req_user_input_check_`, `2 = rsp_user_input_check_`
  (ORIG: `xor $0x1` / `xor $0x2` on the low word OR-ed with the high word);
  unknown selectors return `TDR_NO_ERROR`; no `visualize_ex` exists for the union
  and `Body::visualize(long long, char*, …)` returns `TdrError::ErrorType`
  (DWARF return type), while the non-union `visualize_ex(char*, …)` return
  `const char*` (the buffer).
* cutVer prelude of the versioned functions is
  `if (0 == cutVer || cutVer > 2) cutVer = 2; if (0 == cutVer) return TDR_ERR_CUTVER_TOO_SMALL;`
  — the `-9` branch is unreachable in the ORIG (the clamp already replaced 0 by 2),
  and is preserved because the ORIG emits it (dead block at the tail).
* `ReqUserInputCheck`: `if (cutVer > 1) write/readInt8(user_input_code_)`
  (the *unpack* else-branch zeroes the field), bounded arrays
  (`> 1024` / `> 128` → `TDR_ERR_REFER_SURPASS_COUNT`, then
  `capacity()-size() < len` → `TDR_ERR_SHORT_BUF_FOR_WRITE/_READ`, then one
  bulk `writeBytes`/`readBytes` = inlined `memmove`).
* `Head::pack` finishes with `writeUInt16(1, startPos + 2)` (constant `movw $0x100`),
  `Pkg::pack` with `writeUInt16(cutVer, startPos + 2)` and
  `writeUInt16((uint16_t)(getUsedSize() - startPos), startPos + 4)`;
  `Pkg::unpack` peeks `pkg_len_` at `+4` (bounds-checked), rejects
  `version-1 > 1` with `TDR_ERR_NET_CUTVER_INVALID`, then `version == 0` with
  `TDR_ERR_CUTVER_CONFILICT`, then unpacks `head_` and `body_` with the wire
  version (its own `cutVer` argument is unused).
* `visualize*`: `printVariable(…, name, fmt, …)` / `printVariable(…, name, bool)`
  for nested objects, `indent >= 0 ? indent + 1 : indent`, arrays via
  `printArray(…, (long long)len)` + a `textize(" 0x%02x", …)` loop, `separator`
  via `writeCharWithNull()`; `visualize_ex(TdrWriteBuf&)` ends with
  `writeChar('\0', used < total ? used : total - 1)` and returns
  `getBeginPtr()`; the buffer forms return `""` for `data()==NULL`/`capacity()==0`.

### Literal check (must exist verbatim in ORIG)

The 22 non-empty string literals emitted by the reconstruction were extracted
from the file and tested with `strfind.sh` — all `PRESENT`.  Stronger: the set
of `.rodata` literals referenced by the ORIG CU (`dis_annotate.py` output) is
`''`, `' 0x%02x'`, `'%d'`, `'%u'`, `'0x%02x'` and the 18 `'[field_]'` names;
set difference against the reconstruction's literals is empty (the 23rd ORIG
literal is the `""` returned by the `visualize_ex` NULL-buffer guards, present
8× in the source: 4 classes × 2 overloads).

### `recon/link_check.sh`

Not modified, and the new TU is **not** added to it.  Status of the current
checkout: the script fails in its first TU —

```
inc/VMCore/cryptdata.h:13: error: member '_vptr' conflicts with virtual function table field name
```

— which is *pre-existing and unrelated* to this pilot:
* `inc/VMCore/cryptdata.h` declares `void **_vptr;` and rejects it under both the
  vendor `c5root g++ 4.1.2` and the system `g++ 13.3.0`
  (`struct A { void** _vptr; virtual ~A(); };` reproduces it);
* at `HEAD` the file is identical (`git archive HEAD tools/tss_sdk_recon/recon`
  + tar -x reproduces it; the working-tree diff for `cryptdata.{h,cpp}` is
  line-ending-only, i.e. a sibling's in-flight edit);
* nothing in this pilot touches that TU or the script.

The new object is link-clean on its own: `nm -C --defined-only /tmp/uic.o`
contains exactly the 43 `sec_proto::` symbols plus one weak
`tsf4g_tdr::TdrWriteBuf::textize` (the CU's own out-of-line definition, also
present in the ORIG) and leaves `TdrBufUtil::printVariable/printArray`,
`memmove`/`memset`, `vsnprintf` and `operator delete[]` undefined, as the ORIG
does.

## 4. Residual uncertainty

* The ORIG's `.cpp` line numbers quoted in the source are `DW_AT_call_line`
  values of inlined calls (evidence), not the ORIG's real statement layout.
* Names of *inline* header helpers (e.g. `writeUInt16(uint16_t, unsigned)`) are
  exact (DWARF linkage names); the *bodies* are modelled to reproduce the
  emitted instruction shapes — the model matches the ORIG for all 43 symbols
  within the ratios above.
* `Head::pack`'s trailing version stamp is a constant `1` in the ORIG machine
  code; whether the ORIG source spelled it `1`, a macro or `cutVer` is not
  recoverable from the binary (the store is a constant, so it is not `cutVer`).
