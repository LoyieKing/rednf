# TssSdkFramework dispatch methods

Source: `libtss_sdk.so.1.0.394`
(ELF32 i386, DWARF producer GCC 4.1.0; check build c5root GCC 4.1.2).

New unit:
`recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_framework_dispatch_agent.cpp`

The four methods are **static** members of `tss_sdk_impl::TssSdkFramework`.
DWARF lists no incoming `this` parameter for any of them, and the shipped
prologues read their first argument at `[ebp+8]`.

| Method | ORIG address | ORIG size | DWARF source lines |
|---|---:|---:|---|
| `notify(TssSdkPluginNotifyData)` | `0x00093230` | `0x955` | 715-769 |
| `init_plugins(TssSdkPluginInitData const*, unsigned int)` | `0x00093b90` | `0x1305` | 250-361 |
| `proc_buf_from_backend_(char const*, size_t)` | `0x00094ea0` | `0x945` | 447-557 |
| `proc()` | `0x000957f0` | `0xeb5` | 563-711 |

DWARF locals used as anchors:

- `notify`: `plugin_notify_data` 715, `plugin` 730, `rc` 747.
- `init_plugins`: `plugin_init_data_table`/`plugin_count` 250, `rc` 265,
  `plugin_init_data_vec` 282, `iter_channel`/`iter` 283, `scope_lock` 316,
  `plugin` 318.
- `proc_buf_from_backend_`: `recv_buf`/`recv_buf_size` 447, `rc` 449,
  `app_frame` 451, `dest_plugin_id` 453, `plugin` 525.
- `proc`: `s_last_proc_thread_id` 569, `now` 595,
  `s_last_count_timestamp` 596, `max_recv_pkg_per_time` 598,
  `proc_call_times` 599, `rc` 627, `plugin` 628, `proc_pkg_count` 637,
  `iter` 674.

## Shared type/ABI facts

### tss_sdk_if plugin data

`TssSdkPluginInitData` (`tss_sdk_plugin.h:41`, sizeof 12):

| off | type | field |
|---:|---|---|
| +0x00 | `ETssSdkPluginId` (4 bytes) | `plugin_id_` |
| +0x04 | `int (*)(void*)` | `callback_proc_` |
| +0x08 | `void*` | `init_data_` |

`TssSdkPluginNotifyData` (`tss_sdk_plugin.h:49`, sizeof 8, passed by value):

| off | type | field |
|---:|---|---|
| +0x00 | `ETssSdkPluginId` | `plugin_id_` |
| +0x04 | `void*` | `notify_data_` |

`ETssSdkPluginId`: INVALID=-2, ALL=-1, BEGIN=0, CHANNEL=1,
USER_CONTROL=2, PROTO_PROC=3, SAFEMODE=4, CAPTCHA=5, UIC=6,
DATA_TRANS=7, END=8.

### ITssSdkPlugin

sizeof 16; vtable at `0x00292d80`, address point `0x00292d88`:

| slot | offset | target |
|---:|---:|---|
| 0 | +0x00 | D1 destructor `0x00097250` |
| 1 | +0x04 | D0 deleting destructor `0x00097200` |
| 2 | +0x08 | `init(void*)` |
| 3 | +0x0c | `fini()` |
| 4 | +0x10 | `proc(void*)` |
| 5 | +0x14 | `notify(void*)` |

Layout: vptr +0x00, `callback_proc_` +0x04, `CThreadErrorInfo error_info_`
+0x08 (4 bytes), `sdk_plugin_id_` +0x0c.

Non-virtual direct boundaries used by this unit:
`register_callback(int(*)(void*))` `0x00097110`,
`get_id()` `0x00097120`, `get_last_error_code()` `0x000971d0`,
`get_last_error()` `0x000972d0` (returns `std::string` by value).

### Other layouts

- `TssSdkCommSvc`, sizeof 40: `channel_id_` +0x00, `plugin_mgr_` +0x04,
  `dp_if_` +0x08, six counters at +0x0c (24 bytes), `error_info_` +0x24.
  Used calls: `recv_buf_from_backend(char const**, size_t*)` `0x0008f150`,
  `get_last_error()` `0x0008d7d0`.
- `TssSdkPluginMgr`, sizeof 56: `registered_plugin_count_` +0x00,
  vector invalid +0x04, repeated +0x10, active +0x1c,
  `std::vector<std::pair<ITssSdkPlugin*,bool> >` +0x28,
  `CThreadErrorInfo` +0x34. Used calls: `get_registered_plugin_count()`
  `0x00097990`, `init()` `0x00099f60`, `activate_plugin()` `0x00099830`,
  `get_active_plugin()` `0x00098740`, `get_last_error()` `0x00097bd0`.
