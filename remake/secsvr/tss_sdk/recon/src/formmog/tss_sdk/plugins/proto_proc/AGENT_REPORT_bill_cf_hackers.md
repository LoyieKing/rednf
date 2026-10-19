# AGENT_REPORT_bill_cf_hackers

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Output unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/bill_cf_hackers_agent.cpp`

Semantic reconstruction only: no byte-identity claim, no commit. Addresses below
are virtual addresses in the target. Sources: `nm -S`, `objdump -d -l`,
DWARF function/type extraction, and the shipped source headers found under
`dnf_secsvr` (`comm_app_frame.h`, `comm_bill_record.h`) as corroboration.
Every call target named is the exact symbol the original instruction sequence
references.

## 1. Symbols and original ranges

| ORIG address | Size | Symbol | Binding |
|---|---|---|---|
| `0x000a3120` | `0x2c5` | `tss_dpsdk::DPBillRecorder::write_to_logsvr(sec_proto::LogSvrCommBillRecord const&)` | T |
| `0x000a33f0` | `0x3f` | `tss_dpsdk::DPBillRecorder::~DPBillRecorder()` (D0, deleting) | W |
| `0x000a3430` | `0x2e` | `tss_dpsdk::DPBillRecorder::~DPBillRecorder()` (D1, complete) | W |
| `0x000a37c0` | `0xb93` | `tss_dpsdk::DPCfExtractUserInfo::hack(UserInfo*, int, unsigned, unsigned char*&, unsigned int&)` | T |
| `0x000a4860` | `0x29` | `DPCfExtractUserInfo::~DPCfExtractUserInfo()` (D1) | W |
| `0x000a4890` | `0x39` | `DPCfExtractUserInfo::~DPCfExtractUserInfo()` (D0) | W |
| `0x000a4970` | `0x97` | `tss_dpsdk::DPCfForwardRoomMasterMsg::hack(...)` | T |
| `0x000a4e20` | `0x34` | `DPCfForwardRoomMasterMsg::~DPCfForwardRoomMasterMsg()` (D1) | W |
| `0x000a4e60` | `0x44` | `DPCfForwardRoomMasterMsg::~DPCfForwardRoomMasterMsg()` (D0) | W |
| `0x000a4f50` | `0x20d` | `tss_dpsdk::DPCfRecordUserInfo::hack(...)` | T |
| `0x000a5570` | `0x29` | `DPCfRecordUserInfo::~DPCfRecordUserInfo()` (D1) | W |
| `0x000a55a0` | `0x39` | `DPCfRecordUserInfo::~DPCfRecordUserInfo()` (D0) | W |

Source CUs (DWARF): `dp_bill_recorder.cpp` line 17, `dp_cf_extract_user_info.cpp`
line 16, `dp_cf_forward_room_master_msg.cpp` line 15,
`dp_cf_record_user_info.cpp` line 15.

## 2. Exact mangled signatures

```
_ZN9tss_dpsdk14DPBillRecorder15write_to_logsvrERKN9sec_proto20LogSvrCommBillRecordE
_ZN9tss_dpsdk19DPCfExtractUserInfo4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
_ZN9tss_dpsdk24DPCfForwardRoomMasterMsg4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
_ZN9tss_dpsdk18DPCfRecordUserInfo4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
```

The `hack` parameters are `unsigned char *&` and `unsigned int &` (not
const-qualified; mangling has `RPh`/`Rj`). DWARF names are `user`, `type`,
`cmd_id`, `buf`, `len`.

## 3. Class layouts and vtables

### 3.1 `DPBillRecorder`

```
CommBillRecord  : vptr +0, user_id_ +0x10, sizeof 0x1760 (5984)
DPBillRecorder  : CommBillRecord, bool enbale_bill_to_log_ +0x1760,
                  sizeof 0x1764 (5988)
