// Shared declaration header for tss_sdk_impl::UserInfo.
//
// The canonical 400-byte layout (and every nested aggregate) is declared once
// in src/formmog/tss_sdk/layout/user_info.h.  ORIG evidence for each offset
// is recorded there; the previous ad-hoc model in this file had `room_id_`
// split into two 2-byte members at +8/+10 and an opaque `hack_data_` tail.
//
// TUs that still carry their own UserInfo model are wave-2 migration input:
//   // TODO(migrate): dp_hack_mgr_agent.cpp, hackers_a/b/c_agent.cpp,
//        dp_msg_handler_core_agent.cpp, dp_msg_handler_recv_agent.cpp,
//        dp_vmbase_engine_agent.cpp, user_mgr_agent.cpp, uic_manager_agent.cpp,
//        usercontrol_plugin_agent.cpp, proto_proc_plugin_agent.cpp declare
//        their own tss_sdk_impl::UserInfo (none of them include this header).
#ifndef TSS_SDK_RECON_USER_INFO_H_
#define TSS_SDK_RECON_USER_INFO_H_

#include "src/formmog/tss_sdk/layout/user_info.h"

#endif  // TSS_SDK_RECON_USER_INFO_H_
