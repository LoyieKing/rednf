# AGENT_REPORT_commlib — commlib framework / zenlib / appcomm reconstruction

Scope: `libtss_sdk.so.1.0.394` (ELF32 i386, GCC 4.1.0, unstripped DWARF), reconstructed under
`tools/tss_sdk_recon/recon/src/`. Compiler: vendor GCC 4.1.2 via `sh c5build.sh <src> <o>`.

## Status matrix (per CU)

| CU | file(s) | ORIG symbols in CU | emitted | build rc | size ratio range |
|---|---|---|---|---|---|
| zen_os_adapt_rwlock.cpp | `recon/src/commlib/zenlib/zen_os_adapt_rwlock.{h,cpp}` | 14 | 14 | 0 | 1.000 (all identical) |
| zen_lock_file_lock.cpp | `.../zenlib/zen_lock_file_lock.{h,cpp}` | 15 | 15 | 0 | 1.000 (all identical) |
| zen_lock_thread_rw_mutex.cpp | `.../zenlib/zen_lock_thread_rw_mutex.{h,cpp}` | 12 | 12 | 0 | 0.903–1.047 |
| zen_thread_error_info.cpp | `.../zenlib/zen_thread_error_info.{h,cpp}` | 8 | 8 | 0 | 1.000 (all identical) |
| zen_base64.cpp | `.../zenlib/zen_base64.{h,cpp}` | 4 | 4 | 0 | 0.965–0.984 |
| zen_os_adapt_dirent.cpp | `.../zenlib/zen_os_adapt_dirent.{h,cpp}` | 13 | 13 | 0 | 0.923–1.083 |
| zen_server_status.cpp | `.../zenlib/zen_server_status.{h,cpp}` | 30 | 30 | 0 | 0.737–1.095 |
| comm_socket_channel_base_handler.cpp | `.../framework/comm_socket_channel_base_handler.{h,cpp}` | 9 | 9 | 0 | 1.000–1.069 |
| comm_socket_channel_bus.cpp | `.../framework/comm_socket_channel_bus.{h,cpp}` | 7 | 7 | 0 | 1.024–1.118 |
| comm_socket_channel_info.cpp | `.../framework/comm_socket_channel_info.{h,cpp}` | 8 | 8 | 0 | 1.033–1.140 |
| comm_socket_channel.cpp | `.../framework/comm_socket_channel.{h,cpp}` | 21 | 21 | 0 | 1.000–1.083 (10 identical) |
| comm_socket_channel_tcp_handler.cpp | `.../framework/comm_socket_channel_tcp_handler.{h,cpp}` | 27 | 27 | 0 | 0.870–1.071 (20 identical) |
| app_bus_channel_id_mgr.cpp | `.../formmog/appcomm/app_bus_channel_id_mgr.{h,cpp}` | 15 | 15 | 0 | 0.870–1.146 (6 identical) |
| comm_cfgsvr_sdk.cpp | `.../framework/comm_cfgsvr_sdk.{h,cpp}` | 73 (49 named + 24 weak copies) | 73 | 0 | 0.891–1.148 (23 identical) |

Matrix: `recon/src/commlib/AGENT_REPORT_commlib_matrix.tsv` (209 rows, 134 byte-size
identical). The only apparent >15 % row is the `vtable for CfgSvrSdk` 16-vs-20 entry,
which is a `.dynsym` vs `.symtab` size artefact: both ORIG `nm -S` and the recon object
report `_ZTV9CfgSvrSdk` as 0x14 bytes. All real symbol sizes are within ±15 %.

## Commands used

