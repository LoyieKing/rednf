# AGENT_REPORT_hackers_c

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/hackers_c_agent.cpp`

Semantic reconstruction only; no byte-identity claim, no commit. All out-of-line
call targets are real ORIG symbols (no fake send/log/allow functions).

## 1. ORIG symbols and ranges

| ORIG address | size | symbol |
|---|---|---|
| `0x000a5680` | `0x106` | `DPCfRoomQtalkHacker::notify_client(UserInfo*, Dp2QtMsg const&)` |
| `0x000a5790` | `0x0e5` | `DPCfRoomQtalkHacker::on_login(UserInfo*)` |
| `0x000a5880` | `0x73a` | `DPCfRoomQtalkHacker::hack(UserInfo*, int, unsigned int, uchar*&, uint&)` |
| `0x000a6550` | `0x075` | `DPCfRoomQtalkHacker::init(IDPHackerEventNotify*)` (weak) |
| `0x000a63d0` / `0x000a65d0` | `0x039` / `0x029` | `DPCfRoomQtalkHacker::~...` D0/D1 (weak) |
| `0x000b87a0` | `0x075` | `DPPropertyTransHacker::init(IDPHackerEventNotify*)` |
| `0x000b9f20` | `0x04b` | `DPPropertyTransHacker::hack(...)` |
| `0x000ba870` / `0x000ba8b0` | `0x039` / `0x029` | `DPPropertyTransHacker::~...` D0/D1 (weak) |
| `0x000a99f0` | `0x123` | `DPPkgDelayHacker::init(IDPHackerEventNotify*)` (weak) |
| `0x000b4eb0` | `0x147` | `DPPkgDelayHacker::on_timer(DPCommandBuf&)` |
| `0x000b5000` | `0x0ff` | `DPPkgDelayHacker::insert(DPCommandBuf&)` |
| `0x000b5100` | `0x07f` | `DPPkgDelayHacker::remove_from_delay(UserInfo*, _List_iterator&)` |
| `0x000b5180` | `0x179` | `DPPkgDelayHacker::on_logout(UserInfo*)` |
| `0x000b5300` | `0x15a` | `DPPkgDelayHacker::proc(long)` |
| `0x000b5460` | `0x17b` | `DPPkgDelayHacker::insert(UserInfo*, int, uint, uchar*, uint&)` |
| `0x000b55e0` | `0x23a` | `DPPkgDelayHacker::on_special_control(...)` |
| `0x000b5c30` / `0x000b5da0` | `0x16b` each | `DPPkgDelayHacker::DPPkgDelayHacker()` C1/C2 |
| `0x000b6480` / `0x000b6530` | `0x0a2` / `0x0aa` | `DPPkgDelayHacker::~...` D1/D0 (weak) |

Related DWARF/real boundaries decoded: `IDPHackerSafe` (32 bytes), `DPSafeModeHacker`
(4), `DPPropertyTransImpl` (`sizeof == 0x38`, global `propety_impl` at
`0x002c0aa0`), `ZEN_Thread_Mutex` (28), `Dp2QtMsg` (266), `ClientPkgHead` (12).

## 2. Layouts

`tss_sdk_impl::UserInfo` is 400 bytes. Relevant fields (all DWARF-exact):

```
base_info_ +0x00: uin_ +0, game_id_ +4, room_id_ +8, client_ip_ +0xc,
                  server_ip_ +0x10, game_usr_id_ +0x14, client_version_ +0x18,
                  client_user_id_ +0x1c, flags_ +0x20, role_name_[64] +0x24
hack_data_ +0xdc (220): safemode_level_ +0, dnf_client_uid_ +4,
                  drop_cmd_[8] +8, ban_mode_[128] +40,
                  cf_user_info_ +168, delay_cmd_ +176, captcha_state_ +179
cf_user_info_: game_mode_ +0, role_posi_ +1, role_type_ +2, game_map_ +3,
               channel_id_ +4, room_id_ +6
