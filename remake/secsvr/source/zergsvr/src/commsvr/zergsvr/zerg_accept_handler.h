// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_accept_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_ACCEPT_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_ACCEPT_HANDLER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_socket_acceptor.h"

// sizeof = 60
struct TCP_Accept_Handler : public ZEN_Event_Handler { // line 13
protected:
ZEN_Socket_Acceptor peer_acceptor_;
SERVICES_ID my_svc_info_;
ZEN_Sockaddr_In accept_bind_addr_;
bool sessionkey_verify_;
Zerg_IPRestrict_Mgr *ip_restrict_;
public:
TCP_Accept_Handler(const TCP_Accept_Handler &arg0);
protected:
virtual ~TCP_Accept_Handler(); // line 18
public:
TCP_Accept_Handler(const SERVICES_ID &arg0, const ZEN_Sockaddr_In &arg1, bool arg2); // line 9
int create_listen(); // line 23
virtual int handle_input(); // line 87
virtual int handle_close(); // line 156
virtual ZEN_SOCKET get_handle() const; // line 148
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_ACCEPT_HANDLER_H_H_
