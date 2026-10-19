# AGENT_REPORT_hack_mgr

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/dp_hack_mgr_agent.cpp`

This is a semantic reconstruction, not a byte-identical source drop. The
verdict covers control flow, call targets, object/field offsets, guards,
return values and the observed buffer-reference forwarding. Plugin bodies,
`IDPHacker` default bodies and `DPPkgDelayHacker`'s constructor are external
boundaries and are not fabricated here.

## Symbols (nm -C, target)

Only six out-of-line `tss_dpsdk::DPHackMgr` symbols exist:

| ORIG address | Size | Symbol |
|---|---|---|
| `0x000aac90` | `0x2c8` | `DPHackMgr::hack(tss_sdk_impl::UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)` |
| `0x000ab370` | `0x6f3` | `DPHackMgr::init()` |
| `0x000aba70` | `0x54` | `DPHackMgr::DPHackMgr()` (C1) |
| `0x000abad0` | `0x62` | `DPHackMgr::~DPHackMgr()` (D2) |
| `0x000abb40` | `0x54` | `DPHackMgr::DPHackMgr()` (C2) |
| `0x000abba0` | `0x62` | `DPHackMgr::~DPHackMgr()` (D1) |

Related real symbols decoded for the same object graph:

| ORIG address | Size | Symbol |
|---|---|---|
| `0x000abfc0` | `0x4e` | `IDPHackerEventNotify::~IDPHackerEventNotify()` (D2, weak) |
| `0x000ac010` | `0x5b` | `IDPHackerEventNotify::~IDPHackerEventNotify()` (D1, weak) |
| `0x0021ba42` | `5` | `DPHackMgr::hack(...)::__func__` = `"hack"` |

`nm -C` shows **no** out-of-line `DPHackMgr::on_login`, `on_logout`, `proc`,
`on_ban_mode`, `on_drop_cmd`, `on_verify_response`, `on_verify_msg`,
`on_safe_mode`, `on_special_control` or `get_filter`. They are header-inline
and were recovered from their inlined call sites (see below). There are no
non-inline public getters in this class.

## Object layout (DWARF)

`DPHackMgr` is 124 bytes:

| offset | size | member |
|---|---|---|
| `+0x00` | 12 | `DPHACKERS hackers_` = `std::vector<IDPHacker*>` |
| `+0x0c` | 112 | `IDPHackerEventNotify event_notify_` |

`IDPHackerEventNotify` is 112 bytes: vptr `+0x00`, then nine
`std::vector<IDPHacker*>` at `+0x04 ... +0x70`. The event vector for
`E_EVENT n` is at `event_notify + 4 + 12*n`, i.e. `DPHackMgr + 0x10 + 12*n`.

`E_EVENT`: `E_LOGIN=0, E_LOGOUT=1, E_PROC=2, E_BAN_MODE=3, E_DROP_CMD=4,
E_MSG2CLIENT=5, E_VERIFY_MSG=6, E_SAFE_MODE=7, E_SPECIAL_CONTROL=8,
E_EVENT_COUNT=9`.

`IDPHacker` object: vptr at `+0x00`, sizeof 4 in the base. Vtable address
point `0x00292f68` (slot -> offset from vptr):

```
0 init(IDPHackerEventNotify*)       +0x00   6 on_drop_cmd(...)          +0x18
1 hack(...) = __cxa_pure_virtual    +0x04   7 on_verify_response(...)   +0x1c
2 on_login(UserInfo*)               +0x08   8 on_verify_msg(...)        +0x20
3 on_logout(UserInfo*)              +0x0c   9 on_safe_mode(...)         +0x24
4 proc(time_t)                      +0x10  10 on_special_control(...)   +0x28
5 on_ban_mode(...)                  +0x14  11/12 ~IDPHacker D1/D0       +0x2c/+0x30
```

The dispatch loop in `init()` calls slot 0; `hack()` calls slot 1.

## Hacker table and config type-to-vector mapping

`hackers_` is a flat 256-slot `std::vector<IDPHacker*>` built by `init()`.
Slot contents:

```
0            NULL sentinel (config hack_list values are 1-based)
1..15        plugin instances, in the push order below
16..199      NULL (resize(200))
200..207     DPCmdDropHacker(0) .. DPCmdDropHacker(7)
208..255     NULL (resize(256))
```

The 1-based mapping is confirmed by the deployed config
`/opt/dnf1031/home/template/neople/secsvr/zergsvr/cfg/dp_config.xml`, whose
`cmd_hack.up_cmd_hack` and `down_cmd_hack` fragments use `hack_list` values
`1` and `2`; these must resolve to `DPPkgForwardLaHacker` and
`DPFilterForwardLaHacker` respectively. The same XML specifies only one
`hack_list` entry per fragment, so the remaining zero-filled bytes index the
NULL sentinel at slot 0 and terminate the 8-byte walk (the `if (!hacker)
break;` path observed at `0x000aad98`/`0x000ab805`).

`hack()` selects one of three `tss_dp_comm::DPHackFilter` instances by
`type` (the ORIG adds these exact constants to `glb_dp_config_`):

| type | DPConfig offset | config member |
|---|---|---|
| `1` | `+0x3da4` | `cmd_hack.up_cmd_hack` |
| `2` | `+0x45a6` | `cmd_hack.down_cmd_hack` |
| other | `+0x4da8` | `cmd_hack.raw_cmd_hack` |

Config layout used (DWARF-verified, static-asserted): `DPConfig::cmd_hack`
at `+0x3d7e`; inside `DPCmdHack`, filters at `+0x26/+0x828/+0x102a`; a
`DPHackFilter` is `uint16_t fragment_count` at `+0`, then
`DPHackFragment[128]` at `+2` (packed; sizeof 2050). Each fragment is 16
bytes: `cmd_start +0`, `cmd_end +4`, `hack_list[8] +8`.

## `DPHackMgr::hack` (`0x000aac90`)

Signature: `int hack(UserInfo *user, int type, unsigned int cmd_id,
unsigned char *&buf, unsigned int &len)`.

Decoded flow:

1. `filter = get_filter(type)` (branch on `type == 1`, `type == 2`, else raw;
   no NULL check on `glb_dp_config_`).
2. `if (filter.fragment_count == 0) return 0;` (no log on this path).
3. Trace `"[%u][%s], type=%d, cmd_id=%d, len=%u, fragment_count=%d"`.
   Priority is `RS_INFO (3)` iff `user != 0 && (user->base_info_.flags_ & 1)`,
   else `RS_DEBUG (2)`; the `%u` uin is `user->base_info_.uin_` (UserInfo+0)
   or 0, and `%s` is `__func__` = `"hack"`.
4. `int res = 0;` then fragments `i = 0 .. fragment_count-1`:
   - `cmd_id < fragment.cmd_start` -> stop iterating and return `res`.
   - `cmd_id > fragment.cmd_end` -> next fragment (inclusive range).
   - Otherwise loop `j = 0..7` over `fragment.hack_list[j]`:
     - `hacker = hackers_[fragment.hack_list[j]]`; a NULL slot aborts the
       whole fragment (next fragment; remaining `hack_list` bytes ignored).
     - Trace `"[%u][%s], begin hack, i=%u,j=%u, hacker=%u"` with the
       fragment index, the `hack_list` position and the byte value.
     - Call the virtual slot at vptr `+0x04`:
       `hacker->hack(user, type, cmd_id, buf, len)`.
     - Trace `"[%u][%s], end hack, res=%u"` with the raw return value.
     - Non-zero result is returned immediately.
5. Return `res` (0 if no fragment matched or every selected hacker returned
   0).

Buffer semantics: `buf` and `len` are forwarded as references into the
caller's variables. A hacker may replace the caller's pointer and/or length
in place; `DPHackMgr` performs no allocation, copy, truncation or length
fixup. The return value is the raw hacker code (DPInterface treats `1` as
"packet intercepted"; see `dpinterface_anticheck_agent.cpp` /
`dpinterface_buffer_agent.cpp`).

## `DPHackMgr::init` (`0x000ab370`)

Decoded flow and correspondence:

```
0xab387  hackers_.clear();
0xab38c  hackers_.push_back(NULL);          // slot 0 sentinel
0xab3ad  hackers_.push_back(new DPPkgForwardLaHacker());
0xab3e5  hackers_.push_back(new DPFilterForwardLaHacker());
0xab41d  hackers_.push_back(new DPUicTransHacker());
0xab455  hackers_.push_back(new DPPropertyTransHacker());
0xab48d  hackers_.push_back(new DPPropertyDelayHacker());   // size 0x54
0xab4d8  hackers_.push_back(new DPMailVerifyHacker());      // size 0x60
0xab52d  hackers_.push_back(new DPForbidModeHacker());
0xab565  hackers_.push_back(new DPSafeModeHacker());
0xab59d  hackers_.push_back(new DPPkgDropHacker());
0xab5d5  hackers_.push_back(new DPDelayHacker());           // size 0x54
0xab60f  hackers_.push_back(new DPPkgDumpHacker());
0xab647  hackers_.push_back(new DPCfRoomQtalkHacker());
0xab67f  hackers_.push_back(new DPCfRecordUserInfo());
0xab6b7  hackers_.push_back(new DPCfExtractUserInfo());
0xab6ef  hackers_.push_back(new DPCfForwardRoomMasterMsg());
0xab727  hackers_.resize(200, NULL);
0xab76b  for (int i = 0; i < 8; ++i)
             hackers_.push_back(new DPCmdDropHacker(i));
