# GenProto (Target A + B) — `formmog_proto_tss_sdk_qt.cpp` and `formmog_proto_public_uictransadapt.cpp` reconstruction

CUs (ORIG, tag 394), inside
`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`:

* **A** `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_proto_tss_sdk_qt.cpp`
  — 106 `sec_proto::` functions (DWARF-attributed).
* **B** `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_proto_public_uictransadapt.cpp`
  — 9 `sec_proto::` functions (DWARF-attributed).

Both also carry the CU's only out-of-line `tsf4g_tdr` definition,
`tsf4g_tdr::TdrWriteBuf::textize(char const*, ...)` (0x00136e90, 0x6c bytes),
which is **weak** in the ORIG (`nm -DC`: `W`) — the ORIG's TdrBuf.h defines that
varargs wrapper inside the class body (implicitly inline), so every CU that calls
it (uic, qt, uictransadapt, ...) emits its own comdat copy.  It is therefore
emitted by both TUs here as well.

`readelf -p .comment` of the ORIG shows `GCC 4.1.0 (SUSE Linux)`; the available
vendor toolchain is `c5root g++ 4.1.2 (Red Hat 4.1.2-55)` (see *divergences*).

Artifacts (all new, no commits):

* `recon/src/origsrc/formmog_proto_tss_sdk_qt.cpp` / `.h` — Target A TU (106
  functions + weak `textize`), byte-packed DWARF layouts, layout assertions.
* `recon/src/origsrc/formmog_proto_public_uictransadapt.cpp` / `.h` — Target B
  TU (9 functions + weak `textize`), layout assertions.
* `recon/src/origsrc/genproto_qt_matrix.tsv` — machine-readable
  `addr_orig / size_orig / size_recon / ratio / name` (115 CU rows + the shared
  `textize` row = 116 rows + header).
* `tsf4g_tdr_compat.h` — **reused unchanged**; no CU-specific compat header was
  needed (both CUs use only the binary-TDR primitives already modelled there:
  `writeUInt8/16/32`, `readUInt8/16/32`, `writeBytes`, `readBytes`,
  `writeCharWithNull`, `writeChar(char,unsigned)`, `getUsedSize`, `getTotalSize`,
  `getBeginPtr`, `_set`, `_free`, `set`, `vtextize`, `textize`, and
  `TdrBufUtil::{printVariable(…,const char*,…), printVariable(…,bool), printArray}`).
* `recon/link_check.sh` — **not modified**; neither new TU was added to it.
* `recon/src/origsrc/test_qt_oracle.cpp` + `oracle_qt.sh` (+`oracle_qt_mutations.py`)
  — behavioural differential oracle against the ORIG for all 115 CU symbols,
  `cases=49000 failures=0`, with a 6-mutation negative control (all caught);
  see §7.

## 1. Symbol table used (ORIG address / size)

Enumerated with
`python3 cu_symbols.py <ORIG> <cu-file-substring> --sizes` (DWARF
`DW_TAG_subprogram` DIEs with `DW_AT_low_pc` in the CU, names resolved through
`DW_AT_specification`), then cross-checked against `nm -DC -S --defined-only`:
every address/size below is identical in both.  Adding up the ranges shows the
enumeration is complete — the CU range `0x161b40..0x165bb0` of Target A contains
**exactly 106** defined symbols, all belonging to that CU, and Target B's range
`0x1551c0..0x155d80` contains **exactly 9** (the next symbol,
`sec_proto::DPInterData::construct()` at 0x155d80, belongs to another CU).

