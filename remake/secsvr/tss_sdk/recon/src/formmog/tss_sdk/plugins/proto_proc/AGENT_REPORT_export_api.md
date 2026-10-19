# AGENT_REPORT_export_api

Reconstruction unit:
`recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_export_api_agent.cpp`

Target:
`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
ELF32 i386, GCC 4.1.x, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Status: all 16 assigned functions reconstructed and compiled with the c5root
GCC 4.1.2 toolchain at `-m32 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector -std=gnu++98`.  No commit was made.

## 1. Symbol and address inventory

| Symbol | Address | Size | Source TU |
|---|---:|---:|---|
| `tss_sdk_impl::tss_sdk_add_user(TssSdkAntiAddUserInfo*)` | `0x0008a270` | `0x125` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_del_user(TssSdkAntiDelUserInfo const*)` | `0x0008a1e0` | `0x090` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_is_cheat_pkg(TssSdkAntiGamePkgInfo const*)` | `0x0008a560` | `0x0af` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_decrypt_pkg(TssSdkAntiDecryptPkgInfo*)` | `0x0008a3a0` | `0x1bf` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_encrypt_pkg(TssSdkAntiEncryptPkgInfo*)` | `0x0008a610` | `0x198` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_anti_init(TssSdkAntiInitInfo const*)` | `0x0008a840` | `0x0b9` | `anti/tss_sdk_anti_impl.cpp` |
| `tss_sdk_impl::tss_sdk_captcha_init(TssSdkCaptchaInitInfo const*)` | `0x0008b650` | `0x065` | `captcha/tss_sdk_captcha_impl.cpp` |
| `tss_sdk_impl::tss_sdk_captcha_start(TssSdkCaptchaStartInfo const*)` | `0x0008b950` | `0x12f` | `captcha/tss_sdk_captcha_impl.cpp` |
| `tss_sdk_impl::tss_sdk_captcha_change_pic(TssSdkCaptchaChangePicInfo const*)` | `0x0008b7b0` | `0x080` | `captcha/tss_sdk_captcha_impl.cpp` |
| `tss_sdk_impl::tss_sdk_captcha_verify(TssSdkCaptchaVerifyReqInfo const*)` | `0x0008b830` | `0x118` | `captcha/tss_sdk_captcha_impl.cpp` |
| `tss_sdk_impl::tss_sdk_safemode_init(TssSdkSafemodeInitData const*)` | `0x0008bfd0` | `0x0fd` | `safemode/tss_sdk_safemode_impl.cpp` |
| `tss_sdk_impl::tss_sdk_secoptverify_append_param(TssSdkSecOptVerifyAppendParamInfo*)` | `0x0008bf30` | `0x098` | `safemode/tss_sdk_safemode_impl.cpp` |
| `tss_sdk_impl::tss_sdk_secoptverify_user_operate(TssSdkSecOptVerifyReqInfo const&)` | `0x0008c120` | `0x0ef` | `safemode/tss_sdk_safemode_impl.cpp` |
| `tss_sdk_impl::tss_sdk_uic_init(TssSdkUicInitInfo const*)` | `0x0008c7f0` | `0x07f` | `uic/tss_sdk_uic_impl.cpp` |
| `tss_sdk_impl::tss_sdk_uic_judge_input_chat(TssSdkUicChatUserInputInfo const*)` | `0x0008ca40` | `0x0c7` | `uic/tss_sdk_uic_impl.cpp` |
| `tss_sdk_impl::tss_sdk_uic_judge_input_name(TssSdkUicNameUserInputInfo*)` | `0x0008c960` | `0x0d1` | `uic/tss_sdk_uic_impl.cpp` |

Compile result: `RC=0` with
`g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector
-std=gnu++98 -I recon` (c5root GCC 4.1.2), also clean under
`-Wall -Wextra`.

Compiled object sizes (`nm -S`): 13 of 16 are byte-identical to the shipped
size; `decrypt_pkg` is `0x1bc` vs `0x1bf`, `captcha_start` is `0x12e` vs
`0x12f`, `captcha_verify` is `0x119` vs `0x118`.  Field offsets, constants,
guards, message layouts and return paths were compared instruction-by-
instruction and are semantically identical; only evaluation order and
register allocation differ in those three functions.  The unit also contains
compile-time size assertions for all public structures, callback payloads,
plugin package heads/unions and the 28-byte framework table, plus
`__builtin_offsetof` assertions for every field the reconstructed bodies
touch.

