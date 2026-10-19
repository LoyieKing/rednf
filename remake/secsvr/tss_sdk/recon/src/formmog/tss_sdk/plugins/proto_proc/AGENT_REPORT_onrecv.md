# DPInterface::on_recv_msg — reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
ELF32 i386, GNU C++ 4.1 ABI, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Source: `dpinterface_onrecv_agent.cpp` (this directory).

No shared header, README, CMake, test or existing source was modified.
The shared `dpinterface.h` is included unmodified. No commit was made.

## Symbol and address range

| item | value |
|---|---|
| demangled | `DPInterface::on_recv_msg(tss_sdk_impl::UserInfo*, void*)` |
| mangled (exact) | `_ZN11DPInterface11on_recv_msgEPN12tss_sdk_impl8UserInfoEPv` |
| DWARF `low_pc` | `0x000c04f0` |
| DWARF `high_pc` | `0x000c09c2` |
| next function | `__static_initialization_and_destruction_0` at `0x000c09d0` |
| padding | `0x000c09c2 .. 0x000c09cf` |

The ticket range end `0x000c0dc0` is the end of the following
static-initialization function (`0x000c09d0 .. 0x000c0db4`), not of
`on_recv_msg`. `on_recv_msg` itself ends at `0x000c09c2`; the last instruction
is the `mov`/`lea` alignment padding at `0x000c09c2`/`0x000c09c9`.

The reconstructed object defines exactly this strong symbol:

```text
00000000 T _ZN11DPInterface11on_recv_msgEPN12tss_sdk_impl8UserInfoEPv
```

No identity claim is made for the whole shared object.

## Signature

From DWARF (`dpinterface.cpp:448`) and the mangled name:

```cpp
int DPInterface::on_recv_msg(tss_sdk_impl::UserInfo *user, void *proc_data);
```

`DPInterface` layout used (shared `dpinterface.h`, confirmed by the code):
`is_inited_` +0x00, `msg_handler_` +0x04, `hack_manager_` +0x08.
`msg_handler_` is not touched by this function.

`tss_sdk_impl::UserInfo` fields read:
`base_info_.uin_` +0x00, `base_info_.flags_` +0x20.

## Decoded control flow

### Entry (`0x0c04f0 .. 0x0c0543`, source lines 448–450, 536)

```text
if (user == 0 || proc_data == 0 || !is_inited_ ||
    !tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
    return -1;
```

The fourth test is at `0x0c0538`: `mov eax,[ebx-0x188]` loads the GOT slot
for `_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E` (`0x002943ac` ->
`0x002bb4c0`), then dereferences the `DPConfig*` and tests byte `[+0]`
(`DPConfigBase::is_use_dp`). It is **not** a `g_tss_sdk_enable` check.

Branch `0x0c0524` is the shared `return -1` path (`mov esi,0xffffffff`).

### Billing dump (`0x0c0545 .. 0x0c054c`, `0x0c07c4 .. 0x0c07e2`; lines 458–461)

```text
if (user->base_info_.flags_ & 1)
    tss_dpsdk::DPBillAdapter::dump(
        "on recv msg", user->base_info_.uin_,
        (const unsigned char *)app_frame, app_frame->frame_length_);
```

`app_frame = (const Comm_App_Frame *)proc_data` (line 457). `Comm_App_Frame`
fields: `frame_length_` +0x00, `frame_command_` +0x08.

### Command dispatch (`0x0c0552 .. 0x0c0569`; line 464)

`cmd = app_frame->frame_command_`; if `cmd - 0x493e1 <= 0xb`, an indirect jump
through the table at `0x0021d964` (EBX base `0x00294534`) is taken. Otherwise
the default branch at `0x0c056b` logs and returns 0.

| command | table target | handler | source lines |
|---|---|---|---|
| `0x493e1` (300001) | `0x0c05a5` | `sec_proto::SendToSDK` / `on_verify_response` | 519–530 |
| `0x493e2`,`e3`,`e4` | `0x0c056b` | default | 536 |
| `0x493e5` (300005) | `0x0c060e` | `sec_proto::DpDropCmdlistReq` / `on_drop_cmd` | 482–488 |
| `0x493e6` | `0x0c056b` | default | 536 |
| `0x493e7` (300007) | `0x0c067d` | `sec_proto::DpBanModeReq` / `on_ban_mode` | 469–476 |
| `0x493e8`,`e9` | `0x0c056b` | default | 536 |
| `0x493ea` (300010) | `0x0c06ec` | `sec_proto::DPCaptchaResultRsp` / `on_verify_msg` | 507–513 |
| `0x493eb` | `0x0c056b` | default | 536 |
| `0x493ec` (300012) | `0x0c075b` | `sec_proto::SafemodeStatusRsp` / `on_safe_mode` | 495–501 |

