# TssSdkUicPlugin reconstruction

Target: `libtss_sdk.so.1.0.394` (ELF32 i386, SHA-256
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`).
TU: `src/formmog/tss_sdk/plugins/uic/uic_plugin.cpp`.
Output: `recon/src/formmog/tss_sdk/plugins/uic_plugin_agent.cpp`.

## Address ranges

| Method | Address | End |
|---|---|---|
| `handle_chat_user_input_local(UserMessage*)` | `0x000c6fb0` | `0x000c70e3` |
| `handle_name_user_input(UserMessage*)` | `0x000c70f0` | `0x000c71c4` |
| ctor C1 / C2 | `0x000c71d0` / `0x000c7320` | `0x000c7276` / `0x000c73c6` |
| `get_obj_TssSdkUicPlugin()` | `0x000c7280` | `0x000c7318` |
| `proc(void*)` | `0x000c73d0` | `0x000c74e3` |
| `fini()` | `0x000c74f0` | `0x000c7556` |
| `init(void*)` | `0x000c7560` | `0x000c7659` |
| `handle_chat_user_input(UserMessage*)` | `0x000c7b00` | `0x000c7f06` |
| `notify(void*)` | `0x000c7f10` | `0x000c7fee` |
| dtor D0 / D1 | `0x000c7ff0` / `0x000c8070` | `0x000c806e` / `0x000c80ee` |

## Layouts

`TssSdkUicPlugin`, sizeof **76** (object emitted from the reconstruction:
76 bytes):

```
+0x00 ITssSdkPlugin (vptr, callback_proc_, error_info_, sdk_plugin_id_)
+0x10 UicUserInputInfoMgr user_input_mgr_   (32 bytes)
+0x30 ZEN_Thread_Mutex thread_mutex_        (28 bytes)
```

`UserMessage`, global, sizeof **40**:

```
+0x00 uin_                 +0x14 message_ (unsigned char*)
+0x04 input_type_          +0x18 callback_data_len_ (u16)
+0x08 msg_result_flag      +0x1c callback_data_ (unsigned char*)
+0x0c if_replace_ (char)   +0x20 game_data_len_ (u16)
+0x10 message_len_ (u32)   +0x24 game_data_ (unsigned char*)
```

Enums: `EUITYPE_CHAT=0, NAME=1, IMAGE=2, VIDEO=3`;
`EMSG_NORMAL_FLAG=0, EVIL=1, DIRTY=2, NOT_OPERATE=3`.
`UserInputCode`: GBK=0, UTF8=1.

`UicUserInputInfoMgr` = 32: `hashmap_buf +0`, `hashmap +4`, `mem_pool +8`,
`callback_proc +0xc`, `sequence +0x10`, `add_input_num +0x14`,
`recv_input_total_num +0x18`, `recv_input_valid_num +0x1c`.
`UicUserInputInfo` = 28: `uin 0`, `msg_result_flag 4`, `message_len 8`,
`message 12`, `callback_data_len 16`, `callback_data 20`, `input_type 24`.
`ZEN_Thread_Mutex` = 28; lock is vtable +0x08, unlock +0x10
(dummy virtual at +0x0c preserves the slot numbering).

`sec_proto::UserInputCheckPkg` = 1179 packed:

```
head: proto_id 0, version 2, pkg_len 4, cmd 6, game_id 8
body at 10: ReqUserInputCheck:
  uin 0, session_id 4, session_seq 8, user_input_code 12 (i8),
  user_input_data_len 13 (u16, unaligned), user_input_data[1024] 15,
  user_game_data_len 1039 (u16, unaligned), user_game_data[128] 1041
  (1169 bytes)
```

`Comm_App_Frame` = 50 packed, `SERVICES_ID` = 6 packed.

`g_config` (0x0029fac0, sizeof 1515 packed): `comm_config_[1226]`,
`log_config_ +1226`, `usercontrol_config_ +1230` (`use_idx_ +0x4ce`,
`max_client_obj_count_ +0x4d2`), `UicConfig uic_config_ +1238`.
`UicConfig` offsets: `use_backend_check_ +0x4d6`,
`user_input_code_ +0x4da`, `master ip +0x4db/port +0x501`,
`slave ip +0x505/port +0x529`, queue lengths +0x52b/+0x52f-...,
`use_dirty_file_flag_ +0x52b`, `dirty_file_ +0x52f`, `bus_id_ +0x5af`.

## Decoded behavior

- Ctor: base id=6, constructs `UicUserInputInfoMgr`, constructs
  `ZEN_Thread_Mutex(NULL)`, logs `"In CTssSdkUicPlugin ctor."`.
  Dtor destroys mutex, manager, then base; no extra logic.
- `handle_chat_user_input_local(msg)`:
  1. `!msg || !msg->message_` -> log
     `"TssSdkUicPlugin::handle_chat_user_input_local invalid args\n"`,
     return **-1**.
  2. `MsgResultFlag result = NORMAL`; `UicTdirty::instance()->
     uic_dirty_check(msg->message_, &result)`; failure logs
     `"[uic sdk]...uic_dirty_check failed"` and **continues**.
  3. If `result == DIRTY`, `uic_dirty_replace(msg->message_)`; failure logs
     `"[uic sdk]...uic_dirty_replace failed"` and continues.
  4. Copies `uin`, `result`, `message_len`, `message`, `callback_data_len`,
     `callback_data`, `input_type` into a 28-byte `UicUserInputInfo`.
  5. No callback -> logs `"...callback_proc_ == NULL"`, returns 0.
  6. `callback_proc_(&info)`; non-zero logs `"...callback_proc_ failed"`;
     always returns 0.
- `handle_name_user_input(msg)`:
  1. Invalid args -> same `"invalid args\n"` log, return -1.
  2. Dirty check failure -> log `"uic_dirty_check failed\n"`, return -1.
  3. Only if `result == DIRTY && msg->if_replace_ != 0` call
     `uic_dirty_replace`; failure logs and returns -1.
  4. `msg->msg_result_flag = result`; return 0. No callback.
- `init(init_data)` ignores `init_data`, locks the mutex, then:
  - dirty file = `use_dirty_file_flag_ >= 1` (unsigned) ? `dirty_file_` : NULL;
  - `UicTdirty::instance()->init(dirty_file, uic_config_.bus_id_,
    (UserInputCode)uic_config_.user_input_code_)`; failure logs
    `"[uic sdk]...UicTdirty::instance()->init failed"` -> result -1;
  - otherwise `user_input_mgr_.init(0x4b0, callback_proc_)`; failure logs
    `"[uic sdk]...user_input_mgr_.init failed"` -> result -1; success 0;
  - unlock, return result.
- `fini`: lock, `user_input_mgr_.fini()`, unlock, return 0.
- `proc(proc_data)`:
  1. Lock.
  2. If `uic_config_.use_backend_check_ == 0` (g_config+0x4d6), unlock and
     return 0.
  3. `user_input_mgr_.expire_user_input()`.
  4. If `proc_data != NULL`: `UserInputCheckPkg::unpack((char*)frame + 0x32,
     frame_length_ - 0x32, 0, 0)`. Failure logs
     `"[uic sdk]...check_result.unpack failed, ret = %d"`, unlock, return -1.
  5. `on_recv_check_result(&pkg)`; failure logs
     `"[uic sdk]...user_input_mgr_.on_recv_check_result failed, ret = %d"`,
     unlock, return -1.
  6. Unlock, return 0.
- `handle_chat_user_input(msg)`:
  1. Invalid args -> chat invalid-args log, return -1.
  2. If `user_input_mgr_.get_user_num() > 0x4af` (1199) **or**
     `use_backend_check_ == 0`, delegate to
     `handle_chat_user_input_local` and return its rc.
  3. `add_user_input(msg, &seq)`; failure logs
     `"[uic sdk]...add_user_input failed, uin = %u"` and delegates local.
  4. Builds `UserInputCheckPkg`: sets `head_.version_ = 1`,
     `head_.cmd_ = 1`; `body.req.uin_ = msg->uin_`, `session_id_ = 0`,
     `session_seq_ = seq`, `user_input_code_ = uic_config_.user_input_code_`.
     **ORIG leaves `proto_id_`, `pkg_len_` and `game_id_` uninitialized**;
     the reconstruction preserves this.
  5. Message copy: if last byte of `message_` is non-zero, data length =
     `(u16)(message_len + 1)`, `memcpy` `message_len` bytes, append NUL;
     otherwise data length = `(u16)message_len` and copy exactly.
     Game data: `user_game_data_len_ = game_data_len_`, memcpy that many.
  6. Builds a socket-style frame: `init_framehead(65536, 0, 0)`,
     rcv/proxy (0,0), send (0x2b,0), `frame_uin_ = 0`,
     transaction/backfill/app_id = 0.
  7. `appdata_encode<UserInputCheckPkg>(65469, pkg, 0, 0)`; on encode
     success calls `send_buf_to_backend(frame, frame_length_, false, 6)`.
     Transport failure logs `"[%s]send to backend fail. ret=%d"` with the
     `send_socket_pkg<UserInputCheckPkg>` pretty-function text. **Always
     returns 0**, including encode failure.
- `notify(notify_data)`:
  - null -> log `"[uic sdk]...notify invalid args"`, return -1;
  - `input_type_ == CHAT` -> lock, `handle_chat_user_input`, unlock, return rc;
  - `NAME` -> `handle_name_user_input`, return rc;
  - `IMAGE` / `VIDEO` -> return -1 without logging;
  - anything else (including negative) -> log
    `"[uic sdk]...notify unknown input type"`, return -1.
- `get_obj_*`: function-local static (guard/atexit), as shipped.

## Error/log strings

Exact strings are reproduced in the source, including the newline on the
`invalid args` and `uic_dirty_check failed` messages and the `[uic sdk]`
prefix. The UIC plugin never stores an error code in `error_info_`; all
failures are reported by return value and `ZEN_Trace_LogMsg::debug_errorex`.

## Call targets / external boundaries

`ITssSdkPlugin`, `UicUserInputInfoMgr::*`, `UicTdirty::{instance,init,
uic_dirty_check,uic_dirty_replace}`, `Singleton<TssSdkCommSvc>::instance()`,
`TssSdkCommSvc::send_buf_to_backend`,
`sec_proto::UserInputCheckPkg::{pack,unpack}`,
`Comm_App_Frame::{init_framehead,dumpoutput_framehead}`,
`SERVICES_ID::{ctor,dtor}`, `ZEN_Thread_Mutex::{ctor,dtor,lock,unlock}`,
`ZEN_Trace_LogMsg::debug_errorex`, and `g_config` (+0x4d6, +0x4da, +0x52b,
+0x52f, +0x5af).

Note: the shipped code invokes `lock`/`unlock` through the mutex vtable
(+0x08/+0x10). The reconstructed source calls the same virtual methods; the
c5root compiler devirtualizes the direct-member call to the public
`ZEN_Thread_Mutex::lock()/unlock()` symbols, which have identical observable
effect.

## Compile

c5root GCC 4.1.2, `-m32 -O2 -fPIC -fno-implement-inlines
-fno-stack-protector -std=gnu++98`, `-I recon`, `-nostdinc` with c5root 4.1.1
headers and `-B<c5root>/usr/libexec/gcc/x86_64-redhat-linux/4.1.1/`.
Result: **exit 0, no diagnostics**. `-fdump-class-hierarchy` confirms
`TssSdkUicPlugin` 76, `UserMessage` 40, `UicUserInputInfoMgr` 32,
`UserInputCheckPkg` 1179, `Comm_App_Frame` 50, `SERVICES_ID` 6. The weak
`Comm_App_Frame::appdata_encode<UserInputCheckPkg>` instantiation is emitted.

## Unresolved / omitted

- `UicUserInputInfoMgr` and `UicTdirty` bodies belong to their own TUs; only
  exact layouts/signatures are declared here.
- `TssSdkCommSvc`, `ZEN_Thread_Mutex`, `Comm_App_Frame`, `SERVICES_ID` and
  `g_config` are external.
- TU static-initialization wrappers are compiler-generated and not
  hand-written; `g_obj_TssSdkUicPlugin` (BSS 0x0029f650) is not defined here.
- The uninitialized `proto_id_`, `pkg_len_`, `game_id_` head fields and the
  one-byte-over player-name terminator in the user-control TU are ORIG bugs
  and are intentionally preserved.
