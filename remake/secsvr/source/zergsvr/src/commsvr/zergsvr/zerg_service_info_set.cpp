// Reconstructed from zergsvr DWARF + disassembly.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_service_info_set.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include <cstring>
#include <sstream>
#include <string>

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"

namespace {
typedef __gnu_cxx::hash_map<SERVICES_ID, TCP_Svc_Handler *, HashofSvrInfo,
                            std::equal_to<SERVICES_ID>,
                            std::allocator<TCP_Svc_Handler *> > Service_Info_Map;
}

// line 13
Service_Info_Set::Service_Info_Set() {
}

// line 17
Service_Info_Set::~Service_Info_Set() {
}

// line 22
void Service_Info_Set::init_services_peerinfo(unsigned int szpeer) {
    svr_info_set_.resize(szpeer);
}

// line 28
int Service_Info_Set::find_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *&svrhandle) {
    Service_Info_Map::iterator iter = svr_info_set_.find(svrinfo);
    if (iter != svr_info_set_.end()) {
        svrhandle = iter->second;
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex(
        "[zergsvr] Can't find svchanle info. Svrinfo Type|ID:[%u|%u] .",
        svrinfo.services_type_, svrinfo.services_id_);
    return 0x1149339b;
}

// line 62
int Service_Info_Set::replace_services_peerInfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl,
                                                TCP_Svc_Handler *&old_svchdl) {
    const size_t TMP_ADDR_LEN = 32;
    char new_addr[TMP_ADDR_LEN];
    char old_addr[TMP_ADDR_LEN];
    old_svchdl = NULL;
    Service_Info_Map::iterator iter = svr_info_set_.find(svrinfo);
    if (iter != svr_info_set_.end()) {
        old_svchdl = iter->second;
        strncpy(new_addr, new_svchdl->get_peer_address(), TMP_ADDR_LEN);
        strncpy(old_addr, old_svchdl->get_peer_address(), TMP_ADDR_LEN);
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] replace_services_peerInfo:%u|%u ,Find Old IP|Port:[%s|%u],New IP Port[%s|%u],Replace old.",
            svrinfo.services_type_, svrinfo.services_id_, old_addr, old_svchdl->get_peer_port(),
            new_addr, new_svchdl->get_peer_port());
    }
    svr_info_set_[svrinfo] = new_svchdl;
    return 0;
}

// line 107
int Service_Info_Set::add_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl) {
    const size_t TMP_ADDR_LEN = 32;
    char new_addr[TMP_ADDR_LEN];
    char old_addr[TMP_ADDR_LEN];
    TCP_Svc_Handler *old_svchdl;
    Service_Info_Map::iterator iter = svr_info_set_.find(svrinfo);
    if (iter != svr_info_set_.end()) {
        old_svchdl = iter->second;
        strncpy(new_addr, new_svchdl->get_peer_address(), TMP_ADDR_LEN);
        strncpy(old_addr, old_svchdl->get_peer_address(), TMP_ADDR_LEN);
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] add_services_peerinfo:%u|%u Fail,Find Old IP|Port:[%s|%u],New IP Port[%s|%u],Replace old.",
            svrinfo.services_type_, svrinfo.services_id_, old_addr, old_svchdl->get_peer_port(),
            new_addr, new_svchdl->get_peer_port());
        return 0x11493396;
    }
    svr_info_set_[svrinfo] = new_svchdl;
    return 0;
}

// line 151
size_t Service_Info_Set::del_services_peerInfo(const SERVICES_ID &svrinfo) {
    size_t szdel = svr_info_set_.erase(svrinfo);
    if (szdel == 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Can't Service_Info_Set::del_services_peerInfo Size svr_info_set_ %u: szdel:%u svrinfo:%u|%u .",
            svr_info_set_.size(), 0, svrinfo.services_type_, svrinfo.services_id_);
    }
    return szdel;
}

// line 172
size_t Service_Info_Set::get_services_peersize() {
    return svr_info_set_.size();
}

// line 193
void Service_Info_Set::dump_svr_peerinfo(std::ostringstream &ostr_stream, unsigned int startno, unsigned int numquery) {
    Service_Info_Map::iterator iter_tmp = svr_info_set_.begin();
    Service_Info_Map::iterator iter_end = svr_info_set_.end();
    size_t i = 0;
    for (; iter_tmp != iter_end; ++iter_tmp, ++i) {
        if (i < startno) {
            continue;
        }
        if (numquery != 0 && startno + numquery < i) {
            break;
        }
        SERVICES_ID svr_info = iter_tmp->first;
        TCP_Svc_Handler *svrhandle = iter_tmp->second;
        svrhandle->dump_status_info(ostr_stream);
    }
}

// line 224
void Service_Info_Set::clear_and_closeall() {
    const size_t SHOWINFO_NUMBER = 500;
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Has %u peer want to close. Please wait. ACE that is accursed.",
        svr_info_set_.size());
    while (svr_info_set_.size() > 0) {
        if (svr_info_set_.size() % SHOWINFO_NUMBER == 0) {
            ZEN_Trace_LogMsg::debug_infoex(
                "[zergsvr] Now remain %u peer want to close. Please wait. ACE that is accursed.",
                svr_info_set_.size());
        }
        TCP_Svc_Handler *svrhandle = svr_info_set_.begin()->second;
        svrhandle->handle_close();
    }
}
