# HackersB: DPMailVerifyHacker / DPForbidModeHacker / DPSafeModeHacker / DPUicTransHacker

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
Original TUs:

- `src/formmog/tss_sdk/plugins/proto_proc/dp_mail_verify_hacker.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_forbid_mode_hacker.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_safe_mode_hacker.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_uic_trans_hacker.cpp`

Emitted source: `hackers_b_agent.cpp` (one writer, no commits).

All findings come from `nm -S -C`, DWARF DIEs/line tables, `.rodata`/`.data`
bytes, and disassembly. Addresses are link-time virtual addresses.

## Address ranges and emitted symbols

| Method | ORIG address | ORIG size | Emitted size |
|---|---|---|---|
| `DPMailVerifyHacker::on_special_control` | `0x000ac110` | `0x6a` | `0x6a` |
| `DPMailVerifyHacker::cancel_mail_opt` | `0x000ac180` | `0x39` | `0x39` |
| `DPMailVerifyHacker::on_timer` | `0x000ac1c0` | `0xd1` | `0xce` |
| `DPMailVerifyHacker::is_permit_throught` | `0x000ac2a0` | `0x172` | `0x18f` |
| `DPMailVerifyHacker::on_verify_response` | `0x000ac420` | `0x207` | `0x1f5` |
| `DPMailVerifyHacker::on_verify_msg` | `0x000ac630` | `0x153` | `0x143` |
| `DPMailVerifyHacker::hack` | `0x000acba0` | `0xb7e` | `0x930` |
| `DPMailVerifyHacker::init` | `0x000ad770` | `0x1ec` | `0x20f` (W) |
| `DPForbidModeHacker::hack` | `0x000aa3b0` | `0x1c2` | `0x1e7` |
| `DPForbidModeHacker::on_ban_mode` | `0x000aa580` | `0x164` | `0x158` |
| `DPForbidModeHacker::init` | `0x000aab30` | `0x75` | `0x79` (W) |
| `DPSafeModeHacker::hack` | `0x000bb2f0` | `0xdd` | `0xd4` |
| `DPSafeModeHacker::on_safe_mode` | `0x000bb3d0` | `0xa1` | `0xa1` |
| `DPSafeModeHacker::init` | `0x000bb8c0` | `0x75` | `0x79` (W) |
| `DPUicTransHacker::hack` | `0x000bbe30` | `0x726` | `0x63a` |

`init` overrides are weak (`W`) because the header-inline bodies are emitted
through the vtable; the other overrides are strong (`T`). All four implicit
destructor pairs are weak (`W`), and vtables/typeinfo are `V`, as in ORIG.
`DPMailVerifyHacker` has 15 vtable relocations (`0x40` bytes including the
address-point header), the other three have 14 (`0x3c`); this matches the
target exactly. `DPUicTransHacker` inherits `IDPHacker::init` and registers no
events.

## Class / state layouts

```
IDPHacker                    4    vptr
IDPHackerSafe               32    IDPHacker + ZEN_Thread_Mutex(28) at +4
DPPkgDelayHacker            84    safe base
  +0x20 std::map<u32, DPCommandBuf2> waitting_for_send_
  +0x38 std::list<DPCommandBuf>      cmd_delay_
  +0x40 timeval next_timer_
  +0x48 int flag_position_
  +0x4c u32 node_wait_for_send_
  +0x50 u32 node_in_delay_
DPMailVerifyHacker          96    delay base
  +0x54 u32 captcha_no_response_
  +0x58 u32 captcha_counts_
  +0x5c time_t last_captcha_retry_
DPForbidModeHacker           4    vptr only
DPSafeModeHacker             4    vptr only
DPUicTransHacker             4    vptr only
```

`DPPkgDelayHacker::DPCommandBuf` is 24 bytes: `cmd_id_ +0`, `buf_len_ +4`,
`buf_ +8`, `user_ +12`, `timeval expire_time_ +16`.

Relevant `tss_sdk_impl::UserInfo` (400 bytes) offsets:

```
base_info_              +0x000 (uin_ +0, game_id_ +4, room_id_ +8,
                                client_ip_ +0x0c, flags_ +0x20,
                                role_name_[64] +0x24)
user_code_info_         +0x064
user_timer_             +0x094
user_stat_              +0x0a8
hack_data_              +0x0dc (180 bytes)
  safemode_level_       +0x0dc absolute
  dnf_client_uid_       +0x0e0
  drop_cmd_[32]         +0x0e8
  ban_mode_             +0x104 (time_t keep_time_[32], 128 bytes)
  cf_user_info_         +0x184 (game_mode_ +0x184, room_id_ +0x18a)
  delay_cmd_[3]         +0x18c
  captcha_state_        +0x18f
```