Signatures (return type and parameter types) were taken from the same DWARF (a
scratch pyelftools dump of the CU's `DW_TAG_subprogram` DIEs, resolved through
`DW_AT_specification`; 4.1 keeps name and type on the declaration DIE):
every `pack*`/`unpack*` overload returns
`TdrError::ErrorType`; the *non-union* `visualize_ex` overloads return
`const char*`; `Dp2QtMsgBody::visualize(long long, char*, …)` returns
`TdrError::ErrorType` and the union has **no** `visualize_ex` — all as the pilot
CU showed for `UserInputCheckPkg`.

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | tsf4g_tdr::TdrWriteBuf::textize(char const*, ...) | 108 | 108 | 1.000 |
| 001551c0 | sec_proto::ReqUICTransAdaptMsgForwardPkg::construct() | 53 | 53 | 1.000 |
| 00155200 | sec_proto::ReqUICTransAdaptMsgForwardPkg::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 904 | 890 | 0.985 |
| 00155590 | sec_proto::ReqUICTransAdaptMsgForwardPkg::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00155620 | sec_proto::ReqUICTransAdaptMsgForwardPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001556f0 | sec_proto::ReqUICTransAdaptMsgForwardPkg::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 001557b0 | sec_proto::ReqUICTransAdaptMsgForwardPkg::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 623 | 591 | 0.949 |
| 00155a20 | sec_proto::ReqUICTransAdaptMsgForwardPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00155ab0 | sec_proto::ReqUICTransAdaptMsgForwardPkg::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 595 | 559 | 0.939 |
| 00155d10 | sec_proto::ReqUICTransAdaptMsgForwardPkg::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00161b40 | sec_proto::CreateRoomRes::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 78 | 72 | 0.923 |
| 00161b90 | sec_proto::CreateRoomRes::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 58 | 61 | 1.052 |
| 00161bd0 | sec_proto::ChangeRoomType::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 80 | 80 | 1.000 |
| 00161c20 | sec_proto::RoomMaster::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 146 | 146 | 1.000 |
| 00161cc0 | sec_proto::RoleAction::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 148 | 148 | 1.000 |
| 00161d60 | sec_proto::ChangePosition::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 146 | 146 | 1.000 |
| 00161e00 | sec_proto::ChangeRole::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 80 | 80 | 1.000 |
| 00161e50 | sec_proto::EnterRoom::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 308 | 308 | 1.000 |
| 00161f90 | sec_proto::CreateRoomRes::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 80 | 80 | 1.000 |
| 00161fe0 | sec_proto::CreateRoom::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 146 | 146 | 1.000 |
| 00162080 | sec_proto::EnterChannel::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 80 | 80 | 1.000 |
| 001620d0 | sec_proto::Dp2QtMsg::construct() | 53 | 53 | 1.000 |
| 00162110 | sec_proto::ChangeRoomType::construct() | 13 | 13 | 1.000 |
| 00162120 | sec_proto::RoomMaster::construct() | 15 | 15 | 1.000 |
| 00162130 | sec_proto::RoleAction::construct() | 20 | 20 | 1.000 |
| 00162150 | sec_proto::ChangePosition::construct() | 15 | 15 | 1.000 |
| 00162160 | sec_proto::ChangeRole::construct() | 13 | 13 | 1.000 |
| 00162170 | sec_proto::EnterRoom::construct() | 22 | 22 | 1.000 |
| 00162190 | sec_proto::CreateRoomRes::construct() | 15 | 15 | 1.000 |
| 001621a0 | sec_proto::CreateRoom::construct() | 15 | 15 | 1.000 |
| 001621b0 | sec_proto::EnterChannel::construct() | 13 | 13 | 1.000 |
| 001621c0 | sec_proto::Login::construct() | 53 | 53 | 1.000 |
| 00162200 | sec_proto::Dp2QtMsgBody::construct(long long) | 304 | 301 | 0.990 |
| 00162330 | sec_proto::Login::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 443 | 433 | 0.977 |
| 001624f0 | sec_proto::Dp2QtMsgBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const | 1351 | 1330 | 0.984 |
| 00162a40 | sec_proto::Dp2QtMsg::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 204 | 205 | 1.005 |
| 00162b10 | sec_proto::EnterRoom::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00162ba0 | sec_proto::ChangeRoomType::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 38 | 42 | 1.105 |
| 00162bd0 | sec_proto::RoomMaster::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 68 | 69 | 1.015 |
| 00162c20 | sec_proto::ChangePosition::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 68 | 69 | 1.015 |
| 00162c70 | sec_proto::ChangeRole::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 38 | 42 | 1.105 |
| 00162ca0 | sec_proto::CreateRoom::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 68 | 69 | 1.015 |
| 00162cf0 | sec_proto::EnterChannel::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 38 | 42 | 1.105 |
| 00162d20 | sec_proto::ChangeRoomType::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 59 | 61 | 1.034 |
| 00162d60 | sec_proto::RoomMaster::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 92 | 82 | 0.891 |
| 00162dc0 | sec_proto::ChangePosition::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 92 | 82 | 0.891 |
| 00162e20 | sec_proto::ChangeRole::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 59 | 61 | 1.034 |
| 00162e60 | sec_proto::CreateRoom::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 92 | 82 | 0.891 |
| 00162ec0 | sec_proto::EnterChannel::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 59 | 61 | 1.034 |
| 00162f00 | sec_proto::ChangeRoomType::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00162f70 | sec_proto::RoomMaster::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00162fe0 | sec_proto::ChangePosition::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00163050 | sec_proto::ChangeRole::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 001630c0 | sec_proto::CreateRoomRes::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00163130 | sec_proto::CreateRoom::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 001631a0 | sec_proto::EnterChannel::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00163210 | sec_proto::CreateRoomRes::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 001632a0 | sec_proto::CreateRoom::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163330 | sec_proto::EnterChannel::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 001633c0 | sec_proto::Login::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163450 | sec_proto::Dp2QtMsg::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 001634e0 | sec_proto::ChangeRoomType::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163570 | sec_proto::RoomMaster::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163600 | sec_proto::RoleAction::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163690 | sec_proto::ChangePosition::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 00163720 | sec_proto::ChangeRole::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.000 |
| 001637b0 | sec_proto::EnterRoom::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 176 | 181 | 1.028 |
| 00163860 | sec_proto::EnterRoom::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 155 | 153 | 0.987 |
| 00163900 | sec_proto::EnterRoom::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00163970 | sec_proto::RoleAction::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 143 | 142 | 0.993 |
| 00163a00 | sec_proto::Login::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 270 | 271 | 1.004 |
| 00163b10 | sec_proto::Dp2QtMsgBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 468 | 468 | 1.000 |
| 00163cf0 | sec_proto::Dp2QtMsg::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 115 | 117 | 1.017 |
| 00163d70 | sec_proto::Login::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 270 | 271 | 1.004 |
| 00163e80 | sec_proto::Login::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00163ef0 | sec_proto::RoleAction::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 118 | 121 | 1.025 |
| 00163f70 | sec_proto::Dp2QtMsgBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int) | 468 | 468 | 1.000 |
| 00164150 | sec_proto::Dp2QtMsg::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 95 | 97 | 1.021 |
| 001641b0 | sec_proto::Dp2QtMsg::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00164220 | sec_proto::Dp2QtMsgBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int) | 122 | 122 | 1.000 |
| 001642a0 | sec_proto::RoleAction::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.000 |
| 00164310 | sec_proto::Dp2QtMsg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001643e0 | sec_proto::EnterChannel::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 001644a0 | sec_proto::EnterChannel::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00164530 | sec_proto::Login::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00164600 | sec_proto::Login::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 001646c0 | sec_proto::Login::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00164750 | sec_proto::Dp2QtMsg::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00164810 | sec_proto::Dp2QtMsg::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 001648a0 | sec_proto::Dp2QtMsgBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const | 198 | 198 | 1.000 |
| 00164970 | sec_proto::Dp2QtMsgBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const | 147 | 147 | 1.000 |
| 00164a10 | sec_proto::ChangeRoomType::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00164ae0 | sec_proto::ChangeRoomType::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00164ba0 | sec_proto::ChangeRoomType::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00164c30 | sec_proto::RoomMaster::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00164d00 | sec_proto::RoomMaster::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00164dc0 | sec_proto::RoomMaster::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00164e50 | sec_proto::RoleAction::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00164f20 | sec_proto::RoleAction::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00164fe0 | sec_proto::RoleAction::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00165070 | sec_proto::ChangePosition::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00165140 | sec_proto::ChangePosition::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00165200 | sec_proto::ChangePosition::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00165290 | sec_proto::ChangeRole::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00165360 | sec_proto::ChangeRole::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00165420 | sec_proto::ChangeRole::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 001654b0 | sec_proto::EnterRoom::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 00165580 | sec_proto::EnterRoom::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00165640 | sec_proto::EnterRoom::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 001656d0 | sec_proto::CreateRoomRes::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001657a0 | sec_proto::CreateRoomRes::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00165860 | sec_proto::CreateRoomRes::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 001658f0 | sec_proto::CreateRoom::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |
| 001659c0 | sec_proto::CreateRoom::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.000 |
| 00165a80 | sec_proto::CreateRoom::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.000 |
| 00165b10 | sec_proto::EnterChannel::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.985 |

