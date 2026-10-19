// Reconstructed from libtss_sdk.so.1.0.394: DPInterface::DPInterface()
// at 0x000bf0f0 (C1) / 0x000bf200 (C2).
//
// The original ctor does not touch is_inited_; it allocates a DPMsgHandler
// (4 bytes, `new(4)` + _ZN9tss_dpsdk12DPMsgHandlerC1Ev) and a DPHackMgr
// (124 bytes, `new(0x7c)` + _ZN9tss_dpsdk9DPHackMgrC1Ev), then stores them at
// +4/+8.  Both classes now come from the canonical layout headers
// (src/formmog/tss_sdk/layout/dp_msg_handler.h and dp_hack_mgr.h); the local
// 4/124-byte `reserve_` models this file used to carry are gone.
//
// The ORIG CU dpinterface.cpp static-initialization block lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU).
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"

#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"

DPInterface::DPInterface() {
    msg_handler_ = new tss_dpsdk::DPMsgHandler();
    hack_manager_ = new tss_dpsdk::DPHackMgr();
}
