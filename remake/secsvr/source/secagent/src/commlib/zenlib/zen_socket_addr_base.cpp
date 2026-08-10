// 还原自 gunnersvr 二进制（DWARF 行号与 zen_socket_addr_base.cpp 对应）
// ZEN_Sockaddr：地址描述基类（虚基，set_sockaddr 为纯虚函数）。

#include "zen_socket_addr_base.h"

#include <stddef.h>
#include <sys/socket.h>

namespace ZEN_OS {
    // 声明见 zen_os_adapt_socket.h（该头文件尚为未修复的 DWARF 桩，故本地声明）
    const char *socketaddr_ntop_ex(const sockaddr *sockaddr_ptr, char *buffer, size_t buf_len);
}

// line 6
ZEN_Sockaddr::ZEN_Sockaddr(sockaddr *arg0, int arg1) {
    sockaddr_ptr_ = arg0;
    sockaddr_size_ = arg1;
}

// line 14
ZEN_Sockaddr::~ZEN_Sockaddr() {
}

// line 21
bool ZEN_Sockaddr::operator==(const ZEN_Sockaddr &others_sockaddr) const {
    return sockaddr_ptr_ == others_sockaddr.sockaddr_ptr_
        && sockaddr_size_ == others_sockaddr.sockaddr_size_;
}

// line 32
bool ZEN_Sockaddr::operator!=(const ZEN_Sockaddr &others_sockaddr) const {
    return !(*this == others_sockaddr);
}

// line 37
const char * ZEN_Sockaddr::tostr(char *buffer, size_t buf_len) const {
    return ZEN_OS::socketaddr_ntop_ex(sockaddr_ptr_, buffer, buf_len);
}