```

`enbale_bill_to_log_` is the DWARF spelling. `CommBillRecord::get_userid()` is
the inline `return user_id_;` at `comm_bill_record.h:134`.

Vtable `0x00292ed0`, size `0x14` (5 slots):

| slot | target |
|---|---|
| +0x00 | `0` (offset-to-top) |
| +0x04 | `_ZTIN9tss_dpsdk14DPBillRecorderE` |
| +0x08 | `DPBillRecorder::~DPBillRecorder()` D1 `0xa3430` |
| +0x0c | `DPBillRecorder::~DPBillRecorder()` D0 `0xa33f0` |
| +0x10 | `DPBillRecorder::write_to_logsvr(...)` `0xa3120` |

The local class model reproduces this five-slot vtable exactly; `write_to_logsvr`
is the only virtual added by the derived class.

### 3.2 CF hackers

```
IDPHacker                       4 bytes, vptr only
DPPkgForwardLaHacker : IDPHacker 4 bytes, no members
DPCfExtractUserInfo  : IDPHacker 4
DPCfForwardRoomMasterMsg : DPPkgForwardLaHacker 4
DPCfRecordUserInfo   : IDPHacker 4
```

DWARF inheritance was checked directly (`DW_TAG_inheritance`): the first,
third and fourth classes derive `IDPHacker`; `DPCfForwardRoomMasterMsg` derives
`DPPkgForwardLaHacker`.

`IDPHacker` slot order (matches all four `0x3c`/15-slot vtables):

`+0x00 init`, `+0x04 hack`, `+0x08 on_login`, `+0x0c on_logout`,
`+0x10 proc`, `+0x14 on_ban_mode`, `+0x18 on_drop_cmd`,
`+0x1c on_verify_response`, `+0x20 on_verify_msg`, `+0x24 on_safe_mode`,
`+0x28 on_special_control`, `+0x2c D1`, `+0x30 D0`.

The reconstructed emitters produce `0x3c`-byte vtables whose relocations are
slot-for-slot identical to the target (including
`DPCfForwardRoomMasterMsg::hack` in slot +0x04 and the D1/D0 pair).

### 3.3 `UserInfo` fields used

`UserInfo` is 400 bytes; `hack_data_` is at `+0x0dc` and its anonymous
`DPHackCfUserInfo` member is at `+0x0a8`, so:

| absolute offset | field |
|---|---|
| UserInfo `+0x000` | `base_info_.uin_` (u32) |
| UserInfo `+0x020` | `base_info_.flags_` (u32; bit0 = trace) |
| UserInfo `+0x184` | `cf_user_info_.game_mode_` (u8) |
| UserInfo `+0x185` | `cf_user_info_.role_posi_` (u8) |
| UserInfo `+0x186` | `cf_user_info_.role_type_` (u8) |
| UserInfo `+0x187` | `cf_user_info_.game_map_` (u8) |
| UserInfo `+0x188` | `cf_user_info_.channel_id_` (u8) |
| UserInfo `+0x18a` | `cf_user_info_.room_id_` (u16) |

`DPHackCfUserInfo` is 8 bytes with a one-byte pad at +5 before `room_id_` at +6.

## 4. `DPBillRecorder::write_to_logsvr` (0xa3120)

Source-level shape (line 17/19/21):

```cpp
int DPBillRecorder::write_to_logsvr(const LogSvrCommBillRecord& pkg) {
    ZEN_Trace_LogMsg::debug_output(
        enbale_bill_to_log_ ? RS_INFO : RS_DEBUG, "%s", pkg.string_buffer_);
    return Singleton<TssSdkCommSvc>::instance()->send_ipc_pkg(
        pkg, get_userid(), 0x4e21u, 2);
}
```

Decoded details:

1. `cmpb $1, this+0x1760; sbb edx,edx; add 3` yields priority `RS_INFO (3)`
   when the byte is non-zero, `RS_DEBUG (2)` otherwise.
2. Format string is the two-byte `"%s"` at `0x21b5d8`; the third argument is the
   `LogSvrCommBillRecord*` itself, and its first member is
   `char string_buffer_[2048]`.
3. `get_userid()` compiles to `mov 0x10(%esi),%esi`.
4. ORIG inlines `Singleton<TssSdkCommSvc>::instance()` and
   `send_ipc_pkg<LogSvrCommBillRecord>` into this function. The inlined body is
   the same as the out-of-line instantiation at `0xa34f0`:
   - stack buffer `unsigned char[0x10000]`; `Comm_App_Frame` at its start;
   - `init_framehead(0x10000, 0, 0x4e21)`;
   - temporary services: receive type `2`, proxy `{0,0}`, send type `0x2b`
     (all ids 0), assigned to `recv_service_`, `proxy_service_`,
     `send_service_` at frame +0x16/+0x1c/+0x10;
   - `frame_uin_` (+0x0c) = `get_userid()`;
   - `transaction_id_` (+0x22) and `backfill_trans_id_` (+0x26) = 0;
   - `app_id_` (+0x2a) = `glb_dp_config_head_->game_id` (config head +0x08);
   - dump guard is raw bit 16 of the option word
     (`testb $1, frame+6`, i.e. `inner_option_ & 0x10000`); when set it calls
     `Comm_App_Frame::dumpoutput_framehead(frame, "[TRACK MONITOR][TRANS SEND]",
     RS_INFO)`;
   - `appdata_encode(0xffbd, pkg, 0, 0)`; failure is replaced by `0x139f`
     (the encoder itself returns `0x13a0`, and sets
     `frame_length_ = 0x32 + packed length`);
   - `send_buf_to_backend((char*)frame, frame.frame_length_, true, 0)`;
   - backend failure logs `"[%s]send to backend fail. ret=%d"` with
     `__PRETTY_FUNCTION__` and returns `-2` (`0xfffffffe`).

   Return values: `0` success, `0x139f` encode failure, `-2` backend failure.

This unit does not call the out-of-line template instantiation: it reproduces
the inlined body in source (the same frame/services/appdata/send sequence), so
`write_to_logsvr` itself references `Comm_App_Frame::init_framehead`,
`SERVICES_ID` ctor/dtor, `Comm_App_Frame::appdata_encode<LogSvrCommBillRecord>`
and `TssSdkCommSvc::send_buf_to_backend` directly, exactly as the target. The
local `Singleton<T>::instance()` inline definition re-creates the
function-local `TssSdkCommSvc` static (`obj`/guard), including the
guard/construct/atexit sequence. The same header's out-of-line instantiation
still exists at `0xa34f0` for other callers.

The compiled size is `0x2c5`, identical to ORIG, and a normalized instruction
diff against ORIG differs only in relocation displacements, local stack-slot
ordering and the order in which the three `SERVICES_ID` temporaries are
destroyed (all three are destroyed on every exit path). Note that both ORIG
and this build materialize `return 0` as a 16-bit `xor %si,%si`; the upper half
of ESI (the singleton guard pointer scratch) is not cleared, so an observer of
the success-path EAX sees the same non-zero result as the target. This artifact
is reproduced rather than "fixed"; ordinary callers ignore the return.

## 5. `DPCfExtractUserInfo::hack` (0xa37c0)

All packet reads are unaligned. Length checks are unsigned. `data = *buf`,
`len = *lenref`. Success or handled command returns `0`; malformed packet or
failed enter-room lookup returns `-1`.

Dispatch (the first command test is hoisted before the `type == 1` check in
ORIG, so `0x13e00` reaches group A for every type; `0x53e00` only for type 1):

| group | type | cmd ids | line |
|---|---|---|---|
| A | any for `0x13e00`; else 1 | `0x13e00`, `0x53e00` | 21-65 |
| B | 1 | `0x11f00`, `0x56600` | 34-42 |
| C | 1 | `0x13400`, `0x56e00` | 45-65 |
| D | 2 | `0x13500`, `0x56f00` | 73-89 |
| E | 2 | `0x13700`, `0x14a00`, `0x50900` | 92-127 |
| G | 2 | `0x13d00` | 140-180 |
| H | 2 | `0x51300` | 157-161 |
| I | 2 | `0x12800` | 166-180 |
| J | 2 | `0x14f00` | 185-214 |
| K | 2 | `0x14e00` | 219-225 |
| L | 2 | `0x14800` | 231-243 |
| M | 2 | `0x15b00` | 248-253 |
| N | 2 | `0x14100`, `0x50b00` | 256-262 |

Decoded branches:

- **A** (`0x13e00`/`0x53e00`): requires `len > 0x0d` (expected 0x0e).
  `game_map_ = data[0x0c]`, `game_mode_ = data[0x0e]`; logs
  `"[%u][%s], game_map_=%u, game_mode=%u"`.
- **B**: requires `len > 5` (expected 6).
  `channel_id_ = (u8)(data[6] + 1)`; logs `channel_id_=%u`.
- **C**: requires `len > 0x21` (expected 0x22); `set_user_flag(2)`;
  `role_type_ = role_posi_ = 0`;
  `game_map_ = (cmd == 0x56e00) ? data[0x1e] : data[0x20]`;
  `game_mode_ = data[0x22]`; logs `game_map_`/`game_mode`.
- **D**: requires `len > 0x0b` (expected 0x0c). `data[6]` must equal `0x9c`;
  otherwise logs `"[%u][%s], res=%u"` and returns -1.
  On success `clear_user_flag(0x0c)`;
  `room_id_ = (u16)(data[0x0a] | data[0x0b]<<8) + 1`; logs `room_id_=%u`.
- **E**: `role_info = data + 0x4a` for `0x13700`/`0x14a00`, and
  `data + 0x46` for `0x50900`. `end = data + len`.
  - If `end <= role_info + 4`: log
    `"enter room fail, i=%d"` with i=0, return -1.
  - Scan 16 slots, stride `0x124`; slot uin is the 32-bit value at
    `role_info + i*0x124`, compared with `user->uin_`. The bound for slot i is
    `end > role_info + i*0x124 + 4`; failure logs the current i and returns -1;
    exhaustion logs i=16 and returns -1.
  - On match: `role_type_ = (i > 7)`, `role_posi_ = i % 8`.
    For `0x50900`: `game_map_ = data[0x0e]`, `game_mode_ = data[0x12]`;
    otherwise `game_map_ = data[0x10]`, `game_mode_ = data[0x16]`.
  - `clear_user_flag(0x0e)`; `room_id_ = u16(data+0x0a) + 1`; logs
    `"enter room, role_type_=%u, role_posi_=%u, game_map_=%u, game_mode=%u,
    room_id_=%d"`.
