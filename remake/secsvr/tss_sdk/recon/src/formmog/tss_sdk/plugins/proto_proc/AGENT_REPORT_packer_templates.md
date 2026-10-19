# DPMsgPacker non-core template instantiations — reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, not stripped, DWARF, GCC 4.1.x, `-O2 -fPIC`).

Source: `dp_msg_packer_templates_agent.cpp` (this directory).
Method: `nm -C -S`, per-function `objdump -d -M intel`, DWARF via pyelftools
(types, abstract/origin instances, formal names, line numbers), and
`objdump -s -j .rodata` for log strings. No runtime oracle was used.

**Verdict:** the eleven requested weak template symbols are implemented with
their ORIG mangled names and decoded observable behavior: frame/head fields,
service ids, command ids, protocol layouts, encode/send calls, log strings,
return codes and error paths. This is a semantic reconstruction, not a
byte-identical one; see §8.

## 1. Symbols and address ranges

All sizes below are ORIG `nm -S` sizes. The range is half-open.

| ORIG symbol (demangled) | ORIG range | ORIG size | this object |
|---|---|---|---|
| `DPMsgPacker::generate_comm_frame<LATransAdaptDPCmdMonitor>` | `0xb3710..0xb38c2` | `0x1b2` | `0x1b2` |
| `DPMsgPacker::generate_comm_frame<LATransAdaptDPCmdBufferMonitor>` | `0xb7ed0..0xb8082` | `0x1b2` | `0x1b2` |
| `DPMsgPacker::generate_comm_frame<ReqSafemodeTransAdaptTradeNotify>` | `0xba290..0xba442` | `0x1b2` | `0x1b2` |
| `DPMsgPacker::generate_comm_frame<ReqUICTransAdaptMsgForwardPkg>` | `0xbc620..0xbc7d2` | `0x1b2` | `0x1b2` |
| `DPMsgPacker::send_ipc_pkg<SecsvrCaptchaReq>` | `0xb3e50..0xb4171` | `0x321` | `0x13d` |
| `DPMsgPacker::send_ipc_pkg<ReportErrReq>` | `0xb4180..0xb44a1` | `0x321` | `0x13d` |
| `DPMsgPacker::send_ipc_pkg<ReqSecsvrForwardData>` | `0xb44b0..0xb47d1` | `0x321` | `0x13d` |
| `DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdMonitor>` | `0xb47e0..0xb4e02` | `0x622` | `0x17f` |
| `DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor>` | `0xb80d0..0xb86f2` | `0x622` | `0x17f` |
| `DPMsgPacker::send_datatrans_pkg<ReqSafemodeTransAdaptTradeNotify>` | `0xbac20..0xbb242` | `0x622` | `0x17f` |
| `DPMsgPacker::send_datatrans_pkg<ReqUICTransAdaptMsgForwardPkg>` | `0xbc820..0xbce42` | `0x622` | `0x17f` |

The requested address list in the ticket contained `0xbac20` for the
safemode `generate_comm_frame`; `nm` gives `0xba290` for the generator and
`0xbac20` for the matching `send_datatrans_pkg`. The table above is the
`nm` truth.

All four `generate_comm_frame` bodies are identical apart from the
`Comm_App_Frame::appdata_encode<T>` specialization. All three
`send_ipc_pkg` bodies are identical apart from the inlined
`TssSdkCommSvc::send_ipc_pkg<T>` specialization. All four
`send_datatrans_pkg` bodies are identical apart from the
`generate_comm_frame<T>` specialization.

## 2. DWARF-derived source shape

Abstract DWARF instances (decl file 438 = `dp_msg_packer.cpp`) give these
parameters and locals:

```text
generate_comm_frame(Comm_App_Frame* rsp_msg, unsigned int cmd, int entire,
                    UserInfo* user, const PKG& forward_pkg)
    locals: rcvsvc, proxysvc, sndsvc, ret

send_ipc_pkg(UserInfo* user, const PKG& pkg, unsigned int cmd)
    local: res
    inlined: Singleton<TssSdkCommSvc>::instance()->send_ipc_pkg(
                 pkg, user->uin, cmd, 0x1f)

send_datatrans_pkg(UserInfo* user, int entire, const PKG& forward_pkg,
                   unsigned int cmd)
    locals: trans_send_buffer (unsigned char[65536]), rsp_msg, ret,
            recv_from_sdk (sec_proto::ReqSecsvrForwardData)
    inlined: generate_comm_frame<PKG>(...), send_ipc_pkg<ReqSecsvrForwardData>(...)
```

The third parameter is named `entire` in DWARF (not `entry`). It is a 32-bit
`int` in the ABI; the frame service field receives `(unsigned short)entire`.

## 3. Decoded behavior

### 3.1 `Comm_App_Frame` and `init_framehead`

ORIG `Comm_App_Frame::init_framehead` is at `0xcf910`, signature
`void (unsigned int, unsigned int, unsigned int)`. The calls in these
templates are:

```text
init_framehead(0x10000 /* MAX_LEN_OF_APPFRAME */, 0, cmd)
```

Decoded writes (all confirmed in the `0xcf910` disassembly):

```text
+0x00 frame_length_        = 0x10000
+0x04 frame_option_        = option (0), then byte +0x07 = 1
                              => frame_option_ == 0x01000000
+0x08 frame_command_       = cmd
+0x0c frame_uin_           = 0
+0x10 send_service_        = SERVICES_ID(0,0)
+0x16 recv_service_        = SERVICES_ID(0,0)
+0x1c proxy_service_       = SERVICES_ID(0,0)
+0x22 transaction_id_      = 0
+0x26 backfill_trans_id_   = 0
+0x2a app_id_              = 0
+0x2e send_serial_number_  = 0
```

`SERVICES_ID` is 6 bytes: `uint16 services_type_` +0, `uint32 services_id_`
+2. It is copied fieldwise (dword + word) into the frame.

### 3.2 `generate_comm_frame<T>` @ ORIG `0xb3710` et al.

```text
frame.init_framehead(0x10000, 0, cmd)
SERVICES_ID rcvsvc((uint16)entire, 0);
SERVICES_ID proxysvc(0, 0);
SERVICES_ID sndsvc(0x1f, 0);
frame.frame_uin_        = user->base_info_.uin_;   // user+0x00
frame.recv_service_     = rcvsvc;                  // +0x16
frame.proxy_service_    = proxysvc;                // +0x1c
frame.send_service_     = sndsvc;                  // +0x10
frame.transaction_id_   = 0;                       // +0x22
frame.backfill_trans_id_= 0;                       // +0x26
frame.app_id_           = glb_dp_config_head_->game_id;  // +0x08 of DPConfigHead
if (frame.frame_option_ & 0x00010000)
    Comm_App_Frame::dumpoutput_framehead(&frame,
        "[TRACK MONITOR][TRANS SEND]", RS_INFO /*3*/);
ret = frame.appdata_encode(0xffbd /* MAX_LEN_OF_APPFRAME_DATA */,
                           forward_pkg, 0, 0);
if (ret != 0) {
    ZEN_Trace_LogMsg::debug_output(RS_ERROR,
        "[%u][%s], app data encode fail. ret=%d",
        user->uin, __func__ /* "generate_comm_frame" */, ret);
    ret = 0x139f;
}
return ret;                                        // 0 on success
```

Important detail: the dump condition in ORIG is `test BYTE PTR [frame+6],1`,
i.e. **bit 16** of `frame_option_`, not bit 0. `init_framehead` sets
`frame_option_ = 0x01000000`, so the dump is normally skipped. This
reconstruction reproduces the byte-at-+6 test.

`user` is dereferenced unconditionally (frame_uin_ and the failure log); ORIG
has no NULL check.

