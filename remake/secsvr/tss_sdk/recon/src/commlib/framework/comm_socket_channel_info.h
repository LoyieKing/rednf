// SocketChannelBusinessInfo, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_info.cpp
//
// ORIG DWARF: sizeof(SocketChannelBusinessInfo) == 52
//   +0x00 int business_id_          (ctor writes -1)
//   +0x04 SocketChannelBus *bus_
//   +0x08 std::vector<SocketChannelBaseHandler*> *handler_ary_
//   +0x0c ZEN_Thread_RW_Mutex lock_
// Not polymorphic (only D1/D2 are emitted).
#ifndef TSS_RECON_COMM_SOCKET_CHANNEL_INFO_H_
#define TSS_RECON_COMM_SOCKET_CHANNEL_INFO_H_

#include <vector>

#include "src/commlib/framework/comm_socket_channel_base_handler.h"
#include "src/commlib/framework/comm_socket_channel_bus.h"
#include "src/commlib/zenlib/zen_lock_thread_rw_mutex.h"

// ORIG DWARF: SocketChannelBusinessInfo size 48, NO base class; members
// business_id_ @0, sndrcv_bus_ @4, handlers_vector_ @8,
// handlers_vector_rwlock_ @12.
class SocketChannelBusinessInfo {
    friend class SocketChannel;

public:
    SocketChannelBusinessInfo();
    ~SocketChannelBusinessInfo();

    int init();
    int fini();
    int add_handler(SocketChannelBaseHandler *handler);
    SocketChannelBaseHandler *get_first_valid_handler();

protected:
    int business_id_;                    // +0x00
    SocketChannelBus *bus_;              // +0x04
    std::vector<SocketChannelBaseHandler *> *handler_ary_; // +0x08
    ZEN_Thread_RW_Mutex lock_;           // +0x0c
};

#endif // TSS_RECON_COMM_SOCKET_CHANNEL_INFO_H_
