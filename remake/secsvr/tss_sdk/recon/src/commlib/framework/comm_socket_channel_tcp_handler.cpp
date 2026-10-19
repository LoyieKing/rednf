// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_tcp_handler.cpp
// Address range 0x000d91a0-0x000d9f8c.
#include "src/commlib/framework/comm_socket_channel_tcp_handler.h"

#include <arpa/inet.h>
#include <errno.h>
#include <new>
#include <string.h>

#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/commlib/framework/comm_socket_channel.h"

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace ZEN_OS {
const char *socketaddr_ntop(const sockaddr *addr, char *buffer,
                            unsigned int buf_len);
}

namespace {

const char *const TCP_SEND_BUF_NOT_EMPTY =
    "[socket channel tcp handler]send_buf_len_ != 0";
const char *const TCP_SEND_TOO_LARGE =
    "[socket channel tcp handler]len >= (size_t)BUF_MAX_SIZE";
const char *const TCP_CLOSE_SOCKET =
    "SocketChannelTCPHandler close_socket socket: %d";
const char *const TCP_REGISTER_FAILED =
    "[socket channel tcp handler]reactor()->register_handler failed, ret = %d";
const char *const TCP_NEW_SEND_BUF_FAILED =
    "[socket channel tcp handler]new send_buf_ failed";
const char *const TCP_NEW_RECV_BUF_FAILED =
    "[socket channel tcp handler]new recv_buf_ failed";
const char *const TCP_NEW_PACK_BUF_FAILED =
    "[socket channel tcp handler]new pack_buf_ failed";
const char *const TCP_SEND_EINTR =
    "[socket channel tcp handler]sock_.send == EINTR";
const char *const TCP_SEND_EWOULDBLOCK =
    "[socket channel tcp handler]ZEN_OS::last_error() != EWOULDBLOCK";
const char *const TCP_CONNECT_SUCCESS =
    "[socket channel tcp handler]My God! NonBlock Socket Connect Success, "
    "ACE is a cheat";
const char *const TCP_CONNECT_FAILED =
    "[socket channel tcp handler]socket_connector_.connect failed, ret = %d";
const char *const TCP_INIT_TCP_FAILED =
    "[socket channel tcp handler]init_tcp_handler failed, ret = %d";
const char *const TCP_RECV_EINTR =
    "[socket channel tcp handler]sock_.recv == EINTR";
const char *const TCP_SOCKET_CLOSED =
    "[socket channel tcp handler]socket closed, business_id: %d, ip: %s, "
    "port: %d";
const char *const TCP_RECV_ERROR =
    "[socket channel tcp handler]sock_.recv error, recvret = %d";
const char *const TCP_PUSH_RECV_FAILED =
    "[socket channel tcp handler]sndrcv_buf_->push_recv_buf failed";
const char *const TCP_CHECK_FRAME_ERROR =
    "[socket channel tcp handler]CheckNextFullFrame error, ret = %d";
const char *const TCP_CONNECTED =
    "[socket channel tcp handler]connect to %s|%d success";
const char *const TCP_SEND_BUF_TO_NET_FAILED =
    "[socket channel tcp handler]send_buf_to_net failed, ret = %d";

const size_t TCP_BUF_SIZE = 0x10000;
const size_t TCP_PACK_BUF_SIZE = 0x10032;
const unsigned int TCP_BUF_MAX_SIZE = 0xffff;

} // namespace

// ---------------------------------------------------------------------------
// SocketChannelTCPHandler
// ---------------------------------------------------------------------------

SocketChannelTCPHandler::SocketChannelTCPHandler()
    : sock_(),
      socket_status_(0),
      send_buf_(NULL),
      send_buf_len_(0),
      next_send_pos_(0),
      recv_buf_(NULL),
      pack_buf_(NULL),
      nextRecvStart(0),
      nextReadStart(0) {
    int invalid_handle = -1;
    sock_.set_handle(invalid_handle);
}

SocketChannelTCPHandler::~SocketChannelTCPHandler() {
    disconnect_fini();
}

void SocketChannelTCPHandler::reset() {
    socket_status_ = 0;
    nextRecvStart = 0;
    send_buf_len_ = 0;
    nextReadStart = 0;
    next_send_pos_ = 0;
}