- **G** (`0x13d00`): requires `len > 9`. `data[6]` must be 0, else log `res=%u`
  and return -1. Toggle `role_type_ = !role_type_`;
  `role_posi_ = data[0x0a]`; log `"update, type=%u, posi=%u"`.
- **H** (`0x51300`): no length check and no packet reads; toggles
  `role_type_` and logs the same update message with the existing
  `role_posi_`.
- **I** (`0x12800`): requires `len > 0x0b`. `data[6]` must be 0, else
  `res=%u` + return -1. `role_type_ = data[0x0a]`;
  `role_posi_ = data[0x0c]`; log update.
- **J** (`0x14f00`): requires `len > 9`. `action = data[7]`.
  - `action` in 1..254: if `role_type_ == data[6]` and
    `role_posi_ == data[0x0a]`, `set_user_flag(2)` and log
    `"I am matser"`; otherwise log `"I am not matser"`. Return 0.
  - `action` 0 or 255: if flags bit 2 (`0x4`) or bit 1 (`0x2`) is set,
    `set_user_flag(8)` and log `"start play"`; else log `"not start play"`.
- **K** (`0x14e00`): requires `len > 0x11`. `revert = data[0x12]`; log
  `"join game, revert=%u"`; `revert == 0 || revert == 5` sets flag 8.
