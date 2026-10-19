# AGENT_REPORT_channel_misc

Reconstruction unit:
`recon/src/formmog/tss_sdk/plugins/proto_proc/channel_misc_agent.cpp`

Target:
`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
ELF32 i386, SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.

Status: all assigned symbols reconstructed, source compiles with the c5root
GCC 4.1.2 toolchain.  `TssSdkChannelPlugin::send_pkg_to_*` were **not**
re-defined: they already exist with matching semantics in
`plugins/channel_plugin_agent.cpp:1027-1046` (section 6 verifies them against
the shipped disassembly, per instruction).

## 1. Symbol and address inventory

Ranges are symbol ranges including alignment padding.

| Symbol | Address | Size | Source TU |
|---|---:|---:|---|
| `CIpcChannel::CIpcChannel()` C1 | `0x000a0f90` | `0x60` | `channel/ipc_channel.cpp` |
| `CIpcChannel::CIpcChannel()` C2 | `0x000a0ff0` | `0x60` | `channel/ipc_channel.cpp` |
| `CIpcChannel::open(unsigned int, unsigned int)` | `0x000a0ed0` | `0xb5` | `channel/ipc_channel.cpp` |
| `CIpcChannel::close()` | `0x000a0ab0` | `0x07` | `channel/ipc_channel.cpp` |
| `CIpcChannel::recv(void**, unsigned int*)` | `0x000a1050` | `0xda` | `channel/ipc_channel.cpp` |
| `CIpcChannel::send(void const*, unsigned int)` | `0x000a1130` | `0xea` | `channel/ipc_channel.cpp` |
| `TssSdkPkgPacker::get_packed_buf() const` | `0x00096ce0` | `0x08` | `framework/tss_sdk_pkg_packer.cpp` |
| `TssSdkPkgPacker::get_packed_buf_size() const` | `0x00096cf0` | `0x0e` | `framework/tss_sdk_pkg_packer.cpp` |
| `UICSocketChannelTCPHandler::define_comm_head_except_frame_length(Comm_App_Frame*)` | `0x000a1220` | `0x0f` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::check_if_full_frame(unsigned char*, unsigned int, bool*, int*)` | `0x000a1640` | `0xb5` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::~UICSocketChannelTCPHandler()` D0 | `0x000a1700` | `0x3f` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::~UICSocketChannelTCPHandler()` D1 | `0x000a1740` | `0x2e` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::~UICSocketChannelTCPHandler()` D2 | `0x000a1770` | `0x2e` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::UICSocketChannelTCPHandler()` C1 | `0x000a17a0` | `0x37` | `channel/uic_socket_tcp_handler.cpp` |
| `UICSocketChannelTCPHandler::UICSocketChannelTCPHandler()` C2 | `0x000a17e0` | `0x37` | `channel/uic_socket_tcp_handler.cpp` |
| `vtable for UICSocketChannelTCPHandler` | `0x00292e40` | `0x4c` | `channel/uic_socket_tcp_handler.cpp` |
| `typeinfo for UICSocketChannelTCPHandler` | `0x00292e8c` | `0x0c` | `channel/uic_socket_tcp_handler.cpp` |
| `UicTdirty::UicTdirty()` C2 | `0x000c83a0` | `0x12` | `uic/uic_tdirty.cpp` |
| `UicTdirty::UicTdirty()` C1 | `0x000c83c0` | `0x12` | `uic/uic_tdirty.cpp` |
| `UicTdirty::~UicTdirty()` D2 | `0x000c83e0` | `0x05` | `uic/uic_tdirty.cpp` |
| `UicTdirty::~UicTdirty()` D1 | `0x000c83f0` | `0x05` | `uic/uic_tdirty.cpp` |
| `UicTdirty::init(char*, unsigned int, sec_proto::UserInputCode)` | `0x000c8a90` | `0x6c` | `uic/uic_tdirty.cpp` |
| `UicTdirty::instance()` | `0x000c8b00` | `0x5d` | `uic/uic_tdirty.cpp` |
| `UicTdirty::instance_` | `0x002c3860` | `0x04` B | `uic/uic_tdirty.cpp` |