bool SocketChannelTCPHandler::is_valid() {
    return socket_status_ == 2;
}

bool SocketChannelTCPHandler::is_idle() {
    return socket_status_ == 2 && send_buf_len_ == 0;
}

int SocketChannelTCPHandler::process_connect_register() {
    socket_status_ = 2;
    reactor()->cancel_wakeup(this, 0x10);
    reactor()->schedule_wakeup(this, 0x2);
    return 0;
}

int SocketChannelTCPHandler::handle_timeout(const ZEN_Time_Value &,
                                            const void *) {
    return 0;
}

int SocketChannelTCPHandler::send(unsigned char *buf, size_t len) {
    if (len == 0) {
        return 0;
    }
    if (send_buf_len_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_SEND_BUF_NOT_EMPTY);
        return 1;
    }
    if (len > TCP_BUF_MAX_SIZE) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_SEND_TOO_LARGE);
        return -1;
    }
    memcpy(send_buf_, buf, len);
    send_buf_len_ = len;
    next_send_pos_ = 0;
    reactor()->schedule_wakeup(this, 4);
    return 0;
}

ZEN_SOCKET SocketChannelTCPHandler::get_handle() const {
    return sock_.get_handle();
}

int SocketChannelTCPHandler::close_socket() {
    if (sock_.get_handle() != -1) {
        ZEN_Trace_LogMsg::debug_infoex(TCP_CLOSE_SOCKET, sock_.get_handle());
        ZEN_Event_Handler::handle_close();
        sock_.close();
    }
    reset();
    return 0;
}

int SocketChannelTCPHandler::handle_close() {
    close_socket();
    return 0;
}

int SocketChannelTCPHandler::init_tcp_handler(
    const ZEN_Socket_Stream &sock_stream) {
    sock_ = sock_stream;
    sock_.sock_enable(0x800);
    int ret = reactor()->register_handler(this, 0x10);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_REGISTER_FAILED, ret);
        close_socket();
        return -1;
    }
    int keep_alive = 1;
    sock_.setsockopt(1, 9, &keep_alive, 4);
    int nodelay = 1;
    sock_.setsockopt(6, 1, &nodelay, 4);
    return 0;
}

int SocketChannelTCPHandler::disconnect_fini() {
    close_socket();
    if (send_buf_ != NULL) {
        delete[] send_buf_;
        send_buf_ = NULL;
    }
    if (recv_buf_ != NULL) {
        delete[] recv_buf_;
        recv_buf_ = NULL;
    }
    if (pack_buf_ != NULL) {
        delete[] pack_buf_;
        pack_buf_ = NULL;
    }
    return 0;
}

int SocketChannelTCPHandler::init_connect(SocketChannelInitData &init_data,
                                          SocketChannelBus *sndrcv_bus) {
    int ret = -1;
    if (sndrcv_bus == NULL) {
        return ret;
    }
    remote_addr_.set(init_data.ip_, init_data.port_);
    business_id_ = init_data.business_id_;
    send_buf_ = new (std::nothrow) unsigned char[TCP_BUF_SIZE];
    if (send_buf_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_NEW_SEND_BUF_FAILED);
        return -1;
    }
    recv_buf_ = new (std::nothrow) unsigned char[TCP_BUF_SIZE];
    if (recv_buf_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_NEW_RECV_BUF_FAILED);
        return -1;
    }
    pack_buf_ = new (std::nothrow) unsigned char[TCP_PACK_BUF_SIZE];
    if (pack_buf_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_NEW_PACK_BUF_FAILED);
        return -1;
    }
    set_sndrcv_bus(sndrcv_bus);
    return connect_server();
}

