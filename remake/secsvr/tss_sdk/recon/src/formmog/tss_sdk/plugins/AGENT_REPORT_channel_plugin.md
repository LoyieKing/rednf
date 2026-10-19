# TssSdkChannelPlugin reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Translation unit: `src/formmog/tss_sdk/plugins/channel/channel_plugin.cpp`
Reconstruction: `recon/src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp`

Verdict: **semantic reconstruction, not byte-identical**. Class layout, member
offsets, packet layouts, error codes, stat ids, message strings, call targets
and branch/return semantics match the shipped code. Instruction sizes of the
large methods differ because the available compiler is GCC 4.1.2 while the
target CU reports GCC 4.1.0, and the reconstruction spells out source-level
statements that the original compiler shared/inlined differently.

## 1. Address map

| symbol | range | notes |
|---|---|---|
| `TssSdkChannelPlugin::send_pkg_to_game_channel` | `0x0009ca50-0x0009ca63` | 20 bytes, exact size match |
| `TssSdkChannelPlugin::send_pkg_to_socket_channel` | `0x0009cb50-0x0009cb93` | `0x44`, exact size match |
| `TssSdkChannelPlugin::send_pkg_to_ipc_channel` | `0x0009cba0-0x0009cbd5` | `0x36`, exact size match |
| `TssSdkChannelPlugin::TssSdkChannelPlugin()` C2 | `0x0009cc90-0x0009cd6e` | `0xdf` |
| `TssSdkChannelPlugin::TssSdkChannelPlugin()` C1 | `0x0009cd70-0x0009ce4e` | `0xdf`, identical body |
| `get_obj_TssSdkChannelPlugin()` | `0x0009ce50-0x0009cee5` | `0x96`, exact size match |
| global constructors keyed to `get_obj...` | `0x0009d450-0x0009d462` | forwards `(1,0xffff)` to static init |
| TU static initialization | `0x0009cef0-0x0009d44f` | stat tables + `std::ios_base::Init` |
| `TssSdkChannelPlugin::fini` | `0x0009d470-0x0009d7b0` | `0x341` |
| `TssSdkChannelPlugin::ipc_channel_recv` | `0x0009d7c0-0x0009d8d8` | `0x119`, exact size match |
| `TssSdkChannelPlugin::socket_channel_recv` | `0x0009d8e0-0x0009da30` | `0x151`, exact size match |
| `TssSdkChannelPlugin::notify` | `0x0009da40-0x0009e804` | `0xdc5` |
| `TssSdkChannelPlugin::init` | `0x0009e810-0x0009fbc5` | `0x13b6` |
| `TssSdkChannelPlugin::proc` | `0x0009fbd0-0xa0095d` | `0xd8e` |
| `TssSdkChannelPlugin::~TssSdkChannelPlugin()` D1 | `0x000a0960-0x000a09ff` | `0xa0` |
| `TssSdkChannelPlugin::~TssSdkChannelPlugin()` D0 | `0x000a0a00-0x000a0aa7` | `0xa8`, deletes `this` |
| vtable | `0x00292e00` | 8 entries, address point `+8` |
| typeinfo / name | `0x00292e20` / `0x0021ab80` | `TssSdkChannelPlugin` |
| function-local static guard | `0x002a3360` | 8 bytes |
| `get_obj...::s_obj_TssSdkChannelPlugin` | `0x002a3380` | `0x12884` bytes |
| `STR_TSSSDK_STAT` | `0x002a3020` | `0x33c` bytes, 9 records of `0x5c` |
| `__tcf_0` static-object destructor | `0x0009cbe0-0x0009cc83` | inline D1 body, no `operator delete` |
| `tss_sdk_impl::g_obj_TssSdkChannelPlugin` | `0x0029f644` | 4-byte dead global; no relocation/reference in the image |

Vtable relocations (`R_386_32`) are, in order:
`typeinfo 0x292e20`, `D1 0xa0960`, `D0 0xa0a00`, `init 0x9e810`,
`fini 0x9d470`, `proc 0x9fbd0`, `notify 0x9da40` (slot 0 is the usual
offset-to-top 0). Two extra destructor slot copies are emitted by the
reconstruction; there is no semantic difference.

## 2. Class layout

`tss_sdk_impl::TssSdkChannelPlugin`, `sizeof == 75908`, base
`ITssSdkPlugin` (`sizeof == 16`):

