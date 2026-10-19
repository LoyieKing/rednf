# AGENT_REPORT_code_mgr

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, GCC 4.1.0, DWARF present, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr_agent.cpp`.

Original TU/header:
`src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr.cpp` /
`dp_code_mgr.h`.

## Symbols and exact ORIG ranges

Ranges are half-open (`end = start + size`).

| Symbol | ORIG range | size | Source lines |
|---|---|---:|---|
| `tss_dpsdk::read_file(char const*, unsigned char*, size_t)` | `0x000a73d0..0x000a745d` | `0x8d` | cpp 33-45 |
| `tss_dpsdk::read_file(char const*, ID_SET&)` | no symbol; inlined into `reload_code` | - | cpp 48-66 |
| `DPCodeMgr::CodeInfo::load_cfg(char const*)` | `0x000a7460..0x000a7619` | `0x1b9` | cpp 174-223 |
| `DPCodeMgr::reload_code()` | `0x000a7620..0x000a79ee` | `0x3ce` | cpp 101-133 |
| `DPCodeMgr::reload()` | `0x000a79f0..0x000a7a70` | `0x80` | cpp 91-99 |
| `DPCodeMgr::init()` | `0x000a7a70..0x000a7b38` | `0xc8` | cpp 69-89 |
| `DPCodeMgr::CodeItems<2000>::load_cfg(char const*)` | `0x000a7b40..0x000a7d1c` | `0x1dc` W | cpp 229-268 |
| `DPCodeMgr::CodeItems<20000>::load_cfg(char const*)` | `0x000a7d20..0x000a7efc` | `0x1dc` W | cpp 229-268 |
| `DPCodeMgr::DpCodeInfo::load_cfg(char const*)` | no symbol; inlined at `0xa76af..0xa79c6` | - | cpp 135-167 |

The three weak `std::_Rb_tree<unsigned int,...>` instantiations emitted by this
TU immediately after the loaders are also reproduced by the reconstruction with
exactly the ORIG sizes: `_M_erase` `0xa7f00` (`0x4b`), `_M_insert` `0xa7f50`
(`0x9b`), `insert_unique` `0xa7ff0` (`0x105`). This pairs the `ID_SET`
(`std::set<unsigned int>`) usage with the original TU.

## Exact layouts (DWARF, asserted in the .cpp)

```text
CodeItem<N>    = unsigned short weight_ +0, code_len_ +2, uchar code_buf_[N] +4
                 sizeof 2004 (0x7d4) / 20004 (0x4e24)
CodeItems<N>   = unsigned short total_weight_ +0, CodeItem<N> items_[100] +2
                 sizeof 200402 (0x30ed2) / 2000402 (0x1e8612)
CodeInfo       = CodeItems<2000> cryptdata_code_   +0
                 CodeItems<2000> crypt_code_       +0x30ed2
                 CodeItems<20000> bin_file_        +0x61da4
                 sizeof 2401206 (0x24a3b6)
DpCodeInfo     = ushort version_count_             +0
                 uint version_support_list_[8]     +4
                 uchar version_hash_[16]           +0x24
                 CodeInfo client_code_             +0x34
                 CodeInfo server_code_             +0x24a3ea
                 sizeof 4802464 (0x4947a0)
DpCodeInfos    = uint code_update_time_            +0
                 ushort code_count_                +4
                 DpCodeInfo code_infos_[2]         +8
                 sizeof 9604936 (0x928f48)
DPConfig       = dp_code_info                     +0x154
DPConfigCodeInfo = code_dir_count                 +0x3822 (absolute +0x3976)
                   char code_dir_path[2][256]      +0x3824 (absolute +0x3978)
                   sizeof 14884 (0x3a24); DPConfig sizeof 22842 (0x593a)
