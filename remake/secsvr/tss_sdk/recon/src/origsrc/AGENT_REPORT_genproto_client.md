# GenProto client — `formmog_proto_tss_sdk_client.cpp` reconstruction

CU (ORIG, tag 394):
`/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_proto_tss_sdk_client.cpp`
inside `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ORIG compiled by `GCC 4.1.0 (SUSE Linux)` — `readelf -p .comment`; the
available vendor toolchain is `c5root g++ 4.1.2 (Red Hat 4.1.2-55)`).

Artifacts (new, no commits):
* `recon/src/origsrc/formmog_proto_tss_sdk_client.cpp` — the TU
  (255 `sec_proto::` functions + the CU's `TdrWriteBuf::textize`).
* `recon/src/origsrc/formmog_proto_tss_sdk_client.h` — private protocol header:
  29 classes, `#pragma pack(1)`, anonymous unions, and compile-time
  `sizeof`/`offsetof` assertions against the ORIG DWARF for every member.
* `recon/src/origsrc/tsf4g_tdr_compat_client.h` — this CU's ABI model of
  `tsf4g_tdr`.  It is the same model as the pilot's `tsf4g_tdr_compat.h` plus
  the one extra `TdrBufUtil` overload this CU calls,
  `printVariable(TdrWriteBuf&, int, char, const char*, unsigned int arrIdx,
  bool isStruct)` (used for arrays of structs).  A member function cannot be
  added to the class defined in the shared header, so this CU gets its own
  sibling header, as allowed by the task.  Class layouts / method sets are
  identical apart from that overload; the real runtime header
  (`secagent/src/protocol/common/TdrBufUtil.h`) declares all four overloads.
* `recon/src/origsrc/genproto_client_matrix.tsv` — machine-readable
  `addr, size_orig, size_recon, ratio, name` (256 rows).
* `recon/src/origsrc/test_client_oracle.cpp` + `tools/tss_sdk_recon/oracle_client.sh`
  — differential oracle against the ORIG (28 800 cases, 0 failures; see §5).

## 1. Symbol table used (ORIG address / size)

`cu_symbols.py <ORIG> formmog_proto_tss_sdk_client.cpp --sizes` enumerates every
`DW_TAG_subprogram` with `DW_AT_low_pc` in the CU: 256 rows, all cross-checked
against `nm -D -C -S --defined-only` (address and size identical for every row).
255 rows are `sec_proto::` symbols; they are exactly the set matched by

```
grep -E 'sec_proto::(DPInterData|DPCmdList|DPServerCode|DPClientCode|DPBinCount|DPBinFile|DPConfigData|DPDynamicAlgorithmHead|DPAlgorithmData|DPFixAlgorithmHead|DPClientDecryptInfo|DPClientStatPtc|DpClientShortCmdData|DPClientCmdData|DPClientLogPtc|DPClientInitDataPtc|DPProtocolRequestCode|DPInterDataBody|DPInterDataHead|TClientFrame|TClientFrameHead|TClientFramePiece|TClientFrameBody|TClientPkg|TSafeMode|TSpecialControl|TReviveMouse|TClientPkgBody|ClientPkgHead)::'
```

The 256th is `tsf4g_tdr::TdrWriteBuf::textize(char const*, ...)`
(0x00136e90, 0x6c; weak in the ORIG, defined in this TU because the CU's DWARF
attributes it here).

