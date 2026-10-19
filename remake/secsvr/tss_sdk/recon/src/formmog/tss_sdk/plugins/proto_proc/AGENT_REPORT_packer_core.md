# DPMsgPacker core methods — semantic reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, GCC 4.1.x, `-O2 -fPIC`, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).

Source: `dp_msg_packer_core_agent.cpp` (this directory).
Only this `.cpp` and this report were added; no shared header, README,
CMake, test or existing source was modified; no commit was made.

This is a semantic reconstruction, not a byte-identical one. Compiler
scheduling, register allocation, stack-slot choice and inlining are not
chased. All wire layouts below are byte-packed and were checked with
`__builtin_offsetof`/`sizeof` compile-time asserts in the source.

## 1. Symbols, ranges, signatures

Ranges are half-open `[nm start, start + nm size)`. The next symbol or
alignment padding is shown as the boundary.

| ORIG range | next | mangled symbol |
|---|---|---|
| `0x000b1920`–`0x000b1a9d` | `0x000b1aa0` | `_ZN9tss_dpsdk11DPMsgPacker18send_msg_to_clientEiPN12tss_sdk_impl8UserInfoEPht` |
| `0x000b1aa0`–`0x000b1b83` | `0x000b1b90` | `_ZN9tss_dpsdk11DPMsgPacker19revive_client_mouseEPN12tss_sdk_impl8UserInfoEjNS0_14E_TIPS_WINDOWSE` |
| `0x000b1b90`–`0x000b1d06` | `0x000b1d10` | `_ZN9tss_dpsdk11DPMsgPacker19send_captcha_finishEPN12tss_sdk_impl8UserInfoE` |
| `0x000b1d10`–`0x000b1dfd` | `0x000b1e00` | `_ZN9tss_dpsdk11DPMsgPacker20send_special_controlEPN12tss_sdk_impl8UserInfoEi` |
| `0x000b1e00`–`0x000b1edd` | `0x000b1ee0` | `_ZN9tss_dpsdk11DPMsgPacker21send_safe_mode_notifyEPN12tss_sdk_impl8UserInfoEi` |
| `0x000b1ee0`–`0x000b20ed` | `0x000b20f0` | `_ZN9tss_dpsdk11DPMsgPacker21send_dp_msg_to_clientEPN12tss_sdk_impl8UserInfoEPht` |
| `0x000b2500`–`0x000b2911` | `0x000b2920` | `_ZN9tss_dpsdk11DPMsgPacker20send_captcha_requestEPN12tss_sdk_impl8UserInfoEPKc` |
| `0x000b2920`–`0x000b2d0f` | `0x000b2d10` | `_ZN9tss_dpsdk11DPMsgPacker21send_error_report_msgEPN12tss_sdk_impl8UserInfoEii` |
| `0x000b2d10`–`0x000b33bc` | `0x000b33c0` | `_ZN9tss_dpsdk11DPMsgPacker16send_cmd_monitorEPN12tss_sdk_impl8UserInfoEjbb` |

DWARF declarations (CU `dp_msg_packer.cpp`, class `tss_dpsdk::DPMsgPacker`,
DWARF size 1) and the observed call sites prove these signatures and that
every method is **static** (no object pointer is passed):

```cpp
int DPMsgPacker::send_msg_to_client(int type, UserInfo*, unsigned char*, unsigned short);
int DPMsgPacker::send_dp_msg_to_client(UserInfo*, unsigned char*, unsigned short);
int DPMsgPacker::send_error_report_msg(UserInfo*, int err_id, int err_num);
int DPMsgPacker::send_cmd_monitor(UserInfo*, unsigned int cmd, bool, bool);
int DPMsgPacker::revive_client_mouse(UserInfo*, unsigned int id, E_TIPS_WINDOWS);
int DPMsgPacker::send_safe_mode_notify(UserInfo*, int level);
int DPMsgPacker::send_special_control(UserInfo*, int cmdid);
int DPMsgPacker::send_captcha_request(UserInfo*, const char* notify_data);
int DPMsgPacker::send_captcha_finish(UserInfo*);
```

