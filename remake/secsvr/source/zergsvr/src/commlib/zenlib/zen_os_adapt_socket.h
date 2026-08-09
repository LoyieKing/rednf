// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_os_adapt_socket.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_

#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include <stddef.h>
#include <unistd.h>

namespace ZEN_OS {
    extern ssize_t sendto(ZEN_SOCKET arg0, const void *arg1, size_t arg2, int arg3, const sockaddr *arg4, socklen_t arg5); // line 1027
    extern ssize_t recvfrom(ZEN_SOCKET arg0, void *arg1, size_t arg2, int arg3, sockaddr *arg4, socklen_t *arg5); // line 975
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_