| off | field | type |
|---|---|---|
| `+0x00000` | base | `ITssSdkPlugin` |
| `+0x00010` | `svr_id_pool_mgr_` | `BusChannelIdMgr` (24) |
| `+0x00028` | `channel_id_` | `unsigned int` |
| `+0x0002c` | `m_is_use_socket_channel_` | `bool` |
| `+0x0002d` | `m_is_use_ipc_channel_` | `bool` |
| `+0x00030` | `m_ipc_channel_` | `CIpcChannel` (75856) |
| `+0x12880` | `recv_buf_` | `unsigned char*` |

`ITssSdkPlugin`: `vptr +0`, `callback_proc_ +4`, `CThreadErrorInfo error_info_
+8` (4 bytes), `sdk_plugin_id_ +12`. The plugin writes/reads the raw error code
at `this+0x08` through `error_info_.last_error_info()->error_code`; it does not
call a virtual or `get_last_error_code()`.

`CIpcChannel`, `sizeof == 75856`:

| off | field |
|---|---|
| `+0x00000` | `char recv_buf_[65535]` |
| `+0x10000` | `ZEN_Thread_Mutex thread_mutex_` (28) |
| `+0x1001c` | `ZEN_BusPipe_TwoWay bus_channel_` (10292) |

The recv deque is `bus_channel_+0x200c` (`CIpcChannel+0x12028`); the send deque
is `bus_channel_+0x2010` (`CIpcChannel+0x1202c`). The mutex is locked before
each deque operation and unlocked through its vtable (`+8` lock, `+0x10`
unlock in the shipped objects).

`BusChannelIdMgr`, `sizeof == 24`: `used_svr_id_ +0`, `is_init_ +4`,
`max_channel_id_ +8`, `channel_info_ +12`, `lock_ (ZEN_File_Lock*) +16`,
`is_use_pid_ +20`.

## 3. Channel wire layouts

`tss_sdk_plugin_channel.h` (all channel packages are byte-packed):

| DWARF | line | size | fields |
|---|---|---|---|
| `._488` | 19 | 12 | `unique_instance_id_ u32 +0`, `ipc_bus_size_ u32 +4`, `pid_channel_mode_ i32 +8` |
| `._489` | 26 | 4 | `ETssSdkPluginChannelTypeId`: IPC 1, socket 2, game 3 |
| `._490` | 35 | 4 | `type_id_` |
| `._491` | 41 | 8 | `const char *buf_ +0`, `u32 buf_size_ +4` |
| `._493` | 53 | 10 | `i32 business_id_ +0`, `const char *buf_ +4`, `i16 buf_size_ +8` |
| `._494` | 61 | 14 | `i32 busi_type_ +0`, `u32 uin_ +4`, `unsigned char *buf_ +8`, `i16 buf_size_ +12` |
| `._495` | 69 | 14 | union of `._491`, `._493`, `._494` |
| `._496` | 77 | 18 | `head_ +0`, `body_ +4` |

`notify()` receives `._496`. `send_pkg_to_*` receive the inner `._491`,
`._493`, `._494` types by const reference. Names used in the reconstruction
(`TssSdkPluginChannel*Pkg`) are the names confirmed by the demangled entry
points; the DWARF DIEs themselves are anonymous.

`STssChannelData` is `{ const char *buf; size_t buf_size; }` (8 bytes on
i386), written by both recv helpers.

## 4. Constructor / factory / destructor

`TssSdkChannelPlugin::TssSdkChannelPlugin()` `@0x9cc90` and `@0x9cd70`:

```
ITssSdkPlugin(ETSS_SDK_PLUGIN_ID_CHANNEL /*1*/)
vptr = &vtable + 8
BusChannelIdMgr::BusChannelIdMgr()            // +0x10
channel_id_ = 0                               // +0x28
m_is_use_socket_channel_ = false              // +0x2c
m_is_use_ipc_channel_ = false                 // +0x2d
CIpcChannel::CIpcChannel()                    // +0x30
recv_buf_ = NULL                              // +0x12880
ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkChannelPlugin ctor.")
```

`get_obj_TssSdkChannelPlugin()` `@0x9ce50` is the function-local-static
accessor. It uses the normal `__cxa_guard_acquire` /
`__cxa_guard_release` / `__cxa_guard_abort` sequence, constructs the object at
`0x2a3380` on first use, registers `__tcf_0` with `__cxa_atexit`, and returns
the object address on every call.

D1 `@0xa0960` / D0 `@0xa0a00` set the vptr, destroy members in reverse
declaration order:

1. `CIpcChannel`'s inline destructor: `ZEN_BusPipe_TwoWay` at
   `this+0x1004c`, then `ZEN_Thread_Mutex` at `this+0x10030`;
2. `BusChannelIdMgr` at `this+0x10`;
3. `ITssSdkPlugin` base;
4. D0 additionally `operator delete(this)`.

The destructor does **not** call `fini()` and does **not** delete `recv_buf_`.
A plugin destroyed without `fini()` leaks the 65536-byte socket buffer.

## 5. `init(void*)`

`init_data` is `TssSdkPluginChannelInitData*`. Exact flow (`0x9e810`):

1. `init_data == NULL` -> `"In CTssSdkChannelPlugin::init, init_data is
   NULL."`, code `0x49571`, return code.
2. `svr_id_pool_mgr_.init(200)` (`0xc8`). Nonzero -> `"In
   TssSdkChannelPlugin::init, get_svr_id failed. rc is " << rc << "."`, code
   `0x49572`, return code.
3. Channel id:
   - `pid_channel_mode_ != 0` (offset `+8`) -> `get_bus_channel_id()`;
   - `pid_channel_mode_ == 0` -> `get_bus_channel_id(unique_instance_id_)`.
   The returned `unsigned int` is stored unconditionally in `channel_id_`
   (`+0x28`). **Negative helper results are not checked**; they become large
   unsigned ids.
4. `Singleton<TssSdkCommSvc>::instance()->channel_id_ = channel_id`
   (singleton object at `0x29d820`, field `+0`).
5. If `g_config.comm_config_.use_ipc_ != 0` (`g_config+0`):
   `m_is_use_ipc_channel_ = true`; then
   `m_ipc_channel_.open(channel_id_, init_data->ipc_bus_size_)`.
   Nonzero -> `"Open IPC channel failed. rc is " << rc << "."`, code
   `0x49574`, return code.
6. If `g_config.comm_config_.use_socket_ != 0` (`g_config+4`):
   `m_is_use_socket_channel_ = true`;
   `recv_buf_ = new (std::nothrow) unsigned char[0x10000]`. NULL ->
   `"new unsigned char failed"`, code `0x49573`, return code.
   For each of `g_config.comm_config_.server_num_` (uint16 at `g_config+8`)
   entries in `server_addrs_info_` (stride `0x26`, first at `g_config+0xa`):
   - `SocketChannelInitData.business_id_ = entry.plugin_id_` (entry `+0`);
   - `.ip_ = entry.ip_` (entry `+4`, inline `char[32]`);
   - `.port_ = entry.port_` (entry `+0x24`, uint16);
   - `.handler_ = new (std::nothrow) UICSocketChannelTCPHandler()` (`0x58`
     bytes; NULL handler is passed through);
   - `SocketChannel::instance()->init_connection(init_data)`.
   Nonzero -> `"Open socket channel failed. rc is " << rc << "."`, code
   `0x49573`, return code.
   `SocketChannel::instance()` is called once per entry.
7. `unsigned int svr_id = channel_id_`;
   `SERVICES_ID self_info(0x2b, svr_id)` (packed 6 bytes, service type `0x2b`).
8. `ZEN_Timer_Queue::instance(new ZEN_Timer_Wheel(0x400, 0xf731400, 0x64,
   TRIGGER_MODE_SYSTEM_CLOCK, true))`. `0xf731400` is 259200000 (3 days in
   ms); mode 1 is `TRIGGER_MODE_SYSTEM_CLOCK`.
9. `Comm_Stat_Monitor::instance()->initialize(false, self_info, 9,
   STR_TSSSDK_STAT, true)`. Nonzero -> `"Init stat monitor failed. rc is " <<
   rc << "."`, code `0x497c9`, return code.
10. Zero the nine counters: for `statistic_id = 0x68fb1; statistic_id <
    0x68fba; ++statistic_id`, `set_by_statid(statistic_id, 0, 0, 0)`.
11. Destroy `self_info`, return `0`.

Error returns leave the partial state as-is: `m_is_use_ipc_channel_` /
`m_is_use_socket_channel_` stay true, `channel_id_` stays written, and
`recv_buf_` is not freed on the socket-connection failure path.

### `g_config`

Plain unmangled global at `0x0029fac0`, type `CONFIG` (0x5eb bytes). Only the
packed channel prefix is used:

```
+0x000 int32  use_ipc_
+0x004 int32  use_socket_
+0x008 uint16 server_num_
+0x00a ServerAddrInfo server_addrs_info_[32]
       ServerAddrInfo (packed, 38 bytes):
         +0x00 int32 plugin_id_
         +0x04 char  ip_[32]
         +0x24 uint16 port_
```

