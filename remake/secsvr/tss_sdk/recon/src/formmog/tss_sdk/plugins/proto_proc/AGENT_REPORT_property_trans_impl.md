# DPPropertyTransImpl reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
Original TU (DWARF `DW_AT_name`):
`/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/formmog/tss_sdk/plugins/proto_proc/dp_property_trans_hacker.cpp`
(decl file 1).
Emitted source: `property_trans_impl_agent.cpp` (this directory).

All findings are from the shipped symbol table, DWARF (class/member/line
tables, inlined-subroutine DIEs), `.rodata`, relocations and disassembly.
Addresses are link-time virtual addresses. This is a semantic reconstruction,
not a byte-identical one.

## Address ranges and symbol bindings

| Method | ORIG symbol | body range | size/binding |
|---|---|---|---|
| `on_logout(UserInfo*)` | `_ZN9tss_dpsdk19DPPropertyTransImpl9on_logoutEPN12tss_sdk_impl8UserInfoE` | `0x000b8820-0x000b88af` | `0x90` T |
| `forward_trade_data_to_safemode(UserInfo*, UserInfo*, int, unsigned int, unsigned char const*, unsigned int)` | `...30forward_trade_data_to_safemode...PKhj` | `0x000b8fb0-0x000b96d4` | `0x725` T |
| `hack(UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)` | `...4hack...EijRPhRj` | `0x000b96e0-0x000b9e83` | `0x7a4` T |
| `init(IDPHackerEventNotify*)` | `...4initEPNS_20IDPHackerEventNotifyE` | `0x000ba8e0-0x000ba954` | `0x75` W |
| `~DPPropertyTransImpl()` D0 | `...D0Ev` | `0x000ba960-0x000ba9fe` | `0x9f` W |
| `~DPPropertyTransImpl()` D1 | `...D1Ev` | `0x000baa00-0x000baa96` | `0x97` W |

`init` and the destructors are weak in ORIG because their bodies are defined
inside the class in the original header; the reconstruction defines them
in-class for the same binding. `hack`, `on_logout` and
`forward_trade_data_to_safemode` are strong `T`. The emitted object has the
exact mangled names and bindings for all six symbols, plus the
`DPPropertyTransImpl` vtable/typeinfo.

## Class and helper layouts (DWARF-exact)

`DPPropertyTransImpl`, size 56 (`0x38`):

```
+0x00  IDPHackerSafe base (4-byte vptr + ZEN_Thread_Mutex locker_)
+0x20  std::map<unsigned int, UserInfo*> uid2usr_ (24 bytes)
```

`IDPHackerSafe` size 32: `IDPHacker` vptr at +0, `ZEN_Thread_Mutex locker_`
at +4. `ZEN_Thread_Mutex` size 28: vptr, virtual dtor pair at vtable slots
+0/+4, `lock()` at +0x08, `try_lock()` at +0x0c, `unlock()` at +0x10. ORIG
calls lock through `[vptr+8]` and unlock through `[vptr+0x10]`.

`IDPHackerEventNotify` size 112: vptr at +0, `std::vector<IDPHacker*>
hackers_[9]` at +4. `E_LOGOUT == 1`, so `hackers_[E_LOGOUT]` is at +0x10.

`tss_sdk_impl::UserInfo` used offsets (400-byte object):
`base_info_.uin_ +0x00`, `room_id_ +0x08`, `client_ip_ +0x0c`,
`flags_ +0x20`; `hack_data_.safemode_level_ +0xdc`,
`hack_data_.dnf_client_uid_ +0xe0`.

