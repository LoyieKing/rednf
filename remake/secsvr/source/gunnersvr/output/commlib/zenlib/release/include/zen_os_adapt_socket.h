// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_socket.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

extern unsigned int __x; // line 1339
extern unsigned int __v; // line 1339
extern ssize_t zen_result; // line 911
extern timeval *timeout_tv; // line 1247
extern ZEN_SOCKET socket_hdl; // line 572
namespace ZEN_OS {
    enum HANDLE_READY_TODO {
        HANDLE_READY_READ = 1,
        HANDLE_READY_ACCEPT = 1,
        HANDLE_READY_WRITE = 2,
        HANDLE_READY_CONNECT_SUCCESS = 2,
        HANDLE_READY_EXCEPTION = 4,
        HANDLE_READY_CONNECT_FAIL = 1,
        HANDLE_READY_READ = 1,
        HANDLE_READY_ACCEPT = 1,
        HANDLE_READY_WRITE = 2,
        HANDLE_READY_CONNECT_SUCCESS = 2,
        HANDLE_READY_EXCEPTION = 4,
        HANDLE_READY_CONNECT_FAIL = 1,
    };
    extern uint32_t get_ip_address(const sockaddr_in *arg0); // line 1337
    extern ssize_t sendto(ZEN_SOCKET arg0, const void *arg1, size_t arg2, int arg3, const sockaddr *arg4, socklen_t arg5); // line 1027
    extern ssize_t recv(ZEN_SOCKET arg0, void *arg1, size_t arg2, int arg3); // line 892
    extern int select(int arg0, ._11 *arg1, ._11 *arg2, ._11 *arg3, ZEN_Time_Value *arg4); // line 1183
    extern ssize_t recvfrom(ZEN_SOCKET arg0, void *arg1, size_t arg2, int arg3, sockaddr *arg4, socklen_t *arg5); // line 975
    extern ssize_t send(ZEN_SOCKET arg0, const void *arg1, size_t arg2, int arg3); // line 932
    extern int setsockopt(ZEN_SOCKET arg0, int arg1, int arg2, const void *arg3, socklen_t arg4); // line 799
    extern int set_sockaddr_in(sockaddr_in *arg0, uint32_t arg1, uint16_t arg2); // line 1377
    extern int connect(ZEN_SOCKET arg0, const sockaddr *arg1, socklen_t arg2); // line 686
    extern int getsockname(ZEN_SOCKET arg0, sockaddr *arg1, socklen_t *arg2); // line 736
    extern int getpeername(ZEN_SOCKET arg0, sockaddr *arg1, socklen_t *arg2); // line 711
    extern int getsockopt(ZEN_SOCKET arg0, int arg1, int arg2, void *arg3, socklen_t *arg4); // line 765
    extern int bind(ZEN_SOCKET arg0, const sockaddr *arg1, socklen_t arg2); // line 643
    extern ZEN_SOCKET socket(int arg0, int arg1, int arg2); // line 570
    extern int closesocket(ZEN_SOCKET arg0); // line 666
    extern ssize_t send_n(ZEN_SOCKET arg0, const void *arg1, size_t arg2, ZEN_Time_Value *arg3, int arg4); // line 1091
    extern ssize_t recv_n(ZEN_SOCKET arg0, void *arg1, size_t arg2, ZEN_Time_Value *arg3, int arg4); // line 1065
    extern int shutdown(ZEN_SOCKET arg0, int arg1); // line 868
} // namespace ZEN_OS

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_
