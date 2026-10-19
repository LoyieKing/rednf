# TssSdkPluginMgr reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
Original TU: `src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.cpp`
(DWARF producer path `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/`).
Emitted source: `tss_sdk_plugin_mgr_agent.cpp`.

All conclusions below come from the shipped symbol table, DWARF layouts/line
tables, `.rodata` strings and disassembly. No plugin implementation was
invented. Addresses are link-time virtual addresses in the shipped ELF.

## Address ranges

| Method | Symbol(s) | Range (body, padding omitted) |
|---|---|---|
| `TssSdkPluginMgr::TssSdkPluginMgr()` | C1 `0x97ca0`, C2 `0x98290` | `0x97ca0-0x97e77`, `0x98290-0x98467` |
| `TssSdkPluginMgr::~TssSdkPluginMgr()` | D2 `0x98470`, D1 `0x985c0` | `0x98470-0x985b4`, `0x985c0-0x98704` |
| `get_registered_plugin_count()` | `0x97990` | `0x97990-0x97999` |
| `get_active_plugin_id_table()` | `0x979a0` | `0x979a0-0x979aa` |
| `is_id_invalid(...)` | `0x979b0` | `0x979b0-0x979c3` |
| `get_last_error_code()` | `0x97a40` | `0x97a40-0x97a67` |
| `is_id_repeated(...)` | `0x97a70` | `0x97a70-0x97a88` |
| `is_id_activated(...)` | `0x97a90` | `0x97a90-0x97aef` |
| `register_plugin(...)` | `0x97af0` | `0x97af0-0x97bcf` |
| `get_last_error()` | `0x97bd0` | `0x97bd0-0x97c9f` |
| `get_active_plugin(...)` | `0x98740` | `0x98740-0x99204` |
| `deactivate_plugin(...)` | `0x99210` | `0x99210-0x99824` |
| `activate_plugin(...)` | `0x99830` | `0x99830-0x99f59` |
| `init()` | `0x99f60` | `0x99f60-0x9a794` |

DWARF source lines: ctor 20, dtor 34, `register_plugin` 41, `init` 81,
`activate_plugin` 128, `deactivate_plugin` 173, `get_active_plugin` 200,
`get_last_error` 281.

Return types from DWARF: `get_registered_plugin_count()` -> `unsigned int`;
`get_active_plugin_id_table()` -> pointer to
`std::vector<tss_sdk_if::ETssSdkPluginId>` (not a reference; callers pass the
returned address directly as the source reference argument); the three
`is_id_*` -> `bool`; `register_plugin` -> `void`; `get_last_error_code()` and
`deactivate_plugin()` -> `int`; `init()` -> `int`; `activate_plugin()` and
`get_active_plugin()` -> `ITssSdkPlugin*`; `get_last_error()` -> `std::string`.

## Class layout (DWARF, `sizeof == 56`, no vtable)

```
+0x00  unsigned int registered_plugin_count_
+0x04  std::vector<tss_sdk_if::ETssSdkPluginId> invalid_plugin_id_table_
+0x10  std::vector<tss_sdk_if::ETssSdkPluginId> repeated_plugin_id_table_
+0x1c  std::vector<tss_sdk_if::ETssSdkPluginId> active_plugin_id_table_
+0x28  std::vector<std::pair<ITssSdkPlugin*, bool> > total_plugin_table_
+0x34  CThreadErrorInfo error_info_          (4-byte pthread_key_t)
```

`total_plugin_table_` is indexed by the raw enum value and is resized in the
constructor to `ETSS_SDK_PLUGIN_ID_END` (`8`) entries, each
`{NULL, false}`. `is_id_invalid` accepts only ids 1..7
(`(unsigned)(id - 1) > 6`), so slot 0 and slot 8 are allocated but unused.
`is_id_repeated` reads `total_plugin_table_[id].first` with no bounds check;
all callers validate first.

## Partial `ITssSdkPlugin` interface (exact shipped vtable)

`ITssSdkPlugin` is 16 bytes:

```
+0x00  vptr
+0x04  TssSdkPluginCallbackProc callback_proc_   (int (*)(void*))
+0x08  CThreadErrorInfo error_info_
+0x0c  tss_sdk_if::ETssSdkPluginId sdk_plugin_id_
```

Vtable `0x00292d80`, address point `0x00292d88`; typeinfo `0x00292da0`:

```
[0] 0x00097250  ~ITssSdkPlugin() D1
[1] 0x00097200  ~ITssSdkPlugin() D0 (deleting)
[2] pure        virtual int init(void*)
[3] pure        virtual int fini()
[4] pure        virtual int proc(void*)
[5] pure        virtual int notify(void*)
```

The four pure slots are confirmed by `__cxa_pure_virtual` relocations at
`0x292d90..0x292d9c`; derived vtables (e.g. `TssSdkCaptchaPlugin` at
`0x292dc0`) fill those four slots with `init(void*)`, `fini()`, `proc(void*)`,
`notify(void*)`.