```
sh c5build.sh recon/src/commlib/zenlib/<cu>.cpp /tmp/<cu>.o          # rc=0 for every CU above
bash /tmp/cml_work/cmp.sh <obj> "<mangled-name regex>"               # nm -S vs nm -SD sizes
python3 cu_symbols.py <ORIG> <cu>.cpp --sizes                        # DWARF attributed symbols
python3 cu_layout.py  <ORIG> <cu>.cpp <Type>                         # DWARF layouts
python3 annstr.py <start> <stop>                                     # PIC lea -> rodata strings
python3 vt.py <start> <stop>                                         # vtable slots + relocations
```
`cmp.sh` = per-symbol `nm -S` size join of ORIG (dynamic table) vs our object, plus
ORIG-only / NEW-only sets.

## Semantics notes (verified against ORIG disassembly, not guessed)

* **zen_os_adapt_dirent.cpp** — modelled the 2009 `_FILE_OFFSET_BITS=64` ABI explicitly:
  276-byte `struct dirent`, `DIR = struct __dirstream`, and our own `extern "C"`
  declarations bound with `__asm__("readdir64"/"scandir64"/"alphasort64"/...)`, so the host
  glibc 2.39 `scandir`/`alphasort` prototypes are never used. `dirname` keeps the trailing
  `/` of the original, `mkdir_recurse` ignores `EEXIST`, `basename`/`dirname` use the
  original 4097-byte / caller-buffer conventions.
* **zen_lock_thread_rw_mutex.cpp** — preserves three ORIG artefacts: `try_lock_read()`
  calls `pthread_rwlock_trywrlock`, the ctor's second failure log says
  `ZEN_OS::pthread_mutex_init`, and `duration_lock_{read,write}` add
  `gettimeofday`-based `ZEN_Time_Value` before dispatching through the vtable.
* **zen_base64.cpp** — the ORIG inverse table is 128 bytes at `.rodata 0x240380` and the
  decoder indexes it with `movzbl` of the input byte, so bytes ≥ 0x80 read the encoder
  alphabet and neighbouring literals (`RECV_PIPE`, `SEND_PIPE`, `POSIXLY_CORRECT`, …).
  The 256-byte effective table is reproduced verbatim; the encode wrap path also copies
  only three bytes into the zeroed 32-bit node size when a frame straddles the chunk end.
* **zen_server_status.cpp** — 394 differs from the 435 reuse tree: `ZEN_LIB::shm_vector<T>`
  is 12 bytes without a vptr (`smem_base_/vector_head_/data_base_`), `_shm_vector_head` keeps
  `size_of_mmap_/num_of_node_/num_of_use_`, and `dump_all` grows the caller vector with
  default items when it is shorter than the shared-memory count, otherwise erases
  `[begin+count, end)` (the 435 tree erases the first `count` items, which is wrong for 394).
  `initialize(restore=true)` keeps the shared size untouched; shm geometry
  `+0x10 / +0x18001c` headers, `+0x1c / +0x180028` data.
* **comm_socket_channel_bus.cpp** — `fini()` in the ORIG never frees a non-NULL pipe or
  pop buffer (only `operator delete(NULL)` in the null branch); `pop_recv_bus` returns
  `!pop_front()`; `peek_send_bus` reproduces the 3-byte wrap copy. All lock scopes use an
  RAII guard to reproduce the ORIG landing pads that unlock before `_Unwind_Resume`.
* **comm_socket_channel_info.cpp** — both `add_handler` and `get_first_valid_handler` lock
  with `lock_read` (vtable slot 0x1c); an invalid handler is logged and `connect_server()`d.
* **comm_socket_channel.cpp** — error codes 0x1430/0x1431/0x1432/0x1433/0x1434 preserved,
  `send_to_net` sends `node + 0x32` with `*(uint32_t*)node - 0x32`, `init_connection` deletes
  the handler on every failure path and locks the map with `lock_read` for `insert`,
  `svc()` loops on `ZEN_Reactor::handle_events` with a 1000 µs `ZEN_Time_Value`, `fini()`
  destroys business infos then erases the node.
