# AGENT_REPORT_reload

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_reload_agent.cpp`

## Symbol / range

- Mangled symbol: `_ZN11DPInterface23check_and_reload_configEv`
- Source signature (DWARF): `int DPInterface::check_and_reload_config()`
- Address: `0x000c0de0`
- Extent: `0x000c0de0 .. 0x000c0ef6` (nm size `0x117`); `0xc0ef7..0xc0eff` is alignment padding before `DPInterface::check_timer(long)` at `0xc0f00`.
- Declared private in the existing `dpinterface.h`; that header was not edited.

## Decoded flow (all normal paths return 0)

| Address | Decoded action | DWARF line |
|---|---|---|
| `0xc0df4` | load `tss_dp_comm::DP_CONFIG::glb_dp_config_head_`; if NULL return 0 | dp_config.h:85 via dpinterface.cpp:80 |
| `0xc0e00` | compare `glb_dp_config_head_->update_time` (+0x00) with `glb_config_update_time_`; equal -> return 0 | same |
| `0xc0e0c` | call `tss_dp_comm::reload_dp_config()`; non-zero -> return 0 | dpinterface.cpp:80 |
| `0xc0e1f` | call `tss_dpsdk::DPBillAdapter::reload()` (static, return ignored) | dpinterface.cpp:82 |
| `0xc0e24` | call `tss_dpsdk::DPMonitorReporter::reload()` (static, return ignored) | dpinterface.cpp:84 |
| `0xc0e29` | load `this->msg_handler_` (`this + 0x04`), pass as `this` to `tss_dpsdk::DPMsgHandler::reload()` | dpinterface.cpp:86 |
| `0xc0e37` | load `glb_dp_config_`; `uin_monitor_list.length` (+0x3b78, u16) == 0 -> return 0 | dpinterface.cpp:88 |
| `0xc0e49..0xc0e7e` | `for (unsigned i = 0; i < glb_dp_config_->uin_monitor_list.length; ++i)`; reloads `glb_dp_config_` and the length after each call | dpinterface.cpp:88 |
| `0xc0e80..0xc0eda` | inlined `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()`: guard byte, `__cxa_guard_acquire`, construct `obj`, `__cxa_guard_release`, `__cxa_atexit(__tcf_1, NULL, __dso_handle)` | zen_singleton.h:15 / tss_sdk_comm_svc.h:292 |
| `0xc0e50` | `get_sdk_comm_svc()->get_user_info_by_uin(uin_monitor_list.values[i])`; values at +0x3b7a, stride 4 | dpinterface.cpp:91/92 |
| `0xc0e62..0xc0e66` | if user != NULL: `user->set_trace_flag()` -> `or DWORD PTR [user+0x20], 1` (UserInfo::base_info_.flags_) | dpinterface.cpp:94; user_info.h:59/64 |
| `0xc0e15..0xc0e1e` | common return 0 epilogue (`xor eax,eax; ret`) | dpinterface.cpp:99 |
| `0xc0edf..0xc0ef2` | compiler cleanup: `__cxa_guard_abort`, `_Unwind_Resume` | zen_singleton.h:15 |

The first two tests come from the inline helper `tss_dp_comm::check_dp_config()` (declared dp_config.h:82, body line 85): non-zero only when a header exists and `update_time != glb_config_update_time_`. The source-level condition at dpinterface.cpp:80 is therefore
`if (!check_dp_config() || reload_dp_config() != 0) return 0;`.

## Globals (nm + relocation-resolved GOT slots)

`__i686.get_pc_thunk.bx` sets GOT base `0x294534`; negative offsets reach `.got` at `0x294110`.

| GOT slot | Global symbol | Address |
|---|---|---|
| `0x294358` (`ebx-0x1dc`) | `tss_dp_comm::DP_CONFIG::glb_dp_config_head_` | `0x002bb4c4` |
| `0x29434c` (`ebx-0x1e8`) | `tss_dp_comm::DP_CONFIG::glb_config_update_time_` | `0x002bb4c8` |
| `0x2943ac` (`ebx-0x188`) | `tss_dp_comm::DP_CONFIG::glb_dp_config_` | `0x002bb4c0` |
| `0x2943e0` (`ebx-0x154`) | `_ZZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj` | `0x0029d820` |
| `0x294244` (`ebx-0x2f0`) | `_ZGVZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj` | `0x0029d7f0` |
| `0x2941f4` (`ebx-0x340`) | `__dso_handle` | `0x00297900` |

DWARF layouts used:

- `tss_dp_comm::DP_CONFIG`: 280 bytes; `update_time` unsigned int at +0. It also declares the three statics above.
- `tss_dp_comm::DPConfig`: 22842 bytes; `uin_monitor_list` at +0x3b78.
- `tss_dp_comm::DPValueArray_128`: 514 bytes packed; `length` u16 at +0, `values` u32[128] at +2.
- `tss_dp_impl::UserInfo`: `base_info_` at +0, 100 bytes; `flags_` at +0x20.
- `tss_sdk_impl::TssSdkCommSvc`: 40 bytes.
- `Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()` returns `TssSdkCommSvc*` and owns the function-local static named `obj`.

## Call targets (PLT resolved with `nm -C`)

| PLT stub | Target | Target address |
|---|---|---|
| `0x80ca4` | `tss_dp_comm::reload_dp_config()` (int) | `0x000a8fa0`, size `0x84` |
| `0x7da84` | `tss_dpsdk::DPBillAdapter::reload()` (static int) | `0x000a28e0`, size `0x131` |
| `0x80554` | `tss_dpsdk::DPMonitorReporter::reload()` (static int) | `0x000adad0`, size `0x7` |
| `0x7bde4` | `tss_dpsdk::DPMsgHandler::reload()` (non-static int) | `0x000ae290`, size `0x25` |
| `0x84314` | `tss_sdk_impl::TssSdkCommSvc::get_user_info_by_uin(unsigned int)` -> `UserInfo*` | `0x0008d8b0`, size `0xc52` |
| `0x7f324` | `tss_sdk_impl::TssSdkCommSvc::TssSdkCommSvc()` (C1) | `0x0008d100` |
| `0x7b5f4` | `__cxa_guard_acquire` | libc |
| `0x81b84` | `__cxa_guard_release` | libc |
| `0x7ca94` | `__cxa_guard_abort` | libc |
| `0x7eda4` | `__cxa_atexit` | libc |
| `0x86de4` | `_Unwind_Resume` | libgcc |

`__tcf_1` at `0x000beee0` destroys the singleton object and calls `TssSdkCommSvc::~TssSdkCommSvc()`.

## Semantic coverage (evidence-backed)

| ORIG behavior | Reconstruction | Evidence |
|---|---|---|
| `glb_dp_config_head_` null -> return 0 | `check_dp_config()` returns 0 | `0xc0df4..0xc0dfe`; DWARF `check_dp_config` at dp_config.h:82/85 |
| head `update_time` == `glb_config_update_time_` -> return 0 | same comparison in `check_dp_config()` | `0xc0e00..0xc0e0a`; DWARF `DP_CONFIG::update_time` +0, `glb_config_update_time_` `unsigned int` |
| `reload_dp_config() != 0` -> return 0 | same short-circuit condition | `0xc0e0c..0xc0e13`; PLT target `0x80ca4` -> `0xa8fa0` |
| `DPBillAdapter::reload()` then `DPMonitorReporter::reload()` | same order; both static `int`, returns ignored | `0xc0e1f`, `0xc0e24`; no `this` push; PLT targets `0xa28e0`, `0xadad0` |
| `msg_handler_->reload()` at `this+0x04` | same, via DWARF-correct `tss_dpsdk::DPMsgHandler*` | `0xc0e29..0xc0e32`; PLT target `0xae290`; DWARF `msg_handler_` at +4 |
| `uin_monitor_list.length` (u16 at +0x3b78) == 0 -> return 0 | same | `0xc0e37..0xc0e47`; DWARF `DPConfig::uin_monitor_list` +0x3b78 |
| `for (i = 0; i < length; ++i)` with length/value reloads per iteration | same `for`; global and length re-read | `0xc0e49..0xc0e7e`, `0xc0e6a..0xc0e7e` |
| singleton `instance()` guard/construct/atexit inline | `Singleton<T>::instance()` with function-local `static T obj` | `0xc0e80..0xc0eda`; zen_singleton.h:15, tss_sdk_comm_svc.h:292; guard/`obj` symbols exact |
| `get_sdk_comm_svc()->get_user_info_by_uin(values[i])` | same pointer-result call; uin loaded from +0x3b7a, stride 4 | `0xc0e50..0xc0e5d`, `0xc0e86`; PLT target `0x8d8b0` |
| `if (user) user->set_trace_flag()` -> `flags_ |= 1` at +0x20 | `user->base_info_.flags_ \|= 1u` | `0xc0e62..0xc0e66`; addr2line inline chain set_user_flag (user_info.h:64) / set_trace_flag (59) / dpinterface.cpp:94 |
| common epilogue returns 0 | `return 0`; early and final returns share epilogue | `0xc0e15..0xc0e1e`, `0xc0e70` etc.; final `return 0` at dpinterface.cpp:99 |
| guard-acquire failure cleanup | compiler-generated `__cxa_guard_abort` + `_Unwind_Resume` | `0xc0edf..0xc0ef2`; reproduced by the static-local template |

## Compile result

Primary (closest available target-generation compiler: GCC 4.1.2, c5root):

```
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -nostdinc \
    -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
    -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
    -isystem $C5/usr/include/c++/4.1.2 \
    -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
    -isystem $C5/usr/include/c++/4.1.2/backward \
    -isystem $C5/usr/include \
    -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_reload_agent.cpp \
    -o /tmp/dpinterface_reload_c5.o