0xab7ae  hackers_.resize(256, NULL);
0xab7f2  for (size_t i = 0; i < hackers_.size(); ++i)
             if (hackers_[i] != NULL)
                 hackers_[i]->init(&event_notify_);   // vptr+0x00
0xab831  return 0;
```

Plugin constructors are inline except `DPPkgDelayHacker`'s base constructor:
the ORIG emits calls to `_ZN9tss_dpsdk16DPPkgDelayHackerC2Ev`
(real symbol `0x000b5da0`). The 0x54/0x60 byte object sizes and the
`DPMailVerifyHacker` tail fields (`captcha_no_response_ +0x54`,
`captcha_counts_ +0x58`, `last_captcha_retry_ +0x5c`) are DWARF-exact.
`DPCmdDropHacker` is 8 bytes: vptr + `cur_hacker_pos_` (`+0x04`, set to the
loop index 0..7).

Vtable GOT entries referenced by init (ebx = `.got.plt` base `0x294534`),
in push order: `0x294294, 0x294500, 0x294354, 0x29438c, 0x2943e8,
0x2944b4, 0x294384, 0x2943a0, 0x2941b8, 0x294360, 0x294218, 0x294144,
0x2944d4, 0x2943b4, 0x2941b4`, then `0x294448` for the eight
`DPCmdDropHacker` objects.

`init()` itself does not call `register_event`; each plugin's virtual
`init(IDPHackerEventNotify*)` does. Verified examples: `DPPkgDelayHacker::init`
(0x000a99f0) pushes itself into vectors `+0x10` (E_LOGOUT), `+0x1c` (E_PROC)
and `+0x64` (E_SPECIAL_CONTROL); `DPCmdDropHacker::init` (0x000a6e40)
pushes into `+0x34` (E_DROP_CMD) behind a static-once guard. The inlined
`register_event` body de-duplicates by scanning the target vector for the
same pointer before `push_back`.

## Constructor / destructor

- `DPHackMgr::DPHackMgr()` (C1/C2): default-constructs `hackers_`
  (start=finish=end=0) and `event_notify_` (vptr =
  `_ZTVN9tss_dpsdk20IDPHackerEventNotifyE + 8`, nine empty vectors). The
  ORIG loop zeroes three dwords at `[+0x10,+0x7c)` in 12-byte steps, which is
  exactly the member initialization; both C1 and C2 are emitted.
- `DPHackMgr::~DPHackMgr()` (D1/D2): inlines the
  `IDPHackerEventNotify` destructor (installs the event-notify vptr, then
  frees the backing array of each event vector from index 8 down to 0), then
  frees the `hackers_` backing array. The 15 plugin objects and the eight
  `DPCmdDropHacker` objects themselves are **not** deleted: the ORIG frees
  only the vector storage (observed leak, reproduced by the
  member-destruction semantics).
- `IDPHackerEventNotify::~IDPHackerEventNotify()` is a weak inline virtual
  destructor; D2 frees the nine vector arrays, D1 additionally
  `operator delete(this)`. Emitting it in this TU reproduces both weak
  symbols and lets `~DPHackMgr` inline the body as in ORIG.

## Inline event dispatchers (no real symbols)

All nine `DPHackMgr` dispatchers have the same shape:
`int iRet = 0; for (size_t i = 0; i < hackers.size(); ++i) { iRet =
hackers[i]-><slot>(...); if (iRet != 0) break; } return iRet;` over
`event_notify_.hackers_[E_*]`. Observed mappings and inlined call sites:

| method | event | vtable slot | inlined at |
|---|---|---|---|
| `on_login` | `E_LOGIN` | `+0x08` | `DPInterface::add_user` `0xbfb2d` |
| `on_logout` | `E_LOGOUT` | `+0x0c` | `DPInterface::del_user` `0xbfca9` |
| `proc` | `E_PROC` | `+0x10` | `DPInterface::check_timer` `0xc0f3c` |
| `on_ban_mode` | `E_BAN_MODE` | `+0x14` | `DPInterface::on_recv_msg` `0xc067d` |
| `on_drop_cmd` | `E_DROP_CMD` | `+0x18` | `DPInterface::on_recv_msg` `0xc060e` |
| `on_verify_response` | `E_MSG2CLIENT` | `+0x1c` | `DPInterface::on_recv_msg` `0xc05a5` |
| `on_verify_msg` | `E_VERIFY_MSG` | `+0x20` | `DPInterface::on_recv_msg` `0xc06ec` |
| `on_safe_mode` | `E_SAFE_MODE` | `+0x24` | `DPInterface::on_recv_msg` `0xc075b` |
| `on_special_control` | `E_SPECIAL_CONTROL` | `+0x28` | `DPInterface::recv_buffer_for_decrypt` `0xc02d5` |

`get_filter(int)` is likewise inline; its branch structure is reproduced
inside `hack()`.

## External boundaries / call targets

| Reference | Target |
|---|---|
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | PLT `0x873b4` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | object `0x002bb4c0`, GOT `0x2943ac` (`ebx-0x188`) |
| `operator new(unsigned int)` / `operator delete(void*)` | PLT `0x842a4` / `0x88274` |
| `DPPkgDelayHacker::DPPkgDelayHacker()` C2 | PLT `0x7cf14` (`0x000b5da0`) |
| `std::vector<IDPHacker*>::_M_insert_aux` | PLT `0x815e4` |
| `std::vector<IDPHacker*>::_M_fill_insert` | PLT `0x80c44` |
| `std::vector<IDPHacker*>::erase` | PLT `0x7dd24` |
| hacker vtables (`_ZTVN9tss_dpsdk...E`) | external GOT entries listed above |
| `IDPHacker::init/hack/on_*` bodies | external virtual slots, not fabricated |

String constants reproduced exactly:
`"[%u][%s], type=%d, cmd_id=%d, len=%u, fragment_count=%d"` (@`0x21b9c4`),
`"[%u][%s], begin hack, i=%u,j=%u, hacker=%u"` (@`0x21b9fc`),
`"[%u][%s], end hack, res=%u"` (@`0x21ba27`), `__func__` `"hack"`
(@`0x21ba42`).

## Compile result

Compiler: c5root GCC 4.1.2 20080704 (Red Hat 4.1.2-55), the target's
compiler generation:

```
/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++ \
  -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_hack_mgr_agent.cpp \
  -o /tmp/dp_hack_mgr_agent.o
