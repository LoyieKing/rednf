# TssSdkCaptchaPlugin reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
Original TU: `src/formmog/tss_sdk/plugins/captcha/captcha_plugin.cpp`.
Emitted source: `captcha_plugin_agent.cpp`.

Everything below comes from the shipped symbol table, DWARF, `.rodata`, and
disassembly.  Addresses are link-time virtual addresses.

## Address ranges

| Method | Symbol address | Body range |
|---|---|---|
| `TssSdkCaptchaPlugin::TssSdkCaptchaPlugin()` C1 | `0x0009b6d0` | `0x9b6d0-0x9b722` |
| `TssSdkCaptchaPlugin::TssSdkCaptchaPlugin()` C2 | `0x0009b7e0` | `0x9b7e0-0x9b832` |
| `get_obj_TssSdkCaptchaPlugin()` | `0x0009b740` | `0x9b740-0x9b7d6` |
| `TssSdkCaptchaPlugin::fini()` | `0x0009b620` | `0x9b620-0x9b626` |
| `TssSdkCaptchaPlugin::proc(void*)` | `0x0009b890` | `0x9b890-0x9bc15` |
| `TssSdkCaptchaPlugin::init(void*)` | `0x0009c030` | `0x9c030-0x9c1ad` |
| `TssSdkCaptchaPlugin::notify(void*)` | `0x0009c1b0` | `0x9c1b0-0x9c6a4` |
| `TssSdkCaptchaPlugin::~TssSdkCaptchaPlugin()` D0 | `0x0009c6b0` | `0x9c6b0-0x9c6ee` |
| `TssSdkCaptchaPlugin::~TssSdkCaptchaPlugin()` D1 | `0x0009c6f0` | `0x9c6f0-0x9c71d` |

The destructor is inline/implicit in ORIG (weak D0/D1, no D2); the
reconstruction defines it in-class, producing the same weak pair.

## Class layout (DWARF, `sizeof == 20`)

```
+0x00  vptr
+0x04  tss_sdk_if::TssSdkPluginCallbackProc callback_proc_
+0x08  CThreadErrorInfo error_info_        (4-byte pthread key)
+0x0c  tss_sdk_if::ETssSdkPluginId sdk_plugin_id_   (CAPTCHA = 5)
+0x10  TssSdkCommSvc *sdk_comm_svc_
```

The constructor deliberately does **not** initialise `sdk_comm_svc_`; only
`init()` writes +0x10.  The shipped plugin object is a function-local static
in BSS, so the field starts as zero.

Shipped vtable (`0x00292dc0`) slot order: D1, D0, `init`, `fini`, `proc`,
`notify`; the reconstruction emits the same order.

## ctor / dtor / get_obj

- Both ctor variants call
  `ITssSdkPlugin::ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA)`
  (external symbol, id `5`), install the derived vtable, then
  `ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkCaptchaPlugin ctor.")`.
- `get_obj_TssSdkCaptchaPlugin()` initialises the function-local static
  `s_obj_TssSdkCaptchaPlugin` behind the shipped guard variable and returns
  its address.  Guard/object symbol names match ORIG.
- D1 sets the derived vptr then calls `ITssSdkPlugin::~ITssSdkPlugin()`;
  D0 additionally calls `operator delete`.

## init / fini

`init(void*)` ignores its argument and calls
`Singleton<TssSdkCommSvc>::instance()` (external weak symbol), storing the
result at `this+0x10`.  If the pointer is null it writes:

```
message: "In CTssSdkCaptchaPlugin::init, invalid args."
code:    0x4950d (300301)
```

and returns the code stored in `last_error_info()->error_code`.  Otherwise
returns 0.  `fini()` returns 0 unconditionally.

## proc(void*)

Argument is `Comm_App_Frame*`.

1. `data == NULL`: message `In CTssSdkCaptchaPlugin::proc, invalid args.`,
   code `0x4950d` (300301), return the stored code.
