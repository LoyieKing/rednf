# genproto / latrans + secsvr-extra CU reconstruction report

Two translation units of `libtss_sdk.so.1.0.394` (tag 394), reconstructed from the
ORIG binary's DWARF + disassembly.  No commits, no ORIG-side edits.

| file (in `recon/src/origsrc/`) | CU substring | functions |
|---|---|---|
| `formmog_proto_public_latransadapt.cpp/.h` | `formmog_proto_public_latransadapt.cpp` | 82 = 81 `sec_proto::LATransAdapt*` (9 classes x 9 methods) + weak `tsf4g_tdr::TdrWriteBuf::textize` |
| `formmog_proto_public_secsvr_extra.cpp` | `formmog_proto_public_secsvr.cpp` | 45 (9 methods x 5 classes) of the CU's 199; the other 154 come from `reuse/obj/...secsvr.o` |
| `tsf4g_tdr_compat_latrans.h` | - | per-CU TDR ABI model (extension of the shared `tsf4g_tdr_compat.h`, which was not modified) |

Matrices: `genproto_latrans_matrix.tsv` (82 rows), `genproto_secsvr_extra_matrix.tsv`
(45 rows); columns `addr, size_orig, size_recon, ratio, name`.

## 1. Symbols

`python3 cu_symbols.py <ORIG> <cu-substring> --sizes` (DWARF-attributed) is the
authoritative list.  Full addr/size tables are in the two TSVs.

* Target A: 82 DWARF rows (81 `sec_proto::LATransAdapt*` + `textize`).  All 82 are
  emitted; `textize` is emitted **weak** (`W`, 108/108 bytes) exactly like ORIG
  (weak because it is the only out-of-line tsf4g_tdr definition the CU owns and the
  UIC pilot TU defines the same symbol; a strong redefinition would break the link).
* Target B: the CU has 199 DWARF rows.  `cu_match.sh formmog_proto_public_secsvr.cpp
  reuse/obj/src__protocol__formmog__formmog_proto_public_secsvr.o` ->
  `cu=199 obj=201 matched=154`, and "in CU, absent from object" is **exactly** the
  45 symbols implemented by the extra file (5 x `SecsvrCaptchaReq`,
  `SecsvrCaptchaResultRsp`, `SecsvrGreenUnionRsp`, `SecsvrImInfoNotifyReq`,
  `SecsvrUserInfoNotifyReq`).  Those 5 classes are absent from the reused source
  (tag drift), which is why they had to be written from the ORIG's own machine code.
* Coverage proof: `reuse.o` + `svx.o` provide all 199 CU names (`in CU, absent from
  objects = <empty>`), and `ld -r` links both objects without duplicate definitions
  (they overlap only in local labels / `__i686.get_pc_thunk.bx`).

## 2. Method (per function)

1. Class layouts from DWARF (`cu_layout.py`), cross-checked against the ORIG
   `construct()` memset length (`memset(this,0,sizeof(*this))`): latrans 32009 /
   32006 / 1028 / 1026 / 10252 / 21 / 10246 / 14 / 32786, secsvr-extra 541 / 534 /
   5 / 38 / 2056.  Both files carry `typedef char ... [sizeof(T)==N?1:-1]` and
   `offsetof(trailing array)` assertions; all pass.
2. Call sequence per function from the CU's own DWARF inline records
   (`dwarf_inline_names.py`): exact field order, exact `writeUIntNN/readUIntNN/IntNN`,
   `writeBytes/readBytes`, `printVariable`/`printArray`/`textize`/`writeCharWithNull`
   sequence and the ORIG source line numbers (kept as comments).
3. Machine-code cross-read (`objdump` + `dis_annotate.py`) for: format strings per
   field (`uint32->"%u"`, `uint16/int32/int16->"%d"`, `uint8->"0x%02x"`, arrays ->
   `printArray(..., (int64_t)len)` + `textize(" 0x%02x", elem)`), array capacity
   guards (`>512/1024/10240/32000/32768` -> `TDR_ERR_REFER_SURPASS_COUNT`), the
   `capacity()-size() < len` guard -> `-1/-2` `TDR_ERR_SHORT_BUF_FOR_(WRITE|READ)`,
   bulk `writeBytes/readBytes`, fixed 16-byte array (`client_key_signature_`) without
   a count guard, scalar-only `visualize()` returning the last `printVariable`
   result (no separator write: `LATransAdaptDPCmdMonitor`,
   `SecsvrGreenUnionRsp`), array-ending `visualize()` ending in
   `writeCharWithNull(separator)`.
