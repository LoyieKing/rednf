# AGENT REPORT -- `antibotinterface.cpp` plugin callbacks (8 CUs functions)

| item | value |
|---|---|
| original CU | `src/formmog/tss_sdk/adapter/antibotinterface.cpp` (path in ORIG: `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/...`) |
| ORIG | `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394` (ELF32 i386, GCC 4.1.2, DWARF) |
| reconstruction | `recon/src/formmog/tss_sdk/adapter/antibot_callbacks_agent.cpp` |
| matrix | `recon/src/formmog/tss_sdk/adapter/AGENT_MATRIX_antibot_callbacks.tsv` |
| build | `sh c5build.sh recon/src/formmog/tss_sdk/adapter/antibot_callbacks_agent.cpp build/tmp/antibot_callbacks_agent.o` -> rc 0 (also clean with `-Wall -Wextra`) |
| evidence dumps | `build/tmp/antibot_cb_disasm.txt`, `build/tmp/antibot_cb_annot.txt` (strings resolved), `build/tmp/antibot_cb_lines.txt` (`objdump -dl`) |

Scope: exactly the eight callbacks that `AntibotInterface::Initialize()` stores into
`plugin_array`. The rest of the CU is covered by `antibot_client_agent.cpp`,
`antibot_packets_agent.cpp`, `antibot_policy_agent.cpp`,
`antibotinterface_ctor_agent.cpp` and `antibotinterface_trivial.cpp`; this file
does not redefine their symbols (`g_tss_sdk`, `g_antibot_if`, ... are untouched).

## 1. Symbols (ORIG `nm -S`)

| address | size | symbol | src lines |
|---|---|---|---|
| 0x00088770 | 0x40 | `process_safemode_mba_set_verify_status_cb_(void*)` | 219-237 |
| 0x000887b0 | 0x48 | `process_safemode_mba_set_verify_options_cb_(void*)` | 197-216 |
| 0x00088800 | 0x39 | `process_safemode_mba_set_safe_status_cb_(void*)` | 182-194 |
| 0x00088840 | 0x4e | `process_safemode_set_limit_status_cb_(void*)` | 156-179 |
| 0x00088890 | 0x55 | `process_safemode_msg_cb_(void*)` | 143-152 |
| 0x000888f0 | 0x82 | `process_safemode_cb_(void*)` | 241-281 |
| 0x00089120 | 0x140 | `send_buffer_to_client_cb_(void*)` | 94-102 |
| 0x00089260 | 0x1eb | `punish_user_cb_(void*)` | 106-139 |

Local statics emitted by two of them (ORIG addresses, sizes identical in the
reconstruction):

| ORIG address | size | symbol |
|---|---|---|
| 0x0029d1e8 | 0x0f | `send_buffer_to_client_cb_(void*)::tmp_core_dump_info` (guard at 0x0029d1e0, 8 B) |
| 0x0029d1c0 | 0x20 | `punish_user_cb_(void*)::tmp_core_dump_info` (guard at 0x0029d1a8, 8 B) |
| 0x00218e62 | 0x1a | `send_buffer_to_client_cb_(void*)::__func__` |
| 0x00218e52 | 0x10 | `punish_user_cb_(void*)::__func__` |

## 2. Globals used (not defined here)

| address | type (DWARF) | use |
|---|---|---|
| 0x0029d1a0 | `IBusiness *` (`g_business`) | vtable slot +0x00 in `send_buffer_to_client_cb_`, +0x08 in `process_safemode_msg_cb_`, +0x04 in `punish_user_cb_` |
| 0x0029d1a4 | `unsigned int` (`g_channel_id`) | arg 2 of slot +0x00, arg 1 of slot +0x04, first vararg of the punish log, `punish_info.iGameID` |

Both are declared `extern`, like `antibot_client_agent.cpp` does; the integration
owner supplies the single definition.

## 3. `IBusiness` re-entry (the only non-obvious ABI point)

The CU's DWARF contains `IBusiness` as a *member-less declaration*
(`DW_TAG_structure_type`, `DW_AT_declaration`, no byte size) - the interface is
defined on the game-server side (`ibusiness.h`), so the virtual method names are
**not recoverable from this library** (there is no `_ZTV*IBusiness` and no
`IBusiness::*` definition in the `.so`; only
`AntibotInterface::Initialize(IBusiness*, char*)` references the type).

