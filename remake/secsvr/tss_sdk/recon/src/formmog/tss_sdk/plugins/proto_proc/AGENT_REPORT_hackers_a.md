# AGENT_REPORT_hackers_a

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/hackers_a_agent.cpp`

Semantic reconstruction only: no byte-identity claim, no commit. All addresses
are virtual addresses in the target. `nm -S`, `objdump -d`, DWARF member
offsets and DWARF source line tables were used; every call target below is the
exact symbol the original instruction sequence references.

## Symbols and original ranges

| ORIG address | Size | Symbol |
|---|---|---|
| `0x000a66a0` | `0x1d8` | `DPCmdDropHacker::hack(UserInfo*, int, unsigned, unsigned char*&, unsigned int&)` |
| `0x000a6880` | `0x175` | `DPCmdDropHacker::on_drop_cmd(UserInfo*, const sec_proto::DpDropCmdlistReq&)` |
| `0x000a6e40` | `0x81` W | `DPCmdDropHacker::init(IDPHackerEventNotify*)` |
| `0x002baea0` | `4` V | `DPCmdDropHacker::init(...)::inited` |
| `0x000b6680` | `0x1cd` | `DPPkgDropHacker::hack(...)` |
| `0x000b6d70` | `0x73` | `DPPkgDumpHacker::hack(...)` |
| `0x000a9d30` | `0x133` | `DPFilterForwardLaHacker::hack(...)` |
| `0x000b7720` | `0x6e6` | `DPPkgForwardLaHacker::hack(...)` |
| `0x000a9310` | `0x178` | `DPDelayHacker::hack(...)` |
| `0x000b9e90` | `0x8a` | `DPPropertyDelayHacker::hack(...)` |
| `0x000b88b0` | `0x17a` | `DPPropertyDelayHacker::init(IDPHackerEventNotify*)` |

All seven vtables, typeinfos, typeinfo names and weak D1/D0 destructors were
emitted with the exact ORIG mangled names. The reconstructed `.o` defines the
same twelve out-of-line methods plus the weak `inited` guard; there are no
invented methods or fake allow/deny paths.

## Class layout (DWARF-exact)

```
IDPHacker (4, vptr only; 11 virtual slots + D1/D0, order in header below)
IDPHackerSafe : IDPHacker        32 = vptr + ZEN_Thread_Mutex locker_ (+4, 28)
DPPkgDelayHacker : IDPHackerSafe 84 = +0x20 map waitting_for_send_ (24)
                                        +0x38 list cmd_delay_ (8)
                                        +0x40 timeval next_timer_ (8)
                                        +0x48 int flag_position_
                                        +0x4c uint node_wait_for_send_
                                        +0x50 uint node_in_delay_
