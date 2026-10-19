# AGENT_REPORT_buddy_alloc — comm_buddy_alloc.cpp

CU: `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/commlib/framework/comm_buddy_alloc.cpp`
ORIG: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394` (ELF32 i386, DWARF), CU code range `0xd2db0-0xd58f0`.
Compiler: vendor GCC 4.1.2 through `sh c5build.sh <src> <out.o>`.

## Artifacts

| file | role |
|---|---|
| `recon/src/commlib/comm_buddy_alloc.h` | class/constant/type reconstruction (`BuddyBitSet`, `BuddyBufDesc`, `BuddyAlloc`, `MultiBuddyAlloc`, `BuddyPointerHashFun`, the two `hash_map` typedefs) |
| `recon/src/commlib/comm_buddy_alloc.cpp` | implementation; `#include "src/commlib/comm_buddy_alloc.h"` only |
| `recon/src/commlib/test_buddy_alloc_oracle.cpp` | differential oracle (dlopen ORIG, run both implementations over independent equal-size buffers, compare) |
| `recon/build/buddy/run_oracle.sh` | build ORIG-compatible TU + oracle with the vendor g++ and run it (release, `-O2`) |
| `recon/build/buddy/build_dbg.sh` | same, `-O0 -g` oracle object (kept for triage; results identical) |
| `recon/src/commlib/AGENT_REPORT_buddy_alloc_matrix.tsv` | per-symbol size matrix (51 rows) |

Build status: `sh c5build.sh recon/src/commlib/comm_buddy_alloc.cpp recon/build/buddy/comm_buddy_alloc.o` -> rc=0, no warnings.

## Layout facts recovered from DWARF and confirmed by assertions in the oracle

```
sizeof(BuddyBitSet)=8   sizeof(BuddyBufDesc)=32   sizeof(BuddyAlloc)=476 (0x1dc)   sizeof(MultiBuddyAlloc)=108 (0x6c)
```

`sizeof(BuddyAlloc)==0x1dc` is what `UicUserInputInfoMgr::init` allocates, so the layout is compatible with the
external declaration in `recon/src/formmog/tss_sdk/plugins/proto_proc/uic_manager_agent.cpp` (that file was not touched).

Constants: `BUDDY_MIN_BASE_SIZE=16`, `BUDDY_BUF_DESC_POOL_MAX_NUM=100000`, `BUDDY_FREE_CLASSIFY_MAX_NUM=32`,
`BUDDY_TAOTAL_SIZE_MAX_INDEX=32`, `BUDDY_UNSIGNED_INT_MAX=0xffffffff`,
`MULTIBUDDY_{DEFAULT_STEPS=4,BUDDY_POOL_MAX_NUM=8,BASE_SIZE_START=16,MAX_BASE_SIZE=32,BASE_SIZE_MAX_DISTANCE=16,BUDDY_BUFFER_INTERVAL=4,TAOTAL_SIZE_MAX_INDEX=30}`.
`BuddyAlloc::init(unsigned int* size, unsigned int base_size, bool lack_use_system)` is called by the UIC manager with
size `0x200000`, base `0x10`, flag `true`; that exact call is part of the oracle.

## Symbols and sizes

`python3 recon/build/buddy/cmp_syms2.py <ORIG> 0xd2db0 0xd58f0 recon/build/buddy/comm_buddy_alloc.o`:

```
ORIG funcs in [0xd2db0,0xd58f0): 51   NEW funcs: 85
MISSING (0):
functions outside +-15%: 0
```

All 51 DWARF-attributed functions of the CU are emitted with identical mangled names, identical emission order and
sizes inside +-15% (40 of them byte-identical, e.g. all `BuddyBitSet` methods, both ctors/dtors of both classes,
`init_bitset`, `fini_bitset`, `modify_bitset`, `find_buddy`, `print_info`, `get_ptr_desc`, the free-list helpers).
Largest ratio in range: `BuddyAlloc::alloc` 633/723 = 0.876 (see `recon/build/buddy/cmp_syms_final.txt` and
`.../cmp_dis.py`, which shows the control flow of both versions is the same; the gap is instruction scheduling).
Per-symbol matrix: `AGENT_REPORT_buddy_alloc_matrix.tsv`.