The entire `comm_config_` is packed (`10 + 32*38 = 1226 = 0x4ca`), which is
why `log_config_` starts at `g_config+0x4ca`.

## 6. `fini()`

`@0x9d470`:

```
error code = 0
if (m_is_use_socket_channel_) {
    rc = SocketChannel::instance()->fini()
    if (rc != 0) { "Close socket channel failed. rc is " << rc << "."; code = 0x49578; }
    if (recv_buf_) { delete[] recv_buf_; recv_buf_ = NULL; }
}
if (m_is_use_ipc_channel_) {
    rc = m_ipc_channel_.close()
    if (rc != 0) { "Close IPC channel failed. rc is " << rc << "."; code = 0x49579; }
}
channel_id_ = 0
m_is_use_socket_channel_ = false
m_is_use_ipc_channel_ = false
return error code
```

`fini()` is idempotent for the flags. Both error paths are dead in the
shipped binary: `SocketChannel::fini()` (`0xd7670`) and `CIpcChannel::close()`
(`0xa0ab0`) both return 0 unconditionally, so neither `0x49578` nor `0x49579`
can be written. A socket `fini()` failure would not skip the receive-buffer
delete or the IPC close; the last error code would win if both failed.

## 7. Receive paths

### `ipc_channel_recv(void*)` `@0x9d7c0`

- NULL -> `"In CTssSdkChannelPlugin::ipc_channel_recv, proc_data is NULL."`,
  code `0x49571`, error code returned.
- Else `return m_ipc_channel_.recv((void**)&channel_data->buf,
  (unsigned int*)&channel_data->buf_size);`.

`CIpcChannel::recv(void**, unsigned int*)` `@0xa1050`:

1. Either output pointer NULL -> `-1` without writes.
2. `*recv_buf = NULL; *recv_buf_size = 0xffff;`
3. Lock `thread_mutex_`.
4. If `bus_channel_.recv_deque(+0x200c).empty()` -> unlock, `-1`.
5. Else `pop_front((dequechunk_node*)this)`; `*recv_buf = this` (i.e.
   `recv_buf_` at offset 0); `*recv_buf_size = *(unsigned int*)this`
   (first word of the copied node); unlock, `0`.

So the IPC frame is a shared-memory deque node whose first four bytes are the
node length, limited to `0xffff`, followed by the payload. The plugin returns
a pointer to its own 65535-byte buffer and the low 16-bit node length.

### `socket_channel_recv(void*)` `@0x9d8e0`

- NULL -> `"In CTssSdkChannelPlugin::socket_channel_recv, proc_data is
  NULL."`, code `0x49571`, error code returned.
- `business_id = 0; recv_buf_len = 0;`
  `rc = SocketChannel::instance()->recv_data(&business_id, recv_buf_,
  &recv_buf_len);`
  `channel_data->buf = (const char*)recv_buf_;`
  `channel_data->buf_size = recv_buf_len;`
  `return (rc == 0x1433) ? -1 : 0;`

`SocketChannel::recv_data(int*, void*, unsigned int*)` `@0xd7bb0`:

- any argument NULL -> `debug_errorex("[socket channel]SocketChannel::recv_data
  invalid args")`, `0x1434`;
- lock the business-map RW mutex; for each business, `pop_recv_bus(buf)`;
- none -> `0x1433`;
- found -> `*buf_size = *(u32*)buf` (first word is the frame length),
  `*business_id = business key`, `0`.

Mapping `0x1433` to `-1` is the "channel empty" result; every other value,
including `0`, is mapped to `0` (success). The business id output is not used
by the plugin; the persistent `recv_buf_` remains the buffer.

## 8. Send paths

### `send_pkg_to_ipc_channel` `@0x9cba0`

```
return m_ipc_channel_.send(pkg.buf_, pkg.buf_size_);
```

`CIpcChannel::send(const void*, unsigned int)` `@0xa1130`:

- `send_buf_size > 0xffff` -> `debug_errorex("send buf to bus fail. buf too
  big. buf_size=%u", size)`, `-1`;
- lock mutex;
- `bus_channel_.send_deque(+0x2010).push_end((dequechunk_node const*)send_buf)`;
- push failure -> `debug_alertex("[zenlib] %u Pipe is full or data small?,Some
  data can't put to pipe. Please increase and check. nodesize=%u,
  freesize=%u", *(u32*)send_buf, freesize())`, `-1`;