Configuration layout (DWARF, `sizeof(DPConfig) == 0x593a`):

```
DPConfig::cmd_hack                 +0x3d7e
  safemode_to_client               +0x3da1
  safemode_hint_client             +0x3da2
  captcha_method                   +0x55aa (DPCaptchaMethod, 16 bytes)
    max_timeout_count +0, min_try_count +4, min_try_time +8,
    waitting_response +12
  ban_mode                         +0x585e (u16 length + u32 values[32])
```

`tss_dp_comm::DP_CONFIG::glb_dp_config_` points at `DPConfig`;
`glb_dp_config_head_` points at a 280-byte `DP_CONFIG` whose `game_id_` is at
+8 and `world_id_` at +12.

## Decoded semantics

### DPMailVerifyHacker

- `init` registers, in this order, `E_MSG2CLIENT`, `E_VERIFY_MSG`, `E_LOGOUT`,
  `E_PROC`, `E_SPECIAL_CONTROL`; the logout/proc slots dispatch to the
  `DPPkgDelayHacker` base implementations. Returns 0. No null check.
- `on_special_control` forwards all five arguments to
  `DPPkgDelayHacker::on_special_control`; when that returns exactly 1 it also
  calls `DPMsgPacker::send_captcha_finish(user)`, then returns the forwarded
  result (not the finish result).
- `cancel_mail_opt` clears `hack_data_.captcha_state_` (+0x18f), calls
  `DPMsgPacker::revive_client_mouse(user, cmd_id, E_TIPS_NONE /*0*/)` and
  returns that helper's int result.
- `on_timer(DPCommandBuf&)`: `user = buf.user_`, `cmd_id = buf.cmd_id_`.
  If `captcha_state_ == 1`: increment `captcha_no_response_`, report monitor
  `0x3f4` with value 1, bill `"captcha no response"` line `0x3d` with
  `captcha_no_response_`, then cancel. Otherwise cancel immediately. Returns
  `cancel_mail_opt`'s result.
- `is_permit_throught(user)`: `method = glb_dp_config_->cmd_hack.captcha_method`.
  - `max_timeout_count == 0` or `max_timeout_count > captcha_no_response_`
    (`+0x54`, **not** the counts field) -> return 0.
  - increment `captcha_counts_`; `now = time(NULL)`.
  - `min_try_count == 0` or `captcha_counts_ % min_try_count == 0` ->
    `last_captcha_retry_ = now`, return 0.
  - else if `min_try_time != 0` and `now - last_captcha_retry_ >= min_try_time`
    -> `last_captcha_retry_ = now`, return 0.
  - otherwise bill `"no captcha verify"` line `0x3b` with
    `{max_timeout_count, min_try_count, min_try_time, captcha_counts_}`,
    report `0x3f5`, return 1.
- `on_verify_response(user, data, data_len)`:
  - logs `[%u][%s], captcha_state_=%d`; if state != 1 return -1.
  - `sec_proto::ClientPkgHead::unpack(data, data_len, 0, 0)`; failure logs
    `ClientPkgHead unpack fail, ret=%d, len=%u` and returns -1.
  - unless `protocol_id_ == 0x33` and `sln_id_ == 0x0c`, return 0.
  - under `ZEN_Lock_Guard(locker_)`, find the first `cmd_delay_` entry whose
    `user_ == user`. Copy it to `new_buf`, erase it, decrement
    `node_in_delay_`, set `captcha_state_ = 2`, add 120 seconds to
    `new_buf.expire_time_.tv_sec`, re-insert via `insert(new_buf)` (which
    increments `node_in_delay_` again), log `get captcha resonece`, report
    `0x406`. Then break/unlock and return 0.
- `on_verify_msg(user, pkg)`:
  - logs `captcha_no_response_=%d, state=%d, result=%u`, resets
    `captcha_no_response_ = 0`.
  - returns 0 if `cmd_delay_` is empty or `captcha_state_ == 0`.
  - under the guard, finds the first entry for this user:
    - `pkg.result_ != 0` -> `cancel_mail_opt(user, cmd_id)` then
      `send_captcha_finish(user)`;
    - `pkg.result_ == 0` -> `DPPkgDelayHacker::on_timer(buf)`;
    - then `remove_from_delay(user, IT)` and `captcha_state_ = 0`.
  - Always returns 0.
