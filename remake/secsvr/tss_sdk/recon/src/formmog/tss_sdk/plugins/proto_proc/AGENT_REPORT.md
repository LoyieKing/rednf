# DPInterface buffer methods — reconstruction report

Source file: `dpinterface_buffer_agent.cpp` (this directory).

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`,
ELF32 i386, SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Only the three `DPInterface` buffer methods below are provided. No shared
header, CMake, test, README or existing source was modified.

## Address ranges and symbols

| range | next symbol | mangled symbol |
|---|---|---|
| `0x000bf520`–`0x000bf7ea` | `0x000bf7f0` | `_ZN11DPInterface23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS3_Rj` |
| `0x000bf7f0`–`0x000bfa7e` | `0x000bfa80` | `_ZN11DPInterface23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEjPhjS3_Rj` |
| `0x000bfe00`–`0x000c04ea` | `0x000c04f0` | `_ZN11DPInterface23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS3_Rj` |

DWARF function DIEs: `dpinterface.cpp` lines 227, 180 and 277 respectively
(`DW_AT_low_pc` 0xbf520 / 0xbf7f0 / 0xbfe00). The ticket's end addresses
(0xbf7f0 / 0xbfa80 / 0xc04f0) are the following symbol/padding boundaries.

The object file contains exactly these three newly defined strong symbols
(plus a hidden compiler-generated `__x86.get_pc_thunk.bx`).

## Signatures

Signatures are taken from the DWARF declarations and confirmed by the exact
mangled names above and in the called PLT symbols.

```cpp
int DPInterface::recv_buffer_for_encrypt(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int buf_len,
    unsigned char *out, unsigned int &out_len);

int DPInterface::recv_buffer_for_encrypt(
    tss_sdk_impl::UserInfo *user, unsigned int cmd_id, unsigned char *buf,
    unsigned int buf_len, unsigned char *out, unsigned int &out_len);

int DPInterface::recv_buffer_for_decrypt(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int buf_len,
    unsigned char *out, unsigned int &out_len);
