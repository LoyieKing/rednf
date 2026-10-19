# DPMsgHandler send methods — semantic reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, GCC 4.1.0 family, `-O2 -fPIC`; SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).

Source: `dp_msg_handler_send_agent.cpp` (this directory). Only this `.cpp` and
this report were added; no shared header, CMake, test, README or existing
source was modified; no commit was made.

This is a **semantic** reconstruction. No byte/instruction identity is
claimed. The comparison below is against the ORIG function slices.

## 1. Symbols, ranges, signatures

ORIG ranges are half-open `[start, start+size)`; sizes are from `nm -S`.
The following symbol is the next symbol/padding boundary.

| ORIG range | next | mangled symbol |
|---|---|---|
| `0x000ae3b0`–`0x000ae57b` | `0x000ae580` | `_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoEPht` |
| `0x000ae580`–`0x000ae630` | `0x000ae630` | `_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoERN9sec_proto11DPInterDataEPht` |
| `0x000ae630`–`0x000ae723` | `0x000ae730` | `_ZN9tss_dpsdk12DPMsgHandler12SendBinCountEPN12tss_sdk_impl8UserInfoE` |
| `0x000ae900`–`0x000aea9f` | `0x000aeaa0` | `_ZN9tss_dpsdk12DPMsgHandler14SendClientCodeEPN12tss_sdk_impl8UserInfoE` |
| `0x000aeaa0`–`0x000aecfb` | `0x000aed00` | `_ZN9tss_dpsdk12DPMsgHandler11SendBinFileEPN12tss_sdk_impl8UserInfoE` |
| `0x000aed00`–`0x000aee4c` | `0x000aee50` | `_ZN9tss_dpsdk12DPMsgHandler11SendCmdListEPN12tss_sdk_impl8UserInfoE` |
| `0x000aee50`–`0x000af173` | `0x000af180` | `_ZN9tss_dpsdk12DPMsgHandler14SendServerCodeEPN12tss_sdk_impl8UserInfoE` |

Signatures (confirmed by the ORIG mangled names, DWARF declarations and the
observed call-site argument widths):

```cpp
int DPMsgHandler::SendDpMsgToClient(UserInfo*, unsigned char*, unsigned short);
int DPMsgHandler::SendDpMsgToClient(UserInfo*, sec_proto::DPInterData&,
                                    unsigned char*, unsigned short);
int DPMsgHandler::SendBinCount(UserInfo*);
int DPMsgHandler::SendClientCode(UserInfo*);
int DPMsgHandler::SendBinFile(UserInfo*);
int DPMsgHandler::SendCmdList(UserInfo*);
int DPMsgHandler::SendServerCode(UserInfo*);
```

All are non-virtual. `DPMsgHandler` has only `DPVMBaseEngine* vm_engine_` at
`+0x00` (DWARF size 4).

## 2. Layout / wire evidence used

`tss_sdk_impl::UserInfo` comes from the existing `user_info_recon.h`
(400 bytes). Fields used, all proven by both the ORIG disassembly and
compile-time `__builtin_offsetof` checks in the source:

| field | offset |
|---|---|
| `base_info_.uin_` | `+0x00` |
| `base_info_.client_version_` | `+0x18` |
| `base_info_.flags_` | `+0x20` |
| `user_code_info_.server_new_code_` | `+0x7a` |
| `user_code_info_.client_code_[0]` | `+0x7c` |
| `user_code_info_.bin_file_code_` | `+0x86` |
| `user_code_info_.down_cmd_list_crc_` | `+0x8c` |
| `user_code_info_.up_cmd_list_pos_` | `+0x90` |
| `user_code_info_.down_cmd_list_pos_` | `+0x92` |
| `user_stat_.internal_fix_encrypt_count_` | `+0xb0` |

`sec_proto` protocol structs are `pack(1)` (DWARF sizes 5/1580/1585 and body
at `+5`; compile-time checks in the source):

```text
DPInterDataHead: packet_size_ +0 (u16), protocol_type_ +2 (u16), data_type_ +4 (u8)
DPInterData:     head +0, body union +5; sizeof = 1585
DPClientCode:    code_id_ +0 (u32)
DPBinFile:       length +0 (u16), bin_code_id_ +2 (u16)
DPBinCount:      bin_count_ +0 (u16)
DPServerCode:    client_version_[16] +0, code_id_ +16 (u32)
```

