// Restored from zergsvr binary (DWARF CU zen_socket_datagram.cpp).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_socket_datagram.cpp

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_os_adapt_socket.h"
#include "src/commlib/zenlib/zen_os_adapt_error.h"
#include "src/commlib/zenlib/zen_socket_addr_base.h"
#include "src/commlib/zenlib/zen_socket_datagram.h"
#include <iostream>
#include <sys/socket.h>

Zen_Socket_DataGram::Zen_Socket_DataGram() : ZEN_Socket_Base() {
}

Zen_Socket_DataGram::Zen_Socket_DataGram(const ZEN_SOCKET &socket_handle)
    : ZEN_Socket_Base(socket_handle) {
}

Zen_Socket_DataGram::~Zen_Socket_DataGram() {
}

int Zen_Socket_DataGram::open(int protocol_family, int protocol, bool reuse_addr) {
    return ZEN_Socket_Base::open(2 /*SOCK_DGRAM*/, protocol_family, protocol, reuse_addr);
}

int Zen_Socket_DataGram::open(const ZEN_Sockaddr *local_addr, int protocol_family, int protocol, bool reuse_addr) {
    return ZEN_Socket_Base::open(2 /*SOCK_DGRAM*/, local_addr, protocol_family, protocol, reuse_addr);
}

ssize_t Zen_Socket_DataGram::recvfrom_timeout(void *buf, size_t len, ZEN_Sockaddr *from_addr, ZEN_Time_Value &timeout_ztv, int flags) const {
    return ZEN_OS::recvfrom_timeout(this->socket_handle_, buf, len, from_addr->get_addr(), &from_addr->sockaddr_size_, timeout_ztv, flags);
}

ssize_t Zen_Socket_DataGram::sendto_timeout(const void *buf, size_t len, const ZEN_Sockaddr *to_addr, ZEN_Time_Value &timeout_ztv, int flags) const {
    return ZEN_OS::sendto_timeout(this->socket_handle_, buf, len, to_addr->get_addr(), to_addr->get_size(), timeout_ztv, flags);
}