```

`DpCodeInfo::MAX_VERSION_COUNT = 8`, `MAX_VERSION_CODE = 2`,
`MAX_VERSION_HASH = 16`, `MAX_CODE_ITEM = 100`, `MAX_CODE_BUF_LEN = 2000`,
`MAX_BIN_FILE_LEN = 20000` (DWARF const values).

## Globals and config paths

| Symbol | Address | Value / role |
|---|---|---|
| `tss_dpsdk::dp_code_config_path` | `0x00292340` (.data.rel.ro, local) | pointer `0x0021b65f` = `"/tmp/dp_code_config"` |
| `tss_dpsdk::dp_code_lock` | `0x002bb4b0` (bss, local) | 8-byte `ZEN_File_Lock` |
| `s_dp_code_info_` | `0x002bb4a0` (bss, local, unqualified) | 16-byte `ZEN_ShareMem_Mmap` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | `0x002bb4c0` | `DPConfig*`, GOT slot `0x2943ac` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_head_` | `0x002bb4c4` | head pointer, GOT slot `0x294358`; `update_time` at +0 |
| `tss_dp_comm::DP_CONFIG::glb_config_update_time_` | `0x002bb4c8` | not read by these functions |
| `tss_dp_comm::dp_comm_config_head` | `0x00292344` | `"/tmp/dp_comm_config_head"` (unused here) |
| `tss_dp_comm::dp_comm_config` | `0x00292348` | `"/tmp/dp_comm_config"` (unused here) |

`init()` uses `dp_code_config_path` for both the lock file and the
`get_comm_mem` mapping name. It does not use `dp_comm_config_head` or
`dp_comm_config` in this TU.

Config formats decoded from the strings and the calls:

```text
/tmp/dp_code_config                 lock file + shared DpCodeInfos file
<dir>/dir.cfg                       CodeItems / CodeInfo item index
<dir>/versionhash.cfg               16 raw bytes
<dir>/version.ini                   whitespace-separated %u versions
<dir>/servercodedata                CodeInfo for server_code_
<dir>/clientcodedata                CodeInfo for client_code_

dir.cfg (CodeItems<N>):   "%s %u %u"  -> file name, code_id, weight
dir.cfg (CodeInfo):       "%s %d %d"  -> file name, weight, type
type 1 -> bin_file_; type 2 -> cryptdata_code_; type 3 -> crypt_code_
```

## Decoded behavior

### `read_file(path, buf, len)` — 0xa73d0

`fopen64(path,"rb")`; NULL → `debug_errorex("DPCodeMgr read_file fail, %s",
path)` and return `-1`. Otherwise `fread(buf,1,len,f)`, `fclose(f)`, return the
byte count as `int` (short reads are not an error here). It only opens/reads,
no config parsing.

### `read_file(path, ID_SET& store)` — inlined

Same open/error/return. Loop while `!feof(f)`: `fscanf(f,"%u ",&id) == 1` →
`store.insert(id)`. `fclose`, return `0`. A missing version file is therefore
not fatal; the caller ignores the return value.

### `CodeItems<N>::load_cfg(dir)` — 0xa7b40 / 0xa7d20

1. `snprintf(dir_cfg,0x100,"%s/dir.cfg",dir)`; `fopen64(...,"rb")`.
2. Open failure: `debug_errorex("DPCodeMgr read_file fail, %s", dir_cfg)`,
   return `-1` (no `fclose`).
3. `ret=0`; while `!feof(fp)`:
   - `fgets(dir_cfg,0x100,fp)`; NULL → end loop.
   - `sscanf(dir_cfg,"%s %u %u",one_file,&code_id,&weight) != 3` → continue.
   - `code_id > 99` → continue.
   - `snprintf(dir_cfg,0x100,"%s/%s",dir,one_file)`.
   - `size = read_file(dir_cfg, items_[code_id].code_buf_, N)`.
   - Reject unless `1 <= size < N` and `items_[code_id].code_len_ == 0`.
     ORIG comparison is the unsigned `size-1 > N-2`; accepted sizes are
     1..1999 (`N=2000`) and 1..19999 (`N=20000`).
   - Rejection: log
     `"DPCodeMgr read_file fail,dir=%s, dir_cfg=%s, size=%d, code_id=%d, old_len=%d"`,
     set `ret=-1`, break.
   - Accept: `items_[code_id].code_len_=(u16)size`,
     `items_[code_id].weight_=(u16)weight`,
     `total_weight_=(u16)(total_weight_+weight)`.
4. `fclose(fp)`; return `ret` (0 on normal end, -1 on the item error above).

No validation of `weight`, duplicates are errors (`code_len_ != 0`), and an
empty `dir.cfg` returns 0.

### `CodeInfo::load_cfg(dir)` — 0xa7460

