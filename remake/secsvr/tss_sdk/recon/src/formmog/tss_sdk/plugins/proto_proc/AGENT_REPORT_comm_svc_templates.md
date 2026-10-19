# TssSdkCommSvc template instantiations — reconstruction report

Source: `tss_sdk_comm_svc_templates_agent.cpp` (this directory).

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`,
ELF32 i386, SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`
(the checked copy is `build/tmp/libtss_sdk.so.1.0.394`, same hash).

Original TU: `src/formmog/tss_sdk/framework/tss_sdk_comm_svc.cpp`; the template
bodies are declared in `tss_sdk_comm_svc.h` (DWARF lines 147 and 194). Original
compiler: GCC 4.1.0 (SUSE); check compiler: c5root GCC 4.1.2.

This file is a semantic reconstruction, not a byte-identical copy. It defines
only the ten assigned weak template instantiations and their local ABI types.
No shared header, CMake file, test or existing source was modified.

## Address ranges and symbols

All ten are weak (`W`) text symbols. Sizes are `nm -S` sizes; every IPC body is
`0x222`, the socket body is `0x21c`.

| start | end | size | demangled symbol |
|---|---|---|---|
| `0x0009c7b0` | `0x0009c9d2` | `0x222` | `TssSdkCommSvc::send_ipc_pkg<sec_proto::RecvDataFromSDKReq>(RecvDataFromSDKReq const&, unsigned int, unsigned int, unsigned short)` |
| `0x000a34f0` | `0x000a3712` | `0x222` | `send_ipc_pkg<sec_proto::LogSvrCommBillRecord>` |
| `0x000b3450` | `0x000b3672` | `0x222` | `send_ipc_pkg<sec_proto::SecsvrCaptchaReq>` |
| `0x000b3960` | `0x000b3b82` | `0x222` | `send_ipc_pkg<sec_proto::ReqSecsvrForwardData>` |
| `0x000b3c20` | `0x000b3e42` | `0x222` | `send_ipc_pkg<sec_proto::ReportErrReq>` |
| `0x000c44b0` | `0x000c46d2` | `0x222` | `send_ipc_pkg<sec_proto::ReportLostAntibotPkg>` |
| `0x000c4770` | `0x000c4992` | `0x222` | `send_ipc_pkg<sec_proto::ReportPkgTime>` |
| `0x000cd210` | `0x000cd432` | `0x222` | `send_ipc_pkg<sec_proto::DelUserReq>` |
| `0x000cd4d0` | `0x000cd6f2` | `0x222` | `send_ipc_pkg<sec_proto::AddUserReq>` |
| `0x000c8180` | `0x000c839c` | `0x21c` | `send_socket_pkg<sec_proto::UserInputCheckPkg>(int, UserInputCheckPkg const&)` |

DWARF abstract-origin parameter names:

```cpp
template <class PKG>
int TssSdkCommSvc::send_ipc_pkg(PKG const& pkg, unsigned int uin,
                                unsigned int cmd, unsigned short rcv);

template <class PKG>
int TssSdkCommSvc::send_socket_pkg(int business_id, PKG const& pkg);
```

Local variables and definition lines (DWARF):

| line | variable | line | variable |
|---|---|---|---|
| 149 | `trans_send_buffer` | 196 | `trans_send_buffer` |
| 150 | `rsp_msg` | 197 | `rsp_msg` |
| 154 | `rcvsvc` | 201 | `rcvsvc` |
| 155 | `proxysvc` | 202 | `proxysvc` |
| 156 | `sndsvc` | 203 | `sndsvc` |
| 175 | `ret` | 222 | `ret` |

## Local ABI types

### `Comm_App_Frame` — sizeof 50, alignment 1

The shipped header is packed: `transaction_id_` starts at unaligned `+34`, and
`sizeof == 50` (a normal 4-byte-aligned layout would be 52). The reconstruction
uses `__attribute__((packed))`; c5root reports size 50, align 1.

