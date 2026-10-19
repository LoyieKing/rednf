# AGENT_REPORT_handler_core

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
SHA-256: `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`

Unit: `recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_core_agent.cpp`

This file is a partial reconstruction. It is **not** a byte-identical copy and
the local helper classes are not the complete original classes. The verdict is
semantic equivalence only (control flow, call targets, field offsets, argument
values, guards and return values), supported by the ORIG disassembly and DWARF
below.

## Symbols / ranges

| ORIG address | Size | Mangled symbol | Source signature (DWARF) |
|---|---|---|---|
| `0x000ae2f0` | `0x54` | `_ZN9tss_dpsdk12DPMsgHandlerC1Ev` | `DPMsgHandler::DPMsgHandler()` |
| `0x000ae350` | `0x54` | `_ZN9tss_dpsdk12DPMsgHandlerC2Ev` | `DPMsgHandler::DPMsgHandler()` |
| `0x000ae060` | `0x05` | `_ZN9tss_dpsdk12DPMsgHandlerD2Ev` | `DPMsgHandler::~DPMsgHandler()` |
| `0x000ae070` | `0x05` | `_ZN9tss_dpsdk12DPMsgHandlerD1Ev` | `DPMsgHandler::~DPMsgHandler()` |
| `0x000ae2c0` | `0x25` | `_ZN9tss_dpsdk12DPMsgHandler4initEv` | `int DPMsgHandler::init()` |
| `0x000ae290` | `0x25` | `_ZN9tss_dpsdk12DPMsgHandler6reloadEv` | `int DPMsgHandler::reload()` |
| `0x000ae080` | `0x8d` | `_ZN9tss_dpsdk12DPMsgHandler15is_need_decryptEPN12tss_sdk_impl8UserInfoEi` | `bool DPMsgHandler::is_need_decrypt(UserInfo*, int)` |
| `0x000ae730` | `0x8d` | `_ZN9tss_dpsdk12DPMsgHandler15is_need_encryptEPN12tss_sdk_impl8UserInfoEi` | `bool DPMsgHandler::is_need_encrypt(UserInfo*, int)` |
| `0x000ae7c0` | `0x13d` | `_ZN9tss_dpsdk12DPMsgHandler13anticheck_pkgEPN12tss_sdk_impl8UserInfoEi` | `int DPMsgHandler::anticheck_pkg(UserInfo*, int)` |

DWARF fixes `tss_dpsdk::DPMsgHandler` as a 4-byte class with the single private
member `vm_engine_` at `+0x00` (`dp_msg_handler.h:27`). `has-1`/`has-2` are the
same class; `is_need_*` return `bool`; the other three return `int`.

## Decoded flow

### Constructor (`0xae2f0` C1 / `0xae350` C2), lines 15/17/18

- `operator new(0x44)`.
- `DPVMBaseEngine::C1` on the allocation (`0xbd380`).
- store the pointer into `this->vm_engine_` (`[this+0x00]`).
- cold landing pad: `operator delete(ptr)` then `_Unwind_Resume`.
- no other member is initialized; no null check is emitted.

### Destructor (`0xae060` D2 / `0xae070` D1), lines 20/23

- only `push ebp; mov ebp,esp; pop ebp; ret`.
- empty body. `vm_engine_` is **not** deleted.

### init (`0xae2c0`), line 25/28/29

```
return vm_engine_->init();      // DPVMBaseEngine::init @ 0xbd200
```

### reload (`0xae290`), line 31/34/35

```
return vm_engine_->reload();    // DPVMBaseEngine::reload @ 0xbd1d0
```

### is_need_decrypt (`0xae080`), lines 63..71

1. `line 65`: default index =
   `glb_dp_config_->dp_code_info.up_cmd_list.cur_posi` (`DPConfig+0x2956`).
2. `line 66`: if `user != 0 && user->user_code_info_.up_cmd_list_pos_ >= 0`
   (`UserInfo+0x90`).
3. `line 68`: index = that signed short (zero-extended after the `>= 0` test).
4. `line 70`: search `up_cmd_list.cmd_lists[index]`.
   Emitted addressing is `config + 0x2950 + index*0x202`; the packed element
   has length at `+0x0e` and `values[0]` at `+0x10`
   (`up_cmd_list +0x2802 + 8 = +0x295e`).
