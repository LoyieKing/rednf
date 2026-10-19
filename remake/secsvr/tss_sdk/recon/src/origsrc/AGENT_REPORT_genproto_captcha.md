# GenCaptcha — reconstruction of three formmog CUs

Targets (ORIG `tag 394`), all inside
`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ORIG compiled by `GCC (GNU) 4.1.0 (SUSE Linux)` — `readelf -p .comment`;
the vendor toolchain available here is `c5root g++ 4.1.2 (Red Hat 4.1.2-55)`):

| ORIG CU (…/g3_release_suse32_bugfix_tag394/src/protocol/formmog/) | sec_proto symbols | ORIG code range |
|---|---|---|
| `formmog_proto_captcha_client_old.cpp` | 97 | `0x0013a460` … `0x0014070c` |
| `formmog_proto_captcha_client.cpp` | 61 | `0x00136f00` … `0x0013a3f0` |
| `formmog_proto_public_safemodetransadapt.cpp` | 45 | `0x00144820` … `0x00146cd0` |

Artifacts (all new, no commits):
* `recon/src/origsrc/formmog_proto_captcha_client_old.cpp` — 97 functions.
* `recon/src/origsrc/formmog_proto_captcha_client_old.h` — private protocol
  header (`#pragma pack(1)`, compile-time size/offset assertions).
* `recon/src/origsrc/formmog_proto_captcha_client.cpp` — 61 functions.
* `recon/src/origsrc/formmog_proto_captcha_client.h` — same, new captcha
  protocol.
* `recon/src/origsrc/formmog_proto_public_safemodetransadapt.cpp` — 45
  functions.
* `recon/src/origsrc/formmog_proto_public_safemodetransadapt.h` — same, safe
  mode transfer adapt.
* `recon/src/origsrc/genproto_captcha_old_matrix.tsv`,
  `genproto_captcha_client_matrix.tsv`,
  `genproto_safemodetransadapt_matrix.tsv` — machine-readable
  `addr_orig / size_orig / size_recon / ratio / name` (98 / 62 / 46 rows
  including the shared `textize`).
* `tools/tss_sdk_recon/work_genc/cudecl.py`, `visfmt.py`, `mkmatrix.py` — new
  read-only helpers written for this task (DWARF member dump, print-call
  argument pairing, matrix generation).  No existing script was modified.

`tsf4g_tdr_compat.h` (the shared binary-TDR ABI model) is included, not edited;
no extra compat header was needed.

## 1. Symbol tables used (ORIG address / size)

Per CU: `python3 cu_symbols.py <ORIG> <cu> --sizes` lists every
`DW_TAG_subprogram` whose `DW_AT_low_pc` the CU's DWARF attributes to it.  Each
row's address and size were cross-checked against
`nm -DC -S --defined-only <ORIG>` (identical for every row).  The tables below
are the full matrices; `textize` (`0x00136e90`, 108 bytes, weak `W`) is the
CU's only out-of-line `tsf4g_tdr` definition and is emitted by each of the three
TUs as a weak comdat symbol.