The other 34 functions in the object are the weak STL instantiations that GCC emits for this TU (`std::fill_n`,
`std::fill`, `__uninitialized_fill_n_a`, `hashtable::clear`, `hashtable::resize`, `find_or_insert`,
`_M_fill_insert`, `std::lower_bound`, `__i686.get_pc_thunk.bx`). They exist in the ORIG as well, but outside the CU
address range (the linker puts MW/weak template copies in a shared pool), so they are not part of the 51-row matrix.
Measured differences there:

| symbol | ORIG | recon | note |
|---|---|---|---|
| `std::lower_bound<unsigned long const*, unsigned long>` | 0x43 | 0x43 | identical |
| `std::fill_n<_Hashtable_node<...>**, unsigned, ...* const&>` (x3) | 0x20 | 0x2b | dead copy: no call site in the CU's own code |
| `hashtable<...>::clear()` (x3) | 0x7b | 0x98 | reached via PLT from `init`/`fini`; same instruction sequence, see below |

`clear()` disassembly comparison (`objdump`, ORIG `0xd5ce0`): both versions push ebp/edi/esi/ebx, walk
`_M_buckets` from `_M_buckets[0]` to `_M_buckets_end`, free each node chain with `operator delete`, null the bucket and
store `_M_element_count = 0`. The recon copy is 29 bytes larger only because GCC rotated the loop (bound recomputed at
the bottom instead of jumping to the top test) and inserted a 6-byte `lea 0x0(%esi),%esi` alignment pad. Behaviour is
identical, and it is exercised by the oracle's `init`/`fini` paths (map sizes return to the ORIG's values).

## Differential oracle

`sh recon/build/buddy/run_oracle.sh` (builds with the vendor g++ for the allocator TU, links with the c5root static
`libstdc++.a`/`libgcc` because c5root ships no linkable dynamic libc, links the host i386 glibc + `-ldl`, runs with an
empty `LD_LIBRARY_PATH`). The ORIG `libtss_sdk.so.1.0.394` is `dlopen`ed with `RTLD_NOW|RTLD_LOCAL` and every method of
the CU is resolved with `dlsym(mangled_name)`; `ZEN_Trace_LogMsg::debug_errorex/debug_debugEx` are exported by the test
executable (`-rdynamic`) so the ORIG's PLT calls land in the test's logging stubs and their message text can be
compared. Both implementations get their own independently allocated, equally sized buddy buffers, so the comparison is
done on *relative offsets*, never on absolute addresses.

Observed results (release build, `-O2`, and the `-O0 -g` build identical):

```
sizeof(BuddyBitSet)=8 sizeof(BuddyBufDesc)=32 sizeof(BuddyAlloc)=476 sizeof(MultiBuddyAlloc)=108
after init: buffer_len=2097152 need_order=17 max_size=2097152 base=16 chain_order0_is_empty=1
alloc phase: 421 live blocks, size_used=1858348 size_allocated=2097040 alloc_times=421 alloc_split=92
print_info (live state) produced 1 orig / 1 recon message(s); text identical (296 bytes)
all freed: size_used=0 size_allocated=0 free_times=421 free_merge=92 map_n=332 max_size=2097152 max_order=17
invalid-input messages: orig 5, recon 5
fini after init(0x10000,16) [zeroed tail]  orig: exit=8 sig=0  | recon: exit=8 sig=0
fini after init(0x10000,16) [garbage tail] orig: exit=0 sig=11 | recon: exit=0 sig=11
get_next_order_and_power sweep: 2097217 sizes, 0 mismatches
init(NULL) fresh                 orig: exit=0 sig=11 | recon: exit=0 sig=11
init(size=0) fresh               orig: exit=0 sig=11 | recon: exit=0 sig=11
init(size=8) fresh               orig: exit=0 sig=11 | recon: exit=0 sig=11
init(base=4) fresh               orig: exit=0 sig=0  | recon: exit=0 sig=0
init(size=0) after valid init    orig: exit=0 sig=11 | recon: exit=0 sig=11
descriptor pool drain: orig count=99840 signal=11 ; recon count=99840 signal=11
multi after init: size=2883584 buffer_len=2883600 steps=4 max_order=15 buddy_map_n=0 sys_map_n=0
multi print_info produced 1 orig / 1 recon message(s); text identical (343 bytes)
multi fini after init(0x200000,4) orig: exit=0 sig=11 | recon: exit=0 sig=11

checks=2106605 mismatches=0
ORACLE RESULT: PASS
```

