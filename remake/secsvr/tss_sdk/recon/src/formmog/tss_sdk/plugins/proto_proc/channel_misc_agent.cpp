// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation units covered by this file:
//   src/formmog/tss_sdk/plugins/channel/ipc_channel.cpp
//   src/formmog/tss_sdk/plugins/channel/uic_socket_tcp_handler.cpp
//   src/formmog/tss_sdk/framework/tss_sdk_pkg_packer.cpp
//
// Reconstructed symbols (ranges include alignment padding):
//   CIpcChannel::CIpcChannel() (C1)                        0x000a0f90-0x000a0fef
//   CIpcChannel::CIpcChannel() (C2)                        0x000a0ff0-0x000a104f
//   CIpcChannel::open(unsigned int, unsigned int)          0x000a0ed0-0x000a0f8f
//   CIpcChannel::close()                                   0x000a0ab0-0x000a0ab6
//   CIpcChannel::recv(void**, unsigned int*)               0x000a1050-0x000a1129
//   CIpcChannel::send(void const*, unsigned int)           0x000a1130-0x000a121f
//   TssSdkPkgPacker::get_packed_buf() const                0x00096ce0-0x00096ce7
//   TssSdkPkgPacker::get_packed_buf_size() const           0x00096cf0-0x00096cfe
//   UICSocketChannelTCPHandler::define_comm_head_
//       except_frame_length(Comm_App_Frame*)               0x000a1220-0x000a122e
//   UICSocketChannelTCPHandler::check_if_full_frame(
//       unsigned char*, unsigned int, bool*, int*)         0x000a1640-0x000a16f4
//   UICSocketChannelTCPHandler::~UICSocketChannelTCPHandler()
//       (D0 / D1 / D2)                                     0x000a1700 / 0xa1740 / 0xa1770
//   UICSocketChannelTCPHandler::UICSocketChannelTCPHandler()
//       (C1 / C2)                                          0x000a17a0 / 0xa17e0
//
// TssSdkChannelPlugin::send_pkg_to_{ipc,socket,game}_channel are NOT
// redefined here.  They are already reconstructed with the same semantics in
// recon/src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp:1027-1046 (a
// duplicate strong definition would break the g++ -r link check).
//
// The ZEN/commlib classes below are partial models of external TUs.  Only the
// members and virtual slots touched by the reconstructed methods are modelled;
// every size and member offset is checked against the DWARF layouts.

#include <cstddef>
#include <cstdio>
#include <new>
#include <arpa/inet.h>

#include "src/commlib/framework/comm_socket_channel_base_handler.h"
#include "src/commlib/framework/comm_socket_channel_bus.h"
#include "src/commlib/framework/comm_socket_channel_tcp_handler.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"  // SERVICES_ID, Comm_App_Frame
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/tss_sdk_channel.h"
#include "src/formmog/tss_sdk/layout/zen_bus_pipe.h"

// ORIG static-initialization blocks.  This slice merges three ORIG CUs:
//   ipc_channel.cpp, tss_sdk_pkg_packer.cpp, uic_socket_tcp_handler.cpp
// Each carried an independent block: __ioinit + the 16
// COMM_STAT_ITEM_WITH_NAME entries, nothing else (2 atexit, 1 status dtor,
// _Unwind_Resume).  Emit one real TU-local block per ORIG CU.
#define TSS_SDK_STATIC_INIT_ID ipc_channel
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID tss_sdk_pkg_packer
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID uic_socket_tcp_handler
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

// ---------------------------------------------------------------------------
// Comm_App_Frame and SERVICES_ID are the canonical tag-394 declarations of
// comm_app_frame.h / comm_service_info.h (pulled in by comm_cfgsvr_sdk.h):
//   _APP_FRAME_OPTION 4 (24+8 bitfields), Comm_App_Frame 50 (frame_option_ is
//   an anonymous union at +0x04, frame_appdata_ the flexible array at +0x32),
//   SERVICES_ID 6 packed.
// The local 12-member Comm_App_Frame model this TU used to carry is gone; the
// audit reported it as a second Comm_App_Frame layout in the build.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// uic_socket_tcp_handler.h; sizeof 88, no data members of its own.
// ZEN_Event_Handler / SocketChannelBaseHandler / SocketChannelTCPHandler are
// declared once by the shared commlib headers included above.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

class UICSocketChannelTCPHandler : public SocketChannelTCPHandler {
public:
    UICSocketChannelTCPHandler();
    virtual ~UICSocketChannelTCPHandler();

    virtual int check_if_full_frame(unsigned char *buf, size_t buf_size,
                                    bool *full, int *frame_size);
    virtual void define_comm_head_except_frame_length(Comm_App_Frame *frame);
    static const int FRAME_LEN = 26;
};

}  // namespace tss_sdk_impl

