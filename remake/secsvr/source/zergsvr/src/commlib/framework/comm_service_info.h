// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_service_info.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_

#include <stddef.h>

// sizeof = 6
struct SERVICES_ID { // line 13
public:
static const short unsigned int INVALID_SERVICES_TYPE;
static const unsigned int INVALID_SERVICES_ID;
static const unsigned int DYNAMIC_ALLOC_SERVICES_ID;
short unsigned int services_type_;
unsigned int services_id_;
SERVICES_ID(short unsigned int arg0, unsigned int arg1); // line 38
~SERVICES_ID(); // line 40
void clear(); // line 42
void set_serviceid(short unsigned int arg0, unsigned int arg1); // line 49
bool operator==(const SERVICES_ID &arg0) const; // line 52
bool operator!=(const SERVICES_ID &arg0) const; // line 54
bool operator<(const SERVICES_ID &arg0) const; // line 56
};
// sizeof = 60
struct SERVICES_IP_INFO { // line 77
public:
SERVICES_ID services_info_;
unsigned int ip_addr_;
short unsigned int port_;
unsigned int idc_no_;
unsigned int server_guid_no_;
char cfg_info_[];
SERVICES_IP_INFO(); // line 99
~SERVICES_IP_INFO(); // line 100
};
// sizeof = 1
struct HashofSvrIPInfo { // line 106
public:
size_t operator()(const SERVICES_IP_INFO &arg0) const; // line 108
};
// sizeof = 1
struct EqualSvrIPInfo { // line 115
public:
bool operator()(const SERVICES_IP_INFO &arg0, const SERVICES_IP_INFO &arg1) const; // line 118
};
// sizeof = 1
struct HashofSvrInfo { // line 64
public:
size_t operator()(const SERVICES_ID &arg0) const; // line 66
};
extern const short unsigned int INVALID_SERVICES_TYPE; // line 17
extern const unsigned int INVALID_SERVICES_ID; // line 20
extern const unsigned int DYNAMIC_ALLOC_SERVICES_ID; // line 22

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_INFO_H_H_
