// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/proto_proc/release/include/dp_config.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_CONFIG_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_CONFIG_H_H_

#include "output/protocol/formmog/release/include/formmog_conf_tsssdk_dp.h"

extern /*anon struct*/ int *glb_dp_config_head_; // line 69
extern unsigned int glb_config_update_time_; // line 70
extern short unsigned int i; // line 21
extern /*anon struct*/ int *glb_dp_config_; // line 68
namespace tss_dp_comm {
    extern short unsigned int get_value_posi<tss_dp_comm::DPDropCmd>(const /*anon struct*/ int &arg0, unsigned int arg1); // line 19
    extern bool is_value_exist<tss_dp_comm::DPValueArray_8>(const /*anon struct*/ int &arg0, unsigned int arg1); // line 32
    extern int is_match_filter(const tss_dp_comm::tagDPSubCmdList &arg0, unsigned char arg1, unsigned int arg2); // line 48
    extern int is_subcmd_exist(const tss_dp_comm::tagDPSubCmdList &arg0, unsigned char *arg1, short unsigned int arg2, unsigned int &arg3); // line 59
    extern short unsigned int get_value_posi<tss_dp_comm::DPBanMode>(const /*anon struct*/ int &arg0, unsigned int arg1); // line 19
    extern bool is_value_exist<tss_dp_comm::DPValueArray_128>(const /*anon struct*/ int &arg0, unsigned int arg1); // line 32
    extern int check_dp_config(); // line 82
} // namespace tss_dp_comm


extern int cmp_dphackfragment(const void *lf, const void *rt); // line 25 (inferred)
extern int cmp_dpsubcmdlist(const void *lf, const void *rt); // line 19 (inferred)
extern int cmp_uint32(const void *lf, const void *rt); // line 13 (inferred)
extern void sort_dp_config(/*anon struct*/ int &const dp_config); // line 31 (inferred)

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_RELEASE_INCLUDE_DP_CONFIG_H_H_
