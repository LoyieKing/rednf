# AGENT_REPORT_hacker_base

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/hacker_base_agent.cpp`

Semantic reconstruction only: no byte-identity claim, no commit. All addresses
are virtual addresses in the target. Evidence: `nm -C -S`, `objdump -d -r -M
intel`, `readelf -rW`, DWARF definition DIEs and the source line table. Every
call target named below is the symbol actually referenced by the ORIG
instruction sequence.

## Symbols and ORIG ranges

| ORIG address | Size | Symbol |
|---|---|---|
| `0x000a4770` | `0x7` W | `IDPHacker::init(IDPHackerEventNotify*)` |
| — (vtable `+0x0c`) | — | `IDPHacker::hack(UserInfo*, int, unsigned, unsigned char*&, unsigned int&)` — pure virtual |
| `0x000a4780` | `0x7` W | `IDPHacker::on_login(UserInfo*)` |
| `0x000a4790` | `0x7` W | `IDPHacker::on_logout(UserInfo*)` |
| `0x000a47a0` | `0x7` W | `IDPHacker::proc(long)` |
| `0x000a47b0` | `0x7` W | `IDPHacker::on_ban_mode(UserInfo*, const DpBanModeReq&)` |
| `0x000a47c0` | `0x7` W | `IDPHacker::on_drop_cmd(UserInfo*, const DpDropCmdlistReq&)` |
| `0x000a47d0` | `0x7` W | `IDPHacker::on_verify_response(UserInfo*, unsigned char*, unsigned int)` |
| `0x000a47e0` | `0x7` W | `IDPHacker::on_verify_msg(UserInfo*, const DPCaptchaResultRsp&)` |
| `0x000a47f0` | `0x7` W | `IDPHacker::on_safe_mode(UserInfo*, int)` |
| `0x000a4800` | `0x7` W | `IDPHacker::on_special_control(UserInfo*, unsigned char*, unsigned int&, unsigned char*, unsigned int&)` |
| `0x000a4810` | `0x2e` W | `IDPHacker::~IDPHacker()` — D0, deleting |
| `0x000a4840` | `0x1e` W | `IDPHacker::~IDPHacker()` — D1, non-deleting |
| `0x000a9930` | `0x60` W | `IDPHackerSafe::~IDPHackerSafe()` — D0, deleting |
| `0x000a9990` | `0x58` W | `IDPHackerSafe::~IDPHackerSafe()` — D1, non-deleting |
| `0x000b6480` | `0xa2` W | `DPPkgDelayHacker::~DPPkgDelayHacker()` — D0, deleting |
| `0x000b6530` | `0xaa` W | `DPPkgDelayHacker::~DPPkgDelayHacker()` — D1, non-deleting |
| `0x00292f60` | `0x3c` V | vtable for `IDPHacker` |
| `0x00293180` | `0x3c` V | vtable for `IDPHackerSafe` |
| `0x00293300` | `0x40` V | vtable for `DPPkgDelayHacker` |
| `0x00292f48` | — V | typeinfo for `IDPHacker` (8 bytes, `__class_type_info`) |
| `0x002931bc` | — V | typeinfo for `IDPHackerSafe` (12 bytes, `__si_class_type_info`) |
| `0x00293340` | — V | typeinfo for `DPPkgDelayHacker` (12 bytes, `__si_class_type_info`) |

## Default-slot semantics

All ten non-pure `IDPHacker` slots are the identical seven-byte body seen at
`0x000a4770`..`0x000a4806`:

```
push ebp
xor  eax,eax
mov  ebp,esp
pop  ebp
ret
```

Consequences, verified from the absence of any memory access in those bodies:

* Return value is `int 0`.
* No argument is read or written. In particular `init()` does **not** store
  `event_notify`, and `on_verify_response()`/`on_special_control()` never
  modify the caller's buffer pointers or lengths.
* No member is read or written; the vptr is untouched.
* `hack(...)` has no definition: slot `+0x0c` of every vtable is
  `__cxa_pure_virtual`.

These slots are therefore pure defaults: they always return 0, perform no
event side effect, and leave the event payload untouched.
`tss_sdk_impl::UserInfo` and the three `sec_proto` request types are never
dereferenced by these bodies, so they are modelled as the exact incomplete
types named by the ORIG manglings; no protocol field layout is invented.

## Destructor decode

### `IDPHacker::~IDPHacker()` — D1 `0x000a4840`, D0 `0x000a4810`

D1 and D0 both do exactly:

1. load `_ZTVN9tss_dpsdk9IDPHackerE` through the GOT and store `vtable+8` into
   `[this]` (`0x000a4851`..`0x000a485a` / `0x000a4825`..`0x000a482e`);
2. D0 then calls `operator delete(void*)@plt` with `this`
   (`0x000a4830`..`0x000a4838`).

There are no data members to destroy. The vtable store, not a base call, is
the inlined `IDPHacker` destructor body as seen from derived classes.

### `IDPHackerSafe::~IDPHackerSafe()` — D1 `0x000a9990`, D0 `0x000a9930`

Both variants:

1. store `_ZTVN9tss_dpsdk13IDPHackerSafeE+8` into `[this]`;
2. call `_ZN16ZEN_Thread_MutexD1Ev@plt` with `this+4` (`0x000a995b` D0,
   `0x000a99bb` D1), destroying the sole member `locker_`;
3. execute the inlined `IDPHacker` destructor body: store
   `_ZTVN9tss_dpsdk9IDPHackerE+8` into `[this]`;
4. D0 additionally calls `operator delete(void*)@plt` with `this`
   (`0x000a996b`..`0x000a9973`).

There is no call to `IDPHacker::~IDPHacker`; the base destructor is inlined.
The exception landing pad restores the `IDPHacker` vptr and resumes through
`_Unwind_Resume@plt` if the member destructor throws.

### `DPPkgDelayHacker::~DPPkgDelayHacker()` — D1 `0x000b6530`, D0 `0x000b6480`

Both variants:

1. store `_ZTVN9tss_dpsdk16DPPkgDelayHackerE+8` into `[this]`;
2. destroy `cmd_delay_` (declared at `+0x38`): call
   `_ZNSt10_List_baseIN9tss_dpsdk16DPPkgDelayHacker12DPCommandBufESaIS2_EE8_M_clearEv@plt`
   with `this+0x38` (`0x000b655e` D1 / `0x000b64ae` D0);
3. destroy `waitting_for_send_` (declared at `+0x20`): load the tree root from
   `[this+0x28]` and call
   `_ZNSt8_Rb_treeIjSt4pairIKjN9tss_dpsdk16DPPkgDelayHacker13DPCommandBuf2EESt10_Select1stIS5_ESt4lessIjESaIS5_EE8_M_eraseEPSt13_Rb_tree_nodeIS5_E@plt`
   with `(this+0x20, root)` (`0x000b6570` D1 / `0x000b64c0` D0);
4. store `_ZTVN9tss_dpsdk13IDPHackerSafeE+8` into `[this]` and call
   `_ZN16ZEN_Thread_MutexD1Ev@plt` with `this+4` (`0x000b6586` D1 /
   `0x000b64d6` D0);
5. store `_ZTVN9tss_dpsdk9IDPHackerE+8` into `[this]` (inlined base dtor);
6. D0 calls `operator delete(void*)@plt` with `this` (`0x000b6599`).

`node_in_delay_`, `node_wait_for_send_`, `flag_position_`, `next_timer_` are
POD and produce no code. `DPCommandBuf` and `DPCommandBuf2` are trivially
destructible in ORIG (no destructor symbol exists for either nested type), so
`_M_clear`/`_M_erase` only walk and free the container nodes; both ORIG weak
helpers contain just `operator delete` calls, which the reconstruction
reproduces (`_M_clear` size `0x38`, `_M_erase` size `0x4b`, both match ORIG).
Exception landing pads restore the safe/hacker vptrs and resume through
`_Unwind_Resume@plt`.

## Class layouts (DWARF definition DIEs, byte-exact)

```
IDPHacker (4, vptr only)
  +0x00  _vptr.IDPHacker

