# TssSdkSafeModePlugin reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
Original TU: `src/formmog/tss_sdk/plugins/safemode/safemode_plugin.cpp`.
Emitted source: `safemode_plugin_agent.cpp`.

All findings are from the shipped symbol table, DWARF, `.rodata`, and
disassembly.  Addresses are link-time virtual addresses.

## Address ranges

| Method | Symbol address | Body range |
|---|---|---|
| `TssSdkSafeModePlugin::TssSdkSafeModePlugin()` C1 | `0x000c4a50` | `0xc4a50-0xc4ab0` |
| `TssSdkSafeModePlugin::TssSdkSafeModePlugin()` C2 | `0x000c4b70` | `0xc4b70-0xc4bd0` |
| `get_obj_TssSdkSafeModePlugin()` | `0x000c4ad0` | `0xc4ad0-0xc4b66` |
| `TssSdkSafeModePlugin::fini()` | `0x000c49a0` | `0xc49a0-0xc49a6` |
| `TssSdkSafeModePlugin::init(void*)` | `0x000c5040` | `0xc5040-0xc512f` |
| `TssSdkSafeModePlugin::notify(void*)` | `0x000c5130` | `0xc5130-0xc63a4` |
| `TssSdkSafeModePlugin::proc(void*)` | `0x000c63b0` | `0xc63b0-0xc6e24` |
| `TssSdkSafeModePlugin::~TssSdkSafeModePlugin()` D0 | `0x000c6e30` | `0xc6e30-0xc6e6e` |
| `TssSdkSafeModePlugin::~TssSdkSafeModePlugin()` D1 | `0x000c6e70` | `0xc6e70-0xc6e9d` |

The destructor is inline/implicit in ORIG (weak D0/D1, no D2); the
reconstruction defines it in-class and emits the same weak pair.

## Class layout (DWARF, `sizeof == 24`)

```
+0x00 vptr
+0x04 tss_sdk_if::TssSdkPluginCallbackProc callback_proc_
+0x08 CThreadErrorInfo error_info_
+0x0c tss_sdk_if::ETssSdkPluginId sdk_plugin_id_  (SAFEMODE = 4)
+0x10 DPInterface *dp_if_
+0x14 unsigned int channel_id_
```

Ctor installs id 4, zeroes +0x10/+0x14, installs the derived vtable, and logs
`"In CTssSdkSafeModePlugin ctor."`.  `get_obj_TssSdkSafeModePlugin()`
returns the guarded function-local static.  Vtable slot order is D1, D0,
`init`, `fini`, `proc`, `notify`.

## init / fini

`init(void*)` ignores its argument and calls
`Singleton<TssSdkCommSvc>::instance()` twice (ORIG inlines two guarded
singleton blocks):

```
dp_if_      = instance->dp_if_        // TssSdkCommSvc +0x08
channel_id_ = instance->channel_id_   // TssSdkCommSvc +0x00
return 0;
```

There is no null check and no error path.  `fini()` returns 0.

## Plugin package (DWARF anonymous type, `sizeof == 90`)

```
SafeModeHead (6):
  +0 unsigned short msg_type_
  +2 unsigned int   uin_

SafeModeBody union (84):
  +0 safemode_update_  : ipc_days u16 +0, safemode_level s8 +2,
                         has_verify_mb s8 +3, event_type u16 +4,
                         reserve s8[32] +6
  +0 safemode_ack_     : same 38-byte layout
  +0 limit_status_     : ret int +0, uin u32 +4, limit_status int +8,
                         unlock_url_len u32 +12, unlock_url char* +16
  +0 append_param_     : url char* +0, url_buf_max_len u32 +4,
                         param_count u32 +8, param SafeModeParam* +12
  +0 mba_safe_status_  : uin u32 +0, status u32 +4
  +0 mba_verify_info_  : uin u32 +0, seq u32 +4, verify_url_len s16 +8,
                         verify_url char* +10 (packed, 14)
  +0 mba_launch_verify_: uin/seq/op_level/op_scene_len u32*3 +0..,
                         op_scene[64] +16, client_ip u32 +80
  +0 mba_verify_status_: uin/seq/result u32*

SafeModePkg (90): head +0, body +6
```

`notify()` receives this package directly (framework unwraps
`TssSdkPluginNotifyData::notify_data_`).

## notify(void*)

Null payload:

```
message "In CTssSdkSafeModePlugin::notify, proc_data is NULL."
code    0x4969d (300701)
```

`head_.msg_type_` dispatch:

- `20` (0x14) or `24` (0x18): return 0, no other access.
- other than `22` (0x16): message
  `"In CTssSdkSafeModePlugin::notify, unknown msg type."`,
  code `0x496a4` (300708), return code.
- `22`: append-parameter path below.

### Append-parameter path