DPDelayHacker : DPPkgDelayHacker 84  (no members)
DPPropertyDelayHacker : DPDelayHacker 84 (no members)
DPPropertyTransImpl : IDPHackerSafe 56 (+32 map uid2usr_)
DPPkgForwardLaHacker : IDPHacker 4
DPFilterForwardLaHacker : DPPkgForwardLaHacker 4
DPPkgDropHacker : IDPHacker 4
DPPkgDumpHacker : IDPHacker 4
DPCmdDropHacker : IDPHacker 8  (int cur_hacker_pos_ at +4)
```

`IDPHacker` vtable slots used by the helpers:
`+0x00 init`, `+0x04 hack`, `+0x08 on_login`, `+0x0c on_logout`,
`+0x10 proc`, `+0x14 on_ban_mode`, `+0x18 on_drop_cmd`,
`+0x1c on_verify_response`, `+0x20 on_verify_msg`, `+0x24 on_safe_mode`,
`+0x28 on_special_control`, `+0x2c/+0x30 destructors`.
`DPPkgDelayHacker` derives `IDPHackerSafe` (verified by DW_AT_inheritance), not
directly `IDPHacker`; the +0x4c/+0x50 counters are `node_wait_for_send_` and
`node_in_delay_`. `DPFilterForwardLaHacker` derives `DPPkgForwardLaHacker`.

The DPPkgDelayHacker vtable is 0x40 (14 function slots): it overrides
`init` (0x000a99f0), `on_logout` (0x000b5180), `proc` (0x000b5300) and
`on_special_control` (0x000b55e0), and appends the new virtual
`on_timer(DPCommandBuf&)` (0x000b4eb0) after the D1/D0 pair at +0x34/+0x38
(vptr-relative +0x3c). The reconstruction declares all of these without
defining them, so the emitted DPDelayHacker and DPPropertyDelayHacker vtables
are slot-for-slot identical to ORIG (including the extra on_timer slot and the
0x40 size) while the bodies remain real external symbols. DPPropertyDelayHacker
overrides only `init` and `hack`; the other delay slots stay inherited.

## UserInfo fields used

`UserInfo` is 400 bytes. `base_info_` at +0: `uin_ +0`, `game_id_ +4`,
`room_id_ +8`, `client_ip_ +12`, `server_ip_ +16`, `game_usr_id_ +20`,
`client_version_ +24`, `client_user_id_ +28`, `flags_ +32` (bit 0 = trace),
`role_name_ +36[64]`. `hack_data_` at +0xdc; `hack_data_.drop_cmd_` is
`time_t[8]` at UserInfo+0xe4 (the array the eight `DPCmdDropHacker` instances
index).

## Config (DPConfig / DPCmdHack) offsets

`DPConfig::cmd_hack_` is at +0x3d7e; `DPCmdHack` is 7100 bytes.
`DPSubCmdList` is packed, 42 bytes:
`is_accept_ +0`, `cmd_id_type_ +1`, `dp_subcmd_hack_ +2` (2-byte
`DPGameCmdStruct`), `cmd_id_ +4`, `subcmd_list_ +8` (`DPValueArray_8`, 34
bytes: `uint16 length`, `uint32 values[8]` at +2).
`DPDropCmd` is packed, 42 bytes: `length +0`, `values[8] +2`, `actions[8] +34`.
`DPHackFilter` is packed, 2050 bytes; `DPHackFragment` is 16 bytes.

| absolute offset | member |
|---|---|
| `0x3d7e` | `cmd_hack_.delay_cmd_.delay_cmd_time` (uint8) |
| `0x55ba` | `cmd_hack_.drop_subcmd_count_` (uint16) |
| `0x55bc` | `cmd_hack_.drop_subcmd_list_[8]` |
| `0x570c` | `cmd_hack_.forward_subcmd_count_` (uint16) |
| `0x570e` | `cmd_hack_.forward_subcmd_list_[8]` |
| `0x58e0` | `cmd_hack_.drop_cmd_` |
| `0x5902` | `cmd_hack_.drop_cmd_.actions[0]` |

All offsets, `sizeof(DPConfig)==22842` and `sizeof(DPCmdHack)==7100`, plus the
UserInfo, `DpDropCmdlistReq` (808), `DpDropCmdlistInfo` (8) and
`LATransAdaptDPCmdBufferMonitor` (32786; fields at 0/4/8/12/13/14/18) are
compile-time asserted.

## Method semantics

### `DPCmdDropHacker::hack` (0xa66a0)

```
pos       = cur_hacker_pos_;
keep_time = &user->hack_data_.drop_cmd_[pos];      // user+0xe4+pos*4
if (keep_time == 0) return 0;
trace(priority, "[%u][%s], cur_hacker_pos_=%u, keep_time=%lu",
      uin or 0, __func__, pos, keep_time);