`E_TIPS_WINDOWS`: `E_TIPS_NONE=0`, `E_TIPS_SAFEMODE=1`,
`E_TIPS_SEND_SUCCESS=2` (DWARF, class-scoped enum).

## 2. Reconstructed behaviour

Common logging helper (all nine methods): priority is `RS_INFO` (3) when
`(user->base_info_.flags_ & 1) == 1`, otherwise `RS_DEBUG` (2). The uin
used in log messages is `user->base_info_.uin_`, or 0 when `user == NULL`.
Only `send_msg_to_client` actually tolerates a NULL user for this prefix;
after the first log it dereferences the user unconditionally, exactly like
the ORIG.

### 2.1 `send_msg_to_client` @ 0xb1920

1. `uin = user ? user->uin_ : 0`; priority as above.
2. `debug_output(priority, "[%u][%s], type=%d, len=%d", uin, __func__, type, len)`.
3. `send_id = user->base_info_.game_usr_id_` (offset `+0x14`);
   if `send_id == 0xffffffff` then `send_id = user->base_info_.uin_`
   (offset `+0x00`).
4. `ret = Singleton<TssSdkCommSvc>::instance()->send_buf_to_client(type, send_id, data, (short)len)`.
5. On `ret != 0`:
   `debug_output(RS_ERROR, "[%u][%s], ret=%d, type=%d, len=%u", uin, __func__, ret, type, len)`
   and `DPMonitorReporter::report_data(0x3fb, 1)` (1019, 1).
6. Return `ret` (0 success, otherwise the transport error).

ORIG strings: format `0x21c440`, error format `0x21c4d8`, `__func__`
`0x21c92f`. Singleton object `Singleton<TssSdkCommSvc>::instance()::obj`
is 40 bytes at `0x0029d820`, guard at `0x0029d7f0`; the inline singleton
guard/ctor/`__cxa_atexit` sequence is reproduced by the reconstructed
`Singleton<T>::instance()`.

### 2.2 `revive_client_mouse` @ 0xb1aa0

Builds `sec_proto::TClientPkg` (25 bytes):

```text
head.protocol_id_ = 0x33
head.sln_id_      = 4
head.sub_sln_id_  = 0x135
body.reviseMouse.cmdID  = id
body.reviseMouse.isHint = tips
```

No `seq_` store exists; the ORIG leaves `head.seq_` uninitialised.
`TClientPkg::pack(buf[0x10000], 0xffff, &len, 0)` then
`debug_output(priority, "[%u][%s], revive client mouse, id=%u, tips=%d", ...)`
and `return send_msg_to_client(0, user, buf, (unsigned short)len)`.
Log format `0x21c4fc`, `__func__` `0x21c77e`.

### 2.3 `send_special_control` @ 0xb1d10

`TClientPkg` head `protocol_id_=0x33`, `sln_id_=4`,
`sub_sln_id_=0x136`; body `TSpecialControl`:

```text
shPacketSize = 0
shPacketType = 0x1a
chDataType   = 0
cmdid        = arg
bufLen       = 0
```

Pack with `(buf, 0xffff, &len, 0)`, log
`"[%u][%s], special control, cmd_id=%u"` (format `0x21c550`, `__func__`
`0x21c753`), then `send_msg_to_client(0, ...)`. `head.seq_` remains
uninitialised in ORIG.

### 2.4 `send_safe_mode_notify` @ 0xb1e00

`TClientPkg` head `0x33/4/0x137`; body `TSafeMode.iSafemodeLevel = level`.
`len` is explicitly zeroed before pack (ORIG `[ebp-0x10]=0`), pack with
`(buf, 0xffff, &len, 0)`, log
`"[%u][%s], safe mode notify, level=%d"` (format `0x21c578`,
`__func__` `0x21c768`), then `send_msg_to_client(0, ...)`.
`head.seq_` remains uninitialised in ORIG.

