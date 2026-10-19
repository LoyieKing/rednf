# DPMsgHandler buffer/receive methods — reconstruction report

Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
ELF32 i386, unstripped, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
This CU's DWARF producer is `GNU C++ 4.1.0 (SUSE Linux)`; the target was built
`-O2 -fPIC`.

Source: `dp_msg_handler_buffer_agent.cpp` (this directory).

Evidence provenance: no dedicated function report for these three symbols
exists in the workbench (`docs/class_func_reports` is not present here); the
evidence is the ORIG `objdump -d -Mintel` slices plus DWARF
(`llvm-dwarfdump`) for signatures, line numbers, sizes and member offsets.
The existing `AGENT_REPORT.md` in this directory (DPInterface buffer work)
was read for the DWARF-verified `DPInterDataHead` layout
(`{ u16 packet_size_; u16 protocol_type_; u8 data_type_; }`, packed, size 5).
The generated `dp_msg_handler.cpp` stub was also read for line/local names.

No shared header, README, CMake, test or other agent's file was modified.
No commit was made. The generated `dp_msg_handler.h`, `dp_vmbase_engine.h`,
`dp_bill_adapter.h` and `dp_msg_packer.h` are empty DWARF stubs, so this file
uses local partial ABI declarations (marked INTEGRATION BOUNDARY below).

## 1. Symbols and address ranges

| method | mangled name | DWARF low/high | following symbol |
|---|---|---|---|
| `recv_buffer_for_encrypt` | `_ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rj` | `0x000ae1b0` / `0x000ae200` | `recv_buffer_for_decrypt` at `0x000ae200` |
| `recv_buffer_for_decrypt` | `_ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rjii` | `0x000ae200` / `0x000ae289` | `reload` at `0x000ae290`; `0xae289..0xae28f` is padding |
| `OnRecvPkgFromClient` | `_ZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhj` | `0x000b0fc0` / `0x000b1470` | `__static_initialization_and_destruction_0ii` at `0x000b1470` |

The ticket range end `0x000b1860` is the end of the **following**
static-initialization function (`0x000b1470 .. 0x000b185f`), not of
`OnRecvPkgFromClient`. The receive method itself ends at `0x000b1470`
(last instructions are the two `_Unwind_Resume` cleanup pads at
`0x000b1440..0x000b146b`).

No identity claim is made for the whole shared object. Compilation with the
closest available toolchain (GCC 4.1.2) is reported in §7; byte identity is
not claimed (the original CU was GCC 4.1.0).

## 2. Signatures (DWARF + mangled names)

```cpp
int tss_dpsdk::DPMsgHandler::recv_buffer_for_encrypt(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int buf_len,
    unsigned char *out, unsigned int &out_len);

int tss_dpsdk::DPMsgHandler::recv_buffer_for_decrypt(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int buf_len,
    unsigned char *out, unsigned int &out_len, int pkg_type,
    int is_internal_pkg);

int tss_dpsdk::DPMsgHandler::OnRecvPkgFromClient(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int buf_len);
```

DWARF class layout: `DPMsgHandler` is 4 bytes with private
`DPVMBaseEngine *vm_engine_` at `+0x00`. The definitions load
`[this+0]` and pass it as the engine `this` (`0x0ae1f0`, `0x0ae24e`,
`0x000b0fc?` prologue has no engine use). `DPVMBaseEngine` is 0x44 bytes.

`out_len` is an `unsigned int&` (`Rj` in the mangled name and in the class
declaration DIE). The definition DIE prints `const unsigned int &`, but the
declaration and mangled symbol are non-const; the source uses the
non-const declaration. The wrappers do not write `out_len` themselves; they
pass the reference to the engine, which owns the write.

## 3. Byte-width and wire-layout evidence

### DPMsgHandler / bill recorder

- `DPMsgHandler::vm_engine_` +0x00, size 4 (DWARF).
- `DPBillAdapter` size 4, one member `user_ +0x00` (DWARF). The local bill
  objects at `[ebp-0x14]` are exactly 4 bytes; ctor/dtor calls are
  `_ZN9tss_dpsdk13DPBillAdapterC1EPKcjPN12tss_sdk_impl8UserInfoE` /
  `_ZN9tss_dpsdk13DPBillAdapterD1Ev`.