delay_cmd_:    delay_mode_[3] +0
```

Absolute offsets used by the code: `user+0x184` game_mode, `+0x185` role_posi,
`+0x186` role_type, `+0x188` channel_id, `+0x18a` room_id, `+0x18c..0x18e`
delay flags, `+0xdc` safemode_level.

`DPPkgDelayHacker` (84 bytes, base `IDPHackerSafe`):

```
+0x00 vptr (IDPHackerSafe / IDPHacker base; 32-byte base object)
+0x04 ZEN_Thread_Mutex locker_       (28)
+0x20 std::map<unsigned int, DPCommandBuf2> waitting_for_send_ (24)
+0x38 std::list<DPCommandBuf> cmd_delay_                        (8)
+0x40 timeval next_timer_                                      (8)
+0x48 int flag_position_
+0x4c unsigned int node_wait_for_send_
+0x50 unsigned int node_in_delay_
```

`DPCommandBuf` (24): `cmd_id_ +0`, `buf_len_ +4`, `buf_ +8`, `user_ +0xc`,
`expire_time_ (timeval) +0x10`.
`DPCommandBuf2` (12): `cmd_id_ +0`, `buf_len_ +4`, `buf_ +8`.
The class is abstract in ORIG: its `hack` vtable slot is `__cxa_pure_virtual`.
`on_timer` is an extra virtual slot at vptr `+0x34` (vtable `+0x3c`).

`DPCfRoomQtalkHacker` and `DPPropertyTransHacker` are 4 bytes (vptr only).
`DPSafeModeHacker` is 4 bytes; ORIG constructs one on the stack inside
`DPPkgDelayHacker::on_special_control`.

`sec_proto::Dp2QtMsg` (266, packed): `cmd_id` (uint8) +0, body union +1.
Body union size 265; variants used here:

```
Login           uin +0, server_ip +4, name_len +8, name[256] +9 (265)
EnterChannel    channel_id +0 (1)
CreateRoom      room_type +0, visitor_count +1 (2)
CreateRoomRes   room_id +0 (2)
EnterRoom       room_id +0, room_type +2, visitor_count +3,
                role_type +4, role_position +5 (6)
ChangePosition  role_position +0, role_type +1 (2)
RoleAction      role_action +0, revert +1 (5)
ChangeRoomType  room_type +0 (1)
```

Command ids: 1 Login, 2 EnterChannel, 3 CreateRoom, 4 CreateRoomRes,
5 EnterRoom, 7 ChangePosition, 8 RoleAction, 10 ChangeRoomType.

`ClientPkgHead` (12, packed): `protocol_id_ +0`, `seq_ +2`, `sln_id_ +6`,
`sub_sln_id_ +8`, `body_len_ +10`.

`tss_dp_comm` prefix: `DPConfig::cmd_hack` at `+0x3d7e`; inside `DPCmdHack`,
`delay_cmd` +0, `safemode_to_client` +0x23, `safemode_hint_client` +0x24,
`cf_room_qtalk_enable` +0x25. `DPDelayCmd`: `delay_cmd_time` +0,
`safemode_drop_cmdlist` +1. `DPValueArray_8`: uint16 `length` +0,
uint32 `values[8]` +2 (34 bytes). Absolute config offsets read by the code:
`+0x3da3` (qtalk enable) and `+0x3d7f` (safemode drop list).

## 3. Decoded behavior

### DPCfRoomQtalkHacker

`init`: pushes `this` into `event_notify->hackers_[E_LOGIN]` (vector at
`event_notify+0x04`) with the ORIG de-duplicating scan; returns 0.

`notify_client`: packs the message at buffer offset 12 with capacity `0xfff3`
(`Dp2QtMsg::pack(buf+12, 0xfff3, &len, 0)`). On pack failure logs
`RS_ERROR "[%u][%s], pack fail, res=%d"` (uin is 0 when `user == NULL`) and
returns the pack error. On success builds `ClientPkgHead`:
`protocol_id_=0x33`, `sln_id_=0x0d`, `sub_sln_id_=0`, `body_len_=(uint16)len`;
`seq_` is deliberately uninitialised. `head.pack(buf, 0xffff, 0, 0)` is called
with its return ignored, then
`DPMsgPacker::send_msg_to_client(0, user, buf, (uint16)(len+12))` is returned.

`on_login`: returns 0 immediately when `cmd_hack.cf_room_qtalk_enable == 0`.
Otherwise logs `"[%u][%s], login info, role_name_=%s, server_ip_=%u"` at
`RS_INFO` when `flags_&1`, else `RS_DEBUG`; builds `Dp2QtMsg{cmd_id=1}` with
`uin_`, `server_ip_`, `name_len=(uint8)(strlen(role_name_)+1)` and copies that
truncated byte count into `Login::name`; calls `notify_client`, ignores its
return, returns 0.

`hack`: returns 0 when the qtalk config byte is 0. Type/command table:

```
type 1:
  0x13e00, 0x53e00            -> cmd 10 ChangeRoomType; room_type=game_mode_
  0x11f00, 0x56600            -> cmd 2  EnterChannel; channel_id=channel_id_
  0x13400                     -> cmd 3  CreateRoom after *len > 0x1d
                                 room_type=game_mode_, visitor=buf[0x1e]>>1
  0x56e00                     -> cmd 3  CreateRoom after *len > 0x35
                                 room_type=game_mode_, visitor=buf[0x36]>>1
  other                       -> return 0
