# UserMgr reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, symbol table + DWARF; original producer GCC C++ 4.1.0 (SUSE Linux)).

Source: `recon/src/formmog/tss_sdk/plugins/proto_proc/user_mgr_agent.cpp`
Original TU: `src/formmog/tss_sdk/plugins/user_control/user_mgr.cpp`
Original header: `src/formmog/tss_sdk/plugins/user_control/user_mgr.h`

## Symbol coverage and address ranges

Sizes are from `nm -S`; ranges are half-open and include the alignment padding up
to the next symbol where noted.

| ORIG symbol | Range | Size |
|---|---|---:|
| `UserMgr::del_user(UserInfo*)` | `0x000ce070..0x000ce307` | `0x297` |
| `UserMgr::~UserMgr()` D2 | `0x000ce310..0x000ce34d` (padding to `0xce350`) | `0x3d` |
| `UserMgr::UserMgr()` C1 | `0x000ce350..0x000ce38b` | `0x3b` |
| `UserMgr::UserMgr()` C2 | `0x000ce390..0x000ce3cb` | `0x3b` |
| `UserMgr::add_user(uint, ushort, ushort)` | `0x000ce3d0..0x000ce8f8` | `0x528` |
| `UserMgr::~UserMgr()` D1 | `0x000ce900..0x000ce93d` | `0x3d` |
| `UserMgr::check(on_user_check, void*, long)` | `0x000ce940..0x000ceaaa` | `0x16a` |
| `UserMgr::get_user(uint, ushort, ushort)` | `0x000ceab0..0x000cebe1` | `0x131` |
| `UserMgr::get_user_info(uint&, ushort&, ushort&, uint)` | `0x000cebf0..0x000cec88` | `0x98` |
| `UserMgr::init(char const*, uint, int)` | `0x000cec90..0x000cf20f` | `0x57f` |

C1/C2 are instruction-for-instruction equivalent; D1/D2 are equivalent. The
reconstructed file emits all four (`_ZN12tss_sdk_impl7UserMgrC1Ev`,
`...C2Ev`, `...D1Ev`, `...D2Ev`) plus the six non-trivial methods with the
original mangled names.

`UserMgr` method addresses that are not part of this assignment:
`get_next_prime` `0xcf210` and helpers inlined elsewhere are not emitted.

## Objects and exact offsets

### `tss_sdk_impl::UserMgr` (DWARF size 40)

| Offset | Type | Name |
|---|---|---|
| `+0x00` | `ZEN_LIB::shm_hashtable_expire<...>*` | `user_info_hash_table_` |
| `+0x04` | `time_t` (4) | `last_check_time_` |
| `+0x08` | `int` | `use_idx_` |
| `+0x0c` | `ZEN_Thread_Mutex` (28) | `user_info_hash_table_mutex_` |

The constructor writes only `+0x00 = 0`, `+0x04 = 0` and constructs the mutex
with a null name. `use_idx_` is left uninitialized until `init()` succeeds.
The destructor deletes `+0x00` with `operator delete(void*)` when non-null and
then destroys the mutex. It does not unmap the shared memory or destroy the
values. No method locks/unlocks the mutex; the original only constructs and
destroys it.

### `tss_sdk_impl::UserInfo` (DWARF size 400)

| Offset | Size | Name / contents |
|---|---:|---|
| `+0` | 100 | `base_info_` |
| `+100` | 48 | `user_code_info_` |
| `+148` | 20 | `user_timer_` |
| `+168` | 52 | `user_stat_` |
| `+220` | 180 | `hack_data_` |

`base_info_` (size 100) member offsets, from DWARF:

| Offset | Type | Name |
|---|---|---|
| `+0` | `unsigned int` | `uin_` |
| `+4` | `unsigned int` | `game_id_` |
| `+8` | `unsigned int` | `room_id_` |
| `+12` | `unsigned int` | `client_ip_` |
| `+16` | `unsigned int` | `server_ip_` |
| `+20` | `unsigned int` | `game_usr_id_` |
| `+24` | `unsigned int` | `client_version_` |
| `+28` | `unsigned int` | `client_user_id_` |
| `+32` | `unsigned int` | `flags_` |
| `+36` | `unsigned char[64]` | `role_name_` |