## 2. Framework boundary and file-local state

All calls go through the 28-byte `tss_get_sdk()` table at `0x00297944`
(DWARF anonymous `TssSdkInit` structure; file-local pointer symbol
`tss_sdk_impl::g_tss_sdk` in each original TU):

```
+0x00 init(unsigned int)
+0x04 init_plugins(TssSdkPluginInitData const*, unsigned int)
+0x08 fini(ETssSdkPluginId)
+0x0c proc()
+0x10 notify(TssSdkPluginNotifyData)      // struct passed by value, 8 bytes
+0x14 get_last_error_code()
+0x18 get_last_error() -> char const*
```

`TssSdkPluginInitData` is 12 bytes: `+0 plugin_id_` (int enum),
`+4 callback_proc_`, `+8 init_data_`.  `TssSdkPluginNotifyData` is 8 bytes:
`+0 plugin_id_`, `+4 notify_data_`.

Original TU-local state reconstructed in this unit:

| State | Layout | Shipped address |
|---|---|---|
| `g_tss_sdk` | `TssSdkInit*` | one per original TU (e.g. `0x0029d7e0`, `0x0029e440`, `0x0029ea40`, `0x0029f040`) |
| `g_anti_init_info_` | `TssSdkAntiInitInfo`, 8 | `0x0029d7e4` |
| `g_tss_sdk_captcha_initialized` | bool, 1 | `0x0029e444` |
| `s_tss_sdk_safemode_inited` | bool, 1 | `0x0029ea44` |
| `g_set_limitinfo` | fn ptr, 4 | `0x0029ea48` |
| `g_set_safestatus` | fn ptr, 4 | `0x0029ea4c` |
| `g_set_verifyinfo` | fn ptr, 4 | `0x0029ea50` |
| `g_set_verifyresult` | fn ptr, 4 | `0x0029ea54` |
| `g_tss_sdk_uic_initialized` | bool, 1 | `0x0029f044` |
| `g_on_judge_result` | fn ptr, 4 | `0x0029f048` |

Each original TU's static initializer calls `tss_get_sdk()` and stores the
result (e.g. captcha TU at `0x0008bdc3/0x0008bdc8`).  `tss_get_sdk()` is a
pure accessor returning the table address, so the merged unit uses one
initializer; all four pointers receive the same table.

## 3. Public argument structures (DWARF, file 439)

All `tss_sdk_if` and plugin package structures use byte packing.

```
TssSdkAntiAddUserInfo        16: +0 uin_, +4 client_ver_, +8 client_ip_,
                                  +c role_name_ (char const*)
TssSdkAntiDelUserInfo         4: +0 uin_
TssSdkAntiInitInfo            8: +0 send_data_to_client_, +4 punish_user_
TssSdkAntiDecryptPkgInfo     20: +0 uin_, +4 anti_data_,
                                  +8 anti_data_len_, +c game_pkg_buf_,
                                  +10 game_pkg_buf_len_
TssSdkAntiEncryptPkgInfo     24: +0 uin_, +4 cmd_id_, +8 game_pkg_,
                                  +c game_pkg_len_, +10 anti_data_,
                                  +14 anti_data_len_
TssSdkAntiGamePkgInfo       16: +0 uin_, +4 cmd_id_, +8 game_pkg_,
                                  +c game_pkg_len_
TssSdkCaptchaInitInfo        8: +0 on_recv_pic_data_, +4 on_verify_rsp_
TssSdkCaptchaStartInfo      27: +0 uin_, +4 req_seq_, +8 client_ip_,
                                  +c max_pic_change_num_(u16),
                                  +e max_verify_num_(u16), +10 timeout_,
                                  +14 captcha_type_(u8),
                                  +15 busi_info_len_(u16),
                                  +17 busi_info_ (u8 const*)
TssSdkCaptchaChangePicInfo   4: +0 captcha_seq_
TssSdkCaptchaVerifyReqInfo   8: +0 captcha_seq_, +4 answer_ (char const*)
TssSdkSafemodeInitData      16: +0 set_limit_info_, +4 set_safe_status_,
                                  +8 set_verify_info_, +c set_verify_result_
TssSdkSecOptVerifyAppendParamInfo 1032: +0 url_[1024],
                                  +400 param_count_, +404 param_ (8-byte
                                  {param_key_, param_value_} array)
TssSdkSecOptVerifyReqInfo   82: +0 uin_, +4 seq_, +8 op_level_,
                                  +c op_scene_len_(i16), +e op_scene_[64],
                                  +4e client_ip_
TssSdkUicInitInfo            4: +0 on_chat_judge_result_
TssSdkUicNameUserInputInfo  13: +0 msg_len_, +4 msg_,
                                  +8 if_replace_(char), +9 msg_result_flag_(i32)
TssSdkUicChatUserInputInfo  24: +0 uin_, +4 msg_len_, +8 msg_,
                                  +c callback_data_len_(u16),
                                  +e callback_data_ (u8 const*),
                                  +12 game_data_len_(u16),
                                  +14 game_data_ (u8 const*)
```

