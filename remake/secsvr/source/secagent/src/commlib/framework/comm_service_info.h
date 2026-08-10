// 还原头文件：SERVICES_ID / SERVICES_IP_INFO（基于 gunnersvr DWARF + 反汇编）
// SERVICES_ID 为 #pragma pack(1)（size=6：type@0，id@2，alignment=1；
// CommBillRecord 中 self_svr_ 位于 0x1755（if_thread_sync_ 0x1754 后无填充），
// 三二进制 DWARF 均为 plus_uconst 5973；pack(2) 会把 self_svr_ 推到 0x1756）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_

#pragma pack(push, 1)
struct SERVICES_ID { // line 13
public:
    static const unsigned short INVALID_SERVICES_TYPE = 0;            // line 17
    static const unsigned int INVALID_SERVICES_ID = 0;                // line 20
    static const unsigned int DYNAMIC_ALLOC_SERVICES_ID = 0xffffffff; // line 22
    unsigned short services_type_; // line 28  (0x00)
    unsigned int services_id_;     // line 31  (0x02)
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

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