The reconstructed file models only `base_info_` plus 300 opaque bytes; the
nested blocks are not read or written by UserMgr. `UserInfo::init` is an
external definition at `0x000cdb80` (not reconstructed here); its observed
effect is `memset(this, 0, 400)`, `uin_ = uin`, `game_id_ = (zero-extended)
game_id`, `room_id_ = (zero-extended) room_id`, positions `+0x90/+0x92 =
0xffff`, timers `+0x94/+0x98/+0x9c = now`, return 0.

### ZEN shared-memory hash container

DWARF specialization:
`ZEN_LIB::shm_hashtable_expire<tss_sdk_impl::UserInfo, unsigned long long,
ZEN_LIB::smem_hash<unsigned long long>, tss_sdk_impl::ExtractUserInfo,
std::equal_to<unsigned long long>, tss_sdk_impl::TimeOutProc>`.

Wrapper object (DWARF size 36, allocated by `operator new(0x24)`):

| Offset | Type | Name |
|---|---|---|
| `+0x00` | `void*` (base `_shm_memory_base`) | `smem_base_` |
| `+0x04` | `_hashtable_expire_head*` | `lru_hash_head_` |
| `+0x08` | `int*` | `hash_factor_base_` |
| `+0x0c` | `int*` | `hash_index_base_` |
| `+0x10` | `_shm_list_index*` | `lst_index_base_` |
| `+0x14` | `_shm_list_index*` | `lst_free_node_` |
| `+0x18` | `_shm_list_index*` | `lst_use_node_` |
| `+0x1c` | `int*` | `priority_base_` |
| `+0x20` | `UserInfo*` | `value_base_` |

Mapping base layout for `P = lru_hash_head_->num_of_node_`:

| Base offset | Type | Meaning |
|---|---|---|
| `+0x00` | `_hashtable_expire_head` (20) | `size_of_mmap_`, `num_of_node_` (`+4`), `sz_freenode_` (`+8`), `sz_usenode_` (`+0xc`), `sz_useindex_` (`+0x10`) |
| `+0x14` | `int[P]` | bucket heads, `-1` empty |
| `+0x14+4P` | `int[P]` | bucket chain next, `-1` end |
| `+0x14+8P` | `_shm_list_index[P+2]` | node links; `[P]` is the use-list sentinel, `[P+1]` the free-list sentinel |
| `+0x24+16P` | `int[P]` | priority (time), `-1` after deletion |
| `+0x24+20P` | `UserInfo[P]` | value array, stride 400 |

`_shm_list_index` is 8 bytes: `idx_next_` at `+0`, `idx_prev_` at `+4`.
Lists are circular doubly-linked lists whose sentinels are stored at indices
`P` (use) and `P+1` (free); the two sentinels live immediately after the
`lst_index` array and are addressed through it.

Allocated mapping size is `0x24 + 424*P` (400*P values plus 24*P index/time
space plus 0x24 header). The last `4*P` bytes of that allocation are not
addressed by any of the reconstructed methods.

### `tss_dp_comm::DPConfig` fields used

| Offset | Field |
|---|---|
| `+0x0000` | `dp_base_conf` (6 bytes) |
| `+0x0001` | `dp_base_conf.is_restore_usr_space` (read by `init`) |
| `+0x0006` | `dp_proto_conf[10]` |
| `+0x0010` | `dp_log_conf[296]` |
| `+0x0138` | `dp_time_confg` (28 bytes) |
| `+0x0148` | `dp_time_confg.check_time` (read by `check`) |

`DPConfig` DWARF size is 22842; the tail after `+0x0154` is opaque here.
`glb_dp_config_` is the pointer global at `0x002bb4c0` (GOT slot `0x2943ac`),
type `tss_dp_comm::DP_CONFIG::glb_dp_config_`.

## Key encoding and hashing

Both `ExtractUserInfo` and `smem_hash` are inlined at every ORIG call site.
The 64-bit key lives in an EDX:EAX pair; the game field is shifted left by 16
with `shld $16`/`shl $16`, so its full 32-bit value is preserved in the upper
half of the shift.

