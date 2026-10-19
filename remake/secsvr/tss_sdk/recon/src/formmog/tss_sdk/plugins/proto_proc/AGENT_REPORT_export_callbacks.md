# AGENT_REPORT_export_callbacks

Reconstruction of the five remaining plugin-callback entry points of
`recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_export_api_agent.cpp`
against ORIG
`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(ELF32 i386, GCC 4.1.x, `-O2 -fPIC`).

Evidence sources: `nm -S -C` (dynamic and full symtab), bounded `objdump -d`,
`.debug_info` through `dwarf_funcs.py`/`dwarf_full_extract.py` plus a
throwaway elftools DIE walker (not kept), and `.rodata` dumps.

## 1. Symbols

| demangled symbol | ORIG addr | ORIG size | ORIG binding | recon size | ratio |
|---|---|---|---|---|---|
| `tss_sdk_impl::punish_user_callback(void*)` | 0x0008a7b0 | 0x89 (137) | `t` local | 0x89 (137) | 1.000 |
| `tss_sdk_impl::captcha_callback(void*)` | 0x0008b570 | 0xde (222) | `t` local | 0xd8 (216) | 0.973 |
| `tss_sdk_impl::safemode_plugin_callback_proc(void*)` | 0x0008c210 | 0x16d (365) | `T` global | 0x167 (359) | 0.984 |
| `tss_sdk_impl::plugin_callback_proc(void*)` | 0x0008c790 | 0x59 (89) | `T` global | 0x59 (89) | 1.000 |
| `tss_sdk_impl::send_anti_data_to_client(void*)` | 0x0008a970 | 0x175 (373) | `T` global | 0x175 (373) | 1.000 |

`punish_user_callback` and `captcha_callback` are *local* in ORIG, so they are
deliberately declared `static` here (same `t` binding). The two functions are
referenced only from this TU (`tss_sdk_anti_init`, `tss_sdk_captcha_init`);
a tree-wide grep found no other recon TU using them.
`send_anti_data_to_client` is declared and used by
`tss_sdk_exports_timer_agent.cpp` (registered as the CHANNEL plugin callback in
`tss_sdk_get_comm_interf` @0x0008b010), so it stays external with the same
`int (void*)` signature.

## 2. Decoded semantics

### 2.1 `punish_user_callback` @0x0008a7b0 (DWARF lines 83-88)

Input: the user_control plugin's 32-byte `UserControlPunishUser`
(`.debug_info` ._527); output: public `TssSdkPunishUserInfo` (._492, 24 bytes).

| out field (off) | source (data off) |
|---|---|
| `uin_` +0 | `uin_` +0 |
| `punish_mode_` +4 | `punish_mode_` +8 |
| `punish_time_` +c | `punish_time_` +c |
| `punish_reason_len_` +c (u16) | `punish_reason_len_` +18 (u32, truncated) |
| `punish_reason_` +e | `punish_reason_` +1c |
| `punish_info_len_` +12 (u16) | `punish_info_len_` +10 (u32, truncated) |
| `punish_info_` +14 | `punish_info_` +14 |

(`game_id_` +4 / `room_id_` +6 are not read.)

Log (exact bytes, `.rodata` 0x219544):
`punish_user_, uin = %u, punish_mode = %d, punish_time = %d, punish_reason_len = %d, punish_info_len = %d`
with args `uin`, `punish_mode`, `punish_time`, `(int)reason_len`,
`(int)info_len` (the two lengths are zero-extended 16-bit values).

Tail: `return g_anti_init_info_.punish_user_(&punish_user_info);` — ORIG
`call *0x92b4(%ebx)` = `g_anti_init_info_` (0x0029d7e4) + 4, not null-checked;
the function returns the callback's result unchanged. No `data` null check.

### 2.2 `send_anti_data_to_client` @0x0008a970 (DWARF lines 28-65)

Input: `TssSdkPluginChannelGameChannelPkg` (14 bytes, byte-packed, ._494):
`busi_type_` +0 (int), `uin_` +4, `buf_` +8, `buf_size_` +c (short).
Channel plugin -> `send_pkg_to_game_channel` -> `callback_proc_`.

```
send_data_info.uin_ = pkg->uin_;                     // always, before dispatch
unsigned char send_buffer[0x10000];
busi_type_ == 0:                                     // antibot packet
    const int dp_big_pkg_flag_size = 4;
    sec_proto::DPInterDataHead head;                 // 5 bytes, ._head@0x1271b
    head.packet_size_  = (unsigned short)pkg->buf_size_;   // payload length
    head.protocol_type_ = 0;
    head.data_type_     = 1;
    size_t use_len = 0;
    head.pack((char*)send_buffer + 4, sizeof(send_buffer), &use_len, 0);
    use_len += 4;
    if (use_len + (int)pkg->buf_size_ > sizeof(send_buffer)) return -1;
    memcpy(send_buffer + use_len, pkg->buf_, pkg->buf_size_);
    send_data_info.anti_data_     = send_buffer;
    send_data_info.anti_data_len_ = (unsigned short)(use_len + pkg->buf_size_);
    log "send antibot pkg.cmd=%d len=%d\n",
        ntohs(*(u16*)pkg->buf_), (int)pkg->buf_size_;