IDPHackerSafe : public IDPHacker (32)
  +0x00  IDPHacker base
  +0x04  ZEN_Thread_Mutex locker_           (28 bytes)

DPPkgDelayHacker : public IDPHackerSafe (84)
  +0x00  IDPHackerSafe base
  +0x20  std::map<unsigned int, DPCommandBuf2> waitting_for_send_  (24)
  +0x38  std::list<DPCommandBuf> cmd_delay_                        (8)
  +0x40  timeval next_timer_                                       (8)
  +0x48  int flag_position_
  +0x4c  unsigned int node_wait_for_send_
  +0x50  unsigned int node_in_delay_
```

Nested buffer types from `dp_pkg_delay_hacker.h`:

```
DPPkgDelayHacker::DPCommandBuf (24)
  +0x00 unsigned int cmd_id_
  +0x04 unsigned int buf_len_
  +0x08 unsigned char *buf_
  +0x0c tss_sdk_impl::UserInfo *user_
  +0x10 timeval expire_time_

DPPkgDelayHacker::DPCommandBuf2 (12)
  +0x00 unsigned int cmd_id_
  +0x04 unsigned int buf_len_
  +0x08 unsigned char *buf_
```

Class-scope constants and private helpers decoded from the definition DIE:
`static const char E_IN_DELAY = 1`, `E_IN_SPECIAL_CONTROL = 2`;
`static int isLessThan(timeval&, timeval&)`; `insert(DPCommandBuf&)`,
`insert(UserInfo*, int, unsigned, unsigned char*, unsigned int&)` and
`remove_from_delay(UserInfo*, std::list<DPCommandBuf>::iterator&)` are private
non-virtual members; `on_timer(DPCommandBuf&)` is a private pure virtual
appended after the two destructor slots.

`ZEN_Thread_Mutex` (external, zenlib TU) is 28 bytes: vptr at `+0`, virtual
destructor, `pthread_mutex_t lock_` at `+4` (24 bytes). The reconstruction
declares only that external type's 28-byte shape and its out-of-line
constructor/destructor; the definitions stay in the DSO.

## Vtable slot order (from `.rel.data.rel.ro`, target values)

`IDPHacker` (`0x3c` bytes) and `IDPHackerSafe` (`0x3c`):

| offset | entry |
|---|---|
| `+0x00` | offset-to-top (0) |
| `+0x04` | typeinfo |
| `+0x08` | `IDPHacker::init` |
| `+0x0c` | `__cxa_pure_virtual` (`hack`) |
| `+0x10` | `IDPHacker::on_login` |
| `+0x14` | `IDPHacker::on_logout` |
| `+0x18` | `IDPHacker::proc` |
| `+0x1c` | `IDPHacker::on_ban_mode` |
| `+0x20` | `IDPHacker::on_drop_cmd` |
| `+0x24` | `IDPHacker::on_verify_response` |
| `+0x28` | `IDPHacker::on_verify_msg` |
| `+0x2c` | `IDPHacker::on_safe_mode` |
| `+0x30` | `IDPHacker::on_special_control` |
| `+0x34` | D1 |
| `+0x38` | D0 |

`DPPkgDelayHacker` (`0x40` bytes) overrides `init` (`+0x08`), `on_logout`
(`+0x14`), `proc` (`+0x18`) and `on_special_control` (`+0x30`), keeps the
inherited `on_login`/`on_ban_mode`/`on_drop_cmd`/`on_verify_response`/
`on_verify_msg`/`on_safe_mode` entries, and appends the private pure virtual
`on_timer` at `+0x3c` (`__cxa_pure_virtual`). Its D1/D0 slots are at
`+0x34`/`+0x38` as usual.

## External boundaries (deliberately undefined here)

* `_ZN16ZEN_Thread_MutexD1Ev` — zenlib mutex destructor.
* `_ZN9tss_dpsdk16DPPkgDelayHacker4initEPNS_20IDPHackerEventNotifyE`,
  `...4procEl`, `...9on_logoutEPN12tss_sdk_impl8UserInfoE`,
  `...18on_special_controlE...` — the four strong overrides owned by the
  delay-hacker TU.
* `operator delete(void*)`, `__cxa_pure_virtual`, `_Unwind_Resume`,
  `__gxx_personality_v0`, and the two libstdc++ RTTI class vtables.
* `IDPHackerSafe`'s and `IDPHacker`'s destructors are deliberately inlined, so
  they are not undefined boundaries in derived destructors (matching ORIG).

## Compile result

Compiler: c5root GCC 4.1.2 20080704 (Red Hat 4.1.2-55), matching the target
compiler family/flags.

```
cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon
/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++ \
  -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
  -std=gnu++98 -I recon \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/hacker_base_agent.cpp \
  -o /tmp/hacker_base_agent.o
