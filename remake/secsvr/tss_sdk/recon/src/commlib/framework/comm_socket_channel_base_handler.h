// SocketChannelBaseHandler, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_base_handler.cpp
//
// ORIG DWARF layout:
//   ZEN_Event_Handler          12  vptr, zen_reactor_ @4, event_mask_ @8
//   SocketChannelBaseHandler   52  +0x0c business_id_
//                                  +0x10 sndrcv_bus_
//                                  +0x14 ZEN_Sockaddr_In remote_addr_ (28)
//                                  +0x30 ZEN_Socket_Connector (1)
// Virtual order verified from the ORIG vtable at 0x2938c8.
#ifndef TSS_RECON_COMM_SOCKET_CHANNEL_BASE_HANDLER_H_
#define TSS_RECON_COMM_SOCKET_CHANNEL_BASE_HANDLER_H_

#include <stddef.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_socket_addr_in.h"
#include "src/commlib/zenlib/zen_socket_stream.h"
#include "src/commlib/zenlib/zen_socket_connector.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"

namespace ZEN_OS {
const char *socketaddr_ntop(const sockaddr *addr, char *buffer,
                            unsigned int buf_len);
}

class SocketChannelBaseHandler;
class SocketChannelBus;

class ZEN_Reactor;

struct SocketChannelInitData {
    int business_id_;                    // +0x00
    const char *ip_;                     // +0x04
    unsigned short port_;                // +0x08
    SocketChannelBaseHandler *handler_;  // +0x0c
    SocketChannelInitData();
};

// ZEN_Sockaddr_In, ZEN_Sockaddr, ZEN_Socket_Base, ZEN_Socket_Stream,
// ZEN_Socket_Connector and ZEN_Event_Handler are declared by the secagent
// zenlib headers included above; each matches the ORIG tag-394 DWARF, so no
// local model is kept here.

class SocketChannelBaseHandler : public ZEN_Event_Handler,
                               public ZEN_NON_Copyable {
public:
    SocketChannelBaseHandler();
    virtual ~SocketChannelBaseHandler();

    virtual bool is_valid() = 0;
    virtual bool is_idle() = 0;
    virtual int send(unsigned char *data, size_t len) = 0;
    virtual int init_connect(SocketChannelInitData &init_data,
                             SocketChannelBus *bus) = 0;
    virtual int connect_server() = 0;

    int get_business_id() { return business_id_; }
    void set_business_id(int business_id) { business_id_ = business_id; }
    SocketChannelBus *get_sndrcv_bus() { return sndrcv_bus_; }
    void set_sndrcv_bus(SocketChannelBus *bus);

protected:
    int business_id_;                  // +0x0c
    SocketChannelBus *sndrcv_bus_;     // +0x10
    ZEN_Sockaddr_In remote_addr_;      // +0x14
    ZEN_Socket_Connector socket_connector_; // +0x30
};

#endif // TSS_RECON_COMM_SOCKET_CHANNEL_BASE_HANDLER_H_