| offset | field | size |
|---|---|---|
| `+0` | `unsigned int frame_length_` | 4 |
| `+4` | `union { unsigned int frame_option_; _APP_FRAME_OPTION inner_option_; }` | 4 |
| `+8` | `unsigned int frame_command_` | 4 |
| `+12` | `unsigned int frame_uin_` | 4 |
| `+16` | `SERVICES_ID send_service_` | 6 |
| `+22` | `SERVICES_ID recv_service_` | 6 |
| `+28` | `SERVICES_ID proxy_service_` | 6 |
| `+34` | `unsigned int transaction_id_` | 4 |
| `+38` | `unsigned int backfill_trans_id_` | 4 |
| `+42` | `unsigned int app_id_` | 4 |
| `+46` | `union { unsigned int send_serial_number_; unsigned int send_ip_address_; }` | 4 |
| `+50` | `frame_appdata_` (body) | variable |

`_APP_FRAME_OPTION` is a 4-byte bitfield overlay. c5root (GCC 4.1.2, the same
layout as the shipped GCC 4.1 CUs) places:

- `frame_version_ : 8` at raw bits 24..31 (low byte of the high word);
- `inner_option_ : 24` at raw bits 0..23.

`init_framehead(65536, 0, cmd)` sets `frame_option_ = 0`, then writes byte
`+7 = 1`, producing raw `frame_option_ = 0x01000000`, i.e.
`frame_version_ = TSS_APPFRAME_V1 (1)`, `inner_option_ = 0`.

The dump guard in both templates is the raw bit-16 test
`inner_option_.inner_option_ & 0x10000` (`test BYTE PTR [frame+6],1`). With
`option = 0` it is always false.

### `SERVICES_ID` — sizeof 6, packed

| offset | field |
|---|---|
| `+0` | `unsigned short services_type_` |
| `+2` | `unsigned int services_id_` |

The two fields are copied by the template as one dword plus one word; the
constructor and destructor are external symbols from the shipped library.

### `TssSdkCommSvc` — sizeof 40

Matches the class layout already established in `AGENT_REPORT_comm_svc.md`:
`channel_id_ +0`, `plugin_mgr_ +4`, `dp_if_ +8`, `sdk_channel_status_ +0xc`
(24 bytes), `error_info_ +0x24`. The templates only pass their `this` pointer to
`send_buf_to_backend`; they do not read or write any member directly.

### `tss_dp_comm::DPConfigHead` — sizeof 280

`update_time +0`, `revert +4`, `game_id +8`, `world_id +12`,
`secagent_id +16`, `dp_config_path[256] +20`, `now_time +276`. Only `game_id`
is read by `send_ipc_pkg<>`.

## Decoded behavior

Both templates execute the same sequence; the differences are the command,
the uin, the receive service and the transport selector.

### `send_ipc_pkg<PKG>(pkg, uin, cmd, rcv)`

```
unsigned char trans_send_buffer[65536];
Comm_App_Frame* rsp_msg = (Comm_App_Frame*)trans_send_buffer;   // no ctor

rsp_msg->init_framehead(65536, 0, cmd);
SERVICES_ID rcvsvc(rcv, 0);
SERVICES_ID proxysvc(0, 0);
SERVICES_ID sndsvc(0x2b, 0);

rsp_msg->frame_uin_         = uin;
rsp_msg->transaction_id_    = 0;
rsp_msg->recv_service_      = rcvsvc;
rsp_msg->proxy_service_     = proxysvc;
rsp_msg->send_service_      = sndsvc;
rsp_msg->backfill_trans_id_ = 0;
rsp_msg->app_id_ = tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

if (rsp_msg->inner_option_.inner_option_ & 0x10000)
    Comm_App_Frame::dumpoutput_framehead(
        rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO /*3*/);

int ret = rsp_msg->appdata_encode(65469, pkg, 0, 0);
if (ret != 0)
    return 0x139f;

ret = send_buf_to_backend((char const*)trans_send_buffer,
                          rsp_msg->frame_length_, true /* use_ipc */,
                          0 /* business_id */);
if (ret != 0) {
    ZEN_Trace_LogMsg::debug_errorex(
        "[%s]send to backend fail. ret=%d", __PRETTY_FUNCTION__, ret);
    return -2;
}
return 0;
```