### 3.3 `send_ipc_pkg<T>` @ ORIG `0xb3e50` et al.

Source-level body (the callee was inlined by ORIG, see §8):

```text
res = Singleton<TssSdkCommSvc>::instance()->send_ipc_pkg(
          pkg, user->base_info_.uin_, cmd, 0x1f);
if (res == 0)
    debug_output((user->flags & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], send_ipc_pkg success, cmd=%u",
        user->uin, __func__ /* "send_ipc_pkg" */, cmd);
else
    debug_output(RS_ERROR,
        "[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u",
        user->uin, __func__, res, cmd);
return res;
```

Flags bit is `user->base_info_.flags_ & 1` at `UserInfo+0x20`. Priority is
`RS_INFO` (3) when set, `RS_DEBUG` (2) otherwise; both were verified in the
ORIG instruction sequence (`and eax,1; cmp eax,1; sbb edx,edx; add edx,3`).

The out-of-line `TssSdkCommSvc::send_ipc_pkg<T>` callee (real ORIG weak
symbols `0xb3450`/`0xb3960`/`0xb3c20`, size `0x222`) builds:

```text
unsigned char trans_send_buffer[65536];
Comm_App_Frame* rsp_msg = (Comm_App_Frame*)trans_send_buffer;
rsp_msg->init_framehead(0x10000, 0, cmd);
SERVICES_ID rcvsvc(rcv,0), proxysvc(0,0), sndsvc(0x2b,0);
rsp_msg->frame_uin_ = uin;
rsp_msg->recv_service_ = rcvsvc;    // 0x1f from DPMsgPacker
rsp_msg->proxy_service_ = proxysvc; // 0
rsp_msg->send_service_ = sndsvc;    // 0x2b
rsp_msg->transaction_id_ = 0;
rsp_msg->backfill_trans_id_ = 0;
rsp_msg->app_id_ = glb_dp_config_head_->game_id;
if (frame_option_ & 0x10000)
    dumpoutput_framehead("[TRACK MONITOR][TRANS SEND]", RS_INFO);
ret = appdata_encode(0xffbd, pkg, 0, 0);
if (ret == 0)
    ret = TssSdkCommSvc::instance()->send_buf_to_backend(
              (char*)rsp_msg, rsp_msg->frame_length_, true, 0);
else
    ret = 0x139f;
```

Return contract: `0` success, `0x139f` (5023) encode failure, `-2` backend
send failure. On backend failure the callee logs
`"[%s]send to backend fail. ret=%d"` plus its `__PRETTY_FUNCTION__`.
Command ids are supplied by the DPMsgPacker callers:

```text
SecsvrCaptchaReq      cmd = 0x4bed9  (311001)   from send_captcha_request
ReportErrReq          cmd = 0x4baf7  (310007)   from send_error_report_msg
ReqSecsvrForwardData  cmd = 0x4c6ad  (313005)   hardcoded in send_datatrans_pkg
```

### 3.4 `send_datatrans_pkg<T>` @ ORIG `0xb47e0` et al.

```text
unsigned char trans_send_buffer[65536];
Comm_App_Frame* rsp_msg = (Comm_App_Frame*)trans_send_buffer;
ret = generate_comm_frame(rsp_msg, cmd, entire, user, forward_pkg);
if (ret != 0) {
    debug_output(RS_ERROR,
        "[%u][%s], generate_comm_frame fail, ret=%d, entire=%d, cmd=%u",
        user->uin, __func__ /* "send_datatrans_pkg" */, ret, entire, cmd);
    return ret;
}
ReqSecsvrForwardData recv_from_sdk;
recv_from_sdk.channel_id_ = (uint16)TssSdkCommSvc::instance()->channel_id_;
recv_from_sdk.room_id_    = (uint16)glb_dp_config_head_->world_id; // +0x0c
memcpy(recv_from_sdk.data_, rsp_msg, rsp_msg->frame_length_);
recv_from_sdk.data_len_   = (uint16)rsp_msg->frame_length_;
return send_ipc_pkg<ReqSecsvrForwardData>(user, recv_from_sdk, 0x4c6ad);
```

