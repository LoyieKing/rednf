// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_socket.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

extern unsigned int __v; // line 1810
extern unsigned int __x; // line 1810
extern ssize_t zen_result; // line 1381
extern timeval *select_tv; // line 1718
extern ZEN_SOCKET socket_hdl; // line 1042
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
    extern uint32_t get_ip_address(const sockaddr_in *arg0); // line 1808
    extern ssize_t sendto(ZEN_SOCKET arg0, const void *arg1, size_t arg2, int arg3, const sockaddr *arg4, socklen_t arg5); // line 1497
    extern ssize_t recv(ZEN_SOCKET arg0, void *arg1, size_t arg2, int arg3); // line 1362
    extern int select(int arg0, ._11 *arg1, ._11 *arg2, ._11 *arg3, ZEN_Time_Value *arg4); // line 1653
    extern ssize_t recvfrom(ZEN_SOCKET arg0, void *arg1, size_t arg2, int arg3, sockaddr *arg4, socklen_t *arg5); // line 1445
    extern ssize_t send(ZEN_SOCKET arg0, const void *arg1, size_t arg2, int arg3); // line 1402
    extern int setsockopt(ZEN_SOCKET arg0, int arg1, int arg2, const void *arg3, socklen_t arg4); // line 1269
    extern int set_sockaddr_in(sockaddr_in *arg0, uint32_t arg1, uint16_t arg2); // line 1848
    extern int connect(ZEN_SOCKET arg0, const sockaddr *arg1, socklen_t arg2); // line 1156
    extern int getsockname(ZEN_SOCKET arg0, sockaddr *arg1, socklen_t *arg2); // line 1206
    extern int getpeername(ZEN_SOCKET arg0, sockaddr *arg1, socklen_t *arg2); // line 1181
    extern int getsockopt(ZEN_SOCKET arg0, int arg1, int arg2, void *arg3, socklen_t *arg4); // line 1235
    extern int bind(ZEN_SOCKET arg0, const sockaddr *arg1, socklen_t arg2); // line 1113
    extern ZEN_SOCKET socket(int arg0, int arg1, int arg2); // line 1040
    extern int closesocket(ZEN_SOCKET arg0); // line 1136
    extern ssize_t send_n(ZEN_SOCKET arg0, const void *arg1, size_t arg2, ZEN_Time_Value *arg3, int arg4); // line 1561
    extern ssize_t recv_n(ZEN_SOCKET arg0, void *arg1, size_t arg2, ZEN_Time_Value *arg3, int arg4); // line 1535
    extern int shutdown(ZEN_SOCKET arg0, int arg1); // line 1338
} // namespace ZEN_OS


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SOCKET_H_H_