if (time(0) >= keep_time) { keep_time = 0; return 0; }
bill "drop cmd", log_type = uin + 1000, user;
bill << type << cmd_id << len << keep_time << uin;
report_data(0x409 + pos, 1);
action = config->cmd_hack_.drop_cmd_.actions[pos];
if (action != 0) revive_client_mouse(user, cmd_id, action - 1);
return 1;
```

Priority is `RS_INFO` iff `user != 0 && (user->base_info_.flags_ & 1)`, else
`RS_DEBUG`. The bill/report/revive run while the bill adapter is still alive
(its destructor is last). There is no user-null guard: the first array access
dereferences `user`, as in ORIG. Buffer and length are not modified by this
hacker.

### `DPCmdDropHacker::on_drop_cmd` (0xa6880)

```
if (pkg.count_ == 0) return 0;
now = time(0);
bill "recv drop cmd", log_type 45, user;
for (i = 0; i < pkg.count_; ++i) {
    cmd  = pkg.cmd_lists_[i].cmdlist_id_;
    keep = pkg.cmd_lists_[i].keep_time_;
    index = cmd & 7;
    if (config->drop_cmd_.length != 0) {
        linear scan config->drop_cmd_.values[0..length-1] for cmd;
        hit -> index = matched position; no hit -> index = cmd & 7;
    }
    user->hack_data_.drop_cmd_[index] = keep ? now + keep : 0;
    bill << cmd << keep;
}
report_data(0x413, pkg.count_);          // before bill destruction
return 0;
```

No configured position clamping beyond the 8-entry `values`/`actions` arrays
and no `pkg.count_ <= 100` clamp are added; ORIG has neither.

### `DPCmdDropHacker::init` (0xa6e40, inline)

```
static int inited = 0;                   // weak V symbol, 4 bytes
if (!inited) {
    event_notify->register_event(E_DROP_CMD, this);  // vector at +0x34
    inited = 1;
}
return 0;
```

`register_event` is the header-inline de-duplicating append (scan vector; push
only if absent). The eight DPHackMgr-created instances therefore register only
the first one.

### `DPPkgDropHacker::hack` (0xb6680)

`sub_cmd_id` starts at 0. `count` is `drop_subcmd_count_` treated as signed
(only zero is special).

For each `drop_subcmd_list_[i]`:
1. skip unless `is_match_filter(entry, type, cmd_id)`:
   `cmd_id == entry.cmd_id_` and
   `(entry.cmd_id_type_ & (uint8)type) == (uint8)type`.
2. `exists = get_cmd_id(entry.dp_subcmd_hack_, buf, (uint16)len,
   &sub_cmd_id) == 0 && is_value_exist(entry.subcmd_list_, sub_cmd_id)`.
   `is_value_exist` is the sorted early-exit membership test
   (length 0 -> false; `< values[0]` -> false; equal -> true; repeat).
3. If `exists == (entry.is_accept_ != 0)`, go to the drop path; otherwise
   return 0 immediately.

If `count == 0`, no entry matched, or the loop ends, control also reaches the
drop path. Drop path:

```
bill "drop package", log_type 49, user;
bill << type << cmd_id << sub_cmd_id << len;
report_data(0x402, 1);
return 1;
```

So an `is_accept_ = 1` entry selects its listed sub-commands, and a
`is_accept_ = 0` entry selects the complement; a packet with no matching
cmd entry is selected (dropped). The buffer pointer/length are untouched.

### `DPPkgDumpHacker::hack` (0xb6d70)

```
char name[256];
snprintf(name, 256, "DPPkgDumpHacker, type=%d,cmd_id=0X%X", type, cmd_id);
DPBillAdapter::dump(name, user->base_info_.uin_, buf, len);
return 0;
```

No config, no report, no buffer modification.

### `DPFilterForwardLaHacker::hack` (0xa9d30)

Same predicate as `DPPkgDropHacker` but over `forward_subcmd_list_`:

* `forward_subcmd_count_ == 0` -> forward.
* For each entry, `is_match_filter` then `is_subcmd_exist`.
* `exists == (entry.is_accept_ != 0)` -> forward; otherwise return 0.
* If no entry matches, fall through to forward.

`forward` is a non-virtual qualified call to
`DPPkgForwardLaHacker::hack` with the unchanged arguments, and its return value
is returned. A filtered-out packet returns 0 (not intercepted and not logged by
this class).

### `DPPkgForwardLaHacker::hack` (0xb7720)

```
LATransAdaptDPCmdBufferMonitor pkg;
pkg.room_id_         = user->base_info_.room_id_;   // user+8
pkg.time_stamp_      = (uint32)time(0);
pkg.monitor_cmd_id_  = (int32)cmd_id;
pkg.is_need_encrypt_ = 1;
pkg.is_encrypted_    = (type != 4);
pkg.data_len_        = len;                          // original length
if (len != 0) {
    copy_len = len; if (copy_len > 0x8000) copy_len = 0x8000;
    memcpy(pkg.data_, buf, copy_len);
}
trace(priority, "[%u][%s], forward pkg to la", uin, __func__);
DPMsgPacker::send_datatrans_pkg(user, 0x23, pkg, 0x5573f);
report_data(0x403, 1);
return 0;
```

The clamp is local only: `data_len_` keeps the original length even when it
exceeds 32768, and the caller's `len` is not written. ORIG has no user-null
branch before `user->room_id_`, so the null case faults on the first
assignment. The send result is consumed only by the template's own error logs;
the hacker always reports and returns 0 (never intercepts the original packet).

ORIG inlined `DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor>`;
the reconstruction calls the real weak instantiation at `0x000b80d0`
(`_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto30LATransAdaptDPCmdBufferMonitorEEEiPN12tss_sdk_impl8UserInfoEiRKT_j`),
whose body is owned by the packer-templates reconstruction. `entire = 0x23`
and frame command `0x5573f` are the exact ORIG immediates.

### `DPDelayHacker::hack` (0xa9310)

```
if (type != 1) return 0;
delay_time = config->cmd_hack_.delay_cmd_.delay_cmd_time;
res = DPPkgDelayHacker::insert(user, delay_time, cmd_id, buf, len);
bill(res == 0 ? "delay cmd" : "delay cmd fail", log_type 55, user);
bill << cmd_id << len << node_wait_for_send_ << node_in_delay_ << delay_time;
return res == 0 ? 1 : 0;
```

A successful insert means the packet was copied into the delay queue, so the
original is intercepted (`1`); insert failure returns 0. The bill adapter is
destroyed before the return value is produced. `insert` is the external
`0x000b5460` boundary; `node_wait_for_send_`/`node_in_delay_` are the +0x4c and
+0x50 counters of `DPPkgDelayHacker`.

### `DPPropertyDelayHacker::hack` (0xb9e90)

```
res = propety_impl.DPPropertyTransImpl::hack(user, type, cmd_id, buf, len);
if (res != 0) res = DPDelayHacker::hack(user, type, cmd_id, buf, len);
return res;
```

The call is a direct (qualified, non-virtual-dispatch) call to the
`DPPropertyTransImpl::hack` symbol; the default delay behaviour is the
fallback. `propety_impl` is the TU-local static object at `0x002c0aa0`
(56 bytes, local symbol `_ZN9tss_dpsdk12propety_implE`); the reconstruction
declares it extern and does not fabricate its body.

### `DPPropertyDelayHacker::init` (0xb88b0)

```
event_notify->register_event(E_LOGOUT, &propety_impl);        // vector +0x10
event_notify->register_event(E_PROC, this);                   // vector +0x1c
event_notify->register_event(E_SPECIAL_CONTROL, this);        // vector +0x64
return 0;
```

This override does not call `DPPkgDelayHacker::init`: logout is delegated to
the static property implementation, while proc/special-control stay on this
object. The registration order and event vector offsets are byte-matching with
ORIG. `register_event` de-duplicates before appending.

## External boundaries / call targets

| Reference in unit | Target |
|---|---|
| `DPBillAdapter::DPBillAdapter` / `~DPBillAdapter` / `dump` | `0x000a2730` C2, `0x000a26c0` D1, `0x000a2a70` |
| `CommBillRecord::operator<<(long long)` | `0x000d1590` (via `DPBillAdapter::bill_`) |
| `DPMonitorReporter::report_data(uint,uint)` | `0x000adae0` |
| `DPMsgPacker::revive_client_mouse(UserInfo*, uint, E_TIPS_WINDOWS)` | `0x000b1aa0`; enum `E_TIPS_NONE=0, E_TIPS_SAFEMODE=1, E_TIPS_SEND_SUCCESS=2` |
| `DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor>` | `0x000b80d0` (weak); ORIG inlined it |
| `tss_dp_comm::get_cmd_id(const DPGameCmdStruct&, uchar*, ushort, uint&)` | `0x000a8870` |
| `DPPkgDelayHacker::insert(UserInfo*, int, uint, uchar*, uint&)` | `0x000b5460` |
| `DPPropertyTransImpl::hack(...)` | `0x000b96e0` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | `0x002bb4c0` |
| `IDPHacker::on_*` / `~IDPHacker` inherited slots | weak symbols `0x000a4770..0x000a4840` |
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, const char*, ...)` | `0x001b3e70` |
| `DPPkgDelayHacker::~DPPkgDelayHacker` | `0x000b6480` D1 / `0x000b6530` D0 |
| `DPPkgDelayHacker::init` / `on_logout` / `proc` | `0x000a99f0` / `0x000b5180` / `0x000b5300` (delay vtable slots) |
| `DPPkgDelayHacker::on_special_control` / `on_timer` | `0x000b55e0` / `0x000b4eb0` (delay vtable slots) |