- `DPBillAdapter::bill_` is `_ZN9tss_dpsdk13DPBillAdapter5bill_E`
  (0x002b6e00), type `DPBillRecorder`, which derives from `CommBillRecord`.
  The inlined `operator<<` template expands to
  `CommBillRecord::operator<<(long long)` (`_ZN14CommBillRecordlsEx`,
  0x000d1590). The call site pushes an 8-byte argument: low dword = value,
  high dword = 0 (`mov DWORD PTR [esp+0x8],0x0`). This is why the bill
  statements compile to a `long long` conversion, not an `int` overload.

### DPInterData head/body

DWARF: `sec_proto::DPInterData` has `byte_size 0x631` (1585), with
`head` at `+0x00` and `body` at `+0x05`; `DPInterDataHead` is a packed
5-byte structure; `DPInterDataBody` is a structure containing a union of
size `0x62c` (1580).

| field | offset | width | ORIG access evidence |
|---|---|---|---|
| `head.packet_size_` | `+0x00` | `unsigned short` | `0x000b102a` `movzx ecx,WORD PTR [ebp-0x645]`; log arg 7 store at `0x000b1079` |
| `head.protocol_type_` | `+0x02` | `unsigned short` | `0x000b103a` / `0x000b10c8` `movzx ...,WORD PTR [ebp-0x643]`; switch input |
| `head.data_type_` | `+0x04` | `unsigned char` | `0x000b1031` / `0x000b10f4` `movzx ...,BYTE PTR [ebp-0x641]`; log arg 6 |
| `body` | `+0x05` | union, 1580 bytes | payload base `[ebp-0x640]` at `0x000b125a` etc. |

The reconstructed local object uses the same frame offsets:
`pkg` at `[ebp-0x645]`, `head.protocol_type_` at `[ebp-0x643]`,
`head.data_type_` at `[ebp-0x641]`, `body` at `[ebp-0x640]`. The frame
size is `sub esp,0x67c`, identical to the original, which independently
confirms the 5+1580-byte object plus small locals.

`DPInterDataBody` union member sizes used by this TU (DWARF):

| member | size |
|---|---|
| `request_code_` (`DPProtocolRequestCode`) | 0x0c |
| `init_data_ptc_` (`DPClientInitDataPtc`) | 0x24 |
| `client_stat_ptc_` (`DPClientStatPtc`) | 0x62c |
| `decrypt_info_` (`DPClientDecryptInfo`) | 0x08 |
| `client_log_ptc_` (`DPClientLogPtc`) | 0x86 |
| `cs_test_data_` / `sc_test_data_` | `uint8_t[10]` each |

All union members are at offset 0 of the body. `cs_test_data_` and
`sc_test_data_` alias the same 10 bytes; the assembly cannot distinguish
which name the original source used. The reconstruction uses
`cs_test_data_` because command 14 is a client-to-server test packet.

`size_t used` is at `[ebp-0x10]`, initialized to 0 at `0x000b0fea`.
`DPInterData::unpack(const char*, size_t, size_t*, unsigned int)` is called
with `&used` and `0` (`0x000b0ff1..0x000b1017`).

### Numeric values

- Command/type dispatch values are raw `unsigned short` 9..14; no enum names
  were recovered from DWARF or strings.
- Error/report codes are raw `int` values: `0x192` (decrypt failure),
  `0x1a3` (unpack failure), `0x19e` (test-data mismatch),
  `0x1a4` (unknown internal message). No enum name is claimed.
- `DPBillAdapter` constructor second argument is `unsigned int`; the used
  values are 33 (`"dp pkg decode failed"`) and 26 (`"check test failed"`).
  These are bill type IDs, not string lengths (the strings are 20/17
  characters).
