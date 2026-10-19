# TssSdkCommSvc reconstruction report

Source: `tss_sdk_comm_svc_agent.cpp` (this directory).

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`,
ELF32 i386, SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
Original TU: `src/formmog/tss_sdk/framework/tss_sdk_comm_svc.cpp`
(DWARF producer GCC 4.1.0; c5root GCC 4.1.2 used for the check build).

Only the assigned `TssSdkCommSvc` methods below are defined. No shared header,
CMake, test, README or existing source was modified.

## Address ranges

`high_pc` values are taken from the DWARF subprogram DIEs; the next symbol
usually starts after a few alignment bytes.

| start | high_pc | symbol |
|---|---|---|
| `0x0008cf20` | `0x0008cf2e` | `is_need_trans_uin(unsigned int)` |
| `0x0008cfd0` | `0x0008cff8` | `get_last_error_code()` |
| `0x0008d000` | `0x0008d02b` | `set_errno(int)` |
| `0x0008d030` | `0x0008d078` | `~TssSdkCommSvc()` D1 |
| `0x0008d0b0` | `0x0008d0f8` | `~TssSdkCommSvc()` D2 |
| `0x0008d100` | `0x0008d20f` | `TssSdkCommSvc()` C1 |
| `0x0008d6c0` | `0x0008d7cf` | `TssSdkCommSvc()` C2 |
| `0x0008d7d0` | `0x0008d8a3` | `get_last_error()` |
| `0x0008d8b0` | `0x0008e502` | `get_user_info_by_uin(unsigned int)` |
| `0x0008e510` | `0x0008f142` | `get_user_info(unsigned int&, unsigned int)` |
| `0x0008f150` | `0x0008fbd5` | `recv_buf_from_backend(const char**, size_t*)` |
| `0x0008fbe0` | `0x00090885` | `send_buf_to_backend(const char*, size_t, bool, int)` |
| `0x00090890` | `0x000914ed` | `send_buf_to_client(int, unsigned int, unsigned char*, short)` |

DWARF definition lines: `is_need_trans_uin` 347, `get_last_error_code` 318,
`set_errno` 48, `get_last_error` 324, `get_user_info_by_uin` 277,
`get_user_info` 235, `recv_buf_from_backend` 104, `send_buf_to_backend` 161,
`send_buf_to_client` 57. Class declaration lines: ctor 47, dtor 48,
`get_user_info` 51, `get_user_info_by_uin` 52, `send_buf_to_client` 70,
`recv_buf_from_backend` 73, `send_buf_to_backend` 76, `is_need_trans_uin` 103,
`get_last_error_code` 111, `get_last_error` 113, `set_errno` 115.

## Object layout

`sizeof(TssSdkCommSvc) == 40`, confirmed from DWARF and from
`-fdump-class-hierarchy` on the reconstruction:

| offset | member |
|---|---|
| `+0x00` | `unsigned int channel_id_` |
| `+0x04` | `TssSdkPluginMgr *plugin_mgr_` |
| `+0x08` | `DPInterface *dp_if_` |
| `+0x0c` | `TssSdkChannelStatus sdk_channel_status_` (24 bytes) |
| `+0x24` | `CThreadErrorInfo *error_info_` |

`TssSdkChannelStatus` (DWARF name, `sizeof == 24`):

| offset | counter |
|---|---|
| `+0x00` (`this+0x0c`) | `total_recv_` |
| `+0x04` (`this+0x10`) | `succ_recv_` |
| `+0x08` (`this+0x14`) | `fail_recv_` |
| `+0x0c` (`this+0x18`) | `total_send_` |
| `+0x10` (`this+0x1c`) | `succ_send_` |
| `+0x14` (`this+0x20`) | `fail_send_` |

`CThreadErrorInfo` is the 4-byte `pthread_key_t` object from
`zen_thread_error_info.h`; `last_error_info()` returns the keyed
`SErrorInfo { int error_code; char error_msg[256]; }` block. Only
`error_code` is read/written here.

There is **no user table, shared-memory segment, IPC fd or socket fd** in
this object. The user table belongs to the user-control plugin and the
transport handles belong to the channel plugin; `TssSdkCommSvc` only holds the
manager/interface pointers and the six counters above.

## Reconstructed semantics

### ctor / dtor

Ctor, in observed order (0x8d100):

1. `channel_id_ = 0`
2. `error_info_ = new CThreadErrorInfo()`
3. `error_info_->last_error_info()->error_code = 0`
4. `plugin_mgr_ = Singleton<TssSdkPluginMgr>::instance()`
5. `dp_if_ = DPInterface::instance()`
6. zero all six `sdk_channel_status_` fields

The manager singleton guard/atexit block is inlined in the original; the
reconstruction calls the real `Singleton<TssSdkPluginMgr>::instance()` symbol.
Dtor deletes `error_info_` (with its real `CThreadErrorInfo` dtor + `operator
delete`) and stores null; no other member is owned.

### error helpers

- `is_need_trans_uin(game_id)` returns `game_id == 3` (`cmp ...,3; sete`).
- `set_errno(err_no)` writes `error_info_->last_error_info()->error_code`.
- `get_last_error_code()` reads the same field.
- `get_last_error()` returns an empty `std::string` when the code is 0;
  otherwise it returns `error_info_->get_last_error_msg()` (compiled as
  `strlen` + `basic_string::assign(ptr,len)`), exactly the two paths at
  0x8d7d0.

The original inlines the two tiny helpers at every error site but still emits
their out-of-line bodies (0x8cfd0 / 0x8d000). The reconstruction keeps the
out-of-line definitions and uses the same inlined reads/writes at call sites,
matching the original `last_error_info()` call count and order.

### User lookup (plugin delegation)

Both methods first call
`plugin_mgr_->get_active_plugin(ETSS_SDK_PLUGIN_ID_USER_CONTROL /*2*/)`.
The user-control plugin owns the user table; `TssSdkCommSvc` forwards a
`TssSdkPluginUserControlPkg` through `ITssSdkPlugin::notify(void*)`
(vtable slot at `vptr+0x14`).

`TssSdkPluginUserControlPkg` (`sizeof == 36`):

| offset | field |
|---|---|
| `+0x00` | `head_.msg_id_` (4-byte enum) |
| `+0x04` | `body_.user_info_.uin_` |
| `+0x08` | `body_.user_info_.idx_` |
| `+0x0c` | `body_.user_info_.user_info_` (`UserInfo*`) |

The union is 32 bytes; the largest member is the 32-byte punish packet, so the
user-info member starts at `+0x04` and the packet remains 36 bytes.

`get_user_info_by_uin(uin)`:

- null plugin: builds the error text below, sets code `0x49449`, returns `0`.
  At `0x8dd5e` the original calls `ITssSdkPlugin::get_id()` with `this == 0`;
  this upstream null-dereference bug is preserved.
- success: `msg_id_ = GET_USERINFO_BY_UIN(5)`, `uin_ = uin`, `idx_ = 0`,
  `user_info_ = 0`; after `notify() == 0` returns `body_.user_info_.user_info_`.
- notify failure: builds the error text with the plugin's last error/id, sets
  code `0x4944a`, returns `0`.

`get_user_info(uin_ref, idx)` is the same with `msg_id_ = GET_USERINFO(4)`,
`idx_ = idx`, and on success copies the plugin-updated
`body_.user_info_.uin_` back through `uin_ref` before returning
`body_.user_info_.user_info_`. Its null-plugin message uses constant 2, not
`get_id()`.

### Backend receive (channel plugin `proc`)

`recv_buf_from_backend(recv_buf, recv_buf_size)`:

1. `plugin = plugin_mgr_->get_active_plugin(ETSS_SDK_PLUGIN_ID_CHANNEL /*1*/)`.
2. Null plugin: code `0x49449`, error text, return the code.
3. Call `plugin->proc(&channel_data)` (vtable slot at `vptr+0x10`), where the
   local is exactly
   `struct { const char *buf; size_t buf_size; } channel_data;` (8 bytes,
   uninitialized before the call). Increment `total_recv_` immediately.
4. `rc == 0`: increment `succ_recv_`, store `*recv_buf = channel_data.buf`,
   `*recv_buf_size = channel_data.buf_size`, return `0`.
5. `rc == 0x4957a` (proc timeout): set code `0x4944c`; **do not** increment
   `fail_recv_`; fall into the common error-text builder, then return the
   error code.
6. Any other non-zero: increment `fail_recv_`, set code `0x4944b`, build the
   error text, return the code.

The error text is built *after* the code is written in the failure path; the
return value is a fresh read of `error_info_->last_error_info()->error_code`.

### Backend send (channel plugin `notify`)

`send_buf_to_backend(send_buf, size, use_ipc, business_id)`:

1. `send_buf == NULL`: message
   `"In CTssSdkCommSvc::send_buf_to_backend, send_buf is NULL."`, code
   `0x49448`, return the code.
2. `plugin = get_active_plugin(CHANNEL /*1*/)`; null plugin: code `0x49449`
   with manager error text, return the code.
3. Build `TssSdkPluginChannelPkg` (`sizeof == 18`, packed):
   - `head_.type_id_` at `+0`
   - IPC branch (`use_ipc != false`): type 1, `buf_` at `+4`, `buf_size_`
     (`unsigned int`) at `+8`
   - socket branch: type 2, `business_id_` at `+4`, `buf_` at `+8`,
     `buf_size_` (`short`, low 16 bits of the `size_t`) at `+12`
4. `rc = plugin->notify(&pkg)` (vtable `vptr+0x14`); increment `total_send_`.
5. `rc == 0`: increment `succ_send_`, return `0`.
6. Otherwise increment `fail_send_` first.
   - `rc == 0x4957b` (notify timeout): get the plugin last-error string and
     call `ZEN_Trace_LogMsg::debug_debugEx("%s", str.c_str())`; return `0`
     without touching the stored error code.
   - Other: build the error text, code `0x4944a`, return the code.

### Client send (game channel `notify`)

`send_buf_to_client(busi_type, uin, buf, buf_size)`:

1. `plugin = get_active_plugin(CHANNEL /*1*/)`; null plugin: code `0x49449`,
   error text, return the code.
2. Build a channel packet with `head_.type_id_ = 3` (game channel):
   `busi_type_` `+4`, `uin_` `+8`, `buf_` `+12`, `buf_size_` `+16` (`short`).
3. `rc = plugin->notify(&pkg)`.
4. `rc == 0`: return `0`. This path does not modify any channel counter.
5. Non-zero: error text, code `0x4944a`, return the code.

### Wire layouts

User-control message (36 bytes):

```
+0x00 int msg_id
+0x04 union {
         add_user:     idx, uin, user_id, client_ver, server_ip, client_ip,
                       const char *player_name                  // 28 B
         delete_user:  idx, uin                                  //  8 B
         punish_user:  uin, u16 game_id, u16 room_id, i32
                       punish_mode, i32 punish_time, u32
                       punish_info_len, const u8 *punish_info,
                       u32 punish_reason_len, const u8 *punish_reason // 32 B
         user_info:    uin, idx, UserInfo *user_info             // 12 B
      }
