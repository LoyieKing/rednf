// 还原头文件：SERVICES_ID / SERVICES_IP_INFO / SetOfServicesIPInfo / ServicesConfig
// （基于 zergsvr DWARF + 反汇编）
// SERVICES_ID 为 #pragma pack(2)（size=6：type@0，id@2，与二进制 mov 偏移一致）。
// SERVICES_IP_INFO：services_info_@0(6) ip_addr_@8 port_@0xc idc_no_@0x10
//   server_guid_no_@0x14 cfg_info_[33]@0x18（默认对齐，sizeof=60）。
// ServicesConfig（sizeof=20）：services_conf_map_（hash_set，hash=(type<<16)+id，
//   equal 仅比较 SERVICES_ID）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_

#include <stddef.h>
#include <ext/hash_set>

#include "src/commlib/framework/comm_enum_define.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"

#pragma pack(push, 1)
struct SERVICES_ID { // line 13
public:
    static const unsigned short INVALID_SERVICES_TYPE = 0;            // line 17
    static const unsigned int INVALID_SERVICES_ID = 0;                // line 20
    static const unsigned int DYNAMIC_ALLOC_SERVICES_ID = 0xffffffff; // line 22
    unsigned short services_type_; // line 28  (0x00)
    unsigned int services_id_;     // line 31  (0x02)
SERVICES_ID() {} // line 37（隐式默认构造，不初始化；二进制无此符号）
SERVICES_ID(unsigned short svrtype, unsigned int svrid); // line 38
    ~SERVICES_ID(); // line 40
    inline void clear() { // line 42
        services_type_ = INVALID_SERVICES_TYPE;
        services_id_ = INVALID_SERVICES_ID;
    }
    void set_serviceid(unsigned short svrtype, unsigned int svrid); // line 49
    bool operator==(const SERVICES_ID &others) const; // line 52
    bool operator!=(const SERVICES_ID &others) const; // line 54
    bool operator<(const SERVICES_ID &others) const; // line 56
};
#pragma pack(pop)

// sizeof = 60
struct SERVICES_IP_INFO { // line 68
public:
    SERVICES_ID services_info_;      // line 80 (0x00, 6)
    unsigned int ip_addr_;           // line 83 (0x08)
    unsigned short port_;            // line 86 (0x0c)
    unsigned int idc_no_;            // line 89 (0x10)
    unsigned int server_guid_no_;    // line 92 (0x14)
    char cfg_info_[33];              // line 95 (0x18)
    SERVICES_IP_INFO(); // line 99
    ~SERVICES_IP_INFO(); // line 100
};

// comm_enum_define.h 中的内联实现（oracle：hash = (type<<16)+id，equal 仅比 SERVICES_ID）
inline size_t HashofSvrIPInfo::operator()(const SERVICES_IP_INFO &svr_ip_info) const {
    return ((size_t)svr_ip_info.services_info_.services_type_ << 16)
           + svr_ip_info.services_info_.services_id_;
}

inline bool EqualSvrIPInfo::operator()(const SERVICES_IP_INFO &svr1, const SERVICES_IP_INFO &svr2) const {
    return svr1.services_info_ == svr2.services_info_;
}

typedef __gnu_cxx::hash_set<SERVICES_IP_INFO, HashofSvrIPInfo, EqualSvrIPInfo,
                            std::allocator<SERVICES_IP_INFO> > SetOfServicesIPInfo; // line 18

// sizeof = 20
struct ServicesConfig { // line 21
protected:
    static const int INIT_NUM_OF_SVR_CONFG = 100; // line 24（hash_set 默认桶数 100）
    SetOfServicesIPInfo services_conf_map_;       // line 29
public:
    ServicesConfig(size_t num_svr_config); // line 35
    ~ServicesConfig(); // line 36
    int FindServicesIPConf(const SERVICES_ID &svrinfo, unsigned int &ipaddress, unsigned short &port,
                           unsigned int &idc_no, unsigned int &server_guid_no, char *cfg_info); // line 44
    int FindServicesIPConf(const SERVICES_ID &svrinfo, ZEN_Sockaddr_In &inetadd,
                           unsigned int &idc_no, unsigned int &server_guid_no, char *cfg_info); // line 50
    bool IfHashSERVICES_INFO(const SERVICES_ID &svrinfo); // line 54
    int AddServicesConf(const SERVICES_ID &svrinfo, const ZEN_Sockaddr_In &inetadd, unsigned int idc_no); // line 57
    int LoadSvrConfig(const conf_svcid::svcid_config *svcid_config); // line 61
    void ClearSvrConfig(); // line 63
    static int FindSelfSERVICES_INFO(const char *cfgfile, SERVICES_ID &svrinfo); // line 69
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