type 2:
  0x12800,0x51300,0x13d00     -> cmd 7 ChangePosition
                                 role_position=role_posi_, role_type=role_type_
  0x13500,0x56f00             -> cmd 4 CreateRoomRes; room_id=room_id_
  0x13700,0x50900,0x14a00     -> cmd 5 EnterRoom after *len > 0x0d
                                 room_id, room_type=game_mode_,
                                 visitor = buf[0x0e]>>1, except 0x50900 uses
                                 arithmetic (16-buf[0x0c])/2,
                                 role_type=role_type_, role_position=role_posi_
  0x14e00                     -> JOIN_GAME; requires flags_&8;
                                 cmd 8 RoleAction{6, revert=0}
  0x14f00                     -> START_PLAY after *len > 6;
                                 if (uint8)(buf[7]-1) <= 0xfd return 0;
                                 requires flags_&8; cmd 8 RoleAction{6,0}
  0x15b00                     -> STOP_PLAY; cmd 8 RoleAction{4,0}
  0x14100,0x50b00             -> EXIT_ROOM; cmd 8 RoleAction{5,0}
  other                       -> RS_ERROR unknown-msg log, return 0
other type                     -> notify_client with the uninitialised local msg
```

Every built message is logged at `RS_INFO`/`RS_DEBUG` selected by `flags_&1`,
then `notify_client` is called (return ignored) and `hack` returns 0.
Length failures log
`"[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, expected_len=%u"`
at `RS_ERROR` and return -1 (`expected_len`: 0x1e, 0x36, 0x0e, 7).

### DPPropertyTransHacker

Thin facade over the namespace-scope `tss_dpsdk::propety_impl` object
(`0x002c0aa0`, `DPPropertyTransImpl`, `sizeof == 0x38`).

`init`: registers `&propety_impl` in `hackers_[E_LOGOUT]`
(`event_notify+0x10`), not `this`; returns 0.
`hack`: calls `propety_impl.hack(user,type,cmd_id,buf,len)` directly, discards
its return value and returns 0 (the ORIG has `xor eax,eax` after the call).

### DPPkgDelayHacker

Constructor: initialises the `IDPHackerSafe` base (vptr + `ZEN_Thread_Mutex(0)`),
then the map/list/next_timer/counters. A function-local `static int flags`
(the exact symbol is `DPPkgDelayHacker::DPPkgDelayHacker()::flags`) supplies
`flag_position_ = flags++`; if the slot index is > 2 it does `throw index;`
(typeinfo is `_ZTIi`). Thus only three delay-derived hacker instances may exist
process-wide, matching the three created by `DPHackMgr::init`.

`init`: registers `this` in `E_LOGOUT`, `E_PROC` and `E_SPECIAL_CONTROL`
(vectors at `event_notify+0x10`, `+0x1c`, `+0x64`); returns 0.

`insert(UserInfo*, delay_seconds, cmd_id, buf, len)`:
1. Logs `"[%u][%s], before set_flag %u"` using the current
   `delay_mode_[flag_position_]` **before** validating the position.
2. Returns -1 if `flag_position_ > 2` or `E_IN_DELAY` is already set.
3. Sets `E_IN_DELAY`, logs `"[%u][%s], after set_flag %u"` with the new byte.
4. Builds `DPCommandBuf`: cmd id, user pointer, `gettimeofday`, then
   `expire_time_.tv_sec += delay_seconds`, `buf_len_ = len`,
   `buf_ = new unsigned char[len]`, copies the payload.
5. Locks the mutex, calls `insert(DPCommandBuf&)`, unlocks, returns its result.

`insert(DPCommandBuf&)`: `++node_in_delay_`, then keeps `cmd_delay_` sorted by
`expire_time_`. If the new entry is earlier than `next_timer_` (or the list is
empty), it updates `next_timer_` and `push_front`; otherwise it inserts before
the first strictly later node. Always returns 0.

`on_timer(DPCommandBuf&)`:
1. `ret = DPMsgPacker::send_special_control(buf.user_, (int)buf.cmd_id_)`;
   if non-zero return it (no state change).
2. `DPCommandBuf2 &p = waitting_for_send_[(unsigned int)buf.user_]` (key is the
   `UserInfo*` pointer bits; ORIG map key type is `unsigned int`).
3. If the previous `p.buf_` is non-NULL, scalar-delete it; if NULL,
   `++node_wait_for_send_`.
4. Transfer ownership: `p.buf_ = buf.buf_; buf.buf_ = 0;`
   `p.buf_len_ = buf.buf_len_; buf.buf_len_ = 0; p.cmd_id_ = buf.cmd_id_`.
5. If `flag_position_ <= 2`, set `E_IN_SPECIAL_CONTROL` in
   `delay_mode_[flag_position_]` when not already set. Return ret (0).

`proc(time_t now)`: early-return 0 when `next_timer_.tv_sec > now` or
`cmd_delay_` is empty. Calls `gettimeofday`; returns 0 if the real time is
before `next_timer_`. Locks, then while the list head is expired
(sec/usec `>=`), it copies the payload, calls the virtual `on_timer` (return
ignored), scalar-deletes the local `buf_`, erases the list node, decrements
`node_in_delay_`, clears `E_IN_DELAY` for the saved user, and continues. For the
first non-expired head it updates `next_timer_` and stops. Unlocks, returns 0.

`on_logout(UserInfo*)`: returns 0 if `flag_position_ > 2`; clears
`E_IN_DELAY`/`E_IN_SPECIAL_CONTROL` in `delay_mode_[slot]`, remembering which
were set. If neither was set, returns 0 without locking. Otherwise locks:
for the delay bit, scans `cmd_delay_` for the first payload whose `user_`
equals the argument and calls `remove_from_delay`; for the control bit, finds
`waitting_for_send_[(unsigned int)user]`, scalar-deletes its `buf_`, erases the
node, decrements `node_wait_for_send_`. Unlocks, returns 0.

`remove_from_delay(UserInfo*, iterator&)`: scalar-`delete (*it).buf_`, erases
the list node, decrements `node_in_delay_`, clears `E_IN_DELAY` for the user if
set, returns 0.

`on_special_control(user, buf, buf_len, out, out_len)`: returns 0 if
`flag_position_ > 2` or `E_IN_SPECIAL_CONTROL` is clear. Clears that bit, locks,
finds the map entry by user pointer; on miss returns 0 (bit remains cleared).
Saves the current `buf_len_` before the safemode filter and returns 0 if it is
larger than `out_len`. If `user->hack_data_.safemode_level_ != 0` and the saved
cmd id is present in
`cmd_hack.delay_cmd.safemode_drop_cmdlist.values[0..length)`, it constructs a
4-byte `DPSafeModeHacker` temporary and calls
`DPSafeModeHacker::hack(user, 1, cmd_id, &pending.buf_, &pending.buf_len_)`
directly. It then writes the **saved original length** to `out_len`, copies
that many bytes from `pending.buf_` (which may have been replaced by the
safemode filter), scalar-deletes `pending.buf_`, erases the map entry,
decrements `node_wait_for_send_`, unlocks, and returns 1. The mutated
`pending.buf_len_` from the safemode filter is intentionally not used for the
copy length, matching ORIG.

Memory-management quirks reproduced exactly: packets are allocated with
`new unsigned char[]` but released with scalar `operator delete`; the
`DPPkgDelayHacker` destructor destroys only the list/map nodes and leaks any
owned payload buffers.

## 4. Object/callback boundaries

| Reference | Target |
|---|---|
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | PLT `0x873b4` |
| `DPMsgPacker::send_msg_to_client(int, UserInfo*, uchar*, ushort)` | `0x000b1920` |
| `DPMsgPacker::send_special_control(UserInfo*, int)` | `0x000b1d10` |
| `sec_proto::Dp2QtMsg::pack(char*, uint, uint*, uint) const` | `0x00164810` |
| `sec_proto::ClientPkgHead::pack(char*, uint, uint*, uint) const` | `0x0015be00` |
| `DPSafeModeHacker::hack(...)` | `0x000bb2f0` |
| `DPPropertyTransImpl::hack(...)` | `0x000b96e0`; global `propety_impl` `0x002c0aa0` |
| `ZEN_Thread_Mutex::{ctor,dtor,lock,unlock}` | zenlib, global class |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | GOT `0x2943ac` → `0x002bb4c0` |
| `IDPHacker` default virtuals / dtor | weak ORIG symbols `0xa4770..0xa4840` |
| `operator new/new[]` / `operator delete` | PLT symbols |

The reconstruction emits all assigned mangled symbols exactly, including
`remove_from_delay(UserInfo*, std::_List_iterator<DPCommandBuf>&)`, the weak
`init`/destructor symbols, the local ctor static
`DPPkgDelayHacker::DPPkgDelayHacker()::flags`, and the four `__func__` strings.
The `DPPkgDelayHacker` vtable is 0x40 bytes with the same slot order as ORIG:
`init, hack(pure), on_login, on_logout, proc, on_ban_mode, on_drop_cmd,
on_verify_response, on_verify_msg, on_safe_mode, on_special_control,
~D1, ~D0, on_timer`.

The compiler devirtualises the `locker_.lock()/unlock()` calls to direct
`ZEN_Thread_Mutex::lock/unlock` because the member's dynamic type is exactly
`ZEN_Thread_Mutex`. ORIG calls the same final functions through the mutex vptr
(`+0x08`/`+0x10`); there is no derived override for this member, so the
observable target is identical.

## 5. Compile result

Compiler: c5root GCC 4.1.2 20070115 SUSE (target compiler generation).

```
/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++ \
  -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/hackers_c_agent.cpp \
  -o /tmp/hackers_c_agent.o