- `hack(user, type, cmd_id, buf, len)`:
  - Rejects unless `type == 1` and `hack_data_.safemode_level_ == 0`
    (logs `safemode_level_=%u`), and rejects if `captcha_state_ != 0`
    (logs `captcha_state_=%u` with RS_ERROR).
  - Parses the DNF mail view: requires `len > 0x12`,
    `len > role_name_len_`, and
    `len >= role_name_len_ + 0x11 + 0x10`; logs
    `game pkg is wrong, len=%u, role_name_len_=%u` otherwise (third case
    reports `role_name_len_ + 0x11`).
    `mail_body = (DPHackDnfMailBody*)(buf + role_name_len_ + 0x11)`.
  - Empty mail (`gold_num_ == 0 && items_num_ == 0 &&
    item_.item_posi_ == 0`) logs `mail has nothing` and returns 0.
  - If `is_permit_throught(user)` -> return 0 (packet dropped).
  - Builds a GBK description in a `std::ostringstream`:
    `"您正通过邮件给" + 0x01 + "role_name="`, the raw role name bytes,
    `0x01 + "发送"`; if gold: `0x01+"gold_num="+gold+0x01+"金币"`;
    if items: (`"、"` when gold exists) `0x01+"item_id="+items_[0].item_.
    item_id_+0x01`; if `items_num_ > 1`, `"、"+0x01+"item_id="+` the 32-bit
    value at body+0x13 `+0x01+"等物品"`; then the fixed Chinese confirmation
    sentence. All literals reproduced byte-exactly (octal escapes).
  - `res = DPMsgPacker::send_captcha_request(user, url.c_str())`.
    On success: report `0x405`; `res = DPPkgDelayHacker::insert(user,
    method.waitting_response, 0x61, buf, len)`; on insert success set
    `captcha_state_ = 1`. On either failure call `cancel_mail_opt(user,
    0x61)`.
  - Bills `"captcha verify"` when `res == 0`, else `"captcha verify fail"`
    (line `0x3a`), with `{res, cmd_id, len, node_wait_for_send_,
    node_in_delay_, method.waitting_response}`. Returns 1.

`DPHackDnfMailHead` is 18 bytes nominal: `DPHackDnfCmdHead` 13, role-name
length at +13, role name at +17. `DPHackDnfMailBody` is 16 bytes: gold at +0,
item count at +4, union at +5 (11 bytes). The union holds `item_` at +5 and a
one-element `items_[1]` of `{char pad_; DPHackDnfMailItem item_;}`; the shipped
code indexes `items_[1]`, reading body+0x13.

### DPForbidModeHacker

- `init` registers `E_BAN_MODE` (+0x28) and returns 0.
- `hack`: only `type == 2` and `cmd_id` `0x14e00` or `0x14f00`.
  `game_mode = hack_data_.cf_user_info_.game_mode_` (+0x184).
  Maps it to an index: exact position in `cmd_hack.ban_mode.values[0..
  length_)`, otherwise `game_mode & 0x1f`.
  `keep_time` is the `time_t` reference `ban_mode.keep_time_[index]`
  (UserInfo +0x104 + index*4):
  - 0 -> return 0;
  - `<= time(NULL)` -> set to 0, return 0;
  - otherwise bill `"ban mode"` line `0x2b` with `{cmd_id, game_mode, index,
    keep_time, len}`, set `*buf` to the shipped 11-byte packet
    `f1 04 00 01 43 00 05 00 00 00 f2`, `*len = 11`, report `0x3ee`, return 0.
- `on_ban_mode`: `current_time = time(NULL)`; create bill `"recv ban mode"`
  line `0x2c`; for each `pkg.modes_[i]`, `i < pkg.count_`:
  map `info.mode_id_` to the same ban-mode index, compute
  `keep = info.keep_time_ ? current_time + info.keep_time_ : 0`, store it in
  `user->hack_data_.ban_mode_.keep_time_[index]`, and bill
  `{info.mode_id_, info.keep_time_}`. Then report `0x3ef` with `pkg.count_`
  and return 0.

### DPSafeModeHacker

- `init` registers `E_SAFE_MODE` (+0x58) and returns 0.
- `on_safe_mode`: logs `update safe mode, old=%u, new=%u` with the old
  `hack_data_.safemode_level_` (+0xdc) and the new level. If different, store
  the new level and, when `cmd_hack.safemode_to_client` (+0x3da1) is set,
  call `DPMsgPacker::send_safe_mode_notify(user, level)`. Returns 0.
