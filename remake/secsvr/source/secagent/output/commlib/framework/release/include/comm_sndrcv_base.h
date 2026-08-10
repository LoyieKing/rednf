// Restored from gunnersvr DWARF + disassembly (2026-08-10)
// Tibetan_SendRecv_Package：comm_sndrcv_base.cpp 全部 9 符号已还原。
// 布局：vptr(0) + 3*SERVICES_ID + test_frame_len_(0x18) + trans_id_builder_(0x1c)
//       + send_appframe_(0x20) + recv_appframe_(0x24) + recv_trans_id_(0x28) = 44
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_BASE_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_BASE_H_H_

#include <stddef.h>

#include "src/commlib/framework/comm_service_info.h"

struct Comm_App_Frame;

// sizeof = 44
struct Tibetan_SendRecv_Package { // line 26
public:
    virtual ~Tibetan_SendRecv_Package(); // line 23
protected:
    SERVICES_ID tibetan_recv_service_;    // 0x04
    SERVICES_ID tibetan_send_service_;    // 0x0a
    SERVICES_ID tibetan_proxy_service_;   // 0x10
    size_t test_frame_len_;               // 0x18
    unsigned int trans_id_builder_;       // 0x1c
    Comm_App_Frame *tibetan_send_appframe_; // 0x20
    Comm_App_Frame *tibetan_recv_appframe_; // 0x24
    unsigned int recv_trans_id_;          // 0x28
public:
    Tibetan_SendRecv_Package(); // line 11
    void set_services_id(const SERVICES_ID &recv_service, const SERVICES_ID &send_service,
                         const SERVICES_ID &proxy_service, size_t frame_len); // line 57
    void get_recv_transid(unsigned int &trans_id); // line 75
    Comm_App_Frame * get_send_appframe(); // line 80
    Comm_App_Frame * get_recv_appframe(); // line 86
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_BASE_H_H_