- Log priority is `ZEN_LOG_PRIORITY`: 4 (`RS_ERROR`) for the unknown branch;
  for the success branch 3 (`RS_INFO`) when `user != 0 && (flags & 1)`,
  otherwise 2 (`RS_DEBUG`).

`UserInfo` offsets used: `base_info_.uin_ +0x00`, `base_info_.flags_ +0x20`
(see `user_info_recon.h`, checked against `test BYTE PTR [user+0x20],1`
at `0x000b0fdd`).

## 4. Decoded control flow

### 4.1 `recv_buffer_for_encrypt` — 0x000ae1b0..0x000ae200

```
0xae1b0  push ebp; mov ebp,esp; push ebx; sub esp,0x24
0xae1b7  load out_len& into arg 6 slot
0xae1c5  [esp+0x18] = 0                     ; bool is_internal = false
0xae1cd  [esp+0x14] = out_len&
0xae1d1  [esp+0x10] = out
0xae1d8  [esp+0x0c] = buf_len
0xae1df  [esp+0x08] = buf
0xae1e6  [esp+0x04] = user
0xae1ed  eax = [this]                       ; vm_engine_
0xae1f2  [esp] = eax
0xae1f5  call DPVMBaseEngine::encrypt       ; PLT 0x866e4 -> 0x000be820
0xae1fa  epilogue; ret eax
```

Source: one tail expression, `return vm_engine_->encrypt(user, buf, buf_len,
out, out_len);`. The sixth engine argument is the default `bool = false`;
the original stack slot is zeroed at `0xae1c5`.

### 4.2 `recv_buffer_for_decrypt` — 0x000ae200..0x000ae289

```
0xae200  prologue; sub esp,0x38; save edi/ebx
0xae206  load is_internal_pkg (arg 7 -> [esp+0x1c])
0xae21d/0xae221  load pkg_type (arg 6 -> [esp+0x18])
0xae22f..0xae24b  load out_len&, out, buf_len, buf, user
0xae24b  eax = [this] -> vm_engine_
0xae253  call DPVMBaseEngine::decrypt       ; PLT 0x85ae4 -> 0x000be270
0xae258  test ret
0xae25e  test user
0xae262  [esp+0x08] = 1
0xae26a  [esp+0x04] = 0x192
0xae272  [esp] = user
0xae275  call send_error_report_msg        ; PLT 0x7ea14 -> 0x000b2920
0xae27a  eax = ret; restore; ret
```

Source (DWARF lines 52..61):

```cpp
int ret = vm_engine_->decrypt(user, buf, buf_len, out, out_len, pkg_type,
                              is_internal_pkg);
if (ret != 0 && user != 0)
    DPMsgPacker::send_error_report_msg(user, 0x192, 1);
return ret;
```

The error report is sent only when the engine failed and `user != 0`.
`out_len` is only forwarded; the wrapper never writes it.

### 4.3 `OnRecvPkgFromClient` — 0x000b0fc0..0x000b1470

Source line map from `.debug_line`: 770 entry, 772 flags gate, 774 dump,
777 `used`, 778 `pkg`, 779 unpack/ret, 780 failure test, 782..789 failure
bill path, 792 success log, 795 switch, 799/805/811/817/823/829 cases,
832..842 test-data failure, 850 default log, 853 default report,
858 return.

#### Entry and bill dump (0x000b0fcc..0x000b0fe1, 0x000b1236..0x000b1255)

```
0xb0fcc  eax = user
0xb0fdd  test BYTE PTR [eax+0x20],1
0xb0fe1  jne 0xb1236
...
0xb1236  edx = buf_len; [esp+8] = buf; [esp+0xc] = buf_len
0xb1241  eax = [user]                     ; uin
0xb1243  [esp+4] = uin
0xb1247  eax = "OnRecvPkgFromClient"      ; .rodata 0x21bf0b
0xb124d  [esp] = eax
0xb1250  call DPBillAdapter::dump         ; PLT 0x85504 -> 0x000a2a70
0xb1255  jmp 0xb0fe7
```