```

`DPInterface` layout used: `is_inited_` +0x00, `msg_handler_` +0x04,
`hack_manager_` +0x08 (shared `dpinterface.h`).

## Resolved calls (nm -C, PLT targets)

All calls below were matched to the .so's exported symbols:

| call | symbol |
|---|---|
| `0x873b4` | `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` |
| `0x7e4d4` | `tss_dp_comm::get_cmd_id(DPGameCmdStruct const&, unsigned char*, unsigned short, unsigned int&)` |
| `0x7db64` | `tss_dpsdk::DPHackMgr::hack(UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)` |
| `0x85af4` | `tss_dpsdk::DPMsgHandler::recv_buffer_for_encrypt(UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&)` |
| `0x7ea24` | `tss_dpsdk::DPMsgHandler::recv_buffer_for_decrypt(UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&, int, int)` |
| `0x87ce4` | `tss_dpsdk::DPMsgHandler::OnRecvPkgFromClient(UserInfo*, unsigned char*, unsigned int)` |
| `0x85504` | `tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int, unsigned char const*, unsigned int)` |
| `0x83464` | `tss_dpsdk::DPMonitorReporter::report_data(unsigned int, unsigned int)` |
| `0x81244` | `sec_proto::DPInterDataHead::unpack(char const*, unsigned int, unsigned int*, unsigned int)` |
| `0x88244` | `DPInterface::is_need_encrypt(UserInfo*, int)` (only the 6-arg encrypt overload) |

Widths that are easy to get wrong and are preserved:

- `hack` arg 4 is `unsigned char*&` (`RPh`), **not** `unsigned char**`; arg 2
  is `int` (hack type), arg 3 is `unsigned int` (cmd_id).
- `get_cmd_id` arg 3 is `unsigned short`; both call sites truncate with
  `movzx ..., WORD PTR [buf_len]`.
- `DPInterDataHead::unpack` arg 3 is `unsigned int*` (`used`), arg 4 is
  `unsigned int` (cut version, 0).
- decrypt's `out_len` is `unsigned int&`; `used` is DWARF type `size_t`
  (32-bit), `is_internal_pkg`/`res` are `int`.

## Config and user state used

`tss_dp_comm::DP_CONFIG::glb_dp_config_` is an exported 4-byte object holding
a `DPConfig*` (DWARF size 22842). Members (DWARF offsets):

- `dp_base_conf` +0x0000; first member `is_use_dp` at +0x00.
  This is the byte tested by both encrypt overloads and by decrypt's normal
  post-processing. It is **not** present at decrypt entry.
- `dp_up_cmd_hack` +0x3d7a (`DPGameCmdStruct`) — used by decrypt.
- `dp_down_cmd_hack` +0x3d7c (`DPGameCmdStruct`) — used by both encrypts.
- `DPGameCmdStruct` = `{ unsigned char cmd_id_type; unsigned char
  cmd_id_offset; }` (2 bytes).

`tss_sdk_impl::UserInfo` fields (`user_info_recon.h`):

- `base_info_.uin_` +0x00, passed to logs / bill dumps / hack.
- `base_info_.flags_` +0x20, bit 0: selects log priority 3 (`RS_INFO`) vs 2
  (`RS_DEBUG`) and gates bill dumps. Re-read after the handler call, so it is
  tested at each site rather than cached.
- `user_code_info_.client_state_` +0x64: internal-package packets are forwarded
  to `OnRecvPkgFromClient` only when it is not `ECS_NOT_READY` (0).

## Decoded control flow

### encrypt(user, buf, buf_len, out, out_len) — 0xbf520

1. `!is_inited_` → `-1`; `!glb_dp_config_->dp_base_conf.is_use_dp` → `-1`.
2. Log `"[%u][%s], buf=%p, buf_len=%u, out=%p, out_len=%u"`, priority
   3 if `user && flags&1` else 2, uin (0 when `user == 0`), `__func__`.
3. If `user && flags&1`: bill dump `"recv_buffer_for_encrypt before encrypt"`
   with `(uin, buf, buf_len)`.
4. Only when `user != 0`: `unsigned int cmd_id = 0`;
   `get_cmd_id(dp_down_cmd_hack, buf, (unsigned short)buf_len, cmd_id)`;
   on success call `hack(user, 2, cmd_id, buf, buf_len)`; `hack() == 1` → `-1`.
   `get_cmd_id` failure skips the hack and still calls the handler.
5. `msg_handler_->recv_buffer_for_encrypt(user, buf, buf_len, out, out_len)`.
6. Log result `"[%u][%s], res=%d, out_len=%u"` with current flag/uin.
7. `res == 0`: optional `"recv_buffer_for_encrypt after encrypt"` dump with
   `(uin, out, out_len)`, then `report_data(0x3fe, 1)`.
   `res != 0`: `report_data(0x3ff, 1)`. Return `res`.
   (The `user == 0` path performs the same handler/log/report sequence but
   skips steps 3–4 and logs with uin 0; the source models this with the same
   expressions, and the compiler reproduces the separate blocks.)

### encrypt(user, cmd_id, buf, buf_len, out, out_len) — 0xbf7f0

1. Same two entry guards.
2. Log `"[%u][%s], cmdid=%u, buf=%p, buf_len=%u, out=%p, out_len=%u"`.
3. Optional before-encrypt bill dump.
4. Only when `user != 0`: `hack(user, 2, cmd_id, buf, buf_len)`; `== 1` → `-1`.
5. `is_need_encrypt(user, (int)cmd_id)`; false → `-1`.
6. Handler, result log `"[%u][%s], res=%d, out_len=%u"`.
7. `res == 0`: optional after-encrypt dump, `report_data(0x3fe, 1)`;
   else `report_data(0x3ff, 1)`. Return `res`.

When `user == 0` steps 3–4 and the hack are skipped, but
`is_need_encrypt(0, cmd_id)` is still called (the binary does the same).

### decrypt(user, buf, buf_len, out, out_len) — 0xbfe00

1. Only `!is_inited_` → `-1` (**no** `is_use_dp` entry check).
2. Initial log `"[%u][%s], buf=%p, buf_len=%u, out=%p, out_len=%u"`; optional
   `"recv_buffer_for_decrypt before decrypt"` bill dump.
3. `size_t used = 0`; `DPInterDataHead head`;
   `head.unpack((const char*)buf, buf_len, &used, 0)`.
   If `res != 0 || used >= buf_len`: log
   `"..., DPInterDataHead unpack fail, res=%d, used=%d, buf_len=%d"` at
   priority 4 and return `-1`.
4. Compute `payload = buf + used`,
   `payload_len = buf_len - (unsigned short)used` (16-bit truncation is real).
5. `head.protocol_type_` dispatch (values confirmed; the enum/macro names were
   not recoverable from DWARF):
   - `0x1a` (special control): `user == 0` → `1`. Otherwise call the inline
     `hack_manager_->on_special_control(user, payload, payload_len, out,
     out_len)`, log `"[%u][%s], on_special_control res=%d, out_len=%d"`, then:
     result `== 1` → optional `"on_special_control"` bill dump and return `0`;
     any other result → return `1`.
   - `0x12` (internal package): `user == 0` → `1`. Otherwise unpack the same
     `head` again from `(payload, payload_len)` (same `used`, not reset).
     Failure duplicates the priority-4 log with `unpack fail(2)` and returns
     `-1`. On success: `payload += used`,
     `payload_len -= (unsigned short)used`, `out = internal_buf`,
     `out_len = 0xffff`, `is_internal_pkg = 1`.
   - otherwise `is_internal_pkg = 0` and everything stays unchanged.
   `internal_buf` is `unsigned char[65535]` (DWARF upper bound 65534).
6. Shared log `"[%u][%s], internal=%d, pkg_type=%d"` (uin/flag path for
   `user != 0`, literal 0/2 path for `user == 0`).
7. `msg_handler_->recv_buffer_for_decrypt(user, payload, payload_len, out,
   out_len, (int)head.protocol_type_, is_internal_pkg)`.
8. Log `"[%u][%s], res=%d, out_len=%d"`.
   `res != 0`: priority-4 log
   `"... decrypt fail res=%d, out_len=%d, internal=%d, pkg_type=%d"`,
   `report_data(0x401, 1)`, return `res`.
9. `res == 0`: optional bill dump (label
   `"recv_buffer_for_decrypt internal pkg"` when `is_internal_pkg`, else
   `"recv_buffer_for_decrypt after decrypt"`), then `report_data(0x400, 1)`.
10. Internal package: if `user->user_code_info_.client_state_ !=
    ECS_NOT_READY`, call `OnRecvPkgFromClient(user, out, out_len)` (return
    value ignored); then `out_len = 0` and return `1`.
11. Normal path, `user == 0` → `0`; `!is_use_dp` → `0`;
    `get_cmd_id(dp_up_cmd_hack, out, (unsigned short)out_len, cmd_id)` failure
    → `0`; else `hack(user, 1, cmd_id, out, out_len) == 1 ? 1 : 0`.

## Helper class reconstruction (DWARF-verified)

- `sec_proto::DPInterDataHead`: `{ uint16 packet_size_; uint16
  protocol_type_; uint8 data_type_; }`, size 5, packed. Public members.
  Only `unpack` is declared; its body is an external TDR decoder and was not
  reconstructed.
- `tss_dpsdk::IDPHacker`: vtable order from `_ZTVN9tss_dpsdk9IDPHackerE` and
  the embedded `E_EVENT_FUNC` macro string at .rodata 0x100add4:
  `init`, a pure slot (`__cxa_pure_virtual`, name unknown), `on_login`,
  `on_logout`, `proc`, `on_ban_mode`, `on_drop_cmd`, `on_verify_response`,
  `on_verify_msg`, `on_safe_mode`, `on_special_control` (slot 10 = vptr+0x28).
  The pure slot is declared as an unnamed placeholder; every other slot keeps
  the recovered signature from the macro string. Nothing except
  `on_special_control` is called.
- `tss_dpsdk::IDPHackerEventNotify`: vptr +0, `std::vector<IDPHacker*>
  hackers_[E_EVENT_COUNT]` at +4, size 112.
- `tss_dpsdk::E_EVENT`: `E_LOGIN`=0 … `E_SPECIAL_CONTROL`=8,
  `E_EVENT_COUNT`=9.
- `tss_dpsdk::DPHackMgr`: `std::vector<IDPHacker*> hackers_` +0x00,
  `IDPHackerEventNotify event_notify_` +0x0c, size 124.
  `on_special_control` is **inline-only** in the original (no out-of-line
  symbol in the .so; the call is inlined at 0xc025e–0xc02e0). Its body iterates
  `event_notify_.hackers_[E_SPECIAL_CONTROL]`, i.e. `this+0x70`, calling
  `hackers[i]->on_special_control(...)`, stopping on a non-zero result, and
  returning the last result (0 if none). It is rebuilt inline in the local
  `DPHackMgr` class; the generated object was checked to contain
  `mov edx,[edi+0x70]`, `[edi+0x74]`, index by 4, and `call [edx+0x28]`.

## Compile / verification

```text
wsl -u root bash -lc "cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon && \
g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
  -c recon/src/formmog/tss_sdk/plugins/proto_proc/dpinterface_buffer_agent.cpp \
  -I recon -o /tmp/dpinterface_buffer_agent.o"