Decoded jump-table words: `ff e2c071, ffe2c037, ffe2c037, ffe2c037,
ffe2c0da, ffe2c037, ffe2c149, ffe2c037, ffe2c037, ffe2c1b8, ffe2c037,
ffe2c227`, each added to EBX.

The symbolic command names were not present in DWARF or in the generated
headers. The source uses the raw values with comments; the packet type for
each value is established only by the decode template call in that branch.
Command 300001 is named in the source as the `forward_pkg` branch because the
local variable is `forward_pkg`; its payload type is `sec_proto::SendToSDK`
and it is forwarded only when `to_busi_id_ == 2`.

### Per-branch flow (`res` declared line 456, initialized 0)

All five branches follow:

```text
res = app_frame->appdata_decode(pkg, 0, 0);   // data_start=0, sz_code=0
if (res != 0) {
    ZEN_Trace_LogMsg::debug_output(RS_ERROR, "<branch> failed,len=%u",
        user ? user->base_info_.uin_ : 0u, __func__,
        app_frame->frame_length_);
    break;
}
res = <inlined DPHackMgr method>(user, ...);
break;
```

The calls are `appdata_decode<T>(T &, unsigned int data_start, size_t *sz_code)
const`. The third argument is the null `size_t*` (`Pj` in the 32-bit ABI).

Exact failure strings and addresses:

| branch | string address | string |
|---|---|---|
| `DpBanModeReq` | `0x0021d794` | `[%u][%s], unpack dp ban mode pkg failed, len=%u` |
| `DpDropCmdlistReq` | `0x0021d7c4` | `[%u][%s], unpack dp drop cmdlist pkg failed, len=%u` |
| `SafemodeStatusRsp` | `0x0021d7f8` | `[%u][%s], unpack safemode status pkg failed, len=%u` |
| `DPCaptchaResultRsp` | `0x0021d82c` | `[%u][%s], unpack dp catpcha verify pkg failed,len=%u` |
| `SendToSDK` | `0x0021d864` | `[%u][%s], unpack forward data failed,len=%u` |
| default | `0x0021d94c` | `[%u][%s], unknow cmd=%d` |
| bill tag | `0x0021d940` | `on recv msg` |
| `__func__` | `0x0021d994` | `on_recv_msg` |

All log calls use priority `RS_ERROR` (4).

Branch-specific argument details:

- `DpBanModeReq` (`0x0c067d..0x0c06e7`): `res = decode(pkg)`; success calls
  `on_ban_mode(user, pkg)`.
- `DpDropCmdlistReq` (`0x0c060e..0x0c0678`): success calls
  `on_drop_cmd(user, pkg)`.
- `SafemodeStatusRsp` (`0x0c075b..0x0c07bf`): success reads `pkg.stat_level_`
  (byte at decoded +0x06, zero-extended) and calls
  `on_safe_mode(user, (int)pkg.stat_level_)`.
- `DPCaptchaResultRsp` (`0x0c06ec..0x0c0756`): success calls
  `on_verify_msg(user, pkg)`.
- `SendToSDK` (`0x0c05a5..0x0c0609`, `0x0c07e7..0x0c085c`): success tests
  `forward_pkg.to_busi_id_ == 2` (word at decoded +0x08). If false, returns
  0. If true, calls
  `on_verify_response(user, forward_pkg.data_, forward_pkg.data_len_)`
  (`data_` at decoded +0x0e, `data_len_` word at +0x0c).
- default (`0x0c056b..0x0c05a3`): logs `unknow cmd` and returns 0.

`on_recv_msg` does **not** call `DPMsgHandler`. The packet handling is done by
five inlined `DPHackMgr` loops over `IDPHackerEventNotify` vectors.

### Return values

| path | value |
|---|---|
| null user / null proc_data / `!is_inited_` / `!is_use_dp` | `-1` |
| unknown command | `0` |
| decode failure | decode's non-zero return value |
| decode success plus handler | handler's return value; the inlined loop returns 0 when the vector is empty, otherwise the first non-zero result (early return) |
| SendToSDK decode success but `to_busi_id_ != 2` | `0` |

## Inlined DPHackMgr / IDPHacker event dispatch

