// Restored from zergsvr binary disassembly (2026-08-10).
// All 11 comm_service_config.cpp symbols reconstructed:
//   ServicesConfig(unsigned int) / ~ServicesConfig(),
//   FindServicesIPConf x2, IfHashSERVICES_INFO, AddServicesConf,
//   LoadSvrConfig, ClearSvrConfig, FindSelfSERVICES_INFO.
// Error codes observed: not found/duplicate -> 0x13a3, config error -> 0x1398.
#include "src/commlib/framework/comm_predefine.h"
#include <string.h>
#include <stdint.h>
#include <utility>
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_config_ini_implement.h"
#include "src/commlib/framework/comm_service_config.h"

// line 35
ServicesConfig::ServicesConfig(size_t num_svr_config)
    : services_conf_map_() {
    services_conf_map_.resize(num_svr_config);
}

// line 36
ServicesConfig::~ServicesConfig() {
}

// line 44
int ServicesConfig::FindServicesIPConf(const SERVICES_ID &svrinfo, unsigned int &ipaddress,
                                       unsigned short &port, unsigned int &idc_no,
                                       unsigned int &server_guid_no, char *cfg_info) {
    SERVICES_IP_INFO svc_ip_info;
    svc_ip_info.services_info_ = svrinfo;

    SetOfServicesIPInfo::const_iterator iter = services_conf_map_.find(svc_ip_info);
    if (iter == services_conf_map_.end())
        return 0x13a3;
    ipaddress = iter->ip_addr_;
    port = iter->port_;
    idc_no = iter->idc_no_;
    server_guid_no = iter->server_guid_no_;
    if (cfg_info != 0) {
        strncpy(cfg_info, iter->cfg_info_, 0x20);
        cfg_info[0x20] = 0;
    }
    return 0;
}

// line 50
int ServicesConfig::FindServicesIPConf(const SERVICES_ID &svrinfo, ZEN_Sockaddr_In &inetadd,
                                       unsigned int &idc_no, unsigned int &server_guid_no,
                                       char *cfg_info) {
    unsigned int ip = 0;
    unsigned short port = 0;
    int ret = FindServicesIPConf(svrinfo, ip, port, idc_no, server_guid_no, cfg_info);
    if (ret == 0)
        inetadd.set(ip, port);
    return ret;
}

// line 54
bool ServicesConfig::IfHashSERVICES_INFO(const SERVICES_ID &svrinfo) {
    SERVICES_IP_INFO svc_ip_info;
    svc_ip_info.services_info_ = svrinfo;

    SetOfServicesIPInfo::const_iterator iter = services_conf_map_.find(svc_ip_info);
    return iter != services_conf_map_.end();
}

// line 57
int ServicesConfig::AddServicesConf(const SERVICES_ID &svrinfo, const ZEN_Sockaddr_In &inetadd,
                                    unsigned int idc_no) {
    SERVICES_IP_INFO svc_ip_info;
    svc_ip_info.services_info_ = svrinfo;
    svc_ip_info.ip_addr_ = inetadd.get_ip_address();
    svc_ip_info.port_ = inetadd.get_port_number();
    svc_ip_info.idc_no_ = idc_no;

    services_conf_map_.resize(services_conf_map_.size() + 1);
    std::pair<SetOfServicesIPInfo::iterator, bool> insert_result = services_conf_map_.insert(svc_ip_info);
    if (insert_result.second == false)
        return 0x13a3;
    return 0;
}

