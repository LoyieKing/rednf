// Reconstructed from zergsvr DWARF + disassembly.
// UDP_Svc_Handler：UDP 服务连接处理器，sizeof = 60。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_

#include <vector>

#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_socket_datagram.h"
#include "src/commlib/zenlib/zen_socket_addr_in.h"

struct Comm_App_Frame;
struct Comm_Stat_Monitor;
struct Zerg_Comm_Manager;
struct ZByteBuffer;
struct Zerg_IPRestrict_Mgr;
struct UDPSessionKeyMgr;
struct Zerg_MMAP_BusPipe;
namespace conf_zerg {
    struct zerg_config;
}

// sizeof = 60
struct UDP_Svc_Handler : public ZEN_Event_Handler { // line 34
protected:
    static const size_t ONCE_MAX_READ_UDP_NUMBER; // line 81
    Zen_Socket_DataGram dgram_peer_; // +12
    ZEN_Sockaddr_In udp_bind_addr_; // +16
    SERVICES_ID my_svc_info_; // +44
    bool sessionkey_verify_; // +50
    ZByteBuffer *dgram_databuf_; // +52
    Zerg_IPRestrict_Mgr *ip_restrict_; // +56
    static std::vector<UDP_Svc_Handler *> ary_udpsvc_handler_; // line 28
    static UDPSessionKeyMgr *udp_session_mgr_; // line 30
    static Comm_Stat_Monitor *server_status_; // line 32
    static Zerg_Comm_Manager *zerg_comm_mgr_; // line 34
    static bool if_proxy_; // line 37
    static unsigned int game_id_; // line 40
public:
    UDP_Svc_Handler(const UDP_Svc_Handler &arg0);
    UDP_Svc_Handler(const SERVICES_ID &svr_info, const ZEN_Sockaddr_In &bind_addr,
                    bool sessionkey_verify); // line 45
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
    static int get_udpctrl_conf(const conf_zerg::zerg_config *config); // line 423
};

extern const size_t ONCE_MAX_READ_UDP_NUMBER; // line 81

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_UDP_CTRL_HANDLER_H_H_