The inner datatrans frame is built exactly as in §3.2 with
`recv_service_ = (uint16)entire`, `send_service_ = 0x1f`, `proxy = 0`.
The outer IPC frame (built by the inlined `send_ipc_pkg`) uses
`send_service_ = 0x2b`, `recv_service_ = 0x1f`, `proxy = 0`, and command
`0x4c6ad`. On a `generate_comm_frame` failure ORIG emits **both** logs:
the callee's `app data encode fail. ret=%d` (actual encode code) and then
the wrapper's `generate_comm_frame fail` (mapped `0x139f`); this
reconstruction preserves that order because it calls the reconstructed
generator.

The outer command (`cmd`) is caller-supplied; e.g. `send_cmd_monitor` passes
`entire = 0x23` and `cmd = 0x5573d` for `LATransAdaptDPCmdMonitor`.

ORIG quirk reproduced verbatim: `memcpy` uses the full 32-bit
`rsp_msg->frame_length_` without clamping to `sizeof(recv_from_sdk.data_)`
(32767), and `data_len_` is stored *after* the copy (so a pathological
`frame_length_ > 32767` overruns the stack `ReqSecsvrForwardData` tail before
`data_len_` is written). Normal datatrans frames are small (the protocol
payloads are 14..33044 bytes), so this path is not exercised by the four
in-tree users.

## 4. Wire layouts (DWARF sizes/offsets, verified at compile time)

All structures are `__attribute__((packed))`; offsets are byte offsets.

```text
ReportErrReq (8)
  +0  uint16 channel_id_
  +2  uint16 room_id_
  +4  uint32 error_id_

SecsvrCaptchaReq (541)
  +0   uint16 channel_id_
  +2   uint16 room_id_
  +4   uint32 version_
  +8   uint16 bus_id_
  +10  uint32 time_stamp_
  +14  uint32 client_ip_
  +18  uint16 max_pic_change_num_
  +20  uint16 max_verify_num_
  +22  uint32 timeout_
  +26  uint8  busi_id_
  +27  uint16 busi_info_len_
  +29  uint8  busi_info_[512]

ReqSecsvrForwardData (32773)
  +0      uint16 channel_id_
  +2      uint16 room_id_
  +4      uint16 data_len_
  +6      uint8  data_[32767]

LATransAdaptDPCmdMonitor (14)
  +0   uint32 room_id_
  +4   uint32 time_stamp_
  +8   int32  monitor_cmd_id_
  +12  uint8  is_need_encrypt_
  +13  uint8  is_encrypted_

LATransAdaptDPCmdBufferMonitor (32786)
  +0   uint32 room_id_
  +4   uint32 time_stamp_
  +8   int32  monitor_cmd_id_
  +12  uint8  is_need_encrypt_
  +13  uint8  is_encrypted_
  +14  uint32 data_len_          <-- u32, not u16 (2 bytes of implicit gap in u16 guess)
  +18  uint8  data_[32768]

ReqSafemodeTransAdaptTradeNotify (2073)
  +0   uint16 world_id_
  +2   uint32 src_id_
  +6   uint8  type_
  +7   uint32 send_uin_
  +11  uint32 target_uin_
  +15  uint32 send_ip_
  +19  uint32 target_ip_
  +23  uint16 buf_len_
  +25  uint8  trade_buf_[2048]

ReqUICTransAdaptMsgForwardPkg (33044)
  +0      uint16 world_id_
  +2      uint32 src_id_
  +6      uint32 cmd_id_
  +10     uint8  need_encrypt_
  +11     uint8  is_encrypted_
  +12     uint16 cmd_buf_len_
  +14     uint8  cmd_buf_[32768]
  +32782  uint16 rolename_len_
  +32784  uint8  rolename_[256]
  +33040  uint32 client_ip_
```

