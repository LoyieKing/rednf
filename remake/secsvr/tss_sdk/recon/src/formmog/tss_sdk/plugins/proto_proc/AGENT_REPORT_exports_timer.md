# TssSdkTimerHandler / ITssSdkPlugin / interface factories reconstruction

Target: `libtss_sdk.so.1.0.394` (ELF32 i386, GCC 4.1.x, `-O2 -fPIC`),
SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
Output: `recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_exports_timer_agent.cpp`.

The assigned functions come from eight original translation units, all merged
into one standalone reconstruction file.  Source file names and line-oriented
declarations below come from DWARF; layouts are from DWARF and were checked
against the instruction stream.

## 1. Address ranges (original virtual addresses; end = start + size)

| Function | Start | Size |
|---|---|---|
| `TssSdkTimerHandler::TssSdkTimerHandler()` C1 | `0x0009b450` | `0x42` |
| `TssSdkTimerHandler::TssSdkTimerHandler()` C2 | `0x0009b4a0` | `0x42` |
| `TssSdkTimerHandler::~TssSdkTimerHandler()` D2 | `0x0009b000` | `0x15` |
| `TssSdkTimerHandler::~TssSdkTimerHandler()` D1 | `0x0009b020` | `0x15` |
| `TssSdkTimerHandler::check_monitor(time_t)` | `0x0009b4f0` | `0xb4` |
| `TssSdkTimerHandler::update_time(time_t)` | `0x0009b5b0` | `0x64` |
| `ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId)` C2 | `0x000977b0` | `0xe9` |
| `ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId)` C1 | `0x000978a0` | `0xe9` |
| `ITssSdkPlugin::~ITssSdkPlugin()` D0 | `0x00097200` | `0x42` |
| `ITssSdkPlugin::~ITssSdkPlugin()` D1 | `0x00097250` | `0x31` |
| `ITssSdkPlugin::~ITssSdkPlugin()` D2 | `0x00097290` | `0x31` |
| `ITssSdkPlugin::get_last_error()` | `0x000972d0` | `0xd0` |
| `ITssSdkPlugin::register_callback(TssSdkPluginCallbackProc)` | `0x00097110` | `0x0e` |
| `ITssSdkPlugin::get_id()` | `0x00097120` | `0x0b` |
| `ITssSdkPlugin::get_last_error_code()` | `0x000971d0` | `0x28` |
| `GetAntiBotInterfaceAll()` | `0x00088980` | `0x2c` |
| `tss_sdk_get_anti_interf(const TssSdkAntiInitInfo*)` | `0x0008a900` | `0x6c` |
| `tss_sdk_get_comm_interf(const TssSdkCommInitInfo*)` | `0x0008b010` | `0xe9` |
| `tss_sdk_get_captcha_interf(const TssSdkCaptchaInitInfo*)` | `0x0008b760` | `0x45` |
| `tss_sdk_get_safemode_interf(const TssSdkSafemodeInitData*)` | `0x0008c0d0` | `0x45` |
| `tss_sdk_get_uic_interf(const TssSdkUicInitInfo*)` | `0x0008c910` | `0x45` |

Original TUs:

- `src/formmog/tss_sdk/framework/tss_sdk_time_handler.cpp` (`0x9b...`)
- `src/formmog/tss_sdk/framework/tss_sdk_plugin.cpp` (`0x97...`)
- `src/formmog/tss_sdk/adapter/antibotinterface.cpp` (`0x88980`)
- `src/formmog/tss_sdk/busi_interf/busi_impl/tss_sdk_busi_impl.cpp` (`0x8b010`)
- `src/formmog/tss_sdk/busi_interf/anti/tss_sdk_anti_impl.cpp` (`0x8a900`)
- `src/formmog/tss_sdk/busi_interf/captcha/tss_sdk_captcha_impl.cpp` (`0x8b760`)
- `src/formmog/tss_sdk/busi_interf/safemode/tss_sdk_safemode_impl.cpp` (`0x8c0d0`)
- `src/formmog/tss_sdk/busi_interf/uic/tss_sdk_uic_impl.cpp` (`0x8c910`)

## 2. Layouts (DWARF, checked against stores/loads)

`TssSdkTimerHandler`, sizeof **12** (`tss_sdk_time_handler.cpp`):

```
+0x00 Comm_Time_Provider* time_provider_
+0x04 Comm_Stat_Monitor*  stat_monitor_
+0x08 time_t             last_check_
```

