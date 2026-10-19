# DPMsgHandler recv/timer reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
(frozen copy: `reference/libtss_sdk.so.1.0.394.orig`)
ELF32 i386, GCC 4.1.x generation, `-O2 -fPIC`, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Source: `recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_recv_agent.cpp`.
No SDK shared header, README, CMake or test was modified.  The only source
is the two `check_timer` assignments documented in §2.5; the probe lives under
`probes/check_timer/` and is a separate TU.  No commit was made.

This report claims **semantic/control-flow reconstruction**, not byte or
instruction identity.  The compiler generation is close (target GCC 4.1.0/4.1.2;
recompiled with GCC 4.1.2), but register allocation, stack frames, EH block
layout and some inlined container code differ.

## 1. Exact symbols and ORIG ranges

| # | exact mangled symbol | ORIG range | DWARF decl line |
|---|---|---|---|
| 1 | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientDecryptInfoE` | `0x000af180 .. 0x000af2c8` | 746 |
| 2 | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto14DPClientLogPtcE` | `0x000af2d0 .. 0x000af407` | 568 |
| 3 | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto21DPProtocolRequestCodeE` | `0x000af410 .. 0x000af5ad` | 472 |
| 4 | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto15DPClientStatPtcE` | `0x000af5b0 .. 0x000afe63` | 588 |
| 5 | `_ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl` | `0x000afe70 .. 0x000b0b50` | 105 |
| 6 | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientInitDataPtcE` | `0x000b0b50 .. 0x000b0fbf` | 505 |

All six arguments are non-const references, as proved by the mangled `R...`
(not `RK...`) encodings.  `check_timer`'s second argument is `time_t` / `long`
(mangled `l`).

The next function in the ORIG image starts at `0x000b0fc0`
(`OnRecvPkgFromClient`), so the assigned upper range boundary is the start of
that function, not part of `OnRecvMsgFromClient(DPClientInitDataPtc&)`.

## 2. Decoded flow

### 2.1 `OnRecvMsgFromClient(UserInfo*, DPClientDecryptInfo&)` — 0xaf180

`DPClientDecryptInfo`: `decrypt_id_` +0x00, `encrypt_id_` +0x04 (size 8).

```text
0xaf180..0xaf1b6  construct DPBillAdapter("user confirm", 0x14, user)
0xaf1bb..0xaf266  bill << server_new_code_ (u16)
                       << server_cur_code_ (u16)
                       << client_code_[0] (u16)
                       << client_code_[1] (u16)
                       << cmd.decrypt_id_
                       << cmd.encrypt_id_
                       << client_state_
0xaf26b..0xaf277  if ((unsigned)server_new_code_ != cmd.decrypt_id_)
0xaf279..0xaf28c      DPMsgPacker::send_error_report_msg(user, 0x19d, 1)
0xaf291..0xaf2a5      destroy bill; return 0
0xaf2a6..0xaf2b1  else server_cur_code_ = (u16)cmd.decrypt_id_;
                       client_state_ = ECS_INIT_CONFIRM (5);
                       destroy bill; return 0
```

The comparison zero-extends `server_new_code_` and compares against the full
32-bit `decrypt_id_`; the assignment stores only the low 16 bits.  Return is 0
on both paths.  EH cleanup `0xaf2b3..0xaf2c3` destroys the bill and resumes.

### 2.2 `OnRecvMsgFromClient(UserInfo*, DPClientLogPtc&)` — 0xaf2d0

`DPClientLogPtc`: `error_id_` +0x00, `is_encrypt_` +0x04,
`error_msg_len_` +0x05, `error_msg_[128]` +0x06 (size 134).

```text
0xaf2e7..0xaf2f9  if (cmd.is_encrypt_ == 0)
0xaf2ed..0xaf30c      if (glb_dp_config_->dp_proto_conf.dp_decrypt_fail_protect != 0
                          && client_state_ > ECS_INIT_MSG2 (3))
0xaf30c                   client_state_ = ECS_INIT_FAILED (6)
0xaf2fb           msg = "client decrypt fail"
0xaf320           else msg = "client encrypt fail"
0xaf326..0xaf342  construct bill(msg, 0x22, user)
0xaf347..0xaf3c2  bill << client_state_ << error_id_
                       << is_encrypt_ << error_msg_len_
                       << (const char *)error_msg_
0xaf3c2..0xaf3d8  send_error_report_msg(user, 0x19b, 1)
0xaf3dd..0xaf3f1  destroy bill; return 0
```

`dp_decrypt_fail_protect` is the byte at `DPConfig + 0x0e`
(`DPProtoConfig + 8`).  The `client_state_ > 3` test is signed (`jle` in ORIG),
and the failure state is 6.  Return is 0.

### 2.3 `OnRecvMsgFromClient(UserInfo*, DPProtocolRequestCode&)` — 0xaf410

`DPProtocolRequestCode` (packed): `size` +0x00 (u16),
`encrypt_code_id_` +0x02, `decrypt_code_id_` +0x06, `bin_file_id_` +0x0a
(size 12).

```text
0xaf432..0xaf43e  ++request_code_count_;
                  if (request_code_count_ > 4) return -1;