The binary contains no out-of-line calls to
`DPHackMgr::on_ban_mode/on_drop_cmd/on_verify_response/on_verify_msg/on_safe_mode`;
the line table attributes all loop bodies to `dp_hack_mgr.h:56`. Recovered
layout and vtable order:

- `DPHackMgr` size 124: `DPHACKERS hackers_` +0x00,
  `IDPHackerEventNotify event_notify_` +0x0c.
- `IDPHackerEventNotify`: vptr +0x00, `std::vector<IDPHacker*> hackers_[9]`
  +0x04.
- `E_EVENT`: `E_LOGIN=0`, `E_LOGOUT=1`, `E_PROC=2`, `E_BAN_MODE=3`,
  `E_DROP_CMD=4`, `E_MSG2CLIENT=5`, `E_VERIFY_MSG=6`, `E_SAFE_MODE=7`,
  `E_SPECIAL_CONTROL=8`, `E_EVENT_COUNT=9`.
- `IDPHacker` vtable, address point `0x00292f68`: `init` +0x00, pure slot
  +0x04, `on_login` +0x08, `on_logout` +0x0c, `proc` +0x10,
  `on_ban_mode` +0x14, `on_drop_cmd` +0x18, `on_verify_response` +0x1c,
  `on_verify_msg` +0x20, `on_safe_mode` +0x24,
  `on_special_control` +0x28, destructors after.

Observed vector offsets and virtual slots:

| event | DPHackMgr vector offset | vtable slot | exact signature |
|---|---|---|---|
| `E_BAN_MODE` | +0x34 | +0x14 | `int on_ban_mode(UserInfo*, const sec_proto::DpBanModeReq&)` |
| `E_DROP_CMD` | +0x40 | +0x18 | `int on_drop_cmd(UserInfo*, const sec_proto::DpDropCmdlistReq&)` |
| `E_MSG2CLIENT` | +0x4c | +0x1c | `int on_verify_response(UserInfo*, unsigned char*, unsigned int)` |
| `E_VERIFY_MSG` | +0x58 | +0x20 | `int on_verify_msg(UserInfo*, const sec_proto::DPCaptchaResultRsp&)` |
| `E_SAFE_MODE` | +0x64 | +0x24 | `int on_safe_mode(UserInfo*, int)` |

Each loop is `int iRet = 0; for (i = 0; i < hackers.size(); ++i) { iRet =
hackers[i]->...; if (iRet != 0) break; } return iRet;`.

## Resolved calls (`nm -C`)

| PLT stub | real symbol | signature |
|---|---|---|
| `0x000873b4` | `0x001b3e70` | `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` |
| `0x00085504` | `0x000a2a70` | `tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int, unsigned char const*, unsigned int)` -> `int` |
| `0x00088234` | `0x000c0f90` | `int Comm_App_Frame::appdata_decode<sec_proto::DpBanModeReq>(sec_proto::DpBanModeReq&, unsigned int, unsigned int*) const` |
| `0x000830f4` | `0x000c1000` | `int Comm_App_Frame::appdata_decode<sec_proto::DpDropCmdlistReq>(sec_proto::DpDropCmdlistReq&, unsigned int, unsigned int*) const` |
| `0x00087e54` | `0x000c1070` | `int Comm_App_Frame::appdata_decode<sec_proto::SafemodeStatusRsp>(sec_proto::SafemodeStatusRsp&, unsigned int, unsigned int*) const` |
| `0x00087984` | `0x000c10e0` | `int Comm_App_Frame::appdata_decode<sec_proto::DPCaptchaResultRsp>(sec_proto::DPCaptchaResultRsp&, unsigned int, unsigned int*) const` |
| `0x00087da4` | `0x0009c9e0` | `int Comm_App_Frame::appdata_decode<sec_proto::SendToSDK>(sec_proto::SendToSDK&, unsigned int, unsigned int*) const` |

`size_t` is `unsigned int` in the 32-bit ABI, so the decode template symbol
uses `j` / `Pj`.

Other symbols referenced by the function:
`_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E` (GOT slot `0x002943ac`,
object `0x002bb4c0`).

## Packet layouts used (DWARF-verified)

Only the fields needed for size/offset are modelled; `__attribute__((packed))`
is used to reproduce the observed sizes and stack placement.