* **comm_socket_channel_tcp_handler.cpp** — status 0/1/2, 0x10000 send/recv buffers and a
  0x10032 pack buffer, frame = 4-byte total length + 0x32-byte header
  (`define_comm_head_except_frame_length`), `init_tcp_handler` keeps `SO_KEEPALIVE`/
  `TCP_NODELAY`, `connect_server` treats `EINPROGRESS`/`EINTR` as success and returns -2 when
  `init_tcp_handler` fails, `read_data_from_tcp` maps `EAGAIN`→0 and `EINTR`→0 (logged).
  `ZEN_Sockaddr_In::get_host_addr()` (65-byte function-local static) is emitted here, as in ORIG.
* **app_bus_channel_id_mgr.cpp** — `/tmp/tss_sdk_game_svr_id_pool_file`, lock file opened with
  `0x42/0x1b0`; `write/read_channel_info_to_file_without_lock` return -1 on the first short
  fwrite/fread and -2 on the second; `get_bus_channel_id()` returns -4 uninitialised while
  `get_bus_channel_id(id)` returns -1; the ctor leaves `max_channel_id_` uninitialised;
  `init_bus_channel_info_list()` does not `fclose` on a write failure (ORIG leak preserved).

## Size deviations > ±15 % (8 rows) and explanations

* `SocketChannelTCPHandler::~SocketChannelTCPHandler` (113 vs 121), `SocketChannelBus` /
  `SocketChannelBusinessInfo` dtors and `ZEN_Server_Status` dtors (e.g. 274 vs 372):
  the ORIG GCC elides calls to the empty out-of-line destructors of the element types
  (`ZEN_STATUS_ITEM`, `ZEN_STATUS_ITEM_WITHNAME`, `SocketChannelBaseHandler`) inside
  `std::vector`/`std::map` destruction loops; GCC 4.1.2 emits the PLT calls. Behaviour is
  identical (the destructors are empty), only the loop body is larger.
* `ZEN_Server_Status::{set_by_statid,dump_status_info}` (~18 % smaller): register
  allocation/exception-region differences between GCC 4.1.0 and 4.1.2; the call sequence
  (virtual lock, `find_insert_idx`, vtable unlock, error codes) is identical.
* typeinfo/vtable data symbols: identical layout (`__vmi_class_type_info` after adding
  `ZEN_NON_Copyable` as second base of `SocketChannelBaseHandler`, matching DWARF).

## comm_cfgsvr_sdk.cpp (added)

Source: 435 reuse implementation ported to a **self-contained** header
(`comm_cfgsvr_sdk.h` models `SERVICES_ID` (packed, 6 B), `conf_cfgsdk::cfgsdk_config`
(154 B), the 13 `sec_proto` TDR message structs (pack(1)), `Comm_App_Frame` (0x32-byte
header + appdata), `Tibetan/Zulu_SendRecv_Package` (tibetan_recv_appframe_ @+0x24,
zulu_connected_ @+0x2c, zulu_svc_ip_ @+0x30, zulu_stream_ @+0x4c, sizeof 80) and the
`Comm_Svrd_Config` fields read by this CU). No secagent include path is used.

394 port fixes found by verification:
* `SERVICES_ID` had to be `#pragma pack(1)`; without it the recon `CfgSvrSdk` was 348 B with
  every member shifted (self_info_ +16 ✓ but master_cfgsvr_info_ +24 vs ORIG +22). After the
  fix the DWARF layout of the recon object is byte-identical to ORIG: 344 B, offsets
  +0x10/+0x16/+0x1c/+0x22/+0x24/+0x40/+0x5c/+0xf8/+0x104/+0x110/+0x114/+0x118/+0x11c/
  +0x12c/+0x13c/+0x154/+0x155.
* `SERVICES_ID` ctor/dtor/set_serviceid are declared, not defined, in the ORIG (they live in
  `comm_service_info.cpp`); inlining them shrank the CfgSvrSdk methods. Declaring them out of
  line restored the ORIG call shape (ctor/dtor 853→822 vs 853, dtor 1101→981 vs 1101).