0xaf450..0xaf471  construct bill("user request code", 0x1b, user)
0xaf474..0xaf526  bill << encrypt_code_id_ << decrypt_code_id_
                       << bin_file_id_ << client_code_[0]
                       << server_cur_code_ << bin_file_code_
                       << request_code_count_
0xaf531..0xaf534  destroy bill
0xaf536..0xaf550  if (bin_file_code_ != cmd.bin_file_id_) SendBinFile(user)
0xaf555..0xaf56b  if (cmd.encrypt_code_id_ != (u32)client_code_[0])
                       SendClientCode(user)
0xaf570..0xaf591  if (cmd.decrypt_code_id_ != (u32)server_cur_code_)
                       SendServerCode(user)
0xaf591..0xaf597  return 0
```

The increment is the first operation; the guard compares the incremented
unsigned count against 4.  All three send results are ignored.  Return is -1
only for the count overflow; otherwise 0.  EH `0xaf598..0xaf5a8`.

### 2.4 `OnRecvMsgFromClient(UserInfo*, DPClientStatPtc&)` — 0xaf5b0

`DPClientStatPtc` (packed, size 1580):

| offset | field |
|---|---|
| +0x000 | internal_msg_send_count_ u32 |
| +0x004 | internal_msg_recv_count_ u32 |
| +0x008 | game_pkg_recv_count_ u32 |
| +0x00c | fix_encrypt_count_ u32 |
| +0x010 | game_fix_decrypt_count_ u32 |
| +0x014 | game_dynamic_decrypt_count_ u32 |
| +0x018 | seq_less_count_ u32 |
| +0x01c | seq_greater_count_ u32 |
| +0x020 | seq_discard_count_ u32 |
| +0x024 | down_cmd_list_crc_ u32 |
| +0x028 | need_encrypt_count_ u16 |
| +0x02a | need_encrypt_list_[128] (u32 cmd_id_, u32 cmd_count_) |
| +0x42a | client_error_count_ u16 |
| +0x42c | client_error_list_[128] (u16 cmd_id_, u16 cmd_count_) |

Flow:

```text
0xaf5c7..0xaf5da  time(&user_timer_.last_recv_stat_pkg_time_)       // +0x9c
0xaf5e0..0xaf5ee  send_missing = (int)internal_msg_send_count_ (+0xa8)
                                - (int)cmd.internal_msg_recv_count_ (+4)
0xaf5f1..0xaf5ff  recv_missing = (int)cmd.internal_msg_send_count_ (+0)
                                - (int)internal_fix_decrypt_count_ (+0xbc)
                                - (int)internal_dynamic_decrypt_count_ (+0xc4)
                                + 1
0xaf602..0xaf60e  if (send_missing > 2 || recv_missing > 2)
0xaf614..0xaf70a      bill("missing dp package", 0x28) << send_missing
                          << recv_missing << user.internal_msg_send_count_
                          << cmd.internal_msg_recv_count_
                          << cmd.internal_msg_send_count_
                          << user.internal_fix_decrypt_count_
                          << user.internal_dynamic_decrypt_count_
0xaf70f..0xaf71b  if (down_cmd_list_crc_ (+0x8c) != cmd.down_cmd_list_crc_ (+0x24))
0xaf721..0xaf739      if (client_state_ > 4
                          || game_dynamic_decrypt_count_ (+0xc8) != 0
                          || internal_dynamic_decrypt_count_ (+0xc4) != 0)
0xaf73f..0xaf7c1          bill("cmd list crc error", 0x25)
                              << user.down_cmd_list_crc_ << cmd.down_cmd_list_crc_;
                          send_error_report_msg(user, 0x198, 1)
0xaf7c6..0xaf7cb  if (cmd.seq_less_count_ (+0x18) != 0)
0xafd6a..0xafdfc      bill("server seq error", 0x27)
                          << seq_less_count_ << seq_greater_count_
                          << seq_discard_count_;
                      send_error_report_msg(user, 0x197, seq_less_count_)
0xaf7d1..0xaf7ff  fix_encrypt_mismatch =
                      (int)cmd.game_fix_decrypt_count_ (+0x10)
                      - ((int)user.fix_encrypt_count_ (+0xb4)
                         - (int)user.internal_fix_encrypt_count_ (+0xb0))
0xaf7e8..0xaf7ff  fix_decrypt_mismatch =
                      (int)user.game_fix_decrypt_count_ (+0xc0)
                      + (int)user.internal_fix_decrypt_count_ (+0xbc)
                      - (int)cmd.game_dynamic_decrypt_count_ (+0x0c)