Per class: 26 non-union classes × 9 members (`construct`, `pack`,
`pack(char*)`, `unpack`, `unpack(char*)`, `visualize`, `visualize(char*)`,
`visualize_ex(TdrWriteBuf&)`, `visualize_ex(char*)`), 3 union bodies × 7
(`construct(int64_t)`, `pack(int64_t, TdrWriteBuf&)`,
`pack(int64_t, char*)`, `unpack(int64_t, TdrReadBuf&)`,
`unpack(int64_t, char*)`, `visualize(int64_t, TdrWriteBuf&)`,
`visualize(int64_t, char*)`) = 234 + 21 = 255.

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | tsf4g_tdr::TdrWriteBuf::textize(char const*, ...) | 108 | 108 | 1.0000 |
| 00155d80 | sec_proto::DPInterData::construct() | 53 | 53 | 1.0000 |
| 00155dc0 | sec_proto::DPCmdList::construct() | 53 | 53 | 1.0000 |
| 00155e00 | sec_proto::DPServerCode::construct() | 44 | 44 | 1.0000 |
| 00155e30 | sec_proto::DPClientCode::construct() | 16 | 16 | 1.0000 |
| 00155e40 | sec_proto::DPBinCount::construct() | 15 | 15 | 1.0000 |
| 00155e50 | sec_proto::DPBinFile::construct() | 16 | 16 | 1.0000 |
| 00155e60 | sec_proto::DPConfigData::construct() | 19 | 19 | 1.0000 |
| 00155e80 | sec_proto::DPDynamicAlgorithmHead::construct() | 23 | 23 | 1.0000 |
| 00155ea0 | sec_proto::DPAlgorithmData::construct() | 53 | 53 | 1.0000 |
| 00155ee0 | sec_proto::DPFixAlgorithmHead::construct() | 30 | 30 | 1.0000 |
| 00155f00 | sec_proto::DPClientDecryptInfo::construct() | 23 | 23 | 1.0000 |
| 00155f20 | sec_proto::DPClientStatPtc::construct() | 53 | 53 | 1.0000 |
| 00155f60 | sec_proto::DpClientShortCmdData::construct() | 16 | 16 | 1.0000 |
| 00155f70 | sec_proto::DPClientCmdData::construct() | 23 | 23 | 1.0000 |
| 00155f90 | sec_proto::DPClientLogPtc::construct() | 53 | 53 | 1.0000 |
| 00155fd0 | sec_proto::DPClientInitDataPtc::construct() | 20 | 20 | 1.0000 |
| 00155ff0 | sec_proto::DPProtocolRequestCode::construct() | 30 | 30 | 1.0000 |
| 00156010 | sec_proto::DPInterDataBody::construct(long long) | 453 | 453 | 1.0000 |
| 001561e0 | sec_proto::DPInterDataHead::construct() | 20 | 20 | 1.0000 |
| 00156200 | sec_proto::TClientFrame::construct() | 53 | 53 | 1.0000 |
| 00156240 | sec_proto::TClientFrameHead::construct() | 23 | 23 | 1.0000 |
| 00156260 | sec_proto::TClientFramePiece::construct() | 53 | 53 | 1.0000 |
| 001562a0 | sec_proto::TClientFrameBody::construct(long long) | 35 | 35 | 1.0000 |
| 001562d0 | sec_proto::TClientPkg::construct() | 55 | 55 | 1.0000 |
| 00156310 | sec_proto::TSafeMode::construct() | 16 | 16 | 1.0000 |
| 00156320 | sec_proto::TSpecialControl::construct() | 34 | 34 | 1.0000 |
| 00156350 | sec_proto::TReviveMouse::construct() | 23 | 23 | 1.0000 |
| 00156370 | sec_proto::TClientPkgBody::construct(long long) | 124 | 124 | 1.0000 |
| 001563f0 | sec_proto::ClientPkgHead::construct() | 30 | 30 | 1.0000 |
| 00156410 | sec_proto::DPClientCode::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 79 | 79 | 1.0000 |
| 00156460 | sec_proto::DPBinCount::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 80 | 80 | 1.0000 |
| 001564b0 | sec_proto::DPBinFile::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 146 | 146 | 1.0000 |
| 00156550 | sec_proto::DPConfigData::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 199 | 199 | 1.0000 |
| 00156620 | sec_proto::DPDynamicAlgorithmHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 144 | 144 | 1.0000 |
| 001566b0 | sec_proto::DPFixAlgorithmHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 196 | 196 | 1.0000 |
| 00156780 | sec_proto::DPClientDecryptInfo::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 144 | 144 | 1.0000 |
| 00156810 | sec_proto::DpClientShortCmdData::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 146 | 146 | 1.0000 |
| 001568b0 | sec_proto::DPClientCmdData::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 144 | 144 | 1.0000 |
| 00156940 | sec_proto::DPClientLogPtc::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 448 | 444 | 0.9911 |
| 00156b00 | sec_proto::DPProtocolRequestCode::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 260 | 260 | 1.0000 |
| 00156c10 | sec_proto::DPInterDataHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 199 | 199 | 1.0000 |
| 00156ce0 | sec_proto::TClientFrameHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 204 | 204 | 1.0000 |
| 00156db0 | sec_proto::TSafeMode::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 79 | 79 | 1.0000 |
| 00156e00 | sec_proto::TSpecialControl::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 312 | 312 | 1.0000 |
| 00156f40 | sec_proto::TReviveMouse::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 144 | 144 | 1.0000 |
| 00156fd0 | sec_proto::TClientPkgBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const | 438 | 438 | 1.0000 |
| 00157190 | sec_proto::ClientPkgHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 313 | 313 | 1.0000 |
| 001572d0 | sec_proto::TClientPkg::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 247 | 248 | 1.0040 |
| 001573d0 | sec_proto::DPClientStatPtc::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 1078 | 1073 | 0.9954 |
| 00157810 | sec_proto::DPProtocolRequestCode::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 001578a0 | sec_proto::ClientPkgHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157930 | sec_proto::DPInterDataHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 001579c0 | sec_proto::TClientFrameHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157a50 | sec_proto::TClientPkg::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157ae0 | sec_proto::TSafeMode::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157b70 | sec_proto::TSpecialControl::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157c00 | sec_proto::TReviveMouse::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157c90 | sec_proto::DPClientCode::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157d20 | sec_proto::DPBinCount::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157db0 | sec_proto::DPBinFile::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157e40 | sec_proto::DPConfigData::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157ed0 | sec_proto::DPDynamicAlgorithmHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157f60 | sec_proto::DPFixAlgorithmHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00157ff0 | sec_proto::DPClientDecryptInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00158080 | sec_proto::DPClientStatPtc::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00158110 | sec_proto::DpClientShortCmdData::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 001581a0 | sec_proto::DPClientCmdData::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00158230 | sec_proto::DPClientLogPtc::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 001582c0 | sec_proto::DPConfigData::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 93 | 91 | 0.9785 |
| 00158320 | sec_proto::DPConfigData::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00158390 | sec_proto::DPConfigData::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 109 | 113 | 1.0367 |
| 00158400 | __i686.get_pc_thunk.bx | 78 | 72 | 0.9231 |
| 00158450 | sec_proto::DPBinFile::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 113 | 106 | 0.9381 |
| 001584d0 | sec_proto::DpClientShortCmdData::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 113 | 106 | 0.9381 |
| 00158550 | sec_proto::DPInterDataHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 139 | 135 | 0.9712 |
| 001585e0 | sec_proto::DPClientInitDataPtc::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 550 | 547 | 0.9945 |
| 00158810 | sec_proto::DPClientInitDataPtc::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 001588a0 | sec_proto::TClientFramePiece::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 520 | 513 | 0.9865 |
| 00158ab0 | sec_proto::TClientFrameBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const | 130 | 130 | 1.0000 |
| 00158b40 | sec_proto::TClientFrame::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 247 | 248 | 1.0040 |
| 00158c40 | sec_proto::TClientFrame::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00158cd0 | sec_proto::TClientFramePiece::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00158d60 | sec_proto::DPCmdList::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 610 | 595 | 0.9754 |
| 00158fd0 | sec_proto::DPCmdList::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00159060 | sec_proto::DPServerCode::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 245 | 242 | 0.9878 |
| 00159160 | sec_proto::DPInterDataBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const | 2431 | 2153 | 0.8856 |
| 00159ae0 | sec_proto::DPInterData::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 247 | 248 | 1.0040 |
| 00159be0 | sec_proto::DPInterData::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00159c70 | sec_proto::DPServerCode::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00159d00 | sec_proto::DPAlgorithmData::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const | 443 | 433 | 0.9774 |
| 00159ec0 | sec_proto::DPAlgorithmData::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const | 139 | 139 | 1.0000 |
| 00159f50 | sec_proto::DPBinCount::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 58 | 61 | 1.0517 |
| 00159f90 | sec_proto::DPBinCount::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015a000 | sec_proto::DPBinFile::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 99 | 98 | 0.9899 |
| 0015a070 | sec_proto::DPBinFile::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015a0e0 | sec_proto::DpClientShortCmdData::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 99 | 98 | 0.9899 |
| 0015a150 | sec_proto::DpClientShortCmdData::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015a1c0 | sec_proto::DPInterDataHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 125 | 128 | 1.0240 |
| 0015a240 | sec_proto::DPInterDataHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015a2b0 | sec_proto::TClientFrameHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 171 | 174 | 1.0175 |
| 0015a360 | sec_proto::TClientFrameHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015a3d0 | sec_proto::TClientFrameHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 196 | 190 | 0.9694 |
| 0015a4a0 | sec_proto::TSafeMode::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 97 | 95 | 0.9794 |
| 0015a510 | sec_proto::TSpecialControl::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 306 | 302 | 0.9869 |
| 0015a650 | sec_proto::TReviveMouse::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 163 | 168 | 1.0307 |
| 0015a700 | sec_proto::TClientPkgBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 171 | 171 | 1.0000 |
| 0015a7b0 | sec_proto::ClientPkgHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 277 | 274 | 0.9892 |
| 0015a8d0 | sec_proto::TClientPkg::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 190 | 183 | 0.9632 |
| 0015a990 | sec_proto::DPCmdList::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 456 | 408 | 0.8947 |
| 0015ab60 | sec_proto::DPClientCode::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 97 | 95 | 0.9794 |
| 0015abd0 | sec_proto::DPDynamicAlgorithmHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 163 | 168 | 1.0307 |
| 0015ac80 | sec_proto::DPAlgorithmData::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 270 | 246 | 0.9111 |
| 0015ad90 | sec_proto::DPFixAlgorithmHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 234 | 254 | 1.0855 |
| 0015ae80 | sec_proto::DPClientDecryptInfo::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 163 | 168 | 1.0307 |
| 0015af30 | sec_proto::DPClientCmdData::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 163 | 168 | 1.0307 |
| 0015afe0 | sec_proto::DPClientStatPtc::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 1040 | 970 | 0.9327 |
| 0015b3f0 | sec_proto::DPProtocolRequestCode::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 263 | 256 | 0.9734 |
| 0015b500 | sec_proto::DPClientLogPtc::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 247 | 226 | 0.9150 |
| 0015b600 | sec_proto::DPClientInitDataPtc::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 516 | 471 | 0.9128 |
| 0015b810 | sec_proto::TClientFramePiece::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 329 | 301 | 0.9149 |
| 0015b960 | sec_proto::TClientFrameBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 50 | 50 | 1.0000 |
| 0015b9a0 | sec_proto::TClientFrame::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 108 | 108 | 1.0000 |
| 0015ba10 | sec_proto::DPServerCode::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 174 | 169 | 0.9713 |
| 0015bac0 | sec_proto::DPInterDataBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 709 | 710 | 1.0014 |
| 0015bd90 | sec_proto::DPInterData::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const | 108 | 108 | 1.0000 |
| 0015be00 | sec_proto::ClientPkgHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015be90 | sec_proto::TReviveMouse::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015bf20 | sec_proto::ClientPkgHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015bff0 | sec_proto::ClientPkgHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015c0b0 | sec_proto::DPInterDataBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const | 198 | 198 | 1.0000 |
| 0015c180 | sec_proto::DPInterDataBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const | 147 | 147 | 1.0000 |
| 0015c220 | sec_proto::DPCmdList::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015c2f0 | sec_proto::DPCmdList::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015c3b0 | sec_proto::DPCmdList::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015c440 | sec_proto::DPServerCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015c510 | sec_proto::DPServerCode::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015c5d0 | sec_proto::DPServerCode::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015c660 | sec_proto::DPClientCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015c730 | sec_proto::DPInterData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015c800 | sec_proto::DPInterData::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015c8c0 | sec_proto::DPInterData::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015c950 | sec_proto::TClientPkgBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const | 147 | 147 | 1.0000 |
| 0015c9f0 | sec_proto::TSafeMode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015cac0 | sec_proto::TSafeMode::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015cb80 | sec_proto::TSafeMode::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015cc10 | sec_proto::TSpecialControl::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015cce0 | sec_proto::TSpecialControl::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015cda0 | sec_proto::TSpecialControl::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015ce30 | sec_proto::TReviveMouse::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015cf00 | sec_proto::TReviveMouse::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015cfc0 | sec_proto::DPProtocolRequestCode::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015d080 | sec_proto::DPProtocolRequestCode::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015d110 | sec_proto::DPInterDataHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015d1e0 | sec_proto::DPInterDataHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015d2a0 | sec_proto::DPInterDataHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015d330 | sec_proto::TClientFrame::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015d400 | sec_proto::TClientFrame::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015d4c0 | sec_proto::TClientFrame::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015d550 | sec_proto::TClientFrameBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const | 198 | 198 | 1.0000 |
| 0015d620 | sec_proto::TClientFrameBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const | 147 | 147 | 1.0000 |
| 0015d6c0 | sec_proto::TClientFrameHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015d790 | sec_proto::TClientFrameHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015d850 | sec_proto::TClientFrameHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015d8e0 | sec_proto::TClientFramePiece::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015d9b0 | sec_proto::TClientFramePiece::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015da70 | sec_proto::TClientFramePiece::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015db00 | sec_proto::TClientPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015dbd0 | sec_proto::TClientPkg::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015dc90 | sec_proto::TClientPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015dd20 | sec_proto::TClientPkgBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const | 198 | 198 | 1.0000 |
| 0015ddf0 | sec_proto::DPClientCode::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015deb0 | sec_proto::DPClientCode::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015df40 | sec_proto::DPBinCount::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015e010 | sec_proto::DPBinCount::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015e0d0 | sec_proto::DPBinCount::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015e160 | sec_proto::DPBinFile::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015e230 | sec_proto::DPBinFile::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015e2f0 | sec_proto::DPBinFile::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015e380 | sec_proto::DPConfigData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015e450 | sec_proto::DPConfigData::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015e510 | sec_proto::DPConfigData::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015e5a0 | sec_proto::DPDynamicAlgorithmHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015e670 | sec_proto::DPDynamicAlgorithmHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015e730 | sec_proto::DPDynamicAlgorithmHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015e7c0 | sec_proto::DPAlgorithmData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015e890 | sec_proto::DPAlgorithmData::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015e950 | sec_proto::DPAlgorithmData::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015e9e0 | sec_proto::DPFixAlgorithmHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015eab0 | sec_proto::DPFixAlgorithmHead::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015eb70 | sec_proto::DPFixAlgorithmHead::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015ec00 | sec_proto::DPClientDecryptInfo::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015ecd0 | sec_proto::DPClientDecryptInfo::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015ed90 | sec_proto::DPClientDecryptInfo::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015ee20 | sec_proto::DPClientStatPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015eef0 | sec_proto::DPClientStatPtc::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015efb0 | sec_proto::DPClientStatPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015f040 | sec_proto::DpClientShortCmdData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015f110 | sec_proto::DpClientShortCmdData::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015f1d0 | sec_proto::DpClientShortCmdData::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015f260 | sec_proto::DPClientCmdData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015f330 | sec_proto::DPClientCmdData::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015f3f0 | sec_proto::DPClientCmdData::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015f480 | sec_proto::DPClientLogPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015f550 | sec_proto::DPClientLogPtc::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015f610 | sec_proto::DPClientLogPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015f6a0 | sec_proto::DPClientInitDataPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015f770 | sec_proto::DPClientInitDataPtc::visualize(char*, unsigned int, unsigned int*, int, char) const | 182 | 182 | 1.0000 |
| 0015f830 | sec_proto::DPClientInitDataPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const | 133 | 133 | 1.0000 |
| 0015f8c0 | sec_proto::DPProtocolRequestCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const | 201 | 198 | 0.9851 |
| 0015f990 | sec_proto::DPServerCode::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 167 | 164 | 0.9820 |
| 0015fa40 | sec_proto::DPServerCode::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015fab0 | sec_proto::DPClientCode::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 80 | 77 | 0.9625 |
| 0015fb00 | sec_proto::DPClientCode::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015fb70 | sec_proto::DPDynamicAlgorithmHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 154 | 148 | 0.9610 |
| 0015fc10 | sec_proto::DPDynamicAlgorithmHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015fc80 | sec_proto::DPAlgorithmData::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 270 | 236 | 0.8741 |
| 0015fd90 | sec_proto::DPAlgorithmData::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015fe00 | sec_proto::DPFixAlgorithmHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 225 | 222 | 0.9867 |
| 0015fef0 | sec_proto::DPFixAlgorithmHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 0015ff60 | sec_proto::DPClientDecryptInfo::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 154 | 148 | 0.9610 |
| 00160000 | sec_proto::DPClientDecryptInfo::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160070 | sec_proto::DPClientCmdData::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 154 | 148 | 0.9610 |
| 00160110 | sec_proto::DPClientStatPtc::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 976 | 922 | 0.9447 |
| 001604e0 | sec_proto::DPClientStatPtc::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160550 | sec_proto::DPClientCmdData::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 001605c0 | sec_proto::TReviveMouse::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 154 | 148 | 0.9610 |
| 00160660 | sec_proto::TReviveMouse::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 001606d0 | sec_proto::ClientPkgHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 253 | 254 | 1.0040 |
| 001607d0 | sec_proto::ClientPkgHead::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160840 | sec_proto::DPCmdList::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 396 | 408 | 1.0303 |
| 001609d0 | sec_proto::DPCmdList::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160a40 | sec_proto::TClientFramePiece::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 327 | 285 | 0.8716 |
| 00160b90 | sec_proto::TClientFrameBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int) | 50 | 50 | 1.0000 |
| 00160bd0 | sec_proto::TClientFrame::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 108 | 108 | 1.0000 |
| 00160c40 | sec_proto::TClientFrame::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160cb0 | sec_proto::TClientFrameBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int) | 122 | 122 | 1.0000 |
| 00160d30 | sec_proto::TClientFramePiece::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160da0 | sec_proto::TSafeMode::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 80 | 77 | 0.9625 |
| 00160df0 | sec_proto::TSafeMode::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00160e60 | sec_proto::TSpecialControl::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 269 | 276 | 1.0260 |
| 00160f70 | sec_proto::TClientPkgBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int) | 171 | 171 | 1.0000 |
| 00161020 | sec_proto::TClientPkg::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 143 | 135 | 0.9441 |
| 001610b0 | sec_proto::TClientPkg::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00161120 | sec_proto::TClientPkgBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int) | 122 | 122 | 1.0000 |
| 001611a0 | sec_proto::TSpecialControl::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00161210 | sec_proto::DPClientLogPtc::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 236 | 213 | 0.9025 |
| 00161300 | sec_proto::DPClientLogPtc::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00161370 | sec_proto::DPClientInitDataPtc::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 461 | 431 | 0.9349 |
| 00161540 | sec_proto::DPClientInitDataPtc::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 001615b0 | sec_proto::DPProtocolRequestCode::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 236 | 238 | 1.0085 |
| 001616a0 | sec_proto::DPInterDataBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int) | 709 | 710 | 1.0014 |
| 00161970 | sec_proto::DPInterData::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int) | 108 | 108 | 1.0000 |
| 001619e0 | sec_proto::DPInterData::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |
| 00161a50 | sec_proto::DPInterDataBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int) | 122 | 122 | 1.0000 |
| 00161ad0 | sec_proto::DPProtocolRequestCode::unpack(char const*, unsigned int, unsigned int*, unsigned int) | 102 | 102 | 1.0000 |