2. `frame->frame_command_ != 0x493e1` (300001):
   `debug_errorex("[%s], invalid cmd=%d, uin %u|appid %u", __PRETTY_FUNCTION__,
   frame_command_, frame_uin_, app_id_)`; return `0x497ca` (301002).
   The function name string is ORIG's 0x0021a760
   `virtual int tss_sdk_impl::TssSdkCaptchaPlugin::proc(void*)`.
3. `appdata_decode<sec_proto::SendToSDK>(send, 0, 0)` on a local 32781-byte
   `SendToSDK`.  Failure logs
   `unpack send to sdk pkg failed,uin=%u` with `frame_uin_` and returns the
   decode result unchanged.
4. `TSSCaptchaPkg::unpack(send.data_, send.data_len_, 0, 0)` into a local
   4641-byte package.  Failure sets
   `"Captcha pkg unpack failed."` + code `0x4950e` (300302) and returns it.
5. `callback_proc_` is checked explicitly.  Null sets
   `"TssSdkCaptchaPlugin::proc, m_callback_proc is NULL."` + code `0x4950d`
   (300301).  Non-null calls `callback_proc_(&captcha_pkg)` and returns 0.

`memcpy`/`strlen`/socket calls are absent; the only protocol boundary is the
type-driven `appdata_decode<>` and `TSSCaptchaPkg::unpack`.

## notify(void*)

Argument is `sec_proto::TSSCaptchaPkg*` (the framework has already unwrapped
`TssSdkPluginNotifyData::notify_data_`).

1. `data == NULL`: `CThreadErrorInfo::set_last_error_msg(
   "invalid args. notify_data=%p", 0)`, code `0x4950d` (300301), return it.
2. Packs the package into a 65536-byte `Comm_App_Frame` buffer followed by a
   32785-byte `RecvDataFromSDKReq`; the package bytes go directly into
   `req.data_` with max length `0x7fff` (32767).  `packed_len` starts at 0.
   Pack failure: message `"Captcha pkg pack failed."`, code `0x4950f`
   (300303), return it.
3. Builds the backend request:

```
req.channel_id_  = (uint16)sdk_comm_svc_->channel_id_     (+0x10 -> +0x00)
req.room_id_     = (uint16)glb_dp_config_head_->world_id  (DPConfigHead +12)
req.time_stamp_  = (uint32)time(NULL)
req.to_busi_id_  = 6
req.from_busi_id_= 3
req.data_len_    = (uint16)packed_len
```

`req.to_secsvr_seq_` is **never written** by ORIG; the local object is handed
to `appdata_encode` with that field left as stack garbage.  The reconstruction
preserves that (no initialisation).

4. Builds `Comm_App_Frame` at the 65536-byte buffer:

```
init_framehead(0x10000, 0, 0x4c6ab)   // frame command 313003
frame_uin_          = captcha_pkg->head_.uin_
recv_service_       = SERVICES_ID(0x1f, 0)
proxy_service_      = SERVICES_ID(0, 0)
send_service_       = SERVICES_ID(0x2b, 0)
transaction_id_     = 0
backfill_trans_id_  = 0
app_id_             = glb_dp_config_head_->game_id
```

`SERVICES_ID::SERVICES_ID(unsigned short, unsigned int)` and its destructor
are real external symbols and are called exactly as in ORIG.  If
`inner_option_.inner_option_ & 0x10000` (byte +6 bit 0),
`Comm_App_Frame::dumpoutput_framehead(frame, "[TRACK MONITOR][TRANS SEND]",
RS_INFO)` is called.

5. `appdata_encode<RecvDataFromSDKReq>(0xffbd, req, 0, 0)`.
   - Encode failure sets local result `0x139f` (5023).
   - Success calls
     `sdk_comm_svc_->send_buf_to_backend((char*)frame, frame_length_, true, 0)`.
     Non-zero result logs
     `"[%s]send to backend fail. ret=%d"` with ORIG's
     `send_ipc_pkg<RecvDataFromSDKReq>` pretty-function string (0x0021a6c0)
     and sets local result `-2`.
