// Reconstructed from libtss_sdk.so.1.0.394
//   DPInterface::check_and_reload_config()   0x000c0de0 .. 0x000c0ef7
//
// The body follows the emitted GCC 4.1 i386 control flow and the DWARF line
// table:
//   dpinterface.cpp:80   inlined tss_dp_comm::check_dp_config()
//                        (dp_config.h:85) followed by
//                        tss_dp_comm::reload_dp_config()
//   dpinterface.cpp:82   tss_dpsdk::DPBillAdapter::reload()
//   dpinterface.cpp:84   tss_dpsdk::DPMonitorReporter::reload()
//   dpinterface.cpp:86   msg_handler_->reload()
//   dpinterface.cpp:88   for each entry of
//                        DP_CONFIG::glb_dp_config_->uin_monitor_list
//   dpinterface.cpp:91   tss_sdk_impl::get_sdk_comm_svc()
//   dpinterface.cpp:92   TssSdkCommSvc::get_user_info_by_uin()
//   dpinterface.cpp:94   UserInfo::set_trace_flag() (inlined set_user_flag(),
//                        user_info.h:59/64 -> flags_ |= 1)
//   dpinterface.cpp:99   return 0
//
// The tss_dp_comm configuration comes from the canonical header
// (src/closing/dp_config.h -> formmog_conf_tsssdk_dp.h, ORIG DWARF layouts);
// DPBillAdapter comes from layout/dp_bill.h and DPMsgHandler from
// layout/dp_msg_handler.h.  The local models this file used to carry are gone.
// `check_dp_config()` stays here because it is a header-inline function of
// dp_config.h that the canonical header does not spell out; its ORIG address
// space and byte layout are unchanged.
//
// The ORIG CU dpinterface.cpp static-initialization block lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU).
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

#include "src/commlib/zenlib/zen_singleton.h"

namespace tss_dp_comm {

// dp_config.h:82/85, inlined at dpinterface.cpp:80.  Returns non-zero when a
// live config header exists and its update_time differs from the cached value,
// i.e. when the config must be reloaded.
inline int check_dp_config()
{
    return DP_CONFIG::glb_dp_config_head_ != 0 &&
           DP_CONFIG::glb_dp_config_head_->update_time !=
               DP_CONFIG::glb_config_update_time_;
}

}  // namespace tss_dp_comm

namespace tss_sdk_impl {

inline TssSdkCommSvc *get_sdk_comm_svc()
{
    return Singleton<TssSdkCommSvc>::instance();
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// DPInterface::check_and_reload_config()  @ 0x000c0de0
//
// 0xc0df4  glb_dp_config_head_ == 0                    -> return 0
// 0xc0e00  head->update_time == glb_config_update_time_ -> return 0
// 0xc0e0c  reload_dp_config() != 0                     -> return 0
// 0xc0e1f  DPBillAdapter::reload()
// 0xc0e24  DPMonitorReporter::reload()
// 0xc0e29  this->msg_handler_->reload()   (this + 0x04)
// 0xc0e37  glb_dp_config_ loaded; uin_monitor_list.length == 0 -> return 0
// 0xc0e49  unsigned i = 0
// 0xc0e80  instance() guard / construction of the function-local static
// 0xc0e50  get_sdk_comm_svc()->get_user_info_by_uin(uin_monitor_list.values[i])
// 0xc0e66  user != 0 -> user->set_trace_flag()   (flags_ |= 1 at +0x20)
// 0xc0e6a  ++i; reload length; loop while i < length
// 0xc0e15  return 0
//
// Every normal return path yields 0.  The only other exit is the compiler's
// __cxa_guard_abort/_Unwind_Resume path for the singleton's static guard.
// ---------------------------------------------------------------------------
int DPInterface::check_and_reload_config()
{
    if (!tss_dp_comm::check_dp_config() ||
        tss_dp_comm::reload_dp_config() != 0)
        return 0;

    tss_dpsdk::DPBillAdapter::reload();

    // ORIG 0xc0e24: _ZN9tss_dpsdk17DPMonitorReporter6reloadEv.  The return
    // value is discarded.  layout/dp_bill.h does not declare it yet, so the
    // symbol is declared here with the exact ORIG signature (the definition is
    // in dp_bill_monitor_agent.cpp).
    tss_dpsdk::DPMonitorReporter::reload();

    msg_handler_->reload();

    for (unsigned int i = 0;
         i < tss_dp_comm::DP_CONFIG::glb_dp_config_->
                 uin_monitor_list.length;
         ++i) {
        tss_sdk_impl::UserInfo *user =
            tss_sdk_impl::get_sdk_comm_svc()->get_user_info_by_uin(
                tss_dp_comm::DP_CONFIG::glb_dp_config_->
                    uin_monitor_list.values[i]);
        if (user != 0) {
            // Inlined UserInfo::set_trace_flag() -> set_user_flag(),
            // user_info.h:59/64.  flags_ is base_info_+0x20 (UserInfo+0x20).
            user->base_info_.flags_ |= 1u;
        }
    }

    return 0;
}