`Comm_App_Frame` partial model (50 bytes, packed):

```text
  +0  uint32 frame_length_
  +4  union { uint32 frame_option_; _APP_FRAME_OPTION inner_option_; }
  +8  uint32 frame_command_
  +12 uint32 frame_uin_
  +16 SERVICES_ID send_service_   (u16 +0, u32 +2; 6 bytes)
  +22 SERVICES_ID recv_service_
  +28 SERVICES_ID proxy_service_
  +34 uint32 transaction_id_
  +38 uint32 backfill_trans_id_
  +42 uint32 app_id_
  +46 union { uint32 send_serial_number_; uint32 send_ip_address_; }
  +50 uint8 frame_appdata_[0]
```

`DPConfigHead` (280): `game_id` +8, `world_id` +12.

## 5. Log strings and levels

All strings were read from ORIG `.rodata` and are embedded verbatim by the
reconstruction:

| address | string | level/use |
|---|---|---|
| `0x21a4dc` | `[TRACK MONITOR][TRANS SEND]` | dump tag, `RS_INFO` |
| `0x21c45c` | `[%u][%s], app data encode fail. ret=%d` | `RS_ERROR`, generator |
| `0x21c484` | `[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u` | `RS_ERROR` |
| `0x21c4b0` | `[%u][%s], send_ipc_pkg success, cmd=%u` | `RS_DEBUG`/`RS_INFO` |
| `0x21c5a0` | `[%u][%s], generate_comm_frame fail, ret=%d, entire=%d, cmd=%u` | `RS_ERROR` |

`__func__` strings are exactly `generate_comm_frame` (20 bytes with NUL),
`send_ipc_pkg` (13), `send_datatrans_pkg` (19), matching the ORIG local
`__func__` objects. `ZEN_LOG_PRIORITY`: `RS_DEBUG=2`, `RS_INFO=3`,
`RS_ERROR=4`.

## 6. Call targets / dependencies

Resolved ORIG targets in the call graph of these templates (direct calls and
calls inside the called helpers):

```text
Comm_App_Frame::init_framehead(unsigned,unsigned,unsigned)       0xcf910
Comm_App_Frame::dumpoutput_framehead(Comm_App_Frame const*,char const*,
                                     ZEN_LOG_PRIORITY)            0xd0d50
Comm_App_Frame::appdata_encode<LATransAdaptDPCmdMonitor>          0xb3680
Comm_App_Frame::appdata_encode<ReqSecsvrForwardData>             0xb38d0
Comm_App_Frame::appdata_encode<ReportErrReq>                     0xb3b90
Comm_App_Frame::appdata_encode<LATransAdaptDPCmdBufferMonitor>   0xb7e40
Comm_App_Frame::appdata_encode<ReqSafemodeTransAdaptTradeNotify> 0xba200
Comm_App_Frame::appdata_encode<ReqUICTransAdaptMsgForwardPkg>    0xbc590
Comm_App_Frame::appdata_encode<SecsvrCaptchaReq>                 0xb33c0
SERVICES_ID::SERVICES_ID(unsigned short,unsigned int)            0xd70c0/0xd70e0
SERVICES_ID::~SERVICES_ID()                                      0xd7100/0xd7110
SERVICES_ID::set_serviceid(unsigned short,unsigned int)          0xd7120
Singleton<TssSdkCommSvc>::instance()                             0x8af00 (weak)
  guard variable                                                 0x29d7f0
  instance()::obj (40 bytes)                                     0x29d820
TssSdkCommSvc::TssSdkCommSvc()                                   0x7f324
TssSdkCommSvc::send_buf_to_backend(char const*,unsigned,bool,int) 0x8fbe0
TssSdkCommSvc::send_ipc_pkg<SecsvrCaptchaReq>                    0xb3450 (weak)
TssSdkCommSvc::send_ipc_pkg<ReqSecsvrForwardData>                0xb3960 (weak)
TssSdkCommSvc::send_ipc_pkg<ReportErrReq>                        0xb3c20 (weak)
ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY,char const*,...)  0x1b3e70
ZEN_Trace_LogMsg::debug_errorex(char const*,...)                 0x1b3d30
tss_dp_comm::DP_CONFIG::glb_dp_config_head_                      0x2bb4c4
memcpy                                                           libc (PLT 0x84904)
```

