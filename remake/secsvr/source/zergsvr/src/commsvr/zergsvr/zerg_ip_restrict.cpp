// Reconstructed from zergsvr DWARF + disassembly.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_ip_restrict.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include <cstring>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

#include "src/commlib/zenlib/zen_socket_addr_in.h"
#include "src/commlib/zenlib/zen_string_util.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commsvr/zergsvr/zerg_ip_restrict.h"
#include "src/protocol/common/comm_conf_zerg.h"

Zerg_IPRestrict_Mgr *Zerg_IPRestrict_Mgr::instance_ = NULL; // line 34

// line 12
Zerg_IPRestrict_Mgr::Zerg_IPRestrict_Mgr() {
}

// line 16
Zerg_IPRestrict_Mgr::~Zerg_IPRestrict_Mgr() {
}

// line 21
int Zerg_IPRestrict_Mgr::get_iprestrict_conf(const conf_zerg::zerg_config &config) {
    int ret = 0;
    const size_t TMP_BUFFER_LEN = 0x100;
    char err_outbuf[TMP_BUFFER_LEN + 1];
    std::vector<std::string> v;
    unsigned int i;

    allow_ip_set_.clear();
    reject_ip_set_.clear();
    memset(err_outbuf, 0, sizeof(err_outbuf));

    split<const char *, const char *, std::equal_to<char>,
          std::back_insert_iterator<std::vector<std::string> >, std::string>(
        config.restrict_cfg.allow_ips, strchr(config.restrict_cfg.allow_ips, '\0'),
        "\x01", "\x01" + 1, std::equal_to<char>(), std::back_inserter(v),
        (const std::string *)NULL);
    allow_ip_set_.resize(v.size());
    for (i = 0; i < v.size(); ++i) {
        ZEN_Sockaddr_In inetadd;
        if (inetadd.set(v[i].c_str(), 10) != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", err_outbuf);
            ret = 0x1398;
            break;
        }
        allow_ip_set_.insert(inetadd.get_ip_address());
    }
    if (ret != 0) {
        return ret;
    }

    v.clear();
    split<const char *, const char *, std::equal_to<char>,
          std::back_insert_iterator<std::vector<std::string> >, std::string>(
        config.restrict_cfg.reject_ips, strchr(config.restrict_cfg.reject_ips, '\0'),
        "\x01", "\x01" + 1, std::equal_to<char>(), std::back_inserter(v),
        (const std::string *)NULL);
    reject_ip_set_.resize(v.size());
    for (i = 0; i < v.size(); ++i) {
        ZEN_Sockaddr_In inetadd;
        if (inetadd.set(v[i].c_str(), 10) != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", err_outbuf);
            ret = 0x1398;
            break;
        }
        reject_ip_set_.insert(inetadd.get_ip_address());
    }
    return ret;
}

// line 82
int Zerg_IPRestrict_Mgr::check_iprestrict(const ZEN_Sockaddr_In &remoteaddress) {
    if (allow_ip_set_.empty()) {
        if (reject_ip_set_.find(remoteaddress.get_ip_address()) != reject_ip_set_.end()) {
            ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Reject IP|Port : %s|%u connect me.",
                                           remoteaddress.get_host_addr(), remoteaddress.get_port_number());
            return 0x114933b0;
        }
        return 0;
    }
    if (allow_ip_set_.find(remoteaddress.get_ip_address()) == allow_ip_set_.end()) {
        ZEN_Trace_LogMsg::debug_infoex("[zergsvr] A NO Allowed IP|Port : [%s|%u] Connect me.",
                                       remoteaddress.get_host_addr(), remoteaddress.get_port_number());
        return 0x114933b0;
    }
    if (reject_ip_set_.find(remoteaddress.get_ip_address()) != reject_ip_set_.end()) {
        ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Reject IP|Port : %s|%u connect me.",
                                       remoteaddress.get_host_addr(), remoteaddress.get_port_number());
        return 0x114933b0;
    }
    return 0;
}

// line 117
Zerg_IPRestrict_Mgr *Zerg_IPRestrict_Mgr::instance() {
    if (instance_ == NULL) {
        instance_ = new Zerg_IPRestrict_Mgr();
    }
    return instance_;
}

// line 129
void Zerg_IPRestrict_Mgr::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}