Exact string constants reproduced:
`"[%u][%s], cur_hacker_pos_=%u, keep_time=%lu"` (0x21b56c),
`"drop cmd"` (0x21b563), `"recv drop cmd"` (0x21b55e),
`"drop package"` (0x21c9a2),
`"DPPkgDumpHacker, type=%d,cmd_id=0X%X"` (0x21c9d0),
`"[%u][%s], forward pkg to la"` (0x21ca13), `"hack"` (0x21ca80),
`"delay cmd"` (0x21b8f0), `"delay cmd fail"` (0x21b8fa).

## Buffer / return semantics summary

* `DPCmdDropHacker::hack` returns 1 while a drop window is active and
  0 otherwise; it never changes `buf`/`len`.
* `DPCmdDropHacker::on_drop_cmd` only arms/clears `drop_cmd_` windows and
  returns 0; it never changes `buf`/`len` (it does not receive them).
* `DPPkgDropHacker` and `DPPkgDumpHacker` never change `buf`/`len`.
* `DPFilterForwardLaHacker` and `DPPkgForwardLaHacker` never change the
  caller's `buf`/`len`; the forwarder copies at most 32768 bytes into its
  monitor object and always returns 0.
* `DPDelayHacker` passes the original `buf` and the `len` reference to
  `insert`; it returns 1 only when `insert` returned 0.
