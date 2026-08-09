// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_datagram.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_

#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include <stddef.h>
#include <unistd.h>

// sizeof = 4
struct Zen_Socket_DataGram : public ZEN_Socket_Base { // line 12
public:
Zen_Socket_DataGram(); // line 17
Zen_Socket_DataGram(const ZEN_SOCKET &arg0); // line 18
Zen_Socket_DataGram(const ZEN_Sockaddr &arg0); // line 19
~Zen_Socket_DataGram(); // line 20
int open(int arg0, int arg1, bool arg2); // line 27
int open(const ZEN_Sockaddr *arg0, int arg1, int arg2, bool arg3); // line 34
ssize_t recvfrom(void *arg0, size_t arg1, int arg2, ZEN_Sockaddr *arg3) const; // line 41
ssize_t sendto(const void *arg0, size_t arg1, int arg2, const ZEN_Sockaddr *arg3) const; // line 47
ssize_t recvfrom(void *arg0, size_t arg1, ZEN_Sockaddr *arg2, ZEN_Time_Value *arg3, int arg4) const; // line 54
ssize_t sendto(const void *arg0, size_t arg1, const ZEN_Sockaddr *arg2, ZEN_Time_Value *arg3, int arg4) const; // line 62
ssize_t recvfrom_timeout(void *arg0, size_t arg1, ZEN_Sockaddr *arg2, ZEN_Time_Value &arg3, int arg4) const; // line 69
ssize_t sendto_timeout(const void *arg0, size_t arg1, const ZEN_Sockaddr *arg2, ZEN_Time_Value &arg3, int arg4) const; // line 77
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_
