// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_service_config.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_

#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include <stddef.h>

typedef hash_set<SERVICES_IP_INFO,HashofSvrIPInfo,EqualSvrIPInfo,std::allocator<SERVICES_IP_INFO> > SetOfServicesIPInfo;
// sizeof = 20
struct ServicesConfig { // line 21
protected:
static const int INIT_NUM_OF_SVR_CONFG;
SetOfServicesIPInfo services_conf_map_;
public:
ServicesConfig(size_t arg0); // line 35
~ServicesConfig(); // line 36
int FindServicesIPConf(const SERVICES_ID &arg0, unsigned int &arg1, short unsigned int &arg2, unsigned int &arg3, unsigned int &arg4, char *arg5); // line 44
int FindServicesIPConf(const SERVICES_ID &arg0, ZEN_Sockaddr_In &arg1, unsigned int &arg2, unsigned int &arg3, char *arg4); // line 50
bool IfHashSERVICES_INFO(const SERVICES_ID &arg0); // line 54
int AddServicesConf(const SERVICES_ID &arg0, const ZEN_Sockaddr_In &arg1, unsigned int arg2); // line 57
int LoadSvrConfig(const ServicesConfig::SVCID_CONFIG *arg0); // line 61
void ClearSvrConfig(); // line 63
static int FindSelfSERVICES_INFO(const char *arg0, SERVICES_ID &arg1); // line 69
};
extern const int INIT_NUM_OF_SVR_CONFG; // line 24

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_