`init_framehead` also zeroes the three service ids, `frame_uin_`,
`transaction_id_`, `backfill_trans_id_`, `app_id_` and
`send_serial_number_`/`send_ip_address_`; the explicit stores above are the
shipped source's redundant assignments and are preserved.

`glb_dp_config_head_` is read without a null check; the shipped code
dereferences it unconditionally. That upstream behavior is preserved.

### `send_socket_pkg<PKG>(business_id, pkg)`

Same shape with:

- `init_framehead(65536, 0, 0)` — no frame command;
- all three services constructed as `{0,0}`, `{0,0}`, and `{0x2b,0}`;
- `frame_uin_ = 0`, `transaction_id_ = 0`, `backfill_trans_id_ = 0`,
  `app_id_ = 0`;
- `send_buf_to_backend(..., frame_length_, false /* use_ipc */, business_id)`.

The static dump guard, dump tag, `appdata_encode(65469, pkg, 0, 0)`, encode
failure code and transport failure log are identical to the IPC variant.

## Body packing

`Comm_App_Frame::appdata_encode<PKG>` is a weak out-of-line instantiation per
packet (all 10 are present, size `0x86`):

| packet | `appdata_encode` |
|---|---|
| `RecvDataFromSDKReq` | `0x0009c720` |
| `LogSvrCommBillRecord` | `0x000a3460` |
| `SecsvrCaptchaReq` | `0x000b33c0` |
| `ReqSecsvrForwardData` | `0x000b38d0` |
| `ReportErrReq` | `0x000b3b90` |
| `ReportLostAntibotPkg` | `0x000c4420` |
| `ReportPkgTime` | `0x000c46e0` |
| `DelUserReq` | `0x000cd180` |
| `AddUserReq` | `0x000cd440` |
| `UserInputCheckPkg` | `0x000c80f0` |

Its DWARF parameter names are `szframe_appdata`, `info`, `data_start`,
`sz_code`; observed body:

```
use_len = 0;
ret = info.pack((char*)this + data_start + 50,
                szframe_appdata - data_start,
                &use_len, 0);
if (ret != 0) return 0x13a0;
if (sz_code) *sz_code = use_len;
frame_length_ = data_start + use_len + 50;
return 0;
```

Both templates pass `szframe_appdata = 65469`
(`Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA`), `data_start = 0`,
`sz_code = NULL`. Therefore the protocol body is written by the packet's own
TDR `pack()` at frame offset `+50`, with a maximum body length of 65469 bytes.
The body's internal field layout is owned by the generated `sec_proto::pack`
implementation and is not fabricated in this reconstruction. On packet-pack
failure `appdata_encode` returns `0x13a0`; the template replaces it with
`0x139f`.

## Frame wire layout

The raw frame is sent as-is; the template performs no byte swapping and no
copy. Only `frame_length_` bytes of the 65536-byte stack buffer are handed to
`send_buf_to_backend`.

```
+0    u32  frame_length_            50 + body length after encode
+4    u32  frame_option_            init: 0x01000000 (version 1, inner 0)
+8    u32  frame_command_           IPC: cmd; socket: 0
+12   u32  frame_uin_               IPC: uin; socket: 0
+16   u16  send_service_.type       0x2b
+18   u32  send_service_.id         0
+22   u16  recv_service_.type       IPC: rcv; socket: 0
+24   u32  recv_service_.id         0
+28   u16  proxy_service_.type      0
+30   u32  proxy_service_.id        0
+34   u32  transaction_id_          0
+38   u32  backfill_trans_id_       0
+42   u32  app_id_                  IPC: glb_dp_config_head_->game_id; socket: 0
+46   u32  send_serial_number_ / send_ip_address_   0
+50   ...  TDR-packed PKG body (max 65469)
```

