// Reconstructed from libtss_sdk.so.1.0.394:
//   is_need_encrypt 0x000bf3c0
//   is_need_decrypt 0x000bf470
//   check_timer(user, now) 0x000bef90
// ORIG checks is_inited_ and DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp
// before forwarding. The config pointer is initialized by init_dp_config().
// No separate enable byte exists in the original implementation.
//
// tss_dp_comm::DPConfig/DPConfigBase/DP_CONFIG come from the canonical
// configuration header (src/closing/dp_config.h), which reuses the ORIG-DWARF
// formmog_conf_tsssdk_dp.h layouts.  Both handler classes come from the
// canonical layout headers.
//
// The ORIG CU dpinterface.cpp static-initialization block lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU).
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface_deps.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"

bool DPInterface::is_need_encrypt(tss_sdk_impl::UserInfo *user, int cmd_id) {
    if (!is_inited_ ||
        !tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return false;
    const bool res = msg_handler_->is_need_encrypt(user, cmd_id);
    ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY_LEVEL_2, "DPInterface::is_need_encrypt",
                                   cmd_id, (int)res);
    return res;
}

bool DPInterface::is_need_decrypt(tss_sdk_impl::UserInfo *user, int cmd_id) {
    if (!is_inited_ ||
        !tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return false;
    const bool res = msg_handler_->is_need_decrypt(user, cmd_id);
    ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY_LEVEL_2, "DPInterface::is_need_decrypt",
                                   cmd_id, (int)res);
    return res;
}

int DPInterface::check_timer(tss_sdk_impl::UserInfo *user, time_t now) {
    if (!is_inited_ ||
        !tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return 0;
    if (user->user_code_info_.client_state_ == tss_sdk_impl::ECS_NOT_READY)
        return 0;
    return msg_handler_->check_timer(user, now);
}
