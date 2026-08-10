// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_auto_connect.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_AUTO_CONNECT_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_AUTO_CONNECT_H_H_

#include <map>
#include <vector>

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include <stddef.h>

struct Zerg_MMAP_BusPipe;
namespace conf_zerg {
    struct zerg_config;
}

// sizeof = 72
struct SERVICES_ROUTE_INFO { // line 14
public:
SERVICES_ID main_route_info_;
ZEN_Sockaddr_In main_route_ip_;
bool back_route_valid_;
SERVICES_ID back_route_info_;
ZEN_Sockaddr_In back_route_ip_;
SERVICES_ROUTE_INFO(); // line 17
~SERVICES_ROUTE_INFO(); // line 18
};
typedef std::vector<SERVICES_ROUTE_INFO, std::allocator<SERVICES_ROUTE_INFO> > ArrayOfSvcInfoIPAddr;
typedef std::map<short unsigned int, unsigned int, std::less<short unsigned int>,
                 std::allocator<std::pair<const short unsigned int, unsigned int> > > RouteType2Index;
typedef std::vector<std::vector<unsigned int, std::allocator<unsigned int> >,
                    std::allocator<std::vector<unsigned int, std::allocator<unsigned int> > > > ListOfMainRouteId;
typedef std::vector<std::vector<unsigned int, std::allocator<unsigned int> >,
                    std::allocator<std::vector<unsigned int, std::allocator<unsigned int> > > > ListOfBackRouteId;
// sizeof = 68
struct Zerg_Auto_Connector { // line 57
protected:
ArrayOfSvcInfoIPAddr ary_want_connect_;
size_t size_of_wantconnect_;
ZEN_Socket_Connector zerg_connector_;
private:
ListOfMainRouteId list_of_want_connect_main_id_;
ListOfMainRouteId list_of_want_connect_back_id_;
RouteType2Index want_connect_type_2_index_;
public:
Zerg_Auto_Connector(); // line 82
~Zerg_Auto_Connector(); // line 83
int get_autoconnect_cfg(const conf_zerg::zerg_config *config); // line 86
int reload_cfg(const conf_zerg::zerg_config *config); // line 89
void reconnect_allserver(size_t &szvalid, size_t &szsucc, size_t &szfail); // line 92
size_t numsvr_connect(); // line 96
int get_backupsvcinfo(const SERVICES_ID &main_svrinfo, bool &backroute_valid, SERVICES_ID &backroute_svrinfo); // line 101
int reconnect_server(const SERVICES_ID &reconnect_svrinfo); // line 104
int get_server(short unsigned int svr_type, SERVICES_ID *svrinfo); // line 107
protected:
int connect_server_bysvcid(const SERVICES_ID &svrinfo, const ZEN_Sockaddr_In &inetaddr); // line 111
bool is_connected(const SERVICES_ID &svrinfo); // line 114
private:
void add_auto_connect_info(const SERVICES_ID &main_svrinfo, const SERVICES_ID &back_svrinfo); // line 118
bool is_current_auto_connect(const SERVICES_ID &service, bool is_main_service); // line 121
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_AUTO_CONNECT_H_H_