There is **no null check** before `test [user+0x20]`; `user` is dereferenced
unconditionally at entry, exactly as in the original. Later log/uin
expressions do contain null checks, because the source writes
`user != 0 ? user->base_info_.uin_ : 0u`.

#### Unpack (0x000b0fe7..0x000b101c)

```
used = 0                                   ; [ebp-0x10] = 0
pkg.unpack((const char*)buf, buf_len, &used, 0)
    ; [esp] = &pkg [ebp-0x645], [esp+4] = buf, [esp+8] = buf_len,
    ; [esp+0xc] = &used, [esp+0x10] = 0
    ; PLT 0x7d964 -> sec_proto::DPInterData::unpack 0x001619e0
test eax,eax; jne 0xb1185                  ; unpack failure
```

The original does not call `DPInterDataHead::unpack` or
`DPInterDataBody::unpack` separately here. It calls the aggregate
`sec_proto::DPInterData::unpack`, whose object is the 5-byte head followed
by the 1580-byte body union. The head fields are read after that call, and
the body union member is selected by `head.protocol_type_`.

#### Unpack-failure path (0x000b1185..0x000b1235, lines 782..789)

```
DPBillAdapter bill("dp pkg decode failed", 33, user)
    ; PLT 0x7f7b4 -> 0xa27d0
bill << buf_len;                            ; CommBillRecord::operator<<(long long)
for (i = 0; i < buf_len && i < 12; ++i)     ; 0xb11d0..0xb11f9
    bill << buf[i];
DPMsgPacker::send_error_report_msg(user, 0x1a3, 1) ; PLT 0x7ea14
bill.~DPBillAdapter();                      ; PLT 0x82fe4
return -1;
```

There is no `debug_output` call in this failure path; only the bill record
(including the first up-to-12 raw bytes) and the error report. The loop
bound is 12 (`0xb11f6 cmp esi,0xc`), and it also stops at `buf_len`
(`0xb11d0 cmp esi,[ebp+0x14]; je`). For `buf_len = 0` the loop body never
runs.

#### Success log (0x000b1024..0x000b10c3, lines 792..795)

The log is unconditional. Priority and uin are:

```
user == 0                 -> priority 2, uin 0
user != 0, flags & 1 == 1 -> priority 3, user->uin
user != 0, flags & 1 == 0 -> priority 2, user->uin
```

Format string at `.rodata 0x21c2f4`:

```
[%u][%s], buf_len=%u, used=%u, protocol_type_=%u, data_type_=%u, packet_size_=%u
```

Argument order: `uin, __func__, buf_len, used, protocol_type_,
data_type_, packet_size_`. The three head fields are zero-extended
(`movzx`) and passed as `unsigned int`. `__func__` is the local static
string `"OnRecvPkgFromClient"` at `0x21c3e4`, emitted by the compiler as
`_ZZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhjE8__func__`.
Call PLT `0x873b4 -> ZEN_Trace_LogMsg::debug_output 0x001b3e70`.

#### Dispatch (0x000b10c8..0x000b1183)

```
edx = head.protocol_type_                  ; WORD [ebp-0x643]
eax = edx - 9
cmp ax,5; jbe jump_table                   ; values 9..14 only
jump table at .rodata 0x21c3b0              ; 6 relative entries
default: 0xb10dc
```

