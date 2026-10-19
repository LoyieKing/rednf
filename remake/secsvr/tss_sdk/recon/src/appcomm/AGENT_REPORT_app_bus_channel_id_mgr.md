# AGENT REPORT -- `app_bus_channel_id_mgr.cpp` (BusChannelIdMgr)

| item | value |
|---|---|
| original CU | `src/formmog/appcomm/app_bus_channel_id_mgr.cpp` (path in ORIG: `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/...`) |
| ORIG | `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394` (ELF32 i386, GCC 4.1.2, DWARF) |
| code range | `0x0018c930` - `0x0018d29e` (18 symbols) |
| reconstruction | `recon/src/appcomm/app_bus_channel_id_mgr.cpp` + `recon/src/appcomm/app_bus_channel_id_mgr.h` |
| matrix | `recon/src/appcomm/AGENT_MATRIX_app_bus_channel_id_mgr.tsv` |
| build | `sh c5build.sh recon/src/appcomm/app_bus_channel_id_mgr.cpp build/tmp/app_bus_channel_id_mgr.o` -> rc 0 |
| evidence dumps | `build/tmp/tb_evidence.txt` (`cu_symbols.py`, DWARF function/variable dumps, `objdump -dl -C`) |

## 1. Symbols (ORIG `nm -S`, including the compiler-generated ones)

| address | size | symbol | src lines |
|---|---|---|---|
| 0x0018c930 | 0x24 | `BusChannelIdMgr::BusChannelIdMgr()` [C1] | 51-58 |
| 0x0018c960 | 0x24 | `BusChannelIdMgr::BusChannelIdMgr()` [C2] | 51-58 |
| 0x0018c990 | 0x12 | `BusChannelIdMgr::count_channel_info_size() const` | 354-357 |
| 0x0018c9b0 | 0x2d | `BusChannelIdMgr::alloc_bus_channel_id_from_mem(unsigned int)` | 359-371 |
| 0x0018c9e0 | 0x3a | `BusChannelIdMgr::find_bus_channel_id_from_mem(unsigned int)` | 373-385 |
| 0x0018ca20 | 0x5c | `__static_initialization_and_destruction_0(int, int)` | - |
| 0x0018ca80 | 0x10 | `global constructors keyed to BusChannelIdMgr::BusChannelIdMgr()` (`_GLOBAL__I__ZN15BusChannelIdMgrC2Ev`) | - |
| 0x0018ca90 | 0x26 | `__tcf_0` | - |
| 0x0018cac0 | 0x9f | `write_channel_info_to_file_without_lock(FILE*)` | 332-352 |
| 0x0018cb60 | 0x80 | `read_channel_info_from_file_without_lock(FILE*)` | 312-330 |
| 0x0018cbe0 | 0x88 | `alloc_bus_channel_id_according_pid(unsigned int)` | 139-165 |
| 0x0018cc70 | 0x11c | `get_bus_channel_id()` | 168-215 |
| 0x0018cd90 | 0x10c | `get_bus_channel_id(unsigned int)` | 95-136 |
| 0x0018cea0 | 0x19d | `init_bus_channel_info_list()` | 270-310 |
| 0x0018d040 | 0xbb | `init(unsigned int)` | 72-92 |
| 0x0018d100 | 0xd8 | `free_svr_id()` | 218-267 |
| 0x0018d1e0 | 0x60 | `BusChannelIdMgr::~BusChannelIdMgr()` [D1] | 60-69 |
| 0x0018d240 | 0x60 | `BusChannelIdMgr::~BusChannelIdMgr()` [D2] | 60-69 |

`bool is_valid_channel_id(int)` is declared in the class (DWARF line 57) but has
**no definition anywhere in the library** (no symbol), so it is only declared in
the reconstructed header, exactly like in the ORIG.

The three compiler-generated symbols are reproduced with **identical names and
sizes** (`__static_initialization_and_destruction_0` 0x5c, `_GLOBAL__I_...` 0x10,
`__tcf_0` 0x26) because the TU keeps the original's `#include <iostream>`
(DWARF variable `std::__ioinit` at 0x002da358, size 1).

## 2. Class layout (DWARF, `sizeof == 24`, non-polymorphic)