| type | size | fields read |
|---|---|---|
| `sec_proto::SendToSDK` | 32781 | `to_busi_id_` uint16 +0x08, `data_len_` uint16 +0x0c, `data_[32767]` +0x0e |
| `sec_proto::DpBanModeReq` | 264 | none; passed by const reference |
| `sec_proto::DpDropCmdlistReq` | 808 | none; passed by const reference |
| `sec_proto::SafemodeStatusRsp` | 42 | `stat_level_` uint8 +0x06 |
| `sec_proto::DPCaptchaResultRsp` | 5 | none; passed by const reference |

`Comm_App_Frame` partial model: `frame_length_` +0x00, option union +0x04,
`frame_command_` +0x08.

## Dependencies / integration boundary

The following are **partial declarations only** in this TU and must be
replaced by the real SDK headers on integration:

- `tss_dp_comm::DPConfig`, `tss_dp_comm::DPConfigBase`,
  `tss_dp_comm::DP_CONFIG`. Only `dp_base_conf.is_use_dp` is modelled;
  `glb_dp_config_` is declared, not defined.
- `Comm_App_Frame` and its `appdata_decode<T>` template. The five
  specializations are declared but **not defined**, so the object references
  the original out-of-line instantiations listed above.
- `sec_proto::SendToSDK`, `DpBanModeReq`, `DpDropCmdlistReq`,
  `SafemodeStatusRsp`, `DPCaptchaResultRsp`; only layout fields are modelled.
- `tss_dpsdk::DPBillAdapter::dump`; declaration only.
- `tss_dpsdk::IDPHacker`, `IDPHackerEventNotify`, `DPHackMgr`. The five
  `DPHackMgr` methods and the vtable slot order/layout are reproduced locally
  because the original definitions are inline in `dp_hack_mgr.h:56` and were
  inlined into this function.
- `ZEN_LOG_PRIORITY` and `ZEN_Trace_LogMsg::debug_output`; the enum is kept as
  a distinct type because it is part of the mangled name.
- The shared `dpinterface.h` declares `hack_manager_` as the namespace-less
  DWARF placeholder `::DPHackMgr*`; the reconstructed code casts it directly
  to `tss_dpsdk::DPHackMgr*` at each call site. No cast helper function is
  introduced; the repeated `reinterpret_cast` is the only integration shim.
- `g_tss_sdk_enable` is deliberately **not** referenced; the actual fourth
  guard is `tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp`.

Unresolved symbolic semantics:

- The symbolic names of command ids `300001`, `300005`, `300007`, `300010`,
  `300012` are not recoverable from the binary's DWARF/generated headers; only
  their numeric values and payload types are decoded.
- The `IDPHacker` pure virtual slot at vtable +0x04 has no recoverable name.
  It is declared as `vtable_slot_1()` and is never called here.
- `DPHackMgr::hackers_` (the +0x00 vector) is not touched by this function.

## Compile result

Closest available toolchain (c5root GCC 4.1.2, the target's compiler
generation; target lib itself is GCC 4.1.0):

```text
wsl -u root bash -lc "cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon && \
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root && \
$C5/usr/bin/g++ -m32 -O2 -DNDEBUG -pthread -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include \
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_onrecv_agent.cpp \
  -o /tmp/dpinterface_onrecv_c5.o"
```

Result: exit 0, no diagnostics.

Fallback host compile (`g++ 13.3.0 -m32 -std=gnu++98 -O2 -fPIC
-fno-implement-inlines -fno-stack-protector`) also exits 0 with no
diagnostics.

Defined symbols in the C5 object:

```text
00000000 T _ZN11DPInterface11on_recv_msgEPN12tss_sdk_impl8UserInfoEPv
00000000 T __i686.get_pc_thunk.bx
00000000 r _ZZN11DPInterface11on_recv_msgEPN12tss_sdk_impl8UserInfoEPvE8__func__
```

Undefined dependencies emitted with their exact mangled names:

```text
_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
_ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz
_ZN9tss_dpsdk13DPBillAdapter4dumpEPKcjPKhj
_ZNK14Comm_App_Frame14appdata_decodeIN9sec_proto9SendToSDKEEEiRT_jPj
_ZNK14Comm_App_Frame14appdata_decodeIN9sec_proto12DpBanModeReqEEEiRT_jPj
_ZNK14Comm_App_Frame14appdata_decodeIN9sec_proto16DpDropCmdlistReqEEEiRT_jPj
_ZNK14Comm_App_Frame14appdata_decodeIN9sec_proto17SafemodeStatusRspEEEiRT_jPj
_ZNK14Comm_App_Frame14appdata_decodeIN9sec_proto18DPCaptchaResultRspEEEiRT_jPj
```