`tss_dpsdk` code tables (natural layout; sizes/offsets cross-checked against
DWARF and ORIG displacement arithmetic):

```text
CodeItem<20000>:  weight_ +0, code_len_ +2, code_buf_ +4, sizeof 20004 (0x4e24)
CodeItems<20000>: total_weight_ +0, items_[100] +2, sizeof 2000402
CodeInfo:         cryptdata_code_ +0, crypt_code_ +200402, bin_file_ +400804
DpCodeInfo:       version_count_ +0, version_support_list_[8] +4,
                  version_hash_ +0x24, client_code_ +52 (0x34),
                  server_code_ +2401258 (0x24a3ea), sizeof 4802464 (0x4947a0)
DpCodeInfos:      code_update_time_ +0, code_count_ +4, code_infos_[2] +8
```

Effective ORIG/ours item addresses (compile-time asserts in the source):
`&items_[0] = DpCodeInfo+0x2ac190`, `items_[0].code_len_ = +0x2ac192`,
`items_[0].code_buf_ = +0x2ac194`, stride `0x4e24`, so
`items_[1].code_len_ = +0x2b0fb6`. The ORIG uses
`DpCodeInfo + 0x2ac18e + id*0x4e24`, then `+4` for `code_len_` and `+6` for
`code_buf_`; both reach the same effective addresses.

`tss_dp_comm` config prefix in the source is a partial packed model whose only
purpose is preserving the accessed offsets (compile-time assertions):
`dp_code_info` at `+0x154`, `cmd_list_buf_size` `+0x154`,
`cmd_list_buf` `+0x156`, `up_cmd_list.cur_posi` `+0x2956`,
`down_cmd_list.cur_posi` `+0x3166`, `down_cmd_list.cur_crc` `+0x316a`.
Other `DPConfig` members are not reconstructed.

## 3. Decoded flow

### 3.1 `SendDpMsgToClient(UserInfo*, unsigned char*, unsigned short)` — `0xae3b0`

1. Log priority: `flags_ & 1 ? 3 : 2`; if `user == 0`, uin 0 and priority 2.
   Calls `ZEN_Trace_LogMsg::debug_output(priority, "[%u][%s], buf_len=%u",
   uin, __func__, (unsigned)buf_len)`.
2. ORIG then dereferences `user` unconditionally for the bill flag; this is
   preserved. If `flags_ & 1`, `DPBillAdapter::dump(
   "SendDpMsgToClient before encrypt", uin, buf, buf_len)`.
3. Stack output buffer `unsigned char[65536]`; `out_len = 0xfff6`.
   Local `DPInterDataHead`: `protocol_type_ = 0x13`, `data_type_ = 0`.
4. `vm_engine_->encrypt(user, buf, buf_len, out_buf + 9, out_len, true)`.
   Nonzero return -> `-1`.
5. `head.packet_size_ = (unsigned short)(out_len + 5)`;
   `head.pack((char*)(out_buf + 4), 0xfffb, 0, 0)` (return ignored).
6. `AB_NS_CODEENGINE::EncodeInt(&p, out_len + 9)` with `p = out_buf`
   (writes a 4-byte big-endian total length at `out_buf[0]`).
7. `total_len = (unsigned short)(out_len + 9)`. If `flags_ & 1`, dump
   `"SendDpMsgToClient after encrypt"` with `(uin, out_buf, total_len)`.
8. `DPMsgPacker::send_dp_msg_to_client(user, out_buf, total_len)`.
   Nonzero -> return that result. Success -> increment
   `user_stat_.internal_fix_encrypt_count_` (`+0xb0`), return `0`.

Buffer layout: 4-byte length prefix (`EncodeInt`) + 5-byte outer head
(`pack`) + encrypted payload at `+9`; max payload `0xfff6`, max total
`0xffff`. The `pack` cut size is `0xfffb`.

### 3.2 `SendDpMsgToClient(UserInfo*, DPInterData&, unsigned char*, unsigned short)` — `0xae580`

1. `unsigned char out_buf[0xffff]; unsigned int len = 0;`
2. `data.pack((char*)out_buf, 0xffff, &len, 0)`.
3. If `buf != 0 && buf_len != 0`: `memcpy(out_buf + len, buf, buf_len);
   len += buf_len`.
4. Return the first overload with `(user, out_buf, (unsigned short)len)`.

No bill or monitor call in this overload.