1. Validate every `param_[i]`:
   `strlen(param_key_)` and `strlen(param_value_)` must each be in
   `[1, 127]` (`len >= 128 || len == 0` fails).  Failure sets
   `"In CTssSdkSafeModePlugin::notify, len >= "
   "TSS_SDK_SAFEMODE_PLUGIN_PARAM_MAX_LEN || len == 0."`,
   code `0x4969d` (300701), and returns.
2. Locals: `std::string pre_encode_buf("")`, `std::string url(append.url_)`,
   `seckey_pos = url.find("seckey", 0, 6)`, `erase_len = (size_t)-1`,
   `char scratch[0x400]`, `used = 0`.
   **`scratch` is not initialised by ORIG on the no-seckey branch**; the
   first `strstr(scratch, key)` below reads the existing stack image.  The
   reconstruction preserves this.
3. If a `seckey` parameter exists:
   - `equal_pos = url.find("=", seckey_pos, 1)`; not found =>
     `"In CTssSdkSafeModePlugin::notify, not find =."`, code `0x4969d`.
   - `amp_pos = url.find("&", equal_pos, 1)`.
   - `amp_pos == npos`:
     `url[equal_pos+1] == 0` =>
     `"In CTssSdkSafeModePlugin::notify, NULL after =."`, code `0x4969d`;
     otherwise `pre_encode_buf = url.substr(equal_pos+1, npos)` and
     `erase_len = url.size()-1`.
   - `amp_pos != npos`:
     `url[equal_pos+1] == '&'` =>
     `"In CTssSdkSafeModePlugin::notify, & after =."`, code `0x4969d`;
     otherwise the shipped code performs
     `pre_encode_buf = url.substr(equal_pos+1, amp_pos-1)` (third argument
     is literally `amp_pos - 1`, not `amp_pos - equal_pos - 1`) and
     `erase_len = amp_pos`.
   - `base64_decode(pre_encode_buf.data(), pre_encode_buf.size(),
     scratch, &decoded_len=0x400)`; non-zero =>
     `"In CTssSdkSafeModePlugin::notify, base64_decode error."`,
     code `0x496a3` (300707).
   - `TenTean((unsigned char*)"sadffsdadfefffe", false, scratch,
     decoded_len)`; then append `'&'` and NUL; `used = decoded_len+1`.
4. For every parameter (re-reading `param_count_` each iteration):
   skip when `strstr(scratch, param_key_) != NULL`; otherwise append
   `param_key_`, `'='`, `base64_encode(param_value_, strlen(value),
   value_b64[0x80], &value_b64_len=0x80)`, then `'&'`, NUL.  Encode failure:
   `"In CTssSdkSafeModePlugin::notify, encoded_value_buf base64_encode
   error."`, code `0x496a1` (300705).
5. If `scratch[used-1] == '&'`, decrement `used` and NUL-terminate.
6. `debug_debugEx("pre_encode_buf after add param: %s\n", scratch)`.
7. `TenTean(key, true, scratch, used)`; then
   `base64_encode(scratch, used, url_b64[0x400], &url_b64_len=0x400)`.
   Failure: `"In CTssSdkSafeModePlugin::notify, base64_encode error."`,
   code `0x496a1`.
8. If an old `seckey` exists:
   `erase_pos = (url.at(seckey_pos-1) == '&') ? seckey_pos-1 : seckey_pos`;
   `url.erase(erase_pos, erase_len)`.  `std::string::at`/`erase` throw
   `out_of_range` with the shipped `basic_string::at`/`basic_string::erase`
   literals.
9. Append `"&"`, `"seckey"`, `"="`, then `url_b64` with `strlen`.
10. If `url.size() >= append.url_buf_max_len_`:
    `"In CTssSdkSafeModePlugin::notify, pkg->url_buf_max_len_ error."`,
    code `0x496a2` (300706).
11. `memcpy(append.url_, url.data(), url.size()); append.url_[size] = 0;`
    return 0.

## proc(void*)

Argument is `Comm_App_Frame*`.  Null payload:

```
message "In CTssSdkSafeModePlugin::proc, proc_data is NULL."
code    0x4969d (300701)
```

### Frame command `0x493ec` (300012) — SafemodeStatusRsp

1. `appdata_decode<sec_proto::SafemodeStatusRsp>(status, 0, 0)`.
   Failure logs `"unpack safemode status pkg failed,uin=%u"` with
   `frame_uin_` and returns the decoder result unchanged (no error code).
2. Build a 90-byte SafeModePkg:
   - `msg_type_ = 18`, `uin_ = frame_uin_`
   - `body_.safemode_update_`:
     `ipc_days_ = status.ipc_days_`, `safemode_level_ = status.stat_level_`,
     `has_verify_mb_ = status.has_verify_mb_`,
     `event_type_ = status.event_type_`,
     `reserve_[32] = status.reserve_[32]` (32 bytes copied).
