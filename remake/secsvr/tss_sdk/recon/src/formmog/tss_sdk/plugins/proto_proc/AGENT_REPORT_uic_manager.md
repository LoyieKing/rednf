# UicTdirty / UicUserInputInfoMgr / UserInfo::init reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, symbol table + DWARF, producer GCC 4.1.0 SUSE).

Source: `recon/src/formmog/tss_sdk/plugins/proto_proc/uic_manager_agent.cpp`

Original TUs (from `addr2line`):

- `src/formmog/tss_sdk/plugins/uic/uic_tdirty.cpp`
- `src/formmog/tss_sdk/plugins/uic/uic_user_input_info.cpp`
- `src/formmog/tss_sdk/plugins/user_control/user_info.cpp`

## Symbol coverage and address ranges

Sizes are from `nm -S`; ranges are half-open and end at the next aligned
symbol.

| ORIG symbol | Range | Size |
|---|---|---:|
| `UicTdirty::UicTdirty()` C2 | `0x000c83a0..0x000c83b2` | `0x12` |
| `UicTdirty::UicTdirty()` C1 | `0x000c83c0..0x000c83d2` | `0x12` |
| `UicTdirty::~UicTdirty()` D2 | `0x000c83e0..0x000c83e5` | `0x05` |
| `UicTdirty::~UicTdirty()` D1 | `0x000c83f0..0x000c83f5` | `0x05` |
| `UicTdirty::uic_dirty_replace(int, uchar*, int*)` | `0x000c8810..0x000c88d6` | `0xc6` |
| `UicTdirty::uic_dirty_replace(uchar*)` | `0x000c88e0..0x000c8939` | `0x59` |
| `UicTdirty::uic_dirty_check(int, uchar*, int*, MsgResultFlag*)` | `0x000c8940..0x000c8a1c` | `0xdc` |
| `UicTdirty::uic_dirty_check(uchar*, MsgResultFlag*)` | `0x000c8a20..0x000c8a89` | `0x69` |
| `UicTdirty::init(char*, uint, sec_proto::UserInputCode)` | `0x000c8a90..0x000c8afc` | `0x6c` |
| `UicTdirty::instance()` | `0x000c8b00..0x000c8b5d` | `0x5d` |
| `UicUserInputInfoMgr::UicUserInputInfoMgr()` C2 | `0x000c8b60..0x000c8b9f` | `0x3f` |
| `UicUserInputInfoMgr::UicUserInputInfoMgr()` C1 | `0x000c8ba0..0x000c8bdf` | `0x3f` |
| `UicUserInputInfoMgr::~UicUserInputInfoMgr()` D2 | `0x000c8be0..0x000c8be5` | `0x05` |
| `UicUserInputInfoMgr::~UicUserInputInfoMgr()` D1 | `0x000c8bf0..0x000c8bf5` | `0x05` |
| `UicUserInputInfoMgr::reset_count()` | `0x000c8c00..0x000c8c34` | `0x34` |
| `UicUserInputInfoMgr::print_info()` | `0x000c9050..0x000c90a3` | `0x53` |
| `UicUserInputInfoMgr::get_now_time()` | `0x000c90b0..0x000c9103` | `0x53` |
| `UicUserInputInfoMgr::get_user_num()` | `0x000c9110..0x000c9169` | `0x59` |
| `UicUserInputInfoMgr::fini()` | `0x000c9170..0x000c91dd` | `0x6d` |
| `UicUserInputInfoMgr::on_recv_check_result(const UserInputCheckPkg*)` | `0x000c91e0..0x000c95ed` | `0x40d` |
| `UicUserInputInfoMgr::init(uint, TssSdkPluginCallbackProc)` | `0x000c95f0..0x000c9975` | `0x385` |
| `UicUserInputInfoMgr::add_user_input(const UserMessage*, uint*)` | `0x000c9980..0x000c9e09` | `0x489` |
| `UicUserInputInfoMgr::expire_user_input()` | `0x000c9e10..0x000ca0eb` | `0x2db` |
| `UserInfo::init(uint, ushort, ushort, long)` | `0x000cdb80..0x000cdc04` | `0x84` |

Static storage also reconstructed at the original addresses/sizes:

| Symbol | Address | Size |
|---|---|---:|
| `UicTdirty::instance_` | `0x002c3860` | 4 |
| `UicTimeOutProc::callback_proc_` | `0x002c3e40` | 4 |
| `UicTimeOutProc::user_info_mem_pool_` | `0x002c3e44` | 4 |
| `UicTimeOutProc::timeout_nodes_num_` | `0x002c3e48` | 4 |

