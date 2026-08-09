// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_configure.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONFIGURE_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONFIGURE_H_H_

#include "output/commlib/framework/release/include/comm_enum_define.h"
#include "output/commlib/framework/release/include/comm_service_config.h"
#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include <bits/stringfwd.h>

// sizeof = 36
struct ZERG_SERVICES_INFO { // line 10
public:
SERVICES_ID zerg_svc_info_;
bool zerg_sessionkey_;
ZEN_Sockaddr_In zerg_ip_addr_;
ZERG_SERVICES_INFO(); // line 20
~ZERG_SERVICES_INFO(); // line 21
ZERG_SERVICES_INFO & operator=(const ZERG_SERVICES_INFO &arg0);
ZERG_SERVICES_INFO(const ZERG_SERVICES_INFO &arg0);
};
typedef vector<ZERG_SERVICES_INFO,std::allocator<ZERG_SERVICES_INFO> > SELF_SERVICESINFO_LIST;
// sizeof = 122928
struct Zerg_Server_Config { // line 33
private:
static const size_t MAX_SELF_SERVICES_ID;
public:
static const time_t UPDATE_OPERATION_KEY_TIME;
string zerg_stat_file_;
string cmd_stat_file_;
bool zerg_insurance_;
GET_SVC_IP_WAY get_svc_ip_way_;
bool zerg_need_opkey_;
bool zerg_udp_session_;
ServicesConfig services_info_cfg_;
ServicesConfig::SVCID_CONFIG svcid_info_;
static ZERG_SERVICES_INFO self_svc_info_;
static SELF_SERVICESINFO_LIST slave_svc_ary_;
static Zerg_Server_Config *instance_;
const Zerg_MMAP_BusPipe::ZERG_CONFIG *config_;
private:
Zerg_Server_Config(); // line 35
~Zerg_Server_Config(); // line 36
public:
void dump_status_info(ostringstream &ostr_stream); // line 90
int GetServicesIPInfo(const SERVICES_ID &svc_info, ZEN_Sockaddr_In &ipaddr); // line 93
int init(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config); // line 95
static Zerg_Server_Config * instance(); // line 101
static void clean_instance(); // line 103
};
extern ZERG_SERVICES_INFO self_svc_info_; // line 75
extern SELF_SERVICESINFO_LIST slave_svc_ary_; // line 78

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONFIGURE_H_H_