5. loop (inlined `tss_dp_comm::is_value_exist<DPValueArray_128>`,
   `dp_config.h:32/36/40`):
   for `i < length`: if `values[i] > cmd_id` return false;
   if `values[i] == cmd_id` return true; otherwise continue.
6. `line 71`: no match -> `false`.

### is_need_encrypt (`0xae730`), lines 73..81

Same control flow with the down-list members:

- default index `down_cmd_list.cur_posi` (`DPConfig+0x3166`);
- user override `user->user_code_info_.down_cmd_list_pos_` (`UserInfo+0x92`);
- search `down_cmd_list.cmd_lists[index]`; address is
  `config + 0x3160 + index*0x202`, length at `+0x0e`, values at `+0x10`.

### anticheck_pkg (`0xae7c0`), lines 83..103

1. `line 86`: `if (user == 0 || !user->is_client_inited()) return 0;`
   The inlined `UserInfo::is_client_inited()` (`user_info.h:46`, body line 50)
   is exactly:
   `client_state_ > 4 || game_dynamic_decrypt_count_ != 0 ||
    internal_dynamic_decrypt_count_ != 0`
   (`int` state at `UserInfo+0x64`; counters at `+0xc8` and `+0xc4`).
2. `line 91`: `bool res = false;`
3. `line 92`: `res = is_value_exist(
       glb_dp_config_->dp_code_info.up_cmd_list.cmd_lists[
           user->user_code_info_.up_cmd_list_pos_], cmd_id);`
   The index is the raw signed short at `UserInfo+0x90`; ORIG emits `movsx`,
   so there is no clamp and no fallback to `cur_posi` here.
4. `line 95`: if `res`, construct
   `DPBillAdapter bill("recv cheat pkg", 0x1c, user);`
   (`0x21bd49` is the NUL-terminated string; `0x1c` is the unsigned-int ctor
   argument).
5. `line 96`: `bill << cmd_id;` — the inline template expands to
   `DPBillAdapter::bill_ << (long long)cmd_id`
   (`CommBillRecord::operator<<(long long)` at `0xd1590`, sign-extends the int).
6. `line 98`: `DPMsgPacker::send_error_report_msg(user, 0x191, 1);`
7. `line 100`: `DPMsgPacker::send_cmd_monitor(user, cmd_id, true, false);`
8. `line 103`: `return res;` — ORIG returns `1` on the found path and `0`
   otherwise; the bill object's destructor runs before the `1`.

## Globals / dependencies (PLT and GOT resolved with `nm -C` + `objdump -R`)