## 2. Protocol model recovered from the ORIG

### 2.1 Layout (DWARF, `cu_layout.py`)

29 classes, all byte-packed (`#pragma pack(1)`, verified by the header
assertions): `DPClientCode` 4, `DPBinCount` 2, `DPBinFile` 4, `DPConfigData` 3,
`DPDynamicAlgorithmHead` 8, `DPFixAlgorithmHead` 12, `DPClientDecryptInfo` 8,
`DpClientShortCmdData` 4, `DPClientCmdData` 8, `DPClientLogPtc` 134,
`DPProtocolRequestCode` 12, `DPInterDataHead` 5, `TClientFrameHead` 8,
`TSafeMode` 4, `TSpecialControl` 13, `TReviveMouse` 8, `ClientPkgHead` 12,
`DPClientInitDataPtc` 36, `TClientFramePiece` 51210, `DPCmdList` 1028,
`DPServerCode` 20, `DPClientStatPtc` 1580, `DPAlgorithmData` 265,
`DPInterDataBody` 1580, `TClientFrameBody` 51210, `TClientPkgBody` 13,
`DPInterData` 1585, `TClientFrame` 51218, `TClientPkg` 25.
The packed offsets are visible in the ORIG code, e.g. `ClientPkgHead::seq_` at
+2 (`0x2(%esi)`), `TSpecialControl::cmdid` at +5 (`0x5(%edi)`),
`DPClientInitDataPtc::client_code_id_` at +24.