namespace tss_sdk_impl {

// framework/tss_sdk_pkg_packer.h; sizeof 65540.
class TssSdkPkgPacker {
public:
    const char *get_packed_buf() const;
    size_t get_packed_buf_size() const;

    // Private in the shipped header; public for the offset checks.
    char buf_[65535];               // +0x00000
    size_t used_buf_size_;          // +0x10000
};


// ---------------------------------------------------------------------------
// CIpcChannel (ipc_channel.cpp)
// ---------------------------------------------------------------------------

// 0x000a0f90 / 0x000a0ff0.  Both constructors are identical: construct the
// mutex with a NULL debug name, then the two-way bus pipe.  recv_buf_ is left
// uninitialised.  No vptr and no base class.
CIpcChannel::CIpcChannel()
    : thread_mutex_(NULL), bus_channel_() {}

// 0x000a0ed0.  "<bus name>" is "sec/tss_sdk_bus_%d" in a 4096-byte stack
// buffer; the snprintf limit is 0xfff (the return value is not checked, so a
// truncated name is still handed to initialize).  The two bus sizes are the
// same value, the max node size is 0xffff, creator=true, lock_free=false and
// the share-memory type byte is 2.  Any nonzero initialize() result is logged
// and mapped to -1; 0 is returned unchanged.
int CIpcChannel::open(unsigned int bus_channel_id, unsigned int ipc_bus_size) {
    char bus_name[4096];

    snprintf(bus_name, 0xfff, "sec/tss_sdk_bus_%d", bus_channel_id);

    int rc = bus_channel_.initialize(bus_name, ipc_bus_size, ipc_bus_size,
                                     0xffff, true, false,
                                     (unsigned char)2);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex("bus init fail. ret=%d", rc);
        return -1;
    }

    return 0;
}

// 0x000a0ab0.  Unconditional success; it does not touch the mutex or the bus.
int CIpcChannel::close() {
    return 0;
}
// layout/zen_bus_pipe.h keeps bus_pipe_pointer_ private, while the ORIG class
// reaches it through the inline pop_front_bus/push_back_bus wrappers recorded
// in the ipc_channel.cpp DWARF.  Index the array through the DWARF offset that
// the canonical header itself asserts (8204 = 0x200c).
static ZEN_LIB::shm_dequechunk *bus_pipe_at(ZEN_BusPipe_TwoWay &bus,
                                            unsigned int index) {
    ZEN_LIB::shm_dequechunk **pipes =
        reinterpret_cast<ZEN_LIB::shm_dequechunk **>(
            reinterpret_cast<char *>(&bus) + 0x200c);
    return pipes[index];
}


// 0x000a1050.  The deque node is copied into recv_buf_; the caller receives
// the node base pointer and the node's 32-bit size field, not payload+4.
// Returns -1 for a NULL out parameter or an empty receive pipe, 0 otherwise.
int CIpcChannel::recv(void **recv_buf, unsigned int *recv_buf_size) {
    if (recv_buf == NULL || recv_buf_size == NULL)
        return -1;

    *recv_buf = NULL;
    *recv_buf_size = 0xffff;

    thread_mutex_.lock();

    ZEN_LIB::shm_dequechunk *recv_pipe = bus_pipe_at(bus_channel_, 0);
    if (recv_pipe->empty()) {
        thread_mutex_.unlock();
        return -1;
    }

    recv_pipe->pop_front(
        reinterpret_cast<ZEN_LIB::dequechunk_node *>(recv_buf_));

    *recv_buf = recv_buf_;
    *recv_buf_size = *reinterpret_cast<unsigned int *>(recv_buf_);

    thread_mutex_.unlock();
    return 0;
}

// 0x000a1130.  A node larger than 0xffff is rejected before locking.  The
// node is pushed unchanged to send pipe index 1.  On push failure the alert
// logs the constant pipe id 1, the node's first word and the pipe free size;
// the failure return is -1.  The mutex is always released on the push path.
int CIpcChannel::send(const void *send_buf, unsigned int send_buf_size) {
    if (send_buf_size > 0xffff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "send buf to bus fail. buf too big. buf_size=%u", send_buf_size);
        return -1;
    }

    thread_mutex_.lock();

    int rc = 0;
    ZEN_LIB::shm_dequechunk *send_pipe = bus_pipe_at(bus_channel_, 1);
    if (!send_pipe->push_end(
            reinterpret_cast<const ZEN_LIB::dequechunk_node *>(send_buf))) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] %u Pipe is full or data small?,Some data can't put to "
            "pipe. Please increase and check. nodesize=%u, freesize=%u",
            1, *reinterpret_cast<const unsigned int *>(send_buf),
            send_pipe->freesize());
        rc = -1;
    }

    thread_mutex_.unlock();
    return rc;
}