Vtable order verified against `_ZTVN9tss_dpsdk19DPPropertyTransImplE`
(ORIG `0x00293500` and reconstructed
`.data.rel.ro._ZTVN9tss_dpsdk19DPPropertyTransImplE`), slot for slot:
`init`, `hack`, `on_login`, `on_logout`, `proc`, `on_ban_mode`,
`on_drop_cmd`, `on_verify_response`, `on_verify_msg`, `on_safe_mode`,
`on_special_control`, `~D1`, `~D0`. The inherited signatures are declared
with the real `sec_proto` parameter types (`DpBanModeReq`,
`DpDropCmdlistReq`, `DPCaptchaResultRsp`) so the vtable relocations reference
the shipped `IDPHacker` mangled names, not substitutes.

## Configuration model (partial, offsets exact)

`tss_dp_comm::DPConfig` is 22842 bytes (`0x593a`). The dispatch block is
`cmd_hack` at `+0x3d7e`; its `property_cmd` member is at `+0x1b8c` inside
`DPCmdHack`, i.e. absolute `DPConfig + 0x590a`. `property_cmd` is 48 bytes:

| field | abs. offset | field | abs. offset |
|---|---|---|---|
| `cmd_role_uid` | `0x590a` | `cmd_trade_accept` | `0x591e` |
| `cmd_create_team` | `0x590e` | `cmd_trade_confirm` | `0x5922` |
| `cmd_drop_item` | `0x5912` | `cmd_trade_put_item` | `0x5926` |
| `cmd_create_role` | `0x5916` | `cmd_enchant_put_item` | `0x592a` |
| `cmd_enter_private_store` | `0x591a` | `cmd_enchant_confirm` | `0x592e` |
|  |  | `cmd_simple_mail` | `0x5932` |
|  |  | `cmd_complex_mail` | `0x5936` |

The object is read through the exported pointer
`_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E` (`DP_CONFIG::glb_dp_config_`);
there is no null check, matching ORIG.

## Packet models (packed, sizes/offsets exact)

`DPHackDnfCmdHead` (13): `type_` char +0, `cmd_id_` u16 +1, `length_` int +3,
`crc_` int +7, `sequence_` u16 +11.

`DPHackDnfCmdUid` (26): head +0, `pad_[11]` +13, `role_uid_` u16 +24.

`DPHackDnfCmdRequest` (16): head +0, `role_uid_` u16 +13, `sub_id_` u8 +15.

`DPHackDnfTradeConfirm` (14): head +0, `sub_id_` u8 +13.

`DPHackDnfItemMove` (25): head +0, `flag_` u8 +13, `pad_[10]` +14,
`sub_id_` u8 +24.

`DPHackDnfCmdEnchant` (14): head +0, `flag_` u8 +13.

`DPHackStoreResponse` (18): head +0, `pad_[3]` +13, `role_uid_` u16 +16.

`DPHackDnfMailHead` (18): head +0, `role_name_len_` u32 +13,
`role_name_[1]` +17.

`DPHackDnfMailItem` (10): `item_posi_` u16 +0, `item_id_` u32 +2,
`item_num_` u32 +6.

`DPHackDnfMailBody` (16): `gold_num_` u32 +0, `items_num_` u8 +4,
union at +5 (11 bytes) with `items_[1]` / `item_`; `item_.item_posi_` is at
body +5.

`sec_proto::ReqSafemodeTransAdaptTradeNotify` (2073): `world_id_` u16 +0,
`src_id_` u32 +2, `type_` u8 +6, `send_uin_` u32 +7, `target_uin_` u32 +11,
`send_ip_` u32 +15, `target_ip_` u32 +19, `buf_len_` u16 +23,
`trade_buf_[2048]` +25.

## `hack` — `0xb96e0`

`hack(UserInfo* user, int type, unsigned int cmd_id, unsigned char*& buf,
unsigned int& len)`

