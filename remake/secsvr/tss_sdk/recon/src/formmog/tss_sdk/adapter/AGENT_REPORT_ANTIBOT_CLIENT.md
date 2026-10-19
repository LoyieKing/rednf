# AntibotClient reconstruction report

Agent: `AntibotClient`
Source: `tools/tss_sdk_recon/recon/src/formmog/tss_sdk/adapter/antibot_client_agent.cpp`

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`
(31,904,293 B, ELF32 i386, DWARF, producer `GNU C++ 4.1.0 (SUSE Linux)`).
Scope: only the three `AntibotInterface` lifecycle methods below. No client/M3 work.

## 1. Exact symbols and ranges

| Method | Original source line | Symbol range (high_pc exclusive) | Size | Requested slice |
|---|---|---|---|---|
| `Initialize(IBusiness*, char*)` | 290 | `0x000889b0`..`0x00088b2a` | `0x17a` | `0x889b0`..`0x88b30` (tail padding) |
| `AddClient(int,int,unsigned int,unsigned int,TUserInfo*)` | 365 | `0x00089450`..`0x00089638` | `0x1e8` | `0x89450`..`0x89640` (tail padding) |
| `DeleteClient(int,int,unsigned int)` | 423 | `0x00089640`..`0x0008976b` | `0x12b` | `0x89640`..`0x89770` (tail padding) |

Exact mangled names emitted by the reconstruction (verified with `nm` on the
32-bit object):

```
_ZN16AntibotInterface10InitializeEP9IBusinessPc
_ZN16AntibotInterface9AddClientEiijjP9TUserInfo
_ZN16AntibotInterface12DeleteClientEiij
```

## 2. Header status

`build/tmp/tss_sdk_headers/src/formmog/tss_sdk/adapter/antibotinterface.h` is not
parseable: its included `iantibotinterface.h` contains `char szPlayerName[];`
and `int (***_vptr.IAntibotInterface)(...);` (invalid member name). The source
file therefore uses a minimal declaration set. `AntibotInterface` is declared
with the exact method signatures (virtuality omitted; it has no effect on the
mangled names or body ABI). `TUserInfo` and `IBusiness` are declared at global
scope with their real names so the mangled names stay exact; `IBusiness` is
opaque. The declaration set is partial, not the whole adapter header.

## 3. Globals, guards, and the SDK call table

`ebx` is loaded through `__i686.get_pc_thunk.bx` and equals `0x294534`
(start of `.got.plt`); all positive `ebx` offsets below are direct addresses
(all negative offsets used here are GOT entries).

| Item | Address | Evidence |
|---|---|---|
| `g_tss_sdk` | `0x0029d064`, 4 B, local `b` | `[ebx+0x8b30]`; `nm`: `g_tss_sdk`. DWARF type is pointer to the 28-byte init table (DIE `0xb0ca` -> `0x6dc7`). Written by the adapter's static initializer at `0x890bb`: `call tss_get_sdk@plt` (`0x80844`), `mov [ebx+0x8b30],eax`. Not written by these three methods. |
| `g_business` | `0x0029d1a0`, 4 B, local `b` | `[ebx+0x8c6c]`; `Initialize` stores its `busi` argument. DWARF type `IBusiness*`. |
| `g_channel_id` | `0x0029d1a4`, 4 B, local `b` | `[ebx+0x8c70]`; DWARF type `unsigned int`. |
| AddClient guard | `0x0029d178`, 8 B | `_ZGVZN16AntibotInterface9AddClientEiijjP9TUserInfoE18tmp_core_dump_info` |
| AddClient trace | `0x0029d180`, 29 B (`0x1d`) | `_ZZN16AntibotInterface9AddClientEiijjP9TUserInfoE18tmp_core_dump_info`; packed `char is_in_func_` + 28-byte `TssSdkPluginUserControlAddUser` (core packet at `+1`). |
| DeleteClient guard | `0x0029d160`, 8 B | `_ZGVZN16AntibotInterface12DeleteClientEiijE18tmp_core_dump_info` |
| DeleteClient trace | `0x0029d168`, 9 B | `_ZZN16AntibotInterface12DeleteClientEiijE18tmp_core_dump_info`; packed `char` + 8-byte `TssSdkPluginUserControlDeleteUser` (core packet at `+1`). |

`g_tss_sdk` is a pointer to a data table (not a C++ vtable): slot 0 is
`init(unsigned int)`, slot `+4` is
`init_plugins(const TssSdkPluginCallback*, unsigned int)`, slot `+0x10` is
`notify(TssSdkPluginNotifyData)`. Slots `+8`, `+0xc`, `+0x14`, `+0x18`
(`fini`, `proc`, `get_last_error_code`, `get_last_error`) are not called here.

No `IBusiness` virtual call occurs in any of these three methods; `Initialize`
only stores the pointer into `g_business`.

## 4. Decoded control flow

### Initialize (0x889b0..0x88b2a)

1. `debug_infoex("In AntibotInterface::Initialize. busi=%p, ipc_path_name=%s", busi, ipc_path_name)`.
2. If `busi == 0 || ipc_path_name == 0` -> return `-1` (no error log).
3. `ipc_path_name_num = (unsigned int)strtol(ipc_path_name, 0, 10)`.
4. `g_business = busi`.
5. `rc = g_tss_sdk->init(ipc_path_name_num)`. If `rc != 0`:
   `debug_errorex("In AntibotInterface::Initialize. rc=%d", rc)` and return `rc`.
6. `g_channel_id = ipc_path_name_num`.
7. `init_channel_data = { unique_instance_id_ = ipc_path_name_num,
   ipc_bus_size_ = 0x800000, pid_channel_mode_ = 0 }` (12-byte struct).
8. Build `plugin_array[5]` of 12-byte `{plugin_id_, callback_proc_, init_data_}`:
   `0: {1 CHANNEL, send_buffer_to_client_cb_, &unique_instance_id_}`
   `1: {3 PROTO_PROC, 0, 0}`
   `2: {2 USER_CONTROL, punish_user_cb_, 0}`
   `3: {4 SAFEMODE, process_safemode_cb_, 0}`
   `4: {5 CAPTCHA, 0, 0}`
9. `rc = g_tss_sdk->init_plugins(plugin_array, 5)` (return value not error-checked).
10. `debug_infoex("In AntibotInterface::Initialize, rc=%d, g_channel_id=%d,
    unique_instance_id=%d", rc, g_channel_id, init_channel_data.unique_instance_id_)`.
11. `g_channel_id = init_channel_data.unique_instance_id_` (the framework/channel
    callback may have updated it) and return `rc`.

Callbacks: `send_buffer_to_client_cb_` `0x89120`, `punish_user_cb_` `0x89260`,
`process_safemode_cb_` `0x888f0` (all `int(void*)`).

### AddClient (0x89450..0x89638)

`channel_id` and `room_id` are never read by the shipped body.

1. Build `user_control_pkg` (36 B: 4-byte head + 32-byte body union):
   `head_.msg_id_ = 1 (ADD_USER)`;
   `body_.add_user_ = { idx_=0, uin_=uin, user_id_=?, client_ver_=cli_ver,
   server_ip_=?, client_ip_=?, player_name_=? }`.
2. If `user_info != 0`: `user_id_=(unsigned)user_info->iUserID`,
   `server_ip_=user_info->uiSvrIp`, `client_ip_=user_info->uiCltIp`,
   `player_name_=user_info->szPlayerName` (offset 0, so also the `user_info`
   pointer). If `user_info == 0`: `user_id_=0xffffffff`, IPs already zero,
   `player_name_=0`.
3. `debug_debugEx("In AntibotInterface::AddClient,uin=%u, cli_ver=%u,
   user_id=%u, server_ip=%u, client_ip=%u, player_name=%s", ...)`.
4. Lazily construct the function-local static trace (`__cxa_guard_acquire`).
   Its inline ctor sets `is_in_func_=0` and logs
   `"CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p"`
   with `AddClient` / `TssSdkPluginUserControlAddUser`, then releases the guard.
5. `enter()` copies `body_.add_user_` (28 B) into the static trace and sets
   `is_in_func_=1`.
6. `notify_data = { plugin_id_=2 USER_CONTROL, notify_data_=&user_control_pkg }`;
   `ret = g_tss_sdk->notify(notify_data)` (8-byte struct passed by value).
7. `idx = (int)user_control_pkg.body_.add_user_.idx_` (the plugin writes this
   out field through the packet pointer).
8. If `ret == 0 && idx > 0`: debug log `"...AddClient,uin=%u, idx=%d"`.
   Else: error log `"...AddClient fail, uin=%u, ret=%d, idx=%d"`.
9. `leave()` (`is_in_func_=0`); return `idx`.

### DeleteClient (0x89640..0x8976b)

`channel_id` and `room_id` are never read by the shipped body.

1. Build `user_control_pkg`:
   `head_.msg_id_ = 2 (DELETE_USER)`;
   `body_.delete_user_.uin_ = uin`.
   **The shipped code never initializes `body_.delete_user_.idx_`** (no store
   before the trace copy or notify call); this reconstruction leaves it
   untouched as well.
2. `debug_debugEx("In AntibotInterface::DeleteClient, uin=%u", uin)`.
3. Lazily construct the static trace (`DeleteClient` /
   `TssSdkPluginUserControlDeleteUser`), same ctor log.
4. `enter()` copies `body_.delete_user_` (8 B) and sets `is_in_func_=1`.
5. `notify_data = {2 USER_CONTROL, &user_control_pkg}`;
   `g_tss_sdk->notify(notify_data)`. **The return value is ignored.**
6. `leave()`; return `0` unconditionally.

## 5. Reconstructed layouts (DWARF evidence)

- `TUserInfo`, sizeof 76: `szPlayerName[64]` @0, `iUserID` @64 (`int`),
  `uiSvrIp` @68, `uiCltIp` @72 (both `unsigned int`).
- `TssSdkPluginCallback`, sizeof 12: `plugin_id_` @0, `callback_proc_` @4
  (`int (*)(void*)`), `init_data_` @8 (`void*`).
- `TssSdkInitChannelData`, sizeof 12: `unique_instance_id_` @0,
  `ipc_bus_size_` @4, `pid_channel_mode_` @8.
- `TssSdkPluginNotifyData`, sizeof 8: `plugin_id_` @0, `notify_data_` @4.
- `TssSdkPluginUserControlPkg`, sizeof 36: `head_` @0, `body_` @4 (32 B).
- `TssSdkPluginUserControlAddUser`, sizeof 28: `idx_` @0, `uin_` @4,
  `user_id_` @8, `client_ver_` @0xc, `server_ip_` @0x10, `client_ip_` @0x14,
  `player_name_` @0x18.
- `TssSdkPluginUserControlDeleteUser`, sizeof 8: `idx_` @0, `uin_` @4.
- `CTssCoreDumpPkgTrace<T>` is packed: `is_in_func_` @0, `core_pkg_` @1;
  sizes 29 and 9 for the two T's above (matches `nm -S`).
- SDK init table, sizeof 28: `init` @0, `init_plugins` @4, `fini` @8,
  `proc` @0xc, `notify` @0x10, `get_last_error_code` @0x14,
  `get_last_error` @0x18.

The body union is padded to 32 bytes in the reconstruction because only
`add_user_` and `delete_user_` are touched here; this preserves the 36-byte
packet size and the `body_` offset expected by the user-control plugin.

## 6. PLT targets (resolved with `nm -C`)

| Original call site | PLT address | Target |
|---|---|---|
| Initialize / AddClient / DeleteClient | `0x84d14` | `ZEN_Trace_LogMsg::debug_infoex(char const*, ...)` |
| Initialize / AddClient | `0x80904` | `ZEN_Trace_LogMsg::debug_errorex(char const*, ...)` |
| AddClient / DeleteClient | `0x7c9b4` | `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)` |
| Initialize | `0x82a34` | `__strtol_internal` (target glibc lowering of `strtol(nptr,0,10)`, group=0) |
| trace guards | `0x7b5f4` / `0x81b84` / `0x7ca94` | `__cxa_guard_acquire` / `__cxa_guard_release` / `__cxa_guard_abort` |
| guard cleanup | `0x86de4` | `_Unwind_Resume` |
| static init (outside scope) | `0x80844` | `tss_get_sdk()` -> `g_tss_sdk` |

Exact format/type strings used by the reconstruction:

```
In AntibotInterface::Initialize. busi=%p, ipc_path_name=%s
In AntibotInterface::Initialize. rc=%d
In AntibotInterface::Initialize, rc=%d, g_channel_id=%d, unique_instance_id=%d
In AntibotInterface::AddClient,uin=%u, cli_ver=%u, user_id=%u, server_ip=%u, client_ip=%u, player_name=%s
In AntibotInterface::AddClient,uin=%u, idx=%d
In AntibotInterface::AddClient fail, uin=%u, ret=%d, idx=%d
In AntibotInterface::DeleteClient, uin=%u
CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p
```

## 7. Compile check

```
wsl -u root bash -lc "cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon && \
  g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -Wall -Wextra \
  -c recon/src/formmog/tss_sdk/adapter/antibot_client_agent.cpp \
  -o /tmp/antibot_client_agent.o"
```

Result: exit 0, no warnings. `nm -S` on the object confirms the three exact
mangled `T` symbols, both guard symbols, and the static trace objects with
sizes 29 (`0x1d`) and 9. `readelf -sW` confirms undefined references are only
`g_tss_sdk`, `g_business`, `g_channel_id`, the three callbacks, and the three
logger methods.

Note: the local WSL toolchain is newer than the target's GCC 4.1/glibc. It
lowers `strtol` to `__isoc23_strtol` and emits stack-protector code; the target
object calls `__strtol_internal(nptr,0,10,0)` and has no stack protector. This
is a build-environment difference, not a control-flow difference. For a
byte-oriented rebuild use the original GCC 4.1 `-m32 -std=gnu++98 -O2 -fPIC
-fno-implement-inlines` environment (and `-fno-stack-protector` if desired).

## 8. Dependencies, unknowns, integration boundary

- `g_tss_sdk`, `g_business`, `g_channel_id` are **extern** in the source. In the
  original they are TU-local (`b`) globals shared by all methods of
  `adapter/antibotinterface.cpp`. The integration owner must provide exactly
  one definition set for the merged adapter TU; do not define them twice in
  other reconstructed adapter files.
- `g_tss_sdk` initialization (`tss_get_sdk()` at `0x890bb`) is outside these
  three methods and is not reconstructed here. On entry to `Initialize`, the
  pointer is assumed non-null as in the original.
- The logger implementation, the three callbacks, and `IBusiness` behavior are
  not reconstructed. `IBusiness` is intentionally opaque; no vtable calls are
  faked.
- `DeleteClient` leaves `body_.delete_user_.idx_` uninitialized, matching the
  original instructions (the value is copied to the trace and passed to the
  plugin packet). This is an explicit observed boundary, not an invented
  default.
- `channel_id`/`room_id` are accepted and ignored by both client methods, as in
  the binary.
- `TssSdkPluginUserControlBody` is padded to 32 bytes for size/offset
  compatibility; the punish/get-info members are not modelled because these
  methods never touch them.
- The trace helper is an inline packed template; `always_inline` is used so the
  guard/ctor sequence also works under `-fno-implement-inlines` (verified at
  `-O2` and `-O0`).
- Compile this `.cpp` as its own TU. Its minimal `IBusiness`, `TUserInfo`,
  `ZEN_Trace_LogMsg`, `AntibotInterface`, etc. declarations would collide with
  a real header force-included into the same TU; that header is broken/unusable
  anyway (see section 2).
- **Not integrated, not linked, not run against the live game, and not claimed
  byte-identical.** This is a partial TU implementing only the three named
  methods.