```

Result: `EXIT 0`, no diagnostics. The same source also compiles cleanly with
the WSL system `g++ (Ubuntu 13.x) -m32 -std=gnu++98 -O2 -fPIC
-fno-implement-inlines -fno-stack-protector` (`EXIT 0`); the c5root object is
the comparison artifact.

Exact ORIG mangled names emitted:

```
_ZN9tss_dpsdk9DPHackMgr4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
_ZN9tss_dpsdk9DPHackMgr4initEv
_ZN9tss_dpsdk9DPHackMgrC1Ev
_ZN9tss_dpsdk9DPHackMgrC2Ev
_ZN9tss_dpsdk9DPHackMgrD1Ev
_ZN9tss_dpsdk9DPHackMgrD2Ev
_ZN9tss_dpsdk20IDPHackerEventNotifyD0Ev
_ZN9tss_dpsdk20IDPHackerEventNotifyD1Ev
_ZN9tss_dpsdk20IDPHackerEventNotifyD2Ev
```

Defined user symbols: the six ORIG
`DPHackMgr` symbols, the two weak `IDPHackerEventNotify::~...` symbols, the
weak `IDPHackerEventNotify` vtable/typeinfo/name, the same vector template
instantiations present in the ORIG CU, and the local `hack()::__func__`.
Every one of them has an exact ORIG mangled counterpart. No helper or
fake allow/deny function is defined.

Undefined references are exactly the intended integration boundaries:
`tss_dp_comm::DP_CONFIG::glb_dp_config_`,
`ZEN_Trace_LogMsg::debug_output`, `_ZN9tss_dpsdk16DPPkgDelayHackerC2Ev`,
the 16 external hacker vtables, `operator new/delete`, `memmove`,
`_Unwind_Resume`, and the libstdc++ throw helpers.

## Layout checks enforced in the unit

Static typedef checks assert: `UserInfo` uin `+0x00` / flags `+0x20`;
`DPHackFragment` 16 bytes; `DPHackFilter` 2050 bytes with
`hack_fragments +2`; `DPConfig::cmd_hack +0x3d7e` with filters at
`+0x3da4/+0x45a6/+0x4da8`; `IDPHackerEventNotify` 112 bytes;
`DPHackMgr` 124 bytes with a 12-byte `DPHACKERS` first member (hence
`event_notify_` at `+0x0c`); `IDPHacker` 4; `DPCmdDropHacker` 8;
`DPPkgDelayHacker`/`DPDelayHacker`/`DPPropertyDelayHacker` 0x54;
`DPMailVerifyHacker` 0x60.

## Unresolved / deliberate partials

- Plugin class bodies and `DPPkgDelayHacker`'s out-of-line constructor are
  external; only vptr, size, `cur_hacker_pos_` and the three captcha fields
  are modeled. The `DPPkgDelayHacker` map/list/timeval members are opaque
  padding here and are initialized by the real C2 at `0x000b5da0`.
- `IDPHacker`'s default `on_*` bodies (weak symbols at `0x000a4770..0x000a4800`)
  are not redefined; calls go through the external vtable.
- `register_event` is inline-only (no symbol); its de-duplicating body is
  reconstructed from the inlined expansions in the plugin `init()` methods.
- `DPHackFilter` is modeled packed (fragment array at `+2`, size 2050); the
  exact original pack pragma variant (align 1 vs 2) is unobservable at the
  offsets read by `hack()`.
- Verdict is semantic equivalence only; no byte-identity claim, no commit.