```
unsigned int dest_uid;            // assigned before every read
UserInfo* dest_user = 0;

if (type == 2) {
    if (cmd_id == property_cmd.cmd_role_uid) {
        if (len < sizeof(DPHackDnfCmdUid)) -> len error, return -1;
        dest_uid = ((DPHackDnfCmdUid*)buf)->role_uid_;   // u16 at +0x18
        log RS_INFO/2 "[%u][%s], uid=%u", uin, "hack", dest_uid;
        lock;
        uid2usr_[dest_uid] = user;
        user->hack_data_.dnf_client_uid_ = dest_uid;
        unlock;
        return 0;
    } else if (cmd_id == cmd_create_team || cmd_id == cmd_drop_item) {
        if (len < sizeof(DPHackDnfCmdHead)) -> len error, return -1;
        if (((DPHackDnfCmdHead*)buf)->type_ != 0) {
            log "[%u][%s], type_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_create_role) {
        if (len < sizeof(DPHackDnfCmdHead)) -> len error, return -1;
        if (((DPHackDnfCmdHead*)buf)->type_ != 1) {
            log "[%u][%s], type_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_enter_private_store) {
        if (len < sizeof(DPHackStoreResponse)) -> len error, return -1;
        if (((DPHackStoreResponse*)buf)->header_.type_ != 1) {
            log "[%u][%s], type_=%u"; return 0;
        }
        dest_uid = ((DPHackStoreResponse*)buf)->role_uid_; // u16 at +0x10
        goto lookup;
    } else goto forward;
} else {
    if (cmd_id == cmd_trade_accept) {
        if (len < sizeof(DPHackDnfCmdRequest)) -> len error, return -1;
        sub_id = ((DPHackDnfCmdRequest*)buf)->sub_id_;     // u8 at +0xf
        if (sub_id != 1 && sub_id != 6 && sub_id != 7 && sub_id != 8) {
            log "[%u][%s], sub_id_=%u"; return 0;
        }
        dest_uid = ((DPHackDnfCmdRequest*)buf)->role_uid_; // u16 at +0xd
        goto lookup;
    } else if (cmd_id == cmd_trade_confirm) {
        if (len < sizeof(DPHackDnfTradeConfirm)) -> len error, return -1;
        if (((DPHackDnfTradeConfirm*)buf)->sub_id_ != 3) {   // +0xd
            log "[%u][%s], sub_id_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_trade_put_item) {
        if (len < sizeof(DPHackDnfItemMove)) -> len error, return -1;
        sub_id = ((DPHackDnfItemMove*)buf)->sub_id_;         // +0x18
        flag   = ((DPHackDnfItemMove*)buf)->flag_;           // +0xd
        if (sub_id != 4 && flag != 4) {
            log "[%u][%s], sub_id_=%u, flag_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_enchant_put_item) {
        if (len < sizeof(DPHackDnfCmdEnchant)) -> len error, return -1;
        if (((DPHackDnfCmdEnchant*)buf)->flag_ != 0) {       // +0xd
            log "[%u][%s], flag_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_enchant_confirm) {
        if (len < sizeof(DPHackDnfCmdEnchant)) -> len error, return -1;
        if (((DPHackDnfCmdEnchant*)buf)->flag_ != 1) {       // +0xd
            log "[%u][%s], flag_=%u"; return 0;
        }
        goto forward;
    } else if (cmd_id == cmd_simple_mail || cmd_id == cmd_complex_mail) {
        if (user->hack_data_.safemode_level_ != 0) return 0;
        mail_header = (DPHackDnfMailHead*)buf;              // no pre-check
        length_require = mail_header->role_name_len_ + 0x11; // u32 at +0xd
        mail_body = (DPHackDnfMailBody*)(buf + length_require);
        if (len < length_require + sizeof(DPHackDnfMailBody))
            -> len error, expected=length_require+0x10, return -1;
        if (mail_body->gold_num_ != 0 || mail_body->items_num_ != 0 ||
            mail_body->u_.item_.item_posi_ != 0)
            goto forward;
        log "[%u][%s], no gold or item"; return 0;
    } else goto forward;
}

lookup:
    if (dest_uid != (unsigned int)-1) {
        lock;
        std::map<unsigned int, UserInfo*>::iterator it = uid2usr_.find(dest_uid);
        if (it != uid2usr_.end()) dest_user = it->second;
        unlock;
    }
forward:
    forward_trade_data_to_safemode(user, dest_user, type, cmd_id, buf, len);
    DPMonitorReporter::report_data(0x407, 1);
    return 1;
```