- `CThreadErrorInfo`, sizeof 4; `last_error_info()` `0x001ad390` returns
  `SErrorInfo*` (int `error_code`, then `char error_msg[256]`),
  `get_last_error_msg()` `0x001ad440`, `set_last_error_msg()` `0x001ad470`.
- `ZEN_Thread_Mutex`: `lock()` `0x001a11f0`, `unlock()` `0x001a1170`.
- `ZEN_Trace_LogMsg`: `debug_errorex` `0x001b3d30`, `debug_infoex`
  `0x001b3d80`, `debug_debugEx` `0x001b3dd0`.
- Framework static state, referenced through the GOT:
  `is_init_succ_` `0x002a00ab`, `thread_mutex_` `0x002a00ac`,
  `error_info_` `0x002a00c8`, `plugin_mgr_` `0x002a00cc`,
  `comm_svc_` `0x002a00d0`, `not_need_channel_plugin_id_table_`
  `0x002a00d4` (`std::vector<ETssSdkPluginId>`, 12 bytes),
  `timer_handler_` `0x002a00e0`.
- Commands read from `Comm_App_Frame`: `frame_command_` is +0x08.
  For command 300001, the 16-bit big-endian business id is at
  `recv_buf + 0x3a` = appdata (+0x32) + 0x08, i.e.
  `sec_proto::SendToSDK::to_busi_id_`.

## notify 0x93230

Semantics:

1. `if (!is_init_succ_) return 0;`
2. Scoped timer lock: `time(0)` -> `check_monitor(now)` ->
   `update_time(now)` -> unlock. The lock does **not** cover the plugin call.
3. `plugin = plugin_mgr_->get_active_plugin(plugin_notify_data.plugin_id_)`.
   If null:
   - message
     `"In CTssSdkFramework::notify, plugin mgr get active plugin "`
     + id + `" failed. "` + `plugin_mgr_->get_last_error()`;
   - `set_last_error_msg`, `debug_errorex("%s", get_last_error_msg())`;
   - code `0x494b1` (300209); return that code.
4. `rc = plugin->notify(plugin_notify_data.notify_data_)` via vtable +0x14.
   If `rc == 0`, return 0.
5. On nonzero `rc`:
   - message `"In CTssSdkFramework::notify, plugin "` + id +
     `" noitfy failed. "` + `"rc is "` + rc + `". "` +
     `plugin->get_last_error()` (the `noitfy` typo is in ORIG);
   - `set_last_error_msg`, `debug_errorex("%s", get_last_error_msg())`;
   - code `0x494ad` (300205); return that code. The plugin's `rc` appears
     only in the message; it is not the return value.

## init_plugins 0x93b90

Semantics:

1. `plugin_init_data_table == NULL || plugin_count == 0`:
   message `"In CTssSdkFramework::init_plugins, plugin_init_data_table is
   NULL or plugin_count <= 0."`, log, code `0x494a9` (300201), return it.
2. `debug_infoex("The count of registered plugins is %d.",
   plugin_mgr_->get_registered_plugin_count())`.
3. `plugin_mgr_->init() != 0`:
   message `"In CTssSdkFramework::init_plugins, plugin mgr init failed. "`
   + `plugin_mgr_->get_last_error()`, log, code `0x494ae` (300206), return.
4. Range-copy the caller table into
   `std::vector<TssSdkPluginInitData> plugin_init_data_vec`.
5. `find_if(..., is_equal_to_channel_plugin)`; if found, swap that element
   with `plugin_init_data_vec[0]`. Then
   `find_if(..., is_equal_to_usercontrol_plugin)`; if USER_CONTROL is
   present, call `tss_dp_comm::init_dp_config()` (`0x000a9030`). Nonzero
   result uses the same `"plugin mgr init failed. "` message and code
   `0x494ae`.
   The predicates are the shipped local functions `0x000916e0`
   (`id == 1`) and `0x000916c0` (`id == 2`).
6. Under `ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_)`:
   loop `i = 0 .. plugin_count-1` over the **original caller table** (not
   the vector; the vector copy/swap is never used for activation):
   - `plugin = plugin_mgr_->activate_plugin(table[i].plugin_id_)`
   - null -> message
     `"In CTssSdkFramework::init_plugins, plugin mgr activate plugin "`
     + id + `" failed. "` + manager error, log, code `0x494af` (300207),
     return.
   - `plugin->register_callback(table[i].callback_proc_)`
   - `rc = plugin->init(table[i].init_data_)` via vtable +0x08.
   - `rc != 0` -> message
     `"In CTssSdkFramework::init_plugins, plugin "` + id +
     `" init failed. "` + `plugin->get_last_error()`, log, code `0x494aa`
     (300202), return. The plugin's `rc` is not propagated.