3. Call `callback_proc_(&pkg)` unconditionally (no null check; ORIG has none).
4. `user = Singleton<TssSdkCommSvc>::instance()->get_user_info_by_uin(
   frame_uin_)`.  Null logs
   `"in RSP_TSSSDK_SAFEMODE_STATUS, user is not exist, uin = %u"` and
   returns 0.
5. `dp_if_->on_recv_msg(user, frame)`: negative result sets
   `"In CTssSdkSafeModePlugin::proc, update safe mode level failed."`,
   code `0x4969e` (300702), and returns the stored code; non-negative
   returns 0.

### Frame command `0x493ee` (300014) — SafemodeLimitStatusRsp

1. `appdata_decode<sec_proto::SafemodeLimitStatusRsp>(limit, 0, 0)`;
   failure takes the same log/return path as above.
2. Build a SafeModePkg with `msg_type_ = 21`, `uin_ = frame_uin_`,
   `body_.limit_status_` = `{limit.ret_, limit.uin_, limit.limit_status_,
   limit.unlock_url_len_, (char*)limit.unlock_url_}`; NUL-terminate
   `limit.unlock_url_[limit.unlock_url_len_]`.
3. `debug_debugEx("\n--- unlock_url_: %s ---\n", unlock_url_)`.
4. `std::string url(unlock_url_)`;
   `pos = url.find("?", 0, 1)`;
   `std::string param_str(url, pos+1, npos)` (for `npos`, `pos+1` wraps to 0,
   matching ORIG); log `"--- param_str: %s ---\n"`.
5. `base64_encode(param_str.data(), param_str.size(), param_b64[0x400],
   &param_b64_len=0x400)`.  Failure:
   `"In CTssSdkSafeModePlugin::proc, base64_encode error."`,
   code `0x496a1` (300705).
6. `new_url = url.substr(0, pos)`;
   append `"/cgi-bin/libtobacgi.so.1.0.0?req=query&"` (39 bytes),
   `"seckey"` (6), `"="` (1), `param_b64` (`strlen`).
7. If `new_url.size() >= 0x400`:
   `"In CTssSdkSafeModePlugin::proc, len >= "
   "TSS_SDK_SAFEMODE_PLUGIN_URL_MAX_LEN."`, code `0x496a2` (300706).
8. Copy `new_url` into `limit.unlock_url_`, set payload
   `unlock_url_len_ = new_url.size()`, NUL-terminate, call
   `callback_proc_(&pkg)`, return 0.

### Other commands

`debug_errorex("safemode plugin: recv unknow cmd=%d", frame_command_)` and
return 0.

## Response wire layouts used

```
sec_proto::SafemodeStatusRsp (42, packed):
  channel_id u16 +0, room_id u16 +2, ipc_days u16 +4,
  stat_level u8 +6, has_verify_mb u8 +7, event_type u16 +8,
  reserve u8[32] +10

sec_proto::SafemodeLimitStatusRsp (1038, packed):
  ret int +0, uin u32 +4, limit_status int +8,
  unlock_url_len u16 +12, unlock_url u8[1024] +14
```

## External boundaries left undefined

- `ITssSdkPlugin::{ctor,dtor}`, `CThreadErrorInfo`, `ZEN_Trace_LogMsg`
- `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()`
- `TssSdkCommSvc::get_user_info_by_uin(unsigned int)`
- `DPInterface::on_recv_msg(UserInfo*, void*)`
- `Comm_App_Frame::appdata_decode<SafemodeStatusRsp>` and
  `appdata_decode<SafemodeLimitStatusRsp>`
- `base64_encode`, `base64_decode`, `TenTean(unsigned char*, bool,
  unsigned char*, unsigned int)`
- libc/libstdc++ string/find/append/mutate/strlen/strstr/memcpy/throw
  helpers

## Compile result

Compiled with c5root GCC 4.1.2 and the exact target flags:

```
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -I recon -c safemode_plugin_agent.cpp
```

No errors and no warnings after the packed-layout workaround.  `nm` confirms
the exact shipped names for C1/C2, D0/D1, `init`, `fini`, `proc`, `notify`,
`get_obj`, vtable, typeinfo and guard/object, plus the exact undefined
`Secsvr`/`Safemode` decode and TenTean symbols.

## Unresolved / deliberately preserved ORIG behaviour

- `scratch[0x400]` is uninitialised on the no-`seckey` branch; the first
  `strstr` observes stack garbage.  This is present in ORIG and is not
  “fixed”.
- The existing-`seckey` substring uses `amp_pos - 1` as the count, and the
  erase uses `amp_pos` or `url.size()-1` as the count.  Both are odd but
  match the shipped instruction operands; they are preserved rather than
  normalised.
- `callback_proc_` is called without a null check in both `proc` paths.
- The numeric message ids 18/20/21/22/24 and the anonymous package type
  names are reconstruction labels; the values and field offsets are from the
  shipped code/DWARF.