- success `0`; unlock in both cases.

The `send_buf_size` argument is only a limit check: the deque copies the node
from the buffer, and the node's own first word is the actual length.

### `send_pkg_to_socket_channel` `@0x9cb50`

```
return SocketChannel::instance()->send_data(pkg.business_id_, pkg.buf_);
```

`pkg.buf_size_` is **ignored**. `SocketChannel::send_data(int, const void*)`
`@0xd7db0`:

- NULL buffer -> `debug_errorex("[socket channel]send_buf == NULL")`, `0x1434`;
- missing business -> `debug_errorex("[socket channel]can not find
  corresponding business")`, `0x1430`;
- `push_send_bus(buf)` failure -> `debug_errorex("[socket channel]push_send_buf
  failed")`, `0x1431`;
- success `0`.

The socket frame length is the first word of the Comm_App_Frame buffer; the
network thread later subtracts the 50-byte application-frame header.

### `send_pkg_to_game_channel` `@0x9ca50`

```
return callback_proc_((void*)&game_channel_pkg);
```

`callback_proc_` is the ITssSdkPlugin base field at `+0x04`; the call is
indirect through it and is not null-checked. The game package fields
(`busi_type_`, `uin_`, `buf_`, `buf_size_`) are only interpreted by the
registered callback.

## 9. `notify(void*)`

`@0x9da40`. `notify_data` is `TssSdkPluginChannelPkg*`.

```
NULL -> "In CTssSdkChannelPlugin::notify, notify_data is NULL.",
        code 0x49571, return code
switch (pkg->head_.type_id_):
  IPC (1):
    !m_is_use_ipc_channel_ -> "IPC channel is not used.", code 0x49577, return
    rc = send_pkg_to_ipc_channel(pkg->body_.ipc_channel_pkg_)
    rc == 0: increase 0x68fb3 by 1; return 0
    else: increase 0x68fb4 by 1; "Send to IPC channel failed." << "rc is "
          << rc << "."; code 0x4957e; return
  SOCKET (2):
    !m_is_use_socket_channel_ -> "Socket channel is not used.", code 0x49576
    rc = send_pkg_to_socket_channel(pkg->body_.socket_channel_pkg_)
    rc == 0: increase 0x68fb7 by 1; return 0
    else: increase 0x68fb8 by 1; "Send to socket channel failed." << "rc is "
          << rc << "."; code 0x4957f; return
  GAME (3):
    rc = send_pkg_to_game_channel(pkg->body_.game_channel_pkg_)
    rc == 0: return 0                  (no counter)
    else: "Send to game channel failed." << "rc is " << rc << ".";
          code 0x49582; return
  default:
    "Error notify pkg.", code 0x49583, return
```

The failure strings deliberately preserve the missing space after the period:
`"Send to ... failed.rc is N."`. Success/fail counters are increased before
the error text is built. The stat app-id argument is
`DP_CONFIG::glb_dp_config_head_->game_id` (`head+8`) when the head is non-NULL,
otherwise 0; classify id is 0 and the increment value is 1.

## 10. `proc(void*)`

`@0x9fbd0`. Socket channel has priority when both flags are set.

```
if (m_is_use_socket_channel_) {
  NULL -> "In CTssSdkChannelPlugin::proc, proc_data is NULL.",
          code 0x49571
  rc = socket_channel_recv(proc_data)
  rc == 0  -> increase 0x68fb5 by 1; return 0
  rc == -1 -> "socket channel is empty.", code 0x4957a; return code
  else     -> increase 0x68fb6 by 1;
              "Recv from IPC channel failed. " << "rc is " << rc (no period);
              code 0x4957d; return code
}
if (m_is_use_ipc_channel_) {
  NULL -> same null message, code 0x49571
  rc = ipc_channel_recv(proc_data)
  rc == 0  -> increase 0x68fb1 by 1; return 0
  rc == -1 -> "IPC channel is empty.", code 0x4957a; return code
  else     -> increase 0x68fb2 by 1;
              "Recv from IPC channel failed. " << "rc is " << rc;
              code 0x4957c; return code
}
return 0
```

The socket failure path reuses the IPC error literal; this is shipped
behaviour. Since both receive wrappers only return `0` or `-1`, the
non-`0`/non-`-1` failure branches are unreachable in practice but are
preserved. `0x4957a` (`ERR_TSSSDK_CHANNEL_EMPTY`) is the timeout/empty signal
the framework maps to its own timeout code.