7. On success, `get_not_need_channel_plugin_id_table_()` (`0x00091b30`)
   is called inside the lock; then return 0.

`get_not_need_channel_plugin_id_table_()` copies
`TssSdkPluginMgr::get_active_plugin_id_table()` and removes ids 1, 4, 5, 3
in that order; `proc()` later polls the remaining active ids.

The error paths inside the lock destroy their `ostringstream`, unlock and
free the vector before returning; the RAII guard/local vector reproduce that
order.

## proc_buf_from_backend_ 0x94ea0

`recv_buf_size` is never read by ORIG; there is no NULL or minimum-size
check. `app_frame` is the same pointer as `recv_buf`; only
`frame_command_` (+0x08) is read directly.

Destination selection:

| `frame_command_` | destination |
|---:|---|
| `0x493e1` (300001) | `get_plugin_id(ntohs(*(unsigned short*)(recv_buf+0x3a)))` |
| `0x493e3` (300003) | USER_CONTROL (2) |
| `0x493e5` (300005) | PROTO_PROC (3) |
| `0x493e7` (300007) | PROTO_PROC (3) |
| `0x493ea` (300010) | PROTO_PROC (3) |
| `0x493ec` (300012) | SAFEMODE (4) |
| `0x493ee` (300014) | SAFEMODE (4) |
| `0x493ef` (300015) | UIC (6) |
| anything else | INVALID (-2), after `debug_errorex("%s, invalid cmd %u", __PRETTY_FUNCTION__, frame_command_)` |

`__PRETTY_FUNCTION__` in ORIG is exactly
`static int tss_sdk_impl::TssSdkFramework::proc_buf_from_backend_(const char*, size_t)`
(86 bytes including NUL).

All paths then:

1. `debug_debugEx("In CTssSdkFramework::proc_buf_from_backend_, dest_plugin_id=%d.", dest)`.
2. `plugin = plugin_mgr_->get_active_plugin(dest)`.
   Null -> `"In CTssSdkFramework::proc_buf_from_backend_, Plugin mgr get active plugin "`
   + dest + `" failed. "` + manager error; code `0x494b1` (300209);
   **no `debug_errorex`**; return code.
3. `rc = plugin->proc(recv_buf)` via vtable +0x10. Zero -> return 0.
4. Nonzero -> `"In CTssSdkFramework::proc_buf_from_backend_, plugin "`
   + dest + `" proc failed. "` + plugin error; code `0x494ac` (300204);
   **no `debug_errorex`**; return code.

The invalid-command path is therefore not an early return: it becomes a
lookup of INVALID (-2), which normally fails with `0x494b1`. If a plugin were
active under -2, its `proc` would be invoked.

## proc 0x957f0

1. `if (!is_init_succ_) return 0;`
2. `static unsigned int s_last_proc_thread_id = 0;`
   `tid = syscall(0xe0)` (i386 `SYS_gettid`).
   Zero -> store `tid`. Nonzero and different -> second `syscall(0xe0)` and
   `debug_errorex("In CTssSdkFramework::proc, multi-thread call proc,|%u|%u|%u",
   getpid(), s_last_proc_thread_id, retry_tid)`. This warning does not abort.
3. `now = time(0)`; static `s_last_count_timestamp` is one-shot initialized
   from `now` (guard variable at `0x002a06c0`); statics:
   `proc_call_times` `0x002a06c8` (0),
   `max_recv_pkg_per_time` `0x00297940` (2000 = `0x7d0`).
4. `++proc_call_times`; `elapsed = (unsigned)(now - s_last_count_timestamp)`
   (ORIG uses `ja`, unsigned compare). If `elapsed > 299`:
   `max_recv_pkg_per_time = elapsed * 1000 / proc_call_times + 1`,
   log `"count max_recv_pkg_per_time=%u proc_call_times=%u"`,
   set `proc_call_times = 0`, `s_last_count_timestamp = now`.