`Comm_Time_Provider` = 8 bytes, `ZEN_Time_Value cur_time_` at +0.
`ZEN_Time_Value` = 8 bytes (`struct timeval zen_time_value_`).
`Comm_Stat_Monitor` derives from `ZEN_Server_Status` at offset 0.

`ITssSdkPlugin`, sizeof **16** (`tss_sdk_plugin.cpp`):

```
+0x00 vptr
+0x04 TssSdkPluginCallbackProc callback_proc_    (int (*)(void*))
+0x08 CThreadErrorInfo error_info_               (sizeof 4)
+0x0c ETssSdkPluginId sdk_plugin_id_
```

Vtable `0x00292d80`, address point `0x00292d88`:

```
+0x00 offset-to-top 0
+0x04 typeinfo for ITssSdkPlugin (0x00292da0)
+0x08 D1 0x00097250
+0x0c D0 0x00097200
+0x10 init  = __cxa_pure_virtual
+0x14 fini  = __cxa_pure_virtual
+0x18 proc  = __cxa_pure_virtual
+0x1c notify= __cxa_pure_virtual
```

`TssSdkPluginMgr`, sizeof **56** (same layout as
`tss_sdk_plugin_mgr.cpp`; only storage size is needed here):

```
+0x00 u32 registered_plugin_count_
+0x04 std::vector<ETssSdkPluginId> invalid_plugin_id_table_   (12)
+0x10 std::vector<ETssSdkPluginId> repeated_plugin_id_table_  (12)
+0x1c std::vector<ETssSdkPluginId> active_plugin_id_table_    (12)
+0x28 std::vector<std::pair<ITssSdkPlugin*,bool> > total_plugin_table_ (12)
+0x34 CThreadErrorInfo error_info_                            (4)
```

`Singleton<tss_sdk_impl::TssSdkPluginMgr>::instance()` is a global-namespace
template with a function-local `static T obj`.  Original symbols (BSS):

```
0x0029f658  guard  _ZGVZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE8instanceEvE3obj
0x0029f680  object _ZZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE8instanceEvE3obj  (56 bytes)
```

`AntibotInterface` = 4 bytes, base `IAntibotInterface` = 4 bytes (vptr only).
`IAntibotInterface` vtable `0x00292d20` has exactly 18 pure-virtual slots and
no virtual destructor.  `AntibotInterface` vtable `0x00292ca0` has the same 18
slots at +0x08.. +0x50 and D1/D0 at +0x54/+0x58.  `g_antibot_if` is the
4-byte file-local BSS object at `0x0029d060`.

## 3. Timer handler semantics

`TssSdkTimerHandler::TssSdkTimerHandler()`:

1. `time_provider_ = Comm_Time_Provider::instance()` (PLT `0x83474`).
2. `stat_monitor_ = Comm_Stat_Monitor::instance()` (PLT `0x7bf84`).
3. `last_check_ = time(NULL)` (`time@plt 0x81b04`, argument 0).

`~TssSdkTimerHandler()` (both D variants): writes 0 to `time_provider_` and
`stat_monitor_`; `last_check_` is not touched.  No virtual destructor.

`update_time(time_t now_sec)`:

```
ZEN_Time_Value zen_now(now_sec);          // ctor@plt 0x84574
time_provider_->cur_time_ = zen_now;      // operator=@plt 0x80424
                                          // ~ZEN_Time_Value@plt 0x7f244
```

`check_monitor(time_t now_sec)`:

1. `if (now_sec % 300 != 0) return;` (signed remainder).
2. `if (now_sec == last_check_) return;`.
3. `stat_monitor_->increase_by_statid(0x2395, 0, 0, 1)` (PLT `0x842d4`;
   the `1` is the `long long` value in stack slots +0x10/+0x14, the two u32
   middle arguments are zero).
4. `stat_monitor_->check_overtime(now_sec)` (PLT `0x7e974`; returns void).
5. `real_second = now_sec - last_check_`; if `real_second > 300`, log
   `ZEN_Trace_LogMsg::debug_errorex("check monitor more than five "
   "minutes:real_second=%d", real_second)` (PLT `0x80904`).
6. `last_check_ = now_sec` (always, including after the warning).

## 4. ITssSdkPlugin semantics

Constructor (C1/C2 identical):

```
callback_proc_ = 0;
vptr = &vtable + 2 words;
CThreadErrorInfo::CThreadErrorInfo();             // member at +8
sdk_plugin_id_ = sdk_plugin_id;
error_info_.last_error_info()->error_code = 0;    // last_error_info@plt 0x80b64
Singleton<TssSdkPluginMgr>::instance()->register_plugin(this);
```

