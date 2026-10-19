# TssSdkFramework lifecycle/config reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
Original TU: `src/formmog/tss_sdk/framework/tss_sdk_framework.cpp`
(DWARF producer path
`/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/`).
Emitted source: `tss_sdk_framework_lifecycle_agent.cpp`.

Conclusions come from the shipped symbol table, DWARF types/line tables,
`.rodata` strings, GOT relocations and disassembly. Addresses are link-time
virtual addresses. No plugin/comm/config implementation was invented; manager,
comm-service, timer-handler and configuration constructors/parsers remain
external symbols.

## Address ranges and source lines

`high_pc` is the DWARF subprogram end; inter-symbol padding is excluded.

| Method | Symbols | Range | Definition line |
|---|---|---|---|
| `TssSdkFramework::TssSdkFramework()` | C1 `0x91680`, C2 `0x91690` | `0x91680-0x91684`, `0x91690-0x91694` | 60 |
| `TssSdkFramework::~TssSdkFramework()` | D1 `0x916a0`, D2 `0x916b0` | `0x916a0-0x916a4`, `0x916b0-0x916b4` | 67 |
| `get_plugin_id(unsigned short)` | `0x91860` | `0x91860-0x918da` | 798 |
| `get_last_error_code()` | `0x918e0` | `0x918e0-0x91907` | 772 |
| `get_last_error()` | `0x91910` | `0x91910-0x91956` | 778 |
| `init_config()` | `0x91960` | `0x91960-0x91b25` | 101 |
| `get_not_need_channel_plugin_id_table_()` | `0x91b30` | `0x91b30-0x91c33` | 74 |
| `init_log(char const*, ZEN_LOG_PRIORITY)` | `0x922f0` | `0x922f0-0x9244a` | 167 |
| `init(unsigned int)` | `0x92450` | `0x92450-0x92727` | 201 |
| `fini(ETssSdkPluginId)` | `0x92730` | `0x92730-0x93224` | 365 |

Header declaration lines (framework.h): ctor 19, dtor 20, `init` 21, `fini`
24, `get_last_error_code` 27, `get_last_error` 28, `init_config` 31,
`init_log` 32, `get_not_need...` 33, `get_plugin_id` 35. The eight lifecycle/config/accessor
functions (`get_plugin_id`, `get_last_error_code`, `get_last_error`,
`init_config`, `get_not_need...`, `init_log`, `init`, `fini`) are **static
members**: their bodies have no `this` argument (first user argument at
`[ebp+8]`), and the shipped
`__PRETTY_FUNCTION__` string begins with `static`. The ctor/dtor are ordinary
non-static members with an `this` argument.

`TssSdkFramework` has no non-static data members; DWARF `sizeof == 1`, and the
ctor/dtor bodies are only `push ebp; mov esp,ebp; pop ebp; ret`.

## Global state (exact types/addresses/sizes)

All are private static members of `tss_sdk_impl::TssSdkFramework` except the
three file-scope config globals.

| Symbol | Address | Size | Type / initial state |
|---|---|---|---|
| `is_init_succ_` | `0x2a00ab` | 1 | `volatile bool`, `.bss` (false) |
| `thread_mutex_` | `0x2a00ac` | 0x1c | `ZEN_Thread_Mutex`, `.bss` |
| `error_info_` | `0x2a00c8` | 4 | `CThreadErrorInfo`, `.bss` |
| `plugin_mgr_` | `0x2a00cc` | 4 | `TssSdkPluginMgr *`, `.bss` |
| `comm_svc_` | `0x2a00d0` | 4 | `TssSdkCommSvc *`, `.bss` |
| `not_need_channel_plugin_id_table_` | `0x2a00d4` | 0xc | `std::vector<tss_sdk_if::ETssSdkPluginId>`, `.bss` |
| `timer_handler_` | `0x2a00e0` | 4 | `TssSdkTimerHandler *`, `.bss` |
| `g_is_old_sdk` | `0x29793c` | 4 | `volatile int`, `.data`, initial value **1** |
| `g_path_config` | `0x29f6c0` | 0x400 | `PATH_CONFIG` (`tss_sdk_conf_path_desc::path_config`), `.bss` |
| `g_config` | `0x29fac0` | 0x5eb | `CONFIG` (`tss_sdk_conf_desc::config`), `.bss` |