// ---------------------------------------------------------------------------
// TssSdkPkgPacker (tss_sdk_pkg_packer.cpp)
// ---------------------------------------------------------------------------

// 0x00096ce0.  The packed buffer is the first member, so the shipped code
// simply returns this.
const char *TssSdkPkgPacker::get_packed_buf() const {
    return buf_;
}

// 0x00096cf0.  The used size lives at +0x10000 (one byte of tail padding
// follows the 65535-byte buffer).
size_t TssSdkPkgPacker::get_packed_buf_size() const {
    return used_buf_size_;
}

// ---------------------------------------------------------------------------
// UICSocketChannelTCPHandler (uic_socket_tcp_handler.cpp)
// ---------------------------------------------------------------------------

// 0x000a17a0 / 0x000a17e0.  Only the base constructor runs and the derived
// vtable pointer is installed; the class adds no data members.
UICSocketChannelTCPHandler::UICSocketChannelTCPHandler()
    : SocketChannelTCPHandler() {}

// 0x000a1700 (D0, deleting), 0x000a1740 (D1), 0x000a1770 (D2).  All three
// install the derived vtable and call the base destructor; only D0 frees.
UICSocketChannelTCPHandler::~UICSocketChannelTCPHandler() {}

// 0x000a1220.  Writes the UIC command id 300015 (0x493ef) into the synthetic
// Comm_App_Frame header.  frame_length_ and the payload are filled by the
// caller (SocketChannelTCPHandler::handle_input).
void UICSocketChannelTCPHandler::define_comm_head_except_frame_length(
    Comm_App_Frame *frame) {
    frame->frame_command_ = 0x493ef;
}

// 0x000a1640.  The UIC TCP stream has a fixed 26-byte frame:
//   len <= 9                      -> "no complete head",  *full=false, 0
//   ntohs(u16 at buf+4) != 26     -> "bodyLen error...", -1
//   len < 26                      -> "no complete body",  *full=false, 0
//   otherwise                     -> *full=true, *frame_size=26, log, 0
// The length at buf+4 is compared before the body-length check, so a short
// buffer (>=10) with a nonzero 16-bit type is treated as a fatal protocol
// error rather than an incomplete frame.
int UICSocketChannelTCPHandler::check_if_full_frame(unsigned char *buf,
                                                    size_t buf_size,
                                                    bool *full,
                                                    int *frame_size) {
    if (buf_size <= 9) {
        ZEN_Trace_LogMsg::debug_traceex("no complete head");
        *full = false;
        return 0;
    }

    unsigned short body_len = ntohs(
        *reinterpret_cast<unsigned short *>(buf + 4));
    if (body_len != (unsigned short)FRAME_LEN) {
        ZEN_Trace_LogMsg::debug_errorex("bodyLen error, close connection");
        return -1;
    }

    if (buf_size < (unsigned int)FRAME_LEN) {
        ZEN_Trace_LogMsg::debug_traceex("no complete body");
        *full = false;
        return 0;
    }

    *full = true;
    *frame_size = FRAME_LEN;
    ZEN_Trace_LogMsg::debug_debugEx(
        "find a complete package, total len: %d", FRAME_LEN);
    return 0;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Compile-time layout checks against the DWARF layouts.
// ---------------------------------------------------------------------------

typedef char channel_misc_layout_check
    [(sizeof(ZEN_LIB::dequechunk_node) == 4 &&
      sizeof(ZEN_Thread_Mutex) == 28 &&
      sizeof(ZEN_BusPipe_TwoWay) == 10292 &&
      sizeof(Comm_App_Frame) == 50 &&
      sizeof(SERVICES_ID) == 6 &&
      sizeof(ZEN_Event_Handler) == 12 &&
      sizeof(SocketChannelBaseHandler) == 52 &&
      sizeof(SocketChannelTCPHandler) == 88 &&
      sizeof(tss_sdk_impl::CIpcChannel) == 75856 &&
      sizeof(tss_sdk_impl::TssSdkPkgPacker) == 65540 &&
      sizeof(tss_sdk_impl::UICSocketChannelTCPHandler) == 88 &&
      __builtin_offsetof(Comm_App_Frame, frame_option_) == 0x04 &&
      __builtin_offsetof(Comm_App_Frame, frame_command_) == 0x08 &&
      __builtin_offsetof(Comm_App_Frame, frame_command_) == 0x08 &&
      __builtin_offsetof(tss_sdk_impl::TssSdkPkgPacker, used_buf_size_) == 0x10000)
         ? 1
         : -1];