### 3.3 `SendBinCount(UserInfo*)` — `0xae630`

1. Builds `DPInterData`: head `{packet_size_=7, protocol_type_=5,
   data_type_=0}`, body `bin_count_.bin_count_=5`.
2. `SendDpMsgToClient(user, data, 0, 0)`. Failure: bill
   `"send bin count fail"`, type `0xd`, then `-1` (no bill streams).
3. Success: debug log `[%u][%s], send bin count`;
   `DPMonitorReporter::report_data(0x3f6, 1)`; return `0`.

### 3.4 `SendClientCode(UserInfo*)` — `0xae900`

1. `code_len = 0`;
   `code = vm_engine_->get_client_code(user, user->client_code_[0], code_len)`.
2. If `code == 0 || code_len == 0`: shared bill failure with message
   `"get client code fail"`, type `0xf`, streams `code_len` and
   `client_code_[0]`, return `-1`.
3. Build `DPInterData`: head `{packet_size_=code_len+9, protocol_type_=1,
   data_type_=0}`; body `client_code_.code_id_ = client_code_[0]`.
4. `SendDpMsgToClient(user, data, code, code_len)`. Failure: shared bill
   failure with message `"send client code fail"`, type `0xf`, same two
   streams, return `-1`.
5. Success: debug log
   `[%u][%s], send client code, code_len=%u, client_code_=%u` with
   `(uin, __func__, code_len, client_code_[0])`;
   `report_data(0x3f8, 1)`; return `0`.

### 3.5 `SendBinFile(UserInfo*)` — `0xaeaa0`

1. Locals: `bin_file_code = user->bin_file_code_` (`+0x86`),
   `client_version = user->base_info_.client_version_` (`+0x18`),
   `code_len = 0`.
2. `code_infos = vm_engine_->code_manager_->code_infos_`.
   Traverse `code_count_` entries at `DpCodeInfos+8`, stride `0x4947a0`.
   Skip `version_count_ == 0`; scan `version_support_list_[j]` for
   `j < version_count_` and accept the first value equal to
   `client_version` or `0`.
3. No match, range failure, or zero code length: shared bill failure
   (`"get bin file fail"`, type `0xb`) streaming `code_len` and
   `bin_file_code`; return `-1`.
4. Range check: `(unsigned short)(bin_file_code - 1) > 0x62` -> valid
   `bin_file_code` is `1..99`; index is `items_[bin_file_code]`.
5. `item = found_info->server_code_.bin_file_.items_[bin_file_code]`;
   `code_len = item.code_len_`; `code_len == 0` -> same failure.
6. Build `DPInterData`: head `{packet_size_=code_len+9, protocol_type_=3,
   data_type_=0}`; body `bin_file_.length = code_len+4`,
   `bin_file_.bin_code_id_ = bin_file_code`.
7. `SendDpMsgToClient(user, data, item.code_buf_, code_len)`. Failure:
   shared failure block with message `"send bin file fail"`, type `0xb`,
   same two streams; return `-1`.
8. Success: debug log
   `[%u][%s], send bin file, code_len=%u, bin_file_code_=%u`;
   `report_data(0x3f7, 1)`; return `0`.

The ORIG has an unreachable `je` after `add edi,0x6` at `0xaebfd` (flags from
the pointer add, no zero pointer possible). It is not reproduced; it has no
observable behavior.

### 3.6 `SendCmdList(UserInfo*)` — `0xaed00`

1. `SendDpMsgToClient(user,
   glb_dp_config_->dp_code_info.cmd_list_buf,
   glb_dp_config_->dp_code_info.cmd_list_buf_size)`.
2. Failure: bill `"send cmd list fail"`, type `0x13`, streams
   `cmd_list_buf_size`; return `-1`.
3. Success updates the user state from the live config:
   `up_cmd_list_pos_ (+0x90) = up_cmd_list.cur_posi (+0x2956)`,
   `down_cmd_list_pos_ (+0x92) = down_cmd_list.cur_posi (+0x3166)`,
   `down_cmd_list_crc_ (+0x8c) = down_cmd_list.cur_crc (+0x316a)`.
4. Debug log
   `[%u][%s], send cmd list, up_cmd_list_pos_=%u,
   down_cmd_list_pos_=%u, down_cmd_list_crc_=%u`.
5. `report_data(0x3fa, 1)`; return `0`.

