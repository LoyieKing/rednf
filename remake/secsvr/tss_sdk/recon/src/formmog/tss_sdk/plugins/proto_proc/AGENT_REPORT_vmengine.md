# AGENT_REPORT_vmengine

Semantic reconstruction of `tss_dpsdk::DPVMBaseEngine` from
`libtss_sdk.so.1.0.394` (SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`,
ELF32 i386, DWARF present, original compiler GCC 4.1.x).

Source written:
`recon/src/formmog/tss_sdk/plugins/proto_proc/dp_vmbase_engine_agent.cpp`

## Verdict

All requested `DPVMBaseEngine` methods are reconstructed at semantic level:
class layout, selection logic, wire headers, lengths, guards, return values,
error codes, billing/report calls, VM vtable slots and external call targets
match the original disassembly. This is **not** a claim of byte-identical
code, identical stack layout or identical RNG call-for-call machine code; the
observable semantics listed below are the acceptance basis.

2026-10-19 correction: `fix_encrypt`'s capacity guard is
`out_len < buf_len + 0x11a` (282), not `+17`; the source and the
`fix_encrypt` section below are corrected. The reconstruct is otherwise
unchanged. The captured 57-byte Mars tail failure is in `TenCrypt`
(`CMars::Mars` tail), not in this translation unit.

## Evidence / method

- DWARF `DW_TAG_*` for class/struct layouts, members, virtual method order and
  formal parameter types (`readelf --debug-dump=info`, pyelftools).
- `objdump -d -C -Mintel` for the original ranges listed per function and for
  the `CUPVM` vtable at `0x00293988` (used to confirm CVMBase slot order).
- String/format extraction by virtual address from the original `.rodata`.
- Original mangled symbol check against every external reference emitted by
  the reconstructed object.

## Exact layouts used by this unit

### DPVMBaseEngine (`dp_vmbase_engine.h:28`, size 0x44)

| off | member | type |
|---|---|---|
| +0x00 | `encrypt_vm_` | `CVMBase*` |
| +0x04 | `decrypt_vm_` | `CVMBase*` |
| +0x08 | `code_manager_` | `DPCodeMgr*` |
| +0x0c | `encrypt_vm_thread_mutex_` | `ZEN_Thread_Mutex` (28 bytes) |
| +0x28 | `decrypt_vm_thread_mutex_` | `ZEN_Thread_Mutex` (28 bytes) |

`CVMBase` is 8 bytes: vptr + `int VMError`. The reconstructed vtable order is
`Init, Release, GetDecodeCode, GetEncodeCode, GetHeader, SetHeader,
GetDataBuffer, SetKeyTree, SetKeyMatrix, SetKeyLink, Run, Load, GetLastError,
SetError`. Relevant slots used here:

| slot | offset from vptr | method |
|---|---|---|
| 0 | 0x00 | `Init()` |
| 2 | 0x08 | `GetDecodeCode(unsigned char*, unsigned char*&, unsigned short&)` |
| 3 | 0x0c | `GetEncodeCode(unsigned char*, unsigned char*&, unsigned short&)` |
| 6 | 0x18 | `GetDataBuffer(unsigned char*&, unsigned int&)` |
| 10 | 0x28 | `Run()` |
| 11 | 0x2c | `Load(void*, size_t, void*, size_t)` |

`ZEN_Thread_Mutex` is 28 bytes (confirmed by the two mutex offsets).

### DPCodeMgr / code tables

- `DPCodeMgr`: 4 bytes, `DpCodeInfos *code_infos_` at +0.
- `DpCodeInfos`: 9604936 bytes (`0x928f48`):
  - +0 `code_update_time_` (u32)
  - +4 `code_count_` (u16)
  - +8 `code_infos_[2]`
- `DpCodeInfo`: 4802464 bytes (`0x4947a0`):
  - +0 `version_count_` (u16)
  - +4 `version_support_list_[8]` (u32)
  - +36 `version_hash_[16]`
  - +52 `client_code_` (`CodeInfo`)
  - +2401258 `server_code_` (`CodeInfo`)
- `CodeInfo`: 2401206 bytes: `cryptdata_code_` at +0,
  `crypt_code_` at +200402, `bin_file_` at +400804.
- `CodeItems<N>` has one u16 `total_weight_` and 100 `CodeItem<N>`
  entries; `CodeItem<N>` is u16 weight, u16 code length, `code_buf_[N]`.
  Sizes: `CodeItems<2000>` = 200402, `CodeItems<20000>` = 2000402.
- Inlined `get_code_buf` uses `items_[code_id]` and rejects
  `code_id == 0 || code_id > 99`; it writes the output length first and
  returns `code_buf_` (the +6 offset inside the item).

### sec_proto wire structures (byte-packed)

- `DPInterDataHead` (5): `packet_size_` u16 +0, `protocol_type_` u16 +2,
  `data_type_` u8 +4.
- `DPFixAlgorithmHead` (12): `key_id_` +0, `algorithm_type_` +4, `crc` +8.
- `DPDynamicAlgorithmHead` (8): `code_id_` +0, `crc_` +4.
- `DPAlgorithmData` (265): `usr_id_crc_` +0, `custom_data_` +4,
  `pad_len_` +8, `pad_data_[256]` +9.
- `DPInterDataBody` (1580): union; only `fix_algorithm_data_` and
  `dynamic_algorithm_data_` are named here, remaining members are represented
  by a raw 1580-byte alternative.
- `DPInterData` (1585): `head` +0, `body` +5.

### UserInfo (400 bytes) offsets used

`base_info_.uin_` 0x00, `client_version_` 0x18, `client_user_id_` 0x1c,
`flags_` 0x20; `user_code_info_.client_state_` 0x64,
`last_encrypt_sequence_` 0x68, `last_game_decrypt_sequence_` 0x6c,
`last_internal_decrypt_sequence_` 0x70, `server_cur_code_` 0x78,
`server_new_code_` 0x7a, `client_code_[5]` 0x7c; `user_stat_.fix_encrypt_key_`
0xac, `dynamic_encrypt_count_` 0xb8, `internal_fix_decrypt_count_` 0xbc,
`game_fix_decrypt_count_` 0xc0, `internal_dynamic_decrypt_count_` 0xc4,
`game_dynamic_decrypt_count_` 0xc8.

`E_CLIENT_STATE`: NOT_READY=0, NOT_INIT=1, INIT_MSG=2, INIT_MSG2=3,
INIT_ING=4, INIT_CONFIRM=5, INIT_FAILED=6.

### DPConfig (`tss_dp_comm::DP_CONFIG::glb_dp_config_`)

- +0 `dp_base_conf` (6 bytes)
- +6 `dp_proto_conf`:
  - +8 `dp_max_encrypt_size` (u16)
  - +10 (`0x0a`) `use_sequence_check` (u8)
  - +15 (`0x0f`) `explicit_use_dynamic_encrypt` (u8)

## Function reconstruction

### `DPVMBaseEngine::DPVMBaseEngine()` — ORIG 0x000bd380 (C1), 0x000bd420 (C2)

Both symbols are identical full constructors. Order:
1. `encrypt_vm_ = 0`, `decrypt_vm_ = 0`.
2. `ZEN_Thread_Mutex::ZEN_Thread_Mutex(&encrypt_vm_thread_mutex_, 0)`.
3. `ZEN_Thread_Mutex::ZEN_Thread_Mutex(&decrypt_vm_thread_mutex_, 0)`.
4. `code_manager_ = new DPCodeMgr()` (4 bytes, first word set to 0).

Cold paths destroy the already-constructed mutex(es) and resume unwinding.
The constructor does not call `DPCodeMgr::init()`.

### `~DPVMBaseEngine()` — ORIG 0x000bd2c0 (D2), 0x000bd320 (D1)

Both symbols are identical. Destroys `decrypt_vm_thread_mutex_` (+0x28) first,
then `encrypt_vm_thread_mutex_` (+0x0c). It intentionally does **not** delete
or release `encrypt_vm_`, `decrypt_vm_` or `code_manager_`.

### `int init()` — ORIG 0x000bd200..0x000bd2bf

```
if (code_manager_->init() != 0)          log "DPCodeMgr init fail"; return -1;
encrypt_vm_ = GetVM();
if (!encrypt_vm_ || encrypt_vm_->Init() == 0)
                                          log "Init encrypt_vm failed! %p"; return -1;
decrypt_vm_ = GetUPVM();
if (!decrypt_vm_ || decrypt_vm_->Init() == 0)
                                          log "Init decrypt_vm failed! %p"; return -1;
return 0;
```

`GetVM`/`GetUPVM` are global factories; `Init` is CVMBase slot +0x00. The log
argument is the pointer value (0 when the factory failed).

### `int reload()` — ORIG 0x000bd1d0..0x000bd1ff

`return code_manager_->reload();` No null check, no VM action.

### `void init_encrypt_data(UserInfo*, DPAlgorithmData&, unsigned int)` — ORIG 0x000bd130..0x000bd1cf

```
usr_id_crc_ = (user && user->client_state_ != ECS_NOT_INIT)
              ? user->base_info_.client_user_id_
              : default_user_id_crc;
pad_len_ = rand() % 16;
for (i = 0; i < (pad_len_ >> 2) + 1; ++i)
    ((int*)pad_data_)[i] = (rand() << 16) + rand();
custom_data_ = custom_data;
```

The condition is exactly `user == 0 || client_state_ == 1`. The four-byte pad
writes start at byte offset 9 of `DPAlgorithmData` (unaligned by design). The
reconstruction uses `memcpy` for those four-byte stores to avoid alignment UB
while preserving the byte pattern; original DWARF names the local `int*`.

### `unsigned char *get_server_code(UserInfo*, u16 code_id, u16& len)` — ORIG 0x000bce50..0x000bcf6f

```
unsigned char *buf = 0;
code_data = code_manager_->get_server_code(user->client_version_, code_id, len);
if (!code_data) return 0;
return decrypt_vm_->GetEncodeCode(code_data, buf, len) ? buf : 0;
```

The DPCodeMgr portion is inlined in the original: select the first
`DpCodeInfo` whose `version_support_list_` contains the version or a zero
entry; then:
- `code_id <= 99`: index `code_id`;
- `code_id > 99`: index `code_id - 100`;
- the inlined `get_code_buf` accepts only indices 1..99.

Consequently IDs 1..99 and 101..199 address items 1..99; IDs 0, 100 and >=200
return 0. `len` is written from `CodeItem::code_len_` only after the bound
check. The VM call is slot +0x0c (`GetEncodeCode`) on `decrypt_vm_` (+0x04).
`len` is passed by reference to both the lookup and the VM call. `buf_len == 0`
is not separately rejected.

### `unsigned char *get_client_code(UserInfo*, u16 code_id, u16& len)` — ORIG 0x000bcf70..0x000bd08f

Same flow with `code_manager_->get_client_code` (uses `client_code_`),
`encrypt_vm_` (+0x00) and CVMBase slot +0x08 (`GetDecodeCode`).

### `int fix_encrypt(UserInfo*, buf, buf_len, out, out_len&)` — ORIG 0x000bd4c0..0x000bd88f

Prefix is 17 bytes: `DPInterDataHead` (5) + `DPFixAlgorithmHead` (12).

1. Guard `out_len < buf_len + 0x11a` (282 = 17-byte DPInterData prefix +
   265-byte maximum DPAlgorithmData; ORIG 0x000bd4df `add eax,0x11a`).
   On failure log priority 4 with `buf_len`, `out_len`,
   `head_buf_len=17` (the log value is 17; ORIG 0x000bd81b) and return -1.
   The earlier reconstruction used `+17`; that was corrected 2026-10-19.
2. `head.protocol_type_ = 0x11`, `head.data_type_ = 0`.
3. `algorithm_type_ = rand() % 19 + 1`.
4. Key:
   - user: `user->fix_encrypt_key_ += (rand() >> 8) + 1`; use new value;
   - no user: `(rand() << 16) + rand()`.
5. `dp_data = out + 17`.
6. `init_encrypt_data(user, algorithm_data, 0)`.
7. `algorithm_data.pack(dp_data, out_len - 17, &used, 0)`; then
   `memcpy(dp_data + used, buf, buf_len); used += buf_len`.
8. `fix_data.crc = Crc32(dp_data, used)`.
9. Log key/algorithm/crc/pad.
10. `Cipher(algorithm_type_, dp_data, used, key_id_, true)`; zero (or an
    exception) creates `DPBillAdapter(msg, 0x1d, user)`, records
    `algorithm_type_`, `key_id_`, `used` through
    `CommBillRecord::operator<<(long long)`, and returns -1.
    Messages: `"cipher encrypt fail"`, `"cipher encrypt exception"`.
11. `head.packet_size_ = (u16)(used + 17)`.
12. `DPInterData::pack(out, out_len, 0, 0)`; `out_len = packet_size_`.
13. Log `"[%u][%s], packet_size_=%u"`; return 0.

Length truncations to reproduce: `packet_size_` is u16, and the Cipher call
uses the full 32-bit `used`.

### `int check_decrypt_buf(UserInfo*, buf, buf_len, out, out_len&, crc, err_type, is_internal_pkg)` — ORIG 0x000bd890..0x000bdd6f

1. `crc_value = Crc32(buf, buf_len)`.
2. Log `"[%u][%s], crc=%u, crc_value=%u, buf_len=%d"` at priority 3 if
   `(user->flags_ & 1)`, else 2; user-less uses uin 0.
3. `crc != crc_value`:
   - `DPBillAdapter("decrypt CRC fail", err_type, user)`;
   - record `crc`, `crc_value`, `buf_len` as long long;
   - if user: `send_error_report_msg(user, 0x1a1, 1)`;
   - return -1.
4. `DPAlgorithmData::unpack(buf, buf_len, &used, 0)`.
   If error or `buf_len <= used`: log priority 4
   `"[%u][%s], DPAlgorithmData unpack fail, ret=%d, used=%d, pad=%d, buf_len=%d"`
   and return -1.
5. `remaining = buf_len - (unsigned short)used`; `dp_data = buf + used`.
6. With user:
   - log `used`, `usr_id_crc_`, `custom_data_`, `pad`;
   - if `client_state_ > 1 && client_user_id_ != usr_id_crc_`:
     bill type `err_type`, message `"client user_id crc fail"`,
     record `client_user_id_`, `usr_id_crc_`, send `0x1a0`/1, return -1;
   - choose `last_internal_decrypt_sequence_` if `is_internal_pkg`, else
     `last_game_decrypt_sequence_`;
   - accept if `seq < custom_data_ || seq == 0`;
   - else if `glb_dp_config_->dp_proto_conf.use_sequence_check`:
     bill message `"client sequence error"`, record `is_internal_pkg`
     (sign-extended), `seq`, `custom_data_`, send `0x19f`/1, return -1;
   - update `seq = custom_data_` **before** the output-capacity check.
   Without user: log priority 2 with uin 0 and skip both user-id and sequence
   checks/update.
7. If `out_len < remaining`: log priority 4
   `"[%u][%s], too small out buf in decrypt, out=%d, buf_len=%d"`; return -1.
8. `out_len = remaining`; `memcpy(out, buf + used, remaining)`; return 0.

### `int dynamicy_decrypt(...)` — ORIG 0x000bdd70..0x000be09f (spelling preserved)

1. `DPDynamicAlgorithmHead::unpack(buf, buf_len, &used, 0)`.
   If error or `buf_len <= used`: log priority 4
   `"[%u][%s], DPDynamicAlgorithmHead unpack fail, buf_len=%d"`; return -1.
2. `remaining = buf_len - (unsigned short)used`; `dp_data = buf + used`.
3. Search `user->client_code_[0..4]` for the full 32-bit `code_id_`.
   If absent: bill `"client use other encode id"` type 0x20, record
   `code_id_`, `remaining`, send `0x193`/1, return -1.
4. `code_buf = get_client_code(user, (unsigned short)code_id_, code_len)`.
   Log `code_buf`, `code_len`, full `code_id_`.
5. If `code_buf == 0 || code_len == 0`: error `"get client code fail"`.
   Else `decrypt_vm_->Load(code_buf, code_len, dp_data, remaining)`;
   failure -> `"decrypt load fail"`; then `Run()`, failure ->
   `"decrypt run fail"`; then `GetDataBuffer(&data_buf, &data_len)`.
   Exception -> `"decrypt Exception"`.
6. If `data_buf == 0 || data_len == 0`: bill with the error message type 0x20,
   record `code_id_`, `remaining`, return -1.
7. `check_decrypt_buf(user, data_buf, data_len, out, out_len, dynamic_head.crc_,
   0x20, is_internal_pkg)`.

VM slots: `Load` +0x2c, `Run` +0x28, `GetDataBuffer` +0x18.

### `int fix_decrypt(...)` — ORIG 0x000be0a0..0x000be26f

1. `DPFixAlgorithmHead::unpack(buf, buf_len, &used, 0)`.
   If error or `buf_len <= used`: log priority 4
   `"[%u][%s], DPFixAlgorithmHead unpack fail, ret=%d, buf_len=%d"`;
   return -1.
2. `remaining = buf_len - (unsigned short)used`; `dp_data = buf + used`.
3. `Cipher(algorithm_type_, dp_data, remaining, key_id_, false)`.
   Zero or exception: bill message `"cipher decrypt fail"` or
   `"cipher decrypt exception"`, type 0x1e; record `algorithm_type_`,
   `key_id_`, `remaining`; return -1.
4. `check_decrypt_buf(user, dp_data, remaining, out, out_len, fix_data.crc,
   0x1e, is_internal_pkg)`.

### `int decrypt(..., int pkg_type, int is_internal_pkg)` — ORIG 0x000be270..0x000be35f

- `pkg_type == 0x10`: `fix_decrypt`; if result 0 and user:
  `internal_fix_decrypt_count_++` for internal packages, otherwise
  `game_fix_decrypt_count_++`; return the inner result.
- `user && pkg_type == 0x08`: `dynamicy_decrypt`; if result 0 and user:
  `internal_dynamic_decrypt_count_++` or `game_dynamic_decrypt_count_++`;
  return the inner result.
- Otherwise return -1.

### `int dynamic_encrypt(...)` — ORIG 0x000be360..0x000be81f

Guards, in order:
1. null user -> -1.
2. `client_state_ <= 3 || client_state_ == 6` -> -1.
3. `server_cur_code_ == 0` -> -1.
4. if `explicit_use_dynamic_encrypt` and
   `server_cur_code_ != server_new_code_` -> -1.
5. `out_len < buf_len + 0x116` (278 = 5 + 8 + 265): log priority 4
   `"[%u][%s], Too small outsize, buf_len=%u out_len=%u"`; return -1.

Then:
- `init_encrypt_data(user, algorithm_data, last_encrypt_sequence_ + 1)`.
- `algorithm_data.pack(send_buf, 0xffff, &used, 0)`.
- `memcpy(send_buf + used, buf, buf_len); used += buf_len`.
- Log `pad_len_`, `custom_data_`.
- `get_server_code(user, server_cur_code_, server_code_len)` and log
  `code_buf`, `len`, `server_cur_code_`.
- If code missing/zero length -> `"get server code fail"`.
- Else `encrypt_vm_->Load(server_code, server_code_len, send_buf, used)`;
  failure -> `"encrypt load fail"`; `Run()` failure -> `"encrypt run fail"`;
  `GetDataBuffer(&data_buf, &data_len)`; success sets error string to `""`.
  Exception -> `"encrypt run exception"`.
- If `data_buf == 0 || data_len == 0 || data_len + 13 > out_len`:
  bill type 0x1f, record `server_cur_code_`, `used`, `data_len`, `out_len`;
  return -1.
- Build `DPInterData` (13-byte prefix):
  - `packet_size_ = (u16)(data_len + 13)`;
  - `protocol_type_ = 2`, `data_type_ = 0`;
  - `code_id_ = server_cur_code_`;
  - `crc_ = Crc32(send_buf, used)`.
- `DPInterData::pack(out, out_len, &used, 0)`; after this `used == 13`.
- `memcpy(out + used, data_buf, data_len)`; `out_len = used + data_len`.
- Log `packet_size_`, `crc_`, `code_id_`, `out_len`;
  `last_encrypt_sequence_ += 1`; return 0.

### `int encrypt(..., bool use_fix_encrypt)` — ORIG 0x000be820..0x000bea4f

1. `real_buf_len = out + 4` (the 4-byte length prefix area).
2. `out_len -= 4` unconditionally.
3. `max = glb_dp_config_->dp_proto_conf.dp_max_encrypt_size`;
   `chunk_len = buf_len`, `data_left_size = 0`.
   If `max < buf_len`: `data_left_size = buf_len - max`, `chunk_len = max`, and
   log priority 3/2 `"[%u][%s], dp_max_encrypt_size=%d, data_left_size=%d"`.
4. `temp_out_len = out_len`.
5. If `!use_fix_encrypt` and `dynamic_encrypt(user, buf, chunk_len,
   real_buf_len, temp_out_len) == 0`: `dynamic_encrypt_count_++`
   (no null check; success implies user).
   Otherwise call `fix_encrypt(user, buf, chunk_len, real_buf_len,
   temp_out_len)`; if non-zero return it; if user, `fix_encrypt_count_++`.
6. If `data_left_size`:
   - if `out_len < temp_out_len + data_left_size`: log priority 4
     `"[%u][%s], encrypt data left=%d  temp_out_len=%d out_len=%d"`;
     return -1;
   - `memcpy(real_buf_len + temp_out_len, buf + chunk_len, data_left_size)`;
     `temp_out_len += data_left_size`.
7. Log the same data-left format at priority 3/2.
8. `out_len = temp_out_len + 4`;
   `AB_NS_CODEENGINE::EncodeInt((char**)&out, out_len)`; return 0.

`EncodeInt` writes the 4-byte encoded length at the start of `out`; the caller's
`out_len` includes that prefix.

## Error codes / billing types observed

`DPMsgPacker::send_error_report_msg(UserInfo*, int, int)`:
- `0x1a1`, flag 1: CRC mismatch.
- `0x1a0`, flag 1: `client_user_id_` vs decrypted `usr_id_crc_` mismatch.
- `0x19f`, flag 1: sequence check failure when `use_sequence_check`.
- `0x193`, flag 1: dynamic code id not present in `client_code_[5]`.

`DPBillAdapter` type values:
- 0x1d: `fix_encrypt` Cipher failure/exception.
- 0x1e: `fix_decrypt` Cipher failure/exception and `check_decrypt_buf`
  calls originating from fix decrypt.
- 0x1f: `dynamic_encrypt` failures (no server code, VM failure, output too
  small).
- 0x20: `dynamicy_decrypt` failures and `check_decrypt_buf` calls originating
  from dynamic decrypt.

Bill records are `CommBillRecord::operator<<(long long)`. The argument orders
match the original exactly (e.g. `fix_encrypt`: algorithm type, key id,
payload length; CRC mismatch: expected CRC, computed CRC, input length;
sequence error: internal flag, previous sequence, new sequence).

## External boundaries / call targets

- `tss_dpsdk::DPCodeMgr::init()` and `reload()` (ORIG 0x000a7a70,
  0x000a79f0) — not reconstructed.
- `GetVM()` (0x00196ea0) and `GetUPVM()` (0x00195120) — global factories.
- `CVMBase` subclass virtual methods — only dispatch, no VM/crypto internals.
- `Crc32(unsigned char*, unsigned int)` (0x0019dc80).
- `Cipher(int, unsigned char*, unsigned int, unsigned int, bool)`
  (0x001934c0); non-zero means success, zero means failure in all paths.
- `sec_proto::DPAlgorithmData::pack/unpack`,
  `DPFixAlgorithmHead::unpack`, `DPDynamicAlgorithmHead::unpack`,
  `DPInterData::pack` — external TDR implementations.
- `DPBillAdapter` ctor/dtor and `CommBillRecord::operator<<(long long)`.
- `DPMsgPacker::send_error_report_msg`.
- `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, const char*, ...)` and
  `debug_errorex(const char*, ...)`.
- `ZEN_Thread_Mutex` ctor/dtor.
- Globals: `tss_dp_comm::DP_CONFIG::glb_dp_config_` (0x002bb4c0, 4-byte
  pointer), `tss_dpsdk::default_user_id_crc` (0x002c1c80),
  `tss_dpsdk::DPBillAdapter::bill_` (0x002b6e00).
- `AB_NS_CODEENGINE::EncodeInt(char**, unsigned int)`.
- libc/C++ runtime: `rand`, `memcpy`, `operator new`, `__cxa_*`,
  `_Unwind_Resume`, `__gxx_personality_v0`.

## Compile result

Primary c5root GCC 4.1.2 (same generation as the target, which is 4.1.0):

```text
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
$C5/usr/bin/g++ -m32 -O2 -DNDEBUG -pthread -fPIC -fno-implement-inlines \
  -fno-stack-protector -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include \
  -I recon -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_vmbase_engine_agent.cpp \
  -o build/tmp/dp_vmbase_engine_agent.o