What the 2,106,605 checks cover:

1. layout assertions and `count_total_size` on both sides for a 0x200000 buffer;
2. a deterministic alloc sequence of ~2000 sizes (1..0x200000, powers of two and ±1 neighbours): returned *offsets*
   inside the buffer, NULL/`-2` (system) classification, and after every 41st allocation a full state comparison —
   `buffer_*`, `size_used_`, `size_allocated_`, `alloc_times_`, `alloc_split_series_`, `free_times_`, `free_merge_series_`,
   `need_order_`, both `max_*`, `ptr_area_size_`, all 32 `classify_bitset_` bitmaps, every free-list chain and every
   cached descriptor structure (items compared by name, mismatch text printed for any difference);
3. the live `print_info` output byte-for-byte (296 bytes identical) — this covers `size_used_/size_allocated_`,
   per-order block counts, disabled ranges and the free-list dump;
4. the free phases (in-order, reverse, and mixed), including the *system* (heap) allocation free path
   (`self_ptr_additional_`), ending with `size_used_==0`, `size_allocated_==0` and identical counters;
5. invalid-input messages (5 on both sides: NULL pointer, size 0, unaligned size, size > max, ...) compared as text;
6. `get_next_order_and_power` swept over 2,097,217 sizes (1..0x200000), 0 mismatches;
7. failing `init` inputs (NULL size pointer, size 0, size 8, base 4, size 0 after a valid init) compared in forked
   children by exit code *and* signal, because the ORIG then walks uninitialised state;
8. descriptor-pool exhaustion: both implementations die with SIGSEGV (signal 11) after exactly 99840 16-byte
   allocations when the last descriptor leaves the pool (see the quirk below); the fuzzed child writes its progress
   counter to disk every 256 allocations so a crash still reports how far it got;
9. `MultiBuddyAlloc`: `init(0x200000, 4, true)`, 128 allocations spread over the four pools (offsets and per-pool
   state compared with the same comparator), the live `print_info` text (343 bytes identical), the system-allocation
   fallback for a size above `16 << buddy_max_order_` plus its free path and counters, error messages for
   `free(garbage)`/`free(NULL)`, the state after the frees, and `fini` compared in forked children.

## Deliberately preserved ORIG quirks

* `get_desc_obj_from_pool()` dereferences the new list head unconditionally (`desc_obj_pool_->pre_ = NULL`) — the
  process faults when the descriptor pool is drained; reproduced (identical crash point, 99840 allocations).
* `get_block_index_by_order()` returns 0 on success and emits the two swapped error messages.
* `free()` erases the `ptr_buddy_map_` entry keyed by `desc->ptr_` *after* that field has been set to NULL, so the
  stale entry stays in the map (reproduced: `map_n=332` after the whole alloc phase was freed).
* `BuddyAlloc::fini()` walks `classify_bitset_` slots from `need_order_` upwards, i.e. over uninitialised memory when
  the object did not come from a zeroed allocation (segfault with garbage tails, clean return with zeroed tails).
* `MultiBuddyAlloc::count_total_size()`/`init()` size arithmetic including the `BUDDY_BUFFER_INTERVAL` padding and the
  `size % steps` / `remain / chunk` pool-selection modulo logic.
* `alloc()` never zeroes memory and rounds sizes up to `base_size_` exactly as the ORIG does.

## How to re-run

```
cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon
sh c5build.sh recon/src/commlib/comm_buddy_alloc.cpp recon/build/buddy/comm_buddy_alloc.o   # rc=0
sh recon/build/buddy/run_oracle.sh                                                          # -> ORACLE RESULT: PASS
python3 recon/build/buddy/cmp_syms2.py /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394 \
        0xd2db0 0xd58f0 recon/build/buddy/comm_buddy_alloc.o                                # 0 missing, 0 out of +-15%
```

Caveat: the oracle links the c5root *static* libstdc++ into the test executable; the ORIG's own `libstdc++` symbols
are resolved from the shared library it was built with, which is exactly what happens in the real game, and all
compared structures (`std::vector`/`hash_map` members of both classes) are also compared field by field, so the
different II version cannot mask a difference in the CU's own code.