## Object layouts

### `tss_sdk_impl::UicTdirty` (DWARF size 8)

| Offset | Type | Name |
|---|---|---|
| `+0x00` | `sec_proto::UserInputCode` (4) | `code_` |
| `+0x04` | `bool` (1) | `is_inited_` |

Constructor writes `code_ = USER_INPUT_CODE_GBK (0)` and `is_inited_ = false`.
The destructor is empty. `instance_` is a `.bss` pointer; `instance()` lazily
allocates 8 bytes with `operator new(8, std::nothrow)`, constructs the object,
and stores the pointer even when allocation fails (so a failed allocation is
retried on the next call).

### `tss_sdk_impl::UicUserInputInfoMgr` (DWARF size 32)

| Offset | Type | Name |
|---|---|---|
| `+0x00` | `unsigned char*` | `uic_user_info_hashmap_buf_` |
| `+0x04` | `shm_hashtable_expire<...>*` | `uic_user_info_hashmap_` |
| `+0x08` | `BuddyAlloc*` | `user_info_mem_pool_` |
| `+0x0c` | `TssSdkPluginCallbackProc` | `callback_proc_` |
| `+0x10` | `volatile unsigned int` | `sequence_` |
| `+0x14` | `volatile unsigned int` | `add_input_num_` |
| `+0x18` | `volatile unsigned int` | `recv_input_total_num_` |
| `+0x1c` | `volatile unsigned int` | `recv_input_valid_num_` |

Constructor zeroes all eight fields; destructor is empty. Static constants from
DWARF: `USER_INFO_MEM_POOL_MAX_SIZE = 0x200000`,
`USER_INFO_EXPIRE_TIME = 10`, `USER_INFO_UNSIGNED_INT_MAX = 0xffffffff`.

### `tss_sdk_impl::UicUserInputInfo` (DWARF size 36)

| Offset | Type | Name |
|---|---|---|
| `+0x00` | `unsigned int` | `uin_` |
| `+0x04` | `unsigned int` | `seq_` |
| `+0x08` | `int` | `input_type_` |
| `+0x0c` | `int` | `msg_result_` |
| `+0x10` | `char` | `if_replace_` |
| `+0x14` | `unsigned char*` | `message_` |
| `+0x18` | `int` | `message_len_` |
| `+0x1c` | `unsigned char*` | `callback_data_` |
| `+0x20` | `int` | `callback_data_len_` |

### Shared-memory hash table

The wrapper object is 36 bytes (`_shm_memory_base` + 8 pointers).  The backing
buffer is `0x24 + 60*P` bytes for the selected prime `P`:

| Buffer offset | Contents |
|---|---|
| `+0x00` | `_hashtable_expire_head` (20 bytes) |
| `+0x14` | `int hash_factor_[P]` — bucket heads, `-1` empty |
| `+0x14+4P` | `int hash_index_[P]` — bucket chains, `-1` end |
| `+0x14+8P` | `_shm_list_index lst_[P+2]` — `lst[P]` use sentinel, `lst[P+1]` free sentinel |
| `+0x24+16P` | `unsigned int priority_[P]` |
| `+0x24+20P` | `UicUserInputInfo value_[P]`, stride 36 |
| tail | `4*P` bytes unused allocation slack |

`_hashtable_expire_head`: `size_of_mmap_ +0`, `num_of_node_ +4`,
`sz_freenode_ +8`, `sz_usenode_ +0xc`, `sz_useindex_ +0x10`.
`_shm_list_index`: `idx_next_ +0`, `idx_prev_ +4`.

Prime selection uses the 70-entry `.rodata` table at `0x0021f600`
(53, 97, 193, …, 4294967291); first entry `>= count` wins, and count values
above the last entry fall back to `0xfffffffb`.

### `tss_sdk_impl::UserInfo` (DWARF size 400)

| Offset | Size | Block |
|---|---:|---|
| `+0x00` | 100 | `base_info_` |
| `+0x64` | 48 | `user_code_info_` |
| `+0x94` | 20 | `user_timer_` |
| `+0xa8` | 52 | `user_stat_` |
| `+0xdc` | 180 | `hack_data_` |

`base_info_`: `uin_ +0`, `game_id_ +4`, `room_id_ +8`, `client_ip_ +12`,
`server_ip_ +16`, `game_usr_id_ +20`, `client_version_ +24`,
`client_user_id_ +28`, `flags_ +32`, `role_name_[64] +36`.