0xaf801..0xaf810  if (fix_encrypt_mismatch > 2 || fix_decrypt_mismatch > 2)
0xafa85..0xafbb8      bill("fix encrypt mismatch", 0x29)
                          << fix_encrypt_mismatch << fix_decrypt_mismatch
                          << cmd.game_fix_decrypt_count_
                          << user.fix_encrypt_count_ (+0xb4)
                          << user.internal_fix_encrypt_count_ (+0xb0)
                          << user.internal_fix_decrypt_count_ (+0xbc)
                          << user.game_fix_decrypt_count_ (+0xc0);
                      send_error_report_msg(user, 0x199,
                          abs(fix_encrypt_mismatch) + abs(fix_decrypt_mismatch))
0xaf816..0xaf81b  if (cmd.need_encrypt_count_ (+0x28) != 0)
0xafbc8..0xafca0      bill("server no encrypt", 0x26);
                      for (i = 0; i < need_encrypt_count_; ++i) {
                          bill << list[i].cmd_id_ << list[i].cmd_count_;
                          total += list[i].cmd_count_;
                      }
                      bill << total;
                      send_error_report_msg(user, 0x195, total)
0xaf821..0xaf853  simulate_count =
                      (int)cmd.game_dynamic_decrypt_count_ (+0x14)
                      + (int)cmd.game_fix_decrypt_count_ (+0x10)
                      - (int)user.dynamic_encrypt_count_ (+0xb8)
                      - ((int)user.fix_encrypt_count_ (+0xb4)
                         - (int)user.internal_fix_encrypt_count_ (+0xb0))
                  if (simulate_count > 0)
0xaf859..0xaf94c      bill("simulate encrypt data", 0x2a)
                          << simulate_count << cmd.game_fix_decrypt_count_
                          << cmd.game_dynamic_decrypt_count_
                          << user.dynamic_encrypt_count_
                          << user.fix_encrypt_count_
                          << user.internal_fix_encrypt_count_;
                      send_error_report_msg(user, 0x199, simulate_count)
0xaf951..0xaf959  if (cmd.client_error_count_ (+0x42a) != 0)
0xafca5..0xafd65      bill("client report error", 0x24);
                      for (i = 0; i < client_error_count_; ++i) {
                          bill << list[i].cmd_id_ << list[i].cmd_count_;
                          send_error_report_msg(user,
                              list[i].cmd_id_ + 0x1c2, list[i].cmd_count_);
                      }
0xaf95f..0xafa84  ZEN_Trace_LogMsg::debug_output(
                      (flags_ & 1) ? RS_INFO : RS_DEBUG,
                      "<661-byte format at 0x21c05c>", uin, __func__, ...);
0xafa78..0xafa84  return 0
```

The debug format string is byte-identical to ORIG `.rodata` at `0x21c05c`
(661 bytes, 26 placeholders; the 27th argument is pushed but the format has no
final `=%u`, matching ORIG).  `__func__` is `OnRecvMsgFromClient`.

### 2.5 `DPMsgHandler::check_timer(UserInfo*, time_t)` — 0xafe70

State is `user->user_code_info_.client_state_` (+0x64).  Timer fields:
`last_check_init_pkg_time_` +0x94, `last_send_test_pkg_time_` +0x98,
`last_recv_stat_pkg_time_` +0x9c, `last_send_bin_code_time_` +0xa0,
`last_send_bin_file_time_` +0xa4.  Timeouts are at
`DPConfig + 0x138 + n*4`:

| DPTimeConfig field | DPConfig offset |
|---|---|
| max_init_pkg_timeout | +0x138 |
| max_static_pkg_timeout | +0x13c |
| max_confirm_pkg_timeout | +0x140 |
| max_send_test_pkg_timeout | +0x144 |
| send_bin_file_gap | +0x14c |
| send_bin_code_gap | +0x150 |

State dispatch at `0xafe8a..0xafeb0`:

- `ECS_NOT_INIT (1)` -> `0xafee7`
- `ECS_INIT_MSG (2)` -> `0xaff76`
- `ECS_INIT_MSG2 (3)` -> `0xb0055`
- `ECS_INIT_ING (4)` -> `0xb040c`
- `ECS_INIT_CONFIRM (5)` -> `0xb0136`
- all other values -> common tail `0xafeb6`

```text
state 1 (0xafee7..0xaff71):
  if (unsigned(now - last_check_init_pkg_time_) >= max_init_pkg_timeout) {
      bill("user init timeout", 8) << last_check_init_pkg_time_;
      send_error_report_msg(user, 0x19c, 1);
      last_check_init_pkg_time_ = now;
  }

