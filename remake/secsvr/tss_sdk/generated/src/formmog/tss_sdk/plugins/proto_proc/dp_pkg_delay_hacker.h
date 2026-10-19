// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/dp_pkg_delay_hacker.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_PKG_DELAY_HACKER_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_PKG_DELAY_HACKER_H_H_

#include <bits/time.h>

extern const char E_IN_DELAY; // line 73
extern const char E_IN_SPECIAL_CONTROL; // line 74
namespace tss_dpsdk {
    namespace DPPkgDelayHacker {
        // sizeof = 24
        struct DPCommandBuf { // line 44
        public:
        unsigned int cmd_id_;
        unsigned int buf_len_;
        unsigned char *buf_;
        /*anon struct*/ int *user_;
        timeval expire_time_;
        ~DPCommandBuf();
        };
        // sizeof = 12
        struct DPCommandBuf2 { // line 77
        public:
        unsigned int cmd_id_;
        unsigned int buf_len_;
        unsigned char *buf_;
        };
    } // namespace DPPkgDelayHacker
} // namespace tss_dpsdk

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_PKG_DELAY_HACKER_H_H_