4. `cutVer` is read by **none** of the 127 functions: verified automatically
   (`0x10(%ebp)` argument never loaded in any pack/unpack(TdrWriteBuf&/TdrReadBuf&)).
5. Every `pack/unpack` declaration carries `throw()` — a codegen compensation, not
   an ORIG attribute: ORIG (4.1.0) `_Unwind_Resume` appears only in
   `visualize/visualize_ex(char*)` wrappers, while vendor 4.1.2 adds a cleanup
   landing pad to every wrapper owning a `TdrWriteBuf/TdrReadBuf` local.  An empty
   exception spec does not enter the mangling, so all symbol names stay ORIG-identical.

## 3. Commands run and observed results

| command | result |
|---|---|
| `sh c5build.sh recon/src/origsrc/formmog_proto_public_latransadapt.cpp la.o -I recon/src/origsrc` | rc=0, no warnings, all layout assertions pass |
| `sh c5build.sh recon/src/origsrc/formmog_proto_public_secsvr_extra.cpp svx.o -I recon/src/origsrc` | rc=0, no warnings (see 4.3) |
| `symdiff.sh ORIG la.o 'sec_proto::LATransAdapt\|TdrWriteBuf::textize'` | no NEW-only, no ORIG-only within the CU's own symbols; remaining ORIG-only lines are 6 weak template instantiations (`appdata_encode<LATransAdapt...>`) that belong to other CUs - none of them is in this CU's `cu_symbols.py` list |
| `symdiff.sh ORIG comb.o 'SecsvrCaptchaReq\|...\|SecsvrUserInfoNotifyReq'` (`comb.o` = `ld -r` of reuse.o + svx.o) | NEW-only empty; ORIG-only = the same 3 weak template instantiations `send_ipc_pkg<SecsvrCaptchaReq>` / `appdata_encode<SecsvrCaptchaReq>` (not in this CU's symbol list, verified by `cu_symbols.py`) |
| `sizecmp.sh la.o 'sec_proto::LATransAdapt\|TdrWriteBuf::textize'` | 82/82 present; 47 exactly identical, max ratio 1.011, min 0.833 (outliers in 4.1) |
| `sizecmp.sh svx.o '<5 classes>'` | 45/45 present; 26 identical, min 0.874, max 1.000 |
| per-object literal check (all `.rodata` NUL-terminated strings vs `grep -aF` in ORIG) | 0 missing literals in either object |
| per-function literal+call comparison, ORIG annotated listing vs object (multiset **and** first-occurrence order, `.LCn` resolved through `readelf -sW` + section bytes) | `compared=83 mismatched=0` (latrans), `compared=46 mismatched=0` (secsvr-extra): every field name, format string, callee and their order match ORIG |
| `sizecmp` sanitity rebuild with the two dead uint8 guards removed | object byte-identical (md5 `5212c311291c3e9a234fcdad82b99722`) |

## 4. Divergences (all measured, none semantic)

### 4.1 Size outliers below -12%

| function | ORIG | recon | ratio | cause |
|---|---|---|---|---|
| `LATransAdaptAntibotSigInfo::pack` | 384 | 320 | 0.833 | 4.1.2 instruction selection |
| `LATransAdaptAntibotClientIP::pack` | 203 | 177 | 0.872 | " |
| `LATransAdaptAntibotRPCodeResult::pack` | 247 | 216 | 0.874 | " |
| `LATransAdaptAntibotRPFMC::pack` | 247 | 216 | 0.874 | " |
| `SecsvrImInfoNotifyReq::unpack` | 247 | 216 | 0.874 | " |

Measured cause (same class of divergence the UIC pilot documented): 4.1.2
optimises strictly better than the ORIG's 4.1.0 on exactly the code shapes the
generator emits, and every difference removes instructions:

* 16-bit byte swap: 4.1.2 emits `rol $0x8,%ax`; ORIG emits `mov/shr/shl/or`.
  Counted in the latrans CU range: **ORIG 0 `rol`, recon 17** (and the ORIG CU has
  no `bswap` either).
* `pos_` update: 4.1.2 strength-reduces to `addl $imm,0x4(%reg)` and keeps
  `destBuf`/`this` live in registers; ORIG reloads them and uses
  `mov pos; add; mov pos`.  Counted: **in-place `addl $imm,0x4(%reg)` = 6 in ORIG,
  16 in recon**.
* the `uint16_t be` memcpy stage stays in a stack slot in 4.1.2 (`mov %ax,-0xe(%ebp)`),
  ORIG stores the swap result directly.

An instruction-by-instruction diff of `RPCodeResult::pack` (ORIG 98 instructions
vs recon 77) shows no missing call, guard or store: only the three effects above.
Function-by-function literal/call equality (section 3) proves no field, format or
callee is missing, so these ratios are compiler-generation noise, not scope loss.
All other 122 symbols are within ±12% (49 exactly identical, largest +1.1%).

### 4.2 ORIG guards not reproduced

* `readBytes(void*, unsigned)` in ORIG starts with a `dest == NULL` guard returning
  `TDR_ERR_ARG_POINTER_IS_NULL (-8)`; it is materialised at exactly one call site of
  the latrans CU, `LATransAdaptAntibotClientIP::unpack` (`mov $0xfffffff8,%ecx` +
  `je`, 0x144104, dead in practice because dest is `this + 5`; whole-CU census: 1
  occurrence).  The per-CU header models that guard, which is why this function
  matches at 199/197 = 1.010.  The same guard is *not* modelled for the secsvr-extra
  TU (it uses the shared header): ORIG materialises it once there too
  (`SecsvrCaptchaResultRsp::unpack`, 0x14f616), i.e. 14 bytes of unreachable code
  are missing in that one function; the emitted code at that site is otherwise
  identical and the guard cannot fire for a member array of a valid object.
* `writeBytes` has no such guard in ORIG (the fixed-array pack site would have
  materialised it) - the model matches.

### 4.3 Source-level decisions

* The three `if (msg_len_ > 512)` guards of `SecsvrCaptchaResultRsp`
  (`msg_len_` is `uint8_t`, the array is 512 bytes) are provably dead: the ORIG
  machine code has no compare for them and 4.1.2 warns "comparison is always false".
  They were written, then removed after measuring that the object is byte-identical
  either way (md5 above).  The source now carries a comment recording this.
* The secsvr-extra TU declares its 5 structs inline (the generated
  `formmog_proto_public_secsvr.h` is not in the tree, and the reused header is a
  different tag); layout comes from DWARF and is pinned by the assertions in 2.1.
* `tsf4g_tdr_compat_latrans.h` is a copy of the shared model plus
  `writeInt16/readInt16/writeInt32/readInt32` (DWARF-inlined callee names of the
  latrans CU at .cpp:689/764/2122/2212/2428/2543) and the `readBytes` dest guard of
  4.2.  The shared `tsf4g_tdr_compat.h`, `recon/link_check.sh` and every peer-owned
  file were left untouched.

## 5. Reproduce

```
sh c5build.sh recon/src/origsrc/formmog_proto_public_latransadapt.cpp /tmp/la.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_public_secsvr_extra.cpp /tmp/svx.o -I recon/src/origsrc
sh symdiff.sh <ORIG> /tmp/la.o 'sec_proto::LATransAdapt|TdrWriteBuf::textize'
sh sizecmp.sh /tmp/la.o 'sec_proto::LATransAdapt|TdrWriteBuf::textize'
sh cu_match.sh formmog_proto_public_secsvr.cpp reuse/obj/src__protocol__formmog__formmog_proto_public_secsvr.o
sh sizecmp.sh /tmp/svx.o 'SecsvrCaptchaReq|SecsvrCaptchaResultRsp|SecsvrGreenUnionRsp|SecsvrImInfoNotifyReq|SecsvrUserInfoNotifyReq'
```
(`symdiff.sh`/`sizecmp.sh` write `/tmp/*` files that already exist root-owned in this
sandbox; run them with `bash` and a writable tmp prefix, e.g. `sed 's|/tmp/|$HOME/tmp/|g'`.)

## 6. Behavioural differential oracle (follow-up)

`recon/src/origsrc/test_latrans_oracle.cpp` + `tools/tss_sdk_recon/oracle_latrans.sh`,
mirroring `test_uic_oracle.cpp` / `oracle_uic.sh`.  ORIG is dlopen()ed
`RTLD_GLOBAL`, the recon library `RTLD_LOCAL`; pack/unpack/visualize/
visualize_ex/construct are resolved by dlsym from both and compared on return
value, `*usedSize` and the full buffer (unpack additionally on the full struct
bytes).  The mangled names are built at run time from the class name, so the
test covers all 14 types: 9 `LATransAdapt*` + 5 `Secsvr*`.

```
sh oracle_latrans.sh
loaded; sizeof latrans=32009/32006/1028/1026/10252/21/10246/14/32786 svx=541/534/5/38/2056
cases=5704 failures=0
```

13 check kinds per iteration: `pack`, `pack-short` (size 0..64 -> -1 paths),
`unpack`, `unpack-trunc` (-2), `unpack-empty`, `visualize` (256 KB buffer),
`vis-short` (64 B -> `writeCharWithNull` failure path), `pack-null`,
`unpack-null`, `vis-null` (-19, nothing written), `vis-ex`, `vis-ex-empty`
(size 0 -> static `""`, `*usedSize` untouched) and `construct` (whole object
zeroed).  Length fields are randomised inside the array bounds, with explicit
0 / full-array iterations and an above-bound iteration (32001+, 10241+, 513+,
2049+, ... -> `TDR_ERR_REFER_SURPASS_COUNT`); char[] fields are NUL-terminated;
the multi-KB arrays (32000, 32768, 20480 arrays) are exercised at full length.

### 6.1 `-Wl,-Bsymbolic` is mandatory

Without it the recon library's internal calls (`pack(char*)` ->
`pack(TdrWriteBuf&)`, visualize -> `TdrBufUtil::*`) are preempted by the ORIG
copies resident in the global scope, so the core overloads are never exercised
and the oracle is vacuous.  The link now uses `-Wl,-Bsymbolic`; undefined
runtime symbols still bind to ORIG at dlopen time (that path is itself checked:
all 5704 cases call the reconstructed code paths that go through
`TdrBufUtil::printVariable/printArray` in ORIG).