state 2 (0xaff76..0xb0517):
  if (bin_file_code_ (+0x86) == 0) {
      new_bin = select_server_bin_file_id(user);   // weighted rand on server_code_.bin_file_
      bin_file_code_ = (u16)new_bin;
      if (new_bin == 0 || SendBinFile(user) != 0) {
          bin_file_code_ = 0;
          send_error_report_msg(user, 0x1a2, 1);
          goto tail;
      }
      last_send_bin_file_time_ = now;
  }
  if (up_cmd_list_pos_ (+0x90) < 0)
      if (SendCmdList(user) != 0) goto tail;
  if (client_code_[0] (+0x7c) == 0) {
      new_client = select_client_code_id(user);    // client_code_.select_bin_code_id(bin_file_code_)
      client_code_[0] = (u16)new_client;
      if (new_client != 0 && SendClientCode(user) != 0) {
          client_code_[0] = 0;
          goto tail;
      }
  }
  new_server = select_server_code_id(user);        // server_code_.select_bin_code_id(bin_file_code_)
  server_new_code_ (+0x7a) = (u16)new_server;
  if (new_server == 0 || SendServerCode(user) != 0) {
      server_new_code_ = 0;
      goto tail;
  }
  client_state_ = 4;
  last_send_bin_code_time_ = now;

state 3 (0xb0055..0xb0093):
  if (up_cmd_list_pos_ < 0)
      if (SendCmdList(user) != 0) goto tail;
  if (SendServerCode(user) != 0) goto tail;
  client_state_ = 4;
  last_send_bin_code_time_ = now;

state 4 (0xb040c..0xb049d):
  if (unsigned(now - last_send_bin_code_time_) >= max_confirm_pkg_timeout) {
      bill("confirm overtime", 0x15) << last_send_bin_code_time_;
      send_error_report_msg(user, 0x19a, 1);
      last_send_bin_code_time_ = now;
  }

state 5 (0xb0136..0xb0407):
  if (unsigned(now - last_send_bin_file_time_) >= send_bin_file_gap) {
      old_bin_id = bin_file_code_;
      new_bin_id = select_server_bin_file_id(user);
      bin_file_code_ = (u16)new_bin_id;
      if (new_bin_id != old_bin_id && new_bin_id != 0) {
          if (SendBinFile(user) != 0)
              bin_file_code_ = (u16)old_bin_id;
          else
              last_send_bin_file_time_ = now;
      } else {
          bin_file_code_ = (u16)old_bin_id;
      }
  }
  if (unsigned(now - last_send_bin_code_time_) >= send_bin_code_gap) {
      new_server = select_server_code_id(user);
      new_client = select_client_code_id(user);
      if (new_client > 0 && new_server > 0 &&
          (new_server != (int)server_cur_code_ ||
           new_client != (int)client_code_[0])) {
          old_client = client_code_[0];
          client_code_[0] = (u16)new_client;
          res = SendClientCode(user);
          client_code_[0] = (u16)old_client;
          if (res == 0) {
              memmove(&client_code_[1], &client_code_[0], 8);
              client_code_[0] = (u16)new_client;
              server_new_code_ = (u16)new_server;
              if (SendServerCode(user) == 0) {
                  client_state_ = 4;
                  last_send_bin_code_time_ = now;
              }
          }
      }
  }
  if (unsigned(now - last_send_test_pkg_time_) >= max_send_test_pkg_timeout) {
      last_send_test_pkg_time_ = now;
      DPInterData pkg;                       // packed 1585 bytes
      pkg.head.packet_size_     = 15;
      pkg.head.protocol_type_   = 15;
      pkg.head.data_type_       = 0;
      pkg.body.cs_test_data_    = "0123456789";
      SendDpMsgToClient(user, pkg, 0, 0);
      pkg.head.packet_size_     = 8;
      pkg.head.protocol_type_   = 0x14;
      pkg.head.data_type_       = 0;
      pkg.body.config_data_.proto_check_switch_ =
          glb_dp_config_->dp_proto_conf.proto_check_switch;   // +0xb
      pkg.body.config_data_.seq_check_switch_ =
          glb_dp_config_->dp_proto_conf.seq_check_switch;     // +0xc
      pkg.body.config_data_.raw_check_switch_ =
          glb_dp_config_->dp_proto_conf.raw_check_switch;     // +0xd
      SendDpMsgToClient(user, pkg, 0, 0);
  }
  if (unsigned(now - last_recv_stat_pkg_time_) >= max_static_pkg_timeout) {
      last_recv_stat_pkg_time_ = now;
      bill("static pkg overtime", 0x17);
      send_error_report_msg(user, 0x196, 1);
  }

common tail (0xafeb6..0xafee6):
  if (base_info_.flags_ (+0x20) & 1)
      send_error_report_msg(user, 0x1a5, 1);
  return 0;
```

The two test packets reuse the same `DPInterData` object; the second call
overwrites only header bytes and body bytes 0..2, exactly as ORIG does.
`DPInterData` is 1585 bytes (`head` 5 + `body` 1580), packed.

**ORIG evidence for the config packet (2026-10-19).** The two assignments
above are corrected against the frozen backup
`reference/libtss_sdk.so.1.0.394.orig` (SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`,
identical to `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`):

