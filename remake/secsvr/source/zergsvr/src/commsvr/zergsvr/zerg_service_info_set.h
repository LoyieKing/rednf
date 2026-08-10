// Reconstructed from zergsvr DWARF + disassembly.
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_SERVICE_INFO_SET_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_SERVICE_INFO_SET_H_H_

#include <ext/hash_map>
#include <sstream>
#include <stddef.h>

#include "output/commlib/framework/release/include/comm_service_info.h"

struct TCP_Svc_Handler;

// sizeof = 1
struct HashofSvrInfo { // line 65
public:
    size_t operator()(const SERVICES_ID &svr_info) const {
        return ((size_t)svr_info.services_type_ << 16) + svr_info.services_id_;
    }
};

// sizeof = 20
struct Service_Info_Set { // line 12
protected:
    __gnu_cxx::hash_map<SERVICES_ID, TCP_Svc_Handler *, HashofSvrInfo,
                        std::equal_to<SERVICES_ID>,
                        std::allocator<TCP_Svc_Handler *> > svr_info_set_; // +0
public:
    Service_Info_Set(); // line 13
    ~Service_Info_Set(); // line 17
    void init_services_peerinfo(unsigned int szpeer); // line 22
    int find_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *&svrhandle); // line 28
    int replace_services_peerInfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl, TCP_Svc_Handler *&old_svchdl); // line 62
    int add_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl); // line 107
    size_t del_services_peerInfo(const SERVICES_ID &svrinfo); // line 151
    size_t get_services_peersize(); // line 172
    void dump_svr_peerinfo(std::ostringstream &ostr_stream, unsigned int startno,
                           unsigned int numquery); // line 193
    void clear_and_closeall(); // line 224
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_SERVICE_INFO_SET_H_H_