## 11. Stat table and counters

`STR_TSSSDK_STAT` is `ZEN_STATUS_ITEM_WITHNAME[9]` at `0x002a3020`, size
`0x33c`. Static initialization constructs five named entries, then four
default `ZEN_STATUS_ITEM_WITHNAME()` entries:

| slot | stat id | name |
|---|---|---|
| 0 | `0x68fb1` | `TSSSDK_IPC_CHANNLE_RECV_SUCC_NUM` |
| 1 | `0x68fb2` | `TSSSDK_IPC_CHANNLE_RECV_FAIL_NUM` |
| 2 | `0x68fb3` | `TSSSDK_IPC_CHANNLE_SEND_SUCC_NUM` |
| 3 | `0x68fb4` | `TSSSDK_IPC_CHANNLE_SEND_FAIL_NUM` |
| 4 | `0x68fb9` | `TSSSDK_ANTIBOT_LOST_PKG_NUM` |
| 5-8 | default (id 0) | empty name |

All use `STATICS_PER_FIVE_MINTUES` (=1). The four default entries are
placeholders; `ZEN_Server_Status::find_insert_idx` auto-registers an unknown
stat id on first use, so `0x68fb5..0x68fb8` are created on their first
`increase_by_statid` call. The reconstruction keeps the same five explicit
initializers plus four default-constructed entries; the emitted static-init
sequence matches the shipped offsets `+0x000`, `+0x05c`, ..., `+0x2e0` and
`operator new`-free construction exactly.

Counter mapping used by this TU:

| id | event |
|---|---|
| `0x68fb1` | IPC recv success |
| `0x68fb2` | IPC recv failure / non-timeout error |
| `0x68fb3` | IPC send success |
| `0x68fb4` | IPC send failure |
| `0x68fb5` | socket recv success |
| `0x68fb6` | socket recv failure |
| `0x68fb7` | socket send success |
| `0x68fb8` | socket send failure |
| `0x68fb9` | antibot lost package (not touched here) |

`init` zeroes `0x68fb1..0x68fb9` with `ZEN_Server_Status::set_by_statid(id, 0,
0, 0)` after a successful `Comm_Stat_Monitor::initialize`. Every mutation is
`increase_by_statid(id, game_id, 0, 1)`.

## 12. Error codes

No symbolic names exist in the shipped DWARF.

| code | site |
|---|---|
| `0x49571` | NULL argument: init, proc (both branches), notify, ipc/socket recv |
| `0x49572` | `BusChannelIdMgr::init` failure |
| `0x49573` | socket connection setup failure or 64 KiB receive-buffer allocation failure |
| `0x49574` | IPC channel open failure |
| `0x49576` | socket channel not used (notify) |
| `0x49577` | IPC channel not used (notify) |
| `0x49578` | `SocketChannel::fini()` failure (unreachable; fini always returns 0) |
| `0x49579` | `CIpcChannel::close()` failure (unreachable; close always returns 0) |
| `0x4957a` | channel empty: socket/IPC recv returned -1 |
| `0x4957c` | IPC recv non-empty error |
| `0x4957d` | socket recv non-empty error |
| `0x4957e` | IPC send failure |
| `0x4957f` | socket send failure |
| `0x49582` | game callback send failure |
| `0x49583` | invalid notify package type |
| `0x497c9` | `Comm_Stat_Monitor::initialize` failure |

`0x49575`, `0x4957b`, `0x49580`, `0x49581` are not used by this TU.

## 13. Helper semantics / OS boundaries

### `BusChannelIdMgr` (`@0x18c930` ctor, `@0x18d040` init, `@0x18cc70`/`@0x18cd90` getters, `@0x18d1e0` dtor)

- `init(200)` allocates an 8-byte `ZEN_File_Lock`, opens the configured lock
  path with mode `0x42` (O_RDWR|O_CREAT) and permission `0x1b0` (0660), then
  allocates `max_channel_id*4` bytes and reads the channel-info list. Returns
  `0` on success and `-2` (`0xfffffffe`) when the lock/file open fails.
- `get_bus_channel_id()` returns `-4` when `is_init_` is false, `-2` on open
  failure, `-3` on list-read failure, otherwise a positive 1-based id; it sets
  `is_use_pid_` true and allocates by pid. `get_bus_channel_id(uint)` is the
  same with `-1` for the not-initialized case.
- The plugin assigns the `unsigned int` return to `channel_id_` without a
  range check.