```

Channel message (18 bytes, packed):

```
+0x00 int type_id
+0x04 union {
         ipc:     const char *buf; u32 buf_size                  //  8 B
         socket:  i32 business_id; const char *buf; short size   // 10 B
         game:    i32 busi_type; u32 uin; u8 *buf; short size    // 14 B
      }
```

Type ids: IPC 1, socket 2, game 3. Message ids: add 1, delete 2, punish 3,
get-user-info 4, get-user-info-by-uin 5. All are the real enumerator names
from DWARF (`TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_*`,
`TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_*`). The wrapper structs themselves are
anonymous (`._496`, `._504`) in DWARF; the local names used here follow the
plugin API names. The inner IPC/socket/game/user-control package names are
confirmed by the demangled plugin entry points.

## Error strings and codes

Strings are byte-exact, including the original missing/extra spaces:

| use | address | text |
|---|---|---|
| user null prefix | `0x2197d4` | `In CTssSdkCommSvc::get_user_info, plugin mgr gets active plugin` |
| user notify prefix | `0x219814` | `In CTssSdkCommSvc::get_user_info, plugin ` |
| user notify suffix | `0x2199eb` | ` notify failed. ` |
| recv null prefix | `0x219840` | `In CTssSdkCommSvc::recv_buf_from_backend, plugin mgr gets active plugin ` |
| recv proc prefix | `0x21988c` | `In CTssSdkCommSvc::recv_buf_from_backend, plugin ` |
| recv suffix | `0x2199fc` | ` proc failed. ` |
| send-backend NULL | `0x2198c0` | `In CTssSdkCommSvc::send_buf_to_backend, send_buf is NULL.` |
| send-backend null-plugin prefix | `0x2198fc` | `In CTssSdkCommSvc::send_buf_to_backend, plugin mgr gets active plugin ` |
| send-backend notify prefix | `0x219944` | `In CTssSdkCommSvc::send_buf_to_backend, plugin ` |
| send-backend notify suffix | `0x2199eb` | ` notify failed. ` |
| send-client null-plugin prefix | `0x219974` | `In CTssSdkCommSvc::send_buf_to_client, plugin mgr gets active plugin` |
| send-client notify prefix | `0x2199bc` | `In CTssSdkCommSvc::send_buf_to_client, plugin ` |
| send-client notify suffix | `0x2199ec` | `notify failed. ` |
| timeout log format | `0x21b5d8` | `%s` |

The null-plugin suffixes are `0x219a02` = `failed. ` (user and
send-client) and `0x219a01` = ` failed. ` (recv and send-backend), concatenated
after the decimal plugin id, matching the original text exactly.

Codes written to `error_info_`:

| code | decimal | meaning |
|---|---|---|
| `0x49448` | 300104 | `send_buf_to_backend` NULL buffer |
| `0x49449` | 300105 | `get_active_plugin` returned null |
| `0x4944a` | 300106 | `ITssSdkPlugin::notify` failed |
| `0x4944b` | 300107 | `ITssSdkPlugin::proc` failed |
| `0x4944c` | 300108 | `ITssSdkPlugin::proc` timeout |
| `0x4957a` | 300410 | proc-timeout sentinel return |
| `0x4957b` | 300411 | notify-timeout sentinel return (debug-logged, method returns 0) |

No symbolic names for these seven constants survive in DWARF; they are
reproduced as `static const int` file-local constants.

## External boundaries (all left undefined)

| target | address in target | exact signature |
|---|---|---|
| `TssSdkPluginMgr::get_active_plugin` | `0x00098740` | `ITssSdkPlugin *get_active_plugin(tss_sdk_if::ETssSdkPluginId)` |
| `TssSdkPluginMgr::get_last_error` | `0x00097bd0` | `std::string get_last_error()` |
| `ITssSdkPlugin::get_last_error` | `0x000972d0` | `std::string get_last_error()` |
| `ITssSdkPlugin::get_id` | `0x00097120` | reads `this+0x0c`; return type is the plugin-id enum/int |
| `ITssSdkPlugin::proc` | virtual, `vptr+0x10` | `int proc(void *)` |
| `ITssSdkPlugin::notify` | virtual, `vptr+0x14` | `int notify(void *)` |
| `CThreadErrorInfo::last_error_info` | `0x001ad390` | `SErrorInfo *last_error_info()` |
| `CThreadErrorInfo::set_last_error_msg` | `0x001ad470` | `void set_last_error_msg(const char *, ...)` |
| `CThreadErrorInfo::get_last_error_msg` | `0x001ad440` | `const char *get_last_error_msg()` |
| `CThreadErrorInfo::CThreadErrorInfo` | `0x001ad540` | `CThreadErrorInfo()` |
| `CThreadErrorInfo::~CThreadErrorInfo` | `0x001ad4e0` | `~CThreadErrorInfo()` |
| `ZEN_Trace_LogMsg::debug_debugEx` | `0x001b3dd0` | `void debug_debugEx(const char *, ...)` |
| `Singleton<TssSdkPluginMgr>::instance` | `0x000914f0` | `TssSdkPluginMgr *instance()` |
| `DPInterface::instance` | `0x000bf160` | `DPInterface *instance()` |
| `operator new` / `operator delete` | PLT | 4-byte `CThreadErrorInfo` |
| `strlen` | PLT | used by the non-empty `get_last_error()` branch |

The virtual-slot offsets are verified from the shipped vtables: address point is
`_ZTV+8`; `init/fini/proc/notify` occupy `+0x08/+0x0c/+0x10/+0x14`.
Every receive path uses `+0x10`; every send/user-info path uses `+0x14`.

No OS, shared-memory, socket or IPC helper is called from this TU. Those
operations are behind the channel plugin's `proc`/`notify` boundary; the
reconstruction deliberately defines no such helpers and fabricates no
transport success.

## Compile result

Built with the closest available compiler (c5root GCC 4.1.2):

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
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_agent.cpp \
  -o /tmp/tss_sdk_comm_svc_c5.o
```

