// Reconstructed from libtss_sdk.so.1.0.394: DPInterface::init(char const*)
// original at 0x000beff0.
//
// The ZEN logging boundary is the single shared declaration
// (src/commlib/zenlib/zen_trace_log_msg.h): the ORIG's 364-byte
// ZEN_Trace_LogMsg is a libstdc++-4.1 std::ofstream artifact that no host TU
// can reproduce, and every ORIG calling CU has no DIE for the type at all.
// The local `struct ZEN_Trace_LogMsg` stub this file used to carry is gone;
// every call site below is unchanged.
//
// The ORIG CU dpinterface.cpp static-initialization block lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU).
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface_deps.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"

#include <stdlib.h>

#include "src/commlib/zenlib/zen_trace_log_msg.h"

int DPInterface::init(const char *ipc_name) {
    if (is_inited_) {
        ZEN_Trace_LogMsg::debug_errorex("DPInterface already inited");
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx("DPInterface init, ipc_name=%s", ipc_name);
    tss_dpsdk::DPBillAdapter::init((unsigned int)strtol(ipc_name, 0, 10));

    int res = tss_dpsdk::DPMonitorReporter::init();
    if (res) {
        ZEN_Trace_LogMsg::debug_errorex("DPMonitorReporter init failed, res=%d", res);
        return -1;
    }
    res = msg_handler_->init();
    if (res) {
        ZEN_Trace_LogMsg::debug_errorex("DPMsgHandler init failed, res=%d", res);
        return -1;
    }
    res = hack_manager_->init();
    if (res) {
        ZEN_Trace_LogMsg::debug_errorex("DPHackMgr init failed, res=%d", res);
        return -1;
    }
    is_inited_ = 1;
    return 0;
}
