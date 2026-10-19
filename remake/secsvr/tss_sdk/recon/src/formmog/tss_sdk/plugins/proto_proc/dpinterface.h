// Shared declaration header for the reconstructed DPInterface translation
// units.  Every type below is declared once in
// src/formmog/tss_sdk/layout/; this header only selects which of those
// canonical declarations a translation unit sees.
//
// ORIG scopes both handlers in tss_dpsdk (nm:
// _ZN9tss_dpsdk12DPMsgHandlerC1Ev / _ZN9tss_dpsdk9DPHackMgrC1Ev).
//
// TUs that still carry their own partial handler models define
// TSS_RECON_NO_HANDLER_MODELS before including this header; only the forward
// declarations are then visible.  Those models are wave-2 migration input:
//   // TODO(migrate): dpinterface_anticheck_agent.cpp declares its own
//        tss_dpsdk::DPMsgHandler / DPHackMgr.
//   // TODO(migrate): dpinterface_buffer_agent.cpp declares its own
//        tss_dpsdk::DPMsgHandler / DPHackMgr / IDPHacker /
//        IDPHackerEventNotify.
//   // TODO(migrate): dpinterface_onrecv_agent.cpp declares its own
//        tss_dpsdk::IDPHacker / IDPHackerEventNotify / DPHackMgr.
//   // TODO(migrate): dpinterface_reload_agent.cpp declares its own
//        tss_dpsdk::DPMsgHandler.
//   // TODO(migrate): dpinterface_lifecycle_agent.cpp declares its own
//        tss_dpsdk::IDPHacker / DPBillAdapter / DPMonitorReporter /
//        DPBillRecorder (it does not define TSS_RECON_NO_HANDLER_MODELS, so
//        the canonical DPHackMgr below is complete there).
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/dp_interface.h"

namespace tss_sdk_impl { struct UserInfo; }

// Only forward declarations here: DPInterface stores pointers to both
// handlers.  The complete canonical declarations (with DPHackMgr's ORIG
// header-inline event dispatchers, which need the full IDPHacker interface)
// are pulled in by dpinterface_deps.h, which every DPInterface implementation
// TU includes.
namespace tss_dpsdk { class DPMsgHandler; class DPHackMgr; }


#endif  // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_
