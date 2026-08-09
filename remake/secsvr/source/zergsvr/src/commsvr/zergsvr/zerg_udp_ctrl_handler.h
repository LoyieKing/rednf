// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_udp_ctrl_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include <bits/stl_vector.h>

// sizeof = 60
struct UDP_Svc_Handler : public ZEN_Event_Handler { // line 34
protected:
static const size_t ONCE_MAX_READ_UDP_NUMBER;
Zen_Socket_DataGram dgram_peer_;
ZEN_Sockaddr_In udp_bind_addr_;
SERVICES_ID my_svc_info_;
bool sessionkey_verify_;
ZByteBuffer *dgram_databuf_;
Zerg_IPRestrict_Mgr *ip_restrict_;
static /*anon struct*/ int ary_udpsvc_handler_;
static UDPSessionKeyMgr *udp_session_mgr_;
static Comm_Stat_Monitor *server_status_;
static Zerg_Comm_Manager *zerg_comm_mgr_;
static bool if_proxy_;
static unsigned int game_id_;
public:
UDP_Svc_Handler(const UDP_Svc_Handler &arg0);
UDP_Svc_Handler(const SERVICES_ID &arg0, const ZEN_Sockaddr_In &arg1, bool arg2); // line 45
protected:
virtual ~UDP_Svc_Handler(); // line 58
public:
virtual ZEN_SOCKET get_handle() const; // line 127
virtual int handle_input(); // line 134
virtual int handle_close(); // line 172
int init_udp_services(); // line 81
protected:
int read_data_from_udp(size_t &size_revc); // line 201
int write_data_to_udp(Comm_App_Frame *send_frame); // line 313
public:
static int init_all_static_data(); // line 407
static int send_all_to_udp(Comm_App_Frame *send_frame); // line 373
static int get_udpctrl_conf(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config); // line 423
};
extern const size_t ONCE_MAX_READ_UDP_NUMBER; // line 81

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_