The `dest_uid != (unsigned int)-1` guard is present in ORIG (`cmp
esi,0xffffffff`) and is effectively impossible because `dest_uid` is a
zero-extended 16-bit field; it is preserved rather than "fixed".

Logging details used by every `hack` site: debug priority is `RS_INFO` when
`user != 0 && (user->base_info_.flags_ & 1) == 1`, else `RS_DEBUG`; the `%u`
uin is 0 when `user == 0`. Length errors are always `RS_ERROR` and use
`"[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, expected_len=%u"`
with `__func__ == "hack"` and `expected_len` equal to the checked `sizeof`.
All filter failures return 0; all length failures return -1; every forwarded
command returns 1 after monitor id `0x407`.

## `forward_trade_data_to_safemode` — `0xb8fb0`

```
ReqSafemodeTransAdaptTradeNotify trade_pkg;              // line 62
trade_pkg.type_       = (unsigned char)type;
trade_pkg.send_uin_   = user->base_info_.uin_;
trade_pkg.send_ip_    = htonl(user->base_info_.client_ip_);  // bswap32
trade_pkg.world_id_   = (unsigned short)user->base_info_.room_id_;
trade_pkg.src_id_     = 0;
if (dst_user != 0) {
    trade_pkg.target_uin_ = dst_user->base_info_.uin_;
    trade_pkg.target_ip_  = dst_user->base_info_.client_ip_;
} else {
    trade_pkg.target_uin_ = 0;
    trade_pkg.target_ip_  = 0;
}
if (len > sizeof(trade_pkg.trade_buf_))                  // > 0x800
    trade_pkg.buf_len_ = 0x800;
else
    trade_pkg.buf_len_ = (unsigned short)len;
memcpy(trade_pkg.trade_buf_, buf, trade_pkg.buf_len_);   // length = u16
log(flags&1 ? RS_INFO : RS_DEBUG,
    "[%u][%s], forward to trans, target_uin_=%u",
    user->uin, "forward_trade_data_to_safemode", trade_pkg.target_uin_);
return DPMsgPacker::send_datatrans_pkg<
    sec_proto::ReqSafemodeTransAdaptTradeNotify>(user, 0x26, trade_pkg,
                                                 0x5cc65);
```

`send_ip_` is byte-swapped; `target_ip_` is copied verbatim — the asymmetry is
in the shipped code. `cmd_id` is **not read at all** in `0xb8fb0-0xb96d4`
(no `[ebp+0x18]` load); it is kept in the signature and explicitly discarded.

The ORIG body inlines the entire `send_datatrans_pkg` path: an
`init_framehead(0x10000, 0, 0x5cc65)` frame with services `(0x26,0)`,
`(0,0)`, `(0x1f,0)`, `appdata_encode<ReqSafemodeTransAdaptTradeNotify>`,
then a `ReqSecsvrForwardData` wrapper and
`send_ipc_pkg<ReqSecsvrForwardData>(..., 0x4c6ad)` in a second frame sent with
`send_buf_to_backend`. The reconstruction calls the shipped out-of-line weak
instantiation of `send_datatrans_pkg<ReqSafemodeTransAdaptTradeNotify>`
(`0x000bac20`, symbol
`_ZN9tss_dpsdk11DPMsgPacker18send_datatrans_pkgIN9sec_proto32ReqSafemodeTransAdaptTradeNotifyEEEiPN12tss_sdk_impl8UserInfoEiRKT_j`)
with identical arguments; that weak instantiation is the same source-level
call target and performs the same frame/send sequence. No fake send or
success path exists. Because the callee is out of line, the reconstructed
`forward` body is `0x14d` bytes instead of ORIG's `0x725`; this is a
code-size/inlining difference only.