The GCC 4.1.2 object additionally imports `__gxx_personality_v0` (and emits
the standard `DW.ref.__gxx_personality_v0`). This is a toolchain-emitted
unwind personality reference, not a source-level call and not an invented
symbol; the real SDK/runtime supplies it.

## Semantic coverage (evidenced by source + emitted object)

The emitted object was inspected against the ORIG slice for the semantic
skeleton. The following were verified present and at the ORIG offsets:

- Entry guard reads `[ebp+0xc]` (user), `[ebp+0x10]` (proc_data),
  `[this]` (`is_inited_`), and the `glb_dp_config_->dp_base_conf.is_use_dp`
  byte; all four failures converge on `mov esi,0xffffffff` / return `-1`.
- Bill dump gate `test BYTE PTR [user+0x20],1`; dump arguments are
  `("on recv msg", user->uin, app_frame, app_frame->frame_length_)` in the
  ORIG order.
- Command-range test `frame_command_ - 0x493e1 <= 0xb`; five dispatch cases
  and the unknown-command path.
- Five decode calls with `(pkg, 0, 0)` and the exact specializations:
  `SendToSDK`, `DpDropCmdlistReq`, `DpBanModeReq`, `DPCaptchaResultRsp`,
  `SafemodeStatusRsp`.
- Five failure logs at `RS_ERROR` (4), with `uin`, `__func__`, and
  `frame_length_` in the ORIG argument order and the exact ORIG strings.
- `SendToSDK`: `to_busi_id_ == 2` gate, then
  `on_verify_response(user, data_, data_len_)`; `data_len_` is a 16-bit
  zero-extended value.
- `SafemodeStatusRsp`: `stat_level_` is read as one zero-extended byte and
  passed as `int`.
- Handler dispatch uses `DPHackMgr` vector offsets +0x34, +0x40, +0x4c,
  +0x58, +0x64 and virtual slots +0x14, +0x18, +0x1c, +0x20, +0x24, exactly
  as decoded.
- Per-handler loops return 0 for an empty vector, stop on the first non-zero
  result, and preserve that result.
- Return values: `-1` on guard failure, `0` for unknown command, decode
  result on decode failure, and handler result on success; `SendToSDK` with
  `to_busi_id_ != 2` returns 0.
- No `DPMsgHandler` call and no `g_tss_sdk_enable` read exist in this
  function.

## Compiler-generation residual (not semantic)

Using c5root GCC 4.1.2, the emitted function is semantically equivalent but
not byte-identical to the GCC 4.1.0 ORIG. Observed differences are
codegen-only:

- GCC 4.1.2 allocates the five address-taken packet locals in separate stack
  regions (frame `0x84b8`, each `pkg` at a distinct address), while ORIG
  4.1.0 overlays all packet locals at `ebp-0x8019` (frame `0x8058`). A
  local union view can force the ORIG frame/address, but that changes the
  source declaration structure and was not adopted.
- GCC 4.1.2 places the PIC switch table in `.text` using a `sub`-based
  table; ORIG 4.1.0 uses a `.rodata` table plus `mov/add/jmp`.
- GCC 4.1.2 moves decode-failure blocks to the function tail; ORIG keeps
  each failure block inline after its decode call.
- Register allocation and branch polarity differ in places. These do not
  change the decoded call targets, field offsets, argument widths, or
  return values above.

These are compiler-generation differences between 4.1.0 and 4.1.2, not
semantic differences. No byte-identity claim is made. The reconstruction is
claimed semantically correct only for the statically evidenced points above;
no runtime execution against the real decode/handler implementations was
performed.

## Unproven / external behavior

- The symbolic names of command ids `300001`, `300005`, `300007`, `300010`,
  `300012` remain unknown; only numeric values and payload types are decoded.
- The bodies of the five `Comm_App_Frame::appdata_decode<T>` instantiations
  are external; this TU only proves which type is decoded in which branch,
  the `(pkg,0,0)` arguments, and the failure handling.
- The bodies and semantics of `DPHackMgr`/`IDPHacker` handlers are external;
  this TU proves the vector offsets, virtual slots, argument widths, and
  stop-on-nonzero loop behavior.
- `DPBillAdapter::dump` and `ZEN_Trace_LogMsg::debug_output` bodies are
  external.
- The `IDPHacker` pure virtual slot at vtable +0x04 has no recoverable name;
  it is never called by this function.

This TU is a partial reconstruction: helper classes/globals are local
ABI-accurate models and the decode instantiations are external dependencies.
No byte-identity or whole-`libtss_sdk` identity claim is made.
