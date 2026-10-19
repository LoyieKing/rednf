# AntibotInterface policy/query reconstruction

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`,
ELF32 i386, GCC 4.1.0 SUSE, DWARF).
Source unit: `src/formmog/tss_sdk/adapter/antibotinterface.cpp`.
This report covers only the five methods below. It is **not** an identity claim
and the file is **not** integrated into a build.

## 1. Exact symbols and address ranges

Symbol sizes are from `nm -S -C`; the task's ranges include alignment padding
after each function.

| Method | Mangled symbol | Code range (size) | DWARF decl line |
|---|---|---|---|
| `IsCmdInDownList(unsigned int,int)` | `_ZN16AntibotInterface15IsCmdInDownListEji` | `0x00088670..0x000886e4` inclusive (`0x74`); padding to `0x886f0` | 652 |
| `IsCmdInUpList(unsigned int,int)` | `_ZN16AntibotInterface13IsCmdInUpListEji` | `0x000886f0..0x0008876a` inclusive (`0x7a`); padding to `0x88770` | 612 |
| `QueryProtectPolicy(int,int,unsigned int,SSafeModePkg*)` | `_ZN16AntibotInterface18QueryProtectPolicyEiijP12SSafeModePkg` | `0x00088bd0..0x00088d24` inclusive (`0x154`); next symbol `0x88d30` | 759 |
| `ProcessAntiBotInput()` | `_ZN16AntibotInterface19ProcessAntiBotInputEv` | `0x00089d50..0x00089e1b` inclusive (`0xcb`); next symbol `0x89e20` | 599 |
| `AntiCheatPacket(unsigned int,int)` | `_ZN16AntibotInterface15AntiCheatPacketEji` | `0x00089fd0..0x0008a131` inclusive (`0x161`). Main body ends at `0x8a0b8`; cold/landing pads run to `0x8a131` | 730 |

All five signatures are `int`-returning and are virtual in the original class
(vtable slots: UpList 12, DownList 13, AntiCheat 15, Query 16). The
reconstruction declares them non-virtual in a minimal standalone class; the
mangled names and 32-bit GCC calling convention are identical.

## 2. Decoded control flow

### `IsCmdInDownList` (`0x88670`, returns int)
1. Local `TssSdkPluginProtoProcPkg proto_proc_pkg` at `ebp-0x2c`.
2. `head_.msg_id_ = 305` (`TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST`).
3. `body_.down_list_pkg_.cmd_id_ = cmd_id` (pkg+4).
4. `body_.down_list_pkg_.idx_ = key` (pkg+0xc). **`uin_` (pkg+8) is never
   written** by this function.
5. `body_.down_list_pkg_.is_in_down_list_ = 0` (pkg+0x10).
6. Load `g_tss_sdk`, call table slot `+0x10` with `(3, &proto_proc_pkg)`.
7. `debug_debugEx("...res=%d", key, cmd_id, is_in_down_list_)`.
8. Return the raw `is_in_down_list_` value (no normalization).

Evidence: `0x88691`, `0x886a2`, `0x886a5`, `0x8869b`, `0x886a8-0x886b8`,
`0x886ba`, `0x886da`.

### `IsCmdInUpList` (`0x886f0`, returns int)
1. Local pkg at `ebp-0x2c`.
2. `head_.msg_id_ = 304` (`..._IS_CMD_IN_UP_LIST`).
3. `body_.decrypt_list_pkg_.cmd_id_ = cmd_id` (pkg+8).
4. `body_.decrypt_list_pkg_.idx_ = key` (pkg+0x10). **`uin_` (pkg+0xc) is
   never written.**
5. `body_.decrypt_list_pkg_.check_result_ = -1` (pkg+4).
6. Load `g_tss_sdk`, call slot `+0x10` with `(3, &proto_proc_pkg)`.
7. `debug_debugEx("...result=%d.", key, cmd_id, check_result_)`.
8. Return `(check_result_ == 0)` (`test`/`sete` at `0x8875a-0x88760`).

Evidence: `0x88714`, `0x8871b`, `0x8871e`, `0x88727`, `0x88721-0x88738`,
`0x8873a`, `0x8875a`.

### `QueryProtectPolicy` (`0x88bd0`, returns int)
1. If `safemode_pkg == NULL`: `debug_errorex("...invalid param,uin=%u,safemode_pkg=%p", uin, 0)`
   and `return -1` (`0x88bed`, `0x88cdc-0x88cfb`).
2. `debug_debugEx("...uin=%u,safemode_pkg=%p", uin, safemode_pkg)` (`0x88bf8-0x88c0c`).
3. Build `TssSdkPluginNotifyData{plugin_id_=4, notify_data_=safemode_pkg}`.
4. Function-local static `CTssCoreDumpPkgTrace<TssSdkPluginNotifyData>
   tmp_core_dump_info(__func__, "TssSdkPluginNotifyData")`; first call runs
   the guard/constructor path (`0x88c11-0x88cd7`).
5. Adapter enter copies the 8-byte notify_data into the static core_pkg_ and
   sets `is_in_func_=1`.
6. `rc = g_tss_sdk->notify(notify_data)` through table slot `+0x10`
   (`0x88c2b-0x88c52`).
7. Adapter leave clears `is_in_func_` (`0x88c6f`).
8. `debug_debugEx("...rc=%d", rc)`; `return (rc == 0)` (`0x88c68-0x88c6c`).

The SSafeMode package pointer is passed to the plugin **directly**; unlike the
other methods there is no wrapper package and no message-id word.

### `ProcessAntiBotInput` (`0x89d50`, returns int)
1. `unsigned int dummy = 0` (DWARF line 601; no separate stack slot survives,
   it is the value copied into the trace object).
2. Function-local static `CTssCoreDumpPkgTrace<unsigned int>
   tmp_core_dump_info(__func__, "unsigned int")`; guard at `0x89d6a`.
3. Adapter enter: `is_in_func_=1`, `core_pkg_=0` (`0x89d85-0x89d99`).
4. `call DWORD PTR [g_tss_sdk+0xc]` — table slot `+0x0c` (`proc`) with no
   arguments (`0x89d8c-0x89d9c`).
5. Adapter leave: `is_in_func_=0` (`0x89d9f`).
6. Return the value left in `eax` by `proc()`; there is **no** `xor eax,eax`
   or other normalization in the epilogue (`0x89da6-0x89db2`).

### `AntiCheatPacket` (`0x89fd0`, returns int)
1. Local pkg at `ebp-0x30`.
2. `head_.msg_id_ = 201` (`TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG`).
3. `body_.anti_cheating_pkg_.cmd_id_ = cmd_id` (pkg+8).
4. `body_.anti_cheating_pkg_.idx_ = key` (pkg+0x10). **`uin_` (pkg+0xc) is
   never written.**
5. `body_.anti_cheating_pkg_.game_pkg_ = 0` (pkg+0x14).
6. `body_.anti_cheating_pkg_.game_pkg_len_ = 0` (pkg+0x18).
7. `body_.anti_cheating_pkg_.is_cheating_pkg_ = 0` (pkg+4).
8. Build `TssSdkPluginNotifyData{plugin_id_=3, notify_data_=&proto_proc_pkg}`.
9. Function-local static
   `CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>
   tmp_core_dump_info(__func__, "TssSdkPluginProtoProcAntiCheatingPkg")`;
   guard at `0x8a009`.
10. Adapter enter copies the complete 24-byte anti_cheating_pkg_ into the
    static core_pkg_ (`0x8a02e-0x8a06e`).
11. `g_tss_sdk->notify(notify_data)` via slot `+0x10` (`0x8a072-0x8a082`).
12. Adapter leave (`0x8a0aa`).
13. `debug_debugEx("...is_cheating_pkg=%d.", key, cmd_id, is_cheating_pkg_)`.
14. Return raw `is_cheating_pkg_`.

## 3. Global, guard and one-time-initialization evidence

### SDK function-pointer table
- `tss_get_sdk` at `0x00091700` (unmangled, C linkage) returns the address of
  a static 28-byte struct at `0x00297944`.
- `g_tss_sdk` is a file-local pointer in `.bss` at `0x0029d064` (`nm`: 4-byte
  `b`). It is assigned once in `__static_initialization_and_destruction_0`:
  `0x890bb call tss_get_sdk@plt` / `0x890c0 mov [ebx+0x8b30],eax`.
- Table layout from DWARF (verified against this object's DWARF):

| Offset | Member |
|---|---|
| `+0x00` | `init` |
| `+0x04` | `init_plugins` |
| `+0x08` | `fini` |
| `+0x0c` | `proc` |
| `+0x10` | `notify(tss_sdk_if::TssSdkPluginNotifyData)` |
| `+0x14` | `get_last_error_code` |
| `+0x18` | `get_last_error` |

All `notify` calls are indirect through `[g_tss_sdk+0x10]`; the only `proc`
call is through `[g_tss_sdk+0xc]`.

### `TssSdkPluginNotifyData`
DWARF size 8, no padding: `plugin_id_` (`ETssSdkPluginId`) at `+0`,
`notify_data_` (`void*`) at `+4`. Call sites place the id in `[esp]` and the
data pointer in `[esp+4]`, matching a by-value 8-byte struct argument.

### Function-local statics (`tmp_core_dump_info`)

| Method | Guard variable | Static object | Size | Layout |
|---|---|---|---|---|
| `QueryProtectPolicy` | `0x0029d068` (8 B) | `0x0029d070` | 12 | `bool is_in_func_` +0; `TssSdkPluginNotifyData core_pkg_` +4 |
| `AntiCheatPacket` | `0x0029d080` (8 B) | `0x0029d088` | 25 | `bool is_in_func_` +0; `AntiCheatingPkg core_pkg_` +1 |
| `ProcessAntiBotInput` | `0x0029d0c8` (8 B) | `0x0029d0d0` | 8 | `bool is_in_func_` +0; `unsigned int core_pkg_` +4 |

One-time initialization (first call only):
1. `__cxa_guard_acquire` (`0x7b5f4`) on the 8-byte guard.
2. Constructor sets `is_in_func_ = false` (source line 53).
3. Constructor calls `ZEN_Trace_LogMsg::debug_infoex` (source line 55) with:
   - format `"CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p"` (`0x2185d0`)
   - `__func__` (`0x218db4` Query, `0x218dc7` AntiCheat, `0x218de6` Process)
   - type string (`0x218e97` `TssSdkPluginNotifyData`, `0x218d18`
     `TssSdkPluginProtoProcAntiCheatingPkg`, `0x2190a9` `unsigned int`)
   - `&is_in_func_`, `&core_pkg_`
4. `__cxa_guard_release` (`0x81b84`).
5. Exception landing pads call `__cxa_guard_abort` (`0x7ca94`) and
   `_Unwind_Resume` (`0x86de4`).

Adapter semantics (source lines 61/62/66):
- ctor: `is_in_func_ = true; core_pkg_ = pkg;`
- dtor: `is_in_func_ = false;`
- The adapter object itself is 4 bytes (a const reference to the static trace).

The `core_pkg_` offset `+1` for the anti-cheating trace proves the protocol
package structs have alignment 1 (packed); the reconstruction uses
`__attribute__((packed))`. Verified object symbols: guard sizes `8`, `8`, `8`
and static object sizes `0x0c`, `0x19`, `0x08`, exactly matching the binary.

`g_business` (`IBusiness*`) is at `0x0029d1a0`. It is not referenced by any
of these five ranges; its references are at `0x888c7`, `0x88a16`, `0x89182`,
`0x89375` (other methods).

## 4. Protocol struct layout evidence (DWARF, antibotinterface.cpp CU)

| Type | Size | Fields (offset) |
|---|---|---|
| `TssSdkPluginProtoProcPkg` | 36 | `head_` 0, `body_` 4 (union size 32) |
| `TssSdkPluginProtoProcHead` | 4 | `msg_id_` 0 |
| `TssSdkPluginProtoProcDecryptListPkg` (`._501`) | 16 | `check_result_` 0, `cmd_id_` 4, `uin_` 8, `idx_` 12 |
| `TssSdkPluginProtoProcDownPkg` (`._502`) | 32 | result/cmd/uin/idx/input_buf/input_len/output_buf/output_len |
| `TssSdkPluginProtoProcNotifyPkg` (`._503`) | 16 | `notify_result_` 0, `main_id_` 4, `buf_` 8, `buf_size_` 12 |
| `TssSdkPluginProtoProcAntiCheatingPkg` (`._504`) | 24 | `is_cheating_pkg_` 0, `cmd_id_` 4, `uin_` 8, `idx_` 12, `game_pkg_` 16, `game_pkg_len_` 20 |
| `TssSdkPluginProtoProcAntibotPkg` (`._505`) | 12 | `uin_` 0, `buf_` 4, `buf_size_` 8 |
| `TssSdkPluginProtoProcDpPkg` (`._506`) | 28 | decrypt_result/idx/uin/buffers |
| `TssSdkPluginProtoProcDownListPkg` (`._507`) | 16 | `cmd_id_` 0, `uin_` 4, `idx_` 8, `is_in_down_list_` 12 |
| `SSafeModePkg` | 44 | `Head` 0 (size 6: `MsgType` short 0, `Uin` uint 2), `Body` 6 (size 38) |

Message ids used: 201 (`IS_CHEATING_PKG`), 304 (`IS_CMD_IN_UP_LIST`),
305 (`IS_CMD_IN_DOWN_LIST`), plugin id 3 (`PROTO_PROC`), plugin id 4
(`SAFEMODE`).

## 5. Format strings (exact)

| Address | Caller | String |
|---|---|---|
| `0x218260` | DownList | `In AntibotInterface::IsCmdInDownList, key=%u, cmd_id=%d, res=%d` |
| `0x2182a0` | UpList | `In AntibotInterface::IsCmdInUpList, key=%d, cmd_id=%d, result=%d.` |
| `0x218540` | Query null | `In AntibotInterface::QueryProtectPolicy, invalid param,uin=%u,safemode_pkg=%p` |
| `0x218590` | Query normal | `In AntibotInterface::QueryProtectPolicy, uin=%u,safemode_pkg=%p` |
| `0x218614` | Query result | `In AntibotInterface::QueryProtectPolicy, rc=%d` |
| `0x218d40` | AntiCheat | `In AntibotInterface::AntiCheatPacket, key=%u, cmd_id=%d, is_cheating_pkg=%d.` |
| `0x2185d0` | trace ctor | `CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p` |

## 6. Return-value table

| Method | Return |
|---|---|
| `IsCmdInDownList` | raw `down_list_pkg_.is_in_down_list_` (initialized 0) |
| `IsCmdInUpList` | `decrypt_list_pkg_.check_result_ == 0` (initialized −1) |
| `QueryProtectPolicy` | `-1` if `safemode_pkg == NULL`, otherwise `notify_rc == 0` |
| `ProcessAntiBotInput` | `g_tss_sdk->proc()` return value unchanged |
| `AntiCheatPacket` | raw `anti_cheating_pkg_.is_cheating_pkg_` (initialized 0) |

No policy answer is synthesized in this file: every result comes from the
plugin `notify` / `proc` call (or the explicit null-pointer path).

## 7. Resolved PLT targets

| Symbol | Definition | PLT stub used |
|---|---|---|
| `_ZN16ZEN_Trace_LogMsg13debug_debugExEPKcz` | `0x001b3dd0` | `0x7c9b4` |
| `_ZN16ZEN_Trace_LogMsg12debug_infoexEPKcz` | `0x001b3d80` | `0x84d14` |
| `_ZN16ZEN_Trace_LogMsg13debug_errorexEPKcz` | `0x001b3d30` | `0x80904` |
| `tss_get_sdk` | `0x00091700` | `0x80844` |
| `__cxa_guard_acquire` | imported | `0x7b5f4` |
| `__cxa_guard_release` | imported | `0x81b84` |
| `__cxa_guard_abort` | imported | `0x7ca94` |
| `_Unwind_Resume` | imported | `0x86de4` |

`nm -D -C` confirms `ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)`,
`debug_infoex(char const*, ...)` and `debug_errorex(char const*, ...)` are
variadic; the generated DWARF header incorrectly prints a single parameter.

## 8. Compile check

Command (WSL, 32-bit, C++98 ABI; no link):

```sh
wsl -u root bash -lc "cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon && \
 g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
 -c recon/src/formmog/tss_sdk/adapter/antibot_policy_agent.cpp \
 -o /tmp/antibot_policy_agent.o"