The singleton guard/object logic is inlined in the shipped ctor:
`__cxa_guard_acquire` (`0x7b5f4`), `TssSdkPluginMgr::TssSdkPluginMgr()`
(`0x86834`), `__cxa_guard_release` (`0x81b84`), `__cxa_atexit` (`0x7eda4`),
then `TssSdkPluginMgr::register_plugin` (`0x83c54`).  The reconstructed file
uses the same global-namespace `Singleton<T>::instance()` template; the
compiler emits an out-of-line weak `instance()` (the shipped image also has
one at `0x000914f0`), so the ctor calls that function instead of duplicating
the guard inline.  Guard/object symbols, construction order and the
`register_plugin` call target are unchanged.

Destructors: D1/D2 set vptr then call `CThreadErrorInfo::~CThreadErrorInfo()`
(`0x830a4`); D0 additionally calls `operator delete(void*)` (`0x88274`).

- `register_callback(cb)`: stores `cb` at `+0x04`; returns void.
- `get_id()`: returns the `ETssSdkPluginId` at `+0x0c`.
- `get_last_error_code()`: returns `error_info_.last_error_info()->error_code`.
- `get_last_error()`: returns `std::string` by value.  The shipped code starts
  from the empty string rep, then:
  - code == 0: `assign("", 0)`;
  - otherwise: `assign(get_last_error_msg(), strlen(msg))`.
  Calls: `CThreadErrorInfo::get_last_error_msg()` (`0x7f674`),
  `strlen` (`0x824b4`), `std::string::assign(char const*, unsigned int)`
  (`0x868f4`), plus empty-rep refcount teardown.

## 5. Factory exports

All five `tss_sdk_get_*` symbols and `GetAntiBotInterfaceAll` use C linkage.

### `GetAntiBotInterfaceAll()` (`0x88980`)

```
ZEN_Trace_LogMsg::debug_infoex("In GetAntiBotInterfaceAll.");  // 0x84d14
return &g_antibot_if;                                          // 0x29d060
```

`g_antibot_if` is a file-local `AntibotInterface` object in BSS.  The
`antibotinterface.cpp` static initializer (`0x00089093`-`0x000890b6`) calls
`AntibotInterface::C1` (`0x88580`) and registers `AntibotInterface::D1`
(`0x88b60`) through `__cxa_atexit`.  The same initializer also stores
`tss_get_sdk()` (`0x91700`) into the object file's TU-local `g_tss_sdk`
(`0x0029d064`) at `0x000890bb`; that pointer is not used by the exported
function and is not duplicated in the merged reconstruction.

### `tss_sdk_get_comm_interf(const TssSdkCommInitInfo*)` (`0x8b010`)

Argument struct is 4 bytes: `unique_instance_id_` at +0.  Return type is
`const TssSdkBusiInterf*`; `TssSdkBusiInterf` is 4 bytes with `proc_` at +0.

```
g_is_old_sdk = 0;                       // global D, 0x0029793c
static TssSdkBusiInterf tss_sdk_busi_interf = { g_tss_sdk->proc };
static bool is_tss_sdk_busi_interf_inited = false;
```

- The static table has a guard at `0x0029de48`; on first pass it stores
  `g_tss_sdk->proc` (`g_tss_sdk` is the TU-local pointer at `0x0029de40`,
  initialized at `0x0008b4a3` from `tss_get_sdk()`; `proc` is field +0x0c of
  the 28-byte SDK function table).
- The bool is at `0x0029de54` (BSS, false) and is not guarded.
- If not yet initialized:
  1. `g_tss_sdk->init(init_info->unique_instance_id_)` (vtable slot +0).
     Nonzero result returns NULL.  No NULL check on `init_info` or
     `g_tss_sdk`.
  2. Builds a 12-byte `TssSdkPluginChannelInitData`:
     `unique_instance_id_` = caller value,
     `ipc_bus_size_` = `0x00800000`,
     `pid_channel_mode_` = 0.
  3. Builds `TssSdkPluginInitData`:
     `plugin_id_` = 1 (`ETSS_SDK_PLUGIN_ID_CHANNEL`),
     `callback_proc_` = `tss_sdk_impl::send_anti_data_to_client` (`0x8a970`),
     `init_data_` = address of the channel block.
  4. `g_tss_sdk->init_plugins(&plugin_init_data, 1)` (vtable slot +4).
     Nonzero result returns NULL.
