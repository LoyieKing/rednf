// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/proto_proc/release/include/dp_msg_packer.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_MSG_PACKER_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_MSG_PACKER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/protocol/formmog/release/include/formmog_proto_public_secsvr.h"

extern int ret; // line 163
extern unsigned char trans_send_buffer[]; // line 109
extern Comm_App_Frame *rsp_msg; // line 110
extern SERVICES_ID rcvsvc; // line 142
extern SERVICES_ID proxysvc; // line 143
extern SERVICES_ID sndsvc; // line 144
extern int res; // line 95
extern /*anon struct*/ int recv_from_sdk; // line 122
namespace tss_dpsdk {
    namespace DPMsgPacker {
        enum E_TIPS_WINDOWS {
            E_TIPS_NONE = 0,
            E_TIPS_SAFEMODE = 1,
            E_TIPS_SEND_SUCCESS = 2,
        };
    } // namespace DPMsgPacker
} // namespace tss_dpsdk

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_MSG_PACKER_H_H_