`TssSdkCommSvc` is a 40-byte singleton object: `channel_id_` +0,
`plugin_mgr_` +4, `dp_if_` +8, `sdk_channel_status_` +12 (24 bytes),
`error_info_` +36. Only `channel_id_` is read by this slice.

The reconstruction leaves these as undefined references (no stubs, no
invented bridge): the four `appdata_encode<T>` specializations,
`init_framehead`, the static `dumpoutput_framehead`, both
`SERVICES_ID` methods, `debug_output`, `glb_dp_config_head_`, `memcpy`, the
three `TssSdkCommSvc::send_ipc_pkg<T>` instantiations and (through them)
`send_buf_to_backend`.

## 7. Compile result

```sh
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_packer_templates_agent.cpp \
  -o /tmp/packer_templates.o
```

Result: exit 0, **no diagnostics** (all DWARF size/offset checks pass, including
`SecsvrCaptchaReq` 541, `ReqSecsvrForwardData` 32773,
`LATransAdaptDPCmdBufferMonitor` 32786, `ReqSafemodeTransAdaptTradeNotify`
2073, `ReqUICTransAdaptMsgForwardPkg` 33044, `Comm_App_Frame` 50).

Defined template symbols in the object (all weak `W`, exact ORIG mangling):

```text
_ZN9tss_dpsdk11DPMsgPacker19generate_comm_frameIN9sec_proto24LATransAdaptDPCmdMonitorEEEiP14Comm_App_FramejiPN12tss_sdk_impl8UserInfoERKT_
_ZN9tss_dpsdk11DPMsgPacker19generate_comm_frameIN9sec_proto30LATransAdaptDPCmdBufferMonitorEEEiP14Comm_App_FramejiPN12tss_sdk_impl8UserInfoERKT_
_ZN9tss_dpsdk11DPMsgPacker19generate_comm_frameIN9sec_proto32ReqSafemodeTransAdaptTradeNotifyEEEiP14Comm_App_FramejiPN12tss_sdk_impl8UserInfoERKT_
_ZN9tss_dpsdk11DPMsgPacker19generate_comm_frameIN9sec_proto29ReqUICTransAdaptMsgForwardPkgEEEiP14Comm_App_FramejiPN12tss_sdk_impl8UserInfoERKT_
_ZN9tss_dpsdk11DPMsgPacker12send_ipc_pkgIN9sec_proto16SecsvrCaptchaReqEEEiPN12tss_sdk_impl8UserInfoERKT_j
_ZN9tss_dpsdk11DPMsgPacker12send_ipc_pkgIN9sec_proto12ReportErrReqEEEiPN12tss_sdk_impl8UserInfoERKT_j
_ZN9tss_dpsdk11DPMsgPacker12send_ipc_pkgIN9sec_proto20ReqSecsvrForwardDataEEEiPN12tss_sdk_impl8UserInfoERKT_j
_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto24LATransAdaptDPCmdMonitorEEEiPN12tss_sdk_impl8UserInfoEiRKT_j
_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto30LATransAdaptDPCmdBufferMonitorEEEiPN12tss_sdk_impl8UserInfoEiRKT_j
_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto32ReqSafemodeTransAdaptTradeNotifyEEEiPN12tss_sdk_impl8UserInfoEiRKT_j
_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto29ReqUICTransAdaptMsgForwardPkgEEEiPN12tss_sdk_impl8UserInfoEiRKT_j
```

