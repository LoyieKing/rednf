// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_BASE_H_H_

#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include <stddef.h>
#include <unistd.h>

// sizeof = 4
struct ZEN_Socket_Base { // line 10
protected:
ZEN_SOCKET socket_handle_;
ZEN_Socket_Base(); // line 20
ZEN_Socket_Base(const ZEN_SOCKET &arg0); // line 21
~ZEN_Socket_Base(); // line 22
public:
void set_handle(const ZEN_SOCKET &arg0); // line 28
ZEN_SOCKET get_handle() const; // line 30
int open(int arg0, int arg1, int arg2, bool arg3); // line 36
int open(int arg0, const ZEN_Sockaddr *arg1, int arg2, int arg3, bool arg4); // line 43
int close(); // line 46
int bind(const ZEN_Sockaddr *arg0) const; // line 48
int sock_enable(int arg0) const; // line 51
int sock_disable(int arg0) const; // line 54
int getsockopt(int arg0, int arg1, void *arg2, socklen_t *arg3) const; // line 60
int setsockopt(int arg0, int arg1, const void *arg2, int arg3) const; // line 66
int getpeername(ZEN_Sockaddr *arg0) const; // line 69
int getsockname(ZEN_Sockaddr *arg0) const; // line 72
int connect(const ZEN_Sockaddr *arg0) const; // line 80
ssize_t recv(void *arg0, size_t arg1, int arg2) const; // line 85
ssize_t send(const void *arg0, size_t arg1, int arg2) const; // line 90
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_BASE_H_H_