* `Comm_App_Frame` and `Zulu_SendRecv_Package` inline templates are `noinline`: ORIG keeps
  every `appdata_encode/decode` and `send/receive_svc_package<T>` instantiation out of line
  and calls the weak copies (PLT calls at 0xe1496 etc.). With `noinline`, the previously
  1.53–1.58× methods (get_svc_id 532 vs 841, get_ip_info 555 vs 864, get_svc_deploy_info
  544 vs 830, send_config_file_req 243 vs 385) fall to 1.00–1.08×.
* `ZEN_Sockaddr_In::get_host_addr()` is header-inline (weak copy in both the TCP-handler CU
  and this CU, matching ORIG) and `get_port_number()` reads the ORIG +0x0e port offset.
* Static init: with `<iostream>` included the TU now emits
  `__static_initialization_and_destruction_0(int,int)`, `__tcf_0` and a
  `_GLOBAL__I_*` thunk as the ORIG does. GCC 4.1.2 keys the thunk on
  `CfgSvrSdk::CfgSvrSdk()` whereas ORIG keys it on `CfgSvrSdk::instance_` (TObj ordering);
  the three symbols are compiler-generated init/atexit artefacts, not API symbols.

Verification of 394 vs 435 semantics (instruction-level, all methods):
* `cu_symbols.py` list: 73 symbols; the recon object defines all 73 (raw mangled names).
* Immediate-operand comparator (`immcmp.py`) over all 16 non-trivial methods: the only
  differing immediates are stack-frame sizes (`sub $0xa8/%esp`, `$0x108`) and the extra
  zeros/inlined-constant stores produced by inlining before the `noinline` fix; every API
  constant (0xffbd frame size, 30 s `CHECK_UPDATE_TIME`, 0x1428/0x1429/0x142a/0x142c/0x13e7/
  0x13a0 returns, 50-byte app frame header, 9000 port, 52428800 file cap) is present in both.
* `recv_config_file_rsp` (586 vs 580), `get_config_update_list` (981 vs 1028),
  `get_config_file` (592 vs 599), `update_config_file`, `get_config_file()` and
  `init_zulu_ex` (557 vs 577) preserve the 394 return/cache logic: `CONFIG_IS_NEWEST`
  (0x1428) on local MD5 match, `CONFIG_NEED_GET_MORE` (0x142a) while
  `recv_len_ < cfg_len_`, `CONFIG_IS_GET_OVER` (0x1429) when complete, and the
  `is_updade_config_` flag toggling in `check_config_md5()`/`update_config_file()`.
* `Zulu_SendRecv_Package::receive_svc_package<T>` verified instruction-for-instruction
  against ORIG 0xe5590: loop on `if_loop`, `frame_command_` mismatch → log + close +
  return 0x13e7, `frame_length_ > 0x32` → `appdata_decode`.

Template/weak copies emitted by this CU (all also weak in ORIG, 24 total):
`Comm_App_Frame::appdata_encode<T>` for SvcIpInfoReq/SvcIdReq/DeployInfoReq/CfgFileReq/
CheckCfgFileUpdateReq (5), `appdata_decode<T>` for SvcIpInfoRsp/SvcIdRsp/CfgsvrInfoRsp/
CfgFileRsp/CheckCfgFileUpdateRsp/DeployInfoRsp (6), `Zulu_SendRecv_Package::
send_svc_package<T>` (5) and `receive_svc_package<T>` (6), plus
`std::_Rb_tree<...CfgSvrSdk::CFG_MD5...>::{find,upper_bound,lower_bound,_M_insert,
insert_unique,_M_erase}` (6), `std::vector<unsigned short>::_M_insert_aux`,
`std::vector<std::string>::_M_insert_aux`. They are weak because they are header-inline
templates/STL members instantiated in this TU; ORIG emits the same weak copies for exactly
the same reason (they are not defined anywhere else in the library).

## Not delivered
* `comm_socket_channel_udp_handler.cpp` (if it exists in 394) was not in scope.