- On success: `is_tss_sdk_busi_interf_inited = true;`
  `return &tss_sdk_busi_interf;` (`0x0029de50`, 4 bytes).

The bool store is duplicated by the compiler into the already-initialized
path and the successful path, as in the shipped code.

### `tss_sdk_get_anti_interf(const TssSdkAntiInitInfo*)` (`0x8a900`)

`TssSdkAntiInitInfo` is 8 bytes:

```
+0x00 TssSdkSendDataToClient send_data_to_client_
+0x04 TssSdkPunishUser       punish_user_
```

Flow:

1. If `init_data == NULL`, or `punish_user_ == NULL`, or
   `send_data_to_client_ == NULL`, log `"anti_init: invalid arg."` and return
   NULL.  The shipped instruction order tests +0x04 before +0.
2. Call `tss_sdk_impl::tss_sdk_anti_init(init_data)` (`0x8a840`,
   external to this TU).  Nonzero -> log `"anti init fail. err=%d"`, return
   NULL.
3. Otherwise return `&s_tss_sdk_anti_interf` (`0x0029790c`, 20 bytes).

### `tss_sdk_get_captcha_interf(const TssSdkCaptchaInitInfo*)` (`0x8b760`)

`TssSdkCaptchaInitInfo` is 8 bytes:
`on_recv_pic_data_` +0, `on_verify_rsp_` +4.
Calls `tss_sdk_impl::tss_sdk_captcha_init` (`0x8b650`, external) directly,
with no wrapper-side NULL checks.  Nonzero -> log
`"captcha init fail. err=%d"`, return NULL.  Success returns
`&s_tss_sdk_captcha_interf` (`0x00297920`, 12 bytes).

### `tss_sdk_get_safemode_interf(const TssSdkSafemodeInitData*)` (`0x8c0d0`)

`TssSdkSafemodeInitData` is 16 bytes:
`set_limit_info_` +0, `set_safe_status_` +4, `set_verify_info_` +8,
`set_verify_result_` +0xc.
Calls `tss_sdk_impl::tss_sdk_safemode_init` (`0x8bfd0`, external).  Nonzero ->
log `"safemode init fail. err=%d"`, return NULL.  Success returns
`&s_tss_sdk_safemode_interf` (`0x0029792c`, 8 bytes).

### `tss_sdk_get_uic_interf(const TssSdkUicInitInfo*)` (`0x8c910`)

`TssSdkUicInitInfo` is 4 bytes: `on_chat_judge_result_` +0.
Calls `tss_sdk_impl::tss_sdk_uic_init` (`0x8c7f0`, external).  Nonzero -> log
`"uic init fail. err=%d"`, return NULL.  Success returns
`&s_tss_sdk_uic_interf` (`0x00297934`, 8 bytes).

## 6. Interface tables (exact contents)

All are function-local statics in `.data`, pointer fields in the shipped
relocation order.  The reconstruction emits the same `R_386_32` entries.

`0x0029790c  s_tss_sdk_anti_interf` (20 bytes):

```
+0x00 add_user_        -> tss_sdk_impl::tss_sdk_add_user          (0x8a270)
+0x04 del_user_        -> tss_sdk_impl::tss_sdk_del_user          (0x8a1e0)
+0x08 is_cheat_pkg_    -> tss_sdk_impl::tss_sdk_is_cheat_pkg      (0x8a560)
+0x0c decrypt_pkg_     -> tss_sdk_impl::tss_sdk_decrypt_pkg       (0x8a3a0)
+0x10 encrypt_pkg_     -> tss_sdk_impl::tss_sdk_encrypt_pkg       (0x8a610)
```

Callback signatures (DWARF, exact):

```
TssSdkAddUser               TssSdkProcResult (*)(TssSdkAntiAddUserInfo*)
TssSdkDelUser               TssSdkProcResult (*)(const TssSdkAntiDelUserInfo*)
TssSdkIsCheatPkg            int (*)(const TssSdkAntiGamePkgInfo*)
TssSdkDecryptPkg            TssSdkAntiDecryptResult (*)(TssSdkAntiDecryptPkgInfo*)
TssSdkEncryptPkg            TssSdkAntiEncryptResult (*)(TssSdkAntiEncryptPkgInfo*)
```

`0x00297920  s_tss_sdk_captcha_interf` (12 bytes):