## 4. Plugin package layouts

### proto_proc (`TssSdkPluginNotifyData::plugin_id_ == 3`), sizeof 36

`head_.msg_id_` (`+0`, int) and a 32-byte union at `+4`:
`decrypt_list_pkg_` 16, `down_pkg_` 32, `notify_pkg_` 16,
`anti_cheating_pkg_` 24, `antibot_pkg_` 12, `dp_pkg_` 28,
`down_list_pkg_` 16.  Message ids: 201 `IS_CHEATING_PKG`,
202 `ENCRYPT_PKG`, 203 `DECRYPT_PKG`, 304 `IS_CMD_IN_UP_LIST`,
305 `IS_CMD_IN_DOWN_LIST`.

- `dp_pkg_`: +0 `decrypt_result_`, +4 `idx_`, +8 `uin_`, +c `input_buf_`,
  +10 `input_buf_size_`, +14 `output_buf_`, +18 `output_buf_size_`.
- `down_pkg_`: +0 `encrypt_result_`, +4 `cmd_id_`, +8 `uin_`, +c `idx_`,
  +10 `input_buf_`, +14 `input_buf_size_`, +18 `output_buf_`,
  +1c `output_buf_size_`.
- `anti_cheating_pkg_`: +0 `is_cheating_pkg_`, +4 `cmd_id_`, +8 `uin_`,
  +c `idx_`, +10 `game_pkg_`, +14 `game_pkg_len_`.

### user_control (`plugin_id_ == 2`), sizeof 36

`head_.msg_id_` (`+0`) and 32-byte union at `+4`:
`add_user_` 28, `delete_user_` 8, `punish_user_` 32, `user_info_` 12.
Message ids: 1 ADD, 2 DELETE, 3 PUNISH, 4 GET_USERINFO,
5 GET_USERINFO_BY_UIN.

- `add_user_`: +0 `idx_`, +4 `uin_`, +8 `user_id_`, +c `client_ver_`,
  +10 `server_ip_`, +14 `client_ip_`, +18 `player_name_`.
- `delete_user_`: +0 `idx_`, +4 `uin_`.

### captcha (`plugin_id_ == 5`), sizeof 4641

Head 16 bytes: +0 `msg_id_`(u16), +2 `uin_`, +6 `busi_seq_`, +a `seq_`,
+e `body_len_`(u16).  Body union at +0x10: `captcha_req_` 531,
`captcha_pic_` 4625, `verify_req_` 65, `captcha_res_` 130.

`captcha_req_`: +0 `client_ip_`, +4 `platform_busid_`,
+8 `max_pic_change_num_`, +a `max_verify_num_`, +c `timeout_`,
+10 `busi_id_`, +11 `busi_info_len_`, +13 `busi_info_[512]`.
`verify_req_`: +0 `answer_len_`, +1 `answer_[64]`.

Outgoing head msg ids: `0x0c81` start, `0x0c83` verify,
`0x0c86` change picture.  Start writes `head_.uin_` and
`head_.busi_seq_`; change picture writes only `head_.busi_seq_`;
verify writes only `head_.seq_` (shipped offset +0xa, confirmed at
`0x0008b87a`).  `head_.seq_`/`body_len_` remain uninitialised where the
shipped code leaves them so.

### safemode (`plugin_id_ == 4`), sizeof 90

Head 6 bytes: +0 `msg_type_`(u16), +2 `uin_`.  Body union at +6:
`safemode_update_` 38, `safemode_ack_` 38, `limit_status_` 20,
`append_param_` 16, `mba_safe_status_` 8, `mba_verify_info_` 14,
`mba_launch_verify_` 84, `mba_verify_status_` 12.