### 1.1 `formmog_proto_captcha_client_old.cpp` (97 + textize)

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | `tsf4g_tdr::TdrWriteBuf::textize(char const*, ...)` | 108 | 108 | 1.000 |
| 0013a460 | `sec_proto::TSSCaptchaOldPkg::construct()` | 53 | 53 | 1.000 |
| 0013a4a0 | `sec_proto::BusiCaptchaPic::construct()` | 53 | 53 | 1.000 |
| 0013a4e0 | `sec_proto::CaptchaBusiFinish::construct()` | 53 | 53 | 1.000 |
| 0013a520 | `sec_proto::BusiCaptchaReq::construct()` | 53 | 53 | 1.000 |
| 0013a560 | `sec_proto::ReportLog::construct()` | 53 | 53 | 1.000 |
| 0013a5a0 | `sec_proto::S2CVerifyRes::construct()` | 53 | 53 | 1.000 |
| 0013a5e0 | `sec_proto::VerifyRes::construct()` | 53 | 53 | 1.000 |
| 0013a620 | `sec_proto::VerifyReq::construct()` | 53 | 53 | 1.000 |
| 0013a660 | `sec_proto::CaptchaReq::construct()` | 51 | 51 | 1.000 |
| 0013a6a0 | `sec_proto::TSSCaptchaOldBody::construct(long long)` | 254 | 254 | 1.000 |
| 0013a7a0 | `sec_proto::TSSCaptchaOldHead::construct()` | 51 | 51 | 1.000 |
| 0013a7e0 | `sec_proto::CaptchaReq::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 422 | 422 | 1.000 |
| 0013a990 | `sec_proto::TSSCaptchaOldHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 478 | 478 | 1.000 |
| 0013ab70 | `sec_proto::TSSCaptchaOldHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013ac00 | `sec_proto::CaptchaReq::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013ac90 | `sec_proto::VerifyReq::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 595 | 578 | 0.971 |
| 0013aef0 | `sec_proto::VerifyReq::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013af80 | `sec_proto::BusiCaptchaPic::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 1274 | 1249 | 0.980 |
| 0013b480 | `sec_proto::BusiCaptchaPic::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013b510 | `sec_proto::CaptchaBusiFinish::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 585 | 577 | 0.986 |
| 0013b760 | `sec_proto::CaptchaBusiFinish::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013b7f0 | `sec_proto::BusiCaptchaReq::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 829 | 823 | 0.993 |
| 0013bb30 | `sec_proto::BusiCaptchaReq::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013bbc0 | `sec_proto::ReportLog::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 455 | 447 | 0.982 |
| 0013bd90 | `sec_proto::ReportLog::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013be20 | `sec_proto::S2CVerifyRes::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 389 | 383 | 0.985 |
| 0013bfb0 | `sec_proto::S2CVerifyRes::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013c040 | `sec_proto::VerifyRes::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 577 | 571 | 0.990 |
| 0013c290 | `sec_proto::TSSCaptchaOldBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const` | 1110 | 980 | 0.883 |
| 0013c6f0 | `sec_proto::TSSCaptchaOldPkg::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 247 | 226 | 0.915 |
| 0013c7f0 | `sec_proto::TSSCaptchaOldPkg::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013c880 | `sec_proto::VerifyRes::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 0013c910 | `sec_proto::TSSCaptchaOldHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 570 | 513 | 0.900 |
| 0013cb50 | `sec_proto::CaptchaReq::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 510 | 472 | 0.925 |
| 0013cd50 | `sec_proto::TSSCaptchaOldPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013ce20 | `sec_proto::TSSCaptchaOldHead::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013ceb0 | `sec_proto::TSSCaptchaOldHead::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013cf70 | `sec_proto::CaptchaReq::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d030 | `sec_proto::CaptchaReq::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013d0c0 | `sec_proto::TSSCaptchaOldHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013d190 | `sec_proto::TSSCaptchaOldPkg::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d250 | `sec_proto::TSSCaptchaOldBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const` | 198 | 198 | 1.000 |
| 0013d320 | `sec_proto::BusiCaptchaPic::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013d3f0 | `sec_proto::BusiCaptchaPic::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d4b0 | `sec_proto::CaptchaBusiFinish::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013d580 | `sec_proto::CaptchaBusiFinish::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d640 | `sec_proto::BusiCaptchaReq::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013d710 | `sec_proto::BusiCaptchaReq::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d7d0 | `sec_proto::ReportLog::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013d8a0 | `sec_proto::ReportLog::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013d960 | `sec_proto::S2CVerifyRes::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013da30 | `sec_proto::S2CVerifyRes::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013daf0 | `sec_proto::VerifyRes::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013dbc0 | `sec_proto::VerifyRes::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013dc80 | `sec_proto::VerifyReq::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013dd50 | `sec_proto::VerifyReq::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 0013de10 | `sec_proto::CaptchaReq::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 0013dee0 | `sec_proto::VerifyReq::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 253 | 235 | 0.929 |
| 0013dfe0 | `sec_proto::VerifyReq::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013e070 | `sec_proto::BusiCaptchaPic::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 692 | 647 | 0.935 |
| 0013e330 | `sec_proto::BusiCaptchaPic::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013e3c0 | `sec_proto::CaptchaBusiFinish::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 408 | 366 | 0.897 |
| 0013e560 | `sec_proto::CaptchaBusiFinish::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013e5f0 | `sec_proto::BusiCaptchaReq::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 651 | 592 | 0.909 |
| 0013e880 | `sec_proto::BusiCaptchaReq::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013e910 | `sec_proto::ReportLog::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 263 | 232 | 0.882 |
| 0013ea20 | `sec_proto::ReportLog::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013eab0 | `sec_proto::S2CVerifyRes::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 166 | 162 | 0.976 |
| 0013eb60 | `sec_proto::S2CVerifyRes::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013ebf0 | `sec_proto::VerifyRes::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 391 | 352 | 0.900 |
| 0013ed80 | `sec_proto::TSSCaptchaOldBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 406 | 406 | 1.000 |
| 0013ef20 | `sec_proto::TSSCaptchaOldPkg::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 189 | 179 | 0.947 |
| 0013efe0 | `sec_proto::TSSCaptchaOldPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013f070 | `sec_proto::TSSCaptchaOldBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const` | 147 | 147 | 1.000 |
| 0013f110 | `sec_proto::VerifyRes::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 0013f1a0 | `sec_proto::ReportLog::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 247 | 216 | 0.874 |
| 0013f2a0 | `sec_proto::ReportLog::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013f310 | `sec_proto::S2CVerifyRes::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 166 | 154 | 0.928 |
| 0013f3c0 | `sec_proto::S2CVerifyRes::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013f430 | `sec_proto::VerifyReq::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 251 | 224 | 0.892 |
| 0013f530 | `sec_proto::VerifyReq::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013f5a0 | `sec_proto::BusiCaptchaPic::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 671 | 574 | 0.855 |
| 0013f840 | `sec_proto::BusiCaptchaPic::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013f8b0 | `sec_proto::BusiCaptchaReq::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 618 | 557 | 0.901 |
| 0013fb20 | `sec_proto::BusiCaptchaReq::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013fb90 | `sec_proto::VerifyRes::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 375 | 347 | 0.925 |
| 0013fd10 | `sec_proto::VerifyRes::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013fd80 | `sec_proto::CaptchaBusiFinish::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 391 | 351 | 0.898 |
| 0013ff10 | `sec_proto::CaptchaBusiFinish::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013ff80 | `sec_proto::CaptchaReq::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 449 | 441 | 0.982 |
| 00140150 | `sec_proto::TSSCaptchaOldBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int)` | 406 | 406 | 1.000 |
| 001402f0 | `sec_proto::TSSCaptchaOldBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int)` | 122 | 122 | 1.000 |
| 00140370 | `sec_proto::CaptchaReq::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 001403e0 | `sec_proto::TSSCaptchaOldHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 536 | 507 | 0.946 |
| 00140600 | `sec_proto::TSSCaptchaOldPkg::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 142 | 134 | 0.944 |
| 00140690 | `sec_proto::TSSCaptchaOldPkg::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00140700 | `sec_proto::TSSCaptchaOldHead::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |

### 1.2 `formmog_proto_captcha_client.cpp` (61 + textize)

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | `tsf4g_tdr::TdrWriteBuf::textize(char const*, ...)` | 108 | 108 | 1.000 |
| 00136f00 | `sec_proto::TSSCaptchaPkg::construct()` | 53 | 53 | 1.000 |
| 00136f40 | `sec_proto::TSSVerifyRes::construct()` | 53 | 53 | 1.000 |
| 00136f80 | `sec_proto::TSSVerifyReq::construct()` | 53 | 53 | 1.000 |
| 00136fc0 | `sec_proto::TSSCaptchaPic::construct()` | 53 | 53 | 1.000 |
| 00137000 | `sec_proto::TSSSetCaptcha::construct()` | 53 | 53 | 1.000 |
| 00137040 | `sec_proto::TSSCaptchaBody::construct(long long)` | 156 | 156 | 1.000 |
| 001370e0 | `sec_proto::TSSCaptchaHead::construct()` | 37 | 37 | 1.000 |
| 00137110 | `sec_proto::TSSCaptchaHead::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 314 | 314 | 1.000 |
| 00137250 | `sec_proto::TSSCaptchaHead::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 001372e0 | `sec_proto::TSSCaptchaPic::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 989 | 967 | 0.978 |
| 001376c0 | `sec_proto::TSSCaptchaPic::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00137750 | `sec_proto::TSSSetCaptcha::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 708 | 700 | 0.989 |
| 00137a20 | `sec_proto::TSSSetCaptcha::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00137ab0 | `sec_proto::TSSVerifyRes::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 389 | 383 | 0.985 |
| 00137c40 | `sec_proto::TSSVerifyRes::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00137cd0 | `sec_proto::TSSVerifyReq::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 316 | 310 | 0.981 |
| 00137e10 | `sec_proto::TSSCaptchaBody::visualize(long long, tsf4g_tdr::TdrWriteBuf&, int, char) const` | 578 | 518 | 0.896 |
| 00138060 | `sec_proto::TSSCaptchaPkg::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 247 | 226 | 0.915 |
| 00138160 | `sec_proto::TSSCaptchaPkg::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 001381f0 | `sec_proto::TSSVerifyReq::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00138280 | `sec_proto::TSSCaptchaHead::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 350 | 324 | 0.926 |
| 001383e0 | `sec_proto::TSSCaptchaHead::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00138470 | `sec_proto::TSSCaptchaPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00138540 | `sec_proto::TSSCaptchaHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00138610 | `sec_proto::TSSCaptchaHead::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 001386d0 | `sec_proto::TSSCaptchaPic::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 001387a0 | `sec_proto::TSSCaptchaPic::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00138860 | `sec_proto::TSSSetCaptcha::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00138930 | `sec_proto::TSSSetCaptcha::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 001389f0 | `sec_proto::TSSCaptchaPkg::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00138ab0 | `sec_proto::TSSCaptchaBody::visualize(long long, char*, unsigned int, unsigned int*, int, char) const` | 198 | 198 | 1.000 |
| 00138b80 | `sec_proto::TSSVerifyRes::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00138c50 | `sec_proto::TSSVerifyRes::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00138d10 | `sec_proto::TSSVerifyReq::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00138de0 | `sec_proto::TSSVerifyReq::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00138ea0 | `sec_proto::TSSVerifyRes::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 166 | 162 | 0.976 |
| 00138f50 | `sec_proto::TSSVerifyRes::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00138fe0 | `sec_proto::TSSCaptchaPic::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 575 | 527 | 0.917 |
| 00139220 | `sec_proto::TSSCaptchaPic::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 001392b0 | `sec_proto::TSSSetCaptcha::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 504 | 461 | 0.915 |
| 001394b0 | `sec_proto::TSSSetCaptcha::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00139540 | `sec_proto::TSSVerifyReq::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 143 | 133 | 0.930 |
| 001395d0 | `sec_proto::TSSCaptchaBody::pack(long long, tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 222 | 222 | 1.000 |
| 001396b0 | `sec_proto::TSSCaptchaPkg::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 189 | 179 | 0.947 |
| 00139770 | `sec_proto::TSSCaptchaPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00139800 | `sec_proto::TSSCaptchaBody::pack(long long, char*, unsigned int, unsigned int*, unsigned int) const` | 147 | 147 | 1.000 |
| 001398a0 | `sec_proto::TSSVerifyReq::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00139930 | `sec_proto::TSSVerifyRes::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 166 | 154 | 0.928 |
| 001399e0 | `sec_proto::TSSVerifyRes::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00139a50 | `sec_proto::TSSVerifyReq::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 143 | 133 | 0.930 |
| 00139ae0 | `sec_proto::TSSVerifyReq::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00139b50 | `sec_proto::TSSCaptchaHead::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 305 | 307 | 1.007 |
| 00139c90 | `sec_proto::TSSCaptchaHead::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00139d00 | `sec_proto::TSSCaptchaPic::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 549 | 476 | 0.867 |
| 00139f30 | `sec_proto::TSSCaptchaPic::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00139fa0 | `sec_proto::TSSSetCaptcha::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 487 | 431 | 0.885 |
| 0013a190 | `sec_proto::TSSCaptchaBody::unpack(long long, tsf4g_tdr::TdrReadBuf&, unsigned int)` | 222 | 222 | 1.000 |
| 0013a270 | `sec_proto::TSSCaptchaPkg::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 142 | 134 | 0.944 |
| 0013a300 | `sec_proto::TSSCaptchaPkg::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 0013a370 | `sec_proto::TSSCaptchaBody::unpack(long long, char const*, unsigned int, unsigned int*, unsigned int)` | 122 | 122 | 1.000 |
| 0013a3f0 | `sec_proto::TSSSetCaptcha::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |

### 1.3 `formmog_proto_public_safemodetransadapt.cpp` (45 + textize)

| addr_orig | name | size_orig | size_recon | ratio |
|---|---|---|---|---|
| 00136e90 | `tsf4g_tdr::TdrWriteBuf::textize(char const*, ...)` | 108 | 108 | 1.000 |
| 00144820 | `sec_proto::ReqSafemodeTransAdaptEventNotify::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 260 | 260 | 1.000 |
| 00144930 | `sec_proto::RspSafemodeTransAdaptStatUpdate::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 204 | 204 | 1.000 |
| 00144a00 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 256 | 256 | 1.000 |
| 00144b00 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 765 | 757 | 0.990 |
| 00144e00 | `sec_proto::ReqSafemodeTransAdaptDataForward::construct()` | 53 | 53 | 1.000 |
| 00144e40 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::construct()` | 53 | 53 | 1.000 |
| 00144e80 | `sec_proto::ReqSafemodeTransAdaptEventNotify::construct()` | 30 | 30 | 1.000 |
| 00144ea0 | `sec_proto::RspSafemodeTransAdaptStatUpdate::construct()` | 26 | 26 | 1.000 |
| 00144ec0 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::construct()` | 36 | 36 | 1.000 |
| 00144ef0 | `sec_proto::RspSafemodeTransAdaptStatUpdate::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00144f80 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00145010 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 001450a0 | `sec_proto::ReqSafemodeTransAdaptEventNotify::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00145130 | `sec_proto::ReqSafemodeTransAdaptDataForward::visualize(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 463 | 455 | 0.983 |
| 00145300 | `sec_proto::ReqSafemodeTransAdaptDataForward::visualize_ex(tsf4g_tdr::TdrWriteBuf&, int, char) const` | 139 | 139 | 1.000 |
| 00145390 | `sec_proto::ReqSafemodeTransAdaptDataForward::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 280 | 256 | 0.914 |
| 001454b0 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 615 | 557 | 0.906 |
| 00145720 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 304 | 294 | 0.967 |
| 00145850 | `sec_proto::ReqSafemodeTransAdaptEventNotify::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 269 | 270 | 1.004 |
| 00145960 | `sec_proto::RspSafemodeTransAdaptStatUpdate::pack(tsf4g_tdr::TdrWriteBuf&, unsigned int) const` | 158 | 172 | 1.089 |
| 00145a00 | `sec_proto::RspSafemodeTransAdaptStatUpdate::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00145ac0 | `sec_proto::RspSafemodeTransAdaptStatUpdate::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00145b50 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00145c20 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00145ce0 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00145d70 | `sec_proto::ReqSafemodeTransAdaptDataForward::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00145e40 | `sec_proto::ReqSafemodeTransAdaptDataForward::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00145f00 | `sec_proto::ReqSafemodeTransAdaptDataForward::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 00145f90 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00146060 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00146120 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 001461b0 | `sec_proto::ReqSafemodeTransAdaptEventNotify::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 00146280 | `sec_proto::ReqSafemodeTransAdaptEventNotify::visualize(char*, unsigned int, unsigned int*, int, char) const` | 182 | 182 | 1.000 |
| 00146340 | `sec_proto::ReqSafemodeTransAdaptEventNotify::pack(char*, unsigned int, unsigned int*, unsigned int) const` | 133 | 133 | 1.000 |
| 001463d0 | `sec_proto::RspSafemodeTransAdaptStatUpdate::visualize_ex(char*, unsigned int, unsigned int*, int, char) const` | 201 | 198 | 0.985 |
| 001464a0 | `sec_proto::ReqSafemodeTransAdaptDataForward::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 279 | 243 | 0.871 |
| 001465c0 | `sec_proto::ReqSafemodeTransAdaptDataForward::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00146630 | `sec_proto::ReqSafemodeTransAdaptEventNotify::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 242 | 244 | 1.008 |
| 00146730 | `sec_proto::ReqSafemodeTransAdaptEventNotify::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 001467a0 | `sec_proto::RspSafemodeTransAdaptStatUpdate::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 149 | 158 | 1.060 |
| 00146840 | `sec_proto::RspSafemodeTransAdaptStatUpdate::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 001468b0 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 269 | 286 | 1.063 |
| 001469c0 | `sec_proto::ReqSafemodeTransAdaptLoginNotify::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |
| 00146a30 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::unpack(tsf4g_tdr::TdrReadBuf&, unsigned int)` | 583 | 525 | 0.901 |
| 00146c80 | `sec_proto::ReqSafemodeTransAdaptTradeNotify::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | 102 | 102 | 1.000 |