`user_code_info_`: `client_state_ +0`, four sequence/count uints `+4..+16`,
`server_cur_code_ +20`, `server_new_code_ +22`, `client_code_[5] +24`,
`bin_file_code_ +34`, `send_bin_file_count_ +36`, 2-byte pad `+38`,
`down_cmd_list_crc_ +40`, `up_cmd_list_pos_ +44`,
`down_cmd_list_pos_ +46`.

`user_timer_` (time_t at `+0x94..+0xa7`):
`last_check_init_pkg_time_ +0`, `last_send_test_pkg_time_ +4`,
`last_recv_stat_pkg_time_ +8`, `last_send_bin_code_time_ +12`,
`last_send_bin_file_time_ +16`.

`user_stat_` is thirteen 32-bit counters; `hack_data_` is 180 bytes
(`safemode_level_`, `dnf_client_uid_`, `drop_cmd_[8]`, then 140 opaque bytes).
`UserInfo::init` only touches `base_info_`, `user_code_info_.up/down_cmd_list_pos_`,
and the first three timer entries.

## Decoded semantics

### UicTdirty

- `init(char*, uint, UserInputCode)`: if `is_inited_` is true return 0.  Else
  call `tInitializeDirtyCheck(file, size)`; on negative result log
  `[uic sdk]UicTdirty::init tInitializeDirtyCheck failed` and return `-1`.
  On success store the code enum and return 0.  The original **never writes
  `is_inited_`**, so the guard is dead; this is reproduced, not corrected.
- `uic_dirty_check(int, uchar*, int*, MsgResultFlag*)`:
  - null data/out_len/result -> error log, `-1`, result untouched.
  - `code_ == USER_INPUT_CODE_GBK (0)` -> `tDirtyCheck(size, data, out_len)`.
  - `code_ == USER_INPUT_CODE_UTF8 (1)` -> `utf8DirtyCheck(...)`.
  - other code -> unknown-code log, `-1`.
  - return `> 0` -> `*result = MSG_DIRTY_FLAG (2)`, return 0.
  - return `== 0` -> `*result = MSG_NORMAL_FLAG (0)`, return 0.
  - return `< 0` -> `*result = MSG_NORMAL_FLAG (0)`, log
    `... uic_dirty_check tDirtyCheck failed` (the same text is used for the
    UTF-8 failure path in this build), return `-1`.
- `uic_dirty_check(uchar*, MsgResultFlag*)`: null guard then calls the
  int-taking overload with a literal size **1** and a local zero length.
- `uic_dirty_replace(int, uchar*, int*)`: null guards; GBK -> `tDirtyReplace`,
  UTF-8 -> `utf8DirtyReplace`; `>= 0` success (`0`), negative logs the matching
  GBK/UTF-8 failure string and returns `-1`; unknown code logs and returns
  `-1`.
- `uic_dirty_replace(uchar*)`: null guard then calls the int overload with
  literal size **1**.

### UicUserInputInfoMgr lifecycle

- `init(count, callback)`:
  - null callback -> `[uic sdk]...init call_back == NULL`, return `-1`.
  - `P = next_prime(count)`, `size = 0x24 + 60*P` (32-bit wrapping).
  - `new (std::nothrow) unsigned char[size]`; store at `+0`; on null log
    `...uic_user_info_hashmap_buf_ == NULL`, call `fini()`, return `-1`.
  - stamp head `size_of_mmap_ = size`, `num_of_node_ = P`.
  - allocate the 36-byte wrapper with throwing `operator new`, point its eight
    fields into the buffer as tabulated above, store it at `+4`.
  - initialize head counts, sentinels, bucket heads `-1`, chains `-1`,
    priorities `0`, and the free/use doubly-linked index list
    (`lst[i].next = i+1`, `lst[i].prev = i-1`, then the first/last links to the
    free sentinel).
  - `new (std::nothrow) BuddyAlloc()`; on null log `...user_info_mem_pool_ == NULL`,
    `fini()`, `-1`.
  - `BuddyAlloc::init(&0x200000, 0x10, true)`; on failure log
    `...user_info_mem_pool_->init failed`, `fini()`, `-1`.
  - on success assign `UicTimeOutProc::callback_proc_`, the global
    `UicTimeOutProc::user_info_mem_pool_`, and `this->callback_proc_`; return 0.
- `fini()`: if pool non-null call `BuddyAlloc::fini()`, delete it, clear `+8`;
  if buffer non-null `delete[]`, clear `+0`.  The 36-byte hash wrapper at `+4`
  is intentionally **not** deleted or cleared, matching the original.  Returns 0.

### add_user_input