```
value_key = ((uint64)uin_ << 32) + ((uint64)game_id_ << 16) + room_id_
arg_key   = ((uint64)uin    << 32) + ((uint64)game_id    << 16) + room_id
hash      = (uint32)(key >> 32) + (uint32)key   // wrapping 32-bit add
bucket    = hash % P                            // unsigned divl, P at lru_head+4
```

`add_user` builds the first key from the freshly initialized value (whose
`game_id_` was stored zero-extended by `UserInfo::init`), then rebuilds the
same key from its `unsigned short` arguments for the follow-up `find`.
`del_user` narrows both fields to `unsigned short` before building its search
key. `get_user` takes both ids as `unsigned short`. Stored-node comparisons
rebuild the key from `uin_`, the full 32-bit `game_id_`, and the full 32-bit
`room_id_`, exactly as the emitted `shld/shl $16` sequences do.

## Decoded method semantics

### `init(char const*, unsigned, int)`

1. Builds a 256-byte NUL-padded path:
   `/tmp/tss_sdk_shm_user` + `shm_name` via `strncat(path, shm_name, 0x100)`.
2. Selects `P` = first entry of the 70-entry prime list at `.rodata 0x220080`
   that is `>= count`; if `count > 0xfffffffb`, uses the last entry
   `0xfffffffb`. Minimum `P` is 53 even for `count == 0`.
3. `shm_size = 0x24 + 424*P`.
4. Calls `tss_dp_comm::get_comm_mem(s_dp_user_info_, path, shm_size)`.
   Null result logs `open user info shm file fail. filepath=%s, user_count=%u,
   shm_size=%lu.` with `(path, P, shm_size)` and returns `-2`.
5. If `glb_dp_config_->dp_base_conf.is_restore_usr_space != 0`:
   - If the mapping header at `+0/+4` equals `(shm_size, P)`, stamps those
     values again, allocates a 36-byte wrapper, restores all raw pointers, sets
     `user_info_hash_table_` and `use_idx_`, returns 0.
   - Otherwise sets `user_info_hash_table_ = 0` (without deleting the old
     wrapper) and falls through to fresh initialization on the same mapping.
6. Else (not restoring):
   - If `user_info_hash_table_ != 0`, only writes `use_idx_ = use_idx` and
     returns 0. The mapping call in step 4 has already happened.
7. Fresh initialization:
   - stamps `size_of_mmap_ = shm_size`, `num_of_node_ = P`;
   - allocates the wrapper with `operator new(0x24)`;
   - sets `smem_base_`, `lru_hash_head_ = base`, and the five array pointers;
   - sets `sz_freenode_ = P`, `sz_usenode_ = 0`, `sz_useindex_ = 0`;
   - use sentinel `{P, P}`, free sentinel `{P+1, P+1}`;
   - for `i = 0..P-1`: `hash_factor[i] = -1`, `hash_index[i] = -1`,
     `priority[i] = 0`, `lst[i] = {i+1, i-1}`;
     at `i == 0`, `lst[0].prev = free.next` (`P+1`) then `free.next = 0`;
     at `i == P-1`, `lst[P-1].next = free.prev` (`P+1`) then
     `free.prev = P-1`;
   - stores the wrapper in `user_info_hash_table_`, `use_idx_ = use_idx`,
     returns 0.

No failure path after step 4 frees/unmaps the mapped region. `use_idx_` is not
written if the function returns `-2`.

### `add_user(unsigned, unsigned short, unsigned short)`

1. Logs `UserMgr::AddClient, uin=%u game_id=%u room_id=%u`.
2. `now = time(NULL)`; constructs a stack `UserInfo` and calls
   `UserInfo::init(uin, game_id, room_id, now)`.
3. Inlined `insert()` duplicate scan: computes the key from the temp value,
   bucket, and walks `hash_factor[bucket]` / `hash_index[node]`. If an equal
   key is found, or if `sz_freenode_ == 0`, the code enters the user-level
   second `find(key)`:
   - rebuild the key from the arguments;
   - recompute bucket and walk the chain again;
   - not found -> return `-1`;
   - found -> `memcpy(value_base + idx, &user_info, 400)`. Counters, priority
     and LRU position are not updated.