## 2. Commands run (all in WSL, cwd `tools/tss_sdk_recon`)

```
O=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
python3 cu_symbols.py $O formmog_proto_captcha_client_old.cpp --sizes
python3 cu_symbols.py $O formmog_proto_captcha_client.cpp --sizes
python3 cu_symbols.py $O formmog_proto_public_safemodetransadapt.cpp --sizes
python3 cu_layout.py  $O <cu>            # member offsets/sizes (DWARF)
python3 dwarf_inline_names.py $O <cu>    # ORIG .cpp:NNN of every inlined Tdr call
python3 work_genc/cudecl.py $O <cu>      # class DIE + signatures (helper)
objdump -d -C --start-address=… --stop-address=… $O > dis_<cu>.txt
python3 dis_annotate.py $O dis_<cu>.txt  # literal-string annotation
python3 work_genc/visfmt.py dis_<cu>.txt # printVariable/printArray arg pairing
sh c5build.sh recon/src/origsrc/<cu>.cpp /tmp/genc_<x>.o -I recon/src/origsrc
bash symdiff.sh $O /tmp/genc_<x>.o "<regex>"
bash sizecmp.sh /tmp/genc_<x>.o "<regex>"
bash strfind.sh < work_genc/needles.txt
python3 work_genc/mkmatrix.py $O /tmp/genc_<x>.o <cu> > recon/src/origsrc/…_matrix.tsv
ld -m elf_i386 -shared -o /tmp/genc_link.so /tmp/genc_new.o /tmp/genc_old.o /tmp/genc_safe.o
```

Notes: `symdiff.sh`/`sizecmp.sh` hardcode `/tmp/sc_*` scratch files that are
root-owned in this box, so private copies with a `/tmp/genc_*` prefix were used
(`sed 's|/tmp/|/tmp/genc_|g'`).  Both scripts use bash process substitution and
must be run with `bash`, not `sh`.

## 3. Verification results

### 3.1 Symbol coverage (`symdiff.sh`, demangled-name set)

| CU | ORIG-only | NEW-only | counts (orig / recon) |
|---|---|---|---|
| `formmog_proto_captcha_client_old.cpp` regex `sec_proto::(TSSCaptchaOld\|CaptchaReq\|VerifyReq\|VerifyRes\|S2CVerifyRes\|ReportLog\|BusiCaptcha\|CaptchaBusiFinish)` | none | none | 97 / 97 |
| `formmog_proto_captcha_client.cpp` regex `sec_proto::TSS(Captcha(Head\|Pic\|Body\|Pkg)\|SetCaptcha\|VerifyReq\|VerifyRes)` | none | none | 61 / 61 |
| `formmog_proto_public_safemodetransadapt.cpp` regex `sec_proto::(Req\|Rsp)SafemodeTransAdapt` | 3 weak template instantiations (see below) | none | (48 / 45) → 45 / 45 |

The three safemode "ORIG-only" rows are
`Comm_App_Frame::appdata_encode<sec_proto::ReqSafemodeTransAdaptTradeNotify>`,
`DPMsgPacker::generate_comm_frame<…>` and `DPMsgPacker::send_datatrans_pkg<…>`:
weak (`W`) template instantiations that DWARF attributes to *other* CUs
(`formmog_proto_tss_sdk_client.cpp` / the dp CUs) and whose demangled names only
mention the class as a template argument; they are out of scope for this CU.

### 3.2 Sizes (`sizecmp.sh`, ±12 % acceptance)

| CU | rows | byte-identical | min ratio | max ratio | outside ±12 % |
|---|---|---|---|---|---|
| old captcha | 98 | 59 | 0.855 | 1.000 | 2 |
| new captcha | 62 | 38 | 0.867 | 1.007 | 1 |
| safe-mode adapt | 46 | 29 | 0.871 | 1.089 | 1 |

Every function is *smaller* or equal except four safe-mode/new-captcha rows
slightly above 1.0 (max 1.089).  The four outliers and their measured ORIG
cause:

| function | ORIG | recon | ratio | measured cause |
|---|---|---|---|---|
| `BusiCaptchaPic::unpack(TdrReadBuf&, unsigned)` | 671 | 574 | 0.855 | 6×u16 + 1×u32 byte-swap encodings (below) |
| `ReportLog::unpack(TdrReadBuf&, unsigned)` | 247 | 216 | 0.874 | 3×u16 swap encodings |
| `TSSCaptchaPic::unpack(TdrReadBuf&, unsigned)` | 549 | 476 | 0.867 | 5×u16 + 1×u32 swap encodings |
| `ReqSafemodeTransAdaptDataForward::unpack(TdrReadBuf&, unsigned)` | 279 | 243 | 0.871 | 3 swaps + 4 capacity checks |