- `0x000b0a37`: `mov WORD PTR [ebp-0x63f],0x14` sets
  `head.protocol_type_ = 0x14`. `ebp-0x641` is `head.packet_size_`;
  `ebp-0x63f` is `head.protocol_type_`; `ebp-0x63d` is `head.data_type_`.
- `0x000b0a40`: `mov WORD PTR [ebp-0x641],0x8` sets
  `head.packet_size_ = 8`.
- `0x000b0a49..0x000b0a7a` copies `DPConfig` bytes +0x0b/+0x0c/+0x0d into
  `body.config_data_` +0/+1/+2.
- `sec_proto::DPInterDataBody::packEx` at `0x0015bac0` dispatches on the
  protocol word. Protocol `0x14` calls `DPConfigData::pack` (3 bytes);
  protocol `8` has no case, so `DPInterData::pack` returns success but writes
  only the 5-byte head. The old `packet_size_=20 / protocol_type_=8` therefore
  emitted a 5-byte bodyless frame and silently dropped the three switch bytes.
- ORIG `sec_proto::DPInterData::pack(char*, unsigned, unsigned*, unsigned)` at
  `0x0015c8c0` serializes the head big-endian: `00 08 00 14 00`, then the
  three config bytes; `used = 8`.
- Both test calls go through PLT `0x0007d934`; the GOT slot
  `0x00294e18` is `R_386_JUMP_SLOT` for the reference-overload symbol
  `_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoERN9sec_proto11DPInterDataEPht`.
  The probe executable exports the same symbol, so both the ORIG PLT call and
  the rebuilt DSO's call bind to the probe's output-boundary hook.
- The state-5 config pointer load at `0x000b0136` uses GOT slot
  `0x002943ac`, an `R_386_GLOB_DAT` for
  `_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E` (`0x002bb4c0`), so the probe's
  `dlsym`-written config pointer is the one `check_timer` reads.

Isolated differential probe: `probes/check_timer/` (separate TU; it does not
include the target .cpp). `probes/check_timer/build_and_run.sh` loads ORIG
from the reference backup (the default; `TSS_SDK_ORIG` overrides it), builds
the current source and the patched pre-fix copy as host `g++ -m32` DSOs
(cross-DSO layouts are asserted by the probe header), calls both rebuilt
`check_timer` implementations, and intercepts `SendDpMsgToClient` at the
output boundary.  It covers elapsed
199/200/201 and all eight combinations of the three config switches, with
the bin-file/bin-code/stat timers held at elapsed 0.  The captured
`DPInterData` is serialized by ORIG's real `DPInterData::pack`; the
encryption/send tail is deliberately outside the probe boundary. The probe
requires the pre-fix copy to fail (5-byte bodyless config frame) and the
fixed copy to match ORIG (8-byte config frame), which also rules out the
rebuilt function being silently preempted by ORIG. Per the delegation
contract this subagent did not build or run the probe; the lead runs it.

Run from the canonical root:

```sh
# Builds after/before and requires AFTER == ORIG plus BEFORE != ORIG.
probes/check_timer/build_and_run.sh

# Pure failing-before run (expected exit 1):
probes/check_timer/build/check_timer_probe \
    --orig reference/libtss_sdk.so.1.0.394.orig \
    --after probes/check_timer/build/libcheck_timer_before.so
```

All comparison elapsed values are unsigned 32-bit (`jb`/`jae` in ORIG).

### 2.6 `OnRecvMsgFromClient(UserInfo*, DPClientInitDataPtc&)` — 0xb0b50

`DPClientInitDataPtc` size 36: `code_ver_[16]` +0, `client_info_` +0x10,
`client_user_id_` +0x14, `client_code_id_` +0x18, `server_code_id_` +0x1c,
`bin_code_id_` +0x20, `bin_count_` +0x22.