1. `snprintf(dir_cfg,0x100,"%s/dir.cfg",dir)`; `fopen64(...,"rb")`.
2. Open failure: identical `read_file fail` log and return `-1`.
3. `ret=0`; while `!feof(fp)`:
   - `fgets(dir_cfg,0x100,fp)`; NULL → end loop.
   - `sscanf(dir_cfg,"%s %d %d",one_dir,&weight,&dir_type) != 3` →
     `ret=0; continue`.
   - `snprintf(dir_cfg,0x100,"%s/%s",dir,one_dir)`.
   - type 1: `bin_file_.load_cfg(dir_cfg); ret=0;` — the bin loader's return
     is discarded in the ORIG.
   - type 2: `ret=cryptdata_code_.load_cfg(dir_cfg);`
     `cryptdata_code_.total_weight_=(u16)(cryptdata_code_.total_weight_*weight)`.
   - type 3: `ret=crypt_code_.load_cfg(dir_cfg);`
     `cryptdata_code_.total_weight_=(u16)(cryptdata_code_.total_weight_*weight)`.
   - default: `ret=0`.
   - `if (ret != 0) break`.
4. `fclose(fp)`; return `ret`.

**Observed ORIG quirk preserved:** the type-3 branch loads `crypt_code_`
(`this+0x30ed2`) but multiplies `cryptdata_code_.total_weight_` (`this+0`),
not `crypt_code_.total_weight_` (`0xa78ab..0xa78be`, `0xa75c3..0xa75d1`,
`0xa75e0..0xa75f5`). The reconstruction reproduces the observed target
exactly rather than “fixing” it.

### `DpCodeInfo::load_cfg(dir)` — inlined at reload_code:117

1. `snprintf(base,0xff,"%s/versionhash.cfg",dir); base[0xff]=0`.
   `read_file(base, version_hash_, 16) <= 0` → return `-1`. Any positive
   short read is accepted; no exact-16 check.
2. `snprintf(base,0xff,"%s/version.ini",dir); base[0xff]=0`.
   `read_file(base, versions)` (return ignored).
3. Iterate `versions` (ascending, unique) while
   `it != versions.end() && version_count_ < 8`:
   `version_support_list_[version_count_++] = *it`.
   `version_count_` is not explicitly cleared; the caller's zeroed temporary
   provides that.
4. `snprintf(base,0xff,"%s/servercodedata",dir); base[0xff]=0`;
   `server_code_.load_cfg(base) != 0` → `-1`.
5. `snprintf(base,0xff,"%s/clientcodedata",dir); base[0xff]=0`;
   return `client_code_.load_cfg(base) == 0 ? 0 : -1`.
   ORIG compiles this to the `cmp $1 / sbb / not` idiom at `0xa7993`.

### `reload_code()` — 0xa7620

1. If `code_infos_->code_update_time_ ==
   glb_dp_config_head_->update_time`, return `0` (no lock, no allocation).
2. `temp_config = new DpCodeInfos` (size `0x928f48`); `memset(...,0,0x928f48)`.
3. `ret=0`; iterate `i = 0` while
   `i < glb_dp_config_->dp_code_info.code_dir_count` and, as a second ORIG
   guard, `i < 8`:
   - `dir = glb_dp_config_->dp_code_info.code_dir_path[i]` (256-byte stride,
     base absolute `+0x3978`).
   - `temp_config->code_infos_[i].load_cfg(dir)` (inlined).
   - Non-zero → `ret=-1; break`; ORIG forces `esi=-1` regardless of the
     returned value.
   - Success → `++temp_config->code_count_`.
4. If `ret == 0`, `memcpy(code_infos_, temp_config, sizeof(DpCodeInfos))`.
   On failure the shared block is **not** updated.
5. `delete temp_config`.
6. Always `code_infos_->code_update_time_ =
   glb_dp_config_head_->update_time`, including the failure path. A failed
   reload therefore publishes the new time and is not retried until the config
   head time changes again.
7. Return `ret`.

Note the ORIG cap is `8` (`cmp $8` at `0xa78f6`), although the config layout
only carries `code_dir_path[2][256]` and `DpCodeInfos::code_infos_[2]`; this
cap is reproduced literally.

### `reload()` — 0xa79f0