- `hack`: returns 0 when `safemode_level_ == 0`. Otherwise, when
  `cmd_hack.safemode_hint_client` (+0x3da2) is set, call
  `DPMsgPacker::revive_client_mouse(user, cmd_id, E_TIPS_SAFEMODE /*1*/)`.
  Bill `"safe mode drop"` line `0x30` with `cmd_id`, report `0x408`, return 1
  (packet swallowed).

### DPUicTransHacker

- Has no `init` override (inherits `IDPHacker::init`).
- `hack` builds a `sec_proto::ReqUICTransAdaptMsgForwardPkg` (33044 bytes):
  `world_id_ = base_info_.room_id_`, `src_id_ = 0`, `cmd_id_ = cmd_id`,
  `need_encrypt_ = 1`, `is_encrypted_ = (type != 4)`,
  `cmd_buf_len_ = min(len, 0x8000)`, `cmd_buf_ = first cmd_buf_len_ bytes`,
  `rolename_len_ = 0x41`, `rolename_ = 0x41 bytes from base_info_.role_name_`,
  `client_ip_ = base_info_.client_ip_`.
- Logs `[%u][%s], forward to uic`.
- Inner `Comm_App_Frame`: `init_framehead(0x10000, 0, 0x57e41)`;
  `frame_uin_ = user uin`; services `send=(0x1f,0)`, `recv=(0x24,0)`,
  `proxy=(0,0)`; transaction/backfill 0; `app_id_ = glb_dp_config_head_->
  game_id`. If `inner_option_ & 0x10000`, calls
  `dumpoutput_framehead(frame, "[TRACK MONITOR][TRANS SEND]", RS_INFO)`.
  Then `appdata_encode<ReqUICTransAdaptMsgForwardPkg>(0xffbd, pkg, 0, 0)`.
  On failure: log `app data encode fail. ret=%d` with actual return and
  function name `generate_comm_frame`, then log
  `generate_comm_frame fail, ret=%d, entire=%d, cmd=%u` with
  `ret=0x139f, entire=0x24, cmd=0x57e41`, report `0x404`, return 0.
- Wraps the inner frame in `ReqSecsvrForwardData` (32773 bytes):
  `channel_id_ = Singleton<TssSdkCommSvc>::instance()->channel_id_`,
  `room_id_ = glb_dp_config_head_->world_id`,
  `data_len_ = inner->frame_length_`, `data_ = inner frame bytes`.
- Outer `Comm_App_Frame`: `init_framehead(0x10000, 0, 0x4c6ad)`;
  services `send=(0x2b,0)`, `recv=(0x1f,0)`, `proxy=(0,0)`; same uin/app_id;
  same optional dump. `appdata_encode<ReqSecsvrForwardData>(0xffbd, wrapper,
  0, 0)`; failure logs `send_ipc_pkg fail, res=%d, cmd=%u` with
  `res=0x139f, cmd=0x4c6ad`, reports `0x404`, returns 0.
- Sends `TssSdkCommSvc::send_buf_to_backend((const char*)outer,
  outer->frame_length_, true, 0)`. Failure logs
  `debug_errorex("[%s]send to backend fail. ret=%d", <template pretty name>,
  actual_ret)`, then the `send_ipc_pkg fail` message with `res=-2`; success
  logs `send_ipc_pkg success, cmd=%u` at trace level. Both paths report
  `0x404` and return 0. `*buf`/`*len` are never modified.

Because `init_framehead` clears the 32-bit frame-option word and only sets
bit 24, the `0x10000` bit tested before each dump is zero; the two dump calls
are present in ORIG but unreachable. The reconstruction performs the same
bit test (compiled to `test byte [frame+6],1`) rather than deleting it.

## Call targets / external boundaries

Defined here and verified against ORIG mangled names: all four classes'
`hack`/event methods, `init` overrides, `cancel_mail_opt`,
`is_permit_throught`, `on_timer`, destructor pairs, vtables/typeinfo, the
`std::list<DPCommandBuf>::erase` specialization and
`std::vector<IDPHacker*>::_M_insert_aux` (both weak, as in ORIG). The
`IDPHackerSafe` vtable/typeinfo/destructors are also emitted weakly (ORIG has
them); `DPPkgDelayHacker::on_logout/proc` are referenced from the mail vtable.