```text
0xb0b67  client_state_ = ECS_INIT_MSG (2)
0xb0b79..0xb0b8a  base_info_.client_user_id_ (+0x1c) =
                      Crc32(&cmd.client_user_id_, 4)
0xb0b95..0xb0ba4  if (glb_dp_config_->dp_base_conf.is_use_client_version (+2))
                      base_info_.client_version_ (+0x18) = cmd.client_info_
0xb0ba7..0xb0c18  check_version_hash(client_version_, cmd.code_ver_)
0xb0c1a..0xb0c32  mismatch defaults:
                      is_client_code_mismatch = 1
                      is_server_code_mismatch = 1
                      is_bin_count_mismatch  = -1
                      new_code               = -1 (64-bit)
0xb0df7..0xb0ec7  on support match: memcmp(version_hash_, cmd.code_ver_, 16)
                  then check_client_code(cmd.client_code_id_) and
                  check_server_code(cmd.server_code_id_); success sets
                  is_bin_count_mismatch = 0, new_code = 0
0xb0c39..0xb0d63  bill("user init msg", 7) << new_code (64-bit)
                      << is_client_code_mismatch
                      << is_server_code_mismatch
                      << cmd.client_info_ << cmd.client_code_id_
                      << cmd.server_code_id_ << cmd.bin_code_id_
                      << cmd.bin_count_ << cmd.client_user_id_
                      << base_info_.client_user_id_
0xb0d68..0xb0d6e  now = time(0)
0xb0d73..0xb0d78  if (is_bin_count_mismatch != 0) SendBinCount(user)
0xb0d7e..0xb0d8e  if (client mismatch or server mismatch) skip assignments
                  (still calls check_timer once at 0xb0dea)
0xb0d94..0xb0dd3  else:
                      bin_file_code_          = cmd.bin_code_id_
                      client_code_[0]         = (u16)cmd.client_code_id_
                      server_cur_code_        = (u16)cmd.server_code_id_
                      server_new_code_        = (u16)cmd.server_code_id_
                      last_send_bin_file_time_= now
                      last_send_bin_code_time_= now
                      client_state_           = ECS_INIT_MSG2 (3)
0xb0df6  return check_timer(user, now)
```

The first logged value is a single 64-bit `-1` on the version-hash failure path
and `0` on success (ORIG forms it from the esi:edi pair; source named variable
is not recoverable, so it is modelled as `long long new_code`).  The
`is_server_code_mismatch` flag is tested (together with the client flag) but is
**not** logged separately in ORIG; only `is_client_code_mismatch` appears in
the bill.  The name `is_bin_count_mismatch` is the DWARF name; in this slice it
is initialized to -1 and cleared to 0 by a successful version-hash check, and
drives `SendBinCount`, not a direct comparison of `cmd.bin_count_`.

## 3. Inlined code-manager model (check_timer / init validation)

The ORIG line table attributes the following bodies to
`dp_vmbase_engine.h` / `dp_code_mgr.h`; they are inlined into these six
functions and have no out-of-line symbols in ORIG.  The reconstruction models
them as inline members of partial class declarations, so
`-fno-implement-inlines` emits no new out-of-line symbols.

Container layouts (DWARF verified):

```text
CodeItem<N>   = ushort weight_; ushort code_len_; uchar code_buf_[N]
CodeItems<N>  = ushort total_weight_; CodeItem<N> items_[100]
CodeInfo      = CodeItems<2000> cryptdata_code_;
                CodeItems<2000> crypt_code_;
                CodeItems<20000> bin_file_;
DpCodeInfo    = ushort version_count_; uint version_support_list_[8];
                uchar version_hash_[16]; CodeInfo client_code_;
                CodeInfo server_code_;
DpCodeInfos   = uint code_update_time_; ushort code_count_;
                DpCodeInfo code_infos_[2];
```

Offsets/sizes: `CodeItems<2000>` 200402, `CodeItems<20000>` 2000402,
`CodeInfo` 2401206, `DpCodeInfo` 4802464 (client_code_ +52,
server_code_ +2401258), `DpCodeInfos` 9604936 (code_infos_ +8).

Decoded selection semantics:

```text
CodeItems<N>::select_code_id(r):
  for i = 0..99:
      if (r < items_[i].weight_) return i;
      r -= items_[i].weight_;
  return 100;

CodeInfo::select_bin_file_id():
  r = rand() % bin_file_.total_weight_;
  return bin_file_.select_code_id(r);

CodeInfo::select_bin_code_id(code_id):
  r = rand();
  r %= cryptdata_code_.total_weight_ + crypt_code_.total_weight_;
  if (r < cryptdata_code_.total_weight_ && code_id != 0)
      return code_id;
  r %= crypt_code_.total_weight_;
  return 100 + crypt_code_.select_code_id(r);

DpCodeInfo::is_support(version):
  if (version_count_ == 0) return false;
  if (version_support_list_[0] == version ||
      version_support_list_[0] == 0) return true;
  for (i = 1; i < version_count_; ++i)
      if (version_support_list_[i] == version ||
          version_support_list_[i] == 0) return true;
  return false;

DPCodeMgr::select_server_bin_file_id(version):
  for each code_info with is_support(version)
      return info->server_code_.select_bin_file_id();
  return 0;

DPCodeMgr::select_client_code_id(version, code_id):
  for each code_info with is_support(version)
      return info->client_code_.select_bin_code_id(code_id);
  return 0;

DPCodeMgr::select_server_code_id(version, code_id):
  for each code_info with is_support(version)
      return info->server_code_.select_bin_code_id(code_id);
  return 0;

DPCodeMgr::check_version_hash(version, hash):
  for each code_info with is_support(version)
      return memcmp(info->version_hash_, hash, 16) == 0;
  return false;

DPCodeMgr::check_client_code(version, code_id):
  for each code_info with is_support(version) {
      unsigned short len;
      return info->client_code_.get_bin_code_buf(code_id, len) == 0;
  }
  return true;                       // no support => mismatch

DPCodeMgr::check_server_code(version, code_id):
  same, with server_code_.
```

