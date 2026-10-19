# AGENT_REPORT — VMCore TsLocal units (`CreateTsLocal` / `FreeTsLocal` / `CTsLocalImp` / `ITsLocal`)

Scope: the last unresolved VMCore/TenCrypt symbols of
`libtss_sdk.so.1.0.394` (ELF32 i386, vendor GCC 4.1.2).

| ORIG symbol | address | size |
|---|---|---|
| `FreeTsLocal()` | 0x001b6850 | 0x34 |
| `CreateTsLocal()` | 0x001b6890 | 0x67 |
| `CTsLocalImp::CTsLocalImp` C1 / C2 | 0x00217f20 / 0x00218140 | 0x2c |
| `CTsLocalImp::~CTsLocalImp` D1 / D2 / D0 | 0x00218120 / 0x00218170 / 0x00218190 | 0x1e / 0x1e / 0x2e |
| `CTsLocalImp::LocalGetModuleHandle` | 0x00217f50 | 0xb |
| `CTsLocalImp::LocalGetProcAddress` | 0x00217f60 | 0xb |
| `CTsLocalImp::ProcessLastBytes` | 0x00217f70 | 0x130 |
| `CTsLocalImp::Seattos` | 0x002180a0 | 0x76 |
| `ITsLocal::~ITsLocal` D0 / D1 (weak) | 0x002181c0 / 0x002181f0 | 0x2e / 0x1e |

Deliverables (all new, no existing file modified):

* `recon/src/VMCore/tencrypt_ts_local.h` — `ITsLocal` (header-inline destructor so
  GCC emits the two *weak* copies the ORIG has, `W _ZN8ITsLocalD{0,1}Ev`),
  `CSeattos` layout (16 bytes) + method prototypes, `BOX3D1..BOX3D7`
  declarations, `CreateTsLocal`/`FreeTsLocal` declarations.
* `recon/src/VMCore/ts_local_imp.h` / `.cpp` — `CTsLocalImp : ITsLocal`
  (sizeof 12: vptr, `m_pvGetModuleHandle` +4, `m_pvGetProcAddress` +8).
* `recon/src/VMCore/tencrypt_ts_local.cpp` — the singleton.
* `recon/test_ts_local.cpp` — differential oracle vs ORIG.
* `recon/src/VMCore/ts_local_matrix.tsv` — per-symbol size/kind matrix.

## 1. Reconstructed semantics

`CreateTsLocal` (0x1b6890, 103 bytes) — single unguarded pointer, no refcount:

```cpp
static ITsLocal *g_sTsLocal;                 // ORIG: local .bss sym at 0x3267cc
ITsLocal *CreateTsLocal() {
    if (g_sTsLocal == 0) g_sTsLocal = new CTsLocalImp();   // operator new(12) + C1
    return g_sTsLocal;
}
void FreeTsLocal() { if (g_sTsLocal) { delete g_sTsLocal; g_sTsLocal = 0; } }
```

`FreeTsLocal` calls vtable slot +0x14 (`D0`) and then clears the global; there is
no counter and no second load.

`CTsLocalImp::LocalGetModuleHandle/Address` return `[this+4]` / `[this+8]`; C1/C2
zero both members and store the vtable+8, D1/D2 store the vtable+8 and return,
D0 does the same followed by `operator delete(this)`.

`CTsLocalImp::ProcessLastBytes(pbyKey, bEncrypt, dwBufLen, dwKeySize, pbyBuf)`
dispatches on the low three bits of `dwBufLen` (GCC codegen:
`edx = dwBufLen & ~7; eax = dwBufLen - edx; cmp eax,7; ja epilogue`), with
residue 0 = no-op and residue r = `BOX3Dr(pbyKey, dwKeySize, bEncrypt,
pbyBuf + (dwBufLen & ~7))`. The jump table is `.rodata` 0x25d420 (8 entries,
`target - GOT_base` with GOT base 0x294534); handler order in the binary is
7,1,6,5,4,3,2 after the code-1 entry, i.e. index r must map to BOX3Dr.

`CTsLocalImp::Seattos(key, benc, buf, buflen)` builds a 16-byte stack `CSeattos`
(C1 → `CSeattos::Seattos` → D1) and forwards the arguments **unchanged and in the
same order** (verified dynamically, see §3).

## 2. Static verification (symdiff / sizecmp)

```
g++ -m32 -r build/tmp/vm_engine/tencrypt_ts_local.o build/tmp/vm_engine/ts_local_imp.o \
    -o /mnt/d/tmp/vm_engine/tslocal_union.o
sh  symdiff.sh <ORIG> /mnt/d/tmp/vm_engine/tslocal_union.o 'TsLocal'
bash sizecmp.sh /mnt/d/tmp/vm_engine/tslocal_union.o 'TsLocal'
```