Non-virtual interface methods are direct symbols, not vtable slots:
`register_callback` `0x97110` (stores at `this+0x04`), `get_id` `0x97120`
(returns `this+0x0c`), `get_last_error_code` `0x971d0` (uses `this+0x08`),
`get_last_error` `0x972d0`.

**The manager performs no virtual dispatch.** `register_plugin` calls the
non-virtual `ITssSdkPlugin::get_id()` directly (call at `0x97b13` to PLT
symbol `_ZN12tss_sdk_impl13ITssSdkPlugin6get_idEv`). `init`, `activate_plugin`,
`deactivate_plugin` and `get_active_plugin` call only manager methods and
`CThreadErrorInfo`.

## Registration / duplicate / activation rules

`register_plugin(ITssSdkPlugin* plugin)`:

1. `id = plugin->get_id()` (no null check).
2. If `is_id_invalid(id)`: `invalid_plugin_id_table_.push_back(id)` and return.
3. Else if `is_id_repeated(id)` (`total_plugin_table_[id].first != NULL`):
   `repeated_plugin_id_table_.push_back(id)` and return. Existing slot is not
   overwritten and the counter is not incremented.
4. Otherwise: `++registered_plugin_count_`,
   `total_plugin_table_[id].first = plugin`,
   `total_plugin_table_[id].second = false`, then log.

So a plugin is registered inactive. Only a unique, in-range id increments the
counter.

`is_id_activated(id)` is `std::find(active_plugin_id_table_.begin(), end, id)
!= end`.

`activate_plugin(id)`:

- invalid id: error code `0x494b5`, message, return `NULL`;
- already active and `id != ETSS_SDK_PLUGIN_ID_CHANNEL (1)`:
  error code `0x494b7`, message, return `NULL`;
- already active and `id == 1`: calls `deactivate_plugin(1)` first (removes
  every `1` from `active_plugin_id_table_` and clears slot flag), then falls
  through to normal activation;
- normal path: slot flag `= true`, push id into `active_plugin_id_table_`,
  log, return `total_plugin_table_[id].first`.

`deactivate_plugin(id)`:

- invalid id: error code `0x494b5`, message, return the stored error code;
- normal path: `active_plugin_id_table_.erase(std::remove(begin, end, id),
  end)` (removes all occurrences), slot flag `= false`, log, return `0`.
- It never deletes the plugin object and does not clear `error_info_`.

`get_active_plugin(id)`:

- invalid id: error code `0x494b5`, return `NULL`;
- slot flag false: error code `0x494b8`, message
  `"In CTssSdkPluginMgr::get_active_plugin, the plugin <id> is not activated."`,
  return `NULL`;
- slot flag true: return `total_plugin_table_[id].first` (not NULL-checked).

## `init()` order and returns

`init()` is validation only; it calls no plugin method and does not inspect the
active-id table.

1. `invalid_plugin_id_table_` non-empty: build
   `"In CTssSdkPluginMgr::init, the invalid plugin id is "` followed by every
   invalid id and `"."`; store message and error code `0x494b5`; return that
   code.
2. `repeated_plugin_id_table_` non-empty: same with
   `"In CTssSdkPluginMgr::init, the repeated plugin id is "` and code
   `0x494b6`; return that code.
3. `registered_plugin_count_ == 0`: store
   `"In CTssSdkPluginMgr::init, no plugin registered."` and code `0x494b4`;
   return that code.
4. Otherwise return `0`.

Failure checks are ordered before the empty-count check, so an invalid or
repeated registration masks "no plugin registered".

## Errors and strings

Numeric codes written into `error_info_.last_error_info()->error_code`:

| Code | Decimal | Meaning in this TU |
|---|---|---|
| `0x494b4` | 300212 | `init`: no plugin registered |
| `0x494b5` | 300213 | invalid plugin id (`get_active_plugin`, `deactivate_plugin`, `activate_plugin`, `init`) |
| `0x494b6` | 300214 | `init`: repeated plugin id |
| `0x494b7` | 300215 | `activate_plugin`: plugin already activated |
| `0x494b8` | 300216 | `get_active_plugin`: plugin not activated |

The symbolic names of these five constants are not present in the DWARF enum
data (the compiler emitted the literals); only the numeric values are claimed.

Exact messages/format strings recovered from `.rodata`:

```
In CTssSdkPluginMgr ctor. Address is 0x%x.
In CTssSdkPluginMgr::register_plugin, registered plugin id is %d.The address of plugin is 0x%x.The 'first' is 0x%x.The 'second' is %d.
In CTssSdkPluginMgr::deactivate_plugin, plugin id is %d.The 'first' is 0x%x.The 'second' is %d.
In CTssSdkPluginMgr::activate_plugin, plugin id is %d. The 'first' is 0x%x. The 'second' is %d.
In CTssSdkPluginMgr::get_active_plugin, the invalid plugin id is <id>.
In CTssSdkPluginMgr::get_active_plugin, the plugin <id> is not activated.
In CTssSdkPluginMgr::deactivate_plugin, the invalid plugin id is <id>.
In CTssSdkPluginMgr::activate_plugin, the invalid plugin id is <id>.
In CTssSdkPluginMgr::activate_plugin, the plugin <id> is already activated.
In CTssSdkPluginMgr::init, the invalid plugin id is <id>.<id>.
In CTssSdkPluginMgr::init, the repeated plugin id is <id>.<id>.
In CTssSdkPluginMgr::init, no plugin registered.
```

`get_last_error_code()` returns `error_info_.last_error_info()->error_code`
(macro `m_plugin_mgr_last_error_code` from `tss_sdk_plugin_mgr.h:18`).
`get_last_error()` returns `""` when the code is `0`, otherwise
`get_last_error_msg()` copied with `strlen`; the result starts from the
process-wide empty `std::string` representation.

## Constructor / destructor

Ctor (both C1/C2 copies, source lines 20-23):

- value-initializes `registered_plugin_count_ = 0`;
- default-constructs the four vectors (all zeroed);
- constructs `error_info_`;
- `total_plugin_table_.resize(8, pair<ITssSdkPlugin*,bool>(NULL,false))`
  (DWARF line 21 maps to `std::vector<pair<...> >::resize`; disassembly calls
  `_M_fill_insert(begin, 8, zero_pair)`);
- sets `error_info_.last_error_info()->error_code = 0`;
- logs `"In CTssSdkPluginMgr ctor. Address is 0x%x."` with `this`.

Dtor (D1/D2, source line 34) is default member destruction in reverse order:
`CThreadErrorInfo::~CThreadErrorInfo()` at `+0x34`, then frees vector storage at
`+0x28`, `+0x1c`, `+0x10`, `+0x04`. No vtable, no plugin deletion.

## External boundaries

Undefined references left for the real SDK/zenlib/runtime:

- `CThreadErrorInfo::CThreadErrorInfo()`, `~CThreadErrorInfo()`,
  `last_error_info()`, `set_last_error_msg(char const*, ...)`,
  `get_last_error_msg()`;
- `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)`;
- `ITssSdkPlugin::get_id()`;
- libstdc++ (`std::string`, `basic_ostringstream`, `std::remove`,
  `std::find`, allocator/vector helpers, `strlen`, `operator new/delete`).

No socket, IPC, DP, bill or user-control code is referenced by this TU.
`Singleton<TssSdkPluginMgr>::instance()` / `TssSdkFramework` life-cycle code is
outside this assignment; the manager methods are the complete manager object
logic and can be linked under the real framework.

## Compile result

Built with c5root GCC 4.1.2 and the exact target optimization/ABI flags:

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
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_plugin_mgr_agent.cpp \
  -o /tmp/tss_sdk_plugin_mgr_agent.o
```

Result: **exit 0, no diagnostics**.

Defined manager symbols in the object:

```
TssSdkPluginMgr::TssSdkPluginMgr()                      C1
TssSdkPluginMgr::TssSdkPluginMgr()                      C2
TssSdkPluginMgr::~TssSdkPluginMgr()                     D1
TssSdkPluginMgr::~TssSdkPluginMgr()                     D2
TssSdkPluginMgr::register_plugin(ITssSdkPlugin*)
TssSdkPluginMgr::get_registered_plugin_count()
TssSdkPluginMgr::init()
TssSdkPluginMgr::activate_plugin(ETssSdkPluginId)
TssSdkPluginMgr::deactivate_plugin(ETssSdkPluginId)
TssSdkPluginMgr::get_active_plugin(ETssSdkPluginId)
TssSdkPluginMgr::get_active_plugin_id_table()
TssSdkPluginMgr::get_last_error()
TssSdkPluginMgr::get_last_error_code()
TssSdkPluginMgr::is_id_invalid(ETssSdkPluginId)
TssSdkPluginMgr::is_id_repeated(ETssSdkPluginId)
TssSdkPluginMgr::is_id_activated(ETssSdkPluginId)
```

Object disassembly was checked for the shipped field offsets: counter at
`+0x00`, invalid at `+0x04`, repeated at `+0x10`, active at `+0x1c`, total at
`+0x28`, error info at `+0x34`; `total_plugin_table_` slot stride 8; flag at
`pair+0x04`; error codes `0x494b4..0x494b8` at the branches listed above.

## Unresolved

- The symbolic enum/macro names for error codes 300212..300216 are absent
  from the binary's DWARF; only numeric values are asserted.
- Whether another TU later clears `error_info_` or calls `init()` repeatedly is
  outside this manager TU; no such behavior exists inside the reconstructed
  methods.