`g_config`, `g_path_config`, `g_is_old_sdk` are plain unmangled symbols at
global namespace scope, so they are emitted that way in the reconstruction.

## Class / struct layouts used

DWARF and `-fdump-class-hierarchy` on the reconstruction agree:

- `TssSdkFramework`: size 1, alignment 1, no members.
- `tss_sdk_conf_desc::config`: size `0x5eb`, byte-packed:
  - `+0x000 comm_config_` (extent `0x4ca`)
  - `+0x4ca log_config_`; `log_level_` (`int32`) at `+0x4ca`
  - `+0x4ce usercontrol_config_`; `use_idx_` at `+0x4ce`,
    `max_client_obj_count_` at `+0x4d2`
  - `+0x4d6 uic_config_` (extent `0x115`), ends at `0x5eb`.
- `tss_sdk_conf_path_desc::path_config`: size `0x400`;
  `config_path[1024]` at `+0x000`.
- `tss_sdk_impl::TssSdkPluginMgr`: size 56, no vtable:
  `+0x00` count, `+0x04/+0x10/+0x1c` three id vectors, `+0x28` total plugin
  vector (`pair<ITssSdkPlugin*,bool>`), `+0x34 CThreadErrorInfo`.
- `tss_sdk_impl::TssSdkCommSvc`: size 40:
  `+0x00 channel_id_`, `+0x04 plugin_mgr_`, `+0x08 dp_if_`,
  `+0x0c TssSdkChannelStatus` (six `unsigned int` counters),
  `+0x24 error_info_`.
- `tss_sdk_impl::TssSdkTimerHandler`: size 12:
  `+0x00 time_provider_`, `+0x04 stat_monitor_`, `+0x08 last_check_`.
- `tss_sdk_impl::ITssSdkPlugin`: size 16; vptr `+0x00`, callback `+0x04`,
  `CThreadErrorInfo` `+0x08`, plugin id `+0x0c`. Vtable address point slots:
  0/1 dtor D1/D0, 2 `init(void*)`, 3 `fini()`, 4 `proc(void*)`,
  5 `notify(void*)`. `fini()` is therefore the virtual call through
  `vptr+0x0c`; `get_last_error()` is non-virtual.
- `CThreadErrorInfo`: size 4 (`pthread_key_t`); keyed `SErrorInfo` is
  `{ int error_code; char error_msg[256]; }` (260 bytes).
- `ZEN_Thread_Mutex`: size `0x1c`: polymorphic base subobject (4-byte vptr) at
  `+0x00`, 24-byte pthread mutex at `+0x04`. Only the external
  ctor/lock/unlock/dtor are used; the reconstruction models the base as opaque
  4-byte storage so it does not emit a partial conflicting vtable.
- `ZEN_Lock_Guard<MUTEX>`: size 4, holds a `MUTEX*`; ctor locks, dtor unlocks.

Compile-time checks run against the reconstruction confirmed
`sizeof(TssSdkFramework)==1`, `sizeof(TssSdkPluginMgr)==56`,
`sizeof(TssSdkCommSvc)==40`, `sizeof(TssSdkTimerHandler)==12`,
`sizeof(ITssSdkPlugin)==16`, `sizeof(ZEN_Thread_Mutex)==28`,
`sizeof(CThreadErrorInfo)==4`, `sizeof(path_config)==1024`,
`sizeof(config)==0x5eb`, and config member offsets `0x4ca/0x4ce/0x4d6`.

## Global initialization decoded (0x91c80-0x922c3)

`__static_initialization_and_destruction_0` and `global constructors keyed to
g_is_old_sdk` (`0x922d0`, calls with `eax=1`, `edx=0xffff`) do:

1. `std::ios_base::Init` for `std::__ioinit` at `0x2a00e4`.
2. Construct 16 `ZEN_STATUS_ITEM_WITHNAME` objects (ids `0x238d`-`0x239c`,
   0x5c bytes each) into local symbol `COMM_STAT_ITEM_WITH_NAME` at
   `0x2a0100`, size `0x5c0`, and register their destructor table. This is ZEN
   status-monitor machinery from the same TU, unrelated to the assigned
   lifecycle methods; it is not reconstructed in the source file.
3. `thread_mutex_` with `ZEN_Thread_Mutex(char const*)`, argument `NULL`.
4. `error_info_` with `CThreadErrorInfo()`.
5. `Singleton<TssSdkPluginMgr>::instance()` constructs the function-local
   singleton object at `0x29f680` (guard at `0x29f658`) and
   `plugin_mgr_ = &object`.
6. `Singleton<TssSdkCommSvc>::instance()` constructs its singleton object at
   `0x29d820` (guard at `0x29d7f0`) and `comm_svc_ = &object`.
7. Zero-initializes the three pointers of `not_need_channel_plugin_id_table_`.
8. `operator new(0xc)` + `TssSdkTimerHandler::TssSdkTimerHandler()`, stored in
   `timer_handler_`.
9. Implicit `Singleton<T>::object_creator` statics (guards at `0x29f660` /
   `0x29d7f8`) run their `ObjectCreator` constructors, which call
   `instance()` again (already constructed). These are template-instantiation
   machinery; the reconstruction models their net effect through the two
   `Singleton<T>::instance()` initializers rather than defining a competing
   `Singleton` template body.

The source file reproduces this order with static-member definitions:
`thread_mutex_(NULL)`, `error_info_`, `plugin_mgr_`, `comm_svc_`, empty vector,
`new TssSdkTimerHandler()`. `Singleton<T>::instance()` is left as the shipped
weak external symbol.

## Method semantics

### `get_last_error_code()` / `get_last_error()` (0x918e0 / 0x91910)

- code: `error_info_.last_error_info()->error_code` (`int`).
- `get_last_error()` returns `char const*`, not `std::string`: `""` when the
  code is 0 (the empty literal at `0x21bc86`), otherwise
  `error_info_.get_last_error_msg()`. The empty path does not construct a
  string.

### `get_plugin_id(unsigned short busi_id)` (0x91860)

The function uses a 14-entry switch table over ids 0..13:

| busi id | returned plugin id |
|---|---|
| 1, 2, 10 | `ETSS_SDK_PLUGIN_ID_PROTO_PROC` = 3 |
| 3 | `ETSS_SDK_PLUGIN_ID_CAPTCHA` = 5 |
| 13 | `ETSS_SDK_PLUGIN_ID_SAFEMODE` = 4 |
| anything else | log error, return `ETSS_SDK_PLUGIN_ID_INVALID` = -2 |

Invalid path calls
`ZEN_Trace_LogMsg::debug_errorex("%s, invalid busi id %u.", __PRETTY_FUNCTION__, (unsigned)busi_id)`.
`busi_id` is zero-extended from 16 bits. No error-info state is touched.

### `init_config()` (0x91960)

Locals (DWARF): `int ret` (line 106), `char conf_path_file[1024]` (107),
`char conf_file[1024]` (108), `size_t len` (146).

1. `g_config.construct()` (return ignored).
2. `memset(conf_path_file, 0, 0x400)`, `memset(conf_file, 0, 0x400)`.
3. If `g_is_old_sdk == 0`:
   - `snprintf(conf_path_file, 0x400, "%s", "./tss_sdk_conf_path.xml")`
   - `snprintf(conf_file, 0x400, "%s", "./tss_sdk_conf.xml")`
   - `g_config.usercontrol_config_.use_idx_ = 0` (store at `g_config+0x4ce`).
   Else:
   - `"./antisvrimport_conf_path.xml"`, `"./antisvrimport_conf.xml"`
   - `g_config.usercontrol_config_.use_idx_ = 1`.