- **L** (`0x14800`): requires `len > 5`. `data[6] == 0`: log
  `"I am not ready now"`, `clear_user_flag(4)`. Else log `"I am ready now"`,
  `set_user_flag(4)`.
- **M** (`0x15b00`): no length check; `clear_user_flag(0x0c)`; log
  `"stop play"`.
- **N** (`0x14100`/`0x50b00`): no length check; `clear_user_flag(0x0a)`;
  log `"exit room"`.

Malformed-length logs use `RS_ERROR` and
`"[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, expected_len=%u"`;
the expected lengths are A=0x0e, B=6, C=0x22, D=0x0c, G=0x0a, I=0x0c,
J=0x0a, K=0x12, L=6. A's type field is the actual `type` argument; B/C/D and
the type-2 groups pass their literal type (1 or 2), matching the pushed
immediates.

All other logs compute priority as `flags_ bit0 ? RS_INFO : RS_DEBUG` and use
`uin_`; the `res=%u` and enter-room-fail paths also handle a null user
(priority `RS_DEBUG`, uin 0) as ORIG does.

## 6. `DPCfForwardRoomMasterMsg::hack` (0xa4970)

```cpp
int ret = 0;
if (user->check_user_flag(2)) {   // flags & 0x2
    debug_output(flags&1 ? RS_INFO : RS_DEBUG,
                 "[%u][%s], forward master msg to LA", uin, "hack");
    ret = DPPkgForwardLaHacker::hack(user, type, cmd_id, buf, len);
}
return ret;
```