### 3.7 `SendServerCode(UserInfo*)` — `0xaee50`

1. `code_len = 0`;
   `code = vm_engine_->get_server_code(user, server_new_code_ (+0x7a),
   code_len)`.
2. Failure (`code == 0 || code_len == 0`) or later send failure: shared bill
   failure, type `0x11`, message `"get server code fail"` or
   `"send server code fail"`, streams `code_len` and `server_new_code_`;
   return `-1`.
3. Build `DPInterData`: head `{packet_size_=code_len+0x19,
   protocol_type_=6, data_type_=0}`; body
   `server_code_.code_id_ = server_new_code_`.
4. Traverse the same code table as `SendBinFile`; on the first
   `version_support_list_[j] == client_version_ (+0x18) || == 0`, copy 16
   bytes `info.version_hash_ (+0x24)` into
   `server_code_.client_version_`. If no entry matches, the code still sends
   the packet with the uninitialized 16-byte field (ORIG behavior preserved).
5. `SendDpMsgToClient(user, data, code, code_len)`.
6. Success bill: message `"send server code"`, type `0x10`; streams in order:
   `code_len`, `bin_file_code_ (+0x86)`, `client_code_[0] (+0x7c)`,
   `server_new_code_ (+0x7a)`, `up_cmd_list_pos_ (+0x90, sign-extended)`,
   `down_cmd_list_pos_ (+0x92, sign-extended)`,
   `down_cmd_list_crc_ (+0x8c, zero-extended)`.
   `report_data(0x3f9, 1)` is called while the bill object is alive, then its
   destructor runs; return `0`.

## 4. Bill / monitor / return summary

| method | failure bill | bill type | bill streams | success monitor | returns |
|---|---|---:|---|---:|---|
| raw SendDpMsgToClient | dump before/after (only if `flags_&1`) | — | — | — | `-1` encrypt fail; send result; 0 ok |
| DPInterData overload | none | — | — | — | raw result |
| SendBinCount | `send bin count fail` | `0xd` | none | `0x3f6` | `-1` / 0 |
| SendClientCode | `get client code fail` / `send client code fail` | `0xf` | code_len, client_code_[0] | `0x3f8` | `-1` / 0 |
| SendBinFile | `get bin file fail` / `send bin file fail` | `0xb` | code_len, bin_file_code | `0x3f7` | `-1` / 0 |
| SendCmdList | `send cmd list fail` | `0x13` | cmd_list_buf_size | `0x3fa` | `-1` / 0 |
| SendServerCode | `get server code fail` / `send server code fail` | `0x11` | code_len, server_new_code | `0x3f9` | `-1` / 0 |
| SendServerCode success bill | `send server code` | `0x10` | 7 values listed above | (same method) | 0 |

`DPBillAdapter` failure objects stream `CommBillRecord::operator<<(long long)`
calls, matching ORIG. The success bill in `SendServerCode` is destroyed after
`report_data`, matching ORIG.

## 5. External dependencies

All resolved from the target's symbol table (`nm -C`); ORIG PLT addresses are
the call sites in the send slices.

| dependency | defined at | ORIG PLT |
|---|---:|---:|
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | `0x001b3e70` | `0x873b4` |
| `tss_dpsdk::DPVMBaseEngine::encrypt(UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&, bool)` | `0x000be820` | `0x866e4` |
| `sec_proto::DPInterDataHead::pack(char*, unsigned int, unsigned int*, unsigned int) const` | `0x0015d2a0` | `0x7bd34` |
| `AB_NS_CODEENGINE::EncodeInt(char**, unsigned int)` | `0x00198d30` | `0x7d764` |
| `tss_dpsdk::DPMsgPacker::send_dp_msg_to_client(UserInfo*, unsigned char*, unsigned short)` | `0x000b1ee0` | `0x85274` |
| `tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int, unsigned char const*, unsigned int)` | `0x000a2a70` | `0x85504` |
| `tss_dpsdk::DPBillAdapter::DPBillAdapter(char const*, unsigned int, UserInfo*)` | `0x000a27d0` | `0x7f7b4` |
| `tss_dpsdk::DPBillAdapter::~DPBillAdapter()` | `0x000a26c0` | `0x82fe4` |
| `tss_dpsdk::DPMonitorReporter::report_data(unsigned int, unsigned int)` | `0x000adae0` | `0x83464` |
| `sec_proto::DPInterData::pack(char*, unsigned int, unsigned int*, unsigned int) const` | `0x0015c8c0` | `0x87c54` |
| `tss_dpsdk::DPVMBaseEngine::get_client_code(UserInfo*, unsigned short, unsigned short&)` | `0x000bcf70` | `0x837a4` |
| `tss_dpsdk::DPVMBaseEngine::get_server_code(UserInfo*, unsigned short, unsigned short&)` | `0x000bce50` | `0x86d34` |
| `CommBillRecord::operator<<(long long)` | `0x000d1590` | `0x836d4` |
| `memcpy` | libc | `0x84904` |
| `tss_dp_comm::DP_CONFIG::glb_dp_config_` (B, 4 bytes, `DPConfig*`) | `0x002bb4c0` | GOT `ebx-0x188` |
| `tss_dpsdk::DPBillAdapter::bill_` (B, 0x1764-byte `CommBillRecord`) | `0x002b6e00` | GOT `ebx-0x9c` |
| recursive overload calls | — | `0x87cb4` (raw) / `0x7d934` (DPInterData) |