(`textize` is listed once; it is emitted weakly by both TUs.  `ratio` is
`size_recon / size_orig`.)

## 2. Recovered semantics

### 2.1 Target A — `formmog_proto_tss_sdk_qt.cpp`

Twelve types, layouts from DWARF (`cu_layout.py`; the `._51` union of the DWARF
dump is the anonymous union of `Dp2QtMsgBody` — its members carry no
`DW_AT_name` and are promoted into `Dp2QtMsgBody`) and asserted at compile time
in the TU:

| type | sizeof | members (offset) |
|---|---|---|
| `Login` | 265 | uin_ 0 (u32), server_ip_ 4 (u32), name_len_ 8 (u8), name_ 9 (u8[256]) |
| `EnterChannel` | 1 | channel_id_ 0 (u8) |
| `CreateRoom` | 2 | room_type_ 0, visitor_count_ 1 |
| `CreateRoomRes` | 2 | room_id_ 0 (u16) |
| `EnterRoom` | 6 | room_id_ 0 (u16), room_type_ 2, visitor_count_ 3, role_type_ 4, role_position_ 5 |
| `ChangeRole` | 1 | role_position_ 0 |
| `ChangePosition` | 2 | role_position_ 0, role_type_ 1 |
| `RoleAction` | 5 | role_action_ 0, revert_ 1 (u32) — proves `#pragma pack(1)` |
| `RoomMaster` | 2 | master_role_type_ 0, master_role_position_ 1 |
| `ChangeRoomType` | 1 | room_type_ 0 |
| `Dp2QtMsgBody` | 265 | anonymous union of the ten above |
| `Dp2QtMsg` | 266 | cmd_id_ 0 (u8), body_ 1 |

Field order in `pack`/`unpack` is declaration order; field-by-field the ORIG's
`DW_AT_call_line` values (from `dwarf_inline_names.py`) prove the sequence, e.g.
`Login::pack`: 73/83 `writeUInt32`, 93 `writeUInt8`, 103 `writeBytes`; `unpack`
153/163/173/183.

`visualize` name/format operands are the ORIG's own `.rodata` references
(`dis_annotate.py` resolves the PIC `lea` operands; the object's literal set was
diffed against them, see §4):