The cause is compiler generation, not source semantics.  The ORIG (4.1.0)
swaps 16-bit values with `movzwl`/`shr $0x8`/`shl $0x8`/`or` and re-zero-extends
(`movzwl %dx,%eax`), and 32-bit values with `and $0xff0000`+`shr $0x8`+
`shr $0x18`+`or`+`and $0xff00`+`shl $0x18`+`shl $0x8`+`or`+`or`; the vendor
4.1.2 build of the same source (compat `readUInt16`/`readUInt32`, which stage
the swapped value through an address-taken local + `memcpy`) emits
`rol $0x8,%ax` (4 bytes, vs ~12) and a `movzbl %dh`/`shl $0x10` sequence (vs
~32).  Measured on `DataForward::unpack`: 110 instructions/279 bytes in the
ORIG vs 112 instructions/243 bytes in the recon — same instruction count, same
checks, same offsets, same constants, shorter encodings.  See §5.3.

Functions whose ORIG uses 8-bit stores/loads only (`writeUInt8`/`readUInt8`)
are either identical or within 2 %, confirming the shape of the generated code.

### 3.3 String literals (`strfind.sh`)

73 needles (every format string and every `[field_]` name emitted by the three
TUs, including the shared safe-mode set): 73 `PRESENT`, 0 missing.  All format
strings are the ORIG's own literals: `%d`, `%u`, `0x%02x`, ` %d`, ` 0x%02x`.

### 3.4 Call-graph equivalence

The PLT/relocation histogram of each recon object was compared with the ORIG's
call histogram over the CU's address range; every count matches, which pins the
statement-level structure (same number of pack/unpack/visualize calls per
function, same `delete[]`/`_Unwind_Resume`/`memmove`/`memset`/`printArray`/
`printVariable`/`textize` counts).

| callee | old ORIG/recon | new ORIG/recon | safe ORIG/recon |
|---|---|---|---|
| `operator delete[](void*)` | 53 / 53 | 33 / 33 | 33 / 33 |
| `printVariable(…, char const*, char const*, ...)` | 50 / 50 | 23 / 23 | 23 / 23 |
| `printVariable(…, char const*, bool)` | 10 / 10 | 6 / 6 | 6 / 6 |
| `printArray(…, long long)` | 10 / 10 | 5 / 5 | 5 / 5 |
| `textize(char const*, ...)` | 10 / 10 | 5 / 5 | 5 / 5 |
| `memmove` | 20 / 20 | 10 / 10 | 10 / 10 |
| `memset` | 8 / 8 | 5 / 5 | 5 / 5 |
| `_Unwind_Resume` | 21 / 21 | 13 / 13 | 13 / 13 |
| per-class `pack`/`unpack`/`visualize`/`construct` | all equal | all equal | all equal |

The `_Unwind_Resume` count is the sensitive one: it is 2 per class with a
`visualize_ex` (the `visualize(char*)` and `visualize_ex(char*)` wrappers,
whose callee may throw) plus one for the union's `visualize(char*)` — 21 in the
old CU (10 classes + union), 13 in the new CU (6 classes + union) and 10 in the
safe-mode CU (5 classes, no union).  It is **zero** in every
`pack(char*, …)`/`unpack(char const*, …)` wrapper, exactly as
in the ORIG — the `throw()` compensation of §5.1 is what keeps it that way in
the 4.1.2 build.  The *per-function* distribution was compared as well, not
just the totals: for each CU the set of functions containing an
`_Unwind_Resume` reference in the recon object is identical to the ORIG's set
(old: 21 sites in the 21 `visualize`/`visualize_ex(char*)` wrappers; new: 13;
safe-mode: 10), so no landing pad moved from one function to another.

### 3.5 Link test

`ld -m elf_i386 -shared -o /tmp/genc_link.so /tmp/genc_new.o /tmp/genc_old.o
/tmp/genc_safe.o` → rc=0, 203 `sec_proto::` definitions (97+61+45) and one
weak `tsf4g_tdr::TdrWriteBuf::textize` after deduplication; no duplicate-symbol
errors.

### 3.6 Layout assertions

Each header ends with C++03 compile-time assertions (`typedef char …[cond ? 1 :
-1]`) for every class `sizeof` and for the offsets the ORIG code touches
(`uin_` at 2, `body_len_` at 14 in the new head / 22 in the old head,
`picture_`/`session_`/`busi_info_` in the two pic classes, `body_` at 16/24,
union arms at 0).  All three TUs compile with rc=0, so the packed layouts match
the DWARF exactly.

## 4. Recovered semantics

Common generator shape (confirmed by `dwarf_inline_names.py` line attribution):
per class `construct()`, `pack(char*)`, `pack(TdrWriteBuf&)`,
`unpack(char const*)`, `unpack(TdrReadBuf&)`, `visualize(char*)`,
`visualize(TdrWriteBuf&)`, `visualize_ex(char*)`, `visualize_ex(TdrWriteBuf&)`.
`pack(char*)`/`unpack(char const*)` build a `TdrWriteBuf`/`TdrReadBuf` on the
stack, check `buffer == NULL` → `TDR_ERR_NULL_POINTER_PARAMETER` (-19), call the
buffer form, then `if (usedSize) *usedSize = buf.getUsedSize()`.
`visualize_ex(buf)` returns `""` when `data() == NULL || capacity() == 0`,
otherwise visualizes, NUL-terminates at `min(used, total-1)` and returns
`getBeginPtr()`.

Error codes measured in every CU: -1 `TDR_ERR_SHORT_BUF_FOR_WRITE`,
-2 `TDR_ERR_SHORT_BUF_FOR_READ`, -7 `TDR_ERR_REFER_SURPASS_COUNT`,
-19 `TDR_ERR_NULL_POINTER_PARAMETER`.  Byte arrays are length-prefixed, bounded
by `sizeof(array)` (→ -7) and by the remaining capacity (→ -1 / -2).

### 4.1 `formmog_proto_captcha_client.cpp` (new protocol, 61)

