# AGENT_REPORT_anticheck

## Target

- Binary: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
  (ELF32 i386, GNU C++ 4.1 ABI).
- Unit: `DPInterface::anticheck_pkg(tss_sdk_impl::UserInfo *, int,
  unsigned char *, unsigned int)`.
- Range: `0x000bf270` - `0x000bf3b2` (symbol size `0x142`; next symbol
  `DPInterface::is_need_encrypt` at `0x000bf3c0`, padding in between).
  The requested window `0x000bf270..0x000bf3c0` therefore ends at the next
  function boundary.
- Output file:
  `recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_anticheck_agent.cpp`.

## Exact signature and symbol

```cpp
int DPInterface::anticheck_pkg(tss_sdk_impl::UserInfo *user, int cmd_id,
                               unsigned char *buf, unsigned int len);
```

Mangled (verified on the compiled object with `nm`):

```
_ZN11DPInterface13anticheck_pkgEPN12tss_sdk_impl8UserInfoEiPhj
```

`this` is the first stack argument (`[ebp+0x8]`), then `user` `[+0xc]`,
`cmd_id` `[+0x10]`, `buf` `[+0x14]`, `len` `[+0x18]`.

## Resolved calls (nm -C / objdump -d)

| call site | PLT address | symbol |
|---|---|---|
| `0x000bf2c5` | `0x00084f34` | `tss_dpsdk::DPMsgHandler::anticheck_pkg(tss_sdk_impl::UserInfo *, int)` (`0x000ae7c0`, size `0x13d`) |
| `0x000bf309` | `0x0007db64` | `tss_dpsdk::DPHackMgr::hack(tss_sdk_impl::UserInfo *, int, unsigned int, unsigned char *&, unsigned int &)` (`0x000aac90`, size `0x2c8`) |
| `0x000bf376` | `0x000873b4` | `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const *, ...)` (`0x001b3e70`) |

## Globals

The guard and the final suppression check dereference the GOT slot at
`ebx-0x188` = `0x002943ac`:

```
R_386_GLOB_DAT 002943ac -> 002bb4c0 _ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
```

So `glb_dp_config_` is an exported 4-byte pointer object; the binary loads its
value and dereferences it without a null check. DWARF offsets used:

- `tss_dp_comm::DPConfig::dp_base_conf` +0x0000 (`DPConfig` byte_size 22842).
- `DPConfigBase::is_use_dp` +0 (`unsigned char`), tested at `0xbf29b`.
- `DPConfigBase::disable_anticheat_pkg` +3 (`unsigned char`), tested at
  `0xbf390`.
- `DPConfig::cmd_hack` +0x3d7e (`DPCmdHack`, byte_size 7100); its members
  `up_cmd_hack` +0x26, `down_cmd_hack` +0x828, `raw_cmd_hack` +0x102a give the
  absolute `DPConfig` offsets `+0x3da4`, `+0x45a6`, `+0x4da8` used inside
  `DPHackMgr::hack` for hack types 1, 2 and everything else respectively.
- `UserInfo::base_info_` +0, `uin_` +0, `flags_` +0x20
  (`user_info_recon.h`).

## Decoded control flow

1. `0xbf28d` `mov eax,[this]`; `test`; `je 0xbf2a0` — `is_inited_` is at
   `this+0x00`; if zero, return **0** (not -1).
2. `0xbf293` `mov edx,[ebx-0x188]`; `mov eax,[edx]`; `cmp BYTE PTR [eax],0`;
   `jne 0xbf2b1` — if `glb_dp_config_->dp_base_conf.is_use_dp == 0`, return
   **0**. No null check on `glb_dp_config_`.
3. `0xbf2b1..0xbf2c5` — `msg_handler_` (`this+0x04`) →
   `DPMsgHandler::anticheck_pkg(user, cmd_id)`; result kept in `edi`.
4. `0xbf2cc..0xbf2d7` — compute `user_valid = (user != 0)` into
   `[ebp-0x11]`; if `res != 0` skip the hack pass; if `user == 0` skip it too.
5. `0xbf2df..0xbf309` — only for `res == 0 && user != 0`:
   `hack_manager_` (`this+0x08`) → `hack(user, 4, cmd_id, buf, len)`.
   The call passes `&len` (address of stack slot `[ebp+0x18]`) as argument 5
   and `&buf` (address of stack slot `[ebp+0x14]`) as argument 4. Argument 2
   is the literal `4`, argument 3 is `cmd_id`.
6. `0xbf30e..0xbf317` — `sub eax,1; setne al; movzx eax,al; lea edi,[eax-1]`
   computes `res = (hack(...) == 1) ? -1 : 0` (hack returning 1 means the
   packet was intercepted; any other value, including 0, yields 0).
7. `0xbf320..0xbf32d` — reload `len` from `[ebp+0x18]` and `buf` from
   `[ebp+0x14]`: the hack call may have replaced both through its references.
   The updated values feed only the following log.
8. `0xbf32f..0xbf376` — `ZEN_Trace_LogMsg::debug_output`:
   - priority: `user != 0 && (user->base_info_.flags_ & 1) ? 3 : 2`
     (`RS_INFO` : `RS_DEBUG`); from `cmp eax,1; sbb ecx,ecx; add ecx,3`.
     The separate `user == 0` path at `0xbf3a0` forces priority 2 and uin 0.
   - format at `0x0021d50c`:
     `"[%u][%s], cmd_id=%u, res=%d, buf=%p, len=%u"`.
   - arguments in order: `user ? user->base_info_.uin_ : 0` (`%u`),
     `__func__` (`%s`, string `"anticheck_pkg"` at `0x0021d9a0`,
     `_ZZN11DPInterface13anticheck_pkg...E8__func__`), `cmd_id` (`%u`),
     `res` (`%d`), `buf` (`%p`), `len` (`%u`).
   - The log runs on every path after the handler (user null or not, clean or
     detected).