busi_type_ == 1:                                     // DP packet, verbatim
    send_data_info.anti_data_     = pkg->buf_;
    send_data_info.anti_data_len_ = (unsigned short)pkg->buf_size_;
    log "send dp pkg.cmd=%d len=%d\n",
        ntohs(*(u16*)(pkg->buf_ + 2)), (int)pkg->buf_size_;
otherwise: return -2;
if (g_anti_init_info_.send_data_to_client_ != 0)
    if (g_anti_init_info_.send_data_to_client_(&send_data_info) != 0) return -3;
return 0;
```

Notes:
* `sec_proto::DPInterDataHead::pack(char*, unsigned int, unsigned int*, unsigned int) const`
  @0x0015d2a0 (PLT 0x7bd34) writes the 5-byte head and stores the used length.
* The 4 bytes at `send_buffer[0..3]` are **not written** by ORIG, yet
  `anti_data_len_` counts them; preserved as-is (the "big pkg flag" slot).
* `head.packet_size_` is the *payload* length (no +5), unlike
  `DPMsgHandler::SendDpMsgToClient`'s convention; preserved.
* The byte swap in both logs is glibc `__bswap_16` / `ntohs` (DWARF block
  locals `__v`, `__x` of type `short unsigned int` at lines 58/65); the recon
  uses `ntohs` from `<arpa/inet.h>` and c5root GCC 4.1 emits `rol $8,%ax`
  (ORIG: `ror $8,%ax`, identical 16-bit rotate).
* Callback call site: ORIG `mov 0x92b0(%ebx),%edx; test; je -> 0; call *%edx`
  (`g_anti_init_info_` +0).
* Return codes: `-1` overflow, `-2` bad `busi_type_`, `-3` callback failure,
  `0` success / no callback installed.

### 2.3 `captcha_callback` @0x0008b570 (DWARF lines 29-60)

Input: the captcha plugin reuses the SDK's own 4641-byte `CaptchaPkg`
(`CaptchaPkgHead` 16 bytes + `CaptchaPkgBody` union 4625) for its replies.
Dispatch on `head_.msg_id_` (u16 at +0), both branches `return 0`.

`0x0c82` (picture data) builds public `TssSdkCaptchaPicInfo` (._487, 37 bytes):

| out (off) | source |
|---|---|
| `uin_` +0 | head `uin_` +2 |
| `req_seq_` +4 | head `busi_seq_` +6 |
| `captcha_seq_` +8 | head `seq_` +a |
| `timeout_` +c | body `captcha_pic_.timeout_` +10 |
| `verify_times_limit_` +10 | +14 |
| `verify_times_` +12 | +16 |
| `change_times_limit_` +14 | +18 |
| `change_times_` +16 | +1a |
| `picture_len_` +18 | +1c |
| `picture_` +1a | address of inline `picture_[4096]` at +1e |
| `captcha_type_` +1e (u8) | `busi_id_` +101e |
| `busi_info_len_` +1f | +101f |
| `busi_info_` +21 | address of inline `busi_info_[512]` at +1021 |

then `g_captcha_info.on_recv_pic_data_(&captcha_pic)`.

`0x0c84` (verification result) builds public `TssSdkCaptchaVerifyRspInfo`
(._490, 16 bytes):

| out (off) | source |
|---|---|
| `req_seq_` +0 | head `busi_seq_` +6 (ORIG reads +6, not `seq_` +a) |
| `result_` +4 | body `captcha_res_.result_` +10 (u8, zero-extended) |
| `err_msg_len_` +8 | `captcha_res_.msg_len_` +11 (u8, zero-extended) |
| `err_msg_` +c | address of `captcha_res_.msg_[128]` at +12 |

then `g_captcha_info.on_verify_rsp_(&verify_res)`.

`g_captcha_info` is the 8-byte BSS object at ORIG 0x0029e445 (immediately after
`g_tss_sdk_captcha_initialized`); the two indirect calls are ORIG
`call *0x9f11(%ebx)` / `call *0x9f15(%ebx)`. ORIG's
`tss_sdk_captcha_init` @0x0008b650 never writes that object (its argument is
loaded nowhere, `plugin_array[0].init_data_` is a literal 0), so both calls go
through null pointers in the shipped library. That latent bug is reproduced
faithfully (the global is never assigned here either).

### 2.4 `safemode_plugin_callback_proc` @0x0008c210 (DWARF lines 32-102)

Input: `SafemodePkg` (90 bytes: `SafemodePkgHead` 6 + `SafemodePkgBody`
union 84). `sec_data == 0` -> 1. Switch on `head_.msg_type_` (u16 +0):

* `0x15` `limit_status_` (._502, 20): `uin_` <- body+4, `limit_status_` <-
  body+8, `unlock_url_len_` (u32 body+c) gate `> 0x3ff`, `memcpy` 1024-byte
  `TssSdkSecOptLimitInfo::unlock_url_` from body+10, local NUL at
  `unlock_url_[len]`, `return g_set_limitinfo(&info)`.
* `0x17` `mba_safe_status_` (._503, 8): `uin_`, `status_` from body+0/+4,
  `return g_set_safestatus(&status)`.
* `0x19` `mba_verify_info_` (._505, 14): `uin_`, `seq_` from body+0/+4,
  `verify_url_len_` (short body+8) gate **as u16** `> 0x3ff`, `memcpy` into
  `TssSdkSecOptVerifyInfo::verify_url_` from body+a, then
  `((char*)body_verify_url)[len] = 0` — ORIG terminates the *plugin's source
  string*, not the copy (kept, with comment), `return g_set_verifyinfo(...)`.
* `0x1a` `mba_verify_status_` (._506, 12): `uin_`, `seq_`, `result_` from
  body+0/+4/+8, `return g_set_verifyresult(&verify_status)`.
* default -> 1; the two oversized-URL paths log
  `ZEN_Trace_LogMsg::debug_errorex("not enough buf.")` (.rodata 0x219780) and
  return 1.

`sizeof(info.unlock_url_) - 1` (u32 compare, `cmp $0x3ff,%ecx; jbe`) and
`(unsigned short)verify_url_len > sizeof(...) - 1` (16-bit compare,
`cmp $0x3ff,%dx; ja`) reproduce ORIG's two distinct comparisons.

Call targets (ORIG GOT offsets, all confirmed against `.symtab`):
`0x15` -> `call *0xa514` = `g_set_limitinfo` (0x0029ea48),
`0x17` -> `0xa518` = `g_set_safestatus` (0x0029ea4c),
`0x19` -> `0xa51c` = `g_set_verifyinfo` (0x0029ea50),
`0x1a` -> `0xa520` = `g_set_verifyresult` (0x0029ea54).
Branch values are the saved callbacks' results (ORIG jumps to the common
epilogue without touching `%eax`); only the invalid/oversized paths return 1.

### 2.5 `plugin_callback_proc` @0x0008c790 (DWARF lines 27-32)

Input: the uic plugin's naturally aligned 28-byte `UicUserInputInfo`
(._493; the same layout as its `uic_user_input_info.h` record). Builds public
`TssSdkUicJudgeResult` (._487, 22 bytes):

| out (off) | source |
|---|---|
| `uin_` +0 | +0 |
| `msg_len_` +8 | `message_len_` +8 |
| `msg_` +c | `message_` +c |
| `callback_data_len_` +10 (u16) | +10 |
| `callback_data_` +12 | +14 |
| `msg_result_flag_` +4 | `msg_result_flag_` +4, except `EMSG_NOT_OPERATE_FLAG` (3) -> 0 |

The mapping compiles to ORIG's exact `cmp $3 / sete / sub $1 / and` sequence.
Tail: `return g_on_judge_result(&rsp_user);` — ORIG `call *0xab14(%ebx)` =
`g_on_judge_result` (0x0029f048), not null-checked, result returned unchanged.

## 3. Supporting declarations added/changed in the TU

* `#include <arpa/inet.h>` for `ntohs`.
* `struct TssSdkPluginChannelGameChannelPkg` (global scope, `packed`, 14) —
  same definition as the channel-plugin unit.