```

Result: `C5_COMPILE_OK`.

Secondary syntax check (WSL GCC 13.3):

```
g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_reload_agent.cpp \
    -o /tmp/dpinterface_reload_agent.o
```

Result: `WSL_COMPILE_OK`.

Emitted/required symbols verified with `nm` on both objects:

- defined: `_ZN11DPInterface23check_and_reload_configEv` (T; size `0x120` under GCC 4.1.2)
- undefined: `_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E`
- undefined: `_ZN11tss_dp_comm9DP_CONFIG19glb_dp_config_head_E`
- undefined: `_ZN11tss_dp_comm9DP_CONFIG23glb_config_update_time_E`
- undefined: `_ZN11tss_dp_comm16reload_dp_configEv`
- undefined: `_ZN9tss_dpsdk13DPBillAdapter6reloadEv`
- undefined: `_ZN9tss_dpsdk17DPMonitorReporter6reloadEv`
- undefined: `_ZN9tss_dpsdk12DPMsgHandler6reloadEv`
- undefined: `_ZN12tss_sdk_impl13TssSdkCommSvc20get_user_info_by_uinEj`
- undefined: `_ZN12tss_sdk_impl13TssSdkCommSvcC1Ev`, `_ZN12tss_sdk_impl13TssSdkCommSvcD1Ev`
- defined weak/local under GCC 4.1.2: `_ZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEv` (W, `0x96`),
  `_ZZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj` (V, `0x28`),
  `_ZGVZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj` (V, `0x8`)

These unresolved externals are intentional: they are defined in the target library and other TUs
(`reload_dp_config` 0xa8fa0, `DPBillAdapter::reload` 0xa28e0, `DPMonitorReporter::reload` 0xadad0,
`DPMsgHandler::reload` 0xae290, `TssSdkCommSvc::C1` 0x8d100, `D1` 0x8d030,
`get_user_info_by_uin` 0x8d8b0).

## Instruction-level comparison (not identity)

Using GCC 4.1.2 and the mandated flags, normalized against the ORIG slice
(`0xc0de0..0xc0ef6`, alignment `lea esi,[esi+0x0]` at `0xc0e4d` stripped):

- ORIG instructions: **78**
- reconstructed instructions: **81**
- mnemonic-sequence ndiff: **3**; net instruction count is +3, from the guard-address loads/spill
  described below
- exact residual: GCC 4.1.2 hoists the loop-invariant singleton guard address out of the loop
  (`mov eax,[GOT:guard]` / `mov [ebp-0x10],eax` before the loop) and reloads it after the
  `jne` taken branch; ORIG reloads `[ebx-0x2f0]` at the loop top and reuses `eax` in the slow
  path. This is a GCC 4.1.0 vs 4.1.2 loop-invariant/register-allocation difference.
- Remaining text differences are link-layout placeholders: GOT-base addend and the
  `__tcf_*`/`.text` GOTOFF operand of the `__cxa_atexit` argument.
- The out-of-line `Singleton<TssSdkCommSvc>::instance()` body matches ORIG **40/40 instructions,
  ndiff_mnemonics = 0**; only the GOT-base addend and destructor GOTOFF operand differ.

Byte/instruction identity is **not** claimed. The difference is structural in the available
compiler generation, not a semantic gap.

## Integration boundaries (no identity claim)

These are local models, not recovered definitions:

1. `tss_dpsdk::DPBillAdapter` and `tss_dpsdk::DPMonitorReporter` are partial class declarations only because `dpinterface_deps.h` (not modified) does not declare `reload()`. Both are static and return `int`.
2. `tss_dpsdk::DPMsgHandler` is a partial class declared inside `tss_dpsdk`. The generated `dpinterface.h` models the member as a namespace-less `::DPMsgHandler*` (DWARF namespace artifact); the real DWARF type is `tss_dpsdk::DPMsgHandler*`, hence the `reinterpret_cast` in the body.
3. `tss_sdk_impl::TssSdkCommSvc` is declared with an opaque 40-byte layout and only the constructor, destructor and `get_user_info_by_uin` used here. The real 40-byte layout is `channel_id_`, `plugin_mgr_`, `dp_if_`, `sdk_channel_status_`, `error_info_`.
4. `Singleton<T>` is modelled minimally as `static T* instance() { static T obj; return &obj; }`. The real `zen_singleton.h` class also has a private `ObjectCreator`/`object_creator`, but the inline expansion in this function contains no reference to it; the emitted guard/`obj` symbols and constructor/atexit sequence match.
5. `tss_dp_comm::DP_CONFIG`, `DPConfig`, `DPValueArray_128` and `check_dp_config()` are local reconstructions of `dp_config.h`; pads preserve the DWARF offsets/sizes (`0x3b78`, 22842, 514) and the static member names produce the exact mangled references.
6. `UserInfo` comes from the existing `user_info_recon.h` (not modified). `set_trace_flag()` is an inline member in the original source; this unit writes `user->base_info_.flags_ |= 1u`, the exact inline expansion observed at `0xc0e66`.

No protocol behavior, success values, or extra validation were invented. `check_and_reload_config()` returns 0 on every normal exit, including when `reload_dp_config()` fails; that is the emitted behavior. Semantic correctness is claimed only for the decoded operations listed above; no runtime oracle was executed. Byte/instruction identity is **not** claimed (see the instruction-level comparison). Under the primary GCC 4.1.2 compile the guard/`obj` bindings match ORIG's weak objects (`V`); the secondary GCC 13 compile emits GNU-unique (`u`) bindings and a separate `.cold` clone, which are toolchain artifacts. The single source-spelling inference is the direct `flags_ |= 1u` in place of the inline `set_trace_flag()`; the addr2line inline chain and the observed `or DWORD PTR [eax+0x20],1` at `0xc0e66` are the evidence. Behavior of the external reload functions beyond their call sites and return conventions is not proven here.
