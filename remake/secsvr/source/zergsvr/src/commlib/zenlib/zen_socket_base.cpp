// 还原自 gunnersvr 二进制（DWARF 行号与 zen_socket_base.cpp 对应）
// ZEN_Socket_Base：POSIX socket 描述符基础封装。

#include "zen_socket_base.h"

#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>

namespace ZEN_OS {
    // 声明见 zen_os_adapt_socket.h（该头文件尚为未修复的 DWARF 桩，故本地声明）
    int sock_enable(int handle, int value);
    int sock_disable(int handle, int value);
}

// line 9
ZEN_Socket_Base::ZEN_Socket_Base() {
    socket_handle_ = -1;
}

// line 14
ZEN_Socket_Base::ZEN_Socket_Base(const ZEN_SOCKET &arg0) {
    socket_handle_ = arg0;
}

// line 19
ZEN_Socket_Base::~ZEN_Socket_Base() {
}

// line 24
void ZEN_Socket_Base::set_handle(const ZEN_SOCKET &socket_handle) {
    socket_handle_ = socket_handle;
}

// line 30
ZEN_SOCKET ZEN_Socket_Base::get_handle() const {
    return socket_handle_;
}

// line 39
int ZEN_Socket_Base::open(int type, int protocol_family, int protocol, bool reuse_addr) {
    int ret;
    int one = 1;

    socket_handle_ = ::socket(protocol_family, type, protocol);
    if (socket_handle_ == -1) {
        return -1;
    }
    if (reuse_addr) {
        ret = ::setsockopt(socket_handle_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (ret != 0) {
            ::close(socket_handle_);
            return -1;
        }
    }
    return 0;
}

// line 78
// 原二进制怪癖（保持可观测语义一致）：
//  - local_addr == NULL 时调用 4 参 open 后固定返回 0（丢弃其返回值）；
//  - protocol_family != 0 且与 local_addr 地址族不一致时返回 -1；
//  - 其余情况调用 4 参 open（丢弃返回值）后执行 bind，bind 失败才 close。
int ZEN_Socket_Base::open(int type, const ZEN_Sockaddr *local_addr, int protocol_family, int protocol, bool reuse_addr) {
    int ret;

    if (local_addr == NULL) {
        open(type, protocol_family, protocol, reuse_addr);
        return 0;
    }
    if (protocol_family != 0 && local_addr->get_addr()->sa_family != protocol_family) {
        return -1;
    }
    open(type, local_addr->get_addr()->sa_family, protocol, reuse_addr);
    ret = bind(local_addr);
    if (ret != 0) {
        close();
    }
    return ret;
}

// line 117
int ZEN_Socket_Base::close() {
    int ret;

    ret = ::close(socket_handle_);
    if (ret == 0) {
        socket_handle_ = -1;
    }
    return ret;
}

// line 130
int ZEN_Socket_Base::bind(const ZEN_Sockaddr *add_name) const {
    return ::bind(socket_handle_, add_name->sockaddr_ptr_, add_name->sockaddr_size_);
}

// line 137
int ZEN_Socket_Base::sock_enable(int value) const {
    return ZEN_OS::sock_enable(socket_handle_, value);
}

// line 143
int ZEN_Socket_Base::sock_disable(int value) const {
    return ZEN_OS::sock_disable(socket_handle_, value);
}

// line 152
int ZEN_Socket_Base::getsockopt(int level, int optname, void *optval, socklen_t *optlen) const {
    return ::getsockopt(socket_handle_, level, optname, optval, optlen);
}

// line 161
int ZEN_Socket_Base::setsockopt(int level, int optname, const void *optval, int optlen) const {
    return ::setsockopt(socket_handle_, level, optname, optval, optlen);
}

// line 172
int ZEN_Socket_Base::getpeername(ZEN_Sockaddr *addr) const {
    return ::getpeername(socket_handle_, addr->sockaddr_ptr_, &addr->sockaddr_size_);
}

// line 181
int ZEN_Socket_Base::getsockname(ZEN_Sockaddr *addr) const {
    return ::getsockname(socket_handle_, addr->sockaddr_ptr_, &addr->sockaddr_size_);
}

// line 189
int ZEN_Socket_Base::connect(const ZEN_Sockaddr *addr) const {
    return ::connect(socket_handle_, addr->sockaddr_ptr_, addr->sockaddr_size_);
}

// line 200
ssize_t ZEN_Socket_Base::recv(void *buf, size_t len, int flags) const {
    return ::recv(socket_handle_, buf, len, flags);
}

// line 211
ssize_t ZEN_Socket_Base::send(const void *buf, size_t len, int flags) const {
    return ::send(socket_handle_, buf, len, flags);
}