| field | format |
|---|---|
| `Login::uin_`, `Login::server_ip_` | `"%u"` |
| `Login::name_len_` | `"%d"` |
| `Login::name_` | `printArray("[name]", name_len_)` then `textize(" %d", name_[i])` loop, then `writeCharWithNull(separator)` |
| `EnterChannel::channel_id_` | `"%d"` |
| `CreateRoom::{room_type_,visitor_count_}` | `"%d"` |
| `CreateRoomRes::room_id_` | `"%d"` |
| `EnterRoom::{room_id_,room_type_,visitor_count_,role_type_,role_position_}` | `"%d"` |
| `ChangeRole::role_position_` | `"%d"` |
| `ChangePosition::{role_position_,role_type_}` | `"%d"` |
| `RoleAction::role_action_` | `"%d"` |
| `RoleAction::revert_` | `"%u"` |
| `RoomMaster::{master_role_type_,master_role_position_}` | `"%d"` |
| `ChangeRoomType::room_type_` | `"%d"` |
| `Dp2QtMsg::cmd_id_` | `"%d"`, then `printVariable("[body]", true)` and `body_.visualize((int64_t)cmd_id_, …)` |
| `Dp2QtMsgBody` arms `login`, `enter_channel`, `create_room`, `create_room_res`, `enter_room`, `change_role`, `change_position`, `role_action`, `room_master`, `room_type` | `printVariable("[<arm>]", true)` then the arm's `visualize` with `indent >= 0 ? indent + 1 : indent` |

Structural facts read out of the machine code:

* **construct()**: `Login` and `Dp2QtMsg` inline `memset(this, 0, sizeof(*this))`
  (ORIG passes 0x109 / 0x10a to `memset@plt`); the small bodies are the inline
  memset expansion (measured: 4+1 stores for `RoleAction`'s 5 bytes, 4+2 for
  `EnterRoom`'s 6, one 2-byte store for the two-byte bodies, one byte for the
  one-byte bodies), i.e. also `memset(this, 0, sizeof(*this))`.
* **union selectors**: 1 = `login`, 2 = `enter_channel`, 3 = `create_room`,
  4 = `create_room_res`, 5 = `enter_room`, 6 = `change_role`,
  7 = `change_position`, 8 = `role_action`, 9 = `room_master`,
  10 = `room_type` (ORIG: `xor $0x1`..`xor $0xa` on the low selector word OR-ed
  with the high word, in that order); unknown selectors return `TDR_NO_ERROR`.
* **cutVer**: no message of this CU has a version field and none of the 106
  functions reads its `cutVer` parameter; the union dispatchers pass the
  **constant 1** to each arm call (ORIG: `movl $0x1,0x8(%esp)`), and
  `Dp2QtMsg::pack`/`unpack` do the same for the body dispatcher.  The secsvr
  generator family uses the same literal form
  (`formmog_proto_antibot_client.cpp: trojans_.pack(destBuf, 1)`), so this is the
  generator's idiom, not a decoding artefact.
* **`Login::name_`** (unlike the bounded arrays of Target B) has **no**
  `length > sizeof(array) → TDR_ERR_REFER_SURPASS_COUNT` check in `pack`,
  `unpack` *or* `visualize`: the only guards are `writeUInt8`'s single-byte
  check, `writeBytes`/`readBytes`' capacity check (ORIG `mov $0xffffffff`),
  `readUInt8`'s single-byte check, and the `printArray` call itself.
* **`Dp2QtMsg::unpack`** has no `pkg_len`/version peek (this message has no
  head): `readUInt8(cmd_id_)`, then `body_.unpack((int64_t)cmd_id_, srcBuf, 1)`.
* The buffer-form wrappers are the pilot's shape: `buffer == NULL` →
  `TDR_ERR_NULL_POINTER_PARAMETER`; `visualize_ex(char*, …)` additionally
  returns `""` for `size == 0`; `visualize_ex(TdrWriteBuf&, …)` returns `""` for
  `data() == NULL || capacity() == 0`; the terminating write is
  `writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1)`.

### 2.2 Target B — `formmog_proto_public_uictransadapt.cpp`

`ReqUICTransAdaptMsgForwardPkg`, sizeof 33044, byte-packed
(`#pragma pack(1)`; DWARF puts `src_id_` at +2 and `cmd_id_` at +6):

`world_id_` 0 (u16), `src_id_` 2 (u32), `cmd_id_` 6 (u32), `need_encrypt_` 10
(u8), `is_encrypted_` 11 (u8), `cmd_buf_len_` 12 (u16), `cmd_buf_` 14
(u8[32768]), `rolename_len_` 32782 (u16), `rolename_` 32784 (u8[256]),
`client_ip_` 33040 (u32).

* `construct()` = `memset(this, 0, sizeof(*this))` (ORIG passes 0x8114).
* `pack`/`unpack` write/read the fields in declaration order (ORIG `.cpp`
  lines 73…173 / 223…323) with the two bounded-array guards
  `cmd_buf_len_ > 32768` (ORIG `cmp $0x8000,%ax; ja`) and `rolename_len_ > 256`
  (ORIG `cmp $0x100,%ax; ja`) → `TDR_ERR_REFER_SURPASS_COUNT`, the
  `writeBytes`/`readBytes` capacity guard → `TDR_ERR_SHORT_BUF_FOR_WRITE/_READ`,
  then one bulk `memmove`.
* `visualize` formats: `[world_id_]` `"%d"`, `[src_id_]` `"%u"`, `[cmd_id_]`
  `"%u"`, `[need_encrypt_]` `"0x%02x"`, `[is_encrypted_]` `"0x%02x"`,
  `[cmd_buf_len_]` `"%d"`, then the same `> 32768` guard, `printArray("[cmd_buf_]",
  cmd_buf_len_)`, a `textize(" %d", cmd_buf_[i])` loop and
  `writeCharWithNull(separator)` (ORIG `.cpp`:427); then `[rolename_len_]` `"%d"`,
  the `> 256` guard, `printArray("[rolename_]", rolename_len_)`, a
  `textize(" 0x%02x", rolename_[i])` loop and `writeCharWithNull(separator)`
  (ORIG `.cpp`:463); finally `[client_ip_]` `"%u"`.