```

Result: `C5_COMPILE_EXIT:0`, no diagnostics; object total 0x2570 bytes
(`.text` 0x256c, `.data` 4).
All static layout checks compiled. Every non-runtime undefined symbol in the
object exists in `libtss_sdk.so.1.0.394`; every defined `DPVMBaseEngine`
symbol emitted by the object exists in the original (C1/C2, D1/D2, all 12
non-lifecycle methods, and the six `__func__` strings). Only expected libc/libstdc++/EH
runtime symbols remain unresolved.

## Behavior not proven / deliberate limits

- No live oracle/differential execution was performed. Acceptance is based on
  the disassembly/DWARF semantics above.
- VM/crypto internals (`CVMBase` subclasses, `Load`, `Run`, `GetDataBuffer`,
  `GetDecodeCode`, `GetEncodeCode`, `Cipher`) are treated as black boxes with
  exact vtable slots and signatures; their success/failure behavior is inferred
  from the caller's zero/non-zero convention.
- `DPCodeMgr::init()`/`reload()` bodies and the `DpCodeInfos` shared-memory
  producer are not reconstructed; only the consumer layout and selection
  logic are.
- `DPInterDataBody`'s other union alternatives are represented as raw bytes;
  only the fix and dynamic algorithm heads are modeled.
- `DPBillRecorder` is modeled as deriving from the real global
  `CommBillRecord` solely to reach `operator<<(long long)`. Its full original
  members/vtable are not reproduced.
- `rand()` call sites and arithmetic are reproduced, but exact values depend
  on the process-global RNG state.
- Stack frames, local variable ordering and exception landing-pad layout in
  the reconstruction differ from the original; they are not observable
  through the reconstructed interfaces.
- The code-id remap behavior (1..99 and 101..199 accepted; 100/0/>=200
  rejected) is an observed original quirk, not a redesigned boundary.