```
+0x00 start_       -> tss_sdk_impl::tss_sdk_captcha_start      (0x8b950)
+0x04 change_pic_  -> tss_sdk_impl::tss_sdk_captcha_change_pic  (0x8b7b0)
+0x08 verify_      -> tss_sdk_impl::tss_sdk_captcha_verify      (0x8b830)
```

All three return `TssSdkProcResult` and take a `const` info pointer.

`0x0029792c  s_tss_sdk_safemode_interf` (8 bytes):

```
+0x00 append_param_ -> tss_sdk_impl::tss_sdk_secoptverify_append_param (0x8bf30)
+0x04 verify_user_  -> tss_sdk_impl::tss_sdk_secoptverify_user_operate (0x8c120)
```

`append_param_` takes `TssSdkSecOptVerifyAppendParamInfo*`;
`verify_user_` takes `const TssSdkSecOptVerifyReqInfo&`.  Both return
`TssSdkProcResult`.

`0x00297934  s_tss_sdk_uic_interf` (8 bytes):

```
+0x00 judge_user_input_chat_ -> tss_sdk_impl::tss_sdk_uic_judge_input_chat (0x8ca40)
+0x04 judge_user_input_name_ -> tss_sdk_impl::tss_sdk_uic_judge_input_name (0x8c960)
```

`chat` takes `const TssSdkUicChatUserInputInfo*`; `name` takes a non-const
`TssSdkUicNameUserInputInfo*`.  Both return `TssSdkProcResult`.

The return enums are the anonymous DWARF enums:

```
TSS_SDK_PROC_OK=0, TSS_SDK_PROC_INVALID_ARG=1,
TSS_SDK_PROC_INTERNAL_ERR=2, TSS_SDK_PROC_FAIL=3
TSS_SDK_ANTI_DECRYPT_OK=0, TSS_SDK_ANTI_INTERNAL_PKG=1
TSS_SDK_ANTI_ENCRYPT_OK=0, TSS_SDK_ANTI_NOT_NEED_ENCRYPT=1
```

Public type names for the callback parameters come from the table relocation
mangled symbols (`TssSdkAntiAddUserInfo`, ...).  Callback parameter structs
inside the `*InitInfo` blocks are not named by the shipped mangled symbols;
the reconstruction forward-declares them with the DWARF sizes recorded in the
source comments (10, 24, 37, 16, 1032, 8, 1032, 12, 22 bytes).

## 7. Common SDK table and `tss_get_sdk()`

`tss_get_sdk()` (`0x91700`) returns `&tss_get_sdk::tss_sdk`
(`0x00297944`, 28 bytes).  The 7-pointer table is modeled as
`TssSdkInterface`:

```
+0x00 int (*init)(unsigned int)
+0x04 int (*init_plugins)(const tss_sdk_if::TssSdkPluginInitData*, unsigned int)
+0x08 int (*fini)(ETssSdkPluginId)
+0x0c int (*proc)()
+0x10 int (*notify)(TssSdkPluginNotifyData)       // by value, 8 bytes
+0x14 int (*get_last_error_code)()
+0x18 const char* (*get_last_error)()
```

`tss_sdk_get_comm_interf` starts from field +0x0c for `proc_` and uses
vtable slots +0/+4 of this table for `init`/`init_plugins`.

## 8. External boundaries (declared, not implemented here)

- `ZEN_Trace_LogMsg::debug_infoex`, `debug_errorex` (variadic).
- `ZEN_Time_Value` ctor/dtor/`operator=`; `Comm_Time_Provider::instance`;
  `Comm_Stat_Monitor::instance`;
  `ZEN_Server_Status::increase_by_statid(unsigned int, unsigned int,
  unsigned int, long long)` and `check_overtime(time_t)`.
- `CThreadErrorInfo` ctor/dtor, `last_error_info`, `get_last_error_msg`.
- `TssSdkPluginMgr` ctor/dtor and `register_plugin`.
- `Singleton<TssSdkPluginMgr>::instance()` (emitted weak in this object,
  matching the shipped symbol).
- `tss_get_sdk`.
- `tss_sdk_impl::{tss_sdk_anti_init, tss_sdk_captcha_init,
  tss_sdk_safemode_init, tss_sdk_uic_init}` and all twelve table callbacks;
  `tss_sdk_impl::send_anti_data_to_client`.
- `g_is_old_sdk` (global data, size 4, `0x0029793c`).
- `AntibotInterface` ctor/dtor and the 18 virtual methods (bodies belong to
  `antibotinterface.cpp`; `adapter/antibotinterface_ctor_agent.cpp` and
  `antibotinterface_trivial.cpp` cover the ctor/dtor).