`SERVICES_ID` is packed: `u16 type` immediately followed by an unaligned
`u32 id`; total 6 bytes per service. All multi-byte fields are stored in host
(i386 little-endian) order by this layer.

## Channel selection and counters

`send_buf_to_backend` (reconstructed in `tss_sdk_comm_svc_agent.cpp`,
`0x0008fbe0`) implements the channel decision:

| template | `use_ipc` | channel package |
|---|---|---|
| `send_ipc_pkg` | `true` | `type_id = 1` (IPC); `buf_`, `buf_size_` (`unsigned int`) |
| `send_socket_pkg` | `false` | `type_id = 2` (socket); `business_id_`, `buf_`, `buf_size_` (`short`, low 16 bits of the frame length) |

Send counters are owned by `send_buf_to_backend`, not by the templates:

- `total_send_` (`this+0x18`) increments after the channel plugin's `notify`;
- `succ_send_` (`this+0x1c`) increments on `notify == 0`;
- `fail_send_` (`this+0x20`) increments on any non-zero notify result,
  including the notify-timeout sentinel `0x4957b`.

No counter is incremented for an encode failure: the template returns `0x139f`
before entering `send_buf_to_backend`.

## Return codes and logs

| condition | value | notes |
|---|---|---|
| success | `0` | returned when `send_buf_to_backend` returns 0 |
| `appdata_encode` non-zero | `0x139f` (5023) | packet-pack failure; `appdata_encode` itself returned `0x13a0` |
| `send_buf_to_backend` non-zero | `-2` (`0xfffffffe`) | after logging |
| notify timeout inside `send_buf_to_backend` | `0` from `send_buf_to_backend` | the timeout path logs and returns 0, so the template also returns 0 |

The transport-failure log is exactly:

```
"[%s]send to backend fail. ret=%d"   // .rodata 0x21a568
```

with `__PRETTY_FUNCTION__` of the current instantiation as `%s` and the
non-zero return code as `%d`. The debug-dump tag is exactly
`"[TRACK MONITOR][TRANS SEND]"` (`.rodata 0x21a4dc`) with priority
`RS_INFO = 3`.

Per-instantiation `__PRETTY_FUNCTION__` locations:

| instantiation | address |
|---|---|
| `RecvDataFromSDKReq` | `0x0021a6c0` (duplicate `0x0021e260`) |
| `LogSvrCommBillRecord` | `0x0021aec0` |
| `SecsvrCaptchaReq` | `0x0021c6c0` |
| `ReqSecsvrForwardData` | `0x0021c7e0` (duplicates `0x21cac0`, `0x21cd40`, `0x21cec0`) |
| `ReportErrReq` | `0x0021c8a0` |
| `ReportLostAntibotPkg` | `0x0021e300` |
| `ReportPkgTime` | `0x0021e3a0` |
| `DelUserReq` | `0x0021fd60` |
| `AddUserReq` | `0x0021fe00` |
| `UserInputCheckPkg` | `0x0021ee60` |

## External dependencies (all left undefined in this object)