* `DPPropertyDelayHacker` returns the property handler's non-zero result, or
  the delay handler's result when the property handler returned 0.

## Compile result

Compiler: c5root GCC 4.1.2 20080704 (Red Hat 4.1.2-55), matching the target.

```
cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon
/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++ \
  -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
  -std=gnu++98 -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/hackers_a_agent.cpp \
  -o /tmp/hackers_a_agent.o
```

Result: `EXIT 0`, no diagnostics/warnings.

Defined symbols include exactly the nine strong out-of-line methods
(`DPCmdDropHacker::hack`/`on_drop_cmd`, `DPPkgDropHacker::hack`,
`DPPkgDumpHacker::hack`, `DPFilterForwardLaHacker::hack`,
`DPPkgForwardLaHacker::hack`, `DPDelayHacker::hack`,
`DPPropertyDelayHacker::hack`/`init`), the weak `DPCmdDropHacker::init` and
`inited` guard, the seven vtables/typeinfos and weak D1/D0 destructors.
Undefined references are exactly the intended boundaries listed above, with
ORIG mangled names; the
`send_datatrans_pkg<...LATransAdaptDPCmdBufferMonitor>` reference and the
`DPPropertyTransImpl::hack` reference were checked byte-for-byte against
`nm -S` on the target. Vtable sizes match ORIG exactly: 0x3c for
DPCmdDropHacker/DPPkgDropHacker/DPPkgDumpHacker/DPPkgForwardLaHacker/
DPFilterForwardLaHacker and 0x40 for DPDelayHacker/DPPropertyDelayHacker
(with `DPPkgDelayHacker::on_timer` in the appended slot).

## Unresolved / deliberate boundaries

* `DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor>` is inlined
  in ORIG; the reconstruction calls the real weak instantiation instead of
  duplicating the packer template. Observable forwarding/monitor/report
  behaviour is that function's responsibility.
* `propety_impl` is a TU-local object (`0x002c0aa0`, symbol type `b`); it is
  declared extern here, not redefined, because its constructor/destructor and
  `DPPropertyTransImpl` members belong to the property-translation TU.
* `DPPkgDelayHacker`'s map/list/timeval internals are modelled as opaque
  storage; only the exact prefix size (84) and the two counters at +0x4c/+0x50
  used by `DPDelayHacker::hack` are reconstructed. Its constructor, `insert`,
  `proc`, `on_logout`, `on_special_control` remain external real symbols.
* `DPBillAdapter::bill_` is a `DPBillRecorder` in ORIG; here it is declared as
  a static `CommBillRecord` solely so the streaming template resolves the exact
  `_ZN14CommBillRecordlsEx` call target and `_ZN9tss_dpsdk13DPBillAdapter5bill_E`
  data symbol.
* Per-TU static initialisation artefacts (ZEN status items, `std::ios_base`
  init, comm-singleton guards) are not reproduced; they are unrelated to the
  assigned class methods.
* Verdict: semantic equivalence (control flow, guards, return values, config
  lookups, field offsets, call targets, logging/report/bill constants).