### 2.5 `send_captcha_finish` @ 0xb1b90

`sec_proto::TSSCaptchaOldPkg` (4851 bytes) is zeroed with
`memset(&pkg, 0, 0x12f3)` and filled:

```text
head_.msg_id_    = 0xc14
head_.version_   = 0
head_.uin_       = user->uin_
head_.send_time_ = time(NULL)
head_.head_len_  = 0x18
head_.body_len_  = 0x0f          (rewritten by TSSCaptchaOldPkg::pack)
body_.captcha_busi_finish_.app_id_ = 0x6ad219
body_.captcha_busi_finish_.busi_id_ = 1
```

The body union is selected by `head_.msg_id_ == 0xc14` inside
`TSSCaptchaOldBody::pack` → `CaptchaBusiFinish` (527 bytes, fixed prefix 15).

The CAPTCHA package is packed into the output buffer at offset **12**
(`buf + 12`, cap `0xfff3`); then a `ClientPkgHead` is built at `buf`:

```text
protocol_id_ = 0x33
sln_id_      = 0x0c
sub_sln_id_  = 0x271c
body_len_    = (uint16)captcha_package_length
```

`ClientPkgHead::pack(buf, 0xffff, &head_len, 0)` writes 12. Total length is
`body_len_ + head_len` (39 + 12 = 51 for the fixed message). Log
`"[%u][%s], captcha finish, len=%u"` (format `0x21c52c`, `__func__`
`0x21c680`) then `send_msg_to_client(0, user, buf, (uint16)total)`.
`ClientPkgHead.seq_` is **not** written by ORIG (the memset only covers the
CAPTCHA package); it stays uninitialised in both ORIG and the reconstruction.

### 2.6 `send_dp_msg_to_client` @ 0xb1ee0 (fragmentation)

1. `frag_size = DP_CONFIG::glb_dp_config_->dp_proto_conf.dp_fragmentation_size`
   (`DPConfig+6`; GOT slot `0x002943ac` → `glb_dp_config_` at `0x002bb4c0`).
2. Direct path when `frag_size == 0 || frag_size >= len`:
   `ret = send_msg_to_client(1, user, data, len)`; on success
   `++user->user_stat_.internal_msg_send_count_` (`+0xa8`); return `ret`.
3. Otherwise:
   `total_pieces = ceil(len / frag_size)` stored in `piece.totalPiece`;
   `crc = Crc32(data, len)` (the ORIG contains an unreachable `len == 0`
   early return and it is kept).
4. One 51218-byte `sec_proto::TClientFrame` is built with
   `head.totalSize = len`, `head.protoId = 7`,
   `piece.pieceSerial = 0`, `piece.totalCrc = crc`.
5. Loop: `chunk = min(frag_size, remaining)`;
   `piece.pieceDataSize = chunk`; `head.packetLen = chunk + 18`;
   `memcpy(piece.pieceData, src, chunk)`;
   `frame.pack(buf[0xffff], 0xffff, &packed_len, 0)`;
   then **advance first** (`remaining -= chunk`, `src += chunk`) and call
   `send_msg_to_client(1, user, src, remaining)`.
   On failure return that error; on success
   `++internal_msg_send_count_`; if `remaining != 0`, `++piece.pieceSerial`
   and repeat.
6. Return 0 after the loop.

**ORIG quirk (verified at 0xb2055–0xb2091 and by differential execution):**
the frame packed into `buf` is *never sent*. The call passes the advanced
raw input pointer `src` and the post-decrement `remaining` count. The first
`frag_size` bytes are therefore never transmitted, and the tail is
transmitted repeatedly (e.g. `len=25, frag=10` produces raw sends of
`data+10/15`, `data+20/5`, `data+25/0`). The reconstruction reproduces this
byte-for-byte; the unused `TClientFrame::pack` call is retained because it
is an observable call target.

### 2.7 `send_captcha_request` @ 0xb2500

`sec_proto::SecsvrCaptchaReq` (541 bytes) fields set (all offsets from the
packed layout):