```

Result: exit 0, no diagnostics. Compiler: g++ (Ubuntu 13.3.0) with `-m32`;
C++98 language mode, Itanium mangling. `-fno-stack-protector` was added only
so the object does not reference the host's `__stack_chk_fail_local`.

`nm` on the object:

```text
T _ZN11DPInterface23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS3_Rj
T _ZN11DPInterface23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEjPhjS3_Rj
T _ZN11DPInterface23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS3_Rj
U _ZN11tss_dp_comm10get_cmd_idERKNS_15DPGameCmdStructEPhtRj
U _ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
U _ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz
U _ZN9sec_proto15DPInterDataHead6unpackEPKcjPjj
U _ZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhj
U _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rjii
U _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rj
U _ZN9tss_dpsdk13DPBillAdapter4dumpEPKcjPKhj
U _ZN9tss_dpsdk17DPMonitorReporter11report_dataEjj
U _ZN9tss_dpsdk9DPHackMgr4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
U _ZN11DPInterface15is_need_encryptEPN12tss_sdk_impl8UserInfoEi
```

Every substantive undefined symbol above is present in the .so's `.dynsym`
(checked with `nm -D --defined-only`). The only remaining undefined symbols are
linker-provided (`_GLOBAL_OFFSET_TABLE_`) — none.

Generated code was spot-checked against the original: `+0x3d7c` in the first
encrypt, `+0x3d7a` in decrypt post-processing, `out_len = 0xffff`, the
`hack` argument order (`&buf`/`&buf_len`), the special-control container
offset +0x70 and virtual slot +0x28, and `report_data` ids 0x3fe/0x3ff/0x400/
0x401. Full instruction equality was not attempted.

## Boundaries / unresolved

- **Not byte-identical.** This is a C++ reconstruction; the object is not
  expected to match the original instruction stream. Control flow, call
  targets, argument widths and return values were decoded from the
  disassembly.
- **Not integrated.** Only `dpinterface_buffer_agent.cpp` and this report were
  written. No shared header/CMake/test/README edits, no commit, no full build.
- Helper bodies other than the inline `DPHackMgr::on_special_control` were
  not reconstructed: `get_cmd_id`, `hack`, all `DPMsgHandler` calls,
  `DPBillAdapter::dump`, `DPMonitorReporter::report_data`,
  `DPInterDataHead::unpack`, and `DPInterface::is_need_encrypt` are external
  calls; their semantics are only as described by the decoded control flow.
- Protocol-type values `0x12` (internal package) and `0x1a` (special control)
  are certain; the original enum/macro names were not found in DWARF or
  symbols, so literals are used with comments.
- Hack-type values `1` (decrypt/up) and `2` (encrypt/down) are certain; names
  were not recovered.
- The `0x3fe`/`0x3ff`/`0x400`/`0x401` monitor ids and the second argument `1`
  are passed through; their monitor semantics were not interpreted.
- `DPConfig` is a partial view: only `dp_base_conf` (+0), the two
  `DPGameCmdStruct` members (+0x3d7a/+0x3d7c) are named; the rest is opaque
  padding sized to reproduce the exact member offsets and the 22842-byte size.
- The shared `dpinterface.h` models `msg_handler_`/`hack_manager_` with global
  placeholder classes. The real classes are `tss_dpsdk::DPMsgHandler` and
  `tss_dpsdk::DPHackMgr`; this file casts the fields at each use. If the lead
  replaces the field types in a future header revision, the casts can go away.
- The binary dereferences `glb_dp_config_` without a null check in all three
  functions (and re-reads it in decrypt's post-processing); the reconstruction
  preserves exactly that.
- `dpinterface_deps.h` declares `ZEN_Trace_LogMsg::debug_output(int, ...)`.
  The binary's symbol takes `ZEN_LOG_PRIORITY` (mangled
  `...E16ZEN_LOG_PRIORITYPKcz`), so that declaration does not resolve to the
  same symbol. This file declares the enum correctly and does not include the
  deps header.
- `dpinterface_core.cpp` now reads the authoritative
  `DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp` byte for all three guards,
  matching ORIG at `0xbf3ee`, `0xbf49e`, and `0xbefb6`. The independent
  `g_tss_sdk_enable` switch and its declaration were removed. Full-library
  compile and runtime comparison remain pending.