```

Result: `EXIT 0`, no diagnostics. `-Wall` also emits no diagnostics. The same
source compiles with WSL system `g++ -m32 -std=gnu++98 -O2 -fPIC
-fno-implement-inlines -fno-stack-protector -D_GLIBCXX_USE_CXX11_ABI=0`
(`EXIT 0`).

In-unit layout checks (typedefs) assert: `UserInfo==400` with `hack_data_`
`+220`, `cf_user_info_ +388`, `delay_cmd_ +396`; `Dp2QtMsg==266` with body
`+1`, `Dp2QtMsgBody/Login==265`; `ClientPkgHead==12`; `EnterRoom==6`;
`RoleAction==5`; `DPValueArray_8==34`; `DPDelayCmd==35`; `DPCmdHack::
cf_room_qtalk_enable +0x25`; `DPConfig::cmd_hack +0x3d7e`;
`DPCommandBuf==24`; `DPCommandBuf2==12`; `ZEN_Thread_Mutex==28`;
`IDPHackerSafe==32`; `DPPkgDelayHacker==0x54`; the three facade/temp classes
are 4 bytes.

## 6. Unresolved / deliberate externals

- `DPPropertyTransImpl`'s body (0x38-byte object, `forward_trade_data_to_safemode`,
  `on_logout`, constructors) is outside this unit; the facade's direct call and
  global registration are reconstructed.
- `DPSafeModeHacker::hack`, `DPMsgPacker::{send_msg_to_client,
  send_special_control}`, the two protocol `pack` methods, `ZEN_Thread_Mutex`
  internals and `IDPHacker` default virtuals are real external symbols.
- `ClientPkgHead.seq_` and the `hack()` "other type" `Dp2QtMsg` local are
  intentionally left uninitialised, as in ORIG.
- `new[]` / scalar-`delete` mismatch and the destructor's payload-buffer leak
  are preserved.
- No fake send or fabricated success path exists; all returns follow ORIG.