```text
version_            +4  = 0
room_id_            +2  = (uint16)DP_CONFIG::glb_dp_config_head_->world_id (+12)
bus_id_             +8  = 0
time_stamp_         +10 = time(NULL)
client_ip_          +14 = 0
max_pic_change_num_ +18 = 3
max_verify_num_     +20 = 1
timeout_            +22 = 0x78
busi_id_            +26 = 1
busi_info_len_      +27 = min((uint16)strlen(notify_data), 0x200)
busi_info_          +29 = copy of notify_data[0..len)
```

`channel_id_` (+0) is **not written** by ORIG; it remains stack garbage in
both ORIG and the reconstruction. The ORIG truncates `strlen` to 16 bits
before applying the 0x200 clamp; the source does the same.
Log `"[%u][%s], captcha request, notify=%s"` (format `0x21c5e0`,
`__func__` `0x21c694`), then
`return send_ipc_pkg<SecsvrCaptchaReq>(user, req, 0x4bed9)`.
The ORIG inlined that template; this reconstruction performs the source-level
call to the real weak instantiation.

### 2.8 `send_error_report_msg` @ 0xb2920

`sec_proto::ReportErrReq` (8 bytes):

```text
channel_id_ +0 = (uint16)Singleton<TssSdkCommSvc>::instance()->channel_id_
room_id_    +2 = (uint16)user->base_info_.room_id_
error_id_   +4 = err_id
```

`err_num` is only logged, not transmitted. Log
`"[%u][%s], err_id=%d, err_num=%d"` (format `0x21c608`, `__func__`
`0x21c877`), then `return send_ipc_pkg<ReportErrReq>(user, req, 0x4baf7)`.

### 2.9 `send_cmd_monitor` @ 0xb2d10

`sec_proto::LATransAdaptDPCmdMonitor` (14 bytes):

```text
room_id_         +0  = user->base_info_.room_id_
time_stamp_      +4  = time(NULL)
monitor_cmd_id_  +8  = (int32)cmd
is_need_encrypt_ +12 = (uint8)is_need_encrypt
is_encrypted_    +13 = (uint8)is_encrypted
```

Log `"[%u][%s], cmd monitor, cmd=%u, is_need_encrypt=%u, is_encrypted=%u"`
(format `0x21c628`, `__func__` `0x21c792`), then
`return send_datatrans_pkg<LATransAdaptDPCmdMonitor>(user, 0x23, monitor, 0x5573d)`.
`0x23` is the `SERVICES_ID` service type of the inner `Comm_App_Frame`;
`0x5573d` is its frame command. The ORIG inlined both `send_datatrans_pkg`
and the inner `send_ipc_pkg<ReqSecsvrForwardData>`; this reconstruction calls
the real weak instantiation.

### Encryption / billing boundary

None of these nine methods encrypts or decrypts. They hand already-built
plaintext buffers to `TssSdkCommSvc::send_buf_to_client` (client direction)
or, through the extern templates, to `send_buf_to_backend` (secsvr
direction). The DP encrypt/decrypt path lives in `DPMsgHandler` /
`DPInterface` (`recv_buffer_for_encrypt`, `recv_buffer_for_decrypt`) and is
outside this slice. No `DPBillAdapter` call occurs anywhere in
`0x000b1920`–`0x000b33bc`; the only monitor call is
`DPMonitorReporter::report_data(0x3fb, 1)` in `send_msg_to_client`.

## 3. Template instantiations left extern (required call arguments)

The three template call sites are left as declarations, per the assignment.
Their ORIG instantiations and decoded semantics (used to fix the arguments
above) are:

| ORIG symbol | range | decoded semantics |
|---|---|---|
| `send_ipc_pkg<SecsvrCaptchaReq>` | `0x000b3e50`–`0x000b4171` | frame `init_framehead(0x10000,0,cmd)`; services send `0x2b`, recv `0x1f`, proxy 0; `app_id = glb_dp_config_head_->game_id`; `appdata_encode(pkg,0xffbd,0)`; `send_buf_to_backend(frame,frame.frame_length_,true,0)`; returns 0 / `0x139f` (encode fail) / `-2` (backend fail) |
| `send_ipc_pkg<ReportErrReq>` | `0x000b4180`–`0x000b44a1` | same shape, cmd `0x4baf7` from caller |
| `send_ipc_pkg<ReqSecsvrForwardData>` | `0x000b44b0`–`0x000b47d1` | same shape, used inside `send_datatrans_pkg` with cmd `0x4c6ad` |
| `send_datatrans_pkg<LATransAdaptDPCmdMonitor>` | `0x000b47e0`–`0x000b4e02` | builds outer frame with `init_framehead(0x10000,0,cmd)`, send service `entry`, recv `0x1f`; encodes the PKG (`0xffbd`); builds `ReqSecsvrForwardData` `{channel_id_, room_id_, data_len_=(u16)outer.frame_length_, memcpy(data_,outer,outer.frame_length_)}`; calls `send_ipc_pkg<ReqSecsvrForwardData>(...,0x4c6ad)`; returns 0 / `0x139f` / `-2` |
| `generate_comm_frame<LATransAdaptDPCmdMonitor>` | `0x000b3710`–`0x000b38c2` | inlined into `send_datatrans_pkg`; standalone shape confirms the frame layout and `appdata_encode` failure `0x139f` + log `"[%u][%s], generate_comm_frame fail, ret=%d, entrie=%d, cmd=%u"` |

Return-code contract for the three extern templates (and therefore for
`send_captcha_request` / `send_error_report_msg` / `send_cmd_monitor`):
`0` success, `0x139f` (5023) app-data encode failure, `-2` backend send
failure. `send_datatrans_pkg` additionally logs the `generate_comm_frame`
failure string for the encode failure.

## 4. Protocol/wire layouts used (packed, DWARF-verified)

```text
SERVICES_ID (6):            services_type_ u16 +0, services_id_ u32 +2
ClientPkgHead (12):         protocol_id_ +0, seq_ +2, sln_id_ +6,
                            sub_sln_id_ +8, body_len_ +10
TReviveMouse (8):           cmdID +0, isHint +4
TSpecialControl (13):       shPacketSize +0, shPacketType +2, chDataType +4,
                            cmdid +5, bufLen +9
TSafeMode (4):              iSafemodeLevel +0
TClientPkgBody (13):        union {reviseMouse, specialControl, safeMode}
TClientPkg (25):            head +0, body +12
TSSCaptchaOldHead (24):     msg_id_ +0, version_ +2, uin_ +4, client_ip_ +8,
                            seq_ +12, send_time_ +16, head_len_ +20,
                            body_len_ +22
CaptchaBusiFinish (527):    app_id_ +0, antibot_bus_id_ +4, zone_id_ +8,
                            busi_id_ +12, busi_info_len_ +13, busi_info_ +15
TSSCaptchaOldBody (4827):   union (size fixed by BusiCaptchaPic)
TSSCaptchaOldPkg (4851):    head_ +0, body_ +24
SecsvrCaptchaReq (541):     channel_id_ +0, room_id_ +2, version_ +4,
                            bus_id_ +8, time_stamp_ +10, client_ip_ +14,
                            max_pic_change_num_ +18, max_verify_num_ +20,
                            timeout_ +22, busi_id_ +26, busi_info_len_ +27,
                            busi_info_ +29 (512)
ReportErrReq (8):           channel_id_ +0, room_id_ +2, error_id_ +4
LATransAdaptDPCmdMonitor (14): room_id_ +0, time_stamp_ +4,
                            monitor_cmd_id_ +8, is_need_encrypt_ +12,
                            is_encrypted_ +13
TClientFrameHead (8):       packetLen +0, totalSize +4, protoId +6
TClientFramePiece (51210):  totalPiece +0, pieceSerial +2, totalCrc +4,
                            pieceDataSize +8, pieceData +10 (51200)
TClientFrame (51218):       head +0, body +8
```