`CodeInfo::get_bin_code_buf` maps code ids 1..99 to
`cryptdata_code_.items_[id]` and 101..199 to
`crypt_code_.items_[id - 100]`; ids 0, 100, >199 return null.  The `len`
output is not consumed by these callers and is optimized away; its behaviour
outside these calls is not proven.  `rand()` is consumed even when `code_id`
is nonzero, matching ORIG (six `rand` call sites in `check_timer`).

## 4. Packet / state layouts used

| type | size | key offsets |
|---|---|---|
| `sec_proto::DPClientDecryptInfo` | 8 | decrypt_id_ 0, encrypt_id_ 4 |
| `sec_proto::DPClientLogPtc` | 134 | error_id_ 0, is_encrypt_ 4, error_msg_len_ 5, error_msg_ 6 |
| `sec_proto::DPProtocolRequestCode` | 12 (packed) | size 0, encrypt_code_id_ 2, decrypt_code_id_ 6, bin_file_id_ 0xa |
| `sec_proto::DPClientStatPtc` | 1580 (packed) | see §2.4 |
| `sec_proto::DPClientInitDataPtc` | 36 | code_ver_ 0, client_info_ 0x10, client_user_id_ 0x14, client_code_id_ 0x18, server_code_id_ 0x1c, bin_code_id_ 0x20, bin_count_ 0x22 |
| `sec_proto::DPInterData` | 1585 (packed) | head 0 (5 bytes), body 5 (1580) |
| `tss_sdk_impl::UserInfo` | 400 | base_info_ 0, user_code_info_ 0x64, user_timer_ 0x94, user_stat_ 0xa8, hack_data_ 0xdc |
| `tss_dp_comm::DPConfig` | 22842 | dp_base_conf 0, dp_proto_conf 6, dp_time_confg 0x138 |

`UserInfo` key offsets used: `uin_` 0, `client_version_` 0x18,
`client_user_id_` 0x1c, `flags_` 0x20; `client_state_` 0x64,
`request_code_count_` 0x74, `server_cur_code_` 0x78, `server_new_code_` 0x7a,
`client_code_` 0x7c, `bin_file_code_` 0x86, `down_cmd_list_crc_` 0x8c,
`up_cmd_list_pos_` 0x90; timers 0x94/0x98/0x9c/0xa0/0xa4; counters
0xa8/0xb0/0xb4/0xb8/0xbc/0xc0/0xc4/0xc8.

## 5. External dependencies (exact undefined symbols)

The object references exactly these real ORIG symbols:

```text
_ZN14CommBillRecordlsEPKc
_ZN14CommBillRecordlsEx
_ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz
_Z5Crc32Phj
_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E
_ZN9tss_dpsdk11DPMsgPacker21send_error_report_msgEPN12tss_sdk_impl8UserInfoEii
_ZN9tss_dpsdk12DPMsgHandler11SendBinFileEPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk12DPMsgHandler11SendCmdListEPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk12DPMsgHandler12SendBinCountEPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk12DPMsgHandler14SendClientCodeEPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk12DPMsgHandler14SendServerCodeEPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoERN9sec_proto11DPInterDataEPht
_ZN9tss_dpsdk13DPBillAdapter5bill_E
_ZN9tss_dpsdk13DPBillAdapterC1EPKcjPN12tss_sdk_impl8UserInfoE
_ZN9tss_dpsdk13DPBillAdapterD1Ev
memmove
rand
time
```

`CommBillRecord` is global (not `tss_dpsdk::CommBillRecord`), matching the ORIG
mangled names.  `DPBillAdapter::operator<<` is the header-inline wrapper; the
integral overload forwards to `CommBillRecord::operator<<(long long)` and the
`error_msg_` array is forwarded as `char const*`.

## 6. Verification

### 6.1 Compile (GCC 4.1.2 generation)

```text
sh /home/loyieking/dnf_workspace/dnf_secsvr/toolchain/c5xx.sh \
  -fno-stack-protector -c \
  recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_recv_agent.cpp \
  -o /tmp/dp_msg_handler_recv_agent.o
```

Result: exit 0, no diagnostics.  Defined symbols in the object:

```text
_ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl
_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto14DPClientLogPtcE
_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto15DPClientStatPtcE
_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientDecryptInfoE
_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientInitDataPtcE
_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto21DPProtocolRequestCodeE
__i686.get_pc_thunk.bx
```

No code-manager helper symbols, no invented helpers, no asm bridge, no weak
inline copies.

The same TU also compiles with WSL `g++ (Ubuntu 13.3.0) -m32 -std=gnu++98 -O2
-fPIC -fno-implement-inlines -fno-stack-protector -I recon` (exit 0).