- `append_param_`: +0 `url_`, +4 `url_buf_max_len_`, +8 `param_count_`,
  +c `param_`.
- `mba_launch_verify_`: +0 `uin_`, +4 `seq_`, +8 `op_level_`,
  +c `op_scene_len_`, +10 `op_scene_[64]`, +50 `client_ip_`.

Outgoing head msg types: `0x16` append-parameter, `0x18`
user-operate/launch-verify.

### uic internal plugin input

`UicPluginInput` (implementation-side, not a `tss_sdk_if` public request)
is 40 bytes and **naturally aligned**, unlike the public packed request
structures: `+0 uin_`, `+4 input_type_`, `+8 msg_result_flag_`,
`+c if_replace_`, `+10 message_len_`, `+14 message_`,
`+18 callback_data_len_`, `+1c callback_data_`, `+20 game_data_len_`,
`+24 game_data_`.  The three padding bytes after `if_replace_` are
required; a packed variant changes all offsets and does not match the
shipped stores at `0x0008c9ad` / `0x0008ca84`.

## 5. Per-function semantics

### `tss_sdk_add_user` (0x8a270)
- NULL argument: `debug_errorex("tss_sdk_add_user: invalid param,
  add_user_info=%p")`, return `TSS_SDK_PROC_INVALID_ARG` (1).
- Debug log `uin=%d, cli_ver=%u, client_ip=%x, role_name=%s`.
- Builds `UserControlPkg`: msg id 1; `idx_=0`, `uin_=user_id_=
  add_user_info->uin_`, `client_ver_`, `server_ip_=0`, `client_ip_`,
  `player_name_=role_name_`.
- `g_tss_sdk->notify({plugin 2, &pkg})`.
- Returns 3 if notify fails **or** the plugin-written `idx_` is 0, with
  `"add user fail. ret=%d, uin=%u, idx=%d"`.  Otherwise logs
  `"add user successful, uin=%u, idx=%d"` and returns 0.

### `tss_sdk_del_user` (0x8a1e0)
- NULL: error log, return 1.
- Debug log `"tss_sdk_del_user:uin=%u."`.
- Builds msg id 2 package, only `delete_user_.uin_` written
  (`idx_` stays uninitialised in the shipped code as well).
- Calls `notify({plugin 2, &pkg})` and **ignores its result**; always
  returns 0.

### `tss_sdk_is_cheat_pkg` (0x8a560)
- NULL: error log, return 0.
- Builds proto_proc msg 201: `is_cheating_pkg_=0`, `cmd_id_`, `uin_`,
  `idx_=0`, `game_pkg_`, `game_pkg_len_`.
- Calls `notify({plugin 3, &pkg})` ignoring its result.
- Logs `"tss_sdk_is_cheat_pkg, uin=%u, cmd_id=%d,
  is_cheating_pkg=%d."` and returns the plugin-written
  `is_cheating_pkg_`.

### `tss_sdk_decrypt_pkg` (0x8a3a0)
- NULL: error log, return 1.
- Requires `anti_data_`, `anti_data_len_`, `game_pkg_buf_`,
  `game_pkg_buf_len_` all non-zero; otherwise logs the five-field
  `"invalid param, uin=%u, anti_data_=%p, anti_data_len_=%u,
  game_pkg_=%p, game_pkg_len_=%u"` and returns 1.
- Builds proto_proc msg 203 `dp_pkg_`: `decrypt_result_=-1`, `idx_=0`,
  `uin_`, input = anti_data/length, output = game_pkg_buf/length.
- `ret = notify(...)`.  Error log if `ret != 0` or
  `decrypt_result_ == -1`; returns 1.  If `decrypt_result_ != 0`,
  returns 1 silently.
- Success (result 0): logs output size, writes
  `game_pkg_buf_len_ = output_buf_size_`, returns 0.

### `tss_sdk_encrypt_pkg` (0x8a610)
- NULL: error log, return 1.
- Requires `anti_data_`, `anti_data_len_`, `game_pkg_`,
  `game_pkg_len_` non-zero; same five-field invalid log, return 1.
- Builds proto_proc msg 202 `down_pkg_`: `encrypt_result_=-1`,
  `cmd_id_`, `uin_`, `idx_=0`, input = game_pkg/length,
  output = anti_data/length.