`tss_sdk_impl::UserInfo` fields read (from existing `user_info_recon.h`):
`uin_ +0x00`, `room_id_ +0x08`, `game_usr_id_ +0x14`, `flags_ +0x20`,
`internal_msg_send_count_ +0xa8` (400-byte object).
`TssSdkCommSvc` (40 bytes): `channel_id_ +0`, `plugin_mgr_ +4`, `dp_if_ +8`,
`sdk_channel_status_ +12` (24), `error_info_ +36`.
`DP_CONFIG::glb_dp_config_` `0x002bb4c0` (GOT `0x002943ac`),
`glb_dp_config_head_` `0x002bb4c4` (GOT `0x00294358`);
`DPConfig+6` = `dp_proto_conf.dp_fragmentation_size`;
`DPConfigHead+8` = `game_id`, `+12` = `world_id` (280-byte object).

## 5. Constants / error codes / logs

- Client protocol id `0x33`; `sln_id_` 4; sub commands
  `0x135` revive, `0x136` special control, `0x137` safe mode; special
  `shPacketType 0x1a`.
- CAPTCHA: old msg id `0xc14`, head len `0x18`, body len `0xf`, app id
  `0x6ad219`, busi id 1; final `sln_id 0x0c`, `sub_sln_id 0x271c`.
- CAPTCHA request: max change 3, max verify 1, timeout `0x78`, busi id 1,
  notify clamp `0x200`.
- App-frame commands: CAPTCHA request `0x4bed9`, error report `0x4baf7`,
  datatrans outer `0x5573d`, forward `0x4c6ad`; datatrans entry service type
  `0x23`; appdata cap `0xffbd`.
- `send_msg_to_client` failure monitor: `report_data(0x3fb, 1)`.
- Returns: see §2/§3; `-1` (`0xffffffff`) is the `game_usr_id_` sentinel,
  not a return code in these methods.
- Log formats: `send_msg` `0x21c440`/`0x21c4d8`; revive `0x21c4fc`;
  captcha finish `0x21c52c`; special `0x21c550`; safe `0x21c578`;
  captcha request `0x21c5e0`; error report `0x21c608`; cmd monitor
  `0x21c628`; IPC template encode fail `0x21c45c`, IPC fail `0x21c484`,
  IPC success `0x21c4b0`, backend fail `0x21a568`, generate frame fail
  `0x21c5a0`. All use `__func__` strings at `0x21c680`, `0x21c694`,
  `0x21c6a9`, `0x21c753`, `0x21c768`, `0x21c77e`, `0x21c792`, `0x21c7a3`,
  `0x21c877`, `0x21c92f` as applicable.

## 6. External dependencies / boundaries

Non-libc undefined symbols emitted by the object (all exist in the ORIG
dynamic symbol table; verified):

```text
Crc32(unsigned char*, unsigned int)                            0x0019dc80
ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)
ZEN_Trace_LogMsg::debug_errorex(char const*, ...)
tss_dpsdk::DPMonitorReporter::report_data(unsigned int, unsigned int)
tss_dp_comm::DP_CONFIG::glb_dp_config_                         (B 0x002bb4c0)
tss_dp_comm::DP_CONFIG::glb_dp_config_head_                    (B 0x002bb4c4)
tss_sdk_impl::TssSdkCommSvc::TssSdkCommSvc() / ~TssSdkCommSvc()
tss_sdk_impl::TssSdkCommSvc::send_buf_to_client(int, unsigned int, unsigned char*, short)
sec_proto::TClientPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const
sec_proto::ClientPkgHead::pack(...) const
sec_proto::TSSCaptchaOldPkg::pack(...) const
sec_proto::TClientFrame::pack(...) const
DPMsgPacker::send_ipc_pkg<SecsvrCaptchaReq>(...)               0x000b3e50
DPMsgPacker::send_ipc_pkg<ReportErrReq>(...)                   0x000b4180
DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdMonitor>(...) 0x000b47e0
```

