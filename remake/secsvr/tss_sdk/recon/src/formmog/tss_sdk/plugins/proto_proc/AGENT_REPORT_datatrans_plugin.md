# TssSdkDataTransPlugin reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
Original TU: `src/formmog/tss_sdk/plugins/data_trans/datatrans_plugin.cpp`.
Emitted source: `datatrans_plugin_agent.cpp`.

All findings are from the shipped symbol table, DWARF, `.rodata`, and
disassembly.  Addresses are link-time virtual addresses.

## Address ranges

| Method | Symbol address | Body range |
|---|---|---|
| `TssSdkDataTransPlugin::TssSdkDataTransPlugin()` C1 | `0x000a18f0` | `0xa18f0-0xa1950` |
| `TssSdkDataTransPlugin::TssSdkDataTransPlugin()` C2 | `0x000a1a10` | `0xa1a10-0xa1a70` |
| `get_obj_TssSdkDataTransPlugin()` | `0x000a1970` | `0xa1970-0xa1a06` |
| `TssSdkDataTransPlugin::fini()` | `0x000a1820` | `0xa1820-0xa1826` |
| `TssSdkDataTransPlugin::proc(void*)` | `0x000a1830` | `0xa1830-0xa1836` |
| `TssSdkDataTransPlugin::forward_data_use_ipc(...)` | `0x000a1840` | `0xa1840-0xa1846` |
| `TssSdkDataTransPlugin::init(void*)` | `0x000a20d0` | `0xa20d0-0xa216f` |
| `TssSdkDataTransPlugin::notify(void*)` | `0x000a1ad0` | `0xa1ad0-0xa1cbd` |
| `TssSdkDataTransPlugin::~TssSdkDataTransPlugin()` D0 | `0x000a2170` | `0xa2170-0xa21ae` |
| `TssSdkDataTransPlugin::~TssSdkDataTransPlugin()` D1 | `0x000a21b0` | `0xa21b0-0xa21dd` |

The destructor is inline/implicit in ORIG (weak D0/D1, no D2); the
reconstruction defines it in-class and emits the same weak pair.

## Class layout (DWARF, `sizeof == 24`)

```
+0x00 vptr
+0x04 tss_sdk_if::TssSdkPluginCallbackProc callback_proc_
+0x08 CThreadErrorInfo error_info_
+0x0c tss_sdk_if::ETssSdkPluginId sdk_plugin_id_  (DATA_TRANS = 7)
+0x10 TssSdkCommSvc *m_sdk_comm_svc
+0x14 unsigned int m_channel_id
```

Ctor installs id 7, zeroes +0x10/+0x14, installs the derived vtable, and logs
`"In CTssSdkDataTransPlugin ctor."`.  `get_obj_TssSdkDataTransPlugin()`
returns the guarded function-local static.  Vtable slot order is D1, D0,
`init`, `fini`, `proc`, `notify`.

## init / fini / proc

`init(void*)` ignores its argument and performs:

```
m_sdk_comm_svc = Singleton<TssSdkCommSvc>::instance();
m_channel_id   = m_sdk_comm_svc->channel_id_;   // TssSdkCommSvc +0x00
return 0;
```

There is no null check and no error path.

`fini()` returns 0.  `proc(void*)` ignores its argument and returns 0; both
are real two-instruction no-ops in the shipped object.

## `forward_data_use_ipc(const TssSdkPluginDataTransForwardDataUseIPCPkg&)`

Exact shipped symbol:
`_ZN12tss_sdk_impl21TssSdkDataTransPlugin20forward_data_use_ipcERK41TssSdkPluginDataTransForwardDataUseIPCPkg`.

The body is only `xor eax,eax; ret`, i.e. it returns 0 and does not inspect
the package.  This is not a reconstruction stub: the shipped build contains
no forwarding call here.  The reconstruction is an empty `return 0;`.

## notify(void*)

Argument is `TssSdkPluginDataTransPkg*`.

1. `data == NULL`:
   message `"In CTssSdkDataTransPlugin::notify, proc_data is NULL."`,
   code `0x495d5` (300501), return stored code.
2. `pkg->head_.msg_id_ == 1`
   (`TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_IPC`):
   `forward_data_use_ipc(pkg->body_.ipc_pkg_)`; if it returns 0, return 0.
3. Any other message id (`2` is `..._USE_TCP` but is not dispatched), or a
   non-zero forward result:
   message `"In CTssSdkDataTransPlugin::notify, data trans fail. "`,
   code `0x495d5` (300501), return stored code.

No socket, IPC, or TCP helper is called by this shipped plugin.  The only
transport-looking work is delegated to `forward_data_use_ipc`, which is a
no-op.

## Plugin package (DWARF anonymous structs)

```
enum ETssSdkPluginDataTransMsgId (size 4):
  TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_IPC = 1
  TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_TCP = 2

TssSdkPluginDataTransForwardDataUseIPCPkg (24):
  game_id u32 +0, room_id u32 +4, uin u32 +8,
  dest_svr_id u32 +12, data_len u32 +16,
  const unsigned char* data_ +20

TssSdkPluginDataTransForwardDataUseTCPPkg (20):
  game_id u32 +0, room_id u32 +4, uin u32 +8,
  data_len u32 +12, unsigned char* data_ +16

TssSdkPluginDataTransBody union (24):
  ipc_pkg_ (24), tcp_pkg_ (20)

TssSdkPluginDataTransHead (4):
  msg_id_ (enum, 4)

TssSdkPluginDataTransPkg (28):
  head_ +0, body_ +4
```

`notify` checks the 32-bit `msg_id_` and passes the union at `+4`; the method
parameter type is the global-namespace IPC type above, matching the shipped
mangled symbol exactly.

## Error codes / messages

| Code | Decimal | Where |
|---|---|---|
| `0x495d5` | 300501 | notify NULL payload, unsupported msg id, non-zero forward result |

Exact literals:

```
In CTssSdkDataTransPlugin ctor.
In CTssSdkDataTransPlugin::notify, proc_data is NULL.
In CTssSdkDataTransPlugin::notify, data trans fail. 
```

The failure literal ends with a trailing space.

## External boundaries left undefined

- `ITssSdkPlugin::{ctor,dtor}`
- `CThreadErrorInfo::{last_error_info,set_last_error_msg}`
- `ZEN_Trace_LogMsg::debug_debugEx`
- `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()`
- libstdc++ string/guard/atexit helpers

## Compile result

Compiled with c5root GCC 4.1.2 and the exact target flags:

```
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -I recon -c datatrans_plugin_agent.cpp
```

No errors or warnings.  `nm` confirms the exact shipped names for C1/C2,
D0/D1, `init`, `fini`, `proc`, `notify`, `forward_data_use_ipc`, `get_obj`,
vtable, typeinfo and guard/object.

## Unresolved / deliberately preserved behaviour

- `forward_data_use_ipc` is a no-op in this ship; any future transport logic
  is outside `libtss_sdk.so.1.0.394`.
- Message id 2 (TCP) is defined in DWARF but not handled by `notify`; it
  takes the generic failure path.
- The anonymous package type names and enum labels are reconstruction names;
  their sizes, offsets, and numeric values are DWARF-exact.