Mangled names match the shipped names:

```
_ZN12tss_sdk_impl11CIpcChannelC1Ev / C2Ev
_ZN12tss_sdk_impl11CIpcChannel4openEjj
_ZN12tss_sdk_impl11CIpcChannel5closeEv
_ZN12tss_sdk_impl11CIpcChannel4recvEPPvPj
_ZN12tss_sdk_impl11CIpcChannel4sendEPKvj
_ZNK12tss_sdk_impl15TssSdkPkgPacker14get_packed_bufEv
_ZNK12tss_sdk_impl15TssSdkPkgPacker19get_packed_buf_sizeEv
_ZN12tss_sdk_impl26UICSocketChannelTCPHandler36define_comm_head_except_frame_lengthEP14Comm_App_Frame
_ZN12tss_sdk_impl26UICSocketChannelTCPHandler19check_if_full_frameEPhjPbPi
_ZN12tss_sdk_impl26UICSocketChannelTCPHandlerD{0,1,2}Ev
_ZN12tss_sdk_impl26UICSocketChannelTCPHandlerC{1,2}Ev
_ZN12tss_sdk_impl9UicTdirtyC{1,2}Ev / D{1,2}Ev
_ZN12tss_sdk_impl9UicTdirty4initEPcjN9sec_proto13UserInputCodeE
_ZN12tss_sdk_impl9UicTdirty8instanceEv
```

## 2. CIpcChannel

### Layout (DWARF, `ipc_channel.cpp`)

```
+0x00000 char recv_buf_[65535]        (1 byte tail padding before the mutex)
+0x10000 ZEN_Thread_Mutex thread_mutex_   (28 bytes)
+0x1001c ZEN_BusPipe_TwoWay bus_channel_  (10292 bytes)
sizeof == 75856
```

`ZEN_Thread_Mutex` (real TU `zenlib/zen_lock_thread_mutex.cpp`) is 28 bytes:
vptr at +0, a 24-byte pthread mutex at +4.
Its shipped vtable slot 0x08 is `lock()`, slot 0x0c `try_lock()`, slot 0x10
`unlock()`.

`ZEN_BusPipe_TwoWay` is 10292 bytes and derives from `ZEN_Bus_MMAPPipe`; the
only fields this unit dereferences are `bus_pipe_pointer_[0]` (receive pipe)
and `bus_pipe_pointer_[1]` (send pipe), at `+0x200c`, i.e. `+0x12028` and
`+0x1202c` in `CIpcChannel`.

### Constructors (`0xa0f90` C1, `0xa0ff0` C2, identical)

```
ZEN_Thread_Mutex::ZEN_Thread_Mutex(this+0x10000, NULL)   // C1, 0x1a1490
ZEN_BusPipe_TwoWay::ZEN_BusPipe_TwoWay(this+0x1001c)     // C1, 0x001988e0
```

`recv_buf_` is not initialized.  The unwind path destroys the mutex if the
bus constructor throws.

### `open` (`0xa0ed0`)

```
char bus_name[4096];
snprintf(bus_name, 0xfff, "sec/tss_sdk_bus_%d", bus_channel_id);  // no rc check
rc = bus_channel_.initialize(bus_name,
                             ipc_bus_size, ipc_bus_size,  // send/recv size
                             0xffff,                      // max node size
                             true, false, (unsigned char)2);
if (rc != 0) { debug_errorex("bus init fail. ret=%d", rc); return -1; }
return 0;
```

The shipped frame reserves `0x1038` bytes with the name buffer at
`ebp-0x100c`; this unit reproduces the same stack shape.  The snprintf limit
is 4095, so a name of 4095+ characters is silently truncated.  An unused
`bool` local (`ebp-0xd`, initialized false) exists in ORIG and has no effect.

### `close` (`0xa0ab0`)

```
return 0;
```

No mutex, no bus call, no state change.  (The plugin's `fini()` treats this as
success; the real bus teardown happens through `~ZEN_BusPipe_TwoWay`.)

### `recv` (`0xa1050`)