- Destructor: if `is_use_pid_`, calls `free_svr_id()` (pid lookup, clears the
  slot, rewrites the file); then deletes the lock and `delete[]`s the channel
  info array.

### `SocketChannel` (`@0xd7540` instance, `@0xd7670` fini, `@0xd7e50` init_connection, `@0xd7bb0` recv_data, `@0xd7db0` send_data)

- `instance()` lazily allocates a 0x6c-byte object with `std::nothrow` and
  stores it in the `SocketChannel::instance_` pointer; returns NULL if that
  allocation fails, without a null check at the call sites.
- `init_connection` requires `business_id >= 0`, non-NULL handler, non-NULL ip
  and nonzero port; otherwise logs and returns `0x1434`, deleting the handler
  through its deleting destructor. Thread-start failures return `-1` and also
  delete the handler; success returns 0.
- `fini()` stops/joins the thread, logs, clears the business map and always
  returns 0.
- send/recv return codes above are mapped by the plugin as described.

### `CIpcChannel` (`@0xa0f90` ctor, `@0xa0ed0` open, `@0xa1050` recv, `@0xa1130` send, `@0xa0ab0` close)

- `open(channel_id, ipc_bus_size)` formats `"sec/tss_sdk_bus_%d"` with
  `snprintf(buf, 0xfff, ...)` into a 4096-byte stack buffer and calls
  `bus_channel_.initialize(path, ipc_bus_size, ipc_bus_size, 0xffff, true,
  false, 2)`. Failure logs `"bus init fail. ret=%d"` and returns `-1`;
  success returns 0.
- `recv`/`send` are the length-prefixed shared-memory deque operations in
  section 7/8.
- `close()` is a bare `return 0;` and does not unmap, destroy the bus pipe or
  mark the channel closed.

### Other boundaries

| symbol | address |
|---|---|
| `CIpcChannel::CIpcChannel()` | `0xa0f90` / `0xa0ff0` |
| `ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)` | external zenlib |
| `ZEN_BusPipe_TwoWay` ctor/dtor | external zenlib |
| `UICSocketChannelTCPHandler::UICSocketChannelTCPHandler()` | `0xa17a0` / `0xa17e0` |
| `ZEN_Timer_Wheel` ctor | `0x1b0970` / `0x1b06e0` |
| `ZEN_Timer_Queue::instance(ZEN_Timer_Queue*)` | `0x1ae7b0` |
| `Comm_Stat_Monitor::instance()` | `0xda730` |
| `Comm_Stat_Monitor::initialize(...)` | `0xda6b0` |
| `ZEN_Server_Status::set_by_statid(...)` | `0x1a7ee0` |
| `ZEN_Server_Status::increase_by_statid(...)` | `0x1a7fc0` |
| `ZEN_STATUS_ITEM_WITHNAME` ctors | `0x1a6f60` / `0x1a6d70` |
| `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()` | `0x8af00` (weak) |
| `Singleton<...>::instance()::obj` | `0x29d820` |
| `CThreadErrorInfo::last_error_info()` | `0x1ad390` |
| `CThreadErrorInfo::set_last_error_msg(char const*, ...)` | `0x1ad470` |
| `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)` | `0x1b3dd0` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_head_` | `0x2bb4c4` (GOT `0x294358`); `game_id +8` |
| `g_config` | `0x29fac0`, size `0x5eb` |

The shipped `init` inlines the `Singleton<TssSdkCommSvc>::instance()` guard
and constructor; the reconstruction calls the weak `instance()` symbol, which
performs the same one-time construction. The shipped `fini`/`init` read and
write the per-thread error block at `this+8` directly; the reconstruction uses
the same `error_info_.last_error_info()->error_code` access. All other
reconstructed calls target the exact external symbols listed above.

## 14. String addresses

| address | literal |
|---|---|
| `0x21a850` | `Close IPC channel failed. rc is ` |
| `0x21a82c` | `Close socket channel failed. rc is ` |
| `0x21a874` | `In CTssSdkChannelPlugin::ipc_channel_recv, proc_data is NULL.` |
| `0x21a8b4` | `In CTssSdkChannelPlugin::socket_channel_recv, proc_data is NULL.` |
| `0x21a8f8` | `In CTssSdkChannelPlugin::notify, notify_data is NULL.` |
| `0x21a930` | `Send to socket channel failed.` |
| `0x21a950` | `In CTssSdkChannelPlugin::init, init_data is NULL.` |
| `0x21a984` | `In TssSdkChannelPlugin::init, get_svr_id failed. rc is ` |
| `0x21a9bc` | `Open IPC channel failed. rc is ` |
| `0x21a9dc` | `Open socket channel failed. rc is ` |
| `0x21aa00` | `Init stat monitor failed. rc is ` |
| `0x21aa24` | `In CTssSdkChannelPlugin::proc, proc_data is NULL.` |
| `0x21aa58` | `Recv from IPC channel failed. ` |
| `0x21aa77` | `In CTssSdkChannelPlugin ctor.` |
| `0x21aab1` | `IPC channel is not used.` |
| `0x21aaca` | `Send to IPC channel failed.` |
| `0x21aae6` | `Socket channel is not used.` |
| `0x21ab02` | `Send to game channel failed.` |
| `0x21ab1f` | `Error notify pkg.` |
| `0x21ab31` | `new unsigned char failed` |
| `0x21ab4a` | `socket channel is empty.` |
| `0x21ab63` | `IPC channel is empty.` |
| `0x219af9` | `rc is ` (shared suffix) |
| `0x221e1e` | `.` (shared suffix) |

## 15. Compile result

Compiler: c5root GCC 4.1.2, matching the target's GCC 4.1.x generation.
Command (from the workbench root):

```sh
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -Wno-invalid-offsetof -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include -I recon \
  -c recon/src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp \
  -o /tmp/channel_plugin_agent.o