### 6.2 Defects found by the oracle (fixed)

1. `TdrWriteBuf::writeInt16(int16_t)` in `tsf4g_tdr_compat_latrans.h` swapped
   the signed value (`(v >> 8) | (v << 8)` on `int16_t`), which ORs `0xffff`
   into the result for negative inputs; ORIG swaps through the zero-extended
   16-bit view (`movswl` load + `movzwl` before `shr/shl`, ORIG 0x1437d9).
   Symptom: `LATransAdaptAntibotErrMsg::pack` wrote `ff` instead of the low
   byte (`48/00`, `33/ff` first-diffs).  Fixed with a `uint16_t u = (uint16_t)v;`
   staging view.  Note the pre-`-Bsymbolic` run reported 0 failures here - the
   defective core function was never reached.
2. A `memcpy(data_ + pos_, &be, 2)` store lost while repairing (1); it made
   `writeInt16` a no-op writer (reported as `48/00` at offset 0 with matching
   ret and used size).  Restored; verified by the pristine run above.

### 6.3 Negative controls (non-vacuity proof)

| mutant | build | oracle result |
|---|---|---|
| none (pristine) | `latrans_proto.o` + `latrans_svx.o` | `cases=5704 failures=0` |
| latrans literal `[err_id_]` -> `[err_id_X]` | mutant of `formmog_proto_public_latransadapt.cpp` | `cases=5704 failures=96` |
| secsvr literal `[world_id_]` -> `[world_id_X]` | mutant of `formmog_proto_public_secsvr_extra.cpp` | `cases=5704 failures=192` |

### 6.4 Residual unsafe / uncovered paths

* `visualize_ex(TdrWriteBuf&)` is not driven directly (it needs a caller-owned
  `TdrWriteBuf`); its char*-overload sibling is covered, and it is the only
  entry point of the 127 that the oracle does not reach.
* The `pack/unpack(TdrWriteBuf&/TdrReadBuf&)` overloads are reached through
  their char* wrappers (with `-Bsymbolic`, i.e. the recon's own copies), not by
  direct dlsym; a defect in the wrapper itself would still be caught because
  both wrapper and core are compared.
* `writeInt32`/`readInt16`/`readInt32` are covered (they back
  `is_need_encrypt_`, `monitor_cmd_id_`, `err_id_`, `uin_`, `version_`...); the
  `writeInt16` defect above shows the sign paths are reachable.
* `/tmp` in this sandbox holds root-owned `.o`/`.so` leftovers from unrelated
  runs; the script uses distinct names (`latrans_*.o`, `liblatrans_recon.so`).