| Reference | Address / GOT | Call target |
|---|---|---|
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` | `0x002bb4c0`; GOT `0x2943ac` (`ebx-0x188`) | pointer object (`DPConfig*`) |
| `tss_dpsdk::DPBillAdapter::bill_` | `0x002b6e00`; GOT `0x294498` (`ebx-0x9c`) | `DPBillRecorder` static |
| `DPVMBaseEngine::C1` | PLT `0x841b4` | `0x000bd380` |
| `DPVMBaseEngine::init()` | PLT `0x81934` | `0x000bd200` |
| `DPVMBaseEngine::reload()` | PLT `0x81a04` | `0x000bd1d0` |
| `DPBillAdapter::C1` | PLT `0x7f7b4` | `0x000a27d0` |
| `DPBillAdapter::D1` | PLT `0x82fe4` | `0x000a26c0` |
| `CommBillRecord::operator<<(long long)` | PLT `0x836d4` | `0x000d1590` |
| `DPMsgPacker::send_error_report_msg(UserInfo*, int, int)` | PLT `0x7ea14` | `0x000b2920` |
| `DPMsgPacker::send_cmd_monitor(UserInfo*, unsigned int, bool, bool)` | PLT `0x7c404` | `0x000b2d10` |
| `operator new(unsigned int)` | PLT `0x842a4` | libstdc++ |
| `operator delete(void*)` | PLT `0x88274` | libstdc++ |
| `_Unwind_Resume` | PLT `0x86de4` | libgcc |

`is_value_exist<DPValueArray_128>` and `UserInfo::is_client_inited` have no
out-of-line symbols in the ORIG or in this object; both are inlined.

## Layout evidence

DWARF-derived layouts used by this file (all offsets absolute in `UserInfo` /
`DPConfig`):

- `tss_sdk_impl::UserInfo`: size `0x190`; `user_code_info_` `+0x64` (size
  `0x30`), `user_timer_` `+0x94` (size `0x14`), `user_stat_` `+0xa8` (size
  `0x34`), `hack_data_` `+0xdc`.
- `UserInfo::CodeInfo`: `client_state_` `+0x00` (absolute `+0x64`, signed
  enum/int), two bytes of tail padding make `up_cmd_list_pos_` `+0x2c`
  (absolute `+0x90`) and `down_cmd_list_pos_` `+0x2e` (absolute `+0x92`).
- `UserInfo::StatInfo`: `internal_dynamic_decrypt_count_` `+0x1c` (absolute
  `+0xc4`), `game_dynamic_decrypt_count_` `+0x20` (absolute `+0xc8`).
- `tss_dp_comm::DPConfig`: size `0x593a`; `dp_code_info` `+0x154`.
- `DPCodeInfo`: size `0x3a24`; `up_cmd_list` `+0x2802`, `down_cmd_list`
  `+0x3012`.
- `DPCmdLists`: size `0x810`; `cur_posi` `+0x00`, `lists_count` `+0x02`,
  `cur_crc` `+0x04`, `cmd_lists[4]` `+0x08`.
- `DPValueArray_128`: size `0x202` packed; `length` `+0x00`, `values[128]`
  `+0x02`.

These match the emitted ORIG offsets exactly: `[config+0x2956]`,
`[config+0x3166]`, `[user+0x90]`, `[user+0x92]`, `[user+0x64]`, `[user+0xc8]`,
`[user+0xc4]`, and the array length/value displacements `+0x0e` / `+0x10`.

**Shared-header discrepancy (not edited):** the existing
`user_info_recon.h` omits the two bytes of padding before
`down_cmd_list_crc_`. It places `up_cmd_list_pos_` at code-info `+0x2a`
(absolute `0x8e`) and `down_cmd_list_pos_` at `+0x2c` (absolute `0x90`), while
DWARF and all three functions above prove `+0x2c`/`+0x2e` (absolute
`0x90`/`0x92`). This unit therefore defines an exact local `CodeInfo`; the
shared header was not modified. Consumers of that header should be checked
before integration.

## Compile result

Closest toolchain (GCC 4.1.2, matching the target's 4.1.x generation):

```
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
    -fno-stack-protector -I recon \
    -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_core_agent.cpp \
    -o /tmp/dp_msg_handler_core_agent.o
