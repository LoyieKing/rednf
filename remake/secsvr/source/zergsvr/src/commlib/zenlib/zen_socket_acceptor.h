// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_acceptor.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ACCEPTOR_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ACCEPTOR_H_H_

#include "output/commlib/zenlib/release/include/zen_socket_base.h"

// sizeof = 4
struct ZEN_Socket_Acceptor : public ZEN_Socket_Base { // line 20
private:
static const int ZEN_DEFAULT_BACKLOG = 12; // line 25
public:
ZEN_Socket_Acceptor(); // line 30
~ZEN_Socket_Acceptor(); // line 31
int open(const ZEN_Sockaddr *arg0, bool arg1, int arg2, int arg3, int arg4); // line 40
int accept(ZEN_Socket_Stream &arg0, ZEN_Sockaddr *arg1) const; // line 46
int accept(ZEN_Socket_Stream &arg0, ZEN_Time_Value &arg1, ZEN_Sockaddr *arg2) const; // line 51
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ACCEPTOR_H_H_