4. Otherwise `create_node`:
   - take `newnode = lst_free_node_->idx_next_`; remove it from the free list
     and push it at the use-list head;
   - `memcpy(value_base + newnode, &user_info, 400)`;
   - `priority[newnode] = now`;
   - `sz_usenode_ += 1`, `sz_freenode_ -= 1`;
   - if `newnode != -1`, insert at bucket head:
     `hash_index[newnode] = hash_factor[bucket]; hash_factor[bucket] = newnode`.
5. Return `idx + 1` on success/update, except return `-2` when
   `idx > 200000000` (`0x0BEBC200`, signed compare). Return `-1` when the table
   is full or the second `find` fails.

### `del_user(UserInfo*)`

1. Null pointer -> `-1`.
2. Reads `uin_` full, narrows `game_id_` and `room_id_` to `unsigned short`,
   logs `UserMgr::DeleteClient, uin=%u, game_id=%u, room_id=%u`, builds the key
   from those locals.
3. Bucket empty -> `-2`. Otherwise walks the chain comparing stored keys;
   no match -> `-2`.
4. Unlinks from the bucket: head case updates `hash_factor[bucket]`, otherwise
   `hash_index[prev] = hash_index[node]`.
5. Unlinks from the use list using `lst[node]` neighbors.
6. Pushes `node` at the free-list head:
   `lst[node].next = free.next`, `lst[node].prev = lst[free.next].prev`,
   `free.next = node`, `lst[free.next].prev = node`.
7. `sz_usenode_ -= 1`, `sz_freenode_ += 1`, `priority[node] = -1`,
   `hash_index[node] = 0`.
8. Returns 0. The passed `UserInfo*` is used only for key extraction and
   logging; it is not written.

### `get_user(unsigned, unsigned short, unsigned short)`

Builds the argument key, computes the bucket, walks the chain comparing full
64-bit stored keys, returns `&value_base[node]` on match or `0` on `-1`.
No null-table or empty-table guard is present in the original, and none is
added here.

### `get_user_info(unsigned&, unsigned short&, unsigned short&, unsigned)`

- `use_idx_ == 0`: tail-calls `get_user(uin, game_id, room_id)`; the reference
  arguments are not written.
- `use_idx_ != 0`: `idx` is interpreted as a signed value for the `<= 0`
  guard, then compared unsigned against `num_of_node_`. Returns `0` when
  `idx <= 0`, `idx > P`, or the computed pointer is null. Otherwise writes
  `uin = value.uin_`, `game_id = (unsigned short)value.game_id_`,
  `room_id = (unsigned short)value.room_id_` and returns
  `value_base + (idx - 1)`.
- This direct-index path does not check whether the node is currently used; a
  free node within `1..P` is returned with its stale fields.

### `check(int (*)(UserInfo*, void*, long), void*, long)`

1. If `(unsigned int)(now - last_check_time_) <
   glb_dp_config_->dp_time_confg.check_time`, returns 0 without touching
   `last_check_time_`.
2. Iterates every non-empty bucket in ascending bucket order, following each
   chain; calls `func(&value_base[node], userdata, now)`.
3. The callback's `int` return value is ignored.
4. Stores `last_check_time_ = now` after the iteration (including an empty
   table) and returns 0. No locking.

## Call targets / external boundaries

Direct and PLT-visible references emitted by the reconstruction:

- `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)` (add/del)
- `ZEN_Trace_LogMsg::debug_errorex(char const*, ...)` (init failure)
- `ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)`,
  `ZEN_Thread_Mutex::~ZEN_Thread_Mutex()`
- `ZEN_ShareMem_Mmap` constructor/destructor for the 16-byte static
  `s_dp_user_info_` (`.bss 0x002c5620`, local symbol)
- `tss_dp_comm::get_comm_mem(ZEN_ShareMem_Mmap&, char const*, unsigned int)`
  (`0x000a86c0`)
