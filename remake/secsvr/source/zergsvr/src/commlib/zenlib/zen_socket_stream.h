// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_stream.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_STREAM_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_STREAM_H_H_

#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include <stddef.h>
#include <unistd.h>

// sizeof = 4
struct ZEN_Socket_Stream : public ZEN_Socket_Base { // line 11
public:
ZEN_Socket_Stream(); // line 16
ZEN_Socket_Stream(const ZEN_SOCKET &arg0); // line 17
~ZEN_Socket_Stream(); // line 18
int open(int arg0, int arg1, bool arg2); // line 26
int open(const ZEN_Sockaddr *arg0, int arg1, int arg2, bool arg3); // line 33
int shutdown(int arg0) const; // line 38
ssize_t recv_n(void *arg0, size_t arg1, ZEN_Time_Value *arg2, int arg3) const; // line 45
ssize_t send_n(const void *arg0, size_t arg1, ZEN_Time_Value *arg2, int arg3) const; // line 51
ssize_t recvn_timeout(void *arg0, size_t arg1, ZEN_Time_Value &arg2, int arg3) const; // line 57
ssize_t sendn_timeout(void *arg0, size_t arg1, ZEN_Time_Value &arg2, int arg3); // line 63
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_STREAM_H_H_
