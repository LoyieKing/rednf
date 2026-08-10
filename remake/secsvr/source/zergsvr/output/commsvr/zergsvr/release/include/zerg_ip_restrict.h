// Reconstructed from zergsvr DWARF + disassembly.
// Zerg_IPRestrict_Mgr：IP 白/黑名单（hash_set<unsigned int>），sizeof = 40。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_IP_RESTRICT_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_IP_RESTRICT_H_H_

#include <ext/hash_set>

#include "src/commlib/zenlib/zen_socket_addr_in.h"

namespace conf_zerg {
    struct zerg_config;
}

// sizeof = 40
struct Zerg_IPRestrict_Mgr { // line 10
protected:
    __gnu_cxx::hash_set<unsigned int> allow_ip_set_; // +0
    __gnu_cxx::hash_set<unsigned int> reject_ip_set_; // +20
    static Zerg_IPRestrict_Mgr *instance_; // line 36
public:
    Zerg_IPRestrict_Mgr(); // line 12
    ~Zerg_IPRestrict_Mgr(); // line 16
    int check_iprestrict(const ZEN_Sockaddr_In &remoteaddress); // line 82
    int get_iprestrict_conf(const conf_zerg::zerg_config &config); // line 21
    static Zerg_IPRestrict_Mgr *instance(); // line 117
    static void clean_instance(); // line 129
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_IP_RESTRICT_H_H_