5. Scoped timer lock: `check_monitor(now)`, `update_time(now)`.
6. `error_info_.last_error_info()->error_code = 0`.
7. Receive loop `for (proc_pkg_count = 0;
   proc_pkg_count < max_recv_pkg_per_time; ++proc_pkg_count)`:
   - `recv_buf = NULL; recv_buf_size = 0;`
   - `rc = comm_svc_->recv_buf_from_backend(&recv_buf, &recv_buf_size)`
   - `rc == 0x49449` (300105, channel plugin lookup failed) or
     `rc == 0x4944c` (300108, channel timeout/no data): break silently.
     ORIG has already set/logged the underlying channel-plugin error.
   - other nonzero: message
     `"In CTssSdkFramework::proc, sdk comm svc recv buf from backend failed. "`
     + `comm_svc_->get_last_error()`, log, code `0x494b2` (300210), break.
   - `rc = proc_buf_from_backend_(recv_buf, recv_buf_size)`;
     nonzero -> `debug_errorex("%s", get_last_error_msg())` and continue
     (the error code/message set by `proc_buf` is retained).
8. Walk every id in `not_need_channel_plugin_id_table_`:
   - `plugin = plugin_mgr_->get_active_plugin(id)`
   - null -> message `"In CTssSdkFramework::proc, plugin mgr get active plugin "`
     + id + `" failed. "` + manager error, log, code `0x494b1`, continue.
   - `rc = plugin->proc(NULL)` via vtable +0x10; nonzero -> message
     `"In CTssSdkFramework::proc, Plugin "` + id + `" proc failed. "` +
     plugin error, log, code `0x494ac`; continue.
9. Return `error_info_.last_error_info()->error_code` (the last error set,
   or 0 if no error was recorded).

Because the error code is cleared only once before the receive loop, a
successful later packet does not clear an earlier `proc_buf` failure.

## Compile result

Built with c5root GCC 4.1.2 and the target flags:

```sh
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
  recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_framework_dispatch_agent.cpp \
  -o /tmp/tss_sdk_framework_dispatch_agent.o
```

Result: **exit 0, no diagnostics**.

Emitted ORIG-mangled method symbols:

```text
tss_sdk_impl::TssSdkFramework::notify(tss_sdk_if::TssSdkPluginNotifyData)
tss_sdk_impl::TssSdkFramework::init_plugins(tss_sdk_if::TssSdkPluginInitData const*, unsigned int)
tss_sdk_impl::TssSdkFramework::proc_buf_from_backend_(char const*, unsigned int)  # size_t == unsigned int here
tss_sdk_impl::TssSdkFramework::proc()
```

Object sizes: notify `0x977`, init_plugins `0xd9d`, proc_buf `0x795`,
proc `0xb15`. They differ from ORIG because c5root is GCC 4.1.2 while DWARF
reports GCC 4.1.0; no byte-identity is claimed. The local predicate symbols
emit at exactly `0x11` bytes. `__PRETTY_FUNCTION__` is the ORIG 86-byte
string. The object has unresolved references to the framework static data
members, which are intentionally left to the framework integration TU.

## Residual / intentional differences

- `init_plugins`' vector copy/channel swap is semantically inert for
  activation: ORIG activates from the original caller table. This quirk is
  preserved rather than "fixed".
- `CThreadErrorInfo::set_last_error_msg` is called with the built message as
  the format string, exactly as ORIG does; a `%` embedded in a plugin/manager
  error string would be interpreted by the varargs formatter.
- `proc_buf_from_backend_` has no null/size validation, matching ORIG.
- No packet types beyond `Comm_App_Frame::frame_command_` and the observed
  `u16` at `recv_buf+0x3a` are modeled in this unit; `SendToSDK` itself is an
  external packet definition.

## External boundaries / not reconstructed here

- `TssSdkFramework::init`, `fini`, `get_last_error`, `init_config`,
  `init_log`, `get_not_need_channel_plugin_id_table_`, `get_plugin_id` are
  external to this assignment.
- Static data members `is_init_succ_`, `thread_mutex_`, `error_info_`,
  `plugin_mgr_`, `comm_svc_`, `not_need_channel_plugin_id_table_`,
  `timer_handler_` are declared but not defined here.
- `TssSdkPluginMgr`, `TssSdkCommSvc`, `TssSdkTimerHandler`, `ITssSdkPlugin`
  are partial externs with the layouts/vtable slots above; their bodies live
  in their own reconstructed units or the original SDK.
- libc/libstdc++: `time`, `syscall(0xe0)`, `getpid`, `ntohs`, strings,
  stringstreams, vector, `find_if`, `swap`, allocator/exception helpers.
- `tss_dp_comm::init_dp_config()`, `Singleton`/framework initialization and
  all plugin implementations remain outside this unit.
