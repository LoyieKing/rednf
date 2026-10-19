# Weak-symbol gaps: IDPHacker / IDPHackerSafe / DPPkgDelayHacker (2026-10-18)

Status: **closed**. The seven symbols named in the ticket are defined in
`/tmp/tss_recon_all.o` and are covered by
`sh coverage.sh /tmp/tss_recon_all.o "^(tss_sdk_impl|tss_dpsdk)::"`.
Ticket symbols (all `W`):

```
tss_dpsdk::IDPHacker::init(IDPHackerEventNotify*)
tss_dpsdk::IDPHacker::on_logout(UserInfo*)
tss_dpsdk::IDPHacker::proc(long)
tss_dpsdk::IDPHacker::on_special_control(UserInfo*, unsigned char*, unsigned int&, unsigned char*, unsigned int&)
tss_dpsdk::IDPHacker::~IDPHacker()                     (D0 + D1)
tss_dpsdk::IDPHackerSafe::~IDPHackerSafe()             (D0 + D1)
tss_dpsdk::DPPkgDelayHacker::~DPPkgDelayHacker()       (D0 + D1)
```

## 1. ORIG emission points (verified now, `dwarf_addr_cu.py` on ORIG)

| ORIG addr | size | symbol | CU (ORIG) | header line |
|---|---|---|---|---|
| 0x000a4770 | 0x7 | `IDPHacker::init` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:27` |
| 0x000a4780 | 0x7 | `IDPHacker::on_login` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a4790 | 0x7 | `IDPHacker::on_logout` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47a0 | 0x7 | `IDPHacker::proc` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47b0 | 0x7 | `IDPHacker::on_ban_mode` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47c0 | 0x7 | `IDPHacker::on_drop_cmd` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47d0 | 0x7 | `IDPHacker::on_verify_response` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47e0 | 0x7 | `IDPHacker::on_verify_msg` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a47f0 | 0x7 | `IDPHacker::on_safe_mode` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a4800 | 0x7 | `IDPHacker::on_special_control` | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:51` |
| 0x000a4810 | 0x2e | `IDPHacker::~IDPHacker` D0 | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:55` |
| 0x000a4840 | 0x1e | `IDPHacker::~IDPHacker` D1 | `dp_cf_extract_user_info.cpp` | `dp_hacker_if.h:55` |
| 0x000a9930 | 0x60 | `IDPHackerSafe::~IDPHackerSafe` D0 | `dp_delay_hacker.cpp` | `dp_hacker_safe_if.h:21` |
| 0x000a9990 | 0x58 | `IDPHackerSafe::~IDPHackerSafe` D1 | `dp_delay_hacker.cpp` | `dp_hacker_safe_if.h:21` |
| 0x000a99f0 | 0x123 | `DPPkgDelayHacker::init` | `dp_delay_hacker.cpp` | `dp_pkg_delay_hacker.h:28` |
| 0x000b6480 | 0xa2 | `DPPkgDelayHacker::~DPPkgDelayHacker` D0 | `dp_pkg_delay_hacker.cpp` | `dp_pkg_delay_hacker.h:26` |
| 0x000b6530 | 0xaa | `DPPkgDelayHacker::~DPPkgDelayHacker` D1 | `dp_pkg_delay_hacker.cpp` | `dp_pkg_delay_hacker.h:26` |

ORIG emits **no** constructor symbol for these three classes (no
`IDPHacker::IDPHacker`, no `IDPHackerSafe::IDPHackerSafe` in `nm -DC`), i.e.
the ctors are implicit while every non-pure virtual and the destructor are
inline header bodies. TU mapping used below:
`dp_cf_extract_user_info.cpp` -> `bill_cf_hackers_agent.cpp`,
`dp_delay_hacker.cpp` -> `hackers_a_agent.cpp`,
`dp_pkg_delay_hacker.cpp` -> `hackers_c_agent.cpp`.

## 2. Root cause

1. `bill_cf_hackers_agent.cpp` declared `IDPHacker`'s slots **without bodies**
   (`virtual int init(...); ...`), so the CF TU had nothing to emit: it only
   held undefined references (`U tss_dpsdk::IDPHacker::init` ...) coming from
   its four derived vtables.
2. GCC >= 4.6 does not emit out-of-line copies of inline functions unless their
   address is required, and for an **abstract** class (`IDPHacker`,
   `IDPHackerSafe`, `DPPkgDelayHacker` are abstract: pure `hack`/`on_timer`) it
   emits the vtable with **empty destructor slots** - verified with
   `readelf -rW` on the emitted `_ZTVN9tss_dpsdk13IDPHackerSafeE`
   (`hackers_c_agent.cpp`): entries exist for `+0x08..+0x30`, and the D1/D0
   offsets `+0x34/+0x38` carry no relocation at all. Probe matrix (compiled
   with `-O2`, with and without `-fno-implement-inlines`): defining a concrete
   derived class with its key function in the TU, a file-scope static
   instance, a heap instance (`new`) and a local instance whose address
   escapes all leave the base/abstract-class destructors without an
   out-of-line copy. `-fno-inline` was rejected (it changes the code of the
   whole TU), `-fkeep-inline-functions` was used instead because it only adds
   COMDAT copies. GCC 4.1.x emitted those copies eagerly, which is why the
   ORIG has them, and why the c5 build below needs no flag.

## 3. Changes

1. `recon/src/formmog/tss_sdk/plugins/proto_proc/bill_cf_hackers_agent.cpp`
   - `IDPHacker` now carries the ORIG header bodies (all default slots
     `{ return 0; }`, inline `~IDPHacker()`). No symbol added or removed
     otherwise; the four CF vtables and all `hack` bodies are unchanged.
2. `recon/src/formmog/tss_sdk/plugins/proto_proc/hacker_base_agent.cpp`
   - dropped the explicit empty `IDPHacker() {}` / `IDPHackerSafe() {}`: the
     ORIG emits no constructor symbol for these classes, and with the flag
     below those two ctors would have become 4 extra weak symbols. The
     implicit default ctors call `ZEN_Thread_Mutex(0)` / set up the vptr
     exactly as before (layout guards unchanged, still 4/32/84 bytes).
3. `recon/link_check.sh`
   - `bill_cf_hackers_agent.cpp` and `hacker_base_agent.cpp` are compiled with
     `-fkeep-inline-functions` (in addition to `-fno-implement-inlines`) with
     an inline comment explaining why. Reason: these are the two TUs that carry
     the ORIG header bodies under test with a modern compiler, and per point 2
     no source structure can make GCC 13 emit the three inline destructors. The
     vendor c5 build (GCC 4.1.2) does **not** need the flag and was left
     unchanged.

## 4. Verification (all commands actually run)

* `cd recon && sh link_check.sh /tmp/tss_recon_all.o` -> `rc=0`.
* `sh coverage.sh /tmp/tss_recon_all.o "^(tss_sdk_impl|tss_dpsdk)::"`
  - before: `counts orig=347 recon=477 covered=333`, 14 ORIG-only (the 7 ticket
    names, the `std::fill_n<IDPHacker**,...>` weak copy and 6 channel/callback
    symbols owned elsewhere);
  - after: `counts orig=347 recon=503 covered=346`, ORIG-only = **1** entry
    (`std::fill_n<...>`, see section 5). None of the seven names is missing.
* coverage delta (before/after report diff, regex-filtered):
  - ORIG-only entries that disappeared: the seven ticket names (plus the six
    channel/callback names a parallel ticket closed);
  - ORIG-only entries that appeared: none;
  - new recon-only `tss_dpsdk`/`tss_sdk_impl` entries introduced by flagging
    `bill_cf`: 8, all `W` -
    `DPBillAdapter::operator<< <unsigned char|unsigned int|unsigned short>`,
    `DPBillRecorder::DPBillRecorder()`,
    `std::__addressof<DPPkgDelayHacker::DPCommandBuf>`,
    `UserInfo::{check,clear,set}_user_flag`. They are the TUs own inline
    helpers that GCC 13 only emits with `-fkeep-inline-functions`; every other
    extra is a `std::`/`__gnu_cxx` weak copy that the coverage regex filters
    out.
* `nm -C /tmp/tss_recon_all.o | grep -E " [WT] tss_dpsdk::IDPHacker"` ->
  `init, on_login, on_logout, proc, on_ban_mode, on_drop_cmd,
  on_verify_response, on_verify_msg, on_safe_mode, on_special_control,
  ~IDPHacker (x3)`, plus `IDPHackerSafe::~IDPHackerSafe (x3)`, all `W`.
* c5/vendor toolchain (`sh c5build.sh ...`, no extra flags):
  - `bill_cf_hackers_agent.o` now defines **all 12** `IDPHacker` copies as `W`
    with ORIG sizes: 10 slots `0x7`, `D1Ev 0x1e`, `D0Ev 0x2e`, plus
    `vtable/typeinfo for IDPHacker`. `IDPHacker::init` disassembles to
    `55 31 c0 89 e5 5d c3`, byte-identical to ORIG 0xa4770; the D1/D0 bodies
    are the ORIG sequences (vptr store, `operator delete` in D0).
  - `hacker_base_agent.o`: `IDPHackerSafe::{D1Ev 0x58, D0Ev 0x60}` and
    `DPPkgDelayHacker::{D1Ev 0xa2, D0Ev 0xaa}` - identical to ORIG 0xa9990 /
    0xa9930 and 0xb6480 / 0xb6530. Pre/post symbol sets of this TU are
    identical (30 entries, no added/removed symbol).
  - `bill_cf_hackers_agent.o` pre/post diff: only added symbols (the 12 weak
    copies + IDPHacker vtable/typeinfo); every pre-existing symbol, including
    `DPCfExtractUserInfo::hack 0xb1a`, has the same size as before.
* g13 (`link_check.sh` compiler): `bill_cf` alone yields the 10
  vtable-referenced slots; with the two flagged TUs the linked object carries
  all 12 slots and the three destructors as `W`.
* Added-symbol audit (`readelf -sW` + `nm`, whole-link diff):
  - `hacker_base` flagged only: +127 symbols vs baseline = 92 `W`, 1 `V`,
    34 local COMDAT section aliases (`n`);
  - `bill_cf` + `hacker_base` flagged (what link_check.sh does): +138
    symbols = 103 `W`/`V` + 35 local aliases;
  - no symbol is ever removed, and **there is no new external non-weak
    symbol** in either configuration (0 symbols with `T/D/B/R/G` binding).
    The only strong extras of the affected TUs are `__i686/__x86.
    get_pc_thunk.*` (`FUNC GLOBAL`, `HIDDEN` under g13), which the pre-change
    objects already carried (ORIG keeps its thunks local, `nm` shows `t`).
    `sh extra_globals.sh <tu.o>` on the c5 objects lists only
    `DW.ref.__gxx_personality_v0` (`V`), those two thunks, and no new
    entry vs. the pre-change objects.

## 5. Residual / adjacent finding (not part of the seven names)

The only remaining ORIG-only symbol of the coverage regex is the libstdc++ 4.1
pointer-form instantiation emitted by the hack-manager CU:

```
000abc30 W _ZSt6fill_nIPPN9tss_dpsdk9IDPHackerEjS2_ET_S4_T0_RKT1_
          tss_dpsdk::IDPHacker** std::fill_n<IDPHacker**, unsigned int,
                                            IDPHacker*>(...)
```

`dwarf_addr_cu.py` puts it in CU `dp_hack_mgr.cpp` (`stl_algobase.h:636`), i.e.
our `dp_hack_mgr_agent.cpp`, whose `vector<IDPHacker*>` fill path does not
instantiate the 4.1 pointer-form `std::fill_n` any more (verified: `nm
dp_hack_mgr_agent.o | grep fill_n` is empty for both the c5 and the host-g++
build). A 5-line explicit instantiation in that TU reproduces the ORIG mangled
name exactly (probe compiled with both toolchains, `W` in both):

```cpp
typedef tss_dpsdk::IDPHacker *IDPHackerPtr;
template IDPHackerPtr *std::fill_n<IDPHackerPtr *, unsigned int, IDPHackerPtr>(
    IDPHackerPtr *first, unsigned int n, IDPHackerPtr const &value);
```

It was **not applied**: the ticket names seven symbols and this one is a
different (libstdc++-template) gap in another TU.

## 6. Files changed

```
recon/src/formmog/tss_sdk/plugins/proto_proc/bill_cf_hackers_agent.cpp
recon/src/formmog/tss_sdk/plugins/proto_proc/hacker_base_agent.cpp
recon/link_check.sh
```
