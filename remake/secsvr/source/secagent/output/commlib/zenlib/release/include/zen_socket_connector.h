// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_connector.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_CONNECTOR_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_CONNECTOR_H_H_

#include "zen_socket_stream.h"
#include "zen_socket_addr_base.h"
#include "zen_time_value.h"

// sizeof = 1
struct ZEN_Socket_Connector { // line 18
public:
ZEN_Socket_Connector(); // line 21
~ZEN_Socket_Connector(); // line 22
int connect(ZEN_Socket_Stream &arg0, const ZEN_Sockaddr *arg1, ZEN_Time_Value &arg2, bool arg3, int arg4, const ZEN_Sockaddr *arg5); // line 30
int connect(ZEN_Socket_Stream &arg0, const ZEN_Sockaddr *arg1, bool arg2, bool arg3, int arg4, const ZEN_Sockaddr *arg5); // line 38
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_CONNECTOR_H_H_