```

Result: **exit 0, no diagnostics** with `-Wno-invalid-offsetof` (the only
warnings without it are GCC's non-POD `__builtin_offsetof` notes). The object
defines C1/C2, D0/D1/D2, `init`, `fini`, `proc`, `notify`, the two receive
helpers, the three send helpers and `get_obj_TssSdkChannelPlugin`, plus the
`STR_TSSSDK_STAT` table and the guarded static object. Compile-time checks in
the file enforce every class/member/packet offset and size listed above. The
undefined-reference list contains only the real external helpers above and
libstdc++/libgcc runtime symbols.

Notable exact-size matches against the original: `send_pkg_to_*` (0x14,
0x44, 0x36), both constructors (0xdf), `get_obj` (0x96),
`ipc_channel_recv` (0x119) and `socket_channel_recv` (0x151).

## 16. Unresolved / out-of-scope observations

- The numeric error codes have no enum/name in DWARF; the reconstruction
  keeps the raw values.
- `0x49578` / `0x49579` (close failures) are unreachable because the shipped
  `SocketChannel::fini()` and `CIpcChannel::close()` both `return 0`.
- `socket_channel_recv` maps every `SocketChannel::recv_data` result other
  than `0x1433` to 0, including `0x1434` (invalid arguments). The plugin
  always passes valid arguments, so this is only a quirk of the wrapper.
- The four unnamed stat slots (`0x68fb5..0x68fb8`) are not explicitly present
  in `STR_TSSSDK_STAT`; the shipped ZEN status monitor auto-registers unknown
  ids on first use. No name strings for the socket counters exist in the
  image.
- The same TU static-initialization also constructs an unrelated 16-entry
  `ZEN_STATUS_ITEM_WITHNAME` table at `0x2a2a40` (`COMM_STAT_*` ids
  `0x238d..0x239c`). It is part of the original TU's globals but has no
  channel-plugin access; it is intentionally not reconstructed here.
- `tss_sdk_impl::g_obj_TssSdkChannelPlugin` (`0x29f644`, 4 bytes) has no
  relocation or code reference in the image and is omitted.
- The DWARF for `init` lists a `timeval tv` local and a `statistic_id` local
  at lines 92/185; `tv` produces no instructions in the shipped body and is
  dead. `statistic_id` is the `0x68fb1..0x68fb9` loop.
- `g_config` is the generated 0x5eb-byte `CONFIG`; this TU models only the
  packed `comm_config_` prefix (`use_ipc_ +0`, `use_socket_ +4`,
  `server_num_ +8`, 38-byte address entries at `+0xa`). The remaining
  `log_config_`/`usercontrol_config_`/`uic_config_` fields are not accessed.
- The reconstruction makes the plugin's data members public so the
  compile-time offset assertions can name them; they are private in the
  shipped class. Visibility is not observable in the ABI or behaviour.
- `init`, `fini`, `proc` and `notify` do not have the original instruction
  sizes under GCC 4.1.2. The semantic comparison covered every immediate,
  field offset, call target, string, stat id and return path; no per-function
  byte equality is claimed.