Validation requires `message`, `message->message_ (+0x14)`, and `out_seq`
non-null; failure logs invalid args and returns `-2` (the only `-2` path).

1. `seq = ++sequence_`, `*out_seq = seq`.
2. Read `if_replace_ (+0x0c)`, `input_type_ (+0x04)`, `message_len_ (+0x10)`,
   `callback_data_len_ (+0x18)`.
3. `alloc_size = (int)(message_len + callback_data_len + 2)`.  If `> 0`,
   allocate from `user_info_mem_pool_`; on failure log alloc failure, `-1`.
   Copy `message_len` bytes, NUL-terminate, place callback data at
   `buffer + message_len + 1`, copy callback bytes, NUL-terminate.
4. Key is `((uint64)uin << 32) | seq`; hash is the wrapping 32-bit
   `uin + seq`, bucket `hash % num_of_node_`.  If a node with the same key
   exists: log `find node before insert`, free the message buffer (even when
   null), return `-1`.
5. Read `get_now_time()` after the duplicate scan and before the free-list
   check (ordering preserved).
6. Pop a node from the free list; empty -> log
   `result.first == uic_user_info_hashmap_->end()`, free buffer, `-1`.
7. Fill `UicUserInputInfo`: `uin_`, `seq_`, `input_type_`,
   `msg_result_ = EMSG_NOT_OPERATE_FLAG (3)`, `if_replace_`, `message_`,
   `message_len_`, `callback_data_`, `callback_data_len_`.  Set
   `priority_[idx] = now`, insert at the bucket-chain head, and increment
   `add_input_num_`.  If the incremented value equals
   `USER_INFO_UNSIGNED_INT_MAX (0xffffffff)`, call `reset_count()`.
8. Return 0.

### on_recv_check_result

Reads the response body at `pkg + 0x0a`: `uin_ +0`, `session_seq_ +8`,
`check_result_ +12` (absolute `+0x0a`, `+0x12`, `+0x16`).  `session_id_` is not
used.

1. Increment `recv_input_total_num_`; if the new value is `0xffffffff`, call
   `reset_count()`.
2. Look up `((uint64)uin << 32) | session_seq`.  Not found -> log
   `can not found node`, return **0**.
3. Increment `recv_input_valid_num_`; same `0xffffffff` reset rule.
4. Store `node.msg_result_ = check_result`.
5. If `msg_result_ != EMSG_EVIL_FLAG (1)`, run
   `UicTdirty::instance()->uic_dirty_check(node.message_, &flag)`; on failure
   log `... uic_dirty_check failed`, then store `node.msg_result_ = flag`.
   If `flag == MSG_DIRTY_FLAG` and `node.if_replace_ != 0`, call
   `uic_dirty_replace(node.message_)`; failure logs
   `... uic_dirty_replace failed`.
6. Build the 28-byte callback payload:
   `uin_, msg_result_(updated flag), message_len_, message_,
   callback_data_len_ (truncated to ushort), callback_data_, input_type_`.
7. If `this->callback_proc_` non-null call it with the payload; a nonzero
   return logs `callback_proc_ failed, ret = %d`.  Null callback emits
   `puts("callback_proc_ == NULL")` plus
   `... callback_proc_ == NULL` via `debug_errorex`.
8. Free `node.message_` with `this->user_info_mem_pool_`, erase the node from
   the bucket chain and LRU lists (use -> free, counts updated, chain/priority
   reset).  Erase failure logs `... erase failed` and returns `-1`.
9. Return 0.

### expire_user_input

`now = get_now_time()`, `expire_time = now - USER_INFO_EXPIRE_TIME (10)`.
The use list is traversed from `lst_use_node_->idx_prev_` (tail).  Empty uses
`idx == num_of_node_`; an entry is expired when
`priority_[idx] <= expire_time` (the code compares `expire_time < priority` and
stops).

For each expired node:

- dirty-check its message; failure logs
  `[uic sdk]UicTimeOutProc uic_dirty_check failed`;
- if result is `MSG_DIRTY_FLAG` and `if_replace_`, call
  `uic_dirty_replace`; failure logs the UicTimeOutProc replace string;
- build the same 28-byte payload, but `msg_result_flag_` is the fresh dirty
  result (not `node.msg_result_`);
- call the **static** `UicTimeOutProc::callback_proc_`; null logs the static
  null string, nonzero logs `[uic sdk]UicTimeOutProc callback_proc_ failed`;
- free the message with the **static** `UicTimeOutProc::user_info_mem_pool_`;
- increment the **static** `UicTimeOutProc::timeout_nodes_num_`;
- erase the node, increment the local expired count, and re-read the tail.