```
+0x00 uint32_t         used_svr_id_
+0x04 bool             is_init_
+0x08 unsigned int     max_channel_id_     <- the ctor never writes it
+0x0c uint32_t *       channel_info_
+0x10 ZEN_File_Lock *  lock_
+0x14 bool             is_use_pid_
```

Accessibility from DWARF: ctor/dtor/`init`/`get_bus_channel_id` x2 are public;
`init_bus_channel_info_list`, read/write, `count_channel_info_size`, find,
alloc x2, `free_svr_id`, `is_valid_channel_id`, the members and the private
copy ctor/assignment (lines 78/79) are private - reproduced one to one.

The ctor (lines 53-57) stores 0 into `used_svr_id_`, `is_init_`, `lock_`,
`channel_info_`, `is_use_pid_` - in exactly that address order - and leaves
`max_channel_id_` alone.

## 3. File format and the two path globals

* File layout: 4-byte header = `used_svr_id_` (the object's first dword), then
  `channel_info_[0 .. max_channel_id_)`, i.e. `count_channel_info_size()` ==
  `4 + max_channel_id_ * 4` (`lea 0x4(,%eax,4),%eax`, 0x18c99a).
* `TSS_SVR_ID_POOL_FILE` @0x00292350 (local `const char *const`, DWARF line 16)
  -> `"/tmp/tss_sdk_game_svr_id_pool_file"` @0x227f70
* `TSS_SVR_ID_POOL_LOCK_FILE` @0x00292354 (local `const char *const`, DWARF line 17)
  -> `"/tmp/tss_sdk_game_svr_id_pool_file.lock"` @0x227f94
* mode strings: `"rb+"` @0x227f13, `"wb+"` @0x227f17.
* error strings: `"open %s fail. err=%d:%s"` @0x227f1b,
  `"open lock file fail. %s"` @0x227f33,
  `"write init data to %s fail. ret=%d"` @0x227f4c.

`init()` opens the **lock file** (0x18d073 loads 0x00292354) with
`open(path, 0x42, 0x1b0)` = `O_RDWR|O_CREAT, 0660`, and reports the lock-file path
in its error message. The pool file is only ever opened with `fopen`.

## 4. Shipped behaviour reproduced verbatim (do not "fix")

1. The default ctor leaves `max_channel_id_` (+0x08) uninitialised.
2. `read_channel_info_from_file_without_lock` / `write_...` use the object's
   first dword (`used_svr_id_`) as the 4-byte file header, not `max_channel_id_`
   (`fwrite(this, 4, 1, fp)` at 0x18cb0c, `fread(this, 4, 1, fp)` at 0x18cb90).
3. `write` returns -1 when the first `fwrite` fails and -2 when the second one
   fails (`lea -0x2(%edx,%edx,1),%edx` at 0x18cb59); `read` mirrors it with
   `<= 0` / `> 0` tests.
4. `init()` leaks `lock_` (and never allocates `channel_info_`) when the lock file
   cannot be opened: it logs and returns -2 without `delete`.
5. `init_bus_channel_info_list()` does not `fclose()` the freshly created `wb+`
   file when the initial write fails (returns -2 with the file still open).
6. `get_bus_channel_id()` returns -4 when `is_init_` is false, while
   `get_bus_channel_id(unsigned)` returns -1; both return -2 when the pool file
   cannot be opened and -3 when the read fails.
7. `alloc_bus_channel_id_according_pid` treats a pool slot as reusable whenever
   `kill(pid, 0)` returns -1, i.e. both ESRCH (no such process) and EPERM (not
   our process) free the slot; a slot is only kept when the probe succeeds
   (`rc == 0`). The helper `is_process_active(pid_t)` (DWARF line 21, local `rc`
   at line 38) is `int rc = kill(pid, 0); return rc != -1;`
   (`add $0x1,%eax; jne` at 0x18cc4e) and is inlined at 0x18cc38..0x18cc51.
8. `free_svr_id()` clears `channel_info_[id - 1]`, and shrinks `used_svr_id_`
   only when the cleared slot was the last used one.
9. The guard used around all file access is `ZEN_Write_Guard<ZEN_File_Lock>`
   (`zen_lock_guard.h` line 148, sizeof 4, base `ZEN_NON_Copyable`) whose ctor
   (line 151, body line 154) calls **`ZEN_File_Lock::lock_read()`** - a *shared*
   flock (the model in `recon/src/commlib/zenlib/zen_lock_file_lock.h` calls
   `flock(fd, 1)`) - although the class is named "write" guard; its dtor (line
   170) calls `unlock()`. All three sites (0x18cccc, 0x18cec2, 0x18d132) inline
   the same pair of calls, `lock_write()` is never used through the guard here.
   Since `zen_lock_guard.h` has not been reconstructed, the guard model was put
   in the .cpp (it belongs to `commlib/zenlib/zen_lock_guard.h`; the real header
   also declares `ZEN_Write_Guard(LOCK&, bool)` at line 158, whose body is not
   observable from this CU, and `lock_write()`/`try_lock_write()`/`unlock()` at
   lines 174/179/185).

## 5. Build note: `_FILE_OFFSET_BITS=64`

The ORIG TU was compiled with LFS: it calls `fopen64` (0x18ccc6, 0x18d141,
0x18cef2, 0x18cf75) and `ZEN_OS::stat()` receives a 96-byte `struct stat`
(`zen_os_stat`, `st_size` at +44 - exactly the CU DWARF layout; the default
32-bit `struct stat` of this toolchain is 64 bytes with `st_size` at +32).
`recon/src/appcomm/app_bus_channel_id_mgr.cpp` therefore starts with
`#define _FILE_OFFSET_BITS 64` (the same convention as
`dp_code_mgr_agent.cpp`), so the plain `c5build.sh` command line produces the
correct object. `ZEN_OS::stat` is declared locally
(`_ZN6ZEN_OS4statEPKcP4stat`, ORIG 0x001a2570) and `zen_os_stat` is
`typedef struct stat zen_os_stat` (as in the generated
`zen_os_adapt_predefine.h`: `typedef stat zen_os_stat;`).

## 6. Verification log

```
$ sh c5build.sh recon/src/appcomm/app_bus_channel_id_mgr.cpp \
      build/tmp/app_bus_channel_id_mgr.o
rc=0
$ nm -S build/tmp/app_bus_channel_id_mgr.o     # 18 expected + __ioinit, no is_process_active
$ sh symdiff.sh <ORIG> <obj> 'BusChannelIdMgr'
== ORIG-only ==            (empty)
== NEW-only ==
t global constructors keyed to BusChannelIdMgr::BusChannelIdMgr()
== counts (orig new) == 13 / 14
   (the single NEW-only entry is the local `_GLOBAL__I_` symbol; the ORIG has it
    as a local symbol of the same name/size too - it is only compared against
    the dynamic symbol table)
$ sh sizecmp.sh <obj> 'BusChannelIdMgr'
matched=15 identical=9 differing=6
    284    275  0.968  get_bus_channel_id()
    159    156  0.981  write_channel_info_to_file_without_lock
    268    265  0.989  get_bus_channel_id(unsigned int)
    128    130  1.016  read_channel_info_from_file_without_lock
    413    431  1.044  init_bus_channel_info_list
    216    234  1.083  free_svr_id
$ sh extra_globals.sh <obj>      # only DW.ref.__gxx_personality_v0 and
                                 # __i686.get_pc_thunk.bx (standard toolchain help)
$ sh strfind.sh                  # literals verified in ORIG
PRESENT  open %s fail. err=%d:%s
PRESENT  open lock file fail. %s
PRESENT  write init data to %s fail. ret=%d
PRESENT  /tmp/tss_sdk_game_svr_id_pool_file
PRESENT  /tmp/tss_sdk_game_svr_id_pool_file.lock
PRESENT  rb+
PRESENT  wb+
```

## 7. Integration warning (duplicate file from a concurrent worker)

While this task was running another worker created
`recon/src/formmog/appcomm/app_bus_channel_id_mgr.cpp/.h` for the same CU. That
copy differs from the ORIG in two observable places:

1. `write/read_channel_info_from_file_without_lock` read/write
   `&max_channel_id_`; the ORIG passes `this` (0x18cb0c / 0x18cb90), i.e. it
   transfers `used_svr_id_`.
2. `init()` opens `BUS_CHANNEL_ID_FILE` for the lock; the ORIG opens
   `TSS_SVR_ID_POOL_LOCK_FILE` (0x00292354, the `.lock` path) and logs that same
   path on failure (0x18d073 / 0x18d07c / 0x18d0b3).

Only one of the two files should be kept; if the `recon/src/formmog/appcomm/`
copy is chosen, apply those two fixes (this report's file already has them).
