// Canonical dependency declarations needed by the reconstructed DPInterface.
//
// These are not models: every type is declared once in
// src/formmog/tss_sdk/layout/.  The real objects are linked from the
// corresponding TSS SDK translation units.
//
// TODO(migrate): the following TUs still declare their own DPBillAdapter /
// DPMonitorReporter / DPBillRecorder and will move to these headers in wave 2:
//   dpinterface_lifecycle_agent.cpp, dpinterface_onrecv_agent.cpp,
//   dpinterface_buffer_agent.cpp, dpinterface_reload_agent.cpp,
//   hackers_a_agent.cpp, hackers_b_agent.cpp, dp_msg_handler_send_agent.cpp,
//   dp_msg_handler_buffer_agent.cpp, dp_msg_packer_core_agent.cpp,
//   dp_bill_monitor_agent.cpp, bill_cf_hackers_agent.cpp,
//   dp_msg_handler_core_agent.cpp, dp_vmbase_engine_agent.cpp,
//   dp_msg_handler_recv_agent.cpp, property_trans_impl_agent.cpp.
// DPBillAdapter::init/dump/reload and the four DPMonitorReporter statics are
// declared in layout/dp_bill.h with their ORIG return types (int;
// _ZN9tss_dpsdk13DPBillAdapter4initEj, _ZN9tss_dpsdk13DPBillAdapter6reloadEv,
// _ZN9tss_dpsdk13DPBillAdapter4dumpEPKcjPKhj,
// _ZN9tss_dpsdk17DPMonitorReporter4initEv / 6reloadEv / 11report_dataEjj /
// 11check_timerEl).  The return type is not part of any of those names.
#ifndef TSS_SDK_RECON_DPINTERFACE_DEPS_H_
#define TSS_SDK_RECON_DPINTERFACE_DEPS_H_

#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_code_mgr.h"
#include "src/formmog/tss_sdk/layout/dp_hack_dnf.h"
#include "src/formmog/tss_sdk/layout/dp_hacker.h"

// The two handler classes now come from the canonical headers instead of
// dpinterface.h: DPHackMgr's ORIG header-inline event dispatchers need the
// complete IDPHacker interface, which only the implementation TUs (this
// header's consumers) may see.
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"

// The channel and uic plugin layouts are NOT pulled in here.  They embed
// zenlib/ZEN_BusPipe_TwoWay and drag src/closing/dp_config.h (TDR/rapidxml)
// into every dpinterface TU, which changes the audit surface for unrelated
// types.  Their translation units include them directly:
//   layout/tss_sdk_channel.h  <- channel_plugin_agent.cpp, channel_misc_agent.cpp
//   layout/tss_sdk_uic.h      <- uic_plugin_agent.cpp, uic_manager_agent.cpp
//   layout/dp_hacker_safe.h, layout/dp_vmbase_engine.h, layout/user_mgr.h,
//   layout/dp_property_trans.h, layout/dp_bill_recorder.h, layout/zen_bus_pipe.h
//   <- their owning *.cpp (wave 2; all of them compile standalone, see
//   layout/*.h header comments for the exact local bodies to delete).
#include "src/formmog/tss_sdk/layout/user_info.h"

#include "src/commlib/zenlib/zen_recon_support.h"

#endif  // TSS_SDK_RECON_DPINTERFACE_DEPS_H_