Others defined in the object: `Singleton<TssSdkCommSvc>::instance()` (weak,
`0x60` bytes), its guard variable/object, `__i686.get_pc_thunk.bx`,
`__tcf_0`, and one local `__func__` array per instantiation. The object does
**not** define any additional `DPMsgPacker` specialization.

Integration check: compiling `dp_msg_packer_core_agent.cpp` with the same
flags leaves exactly three undefined references
(`send_ipc_pkg<SecsvrCaptchaReq>`, `send_ipc_pkg<ReportErrReq>`,
`send_datatrans_pkg<LATransAdaptDPCmdMonitor>`); `ld -m elf_i386 -r` of the
two objects succeeds and leaves no undefined `DPMsgPacker` template symbol.

## 8. Unresolved / deliberate divergence

1. **Inlining only.** ORIG inlined `TssSdkCommSvc::send_ipc_pkg<T>` into the
   three `DPMsgPacker::send_ipc_pkg<T>` bodies and inlined both the generator
   and the IPC send into the four `send_datatrans_pkg<T>` bodies. The
   reconstruction performs the same source-level calls to the real weak
   out-of-line instantiations. Wire bytes, service ids, commands, log
   strings/levels, return codes and `debug_errorex` pretty-function argument
   are identical; code size/inlining/exception tables are not
   (`0x13d` vs `0x321`, `0x17f` vs `0x622`). The four
   `generate_comm_frame<T>` bodies are size-identical (`0x1b2`) and
   instruction-equivalent barring store scheduling.
2. **Backend-failure pretty string.** ORIG's inlined callee logs
   `"[%s]send to backend fail. ret=%d"` using
   `TssSdkCommSvc::send_ipc_pkg<PKG>`'s `__PRETTY_FUNCTION__`. That string
   belongs to the callee, not to `DPMsgPacker`; this reconstruction does not
   duplicate it locally and obtains it from the real callee. If byte identity
   is required later, the callee must be inlined together with those exact
   per-type strings.
3. **Protocol classes are field models.** `construct/pack/unpack/visualize`
   methods of the `sec_proto` types are not reproduced; serialization is done
   by the external `Comm_App_Frame::appdata_encode<T>`.
4. **Partial frame model.** `frame_appdata_` is modeled as a zero-length tail
   at +50; no code in these symbols reads it. `_APP_FRAME_OPTION` is modeled
   only as the 4-byte union of `inner_option_`/`frame_version_`.
5. **Caller inputs.** `entire` (destination service type), the datatrans
   command, and the three IPC commands for captcha/error-report are supplied
   by the core packer callers, not by these template bodies (the sole
   hardcoded IPC command is `0x4c6ad`).
6. **No NULL-user guard.** ORIG dereferences `user` unconditionally; the
   reconstruction does too. A NULL user faults in both.
7. `TssSdkCommSvc::send_ipc_pkg<T>` must exist as an out-of-line definition
   for the three types at link time. The ORIG library provides all three;
   a standalone rebuild that omits `tss_sdk_comm_svc` will leave them
   unresolved.

## 9. Verification performed

- `nm -C -S` addresses/sizes of every target and dependency.
- Full Intel-syntax disassembly of all eleven ORIG bodies; instruction
  sequences, stack arguments, service-id construction, log argument order and
  return paths decoded.
- DWARF: structure sizes/member offsets, function signatures, formal/local
  names, abstract instances and inlined-subroutine boundaries
  (`send_ipc_pkg` inlines `TssSdkCommSvc::send_ipc_pkg`; `send_datatrans_pkg`
  inlines `generate_comm_frame` and `send_ipc_pkg<ReqSecsvrForwardData>`).
- `.rodata` dump of every format string and `__func__`.
- c5root GCC 4.1.2 compile with the requested flags: exit 0, no diagnostics;
  `nm` confirms exactly the eleven weak template symbols with ORIG mangling.