| symbol | address | role |
|---|---|---|
| `Comm_App_Frame::init_framehead(unsigned,unsigned,unsigned)` | `0x000cf910` | initializes the 50-byte header |
| `Comm_App_Frame::appdata_encode<PKG>(unsigned,PKG const&,unsigned,unsigned*)` | table above | TDR body pack + length |
| `Comm_App_Frame::dumpoutput_framehead(Comm_App_Frame const*,char const*,ZEN_LOG_PRIORITY)` | `0x000d0d50` | static wrapper; forwards to const member `0x000d06b0` |
| `SERVICES_ID::SERVICES_ID(unsigned short,unsigned int)` | `0x000d70e0` (C1), `0x000d70c0` (C2) | service id init |
| `SERVICES_ID::~SERVICES_ID()` | `0x000d7100` (D1), `0x000d7110` (D2) | cleanup |
| `TssSdkCommSvc::send_buf_to_backend(char const*,unsigned int,bool,int)` | `0x0008fbe0` | channel notify + counters |
| `ZEN_Trace_LogMsg::debug_errorex(char const*,...)` | `0x001b3d30` | error log |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_head_` | `0x002bb4c4` | pointer to `DPConfigHead`; `game_id` at `+8` |

The wrapper `dumpoutput_framehead(Comm_App_Frame const*, char const*, enum)` is
a **static** member: DWARF declares three formal parameters and no artificial
`this`, and the call site passes exactly three stack arguments. Its body loads
`[ebp+8]` (frame) as the inner `this`, `[ebp+0xc]` as the tag and `[ebp+0x10]`
as the priority, then calls the const overload. Declaring it non-static would
add a fourth argument and change the wire-visible dump behavior; the
reconstruction declares it static.

## Verification and compile result

Built with c5root GCC 4.1.2 using the same flags as the other recon units:

```sh
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include \
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_templates_agent.cpp \
  -o /tmp/tss_sdk_comm_svc_templates_c5.o
```

Result: **exit 0, no diagnostics, no warnings**.

Defined symbols (`nm -C -S --defined-only`); the ten assigned functions are
weak and byte-for-byte sized like the originals (`0x222`/`0x21c`):

```
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::RecvDataFromSDKReq>(...)      0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::LogSvrCommBillRecord>(...)    0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::SecsvrCaptchaReq>(...)        0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReqSecsvrForwardData>(...)    0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportErrReq>(...)            0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportLostAntibotPkg>(...)    0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportPkgTime>(...)           0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::DelUserReq>(...)              0x222
W int TssSdkCommSvc::send_ipc_pkg<sec_proto::AddUserReq>(...)              0x222
W int TssSdkCommSvc::send_socket_pkg<sec_proto::UserInputCheckPkg>(...)    0x21c
```

Plus one per-instantiation `__PRETTY_FUNCTION__` rodata object, the local
`.LC0`/`.LC1` strings, `DW.ref.__gxx_personality_v0`, and the compiler's
`__i686.get_pc_thunk.bx`.

Additional structural checks from `-fdump-class-hierarchy` on this object:
`Comm_App_Frame` 50 align 1, `_APP_FRAME_OPTION` 4, `SERVICES_ID` 6 align 1,
`DPConfigHead` 280, `TssSdkChannelStatus` 24, `TssSdkCommSvc` 40.

Normalized disassembly comparison: for all ten instantiations, the shipped
instruction stream and the reconstruction's stream have the same instruction
count and differ only in the sign/encoding of PC-relative GOT displacements
(link-layout dependent). The frame field stores, service construction order,
call arguments, return values and clean-up paths are identical. Object
function sizes are exactly `0x222` for every IPC variant and `0x21c` for the
socket variant.

## Unresolved / out of scope

- No symbolic names for the fixed send service type `0x2b`, the dump-guard bit
  `0x10000`, the encode-failure code `0x139f`, the `appdata_encode` pack-failure
  code `0x13a0`, or the transport-failure return `-2` survive in DWARF. They are
  reproduced as literals with comments, not invented identifiers.
- The internal TDR layouts of the ten `sec_proto` packet bodies are outside
  these templates and are not fabricated; the body is produced by the shipped
  `appdata_encode<PKG>` / `PKG::pack`.
- `send_buf_to_backend`, the channel plugin's `notify`, and the six channel
  counters are reconstructed in `tss_sdk_comm_svc_agent.cpp`; this file only
  calls and summarizes them.
- The shipped `glb_dp_config_head_` dereference has no null check; this is
  upstream behavior, preserved.
- `__PRETTY_FUNCTION__` strings appear more than once in the library because
  the weak template body is COMDAT-deduplicated per referencing CU; the
  reconstruction emits one copy per instantiation.