* `symdiff` — `ORIG-only` empty; `NEW-only` = `b g_sTsLocal`, which is the same
  file-local `.bss` pointer the ORIG has (`0x3267cc`, lowercase `b`); it is not a
  dynamic symbol in ORIG and therefore cannot show up as "matched".
* per-mangled-name sizes: **18 of 19 identical**, `ProcessLastBytes` 304 → 344
  (ratio 1.132, inside ±15 %). `sizecmp.sh` matches by demangled name only and
  pairs `D0` with `D1`, which is why it prints 4 "differing" rows; the
  per-mangled-name table is the meaningful one (`ts_local_matrix.tsv`).
* `ProcessLastBytes` size explanation: our symbol size includes the 32-byte PIC
  switch table that GCC 4.1 emits inside `.text` (`sub ecx,[ebx+eax*4+0x114]`);
  in the ORIG the identical table lives in `.rodata` (0x25d420) and is not
  charged to the symbol. Removing the table, 312 vs 304 bytes remain, and the
  prologue (`call __i686.get_pc_thunk.bx; add ebx,imm32`) is present in both.

## 3. Differential oracle (`recon/test_ts_local.cpp`)

Build (matching `recon/test_vm_engine.cpp` conventions):

```
g++ -m32 -O2 -fno-stack-protector -I recon -rdynamic -o build/tmp/vm_engine/ts_diff \
    recon/test_ts_local.cpp build/tmp/vm_engine/tencrypt_ts_local.o \
    build/tmp/vm_engine/ts_local_imp.o -ldl
./build/tmp/vm_engine/ts_diff        # PASSED: 418 checks, 0 failures
```

The harness `dlopen`s the ORIG and links `-rdynamic`, so the ORIG's PLT calls to
`BOX3D1..7` / `CSeattos::{CSeattos,Seattos,~CSeattos}` bind to the recording
trampolines in the test executable. Two independently useful properties follow:

1. **Interposition is active** (`interposition: active (1 BOX3D calls on probe)`) —
   the ORIG is *not* linked `-Bsymbolic` for these symbols; the assertion of the
   call arguments below therefore observes the ORIG's real call sites.
2. Each trampoline can forward to the ORIG's own exported `BOX3Dn` body
   (`dlsym`), so both implementations execute the *same* cipher code and their
   buffer results can be byte-compared.

Covered checks (all 418 pass):

* `ProcessLastBytes` — for 4 bases {0, 8, 16, 0x1000} × 8 residues: exact call
  count (0 for residue 0, else 1), variant `id == residue`, `a1 == pbyKey`,
  `a2 == dwKeySize`, `a3 == bEncrypt`, `a4 == pbyBuf + (dwBufLen & ~7)`;
  `bEncrypt == false` forwarded verbatim; in-bounds large length (0x3fffd)
  exercising the `& ~7` masking; plus ORIG-vs-us argument tuple parity and full
  256 KiB `pbyKey`/`pbyBuf` byte parity for 3 bases × 7 residues.
* `Seattos` — call sequence C1 → `Seattos` → D1 on the same stack object, with
  `data == key`, `key == buf`, `bEncrypt`, `buflen` identical for ORIG and ours.
* `LocalGetModuleHandle`/`LocalGetProcAddress` — both read +4/+8 on a raw blob.
* Constructors — C1 and C2 zero +4/+8 and set a non-null vptr; our object's vptr
  equals our `_ZTV11CTsLocalImp + 8`; other-object `operator new` size is 12.
* Vtables — `ITsLocal` slots 0..3 of both builds are pure-virtual stubs
  (all four equal, `__cxa_pure_virtual` in ours) and slots 4/5 are distinct
  D1/D0; `CTsLocalImp` slots 4/5 (offsets +0x10/+0x14) present and distinct.
* Singleton — `CreateTsLocal` is idempotent and allocates nothing while live;
  `FreeTsLocal` deletes exactly once (via the D0 slot) and repeated calls are
  no-ops; a later `CreateTsLocal` re-allocates exactly 12 bytes with the correct
  vptr. ORIG's own `CreateTsLocal`/`FreeTsLocal` show the same shape.

## 4. Link shape (`full_link.sh`)

`recon/link_check.sh` builds `/tmp/tss_recon_all.o` from a hard-coded TU list
owned by another unit; that list does **not** include the two new TUs (it must
not be edited here). The augmented object was produced with