- Calls `notify(...)`; **ignores notify's return value**.
- Logs uin, cmd id, output size, encrypt result; returns 1 if
  `encrypt_result_ != 0`, otherwise writes
  `anti_data_len_ = output_buf_size_` and returns 0.

### `tss_sdk_anti_init` (0x8a840)
- **No NULL guard** (the shipped function dereferences the argument).
- Logs `"tss_sdk_anti_init , punish_user=%p,
  send_data_to_client_=%p"`, then stores both callbacks in
  `g_anti_init_info_`.
- `init_plugins(plugin_array, 2)` with:
  `[0] = {PROTO_PROC(3), NULL, NULL}`,
  `[1] = {USER_CONTROL(2), punish_user_callback, NULL}`.
- rc != 0: `"tss_sdk_anti init fail. ret=%d"`, return 3.  Success:
  `"tss_sdk_anti_init successful"`, return 0.

### `tss_sdk_captcha_init` (0x8b650)
- Argument is **not read**.
- If `g_tss_sdk_captcha_initialized`, return 0.
- `init_plugins({{CAPTCHA(5), captcha_callback, NULL}}, 1)`.
- rc != 0: return 3 (no log).  Success: set flag, return 0.

### `tss_sdk_captcha_start` (0x8b950)
- No NULL guard.
- Builds captcha msg `0x0c81`; `head_.uin_ = uin_`,
  `head_.busi_seq_ = req_seq_`; body `client_ip_`,
  `platform_busid_=0`, max counters, timeout, `busi_id_ =
  captcha_type_`.
- If `busi_info_len_ > 0x200`, return 1 **without** notify.
- Else copies `busi_info_len_` bytes into `busi_info_[512]` and calls
  `notify({plugin 5, &pkg})`; failure logs
  `"notify fail. err=%d %s"` with `get_last_error()`, returns 2;
  success returns 0.

### `tss_sdk_captcha_change_pic` (0x8b7b0)
- No NULL guard.  Builds msg `0x0c86` with
  `head_.busi_seq_ = captcha_seq_`; body untouched.
- notify failure: generic notify log, return 2; success 0.

### `tss_sdk_captcha_verify` (0x8b830)
- No NULL guard on the request pointer.
- `answer_ == NULL`: logs
  `"tss_sdk_capthca_verify: bad arg. answer is empty"`, return 1.
- Writes msg `0x0c83` and `head_.seq_ = captcha_seq_` before the
  length check.
- `answer_len = (unsigned char)(strlen(answer_) + 1)`.  This preserves
  the shipped 8-bit truncation.  If `answer_len > 0x40`: logs
  `"tss_sdk_capthca_verify: bad arg. answer is too long"`, return 1.
- Copies `answer_len` bytes into `verify_req_.answer_[64]`, notifies
  plugin 5; failure -> 2, success -> 0.

### `tss_sdk_safemode_init` (0x8bfd0)
- NULL, or any of the four callbacks NULL: return 1.
- If `s_tss_sdk_safemode_inited`: return 0 (the callback NULL checks
  happen first, as shipped).
- Stores `g_set_limitinfo`, `g_set_safestatus`, `g_set_verifyinfo`,
  `g_set_verifyresult` from +0/+4/+8/+0xc.
- `init_plugins({{SAFEMODE(4), safemode_plugin_callback_proc, NULL}}, 1)`.
- rc != 0: `"init_plugins fail. err=%d %s"` with `get_last_error()`,
  return 3.  Success: set flag, return 0.

### `tss_sdk_secoptverify_append_param` (0x8bf30)
- NULL: return 1.
- Builds safemode msg `0x16`, `head_.uin_=0`;
  `append_param_.url_ = info->url_` (decays to `info`),
  `url_buf_max_len_ = sizeof(info->url_)` (0x400),
  `param_count_`, `param_`.
- notify failure: generic log, return 2; success 0.

### `tss_sdk_secoptverify_user_operate` (0x8c120)
- Reference argument; no NULL check possible/emitted.
- Builds safemode msg `0x18`; `head_.uin_ = launch_verify.uin_ =
  req.uin_`; `seq_`, `op_level_`; `op_scene_len_` sign-extended from
  the 16-bit request field into the 32-bit destination;
  `memcpy(op_scene_, req.op_scene_, 64)`; `client_ip_` from +0x4e.
- notify failure: generic log, return 2; success 0.