`if (code_infos_->code_update_time_ == head_->update_time) return 0;`
otherwise construct `ZEN_Write_Guard<ZEN_File_Lock>` on `dp_code_lock` (the
guard's inlined ctor calls `lock_read()`, dtor calls `unlock()`), call
`reload_code()`, unlock, and return its result. `reload_code` re-checks the
timestamp under the lock.

### `init()` — 0xa7a70

1. `ret = dp_code_lock.open(dp_code_config_path, 0x42, 0x1b0)`:
   flags `O_RDWR|O_CREAT` (`0x42`), mode `0660` (`0x1b0`).
2. `ret != 0` → `debug_errorex("open lock fail. ret=%d", ret)`, return `-1`
   (no lock taken).
3. Construct `ZEN_Write_Guard` (inlined `lock_read`).
4. `code_infos_ = (DpCodeInfos*)get_comm_mem(s_dp_code_info_,
   dp_code_config_path, sizeof(DpCodeInfos)=0x928f48)`.
   `get_comm_mem` is the external boundary: it first opens/maps with create
   semantics, retries existing-only on failure, and returns `mem.addr()` or
   NULL.
5. NULL → return `-1` (guard unlocks); otherwise `reload_code()` (return
   ignored), return `0`.

## External calls / boundaries

PLT targets resolved against the ORIG image:

| PLT | Target |
|---|---|
| `0x853e4 snprintf` | libc |
| `0x85184 fopen64` | libc |
| `0x7c794 feof`, `0x81ef4 fgets`, `0x85fb4 sscanf`, `0x84bc4 fclose` | libc |
| `0x80204 fscanf`, `0x80534 fread` | libc |
| `0x84904 memcpy`, `0x865c4 memset`, `0x842a4 operator new`, `0x88274 operator delete` | libc/libstdc++ |
| `0x80904` | `ZEN_Trace_LogMsg::debug_errorex(char const*,...)` @ `0x1b3d30` |
| `0x87c64` | `ZEN_File_Lock::open(char const*,int,unsigned int)` @ `0x19fe40` |
| `0x84024` | `ZEN_File_Lock::lock_read()` @ `0x19ff00` |
| `0x868d4` | `ZEN_File_Lock::unlock()` @ `0x19ff90` |
| `0x80e94` | `tss_dp_comm::get_comm_mem(ZEN_ShareMem_Mmap&,char const*,unsigned int)` @ `0xa86c0` |
| `0x86644` | `CodeItems<2000>::load_cfg` weak @ `0xa7b40` |
| `0x84874` | `CodeItems<20000>::load_cfg` weak @ `0xa7d20` |
| `0x83d94` | `CodeInfo::load_cfg` @ `0xa7460` |
| `0x825a4` | `DPCodeMgr::reload_code()` @ `0xa7620` |
| `0x88064` | `_Rb_tree<unsigned int,...>::insert_unique` weak @ `0xa7ff0` |
| `0x86734` | `_Rb_tree<unsigned int,...>::_M_erase` weak @ `0xa7f00` |
| `0x7d204` | `std::_Rb_tree_increment(_Rb_tree_node_base const*)` (libstdc++) |

`ZEN_File_Lock` is 8 bytes (`int lock_file_hadle_` +0, `bool open_by_self_`
+4); `ZEN_ShareMem_Mmap` is 16 bytes. Both are external models only. `std::set`
uses the target libstdc++ 4.1.2 symbols; the three weak instantiations match
ORIG sizes exactly.

## Error-path summary

| Failure | Result | State |
|---|---|---|
| `read_file` fopen | -1 after log | no close |
| `CodeItems::load_cfg` `dir.cfg` open | -1 after log | no close |
| item file read 0 / size >= N / duplicate slot | -1 after detailed log | dir.cfg closed; earlier items retained; total not changed for the failed item |
| malformed `dir.cfg` line | continue (0) | skipped |
| `code_id > 99` | continue (0) | skipped |
| `CodeInfo::load_cfg` `dir.cfg` open | -1 after log | no close |
| cryptdata/crypt loader failure | loader result, after close | bin loader errors are ignored |
| `DpCodeInfo` hash read <= 0 | -1 | versions/server/client not read |
| `DpCodeInfo` server/client loader failure | -1 | coerced to -1 |
| `reload_code` any dir failure | -1 | shared block unchanged; update time still published |
| `reload` timestamp unchanged | 0 | lock not taken |
| `init` lock open | -1 after log | no mapping |
| `init` mapping failure | -1 | guard unlocks |

## Compile result

Primary (GCC 4.1.2, c5root, `-m32 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector -std=gnu++98 -I recon` plus the c5root `-nostdinc`
include set used by the other units):

```text
C5_COMPILE_OK
```

Emitted symbol sizes (ORIG sizes in parentheses):

```text
CodeInfo::load_cfg                       0x1b9 (0x1b9)
read_file(char const*,uchar*,uint)       0x8d  (0x8d)
CodeItems<2000>::load_cfg                0x1d5 (0x1dc) W
CodeItems<20000>::load_cfg               0x1d5 (0x1dc) W
reload_code                              0x3d5 (0x3ce)
reload                                   0x76  (0x80)
init                                     0xc2  (0xc8)
_Rb_tree insert_unique/_M_insert/_M_erase 0x105/0x9b/0x4b (exact)
```

Undefined references are exactly the intended boundaries:
`glb_dp_config_`, `glb_dp_config_head_`, `get_comm_mem`,
`ZEN_File_Lock::{C1,D1,open,lock_read,unlock}`,
`ZEN_ShareMem_Mmap::{C1,D1}`, `debug_errorex`, libc file/memory calls and
libstdc++ `_Rb_tree` helpers.

Secondary syntax check with host GCC 13.3 (`-m32 -O2 -std=gnu++98 -fPIC
-fno-implement-inlines -fno-stack-protector -I recon`): `HOST_COMPILE_OK`
(format-truncation warnings only; that compiler does not inline
`DpCodeInfo::load_cfg` and emits it as an extra weak symbol, a toolchain
artifact not present in the GCC 4.1.2 object).

## Instruction comparison (not identity)

Normalized mnemonic counts, ORIG vs reconstruction:

```text
CodeInfo::load_cfg   116/116  (0.914 sequence ratio; same size)
reload_code          224/229  (0.155 ratio; schedule/register layout only)
reload                40/38   (0.974; exception cleanup pad differs)
init                  55/54   (0.954; one GOTOFF addressing form differs)
CodeItems<2000>      119/117  (0.958; address-materialization order)
CodeItems<20000>     119/117  (0.958)
read_file             42/44   (0.977; trailing alignment nops)
```

The `reload_code` low mnemonic ratio is entirely GCC 4.1.0-vs-4.1.2
scheduling/register allocation: every original basic block, load, call,
test, loop bound and store is reproduced in the same order of effects (see
the decoded flow above). No identity claim is made.

## Integration notes / unresolved boundaries

1. `get_comm_mem`, `ZEN_File_Lock`, `ZEN_ShareMem_Mmap` and
   `ZEN_Trace_LogMsg` are declarations only; their implementations remain in
   their own TUs. `get_comm_mem` was inspected only to establish that its
   return is `mem.addr()` / NULL.
2. `tss_dp_comm::DPConfig`, `DPConfigCodeInfo`, `DP_CONFIG` and
   `DPConfigHead` are partial models with exact offsets/sizes; unrelated
   config fields are pads.
3. `CodeItems<N>`, `CodeInfo`, `DpCodeInfo`, `DpCodeInfos` are reconstructed
   as nested types of `tss_dpsdk::DPCodeMgr` to emit the exact mangled symbols.
4. `read_file(path, ID_SET&)` and `DpCodeInfo::load_cfg` are inline-only in
   the ORIG; GCC 4.1.2 inlines them and emits no out-of-line symbols, matching.
5. The `dir.cfg`/`version*.cfg`/`*codedata` files themselves and the
   `DpCodeInfos` producer are outside this unit; no code-table bytes, counts
   or version values were fabricated.
6. No runtime oracle was executed. Semantic correctness is claimed for the
   decoded operations and control flow above; byte/instruction identity is
   not claimed.

## Semantic verdict

`SEMANTIC_RECONSTRUCTION_OK`: all assigned functions, their config paths,
file formats, layout offsets, ordering, error returns, update-time
publication, value scaling and the observed type-3 `cryptdata_code_` multiplier
quirk are reproduced from the ORIG; the only differences are compiler
scheduling, address-materialization and exception-cleanup artifacts.