int SocketChannelTCPHandler::send_buf_to_net() {
    while (send_buf_len_ != 0) {
        if (next_send_pos_ >= send_buf_len_) {
            break;
        }
        int sendret = sock_.send(send_buf_ + next_send_pos_,
                                 send_buf_len_ - next_send_pos_, 0);
        if (sendret == 0) {
            if (errno == EINTR) {
                ZEN_Trace_LogMsg::debug_errorex(TCP_SEND_EINTR);
                continue;
            }
            if (errno == EAGAIN) {
                return 0;
            }
            ZEN_Trace_LogMsg::debug_errorex(TCP_SEND_EWOULDBLOCK);
            return -1;
        }
        next_send_pos_ += sendret;
    }
    if (next_send_pos_ < send_buf_len_) {
        return 0;
    }
    reactor()->remove_handler(this, 4);
    send_buf_len_ = 0;
    next_send_pos_ = 0;
    return 0;
}

int SocketChannelTCPHandler::connect_server() {
    int ret = 0;
    if (socket_status_ != 0) {
        return ret;
    }
    close_socket();
    ZEN_Socket_Stream socket;
    socket.open(2, 0, true);
    int opval = 0x50000;
    socket.setsockopt(1, 8, &opval, 4);
    socket.setsockopt(1, 7, &opval, 4);
    ret = socket_connector_.connect(socket, (const ZEN_Sockaddr *)&remote_addr_,
                                    true, true, 0, NULL);
    if (ret >= 0) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_CONNECT_SUCCESS);
        socket_status_ = 1;
        return 0;
    }
    if (errno == EINPROGRESS || errno == EINTR) {
        if (init_tcp_handler(socket) == 0) {
            ZEN_Trace_LogMsg::debug_errorex(TCP_CONNECT_SUCCESS);
            socket_status_ = 1;
            return 0;
        }
        ZEN_Trace_LogMsg::debug_errorex(TCP_INIT_TCP_FAILED, ret);
        return -2;
    }
    ZEN_Trace_LogMsg::debug_errorex(TCP_CONNECT_FAILED, ret);
    socket.close();
    return -1;
}

int SocketChannelTCPHandler::read_data_from_tcp() {
    int recvret = sock_.recv(recv_buf_ + nextRecvStart,
                             TCP_BUF_SIZE - nextRecvStart, 0);
    if (recvret > 0) {
        nextRecvStart += recvret;
        return 0;
    }
    if (recvret == 0) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_SOCKET_CLOSED, business_id_,
                                        remote_addr_.get_host_addr(),
                                        remote_addr_.get_port_number());
        return -1;
    }
    if (errno == EAGAIN) {
        return 0;
    }
    if (errno == EINTR) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_RECV_EINTR);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex(TCP_RECV_ERROR, recvret);
    return -1;
}

int SocketChannelTCPHandler::handle_input() {
    if (read_data_from_tcp() != 0) {
        return -1;
    }
    for (;;) {
        bool bfull = false;
        int frameLen = -1;
        int ret = check_if_full_frame(recv_buf_ + nextRecvStart,
                                      nextReadStart - nextRecvStart, &bfull,
                                      &frameLen);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(TCP_CHECK_FRAME_ERROR, ret);
            return -1;
        }
        if (!bfull) {
            if (nextReadStart == nextRecvStart || nextRecvStart == 0) {
                nextReadStart -= nextRecvStart;
                nextRecvStart = 0;
                return 0;
            }
            memmove(recv_buf_, recv_buf_ + nextRecvStart,
                    nextReadStart - nextRecvStart);
            nextReadStart -= nextRecvStart;
            nextRecvStart = 0;
            return 0;
        }
        Comm_App_Frame *head = (Comm_App_Frame *)pack_buf_;
        memcpy(pack_buf_ + 0x32, recv_buf_ + nextRecvStart, frameLen);
        nextRecvStart += frameLen;
        *(int *)pack_buf_ = frameLen + 0x32;
        define_comm_head_except_frame_length(head);
        if (sndrcv_bus_->push_recv_bus(pack_buf_) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(TCP_PUSH_RECV_FAILED);
        }
    }
}

int SocketChannelTCPHandler::handle_output() {
    if (socket_status_ == 1) {
        const char *ip = remote_addr_.get_host_addr();
        int port = remote_addr_.get_port_number();
        ZEN_Trace_LogMsg::debug_infoex(TCP_CONNECTED, ip, port);
        process_connect_register();
        return 0;
    }
    int ret = send_buf_to_net();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(TCP_SEND_BUF_TO_NET_FAILED, ret);
        return -1;
    }
    return 0;
}