Intentionally external (resolved by the target library): `IDPHacker` base
methods/destructor, `DPPkgDelayHacker` constructors/destructor/`init`/
`on_logout`/`proc`/`on_special_control`/`insert` overloads/
`remove_from_delay`/`on_timer`, `DPBillAdapter` ctor/dtor and `bill_`,
global `CommBillRecord::operator<<(long long)`,
`ZEN_Trace_LogMsg::debug_output/debug_errorex`,
`ZEN_Thread_Mutex` ctor/dtor/lock/unlock, `Singleton<TssSdkCommSvc>::instance`,
`TssSdkCommSvc::send_buf_to_backend`, `DPMsgPacker` helpers,
`DPMonitorReporter::report_data`, `SERVICES_ID` ctor/dtor,
`Comm_App_Frame::{init_framehead,dumpoutput_framehead,appdata_encode<>}`,
`sec_proto::ClientPkgHead::unpack`, libc/libstdc++.

## Compile result

Built with the c5root GCC 4.1.2 toolchain. The c5root install needs its sysroot
and the 4.1.1 internal include directory on this host:

```
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 \
    --sysroot=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root \
    -isystem /home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/lib/gcc/x86_64-redhat-linux/4.1.1/include \
    -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/hackers_b_agent.cpp
```

Result: exit 0, no warnings under `-Wall -Wextra`.

Verification performed:

- `nm -S -C`: all ORIG mangled names for the assigned methods are emitted with
  the same `T/W/V` classes; vtable sizes are `0x40` (mail) and `0x3c`
  (forbid/safe/uic). Only extra local symbol is `s_forbid_mode_pkg` (static
  `.data`, local `d`); only ORIG-only symbols in these classes are the
  per-TU `_GLOBAL__I__...` static-initialization keys.
- Mangled-symbol diff for every undefined non-libc/libstdc++ reference found
  no name missing from the target.
- Byte comparison of all 36 referenced `.rodata` strings (including the GBK
  mail description fragments and the long `send_ipc_pkg` template name)
  against the target: exact match.
- `.data` packet for forbid mode: `f1 04 00 01 43 00 05 00 00 00 f2`, 11 bytes.
- Instruction-level review of field displacements, call argument order,
  monitor ids, bill line numbers and vtable slot order against ORIG.

Static layout assertions cover UserInfo/UserHackData, ZEN mutex,
IDPHackerSafe, DPPkgDelayHacker, DPMailVerifyHacker, IDPHackerEventNotify,
SERVICES_ID, Comm_App_Frame, both UIC packets, ClientPkgHead, DpBanModeReq,
DPHackDnfMailBody, DPConfig, DPCmdHack, DPBanMode, DP_CONFIG, DPCommandBuf
and the vptr-only hacker sizes. All pass.

## Unresolved / deliberately preserved ORIG behaviour

- The mail-body second-item id is read as a 32-bit value at `body+0x13`
  (`items_[1].item_.item_id_` with a declared one-element array). Preserved as
  an unaligned 4-byte read; the first item uses `items_[0].item_.item_id_`
  (`body+8`), the unreachable count==0 fallback uses `item_.item_id_`
  (`body+7`).
- `has_anything` checks `gold_num_`, `items_num_`, and `item_.item_posi_`
  (word at `body+5`), not a packed item array. Preserved.
- Mail length validation only guarantees 16 body bytes; `items_num_ > 1`
  reads past that. Preserved.
- `is_permit_throught` compares `max_timeout_count` against
  `captcha_no_response_` (+0x54) but increments/tests `captcha_counts_`
  (+0x58). This asymmetry is part of the shipped logic and is preserved.
- Role-name copy uses 0x41 bytes from a 64-byte `role_name_` array, so one
  byte of `user_code_info_` is copied into the UIC rolename field. Preserved.
- `DPBillAdapter::bill_` is typed `DPBillRecorder` in DWARF; in these TUs every
  use resolves to global `CommBillRecord::operator<<(long long)` on the same
  static object address. The reconstruction declares the static member as
  `CommBillRecord` to reproduce the exact call target; object address and
  argument sequence match ORIG.
- `DPForbidModeHacker::set_ban_mode` (DWARF-declared inline, no out-of-line
  symbol) is not emitted separately; its config-index mapping and keep-time
  store are inlined exactly into `on_ban_mode`/`hack`.
- The four TUs' `_GLOBAL__I__...` constructors (iostream init and ZEN status
  item registrations pulled in by framework headers) are not reproduced; they
  contain no hacker logic. All method/vtable/typeinfo symbols otherwise match.
- `DPPkgDelayHacker`/`IDPHacker` base implementations, including the virtual
  `on_timer` slot appended at vtable offset `+0x34`, are external boundaries.

Semantic verdict: all four classes' guards, state-field offsets, protocol
field reads/writes, bill/monitor side effects, return values and vtable/call
targets match the shipped `libtss_sdk.so.1.0.394`; no fake success paths were
introduced.
