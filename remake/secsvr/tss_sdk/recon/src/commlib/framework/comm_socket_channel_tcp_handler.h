// SocketChannelTCPHandler, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_tcp_handler.cpp
//
// ORIG DWARF: sizeof 88
//   +0x00 SocketChannelBaseHandler (52)
//   +0x34 ZEN_Socket_Stream sock_ (4)
//   +0x38 volatile int    socket_status_   0 invalid, 1 connecting, 2 connected
//   +0x3c unsigned char*  send_buf_
//   +0x40 volatile size_t send_buf_len_
//   +0x44 volatile size_t next_send_pos_
//   +0x48 unsigned char*  recv_buf_
//   +0x4c unsigned char*  pack_buf_
//   +0x50 int             nextRecvStart
//   +0x54 int             nextReadStart
#ifndef TSS_RECON_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_
#define TSS_RECON_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_

#include <stddef.h>
#include <sys/socket.h>

#include "src/commlib/framework/comm_socket_channel_base_handler.h"

// ZEN_SOCKET/ZEN_HANDLE/ZEN_THREAD_ID (zen_os_adapt_predefine.h),
// ZEN_Socket_Base and ZEN_Socket_Stream (zen_socket_stream.h) come from the
// secagent zenlib headers (included above) and match the ORIG tag-394 DWARF:
//   ZEN_Socket_Base   size 4 = ZEN_SOCKET socket_handle_ @0
//   ZEN_Socket_Stream size 4 = ZEN_Socket_Base
struct Comm_App_Frame;

class SocketChannelTCPHandler : public SocketChannelBaseHandler {
public:
    SocketChannelTCPHandler();
    virtual ~SocketChannelTCPHandler();

    virtual ZEN_SOCKET get_handle() const;
    virtual int handle_input();
    virtual int handle_output();
    virtual int handle_close();
    virtual bool is_valid();
    virtual bool is_idle();
    virtual int send(unsigned char *buf, size_t len);
    virtual int init_connect(SocketChannelInitData &init_data,
                             SocketChannelBus *sndrcv_bus);
    virtual int connect_server();
    virtual int handle_timeout(const ZEN_Time_Value &time, const void *arg);
    virtual int check_if_full_frame(unsigned char *buf, size_t len,
                                    bool *bfull, int *frameLen) = 0;
    virtual void define_comm_head_except_frame_length(
        Comm_App_Frame *frame) = 0;

    void reset();
    int process_connect_register();
    int close_socket();
    int init_tcp_handler(const ZEN_Socket_Stream &sock_stream);
    int disconnect_fini();
    int send_buf_to_net();
    int read_data_from_tcp();

protected:
    ZEN_Socket_Stream sock_;          // +0x34
    volatile int socket_status_;      // +0x38
    unsigned char *send_buf_;         // +0x3c
    volatile size_t send_buf_len_;    // +0x40
    volatile size_t next_send_pos_;   // +0x44
    unsigned char *recv_buf_;         // +0x48
    unsigned char *pack_buf_;         // +0x4c
    int nextRecvStart;                // +0x50
    int nextReadStart;                // +0x54
};

#endif // TSS_RECON_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_