## `init` — `0xba8e0`

`std::vector<IDPHacker*>& hackers = event_notify->hackers_[E_LOGOUT]`
(`event_notify + 0x10`); linear scan compares each element with `this`; if
already present return 0; otherwise `push_back(this)` and return 0. No log,
no error path. ORIG emits the dedup loop and the inlined vector growth check;
the reconstruction's generated loop, `_M_insert_aux` call and return paths
match.

## `on_logout` — `0xb8820`

```
if (user->hack_data_.dnf_client_uid_ == 0) return 0;   // user+0xe0
{
    ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);     // lock via vptr+8
    uid2usr_.erase(user->hack_data_.dnf_client_uid_);  // key u32
    user->hack_data_.dnf_client_uid_ = 0;
}                                                      // unlock via vptr+0x10
return 0;
```

ORIG erases by key (`_Rb_tree<...>::erase(unsigned int const&)`), not by a
per-user scan. The guard is scoped so unlock occurs after the uid is cleared,
including the unwind path.

## Destructors — `0xba960` (D0), `0xbaa00` (D1)

In-class empty virtual destructor. Generated sequence matches ORIG:
install `_ZTVN9tss_dpsdk19DPPropertyTransImplE+8`; destroy `uid2usr_` via
`_Rb_tree<...>::_M_erase(root at map+8)`; install
`_ZTVN9tss_dpsdk13IDPHackerSafeE+8`; call
`ZEN_Thread_Mutex::~ZEN_Thread_Mutex()` on `this+4`; install
`_ZTVN9tss_dpsdk9IDPHackerE+8`; for D0 call `operator delete(this)`, for D1
return. The vtable/typeinfo and both weak destructors are emitted. Sizes are
`0x9f`/`0x97`, identical to ORIG.

## Call targets (verified in the object relocations)

| target | ORIG symbol/address |
|---|---|
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | `0x1b3e70` |
| `DPMsgPacker::send_datatrans_pkg<ReqSafemodeTransAdaptTradeNotify>` | weak `0x000bac20` |
| `DPMonitorReporter::report_data(unsigned int, unsigned int)` | `0x000adae0` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | `0x002bb4c0` |
| `ZEN_Thread_Mutex::~ZEN_Thread_Mutex()` | `0x001a1330` |
| `ZEN_Thread_Mutex::lock/unlock` | virtual, `[vptr+8]` / `[vptr+0x10]` |
| `std::_Rb_tree<u32, pair<const u32, UserInfo*>>::{erase(key), find, lower_bound, insert_unique(hint), insert_unique(value), _M_erase}` | weak ORIG instantiations `0x000ba450`, `0x000b9f70`, `0x000b9fd0`, `0x000ba5f0`, `0x000ba4e0`, `0x000ba050` |
| `std::vector<IDPHacker*>::_M_insert_aux` | PLT target `0x000815e4` |
| `memcpy` | libc |
| `operator new[]/delete`, `_Rb_tree_*` non-member helpers | libstdc++ |

The object also emits the same weak map/vector template instantiations that
ORIG places directly after `hack` (`0x000b9f70-0x000ba5f0`): `find`,
`lower_bound`, `upper_bound`, `insert_unique`, `erase(iterator,iterator)`,
`erase(key)`, `_M_erase`, `_M_insert`. Calls from `hack`/`on_logout` resolve
to these names. The reconstructed `hack` body is `0x78d` vs ORIG `0x7a4`
(`init` `0x6d` vs `0x75`, `on_logout` `0x94` vs `0x90`); control flow, field
offsets and call targets were verified individually.

## External boundaries left undefined