Returns the number of erased nodes.

### get_now_time / get_user_num / print_info / reset_count

- `get_now_time()`: `ZEN_OS::get_uptime()` returns an 8-byte timeval; result is
  `10*tv_sec + tv_usec/100000` — 100 ms ticks.
- `get_user_num()`: if `sequence_ % 8000 == 0`, call
  `user_info_mem_pool_->print_info()` then `print_info()`.  Return
  `lru_hash_head_->sz_usenode_`.
- `print_info()` logs via `debug_debugEx`:
  `add_input_num_ = %u, recv_input_total_num_ = %u, recv_input_valid_num_ = %u, timeout_nodes_num_ = %u\n`
  with `add`, `total`, `valid`, `UicTimeOutProc::timeout_nodes_num_`.
- `reset_count()`: zero `add_input_num_`, `recv_input_total_num_`,
  `recv_input_valid_num_`, and `UicTimeOutProc::timeout_nodes_num_`.

### UserInfo::init

`memset(this, 0, 400)`, then:

- `base_info_.game_id_ = game_id`, `uin_ = uin`, `room_id_ = room_id`;
- `user_code_info_.up_cmd_list_pos_ = -1` (`+0x90`),
  `down_cmd_list_pos_ = -1` (`+0x92`);
- `user_timer_.last_check_init_pkg_time_ = now` (`+0x94`),
  `last_recv_stat_pkg_time_ = now` (`+0x9c`),
  `last_send_test_pkg_time_ = now` (`+0x98`); the bin-code/bin-file timers
  remain zero;
- return 0.

## Call targets / external boundaries

All external calls use the same symbol names/ABI as the original (verified in
the compiled object's undefined-symbol table):

- `tInitializeDirtyCheck(char*, unsigned int)`
- `tDirtyCheck(int, char*, int*)`
- `tDirtyReplace(int, char*, int*)`
- `utf8DirtyCheck(int, char*, int*)`
- `utf8DirtyReplace(int, char*, int*)`
- `ZEN_OS::get_uptime()` (hidden sret timeval)
- `ZEN_Trace_LogMsg::debug_errorex(char const*, ...)`
- `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)`
- `BuddyAlloc::BuddyAlloc()`, `~BuddyAlloc()`,
  `init(unsigned int*, unsigned int, bool)`, `fini()`,
  `alloc(unsigned int)`, `free(void*)`, `print_info()`
- `operator new(unsigned int)`, `operator new(unsigned int, std::nothrow_t const&)`,
  `operator new[](unsigned int, std::nothrow_t const&)`,
  `operator delete(void*)`, `operator delete[](void*)`
- libc `memset`, `memcpy`, `puts`

The reconstructed source defines the four static storage objects listed above
and emits C1/C2/D1/D2 for both classes, plus the exact mangled names for every
assigned method.

## Compile result

Compiled with the c5root GCC 4.1.2 toolchain (target flags) — **exit 0, no
diagnostics**:

```text
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include \
  -I recon -c \
  recon/src/formmog/tss_sdk/plugins/proto_proc/uic_manager_agent.cpp \
  -o /tmp/uic_manager_agent.o
```

Compile-time size/offset checks in the source enforce: `UicTdirty` 8,
`UicUserInputInfoMgr` 32, `UicUserInputInfo` 36, callback payload 28,
`UserInfo` 400, `UserInfoCodeInfo` 48, hash wrapper 36, head 20, list index 8,
`UserMessage` 40, `UserInputCheckPkg` 1179 with body at `0x0a` and check result
at `0x16`.

## Unresolved / non-recoverable details

- The 28-byte callback payload has DWARF type name `._490` only; its C++
  typedef name was not emitted.  Its field offsets/size are exact.
- `UserInputCheckPkg` is packed in the original; the reconstruction models the
  request/response union with `__attribute__((packed))` so the body stays at
  `pkg + 0x0a`.
- `UserInfo::hack_data_` sub-structures (`DPHackBanModeInfo`,
  `DPHackCfUserInfo`, `DPHackDelayCmd`) are not read by any assigned method and
  are modeled as 140 opaque tail bytes after the named 40-byte prefix.
- `UicTdirty::is_inited_` is never set by `init` in the original binary.  This
  looks like a source bug but is preserved.
- The 36-byte hash wrapper and the `4*P` allocation slack are left allocated by
  `fini` exactly as in the original.
- No behavior was fabricated: every branch/return above is backed by the
  disassembly range listed in the table.