### 6.2 Call-multiplicity comparison (ORIG vs this object)

All six functions match exactly, including destruction/unwind counts:

| function | ORIG -> ours |
|---|---|
| DPClientDecryptInfo | ctor 1/1, lsEx 7/7, send 1/1, dtor 2/2, unwind 1/1 |
| DPClientLogPtc | ctor 1/1, lsEx 4/4, lsPKc 1/1, send 1/1, dtor 2/2, unwind 1/1 |
| DPProtocolRequestCode | ctor 1/1, lsEx 7/7, dtor 2/2, SendBinFile 1/1, SendClientCode 1/1, SendServerCode 1/1, unwind 1/1 |
| DPClientStatPtc | time 1/1, ctor 7/7, lsEx 30/30, dtor 10/10, send 6/6, debug 1/1, unwind 3/3 |
| check_timer | send 5/5, ctor 3/3, lsEx 2/2, dtor 4/4, SendServerCode 3/3, rand 6/6, SendClientCode 2/2, memmove 1/1, SendCmdList 2/2, SendBinFile 2/2, SendDpToClient 2/2, unwind 1/1 |
| DPClientInitDataPtc | Crc32 1/1, ctor 1/1, lsEx 10/10, dtor 2/2, time 1/1, check_timer 1/1, SendBinCount 1/1, unwind 1/1 |

### 6.3 Instruction counts (not identity)

Normalized mnemonic/operand comparison (addresses/numbers canonicalized):

| function | ORIG insns | ours | ndiff |
|---|---:|---:|---:|
| DPClientDecryptInfo | 83 | 96 | 57 |
| DPClientLogPtc | 82 | 84 | 77 |
| DPProtocolRequestCode | 106 | 120 | 74 |
| DPClientStatPtc | 533 | 565 | 614 |
| check_timer | 784 | 872 | 1026 |
| DPClientInitDataPtc | 314 | 360 | 278 |

Residues are codegen/structural: GCC 4.1.0 vs 4.1.2 frame/register allocation,
EH block ordering, and inlined container selection code.  No byte-identity is
claimed.

### 6.4 Layout checks

A compile-time probe including this TU verified:

- `sizeof(UserInfo)==400`, offsets 0/100/148/168/220;
- `sizeof(DPClientStatPtc)==1580`, list offsets 42/1066/1068;
- `sizeof(DPProtocolRequestCode)==12`, field offsets 0/2/6/10;
- `sizeof(DPClientInitDataPtc)==36`;
- `sizeof(DPInterData)==1585`, body offset 5;
- `sizeof(DPConfig)==22842`, dp_proto_conf +6, dp_time_confg +312;
- `DPConfigBase::is_use_client_version` +2;
- `DPProtoConfig::proto_check_switch` +5, `dp_decrypt_fail_protect` +8;
- `DPTimeConfig` timeout offsets 0/4/8/12/20/24;
- `CodeInfo==2401206`, `DpCodeInfo==4802464`, `DpCodeInfos==9604936`.

The debug format string emitted by this TU is byte-identical to ORIG
`.rodata` at `0x21c05c` (661 bytes).  All bill labels (`user confirm`,
`client encrypt/decrypt fail`, `user request code`, the seven stat labels,
`user init timeout`, `confirm overtime`, `static pkg overtime`,
`user init msg`) are present with the ORIG text.

## 7. Integration boundary / unresolved

- This TU uses partial SDK type declarations only.  On integration, replace
  them with the real headers; the field offsets/sizes above must remain.
- `tss_dpsdk::DPVMBaseEngine` models only `encrypt_vm_`, `decrypt_vm_`,
  `code_manager_` (the only members touched); the real object is 68 bytes.
- `tss_sdk_impl::UserInfo::HackData` opaque subobjects are byte arrays; fields
  are not used by these six functions.
- `tss_dp_comm::DPConfig` is packed in this partial model to reproduce size
  22842; all accessed field offsets are checked.  `DPLogConf`, `DPCodeInfoConf`,
  `DPValueArray128` and `DPCmdHackConf` are opaque byte regions.
- The first bill value in the init handler is a single 64-bit `-1`/`0` in
  ORIG (formed from esi:edi).  Its original symbolic name/expression is not
  recoverable; it is modelled as `long long new_code`.
- `is_bin_count_mismatch` is the DWARF name but its value is -1/0 driven by the
  version-hash check in this slice; no direct bin-count comparison appears.
- `CodeItems<N>::get_code_buf`'s `unsigned short& len` output is unused by
  these callers and optimized away; its exact outside-call behaviour is not
  proven.
- The `select_bin_code_id` random-consumption semantics are decoded from the
  ORIG instruction order (rand consumed before the code-id short-circuit).
- No identity claim is made for the whole `libtss_sdk.so`.