* `namespace sec_proto { struct DPInterDataHead {...} __attribute__((packed)); }`
  with the 4-arg `pack` declaration (5 bytes).
* `TssSdkCaptchaPicInfo` (37) and `TssSdkCaptchaVerifyRspInfo` (16) in the
  packed public-argument block (previously only forward-declared).
* `tss_sdk_impl::UicUserInputInfo` (28, naturally aligned) after
  `#pragma pack(pop)` — a `pack(1)` version would be 26 bytes; DWARF is 28 with
  `callback_data_` at +0x14.
* `static TssSdkCaptchaInitInfo g_captcha_info;` (ORIG 0x0029e445).
* `SafemodeVerifyInfo::verify_url_` changed `const char*` -> `char*` to match
  DWARF and to allow the preserved source-string terminator write.
* New compile-time checks: sizes 28/14/5/37/16 and offsets
  `picture_` 0x1a, `captcha_type_` 0x1e, `busi_info_` 0x21, `err_msg_` 0xc,
  `UicUserInputInfo::callback_data_` 0x14, `input_type_` 0x18,
  `buf_size_` 0xc, `data_type_` 4.

No other symbol changed: comparing the pre-edit and post-edit `.o` symbol
tables leaves only the five new functions plus `g_captcha_info` as
additions/updates.

