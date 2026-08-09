// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_sndrcv_zulu.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_ZULU_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_ZULU_H_H_

#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include <stddef.h>

// sizeof = 80
struct Zulu_SendRecv_Package : public Tibetan_SendRecv_Package { // line 29
protected:
bool zulu_connected_;
ZEN_Sockaddr_In zulu_svc_ip_;
ZEN_Socket_Stream zulu_stream_;
public:
Zulu_SendRecv_Package(const Zulu_SendRecv_Package &arg0);
Zulu_SendRecv_Package(); // line 16
virtual ~Zulu_SendRecv_Package(); // line 23
protected:
int receive_svc_package(ZEN_Time_Value *time_wait); // line 116
int send_svc_package(ZEN_Time_Value *time_wait); // line 265
public:
int set_zulu_svcinfo(const char *svc_ip, short unsigned int svc_port, const SERVICES_ID &recv_service, const SERVICES_ID &send_service, const SERVICES_ID &proxy_service, size_t frame_len); // line 55
int connect_zulu_server(ZEN_Time_Value *time_wait); // line 72
void close(); // line 105
int receive_svc_package(unsigned int &recv_cmd, ZEN_Time_Value *time_out); // line 307
};

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_ZULU_H_H_