* `cutVer` is unused by both `pack` and `unpack` (the ORIG never reads 0x18(%ebp)).

## 3. Commands run

```
# NB: every `_qt_scratch/...` entry below is a scratch helper written for this
# task (pyelftools dumps, the literal/call-site extractors, the renamed
# symdiff/sizecmp copies and the switch-shape experiment).  They are deleted
# after use; all other tools named here are the shared ones in tools/tss_sdk_recon.
# --- enumeration / layout (step 1) ---------------------------------------
python3 cu_symbols.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_qt.cpp --sizes
python3 cu_symbols.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_public_uictransadapt.cpp --sizes
nm -DC -S --defined-only /opt/.../libtss_sdk.so.1.0.394 > /tmp/orig_nm_dc.txt
python3 cu_layout.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_qt.cpp
python3 cu_layout.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_public_uictransadapt.cpp
python3 _qt_scratch/sig.py  <ORIG> <cu>       # DWARF return/parameter types
python3 -c '<range scan of nm -n -C -S>'      # 106 / 9 symbols inside the CU ranges

# --- per-function evidence (step 2) --------------------------------------
python3 dwarf_inline_names.py <ORIG> formmog_proto_tss_sdk_qt.cpp > /tmp/qt_inl.txt
python3 dwarf_inline_names.py <ORIG> formmog_proto_public_uictransadapt.cpp > /tmp/uicta_inl.txt
python3 _qt_scratch/map.py /tmp/orig_nm_dc.txt /tmp/qt_inl.txt 161b40 165bb0 > /tmp/qt_map.txt
objdump -d -C --start-address=0x161b40 --stop-address=0x165bb0 <ORIG> > /tmp/qt_dis.txt
objdump -d -C --start-address=0x1551c0 --stop-address=0x155d90 <ORIG> > /tmp/uicta_dis.txt
python3 dis_annotate.py <ORIG> /tmp/qt_dis.txt    > /tmp/qt_ann.txt
python3 dis_annotate.py <ORIG> /tmp/uicta_dis.txt > /tmp/uicta_ann.txt
python3 _qt_scratch/lits.py /tmp/qt_ann.txt       # per-function .rodata literals
python3 _qt_scratch/lits.py /tmp/uicta_ann.txt
python3 _qt_scratch/callsites.py <nm -n> /tmp/qt_dis.txt _Unwind_Resume

# --- build / verification (step 3) ---------------------------------------
sh c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_qt.cpp /tmp/genqt.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_public_uictransadapt.cpp /tmp/uicta.o -I recon/src/origsrc
sh symdiff.sh <ORIG> /tmp/genqt.o  'sec_proto::(Login|EnterChannel|CreateRoom|CreateRoomRes|EnterRoom|ChangeRole|ChangePosition|RoleAction|RoomMaster|ChangeRoomType|Dp2QtMsg|Dp2QtMsgBody)::'
sh symdiff.sh <ORIG> /tmp/uicta.o  'sec_proto::ReqUICTransAdaptMsgForwardPkg::'
sh sizecmp.sh /tmp/genqt.o <regex above>
sh sizecmp.sh /tmp/uicta.o 'sec_proto::ReqUICTransAdaptMsgForwardPkg::'
sh strfind.sh < <literal list>
nm -C --defined-only /tmp/genqt.o | grep -v 'sec_proto::'
objdump -r /tmp/genqt.o    | grep -c _Unwind_Resume
objdump -r /tmp/uicta.o    | grep -c _Unwind_Resume
bash obj_fn.sh /tmp/genqt.o "<symbol>" <len>        # per-function object disassembly
objdump -d --start-address=0x165b10 --stop-address=0x165bf0 <ORIG> | grep -c _Unwind_Resume
```

`symdiff.sh`/`sizecmp.sh` write fixed `/tmp` names that are currently owned by
another user (`/tmp/sd_orig.sym`, `/tmp/sc_orig.txt`, mode 0644 root), so they
were run through byte-identical copies with the temp prefix renamed
(`sed 's|/tmp/sd_|/tmp/genqt_sd_|g' symdiff.sh > _qt_scratch/symdiff.sh`, same
for `sizecmp.sh`) — only the temp-file names differ.  Helper scripts of this
task (`_qt_scratch/*.py`, the copies, and the switch-shape experiment) were
deleted after use; the shared `recon/src/origsrc/tsf4g_tdr_compat.h` and the
shared scripts were not modified.

## 4. Results

* **Build**: both TUs compile with `c5build` rc=0 (vendor GCC 4.1.2,
  `-m32 -O2 -fPIC -fno-implement-inlines`), i.e. every compile-time size/offset
  assertion in the two TUs passes (assertions cover all 12 Target A types and
  the Target B type, including the offset that proves the byte packing:
  `RoleAction::revert_ == 1`, `ReqUICTransAdaptMsgForwardPkg::src_id_ == 2`).
* **`symdiff.sh`**: `== ORIG-only ==` empty, `== NEW-only ==` empty,
  counts `106 / 106` (Target A) and `9 / 9` (Target B).