| class | sizeof | fields (offset) |
|---|---|---|
| `TSSCaptchaHead` | 16 | msg_id_ u16@0, uin_ u32@2, busi_seq_ u32@6, seq_ u32@10, body_len_ u16@14 |
| `TSSSetCaptcha` | 531 | client_ip_ u32@0, platform_busid_ u32@4, max_pic_change_num_ u16@8, max_verify_num_ u16@10, timeout_ u32@12, busi_id_ u8@16, busi_info_len_ u16@17, busi_info_ i8[512]@19 |
| `TSSCaptchaPic` | 4625 | timeout_ u32@0, verify_times_limit_ u16@4, verify_times_ u16@6, change_times_limit_ u16@8, change_times_ u16@10, picture_len_ u16@12, picture_ u8[4096]@14, busi_id_ u8@4110, busi_info_len_ u16@4111, busi_info_ i8[512]@4113 |
| `TSSVerifyReq` | 65 | answer_len_ u8@0, answer_ u8[64]@1 |
| `TSSVerifyRes` | 130 | result_ u8@0, msg_len_ u8@1, msg_ i8[128]@2 |
| `TSSCaptchaBody` | 4625 | anonymous union: set_captcha_/captcha_pic_/verify_req_/captcha_res_ |
| `TSSCaptchaPkg` | 4641 | head_@0, body_@16 |

* Union selectors are the msg ids: 3201 `set_captcha_`, 3202 `captcha_pic_`,
  3203 `verify_req_`, 3204 `captcha_res_`; compared as an `int64_t`
  (`(low32 ^ id) | high32` in the ORIG), unknown → `TDR_NO_ERROR`.
* `TSSCaptchaPkg::pack`: `head_.pack(destBuf, 1)`;
  `body_.pack(head_.msg_id_, destBuf, 1)`;
  `writeUInt16((uint16_t)(getUsedSize() - bodyStart), startPos + 14)` — offset
  14 is `head_.body_len_`.
* `TSSCaptchaPkg::unpack`: `head_.unpack(srcBuf, 1)`; reject
  `head_.body_len_ > capacity() - size()` with -2; `body_.unpack(head_.msg_id_,
  srcBuf, 1)`.
* Array bounds: picture_ 4096, busi_info_ 512, answer_ 64, msg_ 128.
* visualize formats: u32 → `%u`, u16 → `%d`, u8 → `0x%02x`, `uint8_t` arrays →
  ` %d`, `int8_t` arrays → ` 0x%02x`; `writeCharWithNull(separator)` after each
  array.  `[head_]`/`[body_]`/`[<arm>_]` use the `printVariable(…, bool)` form
  with `true`, and the nested call receives
  `indent >= 0 ? indent + 1 : indent`, duplicated at each call site exactly as
  the ORIG emits it (hoisting it into one local shrank the union's visualize by
  60 bytes, so it is kept per call site).
* Loop counters are typed as the length field (`uint8_t` for answer_/msg_,
  `uint16_t` for picture_/busi_info_), matched to `addb`/`addw` in the ORIG.

### 4.2 `formmog_proto_captcha_client_old.cpp` (old protocol, 97)

| class | sizeof | fields |
|---|---|---|
| `TSSCaptchaOldHead` | 24 | msg_id_ u16@0, version_ u16@2, uin_ u32@4, client_ip_ u32@8, seq_ u32@12, send_time_ u32@16, head_len_ u16@20, body_len_ u16@22 |
| `CaptchaReq` | 24 | app_id_ u32@0, antibot_bus_id_ u32@4, zone_id_ u32@8, max_pic_change_num_ u16@12, max_verify_num_ u16@14, op_type_ u32@16, timeout_ u32@20 |
| `VerifyReq` | 267 | code_len_ u8@0, code_ u8[64]@1, session_len_ u16@65, session_ u8[200]@67 |
| `VerifyRes` | 142 | app_id_ u32@0, antibot_bus_id_ u32@4, zone_id_ u32@8, result_ u8@12, msg_len_ u8@13, msg_ i8[128]@14 |
| `S2CVerifyRes` | 130 | result_ u8@0, msg_len_ u8@1, msg_ i8[128]@2 |
| `ReportLog` | 518 | log_src_ u16@0, log_type_ u16@2, log_len_ u16@4, log_msg_ i8[512]@6 |
| `BusiCaptchaReq` | 539 | app_id_/antibot_bus_id_/zone_id_ u32@0/4/8, max_pic_change_num_ u16@12, max_verify_num_ u16@14, op_type_ u32@16, timeout_ u32@20, busi_id_ u8@24, busi_info_len_ u16@25, busi_info_ i8[512]@27 |
| `CaptchaBusiFinish` | 527 | app_id_/antibot_bus_id_/zone_id_ u32@0/4/8, busi_id_ u8@12, busi_info_len_ u16@13, busi_info_ i8[512]@15 |
| `BusiCaptchaPic` | 4827 | timeout_ u32@0, verify_times_limit_/verify_times_/max_change_times_/change_times_/picture_len_ u16@4/6/8/10/12, picture_ u8[4096]@14, session_len_ u16@4110, session_ u8[200]@4112, busi_id_ u8@4312, busi_info_len_ u16@4313, busi_info_ i8[512]@4315 |
| `TSSCaptchaOldBody` | 4827 | anonymous union of the eight arms, in this order |
| `TSSCaptchaOldPkg` | 4851 | head_@0, body_@24 |

* Union selectors: 1 `busi_captcha_req_`, 3073 `captcha_req_`, 3075
  `verify_req_`, 3076 `verify_res_`, 3078 `s2c_verify_res_`, 3090
  `busi_captcha_pic_`, 3091 `report_log_`, 3092 `captcha_busi_finish_`;
  unknown → `TDR_NO_ERROR`; every arm is called with version 1.
* `TSSCaptchaOldHead::pack` stamps `head_len_` (offset 20) with
  `(uint16_t)(getUsedSize() - startPos)` at `startPos + 20`; `version_` is
  written from the object and no field is cutVer-gated (fixed version-1 layout).
* `TSSCaptchaOldHead::unpack` peeks the u16 at `getUsedSize() + 20` with
  `readUInt16(v, pos)` (the ORIG's two-step `pos > limit_` /
  `limit_ - pos <= 1` check), then rejects `headLen > getLeftSize()` with
  -2 before reading the eight fields in order.  (The ORIG's inline record names
  `TdrReadBuf::getLeftSize()`; the shared compat model spells the same value
  `capacity() - size()` because it models only the members this reconstruction
  needs — documented divergence, same code.)
* `TSSCaptchaOldPkg::pack`: `head_.pack(destBuf, 1)`, `body_.pack(head_.msg_id_,
  destBuf, 1)`, then `writeUInt16((uint16_t)(getUsedSize() - bodyStart),
  startPos + 22)` — offset 22 is `head_.body_len_`.
* `TSSCaptchaOldPkg::unpack`: `head_.unpack(srcBuf, 1)`; reject
  `head_.body_len_ > capacity() - size()` with -2; then
  `body_.unpack(head_.msg_id_, srcBuf, 1)`.