`__func__` strings: raw `0x21c3c8`, `SendCmdList` `0x21c40c`,
`SendClientCode` `0x21c418`, `SendBinFile` `0x21c427`,
`SendBinCount` `0x21c433`.

## 6. Compile / object verification

Compiler: the available GCC 4.1.2 (Red Hat 4.1.2-55) from
`/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin/g++`,
with the c5root 4.1.2 headers and `-nostdinc`.

Command (abbreviated; full include list is the c5root `isystem` set plus
`-I recon`):

```sh
c5root/usr/bin/g++ -m32 -O2 -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc <c5root isystem paths> \
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_send_agent.cpp \
  -o /tmp/dp_msg_handler_send_agent.o
```

Result: exit 0, no diagnostics. The object defines exactly the seven requested
strong symbols plus the compiler-generated `__i686.get_pc_thunk.bx`; no other
user-defined function or data symbol is emitted. Undefined symbols:

```text
_GLOBAL_OFFSET_TABLE_
_Unwind_Resume
_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
_ZN14CommBillRecordlsEx
_ZN16AB_NS_CODEENGINE9EncodeIntEPPcj
_ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz
_ZN9tss_dpsdk11DPMsgPacker21send_dp_msg_to_clientEPN12tss_sdk_impl8UserInfoEPht
_ZN9tss_dpsdk13DPBillAdapter4dumpEPKcjPKhj
_ZN9tss_dpsdk13DPBillAdapter5bill_E
_ZN9tss_dpsdk13DPBillAdapterC1EPKcjPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk13DPBillAdapterD1Ev
_ZN9tss_dpsdk14DPVMBaseEngine15get_client_codeEPN12tss_sdk_impl8UserInfoEtRt
_ZN9tss_dpsdk14DPVMBaseEngine15get_server_codeEPN12tss_sdk_impl8UserInfoEtRt
_ZN9tss_dpsdk14DPVMBaseEngine7encryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rjb
_ZN9tss_dpsdk17DPMonitorReporter11report_dataEjj
_ZNK9sec_proto11DPInterData4packEPcjPjj
_ZNK9sec_proto15DPInterDataHead4packEPcjPjj
__gxx_personality_v0
memcpy
```

These are exactly the ORIG helpers/globals used by the seven slices; no
invented helper or asm bridge exists.

## 7. Semantic comparison with ORIG

Instruction counts are from `objdump -d` slices, counting instruction lines
only. Call-target multisets/order are from the ORIG direct/PLT calls vs the
object's `R_386_PLT32`/`R_386_PC32` relocations, excluding the
`__i686.get_pc_thunk.bx` helper.

| function | ORIG insns | ours | Δ | call multiset | static call order |
|---|---:|---:|---:|---|---|
| raw `SendDpMsgToClient` | 124 | 121 | −3 | equal | equal |
| `DPInterData` overload | 50 | 49 | −1 | equal | equal |
| `SendBinCount` | 59 | 59 | 0 | equal | equal |
| `SendClientCode` | 107 | 104 | −3 | equal | equal |
| `SendBinFile` | 145 | 161 | +16 | equal | equal |
| `SendCmdList` | 89 | 88 | −1 | equal | equal |
| `SendServerCode` | 194 | 194 | 0 | equal | differs only by block placement* |