What is recoverable is the layout and the call shape; all three sites do
`mov <global>,%edx ; mov (%edx),%ecx ; ... ; call *N(%ecx)` with the object as
the first stack argument:

| slot | call site | recovered argument list |
|---|---|---|
| +0x00 | `0x891bb` | `(pkg->busi_type_, g_channel_id, 0, pkg->uin_, pkg->buf_, (int)pkg->buf_size_)`, result used as `ret` |
| +0x04 | `0x893aa` | `(g_channel_id, 0, pkg->uin_, pkg->punish_mode_, &punish_info, punish_reason)`, result used as `rc` |
| +0x08 | `0x888d6` | `(sec_data)` - the callback's own pointer is forwarded unchanged, result discarded |

The reconstruction therefore models only the vptr (`struct IBusiness { IBusinessVtable *vtable_; }`)
and uses **positional** slot names (`slot_0x00`, `slot_0x04`, `slot_0x08`,
named after their byte offset). No invented API names are introduced.

## 4. Types / templates recovered from the CU DWARF (and the packed question)

`CTssCoreDumpPkgTrace<T>` (ctor lines 53/55, `enter()` 61/62, `leave()` 66) and
`CTssCoreDumpPkgTraceAdapter<T>` (ctor 71, dtor 75, member 81) are instantiated
twice in this CU. Their DWARF sizes decide the packing question:

| instantiation | size | `is_in_func_` | `core_pkg_` |
|---|---|---|---|
| `CTssCoreDumpPkgTrace<TssSdkPluginChannelGameChannelPkg>` | 15 | +0 | +1 |
| `CTssCoreDumpPkgTrace<tagAntibotPunishInfo>` | 32 | +0 | +4 |

So the template itself is **not** packed; only the packet types carry
`#pragma pack(1)` (`TssSdkPluginChannelGameChannelPkg` = 14 B,
`TssSdkPluginUserControlPunishUser` = 32 B, `SafeModePkg` = 90 B / `SafeModeBody`
= 84 B / `SafeModeHead` = 6 B). `tagAntibotPunishInfo` is a normal 28-byte struct
(member order `iGameID, iBanTimeSecond, iVerifyTimeout, iVerifyMaxErrorCount,
iVerifyLevel, punish_info_len, punish_info`; the first five are 4-byte ints), which
is exactly why the punish trace object is 32 B with `core_pkg_` at +4. Compile-time
`typedef char chk_...[...]` checks in the file pin all of these sizes.

Packet member names:
* `TssSdkPluginChannelGameChannelPkg` is *named by the trace string* at 0x218644 and
  its member names come from the anonymous DWARF type of this CU;
* `TssSdkPluginUserControlPunishUser` and the `SafeMode*` family are anonymous in
  this CU (`._513`, `._521`..`._532`); the names used here are the ones the already
  reconstructed user-control and safemode plugin CUs use for the *same* member
  names/offsets (cross-CU corroboration, no invented layout).

## 5. Per-function notes

### 5.1 The four log-only safemode callbacks
Each one casts `sec_data` to `SafeModePkg *` and calls one
`ZEN_Trace_LogMsg::debug_debugEx` with a fixed format string, then `xor eax,eax`
(returns 0). Line numbers and format-string addresses:

| function | log line | format string |
|---|---|---|
| `process_safemode_mba_set_verify_status_cb_` | 225 | 0x2182e4 `in process_safemode_mba_set_verify_status_cb_. uin=%u, seq=%u, result=%u` |
| `process_safemode_mba_set_verify_options_cb_` | 204 | 0x218330 `in process_safemode_mba_set_verify_info_cb_. uin=%u, seq=%u, verify_url_len=%d, verify_url=%s` |
| `process_safemode_mba_set_safe_status_cb_` | 187 | 0x218390 `in process_safemode_mba_set_safe_status_cb_. uin=%u, status=%u` |
| `process_safemode_set_limit_status_cb_` | 164 | 0x2183d0 `recv limit status safemode cb. ret=%d, uin=%u, limit_status=%d, unlock_url_len=%d, unlock_url=%s\n` |

Argument widths taken from the loads: verify status reads three dwords
(pkg+6/+0xa/+0xe); verify options reads dword, dword, `movswl` (signed 16-bit
`verify_url_len_` at pkg+0xe), pointer (pkg+0x10); safe status reads two dwords;
limit status reads four dwords + pointer and uses DWARF local `char *url_ptr`
(line 168) for the URL. The format string of
`process_safemode_mba_set_verify_options_cb_` really does say
`..._set_verify_info_cb_` - kept verbatim.