### `tss_sdk_uic_init` (0x8c7f0)
- If `g_tss_sdk_uic_initialized`, return 0 before checking the argument.
- NULL or NULL `on_chat_judge_result_`: return 1.
- Stores the callback in `g_on_judge_result`.
- `init_plugins({{UIC(6), plugin_callback_proc, NULL}}, 1)`.
- rc != 0: return 3 (no log).  Success: set flag, return 0.

### `tss_sdk_uic_judge_input_name` (0x8c960)
- NULL or NULL `msg_`: return 1 (no log).
- Builds 40-byte `UicPluginInput`:
  `uin_=0`, `input_type_=EUITYPE_NAME(1)`,
  `if_replace_`, `message_len_`, `message_`,
  callback/game fields zero.  `msg_result_flag_` intentionally
  uninitialised for the plugin to write.
- notify plugin 6; failure -> generic log + return 2.
- Success writes `input_info->msg_result_flag_` at **+9** from
  `plugin_input.msg_result_flag_` (+8 of the plugin input) and
  returns 0.

### `tss_sdk_uic_judge_input_chat` (0x8ca40)
- NULL or NULL `msg_`: return 1.
- Builds `UicPluginInput`: `uin_`, `input_type_=EUITYPE_CHAT(0)`,
  `if_replace_=1`, message length/pointer, callback and game
  length/pointer.  The packed request places the callback and game
  pointers at unaligned offsets +0xe and +0x14; the shipped code loads
  and stores those four-byte values exactly as the packed assignments
  do.
- notify plugin 6; failure -> generic log + return 2; success 0.

## 6. External boundaries

Declared, not redefined in this unit:

- `ZEN_Trace_LogMsg::debug_infoex/debug_debugEx/debug_errorex`
- `memcpy`, `strlen`
- `tss_get_sdk` (returns the table at `0x00297944`)
- Same-original-TU callbacks (addresses verified; **outside the assigned
  symbol list**, therefore left as extern definitions):
  - `tss_sdk_impl::punish_user_callback(void*)` — `0x0008a7b0`
  - `tss_sdk_impl::captcha_callback(void*)` — `0x0008b570`
  - `tss_sdk_impl::safemode_plugin_callback_proc(void*)` — `0x0008c210`
  - `tss_sdk_impl::plugin_callback_proc(void*)` — `0x0008c790`

No fake success, stubs, or invented call targets were introduced.  All
`init_plugins`/`notify` calls are through the shipped 28-byte interface
table; no direct absolute calls are used.

## 7. Unresolved / intentionally preserved shipped behavior

1. The four callback functions listed in section 6 are not implemented
   here.  They are required for a complete link of the original TUs and
   for the `g_anti_init_info_`/`g_set_*`/`g_on_judge_result` callbacks to
   fire.  This unit only records their exact symbols and addresses.
   Because the shipped callbacks live in the same original TUs, a later
   reconstruction should place them in this file (or expose the
   file-local state above); a separate callback file cannot see the
   `static` state as written here.
2. The four original TUs each had their own `static g_tss_sdk`.  The
   merged unit uses one pointer initialized once.  `tss_get_sdk()` has no
   side effects and always returns `0x00297944`, so behavior is
   unchanged; only the static-initializer call count differs (4 vs 1).
3. Uninitialised stack fields are deliberately reproduced:
   `del_user` leaves `delete_user_.idx_` unset; `captcha_change_pic`
   (sets only `busi_seq_`) and `captcha_verify` (sets only `seq_`) leave
   `head_.uin_`, the other sequence field and `body_len_` unset; captcha
   start sets `uin_`/`busi_seq_` but leaves `head_.seq_`/`body_len_`
   unset; `uic_judge_input_name` leaves
   `msg_result_flag` for the plugin; `captcha_verify` writes the head
   before validating `answer_len`.
4. `captcha_verify` preserves the shipped 8-bit `strlen+1` truncation
   (`cmp al,0x40`), which can accept an overlong answer after wraparound.
5. `anti_init` has no NULL guard; calling it with NULL crashes exactly
   as the shipped function does.
6. The three remaining size deltas (`decrypt_pkg`, `captcha_start`,
   `captcha_verify`) are GCC 4.1.0-vs-4.1.2 branch/register-allocation
   differences; the guard order, field offsets, log arguments and returns
   were matched to the shipped instruction stream.
7. Log format strings reproduce the shipped typos verbatim
   (`tss_sdk_capthca_verify`, spacing in `tss_sdk_anti_init ,`).