The object also emits the weak `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()`
plus its guard/object, exactly as the ORIG TU does. No invented helper,
bridge or fake-success symbol is defined.

## 7. Compile result

Compiler: GCC 4.1.2 (c5root), matching the ORIG 4.1.x generation.

```sh
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_packer_core_agent.cpp \
  -o /tmp/dp_msg_packer_core_agent.o
```

Result: exit 0, no diagnostics. The object defines exactly the nine strong
`T` symbols requested (mangled names identical to ORIG) plus
`__i686.get_pc_thunk.bx`, `__tcf_0`, and the weak singleton trio. Undefined
symbols are only the list in §6 and the normal C++/libc runtime
(`memcpy`, `memset`, `strlen`, `time`, guard/atexit/unwind).

A second compile with WSL `g++ 13 -m32 -std=gnu++98 -O2 -fPIC
-fno-implement-inlines -fno-stack-protector` also succeeds; the 4.1.2 object
is the comparison artifact.

## 8. Differential runtime verification vs ORIG

An out-of-tree 32-bit harness (`/mnt/d/tmp/diff_harness.cpp`, temp only)
links the reconstructed object, `dlopen`s the ORIG `.so`, and interposes the
executable's exported symbols over ORIG's PLT calls. ORIG's real
`TClientPkg::pack`, `ClientPkgHead::pack`, `TSSCaptchaOldPkg::pack`,
`TClientFrame::pack`, `Crc32`, and the three template instantiations are
called through function pointers at the addresses above; `send_buf_to_client`
and `send_buf_to_backend` are stubs that capture `(cmd, uin, len, bytes)`.
For each test the reconstructed method and the ORIG method are called with
identical inputs and the captured packets, return values, counter updates,
`report_data` calls and formatted log lines are compared.

Passed:

- `send_msg_to_client`: normal, `game_usr_id_ == -1` fallback, and transport
  failure (`report_data(1019,1)`); packets/uin/return/logs identical.
- `revive_client_mouse`, `send_special_control`, `send_safe_mode_notify`,
  `send_captcha_finish`: packet bytes identical except the four
  uninitialised `ClientPkgHead.seq_` bytes (2..5), which are stack garbage
  in ORIG by construction; lengths, returns and logs identical.
- `send_dp_msg_to_client`: direct, `frag_size=0`, `frag_size>len`, length 0,
  exact-multiple fragmentation and non-multiple fragmentation
  (`len=25/frag=10` → three sends `data+10/15`, `data+20/5`, `data+25/0`),
  plus first-chunk transport failure; all packets, return values,
  `internal_msg_send_count_` and logs identical.
- `send_error_report_msg`, `send_cmd_monitor` (all four bool combinations),
  and `send_captcha_request`: backend frames identical except the two
  uninitialised `SecsvrCaptchaReq.channel_id_` bytes at frame offset 50..51;
  returns and logs identical.
- Backend-failure path (`send_buf_to_backend` returns -3): return `-2`,
  `debug_errorex` and the `send_ipc_pkg fail` log identical.

Ignored byte ranges in the comparison are exactly the fields the ORIG never
initialises (`TClientPkg`/`ClientPkgHead.seq_`, `SecsvrCaptchaReq.channel_id_`).

## 9. Behaviour not proven / known limits

- The concrete values of the uninitialised bytes above are stack-dependent
  in ORIG and in this reconstruction; they are intentionally not assigned.
- Byte/instruction identity is not claimed. GCC 4.1.0-vs-4.1.2 scheduling,
  frame layout and inlining differ (the ORIG inlined `send_ipc_pkg<>` /
  `send_datatrans_pkg<>` in the three users; this source calls the real
  instantiations as required).
- No full `.so` link/load as a drop-in replacement was attempted; the
  template instantiations and the protocol encoders remain external.
- Differential tests used a fixed `DPConfigHead` / `DPConfig` and a fixed
  40-byte mock `TssSdkCommSvc` channel id; other configuration values follow
  directly from the decoded field offsets but were not exhaustively swept.