### 5.2 `process_safemode_msg_cb_` (0x88890)
`debug_debugEx("recv update safemode cb. uin=%u level=%d", head_.uin_,
(int)body_.safemode_update_.safemode_level_)` (0x218434; level is a `signed char`
read with `movsbl` at 0x888aa), then `g_business->vtable[+0x08](sec_data)` and
`return 0`.

### 5.3 `process_safemode_cb_` (0x888f0)
`switch (pkg->head_.msg_type_)` lowered to a 9-entry relative jump table at
`0x218d90` (base 0x294534, `mov -0x7b7a4(%ebx,%eax,4),%eax` / `add %ebx,%eax`,
range check `sub $0x12 / cmp $0x8,%ax / ja`). Decoded entries:

| msg_type | target | handler |
|---|---|---|
| 18 (0x12) | 0x88936 | `process_safemode_msg_cb_` |
| 19, 20, 22, 24 | 0x88920 | default: `debug_errorex("in process_safemode_cb_, unknown msg_type")` (0x218460), `return 0` |
| 21 (0x15) | 0x88962 | `process_safemode_set_limit_status_cb_` |
| 23 (0x17) | 0x88954 | `process_safemode_mba_set_safe_status_cb_` |
| 25 (0x19) | 0x8894a | `process_safemode_mba_set_verify_options_cb_` |
| 26 (0x1a) | 0x88940 | `process_safemode_mba_set_verify_status_cb_` |

All paths return 0; handlers are called with the cast `pkg` pointer.

### 5.4 `send_buffer_to_client_cb_` (0x89120)
Guard sequence at 0x89137-0x89150 (`__cxa_guard_acquire`), ctor at 0x891f5-0x8922d
(`is_in_func_ = 0`, `debug_infoex("CTssCoreDumpPkgTrace, func=%s, type=%s,
this=0x%p, core_pkg_=0x%p", __func__, "TssSdkPluginChannelGameChannelPkg", this,
&core_pkg_)` at 0x2185d0, `__cxa_guard_release`), then the adapter `enter()`
(`is_in_func_ = 1`, 14-byte copy of `*pkg` into `core_pkg_`), slot +0x00, log
(0x218668), `leave()` (`is_in_func_ = 0`), `return 0`.
`__func__` = 0x218e62 `send_buffer_to_client_cb_`.

### 5.5 `punish_user_cb_` (0x89260)
1. lines 108-131: `pkg` is cast to `TssSdkPluginUserControlPunishUser *`, then a
   local `AntibotPunishInfo punish_info` (DWARF line 109, CFA-relative fbreg -60 =
   ebp-0x34) is built at line 116:
   `iGameID = g_channel_id`, `iBanTimeSecond = pkg->punish_time_`,
   `iVerifyTimeout/iVerifyMaxErrorCount/iVerifyLevel = 0`,
   `punish_info_len = pkg->punish_info_len_`, `punish_info = pkg->punish_info_`.
   `punish_reason` (line 128) stays 0 unless `pkg->punish_reason_len_ != 0`
   (lines 129/131; the pointer load at 0x89372 is guarded).
2. line 125 log (0x21869c): `channel_id, uin, punish_mode, punish_duration,
   game_id, room_id, punish_info_len, punish_reason_len`; `game_id`/`room_id` are
   `movzwl` (u16) loads and the format uses `%u` for them.
3. line 126: the trace static (`__func__` 0x218e52, type string 0x219044
   `AntibotPunishInfo`), adapter enter copies the **local** `punish_info`
   (0x29d1c4..0x29d1df), not the packet.
4. line 135: slot +0x04 with `(g_channel_id, 0, pkg->uin_, pkg->punish_mode_,
   &punish_info, punish_reason)`.
5. line 136: `debug_debugEx("In punish_user_cb_, rc=%d.", rc)` (0x219056);
   adapter leave at 0x893bf; `return 0` at 0x893c9.

## 6. Deliberate deviations / open items

1. **`IBusiness` is opaque in this library.** Slot +0x00/+0x04/+0x08 names are
   positional; the argument lists are the ones read off the call sites. The vptr
   is modelled as the object's first dword (`vtable_`), which reproduces the
   `mov (%reg),%reg` at all three sites.
