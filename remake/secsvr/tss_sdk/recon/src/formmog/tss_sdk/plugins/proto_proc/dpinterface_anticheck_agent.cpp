// Reconstructed from libtss_sdk.so.1.0.394: DPInterface::anticheck_pkg
// (ORIG 0x000bf270, CU dpinterface.cpp).
//
// DPInterface layout (is_inited_ +0x00, msg_handler_ +0x04, hack_manager_ +0x08)
// and both handler classes come from the canonical layout headers; the local
// models this file used to carry are gone.
//
// Static initialization of the ORIG CU dpinterface.cpp lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU and must not add a second
// block).
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"


// ---------------------------------------------------------------------------
// DPInterface::anticheck_pkg
// ---------------------------------------------------------------------------

int DPInterface::anticheck_pkg(tss_sdk_impl::UserInfo *user, int cmd_id,
                               unsigned char *buf, unsigned int len) {
    if (!is_inited_)
        return 0;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return 0;

    int res = msg_handler_->anticheck_pkg(user, cmd_id);

    // The handler returns only 0 or 1: 0 = no cheating package matched,
    // 1 = a cheating package matched (it has already billed and sent the
    // error-report / cmd-monitor messages).  The hack pass runs only for the
    // clean result and a present user.  hack() returns 0 or 1; 1 means the
    // packet was intercepted, which this method reports as failure, while 0
    // means the hack pass left the packet alone.
    if (res == 0 && user != 0) {
        res = (hack_manager_->hack(user, 4, cmd_id, buf, len) == 1) ? -1 : 0;
    }

    // buf and len are the hack call's in/out references; the log below prints
    // the possibly replaced values.  They are stack parameters passed by
    // value, so replacement does not propagate back to this method's caller.
    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], cmd_id=%u, res=%d, buf=%p, len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, cmd_id, res, buf,
        len);

    if (res == 0)
        return 0;
    // A failure result is suppressed when the config explicitly disables
    // anticheat packages; the function then reports success.
    if (tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf
            .disable_anticheat_pkg)
        return 0;
    return -1;
}