```

Result: `EXIT 0`, no diagnostics. The layout gates in the unit compile only if
`sizeof(IDPHacker)==4`, `sizeof(IDPHackerSafe)==32`,
`sizeof(DPPkgDelayHacker)==84`, `sizeof(DPCommandBuf)==24`,
`sizeof(DPCommandBuf2)==12` and `sizeof(ZEN_Thread_Mutex)==28`.

Defined symbols and sizes in the produced object:

| binding | size | symbol |
|---|---|---|
| W | `0x7` each | all ten `IDPHacker` default slots (`init`, `on_login`, `on_logout`, `proc`, `on_ban_mode`, `on_drop_cmd`, `on_verify_response`, `on_verify_msg`, `on_safe_mode`, `on_special_control`) |
| W | `0x2e` / `0x1e` | `IDPHacker` D0 / D1 |
| W | `0x60` / `0x58` | `IDPHackerSafe` D0 / D1 |
| W | `0xaa` / `0xa2` | `DPPkgDelayHacker` D0 / D1 |
| V | `0x3c`,`0x3c`,`0x40` | vtables `IDPHacker`, `IDPHackerSafe`, `DPPkgDelayHacker` |
| V | `8`,`12`,`12` | typeinfos (plus typeinfo-name strings `0x17`,`0x1c`,`0x1f`) |
| W | `0x38` / `0x4b` | `std::_List_base<DPCommandBuf>::_M_clear`, `std::_Rb_tree<...DPCommandBuf2...>::_M_erase` |

The emitted `IDPHacker` method bodies are byte-identical seven-byte sequences
(`push ebp; xor eax,eax; mov ebp,esp; pop ebp; ret`). The emitted destructor
bodies reproduce the ORIG vptr/member/call order; `IDPHacker` and
`IDPHackerSafe` D0/D1 sizes match ORIG exactly. `DPPkgDelayHacker`'s
D0/D1 section sizes are swapped relative to ORIG (`0xaa`/`0xa2` versus
`0xa2`/`0xaa`) only because GCC 4.1.2 lays out the same exception landing pads
in the opposite sections; the call sequence, offsets and observable behavior
are identical.

Vtable relocations of the produced object were compared entry-by-entry with
the target `.data.rel.ro` relocations: slot order, overrides and
`__cxa_pure_virtual` placements match, and the vtable sizes match.

## Unresolved / deliberate limits

* `DPPkgDelayHacker`'s constructor, `init`, `proc`, `on_logout`,
  `on_special_control`, `insert`, `remove_from_delay` and `on_timer` are other
  TUs; only the class shape needed by the destructor and vtable is declared.
* `ZEN_Thread_Mutex` is modelled as the 28-byte external type (vptr + 24-byte
  mutex storage). It is not reimplemented; only its destructor is called, with
  the exact ORIG symbol `_ZN16ZEN_Thread_MutexD1Ev`.
* `std::map`/`std::list` are the real libstdc++ 4.1 containers, so their
  layout (`+0x20`, `+0x38`) and the `_M_clear`/`_M_erase` helper semantics are
  the library's, not hand-written.
* No behavior remains unresolved for the assigned symbols: every default slot
  is a `return 0` no-op, and all three destructor bodies are fully accounted
  for including exception cleanup.