- `IDPHacker` default virtuals (`on_login`, `proc`, `on_ban_mode`,
  `on_drop_cmd`, `on_verify_response`, `on_verify_msg`, `on_safe_mode`,
  `on_special_control`), `IDPHacker` vtable/typeinfo
- `IDPHackerEventNotify` destructor/vtable (not used by these methods)
- `ZEN_Thread_Mutex` constructor/destructor/lock/try_lock/unlock
- `ZEN_Log`/`ZEN_Trace_LogMsg::debug_output`
- `DPMsgPacker` weak instantiation, `DPMonitorReporter::report_data`
- `tss_dp_comm::DP_CONFIG::glb_dp_config_`
- libstdc++ map/vector/Rb-tree helpers, exception runtime, libc `memcpy`

The global `tss_dpsdk::propety_impl` object, its constructor, and the TU's
static initialization/destruction (`__tcf_3`, `_GLOBAL__I_...`) are outside
this assignment and are not defined here. No shared header, CMake, test,
README or existing source was modified; no commit was made.

## Deliberate ORIG behaviours preserved

- `forward_trade_data_to_safemode`'s `cmd_id` parameter is unused.
- `dest_uid == (unsigned int)-1` guard is impossible for a 16-bit source
  field but is preserved.
- `type_`, `sub_id_` and `flag_` filter constants are raw literals; the
  original enum/macro names were not recovered from DWARF.
- Mail handling reads `role_name_len_` before any minimum-length validation
  (ORIG does the same); the later `len < role_name_len_+0x21` check is the
  only bound.
- The mail path and the role-uid path dereference `user` without a null check,
  as ORIG does.
- `glb_dp_config_` is dereferenced without a null check at every dispatch.
- `htonl` is used for the `send_ip_` byte swap; the compiler emits the same
  shift/or byte-swap sequence as ORIG (no external `htonl` call).
- Map insertion uses `uid2usr_[dest_uid] = user`, reproducing ORIG's
  `lower_bound` + `insert_unique(hint)` sequence and the value at node +0x14.

## Compile result

Compiler: c5root GCC 4.1.2 20080704, 32-bit sysroot (`env-m32.sh` paths).

```
/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++ \
  -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
  -std=gnu++98 -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/property_trans_impl_agent.cpp \
  -o /tmp/property_trans_impl_agent.o
```

Result: exit 0, no diagnostics. In-unit `typedef` layout checks assert all
sizes/offsets listed above (UserInfo 400, trade notify 2073, all DPHack
packets, DPConfig 22842 with `cmd_hack +0x3d7e`/`property_cmd +0x1b8c`, class
56, IDPHackerSafe 32, ZEN_Thread_Mutex 28, IDPHackerEventNotify 112).

`nm` on the object confirms exactly:

```
T _ZN9tss_dpsdk19DPPropertyTransImpl30forward_trade_data_to_safemode...
T _ZN9tss_dpsdk19DPPropertyTransImpl4hack...
W _ZN9tss_dpsdk19DPPropertyTransImpl4initEPNS_20IDPHackerEventNotifyE
T _ZN9tss_dpsdk19DPPropertyTransImpl9on_logoutEPN12tss_sdk_impl8UserInfoE
W _ZN9tss_dpsdk19DPPropertyTransImplD0Ev
W _ZN9tss_dpsdk19DPPropertyTransImplD1Ev
```

The vtable relocations are slot-identical to ORIG's
`_ZTVN9tss_dpsdk19DPPropertyTransImplE` (typeinfo, init, hack, inherited
IDPHacker defaults, on_logout, D1, D0), and the inherited default symbols
carry the exact `sec_proto::DpBanModeReq`/`DpDropCmdlistReq`/
`DPCaptchaResultRsp` mangled names.

Semantic verdict: all assigned methods are reconstructed with the shipped
dispatch, guards, returns, packet/config offsets, logs, map ownership and the
real safemode-forwarding call target. Only the inlining/code-size of the
forward template expansion differs from ORIG; behaviour is the same.