// line 61
int ServicesConfig::LoadSvrConfig(const conf_svcid::svcid_config *svcid_config) {
    int ret = 0;
    const int BUFFER_LEN = 0x80;
    char tmpkey[BUFFER_LEN];
    char tmpbuf[BUFFER_LEN];
    char outbuf[BUFFER_LEN];
    SERVICES_IP_INFO svc_ip_info;
    uint32_t num_svr = 0;
    uint32_t int_value = 0;
    uint32_t i = 0;
    ZEN_Sockaddr_In inetadd;
    std::pair<SetOfServicesIPInfo::iterator, bool> insert_result;

    ZEN_Trace_LogMsg::debug_infoex("[framework] Read Sevices Info Configure File.");
    num_svr = svcid_config->service_num_;
    if (num_svr == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SERVICESINFO|NUMSVRINFO key error.");
        return 0x1398;
    }
    services_conf_map_.clear();

    for (i = 0; i < num_svr; i++) {
        snprintf(tmpkey, BUFFER_LEN, "SERVICESTYPE%u", i);
        snprintf(outbuf, BUFFER_LEN, "SERVICESINFO|%s", tmpkey);
        if (svcid_config->service_info_[i].svr_type_ == 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", outbuf);
            return 0x1398;
        }
        svc_ip_info.services_info_.services_type_ = svcid_config->service_info_[i].svr_type_;

        snprintf(tmpkey, BUFFER_LEN, "SERVICESID%u", i);
        snprintf(outbuf, BUFFER_LEN, "SERVICESINFO|%s", tmpkey);
        int_value = svcid_config->service_info_[i].svr_id_;
        if (int_value == 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", outbuf);
            return 0x1398;
        }
        svc_ip_info.services_info_.services_id_ = int_value;

        snprintf(tmpkey, BUFFER_LEN, "SERVICESIP%u", i);
        snprintf(outbuf, BUFFER_LEN, "SERVICESINFO|%s", tmpkey);
        snprintf(tmpkey, BUFFER_LEN, "SERVICESPORT%u", i);
        snprintf(outbuf, BUFFER_LEN, "SERVICESINFO|%s", tmpkey);
        int_value = svcid_config->service_info_[i].svr_port_;
        if (int_value == 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", outbuf);
            return 0x1398;
        }
        inetadd.set(tmpbuf, (unsigned short)int_value);
        ret = inetadd.set(svcid_config->service_info_[i].svr_ip_, (unsigned short)int_value);
        snprintf(outbuf, BUFFER_LEN, "SERVICESINFO|SERVICEIP|%s key error.", tmpkey);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", outbuf);
            return 0x1398;
        }

        svc_ip_info.ip_addr_ = inetadd.get_ip_address();
        svc_ip_info.port_ = inetadd.get_port_number();
        svc_ip_info.idc_no_ = svcid_config->service_info_[i].svr_idc_;
        strncpy(svc_ip_info.cfg_info_, svcid_config->service_info_[i].svr_info_, 0x20);
        svc_ip_info.cfg_info_[0x20] = 0;

        services_conf_map_.resize(services_conf_map_.size() + 1);
        insert_result = services_conf_map_.insert(svc_ip_info);
        if (insert_result.second == false) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] ServicesConfig::LoadSvrConfig svrinfo repeat ...%u|%u.",
                svc_ip_info.services_info_.services_type_,
                svc_ip_info.services_info_.services_id_);
            return 0x13a3;
        }
    }
    return 0;
}

// line 63
void ServicesConfig::ClearSvrConfig() {
    services_conf_map_.clear();
}

// line 69
int ServicesConfig::FindSelfSERVICES_INFO(const char *cfgfile, SERVICES_ID &svrinfo) {
    int ret = 0;
    const int BUFFER_LEN = 0x100;
    char outbuf[BUFFER_LEN];
    uint32_t int_value = 0;
    ZEN_INI_Implemention read_cfg;
    ZenINIPropertyTree self_svr_cfg;

    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Read Self Services Configure File. [%s] .", cfgfile);
    ret = read_cfg.read(cfgfile, self_svr_cfg);
    snprintf(outbuf, BUFFER_LEN, "Can't Self Services Configure file [%s].", cfgfile);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", outbuf);
        return 0x1398;
    }

    string str_section("SELFCFG");
    string str_key1("SELFSVRTYPE");
    ret = self_svr_cfg.get_uint32_value(str_section, str_key1, int_value);
    if (ret != 0 || int_value == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRTYPE key error.");
        return 0x1398;
    }
    svrinfo.services_type_ = (unsigned short)int_value;

    string str_key2("SELFSVRID");
    ret = self_svr_cfg.get_uint32_value(str_section, str_key2, int_value);
    if (ret != 0 || int_value == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRTYPE key error.");
        return 0x1398;
    }
    svrinfo.services_id_ = int_value;
    return 0;
}