6. If the local result is non-zero, the function calls
   `sdk_comm_svc_->get_last_error()`, passes that string's `c_str()` to
   `set_last_error_msg`, sets code `0x49511` (300305), and returns the stored
   code.  On success returns 0.

## Wire structures used

```
sec_proto::TSSCaptchaHead (16): msg_id u16 +0, uin u32 +2, busi_seq u32 +6,
                                seq u32 +10, body_len u16 +14
sec_proto::TSSCaptchaPkg  (4641): head +0, body[4625] +16
sec_proto::SendToSDK      (32781): channel u16 +0, room u16 +2, timestamp u32 +4,
                                to_busi u16 +8, from_busi u16 +10,
                                data_len u16 +12, data[32767] +14
sec_proto::RecvDataFromSDKReq (32785): channel u16 +0, room u16 +2,
                                timestamp u32 +4, to_busi u16 +8,
                                from_busi u16 +10, to_secsvr_seq u32 +12,
                                data_len u16 +16, data[32767] +18
Comm_App_Frame (50): frame_length +0, option +4, command +8, uin +12,
                     send_service +16, recv_service +22, proxy_service +28,
                     transaction +34, backfill +38, app_id +42, serial +46,
                     appdata +50
SERVICES_ID (6): services_type u16 +0, services_id u32 +2 (packed)
```

`Comm_App_Frame` is packed in ORIG.  GCC 4.1 ignores
`__attribute__((packed))` when the struct contains a non-POD
`SERVICES_ID`, so the reconstruction uses `#pragma pack(push,1)` plus a
packed 6-byte `SERVICES_ID`; the emitted offsets are the ORIG ones
(`app_id_` at +42, `frame_appdata_` at +50).

## Error codes / messages

| Code | Decimal | Where |
|---|---|---|
| `0x4950d` | 300301 | init/init-arg, proc NULL/callback NULL, notify NULL |
| `0x4950e` | 300302 | `TSSCaptchaPkg::unpack` failure |
| `0x4950f` | 300303 | notify package pack failure |
| `0x49511` | 300305 | notify appdata-encode/backend-send failure |
| `0x139f` | 5023 | internal encode-failure result, converted to `0x49511` |
| `0x497ca` | 301002 | proc unknown frame command |

## External boundaries left undefined

- `ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId)`, `~ITssSdkPlugin()`
- `CThreadErrorInfo::{last_error_info,set_last_error_msg}`
- `ZEN_Trace_LogMsg::{debug_debugEx,debug_errorex}`
- `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()`
- `TssSdkCommSvc::{send_buf_to_backend,get_last_error}`
- `Comm_App_Frame::{init_framehead,dumpoutput_framehead,appdata_encode<>,
  appdata_decode<>}`
- `sec_proto::TSSCaptchaPkg::{pack,unpack}`
- `SERVICES_ID::{ctor,dtor}`
- `tss_dp_comm::DP_CONFIG::glb_dp_config_head_`
- libc/libstdc++ (`time`, string/guard/atexit helpers)

## Compile result

Compiled with c5root GCC 4.1.2:

```
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -I recon -c captcha_plugin_agent.cpp
```

No errors.  Only GCC's ignored-`packed` warning remained before the
`#pragma pack` workaround; the final build is warning-free.  `nm` confirms
the exact shipped symbol names for C1/C2, D0/D1, `init`, `fini`, `proc`,
`notify`, `get_obj`, vtable, typeinfo and guard/object.  Intentional ORIG
quirks preserved: uninitialised `req.to_secsvr_seq_`, no callback check in
`notify`, callback check in `proc`, and the suffix-only callback-NULL error
literal.