```
if (recv_buf == NULL || recv_buf_size == NULL) return -1;
*recv_buf = NULL;
*recv_buf_size = 0xffff;
thread_mutex_.lock();                        // vtable +0x08 in ORIG
if (bus_channel_.bus_pipe_pointer_[0]->empty())
    { thread_mutex_.unlock(); return -1; }
bus_channel_.bus_pipe_pointer_[0]->pop_front(
        (ZEN_LIB::dequechunk_node *)recv_buf_);
*recv_buf = recv_buf_;
*recv_buf_size = *(unsigned int *)recv_buf_;   // dequechunk_node::size_of_node_
thread_mutex_.unlock();
return 0;
```

Contract detail: the caller receives the node **base**, including the 4-byte
`size_of_node_` word, not `chunkdata`.  `size_of_node_` is what the 0xffff
default is replaced with.  This matches the explanation in
`channel_plugin_agent.cpp` ("the first four bytes of the pointed-to data are
the length placed in the zenlib deque node").

`dequechunk_node` (DWARF, `zen_shm_lockfree_deque.h:36`) is
`{unsigned int size_of_node_; char chunkdata[];}`, sizeof 4.

### `send` (`0xa1130`)

```
if (send_buf_size > 0xffff) {
    debug_errorex("send buf to bus fail. buf too big. buf_size=%u",
                  send_buf_size);
    return -1;                               // no lock taken
}
thread_mutex_.lock();
rc = 0;
if (!bus_channel_.bus_pipe_pointer_[1]->push_end(
          (const ZEN_LIB::dequechunk_node *)send_buf)) {
    debug_alertex("[zenlib] %u Pipe is full or data small?,Some data can't "
                  "put to pipe. Please increase and check. "
                  "nodesize=%u, freesize=%u",
                  1, *(const unsigned int *)send_buf,
                  bus_channel_.bus_pipe_pointer_[1]->freesize());
    rc = -1;
}
thread_mutex_.unlock();
return rc;
```

The buffer is pushed by reference; the first four bytes must already contain
`size_of_node_`.  The alert's numeric arguments are `(1, node_size,
freesize)`.  `0xffff` itself is accepted; only `> 0xffff` is rejected.

## 3. TssSdkPkgPacker

Layout (`tss_sdk_pkg_packer.cpp`, DWARF):

```
+0x00000 char   buf_[65535]
+0x10000 size_t used_buf_size_     (1 byte tail padding after buf_)
sizeof == 65540
```

`get_packed_buf()` (`0x96ce0`) returns `this` (offset 0; the shipped code is
exactly `mov 0x8(%ebp),%eax; ret`).  Return type from DWARF is
`const char *`.

`get_packed_buf_size()` (`0x96cf0`) returns `*(size_t *)(this + 0x10000)`.

## 4. UICSocketChannelTCPHandler

### Hierarchy, size, vtable

Shipped layout sizes (DWARF):

| Type | Size | Notes |
|---|---:|---|
| `ZEN_Event_Handler` | 12 | vptr +0, `zen_reactor_` +4, `event_mask_` +8 |
| `SocketChannelBaseHandler` | 52 | fields at +0x0c..+0x33 |
| `SocketChannelTCPHandler` | 88 | fields at +0x34..+0x57 |
| `UICSocketChannelTCPHandler` | 88 | adds no data members |

Vtable (`0x00292e40`, 0x4c = 19 entries, 17 function pointers):

```
slot  0 (0x08) D1
slot  1 (0x0c) D0
slot  2 (0x10) SocketChannelTCPHandler::get_handle() const
slot  3 (0x14) ZEN_Event_Handler::handle_input()
slot  4 (0x18) ZEN_Event_Handler::handle_output()
slot  5 (0x1c) ZEN_Event_Handler::handle_exception()
slot  6 (0x20) ZEN_Event_Handler::handle_close()
slot  7 (0x24) ZEN_Event_Handler::reactor(ZEN_Reactor*)
slot  8 (0x28) ZEN_Event_Handler::reactor() const
slot  9 (0x2c) SocketChannelTCPHandler::is_valid()
slot 10 (0x30) SocketChannelTCPHandler::is_idle()
slot 11 (0x34) SocketChannelTCPHandler::send(unsigned char*, unsigned int)
slot 12 (0x38) SocketChannelTCPHandler::init_connect(...)
slot 13 (0x3c) SocketChannelTCPHandler::connect_server()
slot 14 (0x40) SocketChannelTCPHandler::handle_timeout(...)
slot 15 (0x44) UICSocketChannelTCPHandler::check_if_full_frame(...)
slot 16 (0x48) UICSocketChannelTCPHandler::define_comm_head_...
```

`SocketChannelTCPHandler::handle_input()` calls slot +0x3c to test frame
completeness and slot +0x40 to fill the synthetic `Comm_App_Frame` header
after a complete frame, so preserving slots 15/16 is required.  The
reconstruction models the full three-level
hierarchy (not a flat class) and the compiled object's vtable relocations
contain exactly the same 17 target symbols as `0x00292e48..0x292e88`.

### Constructor / destructor

`0xa17a0` C1 and `0xa17e0` C2: call
`SocketChannelTCPHandler::SocketChannelTCPHandler()` C2 (`0x000d9940`) and
install the derived vptr.  No member initialization.

`0xa1700` D0: install vptr, call `SocketChannelTCPHandler::~...` D2
(`0x000d9800`), then `operator delete`.
`0xa1740` D1 and `0xa1770` D2: identical to D0 without the delete.
The reconstructed source emits D0/D1/D2 with the same 0x3f/0x2e/0x2e sizes.

### `define_comm_head_except_frame_length` (`0xa1220`)

```
frame->frame_command_ = 0x493ef;   // +0x08, decimal 300015
```

`0x493ef` is the UIC command id that `TssSdkFramework::proc_buf_from_backend_`
maps to `ETSS_SDK_PLUGIN_ID_UIC`.  Nothing else is written; `frame_length_`
and `frame_appdata_` are filled by `SocketChannelTCPHandler::handle_input`
(total length `payload_len + 50`).

### `check_if_full_frame` (`0xa1640`)

Input buffer is the raw UIC TCP stream at `recv_buf_ + nextReadStart`; `len` is
the unread byte count.  The protocol frame is a fixed 26-byte body whose
big-endian 16-bit length field sits at `buf+4`.

| Guard (in evaluation order) | Action | Return |
|---|---|---:|
| `len <= 9` | `debug_traceex("no complete head")`, `*full = false` | 0 |
| `ntohs(u16 @ buf+4) != 26` | `debug_errorex("bodyLen error, close connection")`; `*full` untouched | -1 |
| `len < 26` | `debug_traceex("no complete body")`, `*full = false` | 0 |
| otherwise | `*full = true`, `*frame_size = 26`, `debug_debugEx("find a complete package, total len: %d", 26)` | 0 |

The type check precedes the body-length check, so a 10..25-byte buffer with a
different length word is a fatal protocol error, not an incomplete frame.
`*frame_size` is not touched unless the full-frame branch is taken.
`FRAME_LEN` is 26 (`UICSocketChannelTCPHandler::FRAME_LEN`, DWARF const 26,
no emitted data symbol).

## 5. UicTdirty

### Layout and lifecycle

```
+0x00 sec_proto::UserInputCode code_
+0x04 bool is_inited_
sizeof == 8
```

`UserInputCode` (DWARF): `USER_INPUT_CODE_GBK = 0`, `USER_INPUT_CODE_UTF8 = 1`.

Constructors (`0xc83a0` C2, `0xc83c0` C1): `code_ = 0`, `is_inited_ = false`.
Destructors (`0xc83e0` D2, `0xc83f0` D1): empty; the class has no virtual
destructor and no D0 symbol.

### `instance` (`0xc8b00`)

```
static UicTdirty *instance_ = NULL;       // .bss, 0x002c3860
UicTdirty *UicTdirty::instance() {
    if (instance_ == NULL)
        instance_ = new (std::nothrow) UicTdirty();
    return instance_;
}
```

Not thread-safe; no `__cxa_guard`, no `__cxa_atexit` registration.  A failed
allocation stores NULL and returns NULL.

### `init` (`0xc8a90`)

```
if (is_inited_) return 0;
if (tInitializeDirtyCheck(dirty_check_file, bus_id) < 0) {
    debug_errorex("[uic sdk]UicTdirty::init tInitializeDirtyCheck failed");
    return -1;
}
code_ = code;
return 0;
```

`tInitializeDirtyCheck` (`0x0018ed80`) is a C-linkage tdirty API taking
`(char *pszFile, unsigned int uiBusid)` and returning `int`; the second
argument is a bus id, not a buffer size.

**Anomaly reproduced from ORIG:** the success path does **not** set
`is_inited_` to true.  There is no byte store to `this+4` anywhere in the
0x6c-byte body.  The guard therefore only suppresses a second call if some
other unit writes the flag.

## 6. Existing `TssSdkChannelPlugin::send_pkg_to_*` (verified, not redefined)

Per instruction these remain in `plugins/channel_plugin_agent.cpp:1027-1046`.
Disassembly verification:

| Symbol | Address | Size | Shipped body | Existing source |
|---|---:|---:|---|---|
| `send_pkg_to_ipc_channel(TssSdkPluginChannelIpcChannelPkg const&)` | `0x0009cba0` | `0x36` | `CIpcChannel::send(pkg.buf_, pkg.buf_size_)` on `this+0x30` | lines 1027-1031, exact |
| `send_pkg_to_socket_channel(TssSdkPluginChannelSocketChannelPkg const&)` | `0x0009cb50` | `0x44` | `SocketChannel::instance()->send_data(pkg.business_id_, pkg.buf_)` | lines 1035-1039, exact |
| `send_pkg_to_game_channel(TssSdkPluginChannelGameChannelPkg const&)` | `0x0009ca50` | `0x14` | `this->callback_proc_((void *)&pkg)`; callback is the function pointer at ITssSdkPlugin `+0x04` | lines 1043-1046, exact |

Additional verified details:

- The socket package is 10 bytes: `business_id_` +0, `buf_` +4,
  `buf_size_` (short) +8.  `send_pkg_to_socket_channel` loads only +0 and +4;
  `buf_size_` is ignored.  The frame length is the first word of `buf_`
  inside `SocketChannel::send_data`.
- The IPC package is 8 bytes: `buf_` +0, `buf_size_` +4.  The 0xffff limit is
  enforced inside `CIpcChannel::send`, not in the wrapper.
- The game package is 14 bytes and is passed by address; the callee is the
  registered callback, not a channel/stats path.
- These symbols are strong (`T`) in the target.  They are not duplicated in
  `channel_misc_agent.cpp` so `g++ -r link_check.sh` cannot see multiple
  definitions.

## 7. External boundaries and call targets

Only declarations/partial layout models are provided for these; no bodies are
emitted.

| Target | Address | Used by |
|---|---:|---|
| `ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)` C1 | `0x001a1490` | `CIpcChannel::CIpcChannel` |
| `ZEN_Thread_Mutex::lock()` | `0x001a11f0` | `recv`, `send` |
| `ZEN_Thread_Mutex::unlock()` | `0x001a1170` | `recv`, `send` |
| `ZEN_BusPipe_TwoWay::ZEN_BusPipe_TwoWay()` C1 | `0x001988e0` | `CIpcChannel::CIpcChannel` |
| `ZEN_BusPipe_TwoWay::initialize(char const*, unsigned, unsigned, unsigned, bool, bool, unsigned char)` | `0x00198790` | `CIpcChannel::open` |
| `ZEN_LIB::shm_dequechunk::empty()` | `0x001ab2d0` | `CIpcChannel::recv` |
| `ZEN_LIB::shm_dequechunk::pop_front(ZEN_LIB::dequechunk_node*)` | `0x001ab820` | `CIpcChannel::recv` |
| `ZEN_LIB::shm_dequechunk::push_end(ZEN_LIB::dequechunk_node const*)` | `0x001ab3e0` | `CIpcChannel::send` |
| `ZEN_LIB::shm_dequechunk::freesize()` | `0x001ab260` | `CIpcChannel::send` error log |
| `snprintf` | libc PLT | `CIpcChannel::open` |
| `ZEN_Trace_LogMsg::debug_errorex/alertex/traceex/debugEx` | `0x1b3d30/0x1b3ce0/0x1b3e20/0x1b3dd0` | all error paths |
| `tInitializeDirtyCheck` | `0x0018ed80` | `UicTdirty::init` |
| `SocketChannelTCPHandler::SocketChannelTCPHandler()` C2 | `0x000d9940` | UICSocket ctor |
| `SocketChannelTCPHandler::~SocketChannelTCPHandler()` D2 | `0x000d9800` | UICSocket dtor |
| `ZEN_Event_Handler::handle_{input,output,exception,close}` | `0x19de40/50/60/70` | UICSocket vtable slots 3-6 |
| `ZEN_Event_Handler::reactor(...)` | `0x19dea0/0x19deb0` | UICSocket vtable slots 7-8 |
| `SocketChannelTCPHandler::{get_handle,is_valid,is_idle,send,init_connect,connect_server,handle_timeout}` | `0xd93d0/0xd91d0/0xd91f0/0xd9320/0xd9600/0xd9af0/0xd9270` | UICSocket vtable slots 2, 9-14 |

No ASM bridges, stubs, or fallback transports are used.

## 8. Compile result

Toolchain: c5root GCC `4.1.2 20080704 (Red Hat 4.1.2-55)` (the c5root
`4.1.2` symlink points at its `4.1.1` libexec directory).  The old driver
needs `-B<...>/4.1.2/` in this environment.

Command:

```sh
g++ -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -I recon -c \
    recon/src/formmog/tss_sdk/plugins/proto_proc/channel_misc_agent.cpp