The base call is a direct (non-virtual) call to the exact symbol
`_ZN9tss_dpsdk21DPPkgForwardLaHacker4hackEPN12tss_sdk_impl8UserInfoEijRPhRj`
at `0xb7720`. All five arguments and both references are forwarded unchanged.
If the gate is clear the function returns 0 without touching the packet.

## 7. `DPCfRecordUserInfo::hack` (0xa4f50)

Gate: `user->flags_ & 0x8` (`check_user_flag(8)`); clear returns 0.

```
cmd == 0x15b00 || 0x14100 || 0x50b00:
    DPBillAdapter bill("cf user stop play", 0x43, user);
    return 0;                                  // destructor at scope exit

cmd == 0x14f00:
    if (len <= 6) { wrong-data log, type, cmd=0x14f00, len, expected=7;
                    return -1; }
    action = data[7];
    if (action != 0 && action != 0xff) return 0;
    // else fall through to the start-play bill

otherwise (and 0x14f00 with action 0/0xff):
    DPBillAdapter bill("cf user start play", 0x42, user);
    bill << channel_id_      // UserInfo +0x188, u8
         << room_id_         // UserInfo +0x18a, u16
         << role_type_       // UserInfo +0x186, u8
         << role_posi_       // UserInfo +0x185, u8
         << game_mode_       // UserInfo +0x184, u8
         << game_map_        // UserInfo +0x187, u8
         << cmd_id;          // u32
    return 0;
```

The seven stream insertions are instantiated as
`CommBillRecord::operator<<(long long)` (`0xd1590`) on the process-global
`tss_dpsdk::DPBillAdapter::bill_` (`0x002b6e00`), exactly as ORIG; each value is
zero-extended to 64 bits and the high word pushed as 0. The temporary
`DPBillAdapter` only brackets the bill record; its ctor/dtor target
`0xa27d0`/`0xa26c0` (C1/C2 pairs) and do not carry the payload.

The wrong-data log uses `RS_ERROR`, the actual `type` argument and
`expected_len=7`, then returns -1.

## 8. External call targets / ABI boundaries

Declared only (defined in the target image or imported):

| symbol | target |
|---|---|
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | `0x001b3e70` |
| `ZEN_Trace_LogMsg::debug_errorex(char const*, ...)` | `0x001b3d30` |
| `CommBillRecord::CommBillRecord()` / `~CommBillRecord()` | external |
| `CommBillRecord::operator<<(long long)` | `0x000d1590` |
| `TssSdkCommSvc::TssSdkCommSvc()` / `~TssSdkCommSvc()` | external |
| `TssSdkCommSvc::send_buf_to_backend(char const*, unsigned, bool, int)` | `0x0008fbe0` |
| `Comm_App_Frame::init_framehead(unsigned, unsigned, unsigned)` | `0x000cf910` |
| `Comm_App_Frame::dumpoutput_framehead(...)` | `0x000d0d50` |
| `Comm_App_Frame::appdata_encode<LogSvrCommBillRecord>(...)` | `0x000a3460` |
| `SERVICES_ID::SERVICES_ID(unsigned short, unsigned int)` | `0x000d70c0`/`0x000d70e0` |
| `SERVICES_ID::~SERVICES_ID()` | `0x000d7100`/`0x000d7110` |
| `DPBillAdapter::DPBillAdapter(char const*, unsigned, UserInfo*)` | `0x000a27d0` |
| `DPBillAdapter::~DPBillAdapter()` | `0x000a26c0` |
| `DPPkgForwardLaHacker::hack(...)` | `0x000b7720` |
| `IDPHacker` other virtuals (init/on_login/.../on_special_control) | `0x000a4770`..`0x000a4800` |