```
g++ -m32 -r /tmp/tss_recon_all.o \
    build/tmp/vm_engine/tencrypt_ts_local.o build/tmp/vm_engine/ts_local_imp.o \
    -o /mnt/d/tmp/vm_engine/recon_plus.o
```

`full_link.sh` could not be re-run verbatim: `/tmp/tss_recon_all.o` and
`/tmp/fl_{have,orig,missing}.txt` are root-owned and not writable by this user,
so a byte-identical copy with only the output paths redirected was used
(`diff` against the original shows only those substitutions):

```
bash full_link_ts.sh /mnt/d/tmp/vm_engine/recon_plus.o /mnt/d/tmp/vm_engine/fl/tss_full_all.o
# orig=5636 have=7194 covered=5391 missing=245   (canonical /tmp run at the same time: missing=260)
```
All 15 previously missing TsLocal entries (`CTsLocalImp::*`, `CreateTsLocal()`,
`FreeTsLocal()`, `ITsLocal::~ITsLocal()`, both typeinfo/typeinfo-name/vtable
pairs) are absent from the new `fl_missing.txt` and present in `fl_have.txt`;
`grep TsLocal fl_missing.txt` returns nothing. No duplicate/conflicting
definitions appear in the linked object (C1/C2 and D1/D2/D0 are aliases as in
the ORIG; `W ITsLocal::~ITsLocal()` twice = the two weak copies).

**Pipeline note (needs the `link_check.sh` owner):** add
`src/VMCore/tencrypt_ts_local.cpp` and `src/VMCore/ts_local_imp.cpp` to the
`OBJS` list of `recon/link_check.sh`, and build them with the *vendor* toolchain
(`sh c5build.sh`, as used for every other reconstructed object here) rather
than that script's generic `g++` rule. Measured with the script's own flags
(system g++, `-O2 -fPIC -fno-implement-inlines -I . -I include`): both TUs
compile, but `ITsLocal::~ITsLocal()` is not emitted at all (the missing-list
entries would survive) and `CTsLocalImp::~CTsLocalImp` shrinks to 58/1/1 bytes
instead of the ORIG's 46/30/30, because GCC >= 4.6 drops out-of-line copies of
inline virtuals; adding `-fkeep-inline-functions` (the existing workaround for
the DP hacker TUs) brings the two `ITsLocal` weak copies back but leaves the
`CTsLocalImp` dtor aliases flattened. The c5build (vendor GCC 4.1.2) objects
shipped here reproduce every ORIG size and weak-ness exactly.

## 5. Caveats / non-goals

* No bodies for `BOX3D1..7` or `CSeattos::*` are provided or invented — they are
  declared extern in `tencrypt_ts_local.h` and already resolved by the existing
  Seattos/BOX3D CUs (they are not in the missing list).
* `recon/include/TenCrypt.h` (owned by another unit) declares the same
  `ITsLocal` with an out-of-line destructor. That version cannot emit the two
  weak dtor copies; my header is used instead and no TU includes both. No
  undefined `ITsLocal` references remain in the full link (`nm | grep ' U '`
  shows none), so the two definitions coexist harmlessly; should that header
  ever be included together with `tencrypt_ts_local.h`, its `virtual ~ITsLocal();`
  should become `virtual ~ITsLocal() {}`.
* `cryptdata.cpp` and the previously delivered VM core files were not touched.

## 6. Reproduction commands

```
cd tools/tss_sdk_recon
sh c5build.sh recon/src/VMCore/tencrypt_ts_local.cpp build/tmp/vm_engine/tencrypt_ts_local.o
sh c5build.sh recon/src/VMCore/ts_local_imp.cpp       build/tmp/vm_engine/ts_local_imp.o
g++ -m32 -r build/tmp/vm_engine/{tencrypt_ts_local,ts_local_imp}.o -o /mnt/d/tmp/vm_engine/tslocal_union.o
sh symdiff.sh /opt/dnf1031/.../libtss_sdk.so.1.0.394 /mnt/d/tmp/vm_engine/tslocal_union.o 'TsLocal'
bash sizecmp.sh /mnt/d/tmp/vm_engine/tslocal_union.o 'TsLocal'
g++ -m32 -O2 -fno-stack-protector -I recon -rdynamic -o build/tmp/vm_engine/ts_diff \
    recon/test_ts_local.cpp build/tmp/vm_engine/tencrypt_ts_local.o \
    build/tmp/vm_engine/ts_local_imp.o -ldl && ./build/tmp/vm_engine/ts_diff
```

Results: `c5build` rc=0 for both TUs, `symdiff` ORIG-only empty, sizes
18/19 identical (1 at 1.132, explained), oracle 418/418 checks pass,
`full_link` missing list loses every TsLocal entry.