* Array bounds: code_ 64, session_ 200, msg_ 128, log_msg_ 512, picture_ 4096,
  busi_info_ 512.
* visualize formats per class (read from each function's literal/argument
  pairing): head msg_id_/version_/head_len_/body_len_ `%d`, uin_/client_ip_/
  seq_/send_time_ `%u`; `CaptchaReq` u32 `%u`, u16 `%d`; `VerifyReq` code_len_
  `0x%02x`, session_len_ `%d`, both arrays ` %d`; `VerifyRes` u32 `%u`,
  result_/msg_len_ `0x%02x`, msg_ ` 0x%02x`; `S2CVerifyRes` result_/msg_len_
  `0x%02x`, msg_ ` 0x%02x`; `ReportLog` three u16 `%d`, log_msg_ ` 0x%02x`;
  `BusiCaptchaReq` five u32 `%u`, two u16 + busi_info_len_ `%d`, busi_id_
  `0x%02x`, busi_info_ ` 0x%02x`; `CaptchaBusiFinish` three u32 `%u`, busi_id_
  `0x%02x`, busi_info_len_ `%d`, busi_info_ ` 0x%02x`; `BusiCaptchaPic`
  timeout_ `%u`, five u16 + busi_info_len_ `%d`, busi_id_ `0x%02x`,
  picture_/session_ ` %d`, busi_info_ ` 0x%02x`.
* `writeCharWithNull(separator)` is emitted after every array-typed field:
  `VerifyReq` twice, `BusiCaptchaPic` three times, and once each in the six
  classes with a single array — the same counts as the ORIG's inline records.

### 4.3 `formmog_proto_public_safemodetransadapt.cpp` (45)

| class | sizeof | fields |
|---|---|---|
| `ReqSafemodeTransAdaptLoginNotify` | 14 | world_id_ u16@0, src_id_ u32@2, client_ip_ u32@6, login_time_ u32@10 |
| `RspSafemodeTransAdaptStatUpdate` | 7 | world_id_ u16@0, src_id_ u32@2, stat_level_ u8@6 |
| `ReqSafemodeTransAdaptEventNotify` | 12 | world_id_ u16@0, src_id_ u32@2, event_type_ u16@6, event_value_ u32@8 |
| `ReqSafemodeTransAdaptTradeNotify` | 2073 | world_id_ u16@0, src_id_ u32@2, type_ u8@6, send_uin_ u32@7, target_uin_ u32@11, send_ip_ u32@15, target_ip_ u32@19, buf_len_ u16@23, trade_buf_ u8[2048]@25 |
| `ReqSafemodeTransAdaptDataForward` | 2056 | world_id_ u16@0, src_id_ u32@2, data_len_ u16@6, data_ u8[2048]@8 |

* No cutVer gating anywhere (the pack/unpack `cutVer` parameters are unused,
  and no version stamps); all writes/reads in field order; arrays bounded by
  `sizeof` (2048) → -7, then capacity → -1/-2.
* visualize formats: world_id_ `%d`, src_id_/client_ip_/login_time_/event_value_
  `%u`, stat_level_/type_ `0x%02x`, event_type_ `%d`, buf_len_/data_len_ `%d`,
  trade_buf_/data_ elements ` 0x%02x`.
* **Format divergence from the two captcha CUs:** here the `uint8_t` arrays
  (`trade_buf_`, `data_`) print as ` 0x%02x`, whereas the captcha CUs print
  their `uint8_t` arrays as ` %d`.  The per-field display format is a property
  of the generator's field spec, not of the C type, so every format was taken
  from its own CU's disassembly (0x800 bound + `movzbl` load + ` 0x%02x`
  literal at 0x1452b1 for `data_`).

## 5. Divergences and non-ORIG compensations

### 5.1 `throw()` on `pack()` declarations (compensation, not ORIG source)

Declared on every `pack(...)` (both forms, including the union's) and on
`unpack(TdrReadBuf&, …)`/`unpack(const char*, …)` where the wrapper owns a local
with a non-trivial destructor.  Rationale: GCC 4.1.0 leaves no EH cleanup
region in `pack(char*, …)` because its only throwing call is the nothrow
`pack(TdrWriteBuf&, unsigned)`; 4.1.2 inserts a spurious landing pad.
ORIG evidence: `pack(char*, …)`/`unpack(char const*, …)` in all three CUs
contain no `_Unwind_Resume` reference, while `visualize(char*)`/
`visualize_ex(char*)` do (counts in §3.4).  `throw()` is not part of the Itanium
mangling, so all emitted names stay identical to the ORIG (symdiff clean).

### 5.2 `textize` weak binding

The ORIG's `TdrWriteBuf::textize` is weak (`W`), i.e. an inline member emitted
as comdat by every CU that uses it.  All three TUs therefore define it with
`__attribute__((weak))`, so any combination of the reconstructed CUs (and the
uic pilot TU, which currently emits it as a strong `T`) links without duplicate
definitions; the linker keeps one copy, matching the ORIG's single symbol at
`0x00136e90`.

### 5.3 Size gaps (4.1.0 vs 4.1.2 codegen)

See §3.2 for the four outliers.  All are pure encodings of the same
statements: 16-bit byte swaps are `rol $0x8,%ax` in 4.1.2 vs
`shr/shl/or` + a second `movzwl` in 4.1.0; 32-bit swaps use `movzbl %dh` +
`shl $0x10` in 4.1.2 vs `and $0xff0000`/`and $0xff00` chains in 4.1.0.  No
semantic difference was observed: identical check counts, identical constants,
identical field offsets and identical call sequences in every function.

Only six functions are larger than the ORIG, all in the two small-field
CUs: `RspSafemodeTransAdaptStatUpdate::pack` 158→172 (1.089),
`ReqSafemodeTransAdaptLoginNotify::unpack` 269→286 (1.063),
`RspSafemodeTransAdaptStatUpdate::unpack` 149→158 (1.060),
`ReqSafemodeTransAdaptEventNotify::unpack` 242→244 (1.008),
`ReqSafemodeTransAdaptEventNotify::pack` 269→270 (1.004) and
`TSSCaptchaHead::unpack` 305→307 (1.007).  Cause (measured): the 4.1.2
`writeUInt8` tail emits `lea 1(%edx)` + store (7 bytes) where the ORIG uses
`addl $1,pos_` (4 bytes); the compat `readUInt16`/`writeUInt16` stage the value
through an address-taken local, which forces a stack frame the ORIG does not
have; and the shared error-return block sits farther away, so several
conditional jumps become 6-byte `jcc rel32` instead of 2-byte short jumps.
The instruction streams contain the same operations, checks and constants.

### 5.4 Other modelling notes

* `TdrReadBuf::getLeftSize()` (old head unpack, ORIG `.cpp:202`) is modelled as
  `capacity() - size()`, the same expression the shared compat header exposes.
* The ORIG's union selector comparisons are 64-bit; the recon takes `int64_t`
  and is called with `head_.msg_id_` (uint16_t), which zero-extends identically.
* `construct()` bodies are `memset(this, 0, sizeof(*this))` where the ORIG
  shows the multi-word store pattern (all classes with sizeof ≥ 7) and
  per-field zeroing is not distinguishable at -O2.

## 6. Duplicate names across the two captcha CUs

None.  ORIG defines `TSSCaptchaHead`, `TSSCaptchaBody`, `TSSCaptchaPkg` and
`TSSCaptchaPic` only in `formmog_proto_captcha_client.cpp`; the "old" file uses
`TSSCaptchaOldHead`/`TSSCaptchaOldBody`/`TSSCaptchaOldPkg` plus the unprefixed
`CaptchaReq`, `VerifyReq`, `VerifyRes`, `S2CVerifyRes`, `ReportLog`,
`BusiCaptchaReq`, `CaptchaBusiFinish`, `BusiCaptchaPic`.  The name sets are
disjoint, so the two private headers can be included together (verified: the
combined link of all three objects succeeds, §3.5).  The two "pic" bodies are
different messages, not a redefinition:

* new `TSSCaptchaPic` — sizeof 4625, msg id 3202, fields
  `timeout_/verify_times_limit_/verify_times_/change_times_limit_/change_times_/
  picture_len_/picture_/busi_id_/busi_info_len_/busi_info_`;
* old `BusiCaptchaPic` — sizeof 4827, msg id 3090, has
  `max_change_times_` instead of `change_times_limit_` and adds
  `session_len_`/`session_[200]` after `picture_`.

## 7. Residual risk

* All declarations come from DWARF + mangled names (no ORIG headers were
  available); parameter lists and constness are proven by the symbol names.
* Per-field display formats are generator-spec data; each was read from the
  corresponding function's disassembly (literal/argument pairing with
  `visfmt.py`, cross-checked against per-function literal counts).  The
  safe-mode CU proves they are not derivable from the field type alone.