4. If `ZEN_OS::access(conf_path_file, 0) == 0` and
   `g_path_config.fromXmlFile(conf_path_file, LIST_ENTRY /*0*/, 0) == TDR_NO_ERROR`:
   - `len = strlen(g_path_config.config_path)`;
   - if `len > 0x300` return **-1** (the only failure return);
   - `memcpy(conf_file, g_path_config.config_path, len)`;
   - `conf_file[len] = '\0'`.
5. If `ZEN_OS::access(conf_file, 0) == 0`, call
   `g_config.fromXmlFile(conf_file, LIST_ENTRY, 0)` and ignore its result.
6. Return `ret`, which is 0.

Consequences preserved: missing/invalid XML files are not errors; a successful
path-config parse replaces the compiled-in config filename with the path from
`g_path_config.config_path`; only an over-0x300 path length fails. The
`construct()` defaults remain in `g_config` when parsing fails.

### `init_log(const char*, ZEN_LOG_PRIORITY)` (0x922f0)

- `log_dir == NULL`: `debug_errorex("log_dir == NULL")`, return -1.
- Construct `std::string log_file_prefix(log_dir)`.
- `const size_t LOG_SIZE_MAX = 100 * 1024 * 1024` (`0x6400000`).
- `ZEN_Trace_LogMsg::instance()->initialize(LOGDEVIDE_BY_SIZE /*101*/,
  log_file_prefix.c_str(), true, true, LOG_SIZE_MAX, 2, 1, 15)`.
  The two bools are thread synchronization / auto-newline; `2` is
  `reserve_file_num`; `1` is `output_way`; `15` is `head_record`.
- Nonzero return: `debug_errorex("init_log initialize failed")`, return -1.
- Otherwise `instance()->set_log_priority(log_priority)` (return ignored) and
  return 0.

### `init(unsigned int instance_id)` (0x92450)

1. `int rc = 0`; `ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_)`
   (lock acquired at entry, released on every return).
2. If `is_init_succ_` is already true: `debug_infoex("TssSdkFramework init
   successful")`, set `is_init_succ_ = true`, `rc = 0`.
3. Else `rc = init_config()`.
   - If nonzero: `err_str = "In CTssSdkFramework::init, init_config failed."`;
     `error_info_.set_last_error_msg(err_str.c_str())`;
     `debug_errorex("%s", error_info_.get_last_error_msg())`;
     `error_info_.last_error_info()->error_code = 0x494b9`;
     `rc = 0x494b9`.
   - Else:
     - `LOG_FILE_NAME_MAX = 1024`; `char log_file_name[1025]` zeroed;
       `snprintf(log_file_name, 1024, "%s_%d", "/tmp/tss_sdk", instance_id)`
       (return value unused; result e.g. `/tmp/tss_sdk_<id>`).
     - `rc = init_log(log_file_name, (ZEN_LOG_PRIORITY)
       g_config.log_config_.log_level_)` (load from `g_config+0x4ca`).
     - If nonzero: the **same** `"init_config failed."` literal is reused
       (same `.rodata` address `0x219c98`), same set/log steps and
       `error_code = 0x494b9`, `rc = 0x494b9`.
     - Else: `debug_infoex("TssSdkFramework init successful")`,
       `is_init_succ_ = true`, `rc = 0`.
4. Unlock (guard dtor), return `rc`.

Thus `init` succeeds for an already-initialized framework (and re-sets the
flag), and configuration read failures are not surfaced as `init` failures.
Only `init_config() == -1` or `init_log() != 0` yields 300217.

### `fini(ETssSdkPluginId plugin_id)` (0x92730)

1. `is_init_succ_ = false` **before** taking the mutex.
2. `ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_)`.
3. Local `std::vector<ETssSdkPluginId> plugin_id_table`:
   - `plugin_id == ETSS_SDK_PLUGIN_ID_ALL (-1)`:
     `plugin_id_table = *plugin_mgr_->get_active_plugin_id_table()`;
   - otherwise `push_back(plugin_id)`.