```

Result: **exit 0**, ELF32 i386 relocatable object.  The only warnings are six
`__builtin_offsetof` warnings (twice each) emitted by GCC 4.1 for non-POD
members used in the layout assertion; they are non-fatal and the assertion
still evaluates at compile time.  Sizes verified by the assertion:

`CIpcChannel` 75856, `ZEN_Thread_Mutex` 28, `ZEN_BusPipe_TwoWay` 10292,
`Comm_App_Frame` 50, `SERVICES_ID` 6, `ZEN_Event_Handler` 12,
`SocketChannelBaseHandler` 52, `SocketChannelTCPHandler` 88,
`TssSdkPkgPacker` 65540, `UicTdirty` 8, `UICSocketChannelTCPHandler` 88,
`dequechunk_node` 4.

Object-level checks performed:

- Every assigned symbol is emitted with the target mangled name and the
  expected binding (`T`; `UicTdirty::instance_` is `B`/4).
- `vtable for UICSocketChannelTCPHandler` is 0x4c and its relocation list
  contains the same 17 target symbols/order as the shipped vtable; typeinfo
  is `__si_class_type_info` with base `_ZTI23SocketChannelTCPHandler`.
- D1/D0/D2 and C1/C2 sizes match the shipped 0x3f/0x2e/0x2e and 0x37/0x37.
- `CIpcChannel` C1/C2, `TssSdkPkgPacker` accessors,
  `define_comm_head_except_frame_length`, `UicTdirty` ctor/dtor/init have the
  same instruction sequences and sizes as the target (relocation encodings
  may differ).
- `CIpcChannel::open/recv/send`, `check_if_full_frame` and
  `UicTdirty::instance` differ only in compiler scheduling, `ntohs` folding
  and branch layout (see section 9).

## 9. Differences from ORIG and unresolved behavior

1. **Mutex virtual dispatch.**  The shipped code calls
   `ZEN_Thread_Mutex::lock/unlock` through vtable slots +0x08/+0x10.  GCC
   4.1.2 devirtualizes calls on the concrete member subobject and emits
   direct PLT calls to the same two symbols.  This is behaviourally
   equivalent (a member subobject cannot have a different dynamic type).
2. **`CIpcChannel::recv` branch layout.**  ORIG funnels the empty-pipe and
   success paths through one unlock call; this build emits two unlock call
   sites.  Outputs and return values are identical.
3. **`CIpcChannel::open` size.**  ORIG is 0xb5, this build 0xb1; the unused
   `bool` local in ORIG accounts for part of the difference.  The
   `snprintf` buffer/limit and all seven `initialize` arguments are identical.
4. **`UICSocketChannelTCPHandler::check_if_full_frame` size.**  ORIG is
   0xb5, this build 0xb2.  GCC 4.1.2 folds `ntohs(u16 @ buf+4) == 26` into
   `cmpw $0x1a00,0x4(buf)`, whereas ORIG keeps `movzwl` + `ror $8` + `cmp`.
   The memory access, compare value, branches and returns are identical.
5. **`UicTdirty::instance` size.**  ORIG is 0x5d, this build 0x79 because GCC
   4.1.2 emits an unreachable nothrow-new cleanup landing pad
   (`operator delete(void*, nothrow_t const&)` + `_Unwind_Resume`).  The
   constructor cannot throw, so the extra path is unreachable.
6. **Partial zenlib/commlib models.**  `ZEN_Thread_Mutex`,
   `ZEN_BusPipe_TwoWay`, `ZEN_LIB::shm_dequechunk`,
   `ZEN_Event_Handler`, `SocketChannelBaseHandler` and
   `SocketChannelTCPHandler` are modelled only to the extent needed for call
   targets, vtable slots and object sizes.  Opaque bytes are used where the
   real members are irrelevant; the external constructors/destructors write
   the real layout into those bytes.  `Comm_App_Frame` models
   `frame_option_` as the unsigned word covering the shipped union
   `{unsigned int frame_option_; _APP_FRAME_OPTION inner_option_;}`.
7. **Static initialization not reproduced.**  The four original TUs each
   carried a 16-entry `ZEN_STATUS_ITEM_WITHNAME` static table (ids
   0x238d..0x239c) and its `__cxa_atexit` registration.  Those are TU-level
   stat tables, not part of the assigned class semantics, and are not emitted
   here.
8. **`UicTdirty::is_inited_` is never set by `init`.**  Reproduced exactly
   from the shipped store pattern; called out in section 5.
9. **No byte-identity claim.**  Addresses, encodings and branch layout differ
   where the compiler version differs; observable semantics, call targets,
   field offsets, guards and return values are the acceptance criterion.

## 10. Semantic verdict

| Item | Verdict |
|---|---|
| `CIpcChannel` ctor/open/close/recv/send | Correct: layout, extern calls, guards and returns verified against disassembly |
| `send_pkg_to_ipc/socket/game_channel` | Already correct in `channel_plugin_agent.cpp`; addresses/arguments re-verified; no duplicate |
| `TssSdkPkgPacker` accessors | Correct and instruction-identical |
| `UICSocketChannelTCPHandler` ctor/dtor/check/define | Correct: size 88, exact vtable slot set, decision table and 0x493ef store verified |
| `UicTdirty` ctor/init/instance | Correct: size 8, lazy nothrow singleton, signed-return guard; `is_inited_` non-store documented |
| Compile | Pass (exit 0, GCC 4.1.2 c5root, -m32) |

## 11. Runtime IPC initialization validation (2026-10-19)

The original-client login reached `Inter_MonitorLoginOK`, then crashed during
`CIpcChannel::send` because the send-pipe pointer was null. Runtime inspection
showed that fresh `ZEN_Bus_MMAPPipe::initialize` calls copied the zero-filled
new mapping into `bus_head_`; `init_all_pipe` then saw `number_of_pipe_ == 0`
and returned success without populating either pipe pointer. The fresh-map
branch now copies the prepared `bus_head_` into the mapping; restore continues
to copy the validated mapping header into `bus_head_`.

Verification after rebuilding and deployment:

- Strict server build passed: 227 translation units compiled, no failures.
- Deployed library SHA-256: `ddf2d0dce5215f67ce93273d2b3ee54093b5f4d53c2ca000a56a49154ad6aecd`.
- `game_siroco11` remained running with port 30011 listening and mapped that
  deployed library.
- The created shared header reports `size_of_sizet=4`, two pipes, and
  8,388,608-byte pipe sizes; each ring header reports an 8,388,636-byte mmap,
  8,388,608-byte ring, and `max_len_node=65535`.
- GDB inspection confirmed non-null receive/send pipe pointers and initialized
  ring headers in the live channel process.
- After deployment, the original client authenticated at 09:08:12, reached
  `Inter_MonitorLoginOK`, and reached character selection; its TCP connection
  to port 30011 remained established. The channel did not crash.
- The character list was empty and the user reports character creation failed
  with a packet error. No `DB_LoadCharac` event appears after this login;
  an earlier session logged character 1 (`测试角色`). Entering the character
  or game world remains unverified.

### Post-login TSS packet path (cause not yet established)

- The game-server sources distinguish opcode 604 (`ENUM_CMDPACKET_DPROTO`) from
  opcode 605 (`ENUM_CMDPACKET_DPROTO_CALLBACK`). `Dispatcher_Antibot_DP::dispatch_sig`
  handles 604 and calls the anti-bot checker vtable slot `+0x2c` with a decrypt
  buffer; this is the TSS `AntibotInterface::RecvBufferForDecrypt` boundary
  (`source/DNFServer/GameServer/Game/Dispatchers/dispatch_sig_13.inc`).
- Opcode 605 has a separate dispatcher (`Dispatcher_Antibot_DPCallBack`); the
  available server `read` and `process` bodies are empty stubs, and the dispatch
  body is not reconstructed in that tree. Historical successful sessions also
  contain 605, so its presence alone is not evidence of a TSS decrypt failure.
- The TSS path for opcode 604 is `RecvBufferForDecrypt` → legacy `DECRYPT_PKG_OLD`
  notify → `TssSdkProtoProcPlugin::handle_decrypt_pkg_old` →
  `DPInterface::recv_buffer_for_decrypt` → `DPVMBaseEngine::decrypt`. The latter
  dispatches package type `0x10` to fixed decrypt and `0x08` (with a user) to
  dynamic decrypt; other types return -1.
- `secagent_bill_20261019.log` records AddUser from SDK at 09:08:12 and deletes
  at 09:09:05/09:09:12. The current SDK log files have no callback result for
  that session (latest relevant `tss_sdk_3.log` entry is 01:05). Historical
  successful sessions also contain DPROTO callbacks. Therefore the current
  evidence does not establish an SDK decrypt failure, and no packet-handler
  code was changed speculatively.

- A fresh original-client launch at 10:42:36 again reached character selection
  with no displayed characters; server log records login, `Inter_MonitorLoginOK`,
  and SDK AddUser. A 30-second debugger capture at the TSS DP decrypt entry/return
  did not trigger before any character action. This confirms the AddUser path
  but does not exercise or validate opcode 604 decryption. The user previously
  reported a creation packet failure; that failing action was not captured by
  this debugger session.

- After the client restart, the user again reached the empty character page.
  Server log records a peer reset at 10:52:55 with `recv(605)`, then a new
  authenticated session at 10:53:08; `secagent_bill_20261019.log` records
  AddUser at 10:42:36 and 10:53:08. The raw packet body and SDK decrypt return
  were not captured. Opcode 605 is a distinct callback dispatcher, not the
  opcode-604 TSS decrypt entry; do not infer SDK corruption from this log alone.

- At 10:59:24 the channel again logged peer reset with `recv(605)`; a new
  login reached `Inter_MonitorLoginOK` at 11:00:05. The armed breakpoints on
  `AntibotInterface::RecvBufferForDecrypt`, `DPVMBaseEngine::decrypt`, and the
  `DPInterface` decrypt return did not fire during that observation window.
  This does not capture the user's earlier failing action or prove the game
  server's 605 handler outcome, but it means no opcode-604 SDK decrypt callback
  was observed in that interval. No packet payload was captured.
