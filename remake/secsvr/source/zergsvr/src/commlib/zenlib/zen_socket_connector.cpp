// 还原自 gunnersvr 二进制（DWARF 行号与 zen_socket_connector.cpp 对应）
// ZEN_Socket_Connector：非阻塞/超时连接器。

#include "zen_socket_connector.h"
#include "zen_os_adapt_socket.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

// line 15
ZEN_Socket_Connector::ZEN_Socket_Connector() {
}

// line 19
ZEN_Socket_Connector::~ZEN_Socket_Connector() {
}

// line 49
int ZEN_Socket_Connector::connect(ZEN_Socket_Stream &new_stream, const ZEN_Sockaddr *remote_addr,
                                  ZEN_Time_Value &timeout, bool reuse_addr, int protocol,
                                  const ZEN_Sockaddr *local_addr) {
    int ret;
    const int HANDLE_READY_ONE = 1;
    int last_err;

    errno = 0;
    if (new_stream.get_handle() == -1) {
        if (local_addr == NULL) {
            ret = new_stream.open(remote_addr->get_addr()->sa_family, protocol, reuse_addr);
        } else {
            ret = new_stream.open(local_addr, local_addr->get_addr()->sa_family, protocol, reuse_addr);
        }
        if (ret != 0) {
            return ret;
        }
    }

    ret = new_stream.sock_enable(O_NONBLOCK);
    if (ret != 0) {
        new_stream.close();
        return ret;
    }
    ret = ::connect(new_stream.get_handle(), remote_addr->sockaddr_ptr_, remote_addr->sockaddr_size_);
    if (ret != 0) {
        last_err = errno;
        if (last_err != EINPROGRESS && last_err != EAGAIN) {
            new_stream.close();
            return ret;
        }
    }
    if (ZEN_OS::handle_ready(new_stream.get_handle(), &timeout, ZEN_OS::HANDLE_READY_CONNECT_SUCCESS)
        == HANDLE_READY_ONE) {
        ret = new_stream.sock_disable(O_NONBLOCK);
        if (ret == 0) {
            return ret;
        }
    }
    new_stream.close();
    return -1;
}

// line 156
int ZEN_Socket_Connector::connect(ZEN_Socket_Stream &new_stream, const ZEN_Sockaddr *remote_addr,
                                  bool non_blocing, bool reuse_addr, int protocol,
                                  const ZEN_Sockaddr *local_addr) {
    int ret;
    int last_error;

    errno = 0;
    if (new_stream.get_handle() == -1) {
        ret = new_stream.open(local_addr, remote_addr->get_addr()->sa_family, protocol, reuse_addr);
        if (ret != 0) {
            return ret;
        }
    }

    if (non_blocing) {
        ret = new_stream.sock_enable(O_NONBLOCK);
        if (ret != 0) {
            new_stream.close();
            return ret;
        }
    }
    ret = ::connect(new_stream.get_handle(), remote_addr->sockaddr_ptr_, remote_addr->sockaddr_size_);
    if (ret != 0) {
        last_error = errno;
        // 原二进制怪癖：非阻塞模式遇到 EINPROGRESS/EAGAIN 直接返回 -1 且不关闭句柄
        if (non_blocing && (last_error == EINPROGRESS || last_error == EAGAIN)) {
            return -1;
        }
        new_stream.close();
        return ret;
    }
    return ret;
}