## 9. Compile result

Compiler: c5root GCC 4.1.2.

```sh
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_exports_timer_agent.cpp \
  -o /home/loyieking/tsswork/tss_sdk_exports_timer_agent.o
```

Result: **exit 0, no diagnostics**.

`-Wall -Wextra` adds only one intentional warning:
`IAntibotInterface has virtual functions but non-virtual destructor`; the
shipped interface really has an 18-slot pure-virtual vtable (`0x00292d20`)
with no destructor slot, so the warning is faithful.

Object symbol sizes (original -> reconstruction):

```
TssSdkTimerHandler C1/C2      0x42 -> 0x42
TssSdkTimerHandler D1/D2      0x15 -> 0x15
check_monitor                 0xb4 -> 0xb4
update_time                   0x64 -> 0x64
ITssSdkPlugin D0/D1/D2  0x42/0x31/0x31 -> same
ITssSdkPlugin get_last_error  0xd0 -> 0xd0
register_callback             0x0e -> 0x0e
get_id                        0x0b -> 0x0b
get_last_error_code           0x28 -> 0x28
GetAntiBotInterfaceAll        0x2c -> 0x2c
tss_sdk_get_anti_interf       0x6c -> 0x6c
tss_sdk_get_comm_interf       0xe9 -> 0xe9
tss_sdk_get_captcha_interf    0x45 -> 0x45
tss_sdk_get_safemode_interf   0x45 -> 0x45
tss_sdk_get_uic_interf        0x45 -> 0x45
Singleton<TssSdkPluginMgr>::instance  0x96 -> 0x96
Singleton object              56 -> 56
```

Only `ITssSdkPlugin::ITssSdkPlugin` differs: 0xe9 original vs 0x7e
reconstructed.  The shipped ctor inlined the singleton guard; the rebuilt
ctor calls the same out-of-line `Singleton<TssSdkPluginMgr>::instance()`
(which is itself byte-size-identical to the shipped 0x96-byte function).  The
guard/object symbols, construction sequence and `register_plugin` target are
unchanged.

The object's undefined symbols reproduce the original call targets exactly:
`tss_sdk_*_init`, the twelve table callbacks, `send_anti_data_to_client`,
`tss_get_sdk`, `g_is_old_sdk`, `Comm_Time_Provider::instance`,
`Comm_Stat_Monitor::instance`, `ZEN_Server_Status::increase_by_statid` and
`check_overtime`, `ZEN_Time_Value` ctor/dtor/operator=, `ZEN_Trace_LogMsg`
loggers, `CThreadErrorInfo` methods, `TssSdkPluginMgr` ctor/dtor and
`register_plugin`, and `AntibotInterface` ctor/dtor.

## 10. Unresolved / partial models

- `IAntibotInterface` and `AntibotInterface` method bodies and the 18 virtual
  implementations are not reconstructed here; only the exact class/vtable
  shape and the `&g_antibot_if` return are modeled.  The object definition
  intentionally references the external `AntibotInterface::C1`/`D1`.
- `TssSdkPluginMgr` bodies are not reconstructed; the class is modeled with
  the exact 56-byte member layout so `Singleton`'s static object storage and
  guard sizes match the shipped `0x0029f658`/`0x0029f680` symbols.
- Callback parameter structures for the anti/captcha/safemode/uic
  `*InitInfo` slots are incomplete (sizes recorded, fields not recovered).
  Table callback parameter structures are forward declarations using the
  exact names from the shipped mangled symbols.
- Static initialization is merged into one
  `__static_initialization_and_destruction_0` because eight original TUs are
  reconstructed in one file.  The shipped image has separate initializers:
  `0x89100` (antibot: construct `g_antibot_if`, then set the antibot-local
  `g_tss_sdk`), `0x8b550` (comm: set `g_tss_sdk`), `0x97790` (plugin
  manager singleton) and `0x9b400` (timer TU's ZEN status/Ios statics).
  The merged initializer sets the comm `g_tss_sdk` before constructing
  `g_antibot_if`; `AntibotInterface::C1` does not read `g_tss_sdk`, so there
  is no observable ordering dependency.  The timer TU's 16
  `ZEN_STATUS_ITEM_WITHNAME` globals and `std::ios_base::Init` are logging
  infrastructure unrelated to the assigned functions and are omitted.
- `TssSdkInterface` is a partial model of `tss_get_sdk::tss_sdk`: the exact
  7-pointer field order/types are modeled; the shipped table's initializer
  bodies are in their own TUs.