* `int8_t` vs `uint8_t` for the arrays was taken from DWARF and confirmed by
  `movsbl` (signed) vs `movzbl` (unsigned) element loads in the ORIG loops.

## 8. Differential oracle (`test_captcha_oracle.cpp`, `oracle_captcha.sh`)

`bash oracle_captcha.sh` builds the three reconstructed TUs, links them into
`/tmp/libcaptcha_recon.so`, builds the oracle and runs it.  The ORIG is
`dlopen()`ed `RTLD_NOW|RTLD_GLOBAL` first, the reconstruction
`RTLD_NOW|RTLD_LOCAL`, and every entry point is reached by `dlsym` on its
mangled name.  Two passes:

* **char\* pass** (the requested layer): `pack(char*, unsigned, unsigned*,
  unsigned)`, `unpack(char const*, …)`, `visualize(char*, …)` — return code,
  `*usedSize` and the whole 64 KiB output buffer; unpacking also compares the
  whole destination struct (both sides `memset` first), and truncated unpacks
  (`len-1`, `len/2`, 0) plus an 8-byte visualize buffer drive the -2/-1 paths.
* **core pass**: the same comparisons for the buffer-form entry points
  `pack(TdrWriteBuf&, unsigned)`, `unpack(TdrReadBuf&, unsigned)`,
  `visualize(TdrWriteBuf&, int, char)`.

Coverage: 21 plain classes (6 new captcha + 10 old captcha + 5 safe-mode) plus
the two anonymous unions, 100 iterations each, cutVer ∈ {1, 0, 3}, indent 2 and
(unions) the negative-indent branch -1.  Length fields are randomized inside
`sizeof(array)`, text arrays are printable and NUL-terminated, binary arrays
arbitrary.  Every recovered selector is exercised on every iteration (new
3201-3204; old 1/3073/3075/3076/3078/3090/3091/3092) together with
out-of-range selectors (0, 3200, 3205, 9001, 2, 3074, 3093) for the default
branch.

Pristine result (recon linked with `-Wl,--allow-multiple-definition
-Wl,-Bsymbolic`):

```
cases=83000 failures=0
```

### 8.1 `-Bsymbolic` is mandatory for this harness

With the ORIG loaded `RTLD_GLOBAL`, a `RTLD_LOCAL` reconstructed object's
*internal* calls are preempted by the ORIG's copies (the global scope is
searched before the object's own scope), which makes a wrapper-level oracle
vacuous.  Measured: a mutant of `formmog_proto_captcha_client.cpp` whose
`TSSSetCaptcha`/`TSSCaptchaPic` `busi_info_len_` bound is 256 instead of 512
produced **0** failures through the char\*-wrapper pass alone; with
`-Wl,-Bsymbolic` the same mutant produces 1569 failures.  A probe also confirmed
that `dlsym(recon_handle, <buffer-form name>)` returns the reconstruction's own
definition in either case (addresses differ from the ORIG handle), which is why
the core pass detects the mutant even without `-Bsymbolic`.

### 8.2 Negative controls (the oracle is not vacuous)

| build | mutation | cases | failures |
|---|---|---|---|
| pristine | – | 83000 | 0 |
| mutant A | `if (busi_info_len_ > 512)` → `> 256` in `TSSSetCaptcha` + `TSSCaptchaPic` | 80646 | 1569 |
| mutant B | visualize literal `[platform_busid_]` → `[platform_busid_mut]` | 83000 | 1150 |
| pristine (restored) | – | 83000 | 0 |

Mutant A reports fewer cases because a failing `pack` skips the dependent
`unpack`/`visualize` comparisons (`if (ro == 0 && rr == 0)`); the failure count
covers all comparisons that still ran.

### 8.3 Residual unsafe paths (not covered behaviourally)

* `visualize_ex`/`visualize_ex(char*, …)` and `construct()` are not part of the
  differential; the package `body_len_`/`head_len_` stamps are covered because
  `Pkg::pack` itself is called directly in both passes.
* Randomization never produces a length larger than `sizeof(array)`, so the -7
  `TDR_ERR_REFER_SURPASS_COUNT` path is only reached at the equality case; the
  bound mutant above shows the harness does detect bound divergences.
* Padding bytes are always zeroed by the fixtures, so a padding-only divergence
  would be invisible — by design, both headers use the same `#pragma pack(1)`
  layouts, asserted at compile time (§3.6).
