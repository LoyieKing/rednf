// Restored from zergsvr binary (DWARF CU zen_socket_acceptor.cpp).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_socket_acceptor.cpp

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_os_adapt_socket.h"
#include "src/commlib/zenlib/zen_os_adapt_error.h"
#include "src/commlib/zenlib/zen_socket_addr_base.h"
#include "src/commlib/zenlib/zen_socket_stream.h"
#include "src/commlib/zenlib/zen_socket_acceptor.h"
#include <iostream>
#include <sys/socket.h>

ZEN_Socket_Acceptor::ZEN_Socket_Acceptor() {
}

ZEN_Socket_Acceptor::~ZEN_Socket_Acceptor() {
    this->close();
}

int ZEN_Socket_Acceptor::open(const ZEN_Sockaddr *local_addr, bool reuse_addr, int protocol_family, int backlog, int protocol) {
    int ret;
    if (protocol_family == 0) {
        protocol_family = local_addr->get_addr()->sa_family;
    }
    ret = ZEN_Socket_Base::open(1 /*SOCK_STREAM*/, protocol_family, protocol, reuse_addr);
    if (ret == 0) {
        ret = this->bind(local_addr);
        if (ret == 0) {
            ret = listen(this->socket_handle_, backlog);
        }
        if (ret != 0) {
            ::close(this->socket_handle_);
        }
    }
    return ret;
}

int ZEN_Socket_Acceptor::accept(ZEN_Socket_Stream &new_stream, ZEN_Sockaddr *remote_addr) const {
    ZEN_SOCKET sock_handle;
    sock_handle = ::accept(this->socket_handle_, remote_addr->get_addr(), &remote_addr->sockaddr_size_);
    if (sock_handle == -1) {
        return -1;
    }
    new_stream.set_handle(sock_handle);
    return 0;
}

int ZEN_Socket_Acceptor::accept(ZEN_Socket_Stream &new_stream, ZEN_Time_Value &timeout, ZEN_Sockaddr *remote_addr) const {
    int ret;
    const int HANDLE_READY_ONE = 1;
    ZEN_SOCKET sock_handle;
    ret = ZEN_OS::handle_ready(this->socket_handle_, &timeout, ZEN_OS::HANDLE_READY_ACCEPT);
    if (ret != HANDLE_READY_ONE) {
        return -1;
    }
    sock_handle = ::accept(this->socket_handle_, remote_addr->get_addr(), &remote_addr->sockaddr_size_);
    if (sock_handle == -1) {
        return -1;
    }
    new_stream.set_handle(sock_handle);
    return 0;
}