- `tss_dp_comm::DP_CONFIG::glb_dp_config_` (`0x002bb4c0`)
- `tss_sdk_impl::UserInfo::init(unsigned int, unsigned short, unsigned short,
  long)` (`0x000cdb80`)
- `operator new(unsigned int)`, `operator delete(void*)`
- `time`, `memset`, `memcpy`, `strncat`
- self-call `UserMgr::get_user` from `get_user_info` when `use_idx_ == 0`

The hash-container member operations (`insert`, `find`, `create_node`,
`destroy_node`) and the key/hash functors are inlined in the original and are
also inlined here; no extra out-of-line UserMgr/container symbols are defined.
The local data/function symbols are the prime table, the static
`ZEN_ShareMem_Mmap` object (`s_dp_user_info_`), and the compiler-generated
static-initialization helpers (`__static_initialization_and_destruction_0`,
`__tcf_*`, `_GLOBAL__I__ZN12tss_sdk_impl7UserMgrC2Ev`), which the original TU
also contains.

## Compile result

Compiled successfully with the c5root GCC 4.1.2 toolchain and the assignment
flags:

```
-m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector -std=gnu++98 -I recon
```

(c5root requires its `-nostdinc` + `-isystem` header set; no `-DNDEBUG` or
`-pthread` was needed for the final compile.)

The object defines:

```
_ZN12tss_sdk_impl7UserMgrC1Ev
_ZN12tss_sdk_impl7UserMgrC2Ev
_ZN12tss_sdk_impl7UserMgrD1Ev
_ZN12tss_sdk_impl7UserMgrD2Ev
_ZN12tss_sdk_impl7UserMgr4initEPKcji
_ZN12tss_sdk_impl7UserMgr8add_userEjtt
_ZN12tss_sdk_impl7UserMgr8del_userEPNS_8UserInfoE
_ZN12tss_sdk_impl7UserMgr5checkEPFiPNS_8UserInfoEPvlES3_l
_ZN12tss_sdk_impl7UserMgr8get_userEjtt
_ZN12tss_sdk_impl7UserMgr13get_user_infoERjRtS2_j
```

C1/C2 are 0x3b bytes and D1/D2 0x3d bytes, matching the ORIG sizes; the other
method sizes differ due to GCC 4.1.0-vs-4.1.2 code generation and because the
duplicate prime-list searches in `init` were folded into one semantically
identical loop.

Compile-time layout checks in the file enforce: `sizeof(UserMgr) == 40`,
`sizeof(UserInfo) == 400`, `sizeof(UserInfoBaseInfo) == 100`,
`sizeof(shm_hashtable_expire) == 36`, `check_time` at `DPConfig + 0x148`, and
`is_restore_usr_space` at `DPConfig + 1`.

## Unresolved / explicitly external behavior

- `get_comm_mem` internals (mmap creation/restoration) are not reconstructed.
  Its null/non-null result is the only contract used.
- `ZEN_ShareMem_Mmap` (16 bytes) and `ZEN_Thread_Mutex` (28 bytes) are opaque
  ABI models; only their construction/destruction/pass-by-reference is used.
- `UserInfo` nested blocks at `+100,+148,+168,+220` and `UserInfo::init` are
  external; the local model only guarantees the 400-byte size and the
  `base_info_` offsets.
- `DPConfig` after `+0x154` is opaque; only `is_restore_usr_space` and
  `check_time` are modeled.
- The 4*P unused tail of the shared mapping and the `sz_useindex_` counter are
  preserved but not otherwise exercised by these methods.
- The `newnode == -1` branch after a non-empty free-list check is treated as
  unreachable under valid table invariants; it returns `-1`, which equals the
  original's outcome after its second `find(key)` fails.
- The original methods do not lock the mutex, so no lock/unlock calls were
  invented. Concurrency behavior is therefore unchanged (none of these methods
  are synchronized).
- Semantic reconstruction only: no byte/instruction identity and no
  full-`libtss_sdk` link claim. Undefined ZEN/config/UserInfo symbols are
  intentional integration boundaries.
