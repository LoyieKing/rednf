# TssSdkUserControlPlugin reconstruction

Target: `libtss_sdk.so.1.0.394` (ELF32 i386, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
TU: `src/formmog/tss_sdk/plugins/user_control/user_control_plugin.cpp`
(DWARF producer path `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/`).
Output: `recon/src/formmog/tss_sdk/plugins/usercontrol_plugin_agent.cpp`.

All claims come from `nm -C -S`, DWARF layouts, `.rodata`, and disassembly of the
shipped ELF. Addresses are link-time virtual addresses.

## Address ranges

| Method | Symbol address | Body end |
|---|---|---|
| `fini()` | `0x000ca130` | `0x000ca140` |
| `check_timer(long)` | `0x000ca1f0` | `0x000ca26c` |
| `get_user_info_by_uin(GetUserInfo&)` | `0x000ca270` | `0x000ca2c4` |
| `get_user_info(GetUserInfo&)` | `0x000ca2d0` | `0x000ca33b` |
| ctor C1 / C2 | `0x000ca340` / `0x000ca4a0` | `0x000ca3f1` / `0x000ca551` |
| `get_obj_TssSdkUserControlPlugin()` | `0x000ca400` | `0x000ca495` |
| `ProcessCheckTimer(UserInfo*,void*,time_t)` | `0x000ca560` | `0x000ca590` |
| `punish_user(PunishUser&)` | `0x000ca9f0` | `0x000caca7` |
| `proc(void*)` | `0x000cacb0` | `0x000cae49` |
| `delete_user(DeleteUser&)` | `0x000cae50` | `0x000cb878` |
| `add_user(AddUser&)` | `0x000cb880` | `0x000cc3d4` |
| `notify(void*)` | `0x000cc3e0` | `0x000cc63e` |
| `init(void*)` | `0x000cc640` | `0x000cd10f` |
| dtor D0 / D1 | `0x000cd110` / `0x000cd150` | `0x000cd14e` / `0x000cd17d` |

## Layouts

`TssSdkUserControlPlugin`, sizeof **40** (object emitted from the reconstruction:
40 bytes):

```
+0x00 ITssSdkPlugin (vptr, callback_proc_, error_info_, sdk_plugin_id_)
+0x10 DPInterface* m_dp_if_
+0x14 unsigned int channel_id_
+0x18 unsigned int m_add_user_seq_       (unused in this TU)
+0x1c unsigned int m_delete_user_seq_    (unused in this TU)
+0x20 UserMgr* user_manager_
+0x24 time_t last_check_timer_
```

`ITssSdkPlugin` is 16 bytes; the ctor calls
`ITssSdkPlugin::ITssSdkPlugin(ETSS_SDK_PLUGIN_ID_USER_CONTROL = 2)`.

`UserMgr` = 40 bytes (`hash-table ptr +0`, `time_t +4`, `int +8`,
`ZEN_Thread_Mutex +0xc`, mutex 28).
`UserInfo` = 400 bytes; named offsets: `uin_ +0`, `game_id_ +4`, `room_id_ +8`,
`client_ip_ +0xc`, `server_ip_ +0x10`, `game_usr_id_ +0x14`,
`client_version_ +0x18`, `role_name_ +0x24`; `user_stat_ +0xa8`
(`antibot_last_seq_ +0xcc`, `send_to_secsvr_seq_ +0xd0`,
`last_report_time_ +0xd4`).

Plugin packet layouts (exact, packed):

```
AddUser     idx 0, uin 4, user_id 8, client_ver 12, server_ip 16,
            client_ip 20, player_name* 24                    (28 bytes)
DeleteUser  idx 0, uin 4                                     (8)
PunishUser  uin 0, game_id 4(u16), room_id 6(u16), punish_mode 8,
            punish_time 12, punish_info_len 16, punish_info* 20,
            punish_reason_len 24, punish_reason* 28          (32)
GetUserInfo uin 0, idx 4, UserInfo* 8                        (12)
Pkg         head msg_id 0, union body 4                      (36)
```

`PunishClientReq` (2068): channel 0(u16), room 2(u16), timestamp 4,
punish_type 8, punish_duration 12, punish_info_len 16, punish_info[1024] 18,
punish_reason_len 1042, punish_reason[1024] 1044.
`AddUserReq` (276): channel 0, room 2, timestamp 4, client_version 8,
client_ip 12, server_ip 16, player_name[256] 20.
`DelUserReq` (8): channel 0, room 2, timestamp 4.

## Decoded behavior

- `fini`: only `channel_id_ = 0`; returns 0. `m_dp_if_` and `user_manager_`
  are untouched.
- `check_timer(now)`:
  1. `m_dp_if_->check_timer(now)` unconditionally.
  2. null-unchecked `DP_CONFIG::glb_dp_config_`; if
     `dp_base_conf.is_use_dp == 0`, return 0.
  3. `elapsed = (unsigned)(now - last_check_timer_)`; if
     `elapsed > dp_time_confg.check_time` (DP_CONFIG +0x148), set
     `last_check_timer_ = now` and call
     `user_manager_->check(ProcessCheckTimer, m_dp_if_, now)`.
  4. return 0. `ProcessCheckTimer` forwards to
     `DPInterface::check_timer(user, now)`.
- `get_user_info_by_uin`: `user_info_ = UserMgr::get_user(uin,
  (u16)channel_id_, (u16)glb_dp_config_head_->world_id)`; return 0.
- `get_user_info`: copies `uin_`, uses `(u16)channel_id_` as game id and
  `(u16)glb_dp_config_head_->world_id` as room id; calls
  `UserMgr::get_user_info(uin&, game_id&, room_id&, idx)`; stores the returned
  pointer and the possibly updated `uin`; return 0.
- `punish_user`:
  1. `comm_svc = Singleton<TssSdkCommSvc>::instance()`.
  2. If `comm_svc->is_need_trans_uin(game_id_)`: look up
     `get_user_info_by_uin(uin_)`; on null log and return **-1**; on success
     replace `uin_` with `UserInfo::base_info_.game_usr_id_` (+0x14) and log.
  3. Log the full punish packet.
  4. `rc = callback_proc_(&pkg)`; on non-zero log, set message
     `"In TssSdkUserControlPlugin::proc, punish user failed."` and code
     **0x49766**; still return 0.
- `proc`:
  1. `check_timer(time(0))`; null `proc_data` -> 0.
  2. `frame_command_ != 0x493e3` -> `snprintf(... "recv invalid cmd %u,
     expect %u.")`, error code **0x497ca**, return it.
  3. `appdata_decode<PunishClientReq>(pkg, 0, 0)` failure -> message
     `"unpack punish client pkg failed. uin %u|appid %u."`, code **0x494b3**.
  4. Builds `PunishUser` with `uin = frame_uin_`, `game_id = app_id_ & 0xffff`,
     `room_id = PunishClientReq::room_id_`, punish fields 1:1, then calls
     `punish_user` and returns its rc.
- `notify`: null -> logs, message
  `"In CTssSdkUserControlPlugin::notify, notify_data is NULL."`, code
  **0x49765**. Message-id switch 1..5 to add/delete/punish/get/get_by_uin.
  Default -> logs, message `"Unknown pkg."`, code **0x49767**.
- `delete_user`:
  1. `pkg.idx_ = -1`; lookup by uin. Missing user only logs
     `"delete user failed. rc=%d, uin=%u."` with rc 0; it still sends IPC.
  2. `m_dp_if_->del_user(user)`; negative rc is logged and ignored.
  3. `user_manager_->del_user(user)`: rc 0 -> log `"delete user ok. idx=%d."`
     and set `idx_ = 0`; non-zero -> log `"delete user failed. rc=%d."`,
     `idx_` stays -1.
  4. Always builds `DelUserReq {channel_id_, world_id, time(0)}` and IPC frame
     `init_framehead(65536, 0, 0x4baf3)`, services rcv `0x1f`, proxy `0`,
     send `0x2b`, `frame_uin_ = pkg.uin_`, `app_id_ = head->game_id`.
  5. `appdata_encode<DelUserReq>(65469, req, 0, 0)` then
     `send_buf_to_backend(frame, frame_length_, true, 0)`.
     Encode failure substitutes `0x139f`; transport failure is logged and
     substitutes -2. Either non-zero result builds
     `"In CTssSdkUserControlPlugin::delete_user, send IPC pkg failed. " +
     comm_svc->get_last_error()`, sets code **0x49768**, returns it.
- `add_user`:
  1. `pkg.idx_ = -1`; `obj_id = user_manager_->add_user(uin, channel_id,
     world_id)`; if `obj_id > 0` set `idx_`, else log and keep -1.
  2. `user = comm_svc->get_user_info(pkg.uin_, pkg.idx_)`.
     Null -> log `"get_user_info failed. uin=%u, obj_id=%d."`, set idx -1,
     and still call `m_dp_if_->add_user(NULL)`.
     Non-null -> fill `game_id_, room_id_, game_usr_id_ (=user_id_),
     client_version_, client_ip_, server_ip_`; `strncpy(role_name_,
     player_name_, 0x3f)` only when player_name is non-null; log ok.
  3. `m_dp_if_->add_user(user)`; non-zero only logged.
  4. Builds `AddUserReq` and IPC frame `init_framehead(65536, 0, 0x4baf1)`
     with same services/uin/app_id rules. Player name: null -> empty;
     `len = strlen`; if `len > 255` clamp to 256; `strncpy` len bytes and
     write a terminator at `player_name_[len]` (for the clamped case this is
     one byte past the 256-byte field, as in ORIG).
  5. Encode/send as delete; failure -> same prefix with
     `"add_user, send IPC pkg failed. "`, code **0x49768**.
- `init`:
  1. `comm_svc = Singleton<TssSdkCommSvc>::instance()`.
  2. `m_dp_if_ = comm_svc->dp_if_`; `channel_id_ = comm_svc->channel_id_`.
  3. Builds decimal channel-id string from `channel_id_`.
  4. `UserMgr::init(ipc_name.c_str(),
     g_config.usercontrol_config_.max_client_obj_count_ (g_config+0x4d2),
     g_config.usercontrol_config_.use_idx_ (g_config+0x4ce))`.
  5. Failure logs `"UserMgr init fail, res=%d, ipc_name=%s,
     max_client_count=%d"` and returns -1; success returns 0.
- Ctor sets `m_dp_if_/channel_id_/seqs/last_check_timer_ = 0`, logs
  `"In CTssSdkUserControlPlugin ctor."`, then `new UserMgr()`.
- Dtor sets vptr and destroys the base only; `user_manager_` is **leaked**
  exactly as in ORIG.
- `get_obj_*` is a function-local static (guard + atexit), matching the shipped
  `_ZZN...s_obj...` and `_ZGVZN...` symbols.

## Call targets / external boundaries

- `ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId)`, `~ITssSdkPlugin()`
- `UserMgr::{UserMgr, init, add_user, del_user, get_user, get_user_info, check}`
- `DPInterface::{check_timer(long), check_timer(UserInfo*,long), add_user,
  del_user}`
- `TssSdkCommSvc::{get_user_info, get_user_info_by_uin, is_need_trans_uin,
  send_buf_to_backend, get_last_error}`
- `Singleton<TssSdkCommSvc>::instance()`
- `Comm_App_Frame::{init_framehead, dumpoutput_framehead}`,
  `SERVICES_ID::{ctor,dtor}`
- `CThreadErrorInfo::{last_error_info, set_last_error_msg}`,
  `ZEN_Trace_LogMsg::{debug_debugEx, debug_errorex}`
- `tss_dp_comm::DP_CONFIG::{glb_dp_config_, glb_dp_config_head_}`
- `sec_proto::PunishClientReq::unpack`, `AddUserReq::pack`,
  `DelUserReq::pack`

## Compile

c5root GCC 4.1.2, `-m32 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector -std=gnu++98`, `-I recon`, `-nostdinc` with the c5root
4.1.1 headers and `-B<c5root>/usr/libexec/gcc/x86_64-redhat-linux/4.1.1/`.
Result: **exit 0, no diagnostics**. The object emits exactly the assigned
symbols; `-fdump-class-hierarchy` confirms the plugin object and packet sizes
above; weak `Comm_App_Frame::appdata_encode<AddUserReq/DelUserReq>` and
`appdata_decode<PunishClientReq>` instantiations are present.

## Unresolved / omitted

- `UserMgr`, `DPInterface`, `TssSdkCommSvc`, the DP config globals and the
  config object `g_config` are declared as exact partial models and left to
  their own TUs; their bodies are not duplicated.
- The compiler-generated TU static initialization (`__static_initialization_
  and_destruction_0`, `__tcf_*`, `_GLOBAL__I_*`) is not hand-written; the
  c5root compiler regenerates guards/atexit for `get_obj`'s local static.
- `g_obj_TssSdkUserControlPlugin` (BSS 0x0029f654) is not defined by this TU;
  the shipped `get_obj` returns the function-local static, and the global
  object is populated outside this TU.
