// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_sndrcv_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_BASE_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_BASE_H_H_

#include "src/commlib/framework/comm_service_info.h"
#include <stddef.h>

// sizeof = 44
struct Tibetan_SendRecv_Package { // line 26
public:
int (***_vptr.Tibetan_SendRecv_Package)(...);
protected:
SERVICES_ID tibetan_recv_service_;
SERVICES_ID tibetan_send_service_;
SERVICES_ID tibetan_proxy_service_;
size_t test_frame_len_;
unsigned int trans_id_builder_;
Comm_App_Frame *tibetan_send_appframe_;
Comm_App_Frame *tibetan_recv_appframe_;
unsigned int recv_trans_id_;
public:
Tibetan_SendRecv_Package(const Tibetan_SendRecv_Package &arg0);
Tibetan_SendRecv_Package(); // line 11
virtual ~Tibetan_SendRecv_Package(); // line 23
void set_services_id(const SERVICES_ID &recv_service, const SERVICES_ID &send_service, const SERVICES_ID &proxy_service, size_t frame_len); // line 57
void get_recv_transid(unsigned int &trans_id); // line 75
Comm_App_Frame * get_send_appframe(); // line 80
Comm_App_Frame * get_recv_appframe(); // line 86
};

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SNDRCV_BASE_H_H_