9. `0xbf37b..0xbf39a` — if `res == 0`, return **0**; else load
   `dp_base_conf.disable_anticheat_pkg`; if nonzero return **0**, otherwise
   return **-1**. (`mov edx,-1` then either falls to the `xor edx,edx`
   epilogue or returns -1.)

Source-level equivalent of the tail:

```cpp
if (res == 0)
    return 0;
if (glb_dp_config_->dp_base_conf.disable_anticheat_pkg)
    return 0;
return -1;
```

## Return codes

The method returns only `0` or `-1`:

- `0` — either entry guard failed (`!is_inited_` or `!is_use_dp`), or the
  handler returned 0 and hack did not intercept, or a failure result was
  suppressed by `disable_anticheat_pkg`.
- `-1` — handler returned nonzero (1 = cheating package detected) or hack
  returned exactly 1, and `disable_anticheat_pkg` is 0.

Note that guard failure returns 0 (allow), unlike the buffer methods that
return -1 on the same guards.

## Callee semantics (call-site evidence only; bodies not reconstructed)

- `DPMsgHandler::anticheck_pkg` (`0xae7c0..0xae8fd`) returns only 0 or 1.
  The 1 path (`0xae854`) constructs a `DPBillAdapter`, appends `cmd_id` to the
  bill record, calls `DPMsgPacker::send_error_report_msg(user, 0x191, 1)` and
  `DPMsgPacker::send_cmd_monitor(user, cmd_id, 1, 0)`, then returns 1.
  Early exits return 0 when no cheating-package match is found.
- `DPHackMgr::hack` (`0xaac90..0xaaf58`) returns the first nonzero result of
  the invoked hacker objects (0 = untouched, 1 = intercepted). The outer
  method compares exactly `== 1`, not `!= 0`.
- `hack` argument 2 = `4` selects `DPConfig::cmd_hack.raw_cmd_hack`
  (`DPConfig+0x4da8`); it is the only type-4 call site in the binary
  (types 1/2 are the up/down command hacks used by the buffer methods).
  No symbolic enum name for these ints survives in DWARF.

## Buffer/len update semantics

`hack` receives references to this method's by-value stack argument slots.
It may replace the pointer and length (the original reads them back at
`0xbf320`/`0xbf327`), but because `buf` and `len` are plain by-value
parameters of `DPInterface::anticheck_pkg`, the replacements are not visible
to the caller of this method. `hack` can still mutate the buffer contents
in place. The following log prints the possibly replaced values.

## Local helper declarations (marked)

Declarations are declaration-only and ABI-faithful; they exist to emit the
exact mangled symbols and read exact offsets. They are not definitions of the
real classes and no protocol behavior is invented.

- `enum ZEN_LOG_PRIORITY { RS_TRACE=1, RS_DEBUG=2, RS_INFO=3, RS_ERROR=4,
  RS_ALERT=5, RS_FATAL=6 }` — same values as the other reconstructed units;
  the enum type is required by the mangled `debug_output` symbol.
- `namespace tss_dp_comm`: `DPConfigBase` (byte_size 6, only
  `is_use_dp`/`disable_anticheat_pkg` used), a partial `DPConfig` containing
  only `dp_base_conf` at +0, and `class DP_CONFIG` with
  `static DPConfig *glb_dp_config_`.
- `namespace tss_dpsdk`: partial `class DPMsgHandler` with
  `anticheck_pkg(UserInfo*, int)` and partial `class DPHackMgr` with
  `hack(UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)`.
- `dpinterface.h` is included unchanged. Its `DPMsgHandler`/`DPHackMgr`
  placeholders are global-scope classes while the real classes are
  `tss_dpsdk::...`; the private member pointers are therefore
  `reinterpret_cast` to the namespaced partial declarations inside the member
  function definition (legal access to `is_inited_`, `msg_handler_`,
  `hack_manager_`).

Per-TU partial views of `DPConfig` do not change any emitted symbol; the only
referenced symbol is `_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E`. Even so,
integration should place these views under the eventual shared
`dp_config.h`.

## Compile check

```
wsl -u root bash -lc "cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon && \
  g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_anticheck_agent.cpp \
  -o /tmp/dpinterface_anticheck_agent.o"
```

Result: `COMPILE_OK`.

Symbols from the object:

```
T _ZN11DPInterface13anticheck_pkgEPN12tss_sdk_impl8UserInfoEiPhj
U _ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
U _ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz
U _ZN9tss_dpsdk12DPMsgHandler13anticheck_pkgEPN12tss_sdk_impl8UserInfoEi
U _ZN9tss_dpsdk9DPHackMgr4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
```

The object disassembly reproduces the original call sequence: handler call
with `(user, cmd_id)`, conditional `hack(user, 4, cmd_id, &buf, &len)`, the
same log argument order/format, and the `disable_anticheat_pkg`-gated return.

## Explicit boundary

- Only `DPInterface::anticheck_pkg` is reconstructed. The bodies of
  `DPMsgHandler::anticheck_pkg`, `DPHackMgr::hack`, `debug_output`, the
  `DPConfig` object, and any bill/monitor helpers are not reconstructed here.
- No claim is made that the source is identical to the original source or
  that the output object is byte-identical to `0xbf270..0xbf3b2`; the report
  documents observed instruction-level behavior and exact symbols.
- No shared header, CMake, README, test, or other reconstructed unit was
  edited. No commit.