Cases and call targets (source order in the original: 9, 10, 13, 11, 12,
14; the compiler's physical body order is 9, 10, 11, 12, 13, 14):

| value | body | call target | packet type |
|---|---|---|---|
| 9 | `0xb125a` | `OnRecvMsgFromClient(UserInfo*, DPProtocolRequestCode&)` PLT `0x7c3b4` | 12-byte body |
| 10 | `0xb127d` | `OnRecvMsgFromClient(..., DPClientInitDataPtc&)` PLT `0x821e4` | 36-byte body |
| 11 | `0xb12a0` | `OnRecvMsgFromClient(..., DPClientStatPtc&)` PLT `0x7e744` | 1580-byte body |
| 12 | `0xb12c3` | `OnRecvMsgFromClient(..., DPClientDecryptInfo&)` PLT `0x7e3c4` | 8-byte body |
| 13 | `0xb12e6` | `OnRecvMsgFromClient(..., DPClientLogPtc&)` PLT `0x7e474` | 134-byte body |
| 14 | `0xb1309` | no packet handler: 10-byte test-data comparison | `cs_test_data_[10]` at body +0 |

Every case 9..13 body is:

```
lea eax,[ebp-0x640]        ; &pkg.body
mov [esp+8],eax
mov eax,[ebp+0xc]          ; user
mov [esp+4],eax
mov edx,[ebp+8]            ; this
mov [esp],edx
call OnRecvMsgFromClient
mov ecx,eax                ; ret
jmp shared_return          ; 0xb116a
```

Case 14 (`0xb1309..0xb143b`):

```
repz cmps 10 bytes at pkg.body.cs_test_data_ vs "0123456789" (.rodata 0x21c3da)
if equal -> ret = 0; jmp shared_return
else:
  DPBillAdapter bill("check test failed", 26, user)   ; PLT 0x7f7b4
  bill << buf_len
  bill << head.protocol_type_      ; unsigned short, zero-extended
  bill << head.data_type_          ; unsigned char, zero-extended
  bill << head.packet_size_        ; unsigned short, zero-extended
  for (i = 0; i < 10; ++i)
      bill << pkg.body.cs_test_data_[i];
  DPMsgPacker::send_error_report_msg(user, 0x19e, 1)
  bill.~DPBillAdapter()
  return -1
```

The comparison is `memcmp(..., 10) == 0`; any mismatch takes the bill/error
path. The `"0123456789"` literal is at `.rodata 0x21c3da` (note it shares
storage with the `__func__` string; the 10-byte compare starts exactly at
the digit sequence).

#### Default branch (0xb10dc..0xb1176, lines 850..858)

```
debug_output(RS_ERROR,
  "[%u][%s], unknown interanl msg,buf_len=%u, used=%u, protocol_type_=%u, data_type_=%u, packet_size_=%u",
  user ? user->uin : 0, __func__, buf_len, used,
  protocol_type_, data_type_, packet_size_);
send_error_report_msg(user, 0x1a4, 1);
return -1;
```

The original string contains the typo `interanl` (not `internal`); it is
at `.rodata 0x21c348` and is reproduced byte-for-byte.

#### Shared return (0xb116a / 0xb1224 / 0xb122a)

`ret` holds the `OnRecvMsgFromClient` result for cases 9..13, 0 for a
successful test-data comparison, and -1 for the three explicit failure
paths. The epilogue restores `esp`, `ebx`, `esi`, `edi` and returns `ret`.

## 5. Source-to-address mapping summary

| ORIG range | behavior | reconstructed source |
|---|---|---|
| `0xae1b0..0xae200` | encrypt wrapper | `recv_buffer_for_encrypt` |
| `0xae200..0xae289` | decrypt wrapper + failure report | `recv_buffer_for_decrypt` |
| `0xb0fcc..0xb0fe1`, `0xb1236..0xb1255` | flags gate + bill dump | `if ((user->base_info_.flags_ & 1) != 0) DPBillAdapter::dump(...)` |
| `0xb0fe7..0xb101c` | `used=0`; aggregate unpack | `size_t used = 0; DPInterData pkg; ret = pkg.unpack(...)` |
| `0xb1185..0xb1235` | failure bill + 12-byte loop + report | failure `if (ret != 0)` block |
| `0xb1024..0xb10c3` | success diagnostic log | `ZEN_Trace_LogMsg::debug_output(...)` |
| `0xb10c8..0xb1183` | switch 9..14 | `switch (pkg.head.protocol_type_)` |
| `0xb125a..0xb1304` | five `OnRecvMsgFromClient` calls | `case 9/10/13/11/12` |
| `0xb1309..0xb143b` | test-data compare/bill | `case 14` |
| `0xb10dc..0xb1176` | unknown-message log/report | `default` |
| `0xb116a..0xb146b` | shared return + EH pads | `return ret;` |

## 6. Helper/global dependencies (all extern)

| dependency | ORIG address | call site |
|---|---|---|
| `tss_dpsdk::DPVMBaseEngine::encrypt(UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&, bool)` | `0x000be820` | `0xae1f5` (PLT `0x866e4`) |
| `tss_dpsdk::DPVMBaseEngine::decrypt(UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&, int, int)` | `0x000be270` | `0xae253` (PLT `0x85ae4`) |
| `tss_dpsdk::DPMsgPacker::send_error_report_msg(UserInfo*, int, int)` | `0x000b2920` | `0xae275`, `0xb1160`, `0xb1211`, `0xb1423` (PLT `0x7ea14`) |
| `tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int, unsigned char const*, unsigned int)` | `0x000a2a70` | `0xb1250` (PLT `0x85504`) |
| `tss_dpsdk::DPBillAdapter::DPBillAdapter(char const*, unsigned int, UserInfo*)` | `0x000a27d0` (C1), `0x000a2730` (C2) | `0xb11aa`, `0xb1352` (PLT `0x7f7b4`) |
| `tss_dpsdk::DPBillAdapter::~DPBillAdapter()` | `0x000a26c0` (D1) | `0xb121f`, `0xb1431`, `0xb144b`, `0xb1463` (PLT `0x82fe4`) |
| `tss_dpsdk::DPBillAdapter::bill_` (`DPBillRecorder`, `0x002b6e00`) | object | `[ebx-0x9c]` GOT slot `0x00294498` |
| `tss_dpsdk::CommBillRecord::operator<<(long long)` | `0x000d1590` | `0xb11c7`, `0xb11ee`, `0xb136f`, `0xb1390`, `0xb13b1`, `0xb13d2`, `0xb1400` (PLT `0x836d4`) |
| `sec_proto::DPInterData::unpack(char const*, unsigned int, unsigned int*, unsigned int)` | `0x001619e0` | `0xb1017` (PLT `0x7d964`) |
| `ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)` | `0x001b3e70` | `0xb10c3`, `0xb1145` (PLT `0x873b4`) |
| `DPMsgHandler::OnRecvMsgFromClient` overloads | `0xaf180`, `0xaf2d0`, `0xaf410`, `0xaf5b0`, `0xb0b50` | case bodies (PLT `0x7e3c4`, `0x7e474`, `0x7e744`, `0x7c3b4`, `0x821e4`) |

The `DPVMBaseEngine` methods are non-static member functions; the first
stack argument is `[this]` (`vm_engine_`). `send_error_report_msg` and
`DPBillAdapter::dump` are static (no `this` stack argument; the first
argument is `user`/`tag`).

## 7. Compile and symbol evidence

Primary compile (closest generation available):

```
sh /home/loyieking/dnf_workspace/dnf_secsvr/toolchain/c5xx.sh \
   -fno-stack-protector -I recon \
   -c recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_buffer_agent.cpp \
   -o /tmp/dp_msg_handler_buffer_agent.o
```

`c5xx.sh` invokes `g++ (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)` with
`-m32 -O2 -DNDEBUG -pthread -fPIC -fno-implement-inlines -std=gnu++98
-nostdinc` plus the c5root 4.1.1/4.1.2 C++ and system include paths.

Result: **compiled successfully, 0 errors / 0 warnings**.

The object defines exactly the three intended strong symbols:

```text
T _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rj
T _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rjii
T _ZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhj
```

plus the local read-only `__func__` object
`_ZZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhjE8__func__`.
The other defined symbols are compiler artifacts: local `.LC0..LC5` rodata
labels, `DW.ref.__gxx_personality_v0`, and the hidden COMDAT group section
`.text.__i686.get_pc_thunk.bx` (`__i686.get_pc_thunk.bx`), which the linker
merges with the copies emitted by other TUs.
Undefined externals are exactly the dependencies in §6 plus
`__gxx_personality_v0`, `_Unwind_Resume` and `_GLOBAL_OFFSET_TABLE_`.

A second syntax check with the ticket's generic command also succeeded:

```
g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines \
    -fno-stack-protector -I recon -c ... -o /tmp/dp_msg_handler_buffer_agent_wsl.o
```

Structural comparison with the ORIG slice (not an identity claim):

- The two wrapper bodies reproduce the ORIG instruction sequence and sizes
  modulo relocation immediates (`encrypt` 0x50 bytes, `decrypt` 0x89 bytes;
  same prologue, same argument slots, same call/PLT targets, same return
  path).
- `OnRecvPkgFromClient` is semantically aligned but not byte-identical.
  Observed codegen-only differences between ORIG GCC 4.1.0 and c5root GCC
  4.1.2:
  1. ORIG keeps `test user,user` checks in the log/default-uin blocks;
     4.1.2 elides them after the unconditional entry dereference of
     `user->base_info_.flags_`.
  2. ORIG lowers `memcmp(...,10) == 0` to `repz cmps` + `seta/setb/cmp`;
     4.1.2 collapses the equality-only use to `repz cmps` + `je`.
  3. ORIG's PIC switch table is an `add ebx`-relative table in `.rodata`;
     4.1.2 emits a `sub`-relative table in `.text`.
  4. ORIG keeps the shared `ret` in `%ecx`; 4.1.2 spills it to a stack slot,
     which shifts two unrelated spill slots.
  These are compiler-generation differences; the reconstructed source keeps
  the original observable behavior, call targets, field offsets/wire layout,
  guards, return values and error paths. No byte-identity is claimed.

## 8. INTEGRATION BOUNDARY / unresolved items

The following are deliberately partial and must be replaced by the real SDK
headers when this TU is wired into a full build:

1. `tss_dpsdk::DPMsgHandler` is declared with only `vm_engine_` and the
   methods used here. The real class has more methods/members; size and
   `vm_engine_` offset are exact.
2. `tss_dpsdk::DPVMBaseEngine` declares only `encrypt`/`decrypt`; the real
   class is 0x44 bytes with the members shown by DWARF.
3. `tss_dpsdk::DPBillAdapter` / `DPBillRecorder` / `CommBillRecord` are
   partial models. `DPBillRecorder` is declared as deriving from
   `CommBillRecord` solely so the inlined `operator<<` selects
   `CommBillRecord::operator<<(long long)`. No recorder body or vtable is
   defined here.
4. `sec_proto::DPProtocolRequestCode`, `DPClientInitDataPtc`,
   `DPClientStatPtc`, `DPClientDecryptInfo`, `DPClientLogPtc` are opaque
   byte arrays with exact DWARF sizes. Their packet fields are **not**
   reconstructed, and no wire layout beyond size/offset is claimed.
5. `sec_proto::DPInterData::unpack` is declared returning `int`; the real
   DWARF return type is `tsf4g_tdr::TdrError::ErrorType` (an int-sized enum).
   The return value is only tested for non-zero and stored in an `int`, so
   the call ABI is unchanged.
6. `ZEN_LOG_PRIORITY` values 1..6 are taken from the SDK trace-log enum
   layout; only 2/3/4 are used here.
7. The protocol type enum names for values 9..14 and the error/report code
   enum names for 0x192/0x1a3/0x19e/0x1a4 were not recoverable; only the
   raw numeric values and their call sites are asserted.
8. The original source member name for the 10-byte test payload is
   ambiguous between `cs_test_data_` and `sc_test_data_` (both are the same
   `uint8_t[10]` union arm at offset 0). `cs_test_data_` is used with a
   comment; this does not affect the emitted addresses.
9. The entry `user->base_info_.flags_` dereference is unconditional in the
   original; `user == 0` is undefined at entry. The later null checks in the
   log expressions are preserved from the original source even though they
   are unreachable after that dereference.

No behavior was invented: every call target, field offset, argument width,
guard, error code and return value above is tied to an ORIG address. Where
the original packet/helper implementations are unavailable, they remain
extern or opaque and are marked as such.
