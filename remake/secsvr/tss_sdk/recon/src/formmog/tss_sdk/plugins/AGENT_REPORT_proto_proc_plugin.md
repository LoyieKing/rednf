# TssSdkProtoProcPlugin reconstruction

Target: `libtss_sdk.so.1.0.394` (ELF32 i386, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
TU: `src/formmog/tss_sdk/plugins/proto_proc/proto_proc_plugin.cpp`.
Output: `recon/src/formmog/tss_sdk/plugins/proto_proc_plugin_agent.cpp`.

## Address ranges

| Method | Address | End |
|---|---|---|
| `fini()` | `0x000c1150` | `0x000c1157` |
| `handle_notify_pkg(NotifyPkg*)` | `0x000c1160` | `0x000c116f` |
| `proc_special_antibot_pkg(u8*,u16,u32)` | `0x000c1250` | `0x000c145d` |
| `handle_decrypt_pkg_old(DpPkg*)` | `0x000c1460` | `0x000c14cc` |
| `recv_buffer_for_encrypt(DownPkg*)` | `0x000c14d0` | `0x000c1543` |
| `recv_buffer_for_encrypt_old(DownPkg*)` | `0x000c1550` | `0x000c15bc` |
| `is_need_encrypt(DownListPkg*)` | `0x000c15c0` | `0x000c161b` |
| `is_need_decrypt(DecryptListPkg*)` | `0x000c1620` | `0x000c167c` |
| ctor C1 / C2 | `0x000c1680` / `0x000c17b0` | `0x000c16f0` / `0x000c1820` |
| `get_obj_TssSdkProtoProcPlugin()` | `0x000c1710` | `0x000c17a5` |
| `proc_forward_data_to_client_pkg(void*)` | `0x000c1e00` | `0x000c216b` |
| `proc(void*)` | `0x000c2170` | `0x000c2237` |
| `is_cheating_pkg(AntiCheatingPkg*)` | `0x000c2240` | `0x000c2582` |
| `handle_antibot_pkg(u32,u8 const*,u32)` | `0x000c2590` | `0x000c3538` |
| `handle_decrypt_pkg(DpPkg*)` | `0x000c3540` | `0x000c3715` |
| `notify(void*)` | `0x000c3720` | `0x000c3c0f` |
| `init_dpsdk()` | `0x000c3c10` | `0x000c42ef` |
| `init(void*)` | `0x000c42f0` | `0x000c43af` |
| dtor D0 / D1 | `0x000c43b0` / `0x000c43f0` | `0x000c43ee` / `0x000c4420` |

## Layouts

`TssSdkProtoProcPlugin`, sizeof **32** (object emitted from the reconstruction:
32 bytes):

```
+0x00 ITssSdkPlugin (vptr, callback_proc_, error_info_, sdk_plugin_id_)
+0x10 unsigned int m_channel_id
+0x14 TssSdkCommSvc* m_sdk_comm_svc
+0x18 unsigned int m_recv_buf_seq        (unused in this TU)
+0x1c unsigned int m_notify_svr_seq      (unused in this TU)
static DPInterface* m_dp_if              0x002c22a0 (BSS, defined by this TU)
```

`UserInfo` = 400 bytes; offsets used: `uin_ +0`, `game_usr_id_ +0x14`,
`user_stat_.antibot_last_seq_ +0xcc`, `send_to_secsvr_seq_ +0xd0`,
`last_report_time_ +0xd4`.
`DPConfigHead` = 280: `game_id +8`, `world_id +0xc`, `now_time +276`.
`Comm_App_Frame` = 50 packed: `frame_length_ 0`, `frame_option_ 4`,
`frame_command_ 8`, `frame_uin_ 0xc`, `send_service_ 0x10`,
`recv_service_ 0x16`, `proxy_service_ 0x1c`, `transaction_id_ 0x22`,
`backfill_trans_id_ 0x26`, `app_id_ 0x2a`, `send_serial_number_ 0x2e`,
`frame_appdata_ 0x32`. `SERVICES_ID` = 6 packed.

Wire packets (packed, exact):

```
SendToSDK            channel 0, room 2, time 4, to_busi 8, from_busi 10,
                     data_len 12, data[32767] 14           (32781)
RecvDataFromSDKReq   channel 0, room 2, time 4, to_busi 8, from_busi 10,
                     to_secsvr_seq 12, data_len 16, data[32767] 18 (32785)
ReportLostAntibotPkg channel 0, room 2, last_seq 4, now_seq 8 (12)
ReportPkgTime        channel 0, room 2, last_report 4, now_report 8,
                     now_cmd 12                            (16)
DPInterDataHead      packet_size 0(u16), protocol_type 2(u16), data_type 4 (5)
AntibotClientPkg     head{cmd_id 0(u16), seq 2(u32)}, body[38541] at 6 (38547)
```

`TssSdkPluginProtoProcPkg` = 36 bytes: `msg_id_ +0`, union body `+4`.
Union member offsets: decrypt list 16, down 32, notify 16, anti-cheating 24,
antibot 12, dp 28, down list 16.

## Decoded behavior

- `fini`: no-op, 0. `handle_notify_pkg`: `pkg->notify_result_ = 0`, 0.
- `init`: `comm = Singleton<TssSdkCommSvc>::instance()`;
  `m_sdk_comm_svc = comm`; `m_channel_id = comm->channel_id_`;
  static `m_dp_if = comm->dp_if_`; return `init_dpsdk()` mapped to 0/-1
  (`rc == 0 ? 0 : -1`).
- `init_dpsdk`: decimal `m_channel_id` via `ostringstream`, then
  `m_dp_if->init(name.c_str())`; on nonzero logs
  `"in TssSdkProtoProcPlugin::init_dpsdk, m_dp_if->init failed"` and returns
  -1, else 0.
- `proc`: null -> 0. Commands `0x493e5`, `0x493e7`, `0x493ea`: look up
  `get_user_info_by_uin(frame_uin_)`; call `m_dp_if->on_recv_msg(user,
  frame)`, or log `"in RSP_TSSSDK_DP_*, user is not exist, uin = %u"`.
  Command `0x493e1`: `proc_forward_data_to_client_pkg(frame)`. Everything else
  logs `"%s, invalid cmd %u"` with the ORIG `__PRETTY_FUNCTION__`. Always
  returns 0.
- `proc_forward_data_to_client_pkg`:
  1. `appdata_decode<SendToSDK>` fail -> `"unpack forward data to client pkg
     failed. uin %u|appid %u."`, code **0x494b3**.
  2. `to_busi_id_ == 2` -> lookup and `on_recv_msg` (failure only logged).
     `to_busi_id_ == 1 || == 10` -> forward only. Anything else logs
     `"[%s], invalid busi id %u, cmd %u, uin %u|appid %u."` and returns 0.
  3. If `comm->is_need_trans_uin(head->game_id)`, replace the outgoing uin
     with `UserInfo::game_usr_id_`; lookup failure logs and returns **-1**.
  4. `proc_special_antibot_pkg(pkg.data_, pkg.data_len_, 0x7fff)` (return
     ignored), then `send_buf_to_client(0, uin, pkg.data_, (short)len)`.
     Transport failure -> `"In CTssSdkProtoProcPlugin::proc, proc send msg to
     client pkg failed. uin %u|appid %u."`, code **0x4963c**.
  5. Success logs `uin, frame_command_, frame_length_, app_id_,
     send_service(type,id)`; returns 0.
- `proc_special_antibot_pkg`: reads big-endian u16 command at data[0];
  accepts only 0x78/0x79 (else returns 0). `AntibotClientPkg::unpack(data,
  data_len)`; failure logs `"[%s], unpack antibot pkg fail, ret=%d, cmd=%d"`
  and returns the unpack code. For 0x78 logs the two `TAB_SC_DELAY_CHECK`
  uint64s; for 0x79 writes `sdk_up_time_ = now.total_msec()` and logs
  `secsvr/sdk down`, `client_up`, `sdk_up`. Then `pack(data, buf_len)`,
  returning the pack code on failure.
- `is_cheating_pkg`: user lookup; on non-null user logs
  `"is cheating pkg uin=%u,now_time=%u"`. If
  `(unsigned)(head->now_time - last_report_time_) > 59`, builds
  `ReportPkgTime {channel_id_, world_id, last_report, now, cmd_id_}`,
  writes `last_report_time_ = now`, sends an IPC frame
  `init_framehead(65536,0,0x4bafd)` with rcv 0x1f / send 0x2b / frame_uin /
  app_id=head->game_id, then always logs
  `"send report pkg time fail, rc=%d, uin=%u"` (even on rc 0). Finally
  `pkg->is_cheating_pkg_ = m_dp_if->anticheck_pkg(user, cmd_id_, game_pkg,
  game_pkg_len)`; returns 0.
- `handle_decrypt_pkg`: `DPInterDataHead::unpack(input_buf,
  input_buf_size)`. Failure sets `decrypt_result_ = -1`; if the unpack code
  is negative it logs `"in TssSdkProtoProcPlugin::handle_dp_pkg,
  head.unpack failed, uin = %u"`, sets `"In CTssSdkProtoProcPlugin::
  handle_dp_pkg, invalid dp pkg."`, code **0x49645**, and returns it.
  `data_type_ == 1` -> `handle_antibot_pkg(uin, input_buf + used_len,
  input_size - used_len)`, `decrypt_result_ = 1`, clears error code and
  returns 0. Otherwise logs `"recv dp pkg. cmd=%d uin=%u"` with
  `protocol_type_` and calls `handle_decrypt_pkg_old`.
- `handle_decrypt_pkg_old`: `get_user_info(&uin_, idx_)` then
  `recv_buffer_for_decrypt(user, input, in_len, output, output_size&)`;
  stores the result in `decrypt_result_`, returns 0.
- `recv_buffer_for_encrypt`: lookup then
  `recv_buffer_for_encrypt(user, cmd_id_, input, in_len, output,
  output_size&)`; stores `encrypt_result_`, returns 0. The `_old` variant
  omits `cmd_id_`.
- `is_need_encrypt`: lookup then `is_in_down_list_ =
  m_dp_if->is_need_encrypt(user, cmd_id_) ? 1 : 0`; 0.
- `is_need_decrypt`: lookup then `check_result_ = result ? 0 : -1`
  (matches `cmp al,1; sbb eax,eax`); 0.
- `handle_antibot_pkg`:
  1. user lookup; missing -> logs `"[%s], user %u is not exist"`, returns 0.
  2. `input_buf_size > 0x7fff` -> logs
     `"recv antibot msg too long. recv_len=%u max_size=%u"`, returns -1.
  3. Fills `RecvDataFromSDKReq`: channel=this, room=head->world_id,
     timestamp=time(0), to/from busi=1, `to_secsvr_seq =
     ++user->send_to_secsvr_seq_`; copies the input into `data_`.
  4. `proc_special_antibot_pkg(data_, data_len_, 0x7fff)`.
  5. **ORIG quirk**: it then sets `antibot_last_seq_ = last_seq + 1` and
     computes `lost_num = (last_seq + 1) - last_seq - 1`, i.e. always 0;
     the loss-report path is therefore unreachable from this code. The
     reconstruction preserves this exactly (including the bill/stat code).
  6. Sends IPC frame `init_framehead(65536,0,0x4c6ab)` with rcv 0x1f,
     send 0x2b, `frame_uin_ = uin`, `app_id_ = head->game_id`.
     Encode failure -> 0x139f; transport failure -> log and -2. Non-zero
     result builds `"In CTssSdkProtoProcPlugin::handle_antibot_pkg,
     send_ipc_pkg failed, rc is " + rc + "." + comm->get_last_error()`,
     code **0x4964b**, returns it.
  7. If the (dead) loss flag were set, sends `ReportLostAntibotPkg
     {channel, world_id, last_seq, last_seq+1}` in frame `0x4bafb`; same
     error code handling.
- `notify` routing on `head_.msg_id_`:
  - 201 anti-cheating; 202 encrypt; 203 decrypt (failure -> message
    `"In CTssSdkProtoProcPlugin::notify, handle_decrypt_pkg failed."`,
    code **0x49645**); 304 decrypt list; 305 down list; 307 encrypt old;
    308 decrypt old; 309 antibot (failure -> message
    `"In CTssSdkProtoProcPlugin::notify, Handle antibot pkg failed."`,
    code **0x49644**).
  - null -> `"In CTssSdkProtoProcPlugin::notify, Invalid args."`, code
    **0x49639**.
  - default -> `"In CTssSdkProtoProcPlugin::notify, unknown pkg."`, code
    **0x49649**.
- Ctor: base id=3, zeroes all four members, logs
  `"In CTssSdkProtoProcPlugin ctor."`. Dtor: base only, no cleanup.
- `get_obj_*`: function-local static (guard/atexit), as shipped.

## Error codes

`0x494b3` forward decode; `0x4963c` send-to-client failure; `0x49639`
notify null; `0x49644` antibot notify failure; `0x49645` decrypt/DP failure;
`0x49649` unknown notify packet; `0x4964b` antibot send failure.

## Call targets / external boundaries

`ITssSdkPlugin`, `UserInfo`, `TssSdkCommSvc`, `DPInterface`,
`Comm_App_Frame`, `SERVICES_ID`, `Singleton<TssSdkCommSvc>`,
`CThreadErrorInfo`, `ZEN_Trace_LogMsg`, `ZEN_Time_Value`,
`tss_dp_comm::DP_CONFIG::glb_dp_config_head_`, `sec_proto` packet
pack/unpack, `tss_dpsdk::DPBillAdapter` (+ its static `bill_`),
`CommBillRecord::operator<<(long long)`, `Comm_Stat_Monitor::instance()`,
`ZEN_Server_Status::increase_by_statid`, `Crc32(unsigned char*, unsigned int)`.

## Compile

c5root GCC 4.1.2 with `-m32 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector -std=gnu++98`, `-I recon`, `-nostdinc` and the c5root
4.1.1 headers / `-B` cc1plus path. Result: **exit 0, no diagnostics**.
`-fdump-class-hierarchy` confirms plugin 32, `UserInfo` 400,
`SendToSDK` 32781, `RecvDataFromSDKReq` 32785, `AntibotClientPkg` 38547,
`DPInterDataHead` 5, `Comm_App_Frame` 50, `SERVICES_ID` 6. Weak
`Comm_App_Frame::appdata_encode<ReportPkgTime/RecvDataFromSDKReq/
ReportLostAntibotPkg>` and `appdata_decode<SendToSDK>` are emitted.

## Unresolved / omitted

- `DPInterface`, `TssSdkCommSvc`, `DPBillAdapter`, `UserInfo` internals,
  the DP config objects and the bill/stat services are partial externs.
- TU static-initialization wrappers/status tables are compiler-generated and
  not hand-written; `get_obj`'s guard/atexit is regenerated by the toolchain.
- `g_obj_TssSdkProtoProcPlugin` (BSS 0x0029f648) is not defined here.