`DPInterData`/`TClientFrame`/`TClientPkg` are `{head; body;}` with member names
`head`/`body`; the three bodies are classes with a single **anonymous** union
(the DWARF union member has no `DW_AT_name`), which is what makes
`sizeof(TClientPkg)==25` (12 + 13) instead of 37.

### 2.2 No versioned fields

Every `pack`/`unpack` of the CU *ignores* its `cutVer` argument: the ORIG never
reads the `cutVer` stack slot in those bodies (checked per function: the only
`0x10(%ebp)`/`0x18(%ebp)` reads belong to `construct`'s 64-bit selector,
`visualize`'s `indent`, and the `char*` wrappers forwarding `cutVer`).  All
children are called with the literal `1`
(`movl $0x1,0x8(%esp)` / `0x10(%esp)` in the ORIG), so the reconstruction does
the same.  There is no `TDR_ERR_CUTVER_*` path anywhere in the CU.

### 2.3 Union selectors (recovered from `construct`/`pack`/`unpack` dispatch)

`DPInterDataBody::*` switches on `DPInterData::head.protocol_type_` (a
`long long` argument, zero-extended from the `uint16_t` member):

| selector | member | selector | member |
|---|---|---|---|
| 1 | `client_code_` (`DPClientCode`) | 11 | `client_stat_ptc_` (`DPClientStatPtc`) |
| 2 | `dynamic_algorithm_data_` | 12 | `decrypt_info_` |
| 3 | `bin_file_` | 13 | `client_log_ptc_` |
| 4 | `cmd_list` | 14 | `cs_test_data_[10]` |
| 5 | `bin_count_` | 15 | `sc_test_data_[10]` |
| 6 | `server_code_` | 17 | `fix_algorithm_data_` |
| 9 | `request_code_` | 20 | `config_data_` |
| 10 | `init_data_ptc_` | other | `TDR_NO_ERROR` |

`TClientPkgBody::*` switches on `TClientPkg::head.sub_sln_id_`
(`0x8(%edx)` in `TClientPkg::pack`):

| selector | member |
|---|---|
| `0x135` | `reviseMouse` (`TReviveMouse`) |
| `0x136` | `specialControl` (`TSpecialControl`) |
| `0x137` | `safeMode` (`TSafeMode`) |
| other | `TDR_NO_ERROR` |

`TClientFrameBody` has a single arm (`framePiece`, `TClientFramePiece`) and the
ORIG **ignores** the selector entirely: `construct`/`pack`/`unpack`/`visualize`
call the arm unconditionally (no compare in the body, e.g. `TClientFrameBody::pack`
at 0x15a6d0 is `mov 0x14(%ebp),%eax; movl $0x1,0x8(%esp); ... call framePiece_.pack`).
The selector passed by `TClientFrame` is `head.protoId` (`0x6(%edx)`).

The ORIG dispatch is a linear `if (unionSelector == N)` chain, not a switch:
each test is the 64-bit `xor $N,%eax; or %ecx,%eax; je` idiom (e.g.
`DPInterDataBody::pack` 0x15bac0).  The reconstruction emits the same chain
(the first version used `switch` and produced a jump table, 700 bytes smaller
in `DPInterDataBody::visualize`; switched to `if`/`else if` to match).

### 2.4 Pack/unpack element semantics

* Scalars: `writeUInt8/16/32` and `readUInt8/16/32`, declaration order.
* Fixed-size byte arrays (`code_ver_[16]`, `client_version_[16]`, and the
  union's `cs_test_data_[10]`/`sc_test_data_[10]` arms):
  `if (arr == 0) return -8;` then `capacity()-size() < sizeof(arr)` → -1 / -2,
  then one bulk `writeBytes`/`readBytes` (inlined `memmove`).  The ORIG emits
  exactly this order (`test this; je -8` then the capacity compare), proved at
  `DPServerCode::pack` 0x15ba35 and `DPInterDataBody::pack`'s array arms.
* Length-prefixed byte arrays (`error_msg_[128]`, `pad_data_[256]`,
  `pieceData[51200]`): `writeUInt8/16(len)` first (the length field is a
  normal member, emitted in order), then the bound check **only when the
  length type can exceed the capacity** — `error_msg_len_`/`pieceDataSize`
  (`> 128` / `> 51200` → -7), but *not* `pad_len_` (`uint8_t` max 255 < 256;
  the ORIG has no compare at all in `DPAlgorithmData::pack/unpack`), then
  `capacity()-size() < len` → -1 / -2, then one bulk transfer.
* `uint32_t up_cmd_list_[128]` / `down_cmd_list_[128]`: length `> 128` → -7,
  then a per-element `uint16_t i` loop of `writeUInt32`/`readUInt32`
  (no upfront capacity check — each element checks).
* Struct arrays `need_encrypt_list_[128]` / `client_error_list_[128]`:
  length `> 128` → -7, then a `uint16_t i` loop of
  `elem.pack(destBuf, 1)` / `elem.unpack(srcBuf, 1)`.
* Composite members: `head.pack(destBuf, 1)` then
  `body.pack(<selector>, destBuf, 1)`.
* `TClientPkg::pack` (0x15a8d0) **stamps** `head.body_len_` at the end with the
  *body* byte count: `startPos = getUsedSize()` before the head is written,
  `headEnd = getUsedSize()` after `head.pack`, then
  `writeUInt16((uint16_t)(getUsedSize() - headEnd), startPos + 10)` with the
  two-step `pos > limit` / `limit-pos <= 1` checks (both → -1).  The ORIG keeps
  `startPos` at `-0x14(%ebp)` and `headEnd` at `-0x10(%ebp)` and subtracts
  `-0x10` at 0x15a96f; the first reconstruction used `used - startPos` and the
  differential oracle caught it (stamp 20 vs 8 for an 8-byte body).  No other
  class of this CU stamps a length.
* `TClientPkg::unpack` (0x161020) validates the declared body length before
  the body: `if (head.body_len_ > srcBuf.capacity() - srcBuf.size())
  return -2;` then `body.unpack(head.sub_sln_id_, srcBuf, 1)`.

### 2.5 visualize / visualize_ex

* Scalars: `printVariable(destBuf, indent, separator, "[name]", fmt, value)`
  with `"%u"` for `uint32_t` and `"%d"` for `uint16_t`/`uint8_t`
  (the 8-bit and 16-bit fields are zero-extended into the varargs slot, as in
  the ORIG).
* Byte arrays: `printArray(..., "[name]", (long long)count)` then a
  `uint32_t i` loop of `destBuf.textize(" %d", arr[i])` and a final
  `writeCharWithNull(separator)`.  Fixed-size arrays use their constant length
  and the same append/terminator sequence (`code_ver_`, `client_version_`, and
  the union's 10-byte arms).
* `uint32_t` arrays: `printArray` + `textize(" %u", ...)` + `writeCharWithNull`.
* Struct arrays: **no** `printArray`, no trailing separator.  Per element:
  `printVariable(destBuf, indent, separator, "[list]", (unsigned int)i, true)`
  (the 6-argument overload) followed by the element's `visualize` with
  `indent+1` (two call sites, one for the `indent >= 0` branch and one for
  the negative-indent branch; 4.1.2 merges some pairs, see divergences).
* Nested struct/union members: `printVariable(..., "[head]", true)` then the
  child's `visualize`/`visualize` (union) with the same two-call indent
  pattern; union arm bodies are
  `printVariable(..., "[arm]", true)` + `return arm.visualize(destBuf,
  indent+1 or indent, separator)`.
* `visualize_ex(TdrWriteBuf&)` (139 bytes, identical to the pilot):
  `if (data()==NULL || capacity()==0) return "";` then `visualize`, then
  `writeChar('\0', used < total ? used : total-1)`, return `getBeginPtr()`.
* `visualize_ex(char*)` (198 bytes): `if (buffer==NULL || size==0) return "";`,
  run `visualize` on a stack `TdrWriteBuf`, NUL-terminate at
  `min(used, size-1)`, store `*usedSize`, return the buffer.
* `visualize(char*)` (182 bytes): `buffer == NULL` → -19, else run, store
  `*usedSize`, return the `visualize` result.
* The **union** bodies name their dispatcher `visualize` (the DWARF linkage
  names read `…9visualizeExRN…`, where `E` ends the name and `x` is the
  `long long` selector — nm confirms 252 `visualize`-family symbols, not
  `visualizeEx`).  Their `visualize(int64_t, char*, …)` is the
  `visualize(char*)` shape (buff=`NULL` → -19, **no** NUL termination and no
  `size == 0` guard; returns `TdrError::ErrorType`), unlike a class's
  `visualize_ex`, and there is no `visualize_ex` for unions at all.

### 2.6 Wrappers and EH

`pack(char*)`/`unpack(char*)` are the 133/102-byte shapes
(`buffer == NULL` → -19, stack `TdrWriteBuf`/`TdrReadBuf`, forward, store
`*usedSize`, return).  `construct()` is `memset(this, 0, sizeof(*this))`
(ORIG: `memset@plt` for the large classes, inline stores / `rep stos` for the
small ones — the same GCC decision reproduced) and the union `construct(
selector)` calls the selected arm's `construct()` / `memset`s the array arms.

`throw()` on `pack`/`unpack` (buf and `char*`, plain and union) is a **codegen
compensation, not an ORIG attribute**: GCC 4.1.2 emits an EH cleanup region
for the wrappers that hold a buffer object, GCC 4.1.0 did not (the wrappers
only call the nothrow primitives / nothrow members).  Proof from the ORIG:
`_Unwind_Resume` is referenced exactly **55** times in the CU range — 26
`visualize(char*)` + 26 `visualize_ex(char*)` + 3 union `visualize(char*)` —
and never in a `pack`/`unpack(char*)` wrapper.  The reconstruction also emits
exactly 55 `_Unwind_Resume` relocations (`readelf -r | grep -c _Unwind_Resume`),
i.e. the compensation reproduces the ORIG EH layout exactly.  An empty
exception specification is not mangled, so all names are unchanged.

## 3. Commands run

```
# enumeration / layout
python3 cu_symbols.py /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_client.cpp --sizes
python3 cu_layout.py   /opt/.../libtss_sdk.so.1.0.394 formmog_proto_tss_sdk_client.cpp > /tmp/client_layout.txt
nm -D -S --defined-only /opt/.../libtss_sdk.so.1.0.394 | grep -E 'DPInterDataBody|TClientPkgBody|TClientFrameBody'
nm -D -C --defined-only /opt/.../libtss_sdk.so.1.0.394 | grep TdrBufUtil          # runtime overloads
readelf -p .comment /opt/.../libtss_sdk.so.1.0.394                                  # GCC 4.1.0 (SUSE)
grep -rn 'TDR_ERR_REFER_SURPASS_COUNT' .../secagent/output/protocol/...             # generator reference

# disassembly (whole CU range 0x155d80..0x161b40)
objdump -d -C --start-address=0x155d80 --stop-address=0x161b40 /opt/.../libtss_sdk.so.1.0.394 > /tmp/cl_dis.txt
python3 dis_annotate.py /opt/.../libtss_sdk.so.1.0.394 /tmp/cl_dis.txt > /tmp/cl_ann.txt
# per-function cmp-constant / literal / call-target summaries over /tmp/cl_ann.txt
# (scratch scanners kept outside the repo), plus objdump of /tmp/cl.o per function

# build / verification
sh c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_client.cpp /tmp/cl.o -I recon/src/origsrc   # rc=0
sh symdiff.sh /opt/.../libtss_sdk.so.1.0.394 /tmp/cl.o '<29-class regex above>'
sh sizecmp.sh /tmp/cl.o '<29-class regex above>'
sh strfind.sh < <literals extracted from the source>        # 100 PRESENT / 0 MISSING
objdump -d /tmp/cl.o ; readelf -r /tmp/cl.o | grep -c _Unwind_Resume                # 55
objdump -d --start-address=0x155d80 --stop-address=0x161b40 /opt/.../libtss_sdk.so.1.0.394 | grep -cP '\trol\s'  # 0
objdump -d /tmp/cl.o | grep -cP '\trol\s'                                           # 53
```
`symdiff.sh`/`sizecmp.sh` write fixed `/tmp` paths; they were run through
byte-identical copies with the temp prefix renamed (`/tmp/gpc_*`); only the
temp-file names differ.  The source and matrix were produced by a scratch
generator (`/tmp`), not committed.

## 4. Results

* `symdiff.sh`: `== ORIG-only ==` empty, `== NEW-only ==` empty; counts
  **255 / 255** plus the shared `textize` symbol (108 = 108 bytes).
* `sizecmp.sh` (255 `sec_proto::` symbols): `matched=255 identical=168
  differing=87`; min ratio **0.872**, max **1.085**; total 45792 recon bytes
  vs 46641 ORIG bytes (the 256th row, `textize` 108→108, is 1.000).
* Literals: all 100 string literals emitted by the reconstruction are PRESENT
  verbatim in the ORIG; 0 MISSING.  The set is `""`, `"%d"`, `"%u"`, `" %d"`,
  `" %u"` and the 29 `[member]` names.
* Undefined externals of the object: `TdrBufUtil::printVariable` (3 overloads),
  `printArray`, `memmove`, `memset`, `vsnprintf`, `operator delete[]`,
  `__gxx_personality_v0`, `_Unwind_Resume` — all present in the ORIG
  (checked with `nm -D -u`).  No invented helper stays undefined.

### Divergences (all measured, all in the *smaller* direction except the tail)

| symbol | orig | recon | ratio | cause |
|---|---|---|---|---|
| `TClientFramePiece::unpack(TdrReadBuf&, unsigned int)` | 327 | 285 | 0.872 | 4× 16-bit big-endian read swaps + bulk read; ORIG expands the swap as `shr/shl/or`, 4.1.2 emits `rol $0x8,%ax` (whole CU: ORIG 0 `rol`, recon 53), and 4.1.2 spills the `memcpy` staging value of the `uint32_t` swap |
| `DPAlgorithmData::unpack(TdrReadBuf&, unsigned int)` | 270 | 236 | 0.874 | two 32-bit reads: same staging spill; no bound compare exists here in ORIG (u8 length, 256-byte array) and none is emitted |
| `DPInterDataBody::visualize(long long, TdrWriteBuf&, int, char) const` | 2431 | 2153 | 0.886 | 15-arm if-chain; the ORIG keeps **two** child-`visualize` call sites per struct arm (indent≥0 / indent<0), 4.1.2 CSEs 8 of those pairs (object: 36 call sites, ORIG: 44) |
| `DPCmdList::pack(TdrWriteBuf&, unsigned int) const` | 456 | 408 | 0.895 | 2 `writeUInt16` rotas + 2 element loops; only the rotate/spill difference (per-element instruction sequences verified identical) |
| 5 more symbols | — | — | 0.902–0.916 | same two 4.1.2 artifacts (`rol`, memcpy staging) |
| `DPFixAlgorithmHead::pack(TdrWriteBuf&, unsigned int) const` | 234 | 254 | 1.085 | 3 × 32-bit writes; 4.1.2 materialises the swapped value through a stack temporary before the 32-bit store |
| `DPBinCount::unpack`, `DPConfigData::pack`, 3 × 2-u32 `pack`s | — | — | 1.031–1.052 | same |

No symbol has a *behavioural* difference: every divergence is instruction
selection for the same statement sequence, verified by reading the ORIG body
next to the recon body (the `cmp` constants, branch targets, return codes,
call order and argument setup match).

### Deliberate structural decisions (not divergences)

* `if`/`else if` selector chains instead of `switch` — reproduces the ORIG's
  64-bit compare chain (a `switch` produced a jump table).
* Two-call `indent >= 0 ? indent+1 : indent` bodies — reproduces the ORIG's
  two call sites where the compiler kept them.
* `memset`-based `construct()` — matches the ORIG's inline-store /
  `memset@plt` decision for every class size.
* `throw()` on `pack`/`unpack` — the EH compensation described in §2.6.
* `TdrWriteBuf::textize` is a strong `T` here vs weak `W` in the ORIG
  (the ORIG's runtime header defines it implicitly inline).  Name, signature
  and size (0x6c) are identical.

## 5. Differential oracle (behavioural)

Files: `recon/src/origsrc/test_client_oracle.cpp` +
`tools/tss_sdk_recon/oracle_client.sh` (mirrors the UIC oracle).

Method: both TUs are built with `c5build.sh`; the CU object is linked into
`/tmp/libclient_recon.so` with `-shared -Wl,--allow-multiple-definition
-Wl,-Bsymbolic`.  The test binary `dlopen`s the ORIG
`RTLD_NOW|RTLD_GLOBAL` (so the recon object's `TdrBufUtil`/`memmove`/... 
references bind to the ORIG runtime) and the recon `.so` `RTLD_NOW|RTLD_LOCAL`,
then resolves the
`pack(char*, unsigned, unsigned*, unsigned)`,
`unpack(char const*, unsigned, unsigned*, unsigned)` and
`visualize(char*, unsigned, unsigned*, int, char)` entry points by mangled name
(the name is built as `_ZNK`/`_ZN` + `9sec_proto` + `<len><Class>` + member
suffix, so every resolution is checked; a missing symbol is a failure).
Per case it compares **ret, usedSize and the full output buffer** (pack,
visualize) and **ret, usedSize and all `sizeof(T)` struct bytes** (unpack), on
identical inputs.

`-Bsymbolic` is required for the oracle to be meaningful: the ORIG is loaded
RTLD_GLOBAL, so without it the recon `.so`'s *internal* calls
(e.g. `ClientPkgHead::pack -> TdrWriteBuf&`, `body_len_` stamp) are preempted
by the ORIG's copies and the comparison degenerates to ORIG-vs-ORIG.
Negative control (scratch copy of the CU outside the repo, one `pack` body
mutated to XOR `0x5a5a5a5a` into `code_id_`): **0 failures without
`-Bsymbolic`, 368 failures with it** — the shipped oracle detects a
one-field behavioural change.

Coverage: 29 classes × 120 iterations × 3 operations (26 plain classes + the
3 containers through their `char*` forms), the 3 union bodies with explicit
selectors (15 valid `DPInterData` selectors {1,2,3,4,5,6,9,10,11,12,13,14,15,
17,20} plus 8 out-of-range ones {0,7,8,16,18,19,21,0xffff}; 3 valid
`TClientPkg` selectors {0x135,0x136,0x137} plus 5 invalid; 4 `TClientFrame`
selectors including ones the single-arm body must ignore), the `DPInterData`
container cycling through all 15 selectors, and `TClientPkg::head.body_len_`
differential cases (stamp value check plus unpack of deliberately corrupted
`body_len_` = 0xffff / 0 / exact).  Every plain-class iteration additionally
unpacks two truncated prefixes (`used/2`, `used-1`) to differential the
short-read error paths.

Randomization is bounded so neither side reads out of bounds: every length
field is drawn inside its array (`error_msg_len_`,`need_encrypt_count_`,
`client_error_count_`,`up/down_cmd_count_` ≤ 128, `pieceDataSize` ≤ 51200,
`pad_len_` ≤ 255 of 256); byte payloads are NUL-terminated; unions only
populate the selected arm.  Visualize buffers are 256 KiB (`kCap`), above the
worst case (~205 KB for a 51200-byte `pieceData` at 4 chars/byte).

Observed result:

```
loaded; sizes: DPInterData=1585 TClientFrame=51218 TClientPkg=25
               DPInterDataBody=1580 TClientFrameBody=51210 TClientPkgBody=13
cases=28800 failures=0
```

The oracle found one real divergence, now fixed: `TClientPkg::pack` had been
stamping `used - startPos` (total length) instead of the body length
`used - headEnd` (ORIG subtracts the post-head position at 0x15a96f).  After
the fix `TClientPkg::pack` measures 183 vs 190 bytes (0.963) and the oracle is
clean.  No production file other than this CU's own `.cpp` was touched.

Residual unsafe paths in the oracle: it never calls the `TdrWriteBuf&`
entry points directly (they are exercised transitively through the `char*`
forms, with `-Bsymbolic` binding the internal calls to the recon bodies); the
`NULL`-buffer guards of `pack`/`unpack`/`visualize` are not randomized (they
take no buffer and return before any read); buffer-overflow paths for
`visualize` are not forced (would require a cap below the payload and would
only re-test the already-covered short-buffer checks).

## 6. Residual uncertainty

* ORIG `.cpp` statement layout is not recoverable; the source is written in the
  generator's canonical order (fields in declaration order, checks before the
  bulk transfer), which the sizes above confirm.
* `bounds` checks that the compiler can prove redundant are absent in the ORIG
  and are absent here by the same rule (`u8` length vs 256-byte array).  If a
  future CU shows a `> 256` compare for a `uint8_t` length, that rule would
  need revisiting.
* The `TdrBufUtil::*` bodies are external PLT calls in both ORIG and recon, so
  their inline behaviour is modelled only through call sites.
* `DPConfigData`/`TSafeMode` etc. field *meaning* is unknown (protocol data);
  only layout and codegen were reconstructed.