* **`sizecmp.sh`**: Target A `matched=106 identical=69 differing=37`
  (min ratio 0.891, max 1.105); Target B `matched=9 identical=5 differing=4`
  (min 0.939, max 1.034).  **Every** symbol is inside the ±12 % band; the four
  rows ≥5 % off are:

  | size_orig | size_recon | ratio | name | measured ORIG cause |
  |---|---|---|---|---|
  | 92 | 82 | 0.891 | `CreateRoom::pack(TdrWriteBuf&, unsigned)` (+ `ChangePosition`, `RoomMaster`) | register allocation on the two single-byte writes: the ORIG re-loads and increments the position in memory (`mov 0x4(%ecx),%edx; add $0x1,%edx; mov %edx,0x4(%ecx)`, and `addl $0x1,0x4(%ecx)` for the second byte), 4.1.2 keeps the position in a register and uses `lea 0x1(%ecx),%edx` / `lea 0x2(%ecx),%eax`; shorter code, same statements |
  | 38 | 42 | 1.105 | `EnterChannel::unpack(TdrReadBuf&, unsigned)` (+ `ChangeRoomType`, `ChangeRole`) | mirror image: the ORIG keeps the buffer in `%ecx` and uses the memory-operand `addl $0x1,0x4(%ecx)`, 4.1.2 spills the buffer to `%esi` (`push %esi`) and uses `lea`+store |
  | 58 | 61 | 1.052 | `CreateRoomRes::unpack(TdrReadBuf&, unsigned)` | same `lea`-vs-memory-operand difference on the single 16-bit read's position update |
  | 78 | 72 | 0.923 | `CreateRoomRes::pack(TdrWriteBuf&, unsigned)` | same `lea`-vs-reload difference on the 16-bit write |

  Smaller divergences (all ≤ ±3 %) are the same family plus the known
  4.1.2-vs-4.1.0 instruction-selection noise documented by the pilot CU
  (`rol`-recognised byte swaps, address CSE); e.g. `Login::pack/unpack`
  270 → 271, `Dp2QtMsg::pack` 115 → 117, `Dp2QtMsgBody::visualize` 1351 → 1330
  (jump table instead of the ORIG's compare chain, see *divergences*).
* **Literals**: every string literal emitted by the two TUs is PRESENT in the
  ORIG (`strfind.sh`): `%d`, `%u`, ` %d`, `0x%02x`, ` 0x%02x`, `""` and the 39
  `[field]` / `[arm]` names.  Stronger: the `.rodata.str1.1` contents of
  `/tmp/genqt.o` are exactly `%d`, `%u`, ` %d`, `""` and the 27 names that the
  ORIG's Target A CU references (`[uin] [server_ip] [name_len] [name]
  [room_type] [room_id] [visitor_count] [role_type] [role_position]
  [role_action] [revert] [master_role_type] [master_role_position] [channel_id]
  [cmd_id] [body] [login] [enter_channel] [create_room] [create_room_res]
  [enter_room] [change_role] [change_position] [room_master]`), with no extra
  literal; Target B's set is `%d`, `%u`, ` %d`, `0x%02x`, ` 0x%02x`, `""` and
  `[world_id_] [src_id_] [cmd_id_] [need_encrypt_] [is_encrypted_]
  [cmd_buf_len_] [cmd_buf_] [rolename_len_] [rolename_] [client_ip_]`.
* **Exception-handling shape** (proof for the `throw()` compensation):
  `/tmp/genqt.o` has 23 relocations to `_Unwind_Resume` and `/tmp/uicta.o` has 2.
  The ORIG has exactly the same numbers — Target A has 22
  `_Unwind_Resume@plt` sites in the disassembly window 0x161b40..0x165bb0 plus a
  third one past that window's end, in `EnterChannel::visualize_ex(char*, …)`
  (the CU's last function, 0x165b10..0x165bd9, whose landing pad sits at
  0x165bb1) = 23; Target B's 2 are in `visualize(char*)` /
  `visualize_ex(char*)`.  Each target is inside a `visualize*` wrapper that
  holds a `TdrWriteBuf` local, and **none** inside a
  `pack(char*)`/`unpack(char*)` wrapper — exactly what the `throw()` on the
  `pack`/`unpack` declarations reproduces.
* **Object symbol set**: `nm -C --defined-only /tmp/genqt.o` is exactly the 106
  `sec_proto::` symbols plus weak `tsf4g_tdr::TdrWriteBuf::textize` (0x6c bytes,
  ratio 1.000), `__i686.get_pc_thunk.bx` and `DW.ref.__gxx_personality_v0`
  (both present in the ORIG as well); `/tmp/uicta.o` likewise with its 9
  symbols.  No invented helper, alias or shim.

## 5. Divergences (compiler-generation artifacts, reported not chased)

ORIG = GCC 4.1.0, vendor toolchain = 4.1.2.  Four differences were found: three
are compensated in the source (each documented in the headers and in the TU
comments) and the fourth is instruction-selection noise that stays inside the
size band:

1. **`throw()` on `pack`/`unpack`** (compensation, pilot-identical): 4.1.2 emits
   a cleanup landing pad for every wrapper that holds a `TdrWriteBuf` local and
   calls a provably-nothrow member, which the ORIG does not have.  Measured on a
   `throw()`-stripped copy of this TU: the 11 `pack(char*, …)` wrappers grow
   133 → 166 bytes and the union's `pack(int64_t, char*, …)` 147 → 182 bytes
   (the 11 `unpack(char const*, …)` wrappers stay at 102, because `TdrReadBuf`
   has no destructor), and that object carries 35 instead of the ORIG's 23
   `_Unwind_Resume` relocations.  With `throw()` in place, 69/106 Target A
   symbols and 5/9 Target B symbols are byte-identical.
   `throw()` is not part of the Itanium mangling for an empty exception
   specification, so no symbol name changes.
2. **Ten-arm union dispatchers** (`Dp2QtMsgBody::construct/pack/unpack`):
   the ORIG expands them as a *decision tree* — all ten `xor`-based 64-bit
   selector comparisons first, each `je` to an arm body placed later.  4.1.2
   turns the equivalent ten-case 64-bit switch into a **jump table** (measured
   261 bytes for `construct` against the ORIG's 304, ratio 0.859, outside the
   ±12 % band).  The dispatchers are therefore spelled as the semantically
   identical `if / else if` chain with the same selector values, same arm calls
   and same default result (the parameter has no side effects and every
   comparison is evaluated at most once in both forms); `construct` then lands at
   301/304 and `pack`/`unpack` inside the band.  The switch form *is* kept for
   `Dp2QtMsgBody::visualize`, where the ten arm bodies are large enough that
   4.1.2's table is only 1330 bytes against the ORIG's 1351 (0.984, inside the
   band) — i.e. the table/tree choice only matters for the small dispatchers.
3. **`textize` binding** (compensation): the compat header declares
   `TdrWriteBuf::textize` out-of-line, so the TU definitions repeat the `inline`
   specifier to reproduce the ORIG's weak comdat copy (`W`, 0x6c bytes) instead
   of a strong `T` symbol — otherwise the two TUs (and the pilot's uic TU) would
   collide at link time with duplicate definitions.  Calls stay out-of-line PLT
   calls exactly as in the ORIG.
4. **Instruction-selection noise** (not compensated, all inside the band):
   `lea`-based vs reload+add/memory-operand position updates (see the ≥5 % rows
   above), 16-bit access byte-swaps that 4.1.2 recognises as `rol`, and address
   CSE.  These are the same 4.1.0/4.1.2 differences the pilot CU reported; the
   *statement* sequences are identical.

## 6. Residual uncertainty

* The `.cpp` line numbers quoted in the TUs are `DW_AT_call_line` values of the
  inlined TDR calls (evidence for the ORIG's call sequence and for the section
  order), not the ORIG's real statement layout.
* Whether the ORIG's union dispatchers were source-level `switch` statements
  (which 4.1.0 expanded as a tree) or `if/else if` chains is **not recoverable**
  from the binary; only the compensated form reproduces the ORIG's code shape on
  the 4.1.2 toolchain, so that form is used for the three small dispatchers.
* `Login::name_`'s missing `REFER_SURPASS_COUNT` guard is an observed property of
  the ORIG machine code (no such compare/branch is emitted in `pack`, `unpack`
  or `visualize`), not an omission of this reconstruction; the generated header
  for this message group evidently carries no max-length attribute for `name_`
  while Target B's arrays do carry one.
* `pack(destBuf, 1)` inside the union arms: the literal 1 is what the ORIG's
  machine code stores into every arm call's `cutVer` slot, and the secsvr
  generator family spells the same literal in its union arms; whether the DNF
  client's generated file spelled it `1`, a macro or a per-message constant is
  not recoverable from the binary.
* Names of inline header helpers come from DWARF linkage names; their bodies are
  the model of `tsf4g_tdr_compat.h`, validated here by the size/identity
  statistics above rather than by byte-identity of every function.

## 7. Behavioural differential oracle (`test_qt_oracle.cpp`, `oracle_qt.sh`)

Requested follow-up verification: the reconstruction is not only size-matched
but behaviourally identical to the ORIG for every entry point of both CUs.

```
sh oracle_qt.sh                 # build + run the differential oracle
python3 oracle_qt_mutations.py  # negative control (mutate, expect failures)
```

**Method** (`recon/src/origsrc/test_qt_oracle.cpp`, mirrors
`test_uic_oracle.cpp`): both reconstructed CUs are compiled with `c5build` and
linked into `/tmp/libqt_recon.so` with
`-Wl,-Bsymbolic -Wl,--allow-multiple-definition` (the second flag folds the
weak `TdrWriteBuf::textize` copy each TU emits, exactly as the ORIG's linker
did).  The ORIG is `dlopen()`ed `RTLD_GLOBAL` first, so the reconstructed
object's undefined runtime symbols (`TdrBufUtil::printVariable/printArray`,
`TdrWriteBuf::vtextize`, ...) bind to the ORIG runtime; the reconstructed
object is `dlopen()`ed `RTLD_LOCAL` and entered via `dlsym` on the mangled
names.  `-Bsymbolic` is required: without it the reconstructed object's
*internal* calls (`pack(char*)` → `pack(TdrWriteBuf&)`, `Dp2QtMsg::pack` →
`body_.pack`, …) are preempted by the ORIG's global copies and those wrapped
paths become vacuous.

The reverse direction is hardened too, which matters because this ORIG is *not*
`-Bsymbolic` internally: its PLT calls to its own exported symbols are
interposable (VM engine's finding).  The oracle executable exports no
definitions — `nm -D --defined-only /tmp/qt_oracle` shows **0**
`sec_proto`/`tsf4g_tdr` symbols and `nm -D --undefined-only` **0** references
to them (every call goes through a `dlsym` pointer) — and the reconstructed
object is `RTLD_LOCAL`.  The ORIG's preemptible internal calls therefore find no
competing definition in the global scope and execute the ORIG's own code, while
the reconstructed object runs its own copies: both sides of every comparison
are genuinely different implementations, which the mutation control below
confirms empirically.

Per case both sides are called with the *same* input object and compared on
return value, produced used-size, and the full output buffer (memset before,
so an extra write also shows up); `unpack`/`construct` additionally compare the
full struct bytes.  A missing symbol fails the run.

All 115 CU symbols are looked up and exercised (106 Target A + 9 Target B), in
particular `construct`, both `pack`, both `unpack`, both `visualize` and both
`visualize_ex` overloads of every class, and the seven selector-form
`Dp2QtMsgBody` entries with selectors **0..11** (0 and 11 hit the default arm).
Inputs: random in-bounds lengths with NUL-terminated payloads
(`Login::name_` ≤ 256, uicta `cmd_buf_` ≤ 32768, `rolename_` ≤ 256), full-range
32-bit draws (so `"%u"`/`"%d"` operands are distinguishable), the uicta
`cmd_buf_len_ > 32768` and `rolename_len_ > 256` REFER_SURPASS modes, exact and
one-byte-short capacities (33044/33043, 32768), truncated inputs (`size/2`,
`size-1`), small write buffers (1/4/16), a visualize buffer too small for the
32 KiB dump, NULL buffers, `usedSize == NULL`, `indent == -1`, an alternate
separator, `visualize_ex` with `size == 0` and with a zero-capacity buffer.

**Pristine result**: `cases=49000 failures=0` (exit 0), coverage split

| group | cases | group | cases |
|---|---|---|---|
| Login | 2810 | ChangePosition | 2810 |
| EnterChannel | 2410 | RoleAction | 2810 |
| CreateRoom | 2810 | RoomMaster | 2810 |
| CreateRoomRes | 2810 | ChangeRoomType | 2410 |
| EnterRoom | 2810 | Dp2QtMsg | 2694 |
| ChangeRole | 2410 | Dp2QtMsgBody (12 selectors) | 18204 |
| | | ReqUICTransAdaptMsgForwardPkg | 1202 |

Because the `visualize*` comparisons run through the reconstructed
`TdrWriteBuf::textize`/`vtextize`/`writeCharWithNull` and, on the other side,
through the ORIG's, those several thousand byte-identical dumps are also a
differential test of the TdrBuf layer itself.

**Negative control** (`oracle_qt_mutations.py`): copies the two CUs, applies one
semantic mutation at a time, rebuilds `/tmp/libqt_recon.so`, re-runs the oracle
and requires (a) failures > 0 and (b) at least one `FAIL` line naming the
expected class:

| mutation | failures | first failing check |
|---|---|---|
| union `construct` selector 8 → arm 9 (`role_action`→`room_master`) | 200 | `Body construct-sel` |
| uicta `cmd_buf_len_` bound 32768 → 32767 | 240 | `UICTransAdapt pack` (ret 0/−7, first diff at 14) |
| `Login::visualize` literal `"[uin]"` → `"[uinX]"` | 1231 | `Login visualize` |
| `ChangePosition::pack` write order swapped | 1900 | `ChangePosition pack` (diff at 0) |
| `Login::unpack` read order swapped (`uin_`/`server_ip_`) | 1430 | `Login unpack` (diff at 0) |
| `RoleAction::visualize` `revert_` operand `"%u"` → `"%d"` | 672 | `RoleAction visualize` (diff at 45) |

`mutations_not_caught=0 of 6`, i.e. the oracle is not vacuous: it detects a
single-literal, single-field-order, single-bound and single-dispatch-arm change
in both CUs, including inside paths only reachable through the reconstructed
object's internal calls.

**Residual unsafe / uncovered paths** (honest limits of this oracle):

* comparisons are bounded by the two buffers (64 KiB for Target A, 256 KiB for
  Target B), so a hypothetical extra write beyond those caps would not be
  detected; both are larger than any message or visualization of these CUs
  (max packed size 33044, max dump ≈ 160 KiB for a full `cmd_buf_`).
* the OUT-OF-BOUNDS array paths of Target B are only reachable with
  `cmd_buf_len_ > 32768`/`rolename_len_ > 256`; the oracle feeds exactly those
  values and the observed behaviour is `-7` with the 14-byte prefix written and
  the array untouched on **both** sides, so no out-of-bounds access occurs on
  either side while the guard itself is compared.
* `Login::name_` cannot carry an out-of-bounds length (uint8 ≤ 255 < 256), and
  the uicta arrays are NUL-terminated where filled; the oracle therefore never
  relies on undefined input.
* no file-static/global state exists in either reconstructed TU, so cases cannot
  leak into each other; each case re-zeroes buffers and objects.
* allocation failures (`TdrWriteBuf` growing constructors) are not modelled -
  neither CU uses the growing form.