## 4. Verification actually run (inside WSL, from `tools/tss_sdk_recon`)

```
sh c5build.sh recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_export_api_agent.cpp /tmp/export_final.o
  -> rc=0 (no diagnostics)

sh symdiff.sh <ORIG> /tmp/export_final.o \
   "punish_user_callback|captcha_callback|safemode_plugin_callback_proc|plugin_callback_proc|send_anti_data_to_client"
  -> == ORIG-only ==   (empty)
  -> == NEW-only ==    t tss_sdk_impl::captcha_callback(void*)
                       t tss_sdk_impl::punish_user_callback(void*)
  -> counts (orig new) 3 / 5

bash sizecmp.sh /tmp/export_final.o "punish_user_callback|captcha_callback|safemode_plugin_callback_proc|plugin_callback_proc|send_anti_data_to_client"
  -> matched=3 identical=2 differing=1
       365    359  0.984  tss_sdk_impl::safemode_plugin_callback_proc(void*)

nm -C -S --defined-only <ORIG> | grep ' t tss_sdk_impl::...'   (full symtab)
nm -C -S /tmp/export_final.o | ...
  -> punish_user_callback      ORIG 0x89 t   recon 0x89 t
  -> captcha_callback          ORIG 0xde t   recon 0xd8 t
  -> safemode_plugin_callback  ORIG 0x16d T  recon 0x167 T
  -> plugin_callback_proc      ORIG 0x59 T   recon 0x59 T
  -> send_anti_data_to_client  ORIG 0x175 T  recon 0x175 T

bash recon/link_check.sh /tmp/tss_recon_all.o
  -> rc=0

printf '%s\n' 'punish_user_, uin = %u, ...' 'not enough buf.' | bash strfind.sh
  -> PRESENT (both)

python3 strcheck.py <ORIG> "<needle1>" "<needle2>" "<needle3>" "<needle4>"
python3 strcheck.py /tmp/export_final.o <same needles>
  -> PRESENT x4 in both  (needles are the byte-exact strings, i.e. the two
     send_* formats carry a real trailing 0x0a; the punish format is the exact
     0x219544 string; "not enough buf." is 0x219780)
```

