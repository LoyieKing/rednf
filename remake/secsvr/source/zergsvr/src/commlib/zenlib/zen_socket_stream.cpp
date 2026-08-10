// 还原自 gunnersvr 二进制（DWARF 行号与 zen_socket_stream.cpp 对应）
// ZEN_Socket_Stream：TCP 流式套接字（SOCK_STREAM 封装）。

#include "zen_socket_stream.h"

#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>

namespace ZEN_OS {
    // 声明见 zen_os_adapt_socket.h（该头文件尚为未修复的 DWARF 桩，故本地声明）
    ssize_t sendn_timeout(int handle, const void *buf, size_t len, ZEN_Time_Value &timeout_ztv, int flags);
    ssize_t recvn_timeout(int handle, void *buf, size_t len, ZEN_Time_Value &timeout_ztv, int flags);
    ssize_t sendn(int handle, const void *buf, size_t len, int flags);
    ssize_t recvn(int handle, void *buf, size_t len, int flags);
}

// line 9
ZEN_Socket_Stream::ZEN_Socket_Stream()
    : ZEN_Socket_Base() {
}

// line 14
ZEN_Socket_Stream::ZEN_Socket_Stream(const ZEN_SOCKET &arg0)
    : ZEN_Socket_Base(arg0) {
}

// line 19
ZEN_Socket_Stream::~ZEN_Socket_Stream() {
}

// line 27
int ZEN_Socket_Stream::open(int protocol_family, int protocol, bool reuse_addr) {
    return ZEN_Socket_Base::open(SOCK_STREAM, protocol_family, protocol, reuse_addr);
}

// line 46
int ZEN_Socket_Stream::open(const ZEN_Sockaddr *local_addr, int protocol_family, int protocol, bool reuse_addr) {
    return ZEN_Socket_Base::open(SOCK_STREAM, local_addr, protocol_family, protocol, reuse_addr);
}

// line 68
int ZEN_Socket_Stream::shutdown(int how) const {
    return ::shutdown(socket_handle_, how);
}

// line 79
ssize_t ZEN_Socket_Stream::recv_n(void *buf, size_t len, ZEN_Time_Value *timeout_tv, int flags) const {
    if (timeout_tv != NULL) {
        return ZEN_OS::recvn_timeout(socket_handle_, buf, len, *timeout_tv, flags);
    }
    return ZEN_OS::recvn(socket_handle_, buf, len, flags);
}

// line 92
ssize_t ZEN_Socket_Stream::send_n(const void *buf, size_t len, ZEN_Time_Value *timeout_tv, int flags) const {
    if (timeout_tv != NULL) {
        return ZEN_OS::sendn_timeout(socket_handle_, buf, len, *timeout_tv, flags);
    }
    return ZEN_OS::sendn(socket_handle_, buf, len, flags);
}

// line 105
ssize_t ZEN_Socket_Stream::recvn_timeout(void *buf, size_t len, ZEN_Time_Value &timeout_tv, int flags) const {
    return ZEN_OS::recvn_timeout(socket_handle_, buf, len, timeout_tv, flags);
}

// line 118
ssize_t ZEN_Socket_Stream::sendn_timeout(void *buf, size_t len, ZEN_Time_Value &timeout_tv, int flags) {
    return ZEN_OS::sendn_timeout(socket_handle_, buf, len, timeout_tv, flags);
}