```

Result: `COMPILE_OK` (exit 0). `nm` on the object shows exactly:

```
T _ZN16AntibotInterface13IsCmdInUpListEji
T _ZN16AntibotInterface15IsCmdInDownListEji
T _ZN16AntibotInterface15AntiCheatPacketEji
T _ZN16AntibotInterface18QueryProtectPolicyEiijP12SSafeModePkg
T _ZN16AntibotInterface19ProcessAntiBotInputEv
```

plus GCC cold clones (`*.cold`) and the three local guard/static symbols.
DWARF on the object confirms `TssSdk` size 28 with `proc` at 12 and `notify` at
16; package sizes 36/24/16; trace sizes 12/8/25 with `core_pkg_` at 4/4/1.

## 9. Dependencies / unknowns

External, intentionally unresolved until link:
- `ZEN_Trace_LogMsg::debug_debugEx`, `debug_infoex`, `debug_errorex` (variadic
  static members; real symbols in the target library).
- `tss_get_sdk` (C linkage; returns the real `TssSdk*` table).

Locally declared because the generated headers do not parse or do not define
them:
- `TssSdk` (real DWARF type is anonymous `._486`; name is inferred).
- `tss_sdk_if::ETssSdkPluginId` / `TssSdkPluginNotifyData`.
- `TssSdkPluginProtoProc*` package types (real DWARF names are `._498`–
  `._509`; the public names are inferred from
  `tss_sdk_plugin_proto_proc.h` and the trace type strings).
- `CTssCoreDumpPkgTrace<T>` and its nested adapter (all original uses are
  inlined; no out-of-line symbols exist in the binary).
- `SSafeModePkg` is only forward-declared; no field is accessed here.

Deliberate observations, not bugs introduced by this reconstruction:
- `uin_` is left uninitialized in `IsCmdInDownList`, `IsCmdInUpList` and
  `AntiCheatPacket`; the original stores only `cmd_id_`, `idx_` and the
  result/sentinel fields.
- `IsCmdInDownList` and `IsCmdInUpList` do not use the trace static; only
  Query, Process and AntiCheat do.
- `QueryProtectPolicy` passes `SSafeModePkg*` directly to the safemode plugin;
  the plugin mutates the caller's structure.
- `ProcessAntiBotInput` returns `proc()`'s value directly; the disassembly has
  no explicit return-zero.

## 10. Boundary

- **Not identical**: this is a behavior/ABI reconstruction from disassembly and
  DWARF, not a byte-identical rebuild of the original TU. Compiler-generated
  code shape (stack protector, PIC thunks, cold partitions) differs; semantics,
  field offsets and call targets were matched.
- **Not integrated**: the file is standalone. `AntibotInterface` is a minimal
  declaration (no `IAntibotInterface` inheritance, no vtable, no
  `Initialize/AddClient/DeleteClient/...`). It was not added to CMake, not
  linked, and not exercised against the live library.
- **No IBusiness vtable call** exists in these five methods. The only indirect
  calls are through the `TssSdk` function-pointer table (`+0x0c` proc,
  `+0x10` notify). If integration expects `g_business`, that belongs to the
  other methods (`0x888c7`, `0x88a16`, `0x89182`, `0x89375`).
- Lead action: merge the five bodies into the real class header, replace the
  local dependency declarations with canonical headers if they parse, and
  decide whether to keep this TU's file-local `g_tss_sdk` or share one.