```

Result: `COMPILE_OK`. The same source also compiles with WSL
`g++ (Ubuntu 13.3.0) -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector` (`COMPILE_OK`); the 4.1.2 object is the comparison
artifact.

Defined symbols in the 4.1.2 object (all nine are the exact ORIG mangled
names; no helper/operator/function symbols are additionally defined):

```
_ZN9tss_dpsdk12DPMsgHandlerD2Ev
_ZN9tss_dpsdk12DPMsgHandlerD1Ev
_ZN9tss_dpsdk12DPMsgHandler15is_need_encryptEPN12tss_sdk_impl8UserInfoEi
_ZN9tss_dpsdk12DPMsgHandler6reloadEv
_ZN9tss_dpsdk12DPMsgHandler4initEv
_ZN9tss_dpsdk12DPMsgHandlerC1Ev
_ZN9tss_dpsdk12DPMsgHandlerC2Ev
_ZN9tss_dpsdk12DPMsgHandler15is_need_decryptEPN12tss_sdk_impl8UserInfoEi
_ZN9tss_dpsdk12DPMsgHandler13anticheck_pkgEPN12tss_sdk_impl8UserInfoEi
```

Undefined references are exactly the ORIG dependencies:
`_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E`, `_ZN14CommBillRecordlsEx`,
`_ZN9tss_dpsdk11DPMsgPacker16send_cmd_monitorEPN12tss_sdk_impl8UserInfoEjbb`,
`_ZN9tss_dpsdk11DPMsgPacker21send_error_report_msgEPN12tss_sdk_impl8UserInfoEii`,
`_ZN9tss_dpsdk13DPBillAdapter5bill_E`,
`_ZN9tss_dpsdk13DPBillAdapterC1EPKcjPN12tss_sdk_impl8UserInfoE`,
`_ZN9tss_dpsdk13DPBillAdapterD1Ev`,
`_ZN9tss_dpsdk14DPVMBaseEngine4initEv`,
`_ZN9tss_dpsdk14DPVMBaseEngine6reloadEv`,
`_ZN9tss_dpsdk14DPVMBaseEngineC1Ev`, `_Znwj`, `_ZdlPv`, `_Unwind_Resume`,
`__gxx_personality_v0`.

## Instruction comparison (semantic evidence, not identity)

ORIG slices were taken by `objdump -d -Mintel` at the addresses above; our
functions were sliced by their `nm -S` sizes. Counts include the cold EH
landing pads where present.

| Function | ORIG insns | Ours insns | Normalized residual |
|---|---:|---:|---|
| `D2` | 4 | 4 | none |
| `D1` | 4 | 4 | none |
| `init` | 14 | 14 | only PLT vs local symbol spelling and GOT-base immediate |
| `reload` | 14 | 14 | only PLT vs local symbol spelling and GOT-base immediate |
| `C1` | 26 | 26 | only PLT vs local symbol spelling and GOT-base immediate |
| `C2` | 26 | 26 | only PLT vs local symbol spelling and GOT-base immediate |
| `is_need_decrypt` | 53 | 52 | `is_value_exist` lowering: length load `movzx/test/movzx` vs `movzx/test`; loop tail `movzx eax,dx; cmp ecx,eax; ja` vs `cmp dx,cx; jne`; one alignment nop |
| `is_need_encrypt` | 53 | 52 | same residual as `is_need_decrypt` |
| `anticheck_pkg` | 102 | 101 | same helper lowering; register roles `eax/ecx` and `esi/ecx` vs `ecx` and `edx/eax`; otherwise same calls/offsets/returns |

The `is_need_*` and `anticheck` residuals are GCC 4.1.0 vs 4.1.2 codegen
choices: 4.1.2 compares the `unsigned short` loop index in 16 bits
(`cmp dx,cx`) and loads the length directly into the compare register, while
the ORIG 4.1.0 extends both to 32 bits. The branch decision, element stride,
length/values displacements, call targets and return values are the same.

No byte-identical claim is made. The constructor/destructor/init/reload
instruction sequences are operation-for-operation equal; the two list
predicates and `anticheck_pkg` are semantically equal by the decoded
`jb/je`-per-element decision procedure, but their machine code differs in the
listed codegen details.

## Integration boundaries (no identity claim)

1. `tss_sdk_impl::UserInfo`, `tss_dp_comm::DPConfig`/`DPCodeInfo`/
   `DPCmdLists`/`DPValueArray_128` and all helper classes are local partial
   views. Only the members used by these functions are named; pads preserve
   the DWARF offsets/sizes.
2. `tss_dpsdk::DPVMBaseEngine` is modelled as an opaque `0x44`-byte class with
   the default constructor and the two referenced methods. Its real base
   classes/virtual layout are not modelled.
3. `tss_dpsdk::DPBillAdapter` is modelled as a 4-byte class (`user_` only) with
   the external constructor/destructor and an inline template `operator<<`
   forwarding to the external static `bill_`. The real `DPBillRecorder`
   derives from `CommBillRecord` and has a virtual destructor; that vtable is
   irrelevant here because this TU neither constructs nor destroys `bill_`.
4. `CommBillRecord` is declared with only `operator<<(long long)`; the real
   class has many other overloads and data. The call selects the exact ORIG
   `long long` overload.
5. `DPMsgPacker::send_error_report_msg` and `send_cmd_monitor` are declared
   `static int` with the DWARF-confirmed signatures; their bodies are in other
   TUs.
6. The exact C++ spelling of `is_value_exist` is not recoverable from the
   binary. This reconstruction uses the `> then ==` form, which reproduces the
   ORIG decision `value < values[i] -> false; value == values[i] -> true;
   otherwise continue` for every element. The original may have been written
   as `values[i] >= value ? values[i] == value : continue`; both are
   observationally identical.
7. `is_client_inited` is reconstructed from the inlined body, not from a
   surviving definition. Its three-part predicate above is the exact
   condition emitted at `0xae7de..0xae7e7` and `0xae83e..0xae854`.
8. No shared header, README, CMake file, test, or other agent file was
   modified. No commit was made.