2. **`CTssCoreDumpPkgTraceAdapter<T>` member constness**: DWARF prints the member
   as `const CTssCoreDumpPkgTrace<T> &`, but the inlined `enter()`/`leave()` mutate
   the trace, so the reconstruction uses a non-const reference (no symbol is
   emitted either way; noted in the source).
3. **Dead stack temporary**: `punish_user_cb_` zeroes a second 28-byte block at
   ebp-0x58 (stores at 0x89277/0x8927e/0x89285/0x8928c/0x89293/0x892a0/0x892a7, all
   attributed to line 116) that is never read and has **no DWARF variable entry**.
   It is a compiler temporary and is not modelled (semantically invisible).
4. **`process_safemode_cb_` size**: 178 B vs 130 B. This GCC 4.1 build emits the
   PIC jump table *inside* `.text` (36 B at offset 0x1b0 of the symbol) whereas the
   ORIG keeps the same 9-entry table in `.rodata` (0x218d90, not counted in the
   symbol size); 12 further bytes are branch-target alignment padding
   (`lea 0x0(%esi),%esi`). Code without the in-text table: 142 B = +9.2 %.
5. `SafeModePkg`/`SafeModeBody`/... and `TssSdkPluginUserControlPunishUser` are
   anonymous in this CU; names were taken from the already reconstructed plugin
   CUs (identical member names and offsets), not invented.
6. `#include <iostream>` is not used; the ORIG's `__static_initialization_and_
   destruction_0` of this CU is not part of these eight symbols.

## 7. Verification log (all commands run in the repo root)

```
$ sh c5build.sh recon/src/formmog/tss_sdk/adapter/antibot_callbacks_agent.cpp \
      build/tmp/antibot_callbacks_agent.o
rc=0                                          # -Wall -Wextra also clean
$ nm -S build/tmp/antibot_callbacks_agent.o   # all 8 T symbols + 2 statics + 2 guards
$ sh symdiff.sh <ORIG> <obj> 'process_safemode|send_buffer_to_client_cb_|punish_user_cb_'
== ORIG-only ==            (empty)
== NEW-only ==
b guard variable for punish_user_cb_(void*)::tmp_core_dump_info
b guard variable for send_buffer_to_client_cb_(void*)::tmp_core_dump_info
b punish_user_cb_(void*)::tmp_core_dump_info
b send_buffer_to_client_cb_(void*)::tmp_core_dump_info
r punish_user_cb_(void*)::__func__
r send_buffer_to_client_cb_(void*)::__func__
== counts (orig new) == 8 / 14
   (the 6 NEW-only entries are local symbols; they exist in ORIG as local
    symbols too - nm -S shows the identical names/sizes - and only show up
    here because symdiff compares against the dynamic symbol table)
$ sh sizecmp.sh <obj> '<rx>'
matched=8 identical=4 differing=4
    491    445  0.906  punish_user_cb_(void*)
     85     87  1.024  process_safemode_msg_cb_(void*)
    320    328  1.025  send_buffer_to_client_cb_(void*)
    130    178  1.369  process_safemode_cb_(void*)     (see 6.4: table in .text)
$ sh extra_globals.sh <obj>   # no globals only in recon
$ sh strfind.sh  # every emitted literal
PRESENT  in process_safemode_mba_set_verify_status_cb_. uin=%u, seq=%u, result=%u
PRESENT  in process_safemode_mba_set_verify_info_cb_. uin=%u, seq=%u, verify_url_len=%d, verify_url=%s
PRESENT  in process_safemode_mba_set_safe_status_cb_. uin=%u, status=%u
PRESENT  recv limit status safemode cb. ret=%d, uin=%u, limit_status=%d, unlock_url_len=%d, unlock_url=%s
PRESENT  recv update safemode cb. uin=%u level=%d
PRESENT  in process_safemode_cb_, unknown msg_type
PRESENT  send buffer to client: uin=%u type=%d len=%d ret=%d
PRESENT  In punish_user_cb_, channel_id=%u, uin=%u, punish_mode=%d, punish_duration=%u, game_id=%u, room_id=%u, punish_info_len=%u, punish_reason_len=%d
PRESENT  In punish_user_cb_, rc=%d.
PRESENT  CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p
PRESENT  TssSdkPluginChannelGameChannelPkg
PRESENT  AntibotPunishInfo
PRESENT  send_buffer_to_client_cb_
PRESENT  punish_user_cb_
```