`glb_dp_config_head_` is symbol `_ZN11tss_dp_comm9DP_CONFIG19glb_dp_config_head_E`
at `0x002bb4c4`; the inlined template reads `game_id` at `+0x08`. The
`Comm_App_Frame` model is 50 bytes packed; `frame_appdata_` starts at +0x32.
`LogSvrCommBillRecord` is 2048 bytes (`string_buffer_` at +0).

## 9. Compile result

Command (c5root GCC 4.1.2, matching target ABI):

```
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector -std=gnu++98 \
    -Wall -Wextra \
    --sysroot=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root \
    -isystem /home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/lib/gcc/x86_64-redhat-linux/4.1.1/include \
    -I recon -c bill_cf_hackers_agent.cpp -o /tmp/bill_cf_hackers_agent.o
```

Result: **exit 0, no warnings**. Compile-time guards confirm
`sizeof(CommBillRecord)==5984`, `sizeof(DPBillRecorder)==5988`,
`sizeof(DPBillAdapter)==4`, `sizeof(UserInfo)==400`,
`sizeof(DPHackCfUserInfo)==8`, cf offset `0x184`, protocol record 2048,
`sizeof(SERVICES_ID)==6`, `sizeof(Comm_App_Frame)==50`,
`sizeof(DPConfigHead)==280`, and all three hackers 4 bytes.

Emitted symbol/vtable checks:

- `DPBillRecorder::write_to_logsvr` T, `0x2c5` bytes;
  vtable `0x14`, exact five slots; D1 `0x2e` and D0 `0x3f` weak.
- `DPCfExtractUserInfo::hack` T, `0xb1a` bytes; `DPCfForwardRoomMasterMsg::hack`
  T, `0x7f`; `DPCfRecordUserInfo::hack` T, `0x23a`.
- all three CF vtables `0x3c` with slot-for-slot identical relocations;
  D1 `0x1e` and D0 `0x2e` weak per class.
- `DPBillAdapter::bill_` size `0x1764` (DPBillRecorder), seven
  `CommBillRecord::operator<<(long long)` call sites in the record function,
  two `DPBillAdapter` ctor/dtor scopes.

## 10. Notes / unresolved

- No byte-identity claim. The reconstruction uses equivalent high-level control
  flow; the original's exact instruction scheduling differs.
- `write_to_logsvr` is reconstructed with the send template fully inlined,
  matching ORIG's size (`0x2c5`) and call targets. The same toolchain also emits
  a weak `Singleton<TssSdkCommSvc>::instance()` helper in addition to the
  target's `instance()::obj`/guard storage; this is compiler-generated inline
  code, not a fake operation, and the object/guard symbols are reproduced.
- The ORIG success path's 16-bit `xor %si,%si` return materialization is
  reproduced by the same source/compiler combination (see section 4).
- CF destructor D1 code size differs (`0x1e` here vs `0x29`/`0x34` in ORIG)
  because GCC 4.1.2 optimizes the empty inline base destructor into a single
  vptr store, whereas the shipped 4.1.0 code also rewrites the `IDPHacker`
  vptr. Destruction has no data members and no side effects in either form; the
  weak symbols, class size and vtable slots are exact.
- `IDPHacker`'s non-overridden virtuals are declared only; their definitions
  are external target symbols.
- No live oracle run was performed; the verdict is based on disassembly, DWARF,
  exact symbol relocations and compilation.