`strfind.sh` is line-oriented and therefore cannot express the two format
strings that end in `\n` (its input would be split); `strcheck.py` (12 lines,
kept in the tools dir) does a byte-exact search and was used for those. A
weaker prefix check also passes for all four via `grep -acF` (result `1` each).

Instruction-level comparison (`objdump -d`) of ORIG vs the new `.o`:
`punish_user_callback`, `plugin_callback_proc` and `send_anti_data_to_client`
are instruction-for-instruction identical (same field offsets, same operand
order, same `rol`/`ror` equivalence, same call slots); `captcha_callback` and
`safemode_plugin_callback_proc` differ only in stack-slot offsets and the
scheduling of independent stores.

## 5. Divergences / unresolved

1. `symdiff.sh` reads ORIG's **dynamic** symbol table. `punish_user_callback`
   and `captcha_callback` are local in ORIG (`t`, no `.dynsym` entry), so no
   tool using `nm -D` can ever "match" them; they necessarily appear as
   NEW-only there. Against ORIG's full `.symtab` name, binding and size match
   exactly.
2. `_GLOBAL__I_` static-initializer key names: ORIG has one per source TU
   (`send_anti_data_to_client`, `tss_sdk_captcha_init`,
   `safemode_plugin_callback_proc`, `plugin_callback_proc`); this merged TU
   emits a single key, `global constructors keyed to
   tss_sdk_impl::tss_sdk_add_user(...)`. Reproducing four keys would need
   per-TU compilation or asm, both out of scope/forbidden. They are local
   symbols, so they never show up in a `-D`-based diff.
3. `safemode_plugin_callback_proc` frame: recon `sub $0x858` vs ORIG
   `sub $0x428`. GCC kept two separate 1032-byte slots for the disjoint
   `TssSdkSecOptLimitInfo info` (case 0x15) and `TssSdkSecOptVerifyInfo
   verify_info` (case 0x19); ORIG shared one slot. Function code size is 359 vs
   365 bytes; no observable behaviour depends on the frame size (both objects
   are fully written before use in their branches).
4. `g_captcha_info` is never written by the shipped build, so
   `captcha_callback` performs its two indirect calls with null pointers when
   the captcha plugin replies. Reproduced intentionally (ORIG behaviour), see
   2.3.
5. The first four bytes of `send_anti_data_to_client`'s send buffer are
   uninitialised in ORIG yet included in `anti_data_len_`; reproduced, see 2.2.
6. `safemode_plugin_callback_proc` writes the NUL terminator through the
   plugin's `verify_url_` pointer (ORIG does the same); this mutates caller
   memory and is preserved verbatim.