4. For each id in the local vector:
   - `plugin = plugin_mgr_->get_active_plugin(id)`;
     if NULL: build
     `"In CTssSdkFramework::fini, plugin mgr get active plugin " + id +
     " failed. " + plugin_mgr_->get_last_error()`, set it as the framework
     error message, log `"%s"`, store error code **0x494b1 (300209)** and
     return it.
   - `plugin->fini()` (virtual slot at vtable+0x0c). Nonzero: build
     `"In CTssSdkFramework::fini, plugin " + id + " fini failed. " +
     plugin->get_last_error()`, set/log, store error code **0x494ab (300203)**
     and return it.
   - `plugin_mgr_->deactivate_plugin(id)`. Nonzero: build
     `"In CTssSdkFramework::fini, plugin mgr deactivate plugin " + id +
     " failed. " + plugin_mgr_->get_last_error()`, set/log, store error code
     **0x494b0 (300208)** and return it.
5. After the loop: `get_not_need_channel_plugin_id_table_()` (return ignored),
   return 0.

Error paths use a default-constructed `std::ostringstream`
(`std::_Ios_Openmode 0x10`), `set_last_error_msg(oss.str().c_str())`, then
`debug_errorex("%s", error_info_.get_last_error_msg())`, then write and read
`error_info_.last_error_info()->error_code`. Plugins are only deactivated, not
deleted. `fini` stops at the first failing plugin, so later ids in the snapshot
remain active.

### `get_not_need_channel_plugin_id_table_()` (0x91b30)

1. `not_need_channel_plugin_id_table_ =
   *plugin_mgr_->get_active_plugin_id_table()` (copy assignment).
2. Erase-remove, in this exact order:
   `ETSS_SDK_PLUGIN_ID_CHANNEL (1)`, `SAFEMODE (4)`, `CAPTCHA (5)`,
   `PROTO_PROC (3)`. Four `std::remove` calls followed by one
   `vector::erase(iter, end)`.
3. Return 0.

The surviving snapshot therefore contains only ids that were active and are
not 1/3/4/5 (normally 2/6/7). The channel id is removed too, despite the
function name.

## Return/error-code summary

No symbolic names for these five constants exist in the DWARF enumerations;
only the literal values are asserted.

| Value | Decimal | Site |
|---|---|---|
| `0x494b9` | 300217 | `init`: `init_config()` nonzero or `init_log()` nonzero |
| `0x494b1` | 300209 | `fini`: `get_active_plugin()` returned NULL |
| `0x494ab` | 300203 | `fini`: `plugin->fini()` returned nonzero |
| `0x494b0` | 300208 | `fini`: `deactivate_plugin()` returned nonzero |
| `-2` | `ETSS_SDK_PLUGIN_ID_INVALID` | `get_plugin_id` default |

`get_plugin_id` returns `3` (PROTO_PROC), `5` (CAPTCHA), or `4` (SAFEMODE) on
its valid business ids. `init_config` returns `-1` only for a path longer than
0x300 bytes, otherwise 0. `get_not_need...` always returns 0. `init` and
`fini` return 0 on their success paths/empty snapshots.

Exact `.rodata` messages recovered and used:

```
%s, invalid busi id %u.
./tss_sdk_conf_path.xml
./tss_sdk_conf.xml
./antisvrimport_conf_path.xml
./antisvrimport_conf.xml
%s
log_dir == NULL
init_log initialize failed
/tmp/tss_sdk
%s_%d
In CTssSdkFramework::init, init_config failed.
TssSdkFramework init successful
In CTssSdkFramework::fini, plugin mgr get active plugin
In CTssSdkFramework::fini, plugin
In CTssSdkFramework::fini, plugin mgr deactivate plugin
 failed.
 fini failed.
```

The init/log messages are called with
`debug_errorex("%s", error_info_.get_last_error_msg())`; the
"invalid busi id" message passes `__PRETTY_FUNCTION__` and the numeric id.

## External boundaries / call targets

All remain undefined in the reconstruction object and are provided by the real
SDK/zenlib/runtime:

- `tss_sdk_conf_desc::config::construct()` `0xe7bf0`;
  `config::fromXmlFile(char const*, tsf4g_tdr::TdrXmlFormat, unsigned int)`
  `0xe9f40`.
- `tss_sdk_conf_path_desc::path_config::fromXmlFile(...)` `0xeeb30`.
- `ZEN_OS::access(char const*, int)` `0x1a2490` (mode 0).
- `ZEN_Trace_LogMsg::instance()` `0x1b4090`,
  `debug_errorex(char const*, ...)` `0x1b3d30`,
  `debug_infoex(char const*, ...)` `0x1b3d80`.
- `ZEN_LogTrace_Basic::initialize(ZEN_LOGFILE_DEVIDE, char const*, bool, bool,
  size_t, size_t, unsigned int, unsigned int)` `0x1b3850`;
  `set_log_priority(ZEN_LOG_PRIORITY)` `0x1b1030`.
- `CThreadErrorInfo` ctor `0x1ad540`, dtor `0x1ad4e0`,
  `last_error_info()` `0x1ad390`, `set_last_error_msg(char const*, ...)`
  `0x1ad470`, `get_last_error_msg()` `0x1ad440`.
- `ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)`, `lock()`, `unlock()`,
  dtor.
- `Singleton<tss_sdk_impl::TssSdkPluginMgr>::instance()` `0x914f0` and
  `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()` `0x8af00` (weak).
- `TssSdkPluginMgr::get_active_plugin` `0x98740`,
  `get_active_plugin_id_table` `0x979a0`, `deactivate_plugin` `0x99210`,
  `get_last_error` `0x97bd0`.
- `ITssSdkPlugin::get_last_error()` `0x972d0`; virtual `fini()` through
  address point slot 3.
- `TssSdkTimerHandler::TssSdkTimerHandler()` `0x9b450`.
- libstdc++ (`std::string`, `std::ostringstream`, vector algorithms,
  allocator/exception helpers) and libc (`snprintf`, `memset`, `memcpy`,
  `strlen`, `operator new/delete`).

## Compile result

Built with c5root GCC 4.1.2 and the required target flags:

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
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_framework_lifecycle_agent.cpp \
  -o /tmp/tss_sdk_framework_lifecycle_agent.o
```

Result: **exit 0, no diagnostics**. The object defines all ten reconstructed
functions (C1/C2 and D1/D2 included), the three config globals and the seven
framework statics, and leaves exactly the external references listed above.
The object was also checked with `-fdump-class-hierarchy` and separate
compile-time size/offset assertions; both passed. Disassembly of the
reconstruction was compared against the shipped bodies for argument offsets,
config field offsets `0x4ca`/`0x4ce`, the `init` `/tmp/tss_sdk_%d` buffer and
log-level load, the `+0x0c` virtual `fini` call, the remove order
1/4/5/3, all error-code immediates, and the `%s` log/error-string sequence.

## Unresolved / out-of-scope observations

- Symbolic names for the four numeric framework error codes are absent from
  the binary's DWARF; numeric values only.
- The same static-init function constructs an unrelated 16-entry
  `ZEN_STATUS_ITEM_WITHNAME` table (`0x2a0100`, ids `0x238d`-`0x239c`) used by
  the ZEN status-monitor machinery. It is part of the original TU's global
  initialization but outside the assigned lifecycle/config methods and is not
  reconstructed.
- `Singleton<T>::object_creator` static members and their guard variables are
  implicit template machinery. They are not re-declared in the reconstruction;
  the two external `Singleton<T>::instance()` initializers reproduce the
  observable construction of both singleton objects.
- `g_is_old_sdk` is a mutable `volatile int` shared with other TUs and is 1 in
  the shipped image. `init_config` branches on its runtime value; the
  reconstruction defines the same shared global rather than assuming a
  constant.
- `init_config` ignores `config::fromXmlFile` and `path_config::fromXmlFile`
  parse failures (the latter only gates the path copy); this is shipped
  behavior, not missing reconstruction.
- `init` reports the literal "init_config failed." even when `init_log` is the
  failing step; verified at the same `.rodata` address in the shipped binary.