\* `SendServerCode`: ORIG places the shared failure-bill block after the
`SendDpMsgToClient` call in `.text`; ours places it before. Per execution path
the sequence is the same: `get_server_code` -> (failure bill and return −1)
or (`SendDpMsgToClient` -> success bill -> `report_data` -> bill dtor ->
return 0).

Key non-stack field offsets were checked in both disassemblies: user
`0x18/0x20/0x7a/0x7c/0x86/0x8c/0x90/0x92/0xb0`; config
`0x154/0x156/0x2956/0x3166/0x316a`; code-table `0x4/0x8/0x24/0x4947a0/0x4e24`;
effective item addresses `0x2ac192` (`code_len_`) and `0x2ac194`
(`code_buf_`), matching ORIG `0x2ac18e+4/+6`.

Residual differences are compiler/layout differences, not semantic gaps:

- GCC 4.1.0 (ORIG) vs GCC 4.1.2 (reconstruction) register/stack allocation and
  block placement. Frames: raw `0x1003c` both; wrapper `0x10038` both;
  `SendBinCount` `0x660` both; `SendClientCode` `0x668` vs ORIG `0x65c`;
  `SendBinFile` `0x67c` vs ORIG `0x66c`; `SendCmdList` `0x2c` vs ORIG
  `0x3c`; `SendServerCode` `0x66c` both.
- `SendBinFile` is 16 instructions larger because the unified failure/search
  layout materializes `code_len`/`fail_msg` differently; call multiset and
  per-path order are unchanged.
- The ORIG unreachable `je` at `0xaebfd` (after `add edi,0x6`) is not emitted.
- The 16-byte `version_hash_` copy is a `memcpy(...,16)` in source; ORIG
  expands it to four dword moves. Copied bytes and destination offset are
  identical.

No `ndiff`/identity claim is made. The evidence above is semantic: same
guards, packet field values/widths, output lengths, call targets, bill values,
monitor codes, and return codes on each path.

## 8. Boundaries / integration notes

- The `.cpp` contains **partial declarations** of `DPMsgHandler`,
  `DPVMBaseEngine`, `DPCodeMgr`, `DpCodeInfos`, `DpCodeInfo`, `CodeInfo`,
  `CodeItem`, `DPBillAdapter`, `CommBillRecord`, `DPMsgPacker`,
  `DPMonitorReporter`, `sec_proto::DPInterData*` and a partial
  `tss_dp_comm::DPConfig` prefix. They exist only to emit the exact mangled
  symbols and preserve the accessed offsets. They are not authoritative class
  definitions and must be replaced by the real headers during integration.
- `sec_proto::DPInterData::pack` and `DPInterDataHead::pack` are declared
  returning `int`; DWARF says `enum ErrorType`. On this 32-bit ABI the return
  width is 4 bytes and the mangled symbol is unchanged. The real declaration
  should be used when available.
- `ZEN_LOG_PRIORITY` enumerator names are placeholders
  (`ZEN_LOG_PRIORITY_DEBUG = 2`, `ZEN_LOG_PRIORITY_INFO = 3`); the ABI type
  name and the values are exact. `debug_output` is declared variadic.
- `CommBillRecord` is a partial class with only
  `operator<<(long long)`; `DPBillAdapter::bill_` is modeled as a static
  `CommBillRecord` object (ORIG: B, size `0x1764`).
- `DPVMBaseEngine` is modeled with three leading pointers so that
  `code_manager_` is at `+8`; its real size is 68 bytes and its other members
  are not reconstructed.
- `DPConfig` models only the 340-byte prefix and `dp_code_info` offsets
  required by `SendCmdList`; the remaining config fields are not
  reconstructed.
- `sec_proto::DPInterDataBody` names only the body members used by these
  methods; `raw_[1580]` forces the exact union size.
- External behavior not reimplemented here: `encrypt`, `get_client_code`,
  `get_server_code`, the two `pack` methods, `DPBillAdapter` construction /
  destruction / bill storage, `report_data`, and `memcpy`. Their call
  contracts are preserved as listed above.
- Preserved ORIG edge behavior: the raw send path dereferences `user` for the
  bill flag even though its log path tolerates `user == NULL`;
  `SendServerCode` sends an uninitialized `server_code_.client_version_` when
  no code-info version matches; `SendBinFile` streams `code_len == 0` in its
  failure bill; failure bill objects are destroyed immediately before
  returning `-1`.