Result: **exit 0, no diagnostics**. Defined symbols (mangled, `nm -C`):

```
TssSdkCommSvc::TssSdkCommSvc()                          C1
TssSdkCommSvc::TssSdkCommSvc()                          C2
TssSdkCommSvc::~TssSdkCommSvc()                         D1
TssSdkCommSvc::~TssSdkCommSvc()                         D2
TssSdkCommSvc::is_need_trans_uin(unsigned int)
TssSdkCommSvc::get_last_error_code()
TssSdkCommSvc::set_errno(int)
TssSdkCommSvc::get_last_error()
TssSdkCommSvc::get_user_info_by_uin(unsigned int)
TssSdkCommSvc::get_user_info(unsigned int&, unsigned int)
TssSdkCommSvc::recv_buf_from_backend(char const**, unsigned int*)
TssSdkCommSvc::send_buf_to_backend(char const*, unsigned int, bool, int)
TssSdkCommSvc::send_buf_to_client(int, unsigned int, unsigned char*, short)
```

Class-size checks from `-fdump-class-hierarchy`: `TssSdkCommSvc` 40,
`TssSdkChannelStatus` 24, `CThreadErrorInfo` 4, user-control pkg 36 (body 32),
channel pkg 18 (body 14; IPC 8, socket 10 packed, game 14 packed),
`recv_buf_from_backend` local 8.

Codegen-only differences from the original: the original inlines the
`basic_ostringstream` constructor and the `Singleton<TssSdkPluginMgr>`
guard/instance body; the c5root 4.1.2 headers/compiler emit a call to the real
`basic_ostringstream::C1E` and to
`Singleton<TssSdkPluginMgr>::instance()` instead. The reconstructed error-path
`last_error_info()` call count (24 total in the object) and order match the
original exactly.

## Unresolved / out of scope

- The user table itself is in the user-control plugin; the IPC/socket/shm
  transport and fd handles are in the channel plugin. Only the `TssSdkCommSvc`
  side of those boundaries is reconstructed.
- Exact symbolic names of error codes `0x49448`-`0x4944c`, `0x4957a`,
  `0x4957b` are absent from DWARF; literals are used.
- The package wrapper types are anonymous in DWARF; names were chosen from
  the plugin API signatures where available and are reconstruction-local for
  the two wrappers.
- `get_user_info_by_uin`'s null-plugin branch calls `get_id()` through a null
  `this` in the shipped code (`0x8dd5e`). Reproduced as-is; it is an upstream
  defect, not fabricated behavior.
