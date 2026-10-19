// Reconstructed from libtss_sdk.so.1.0.394: DPInterface::instance() at 0x000bf160.
// The original guard variable/static object are at 0x002c2280/0x002c2288.
// Assembly: guard acquire -> DPInterface::DPInterface() -> guard release ->
// __cxa_atexit(dtor, &object, __dso_handle); return &object.
//
// This TU owns the ORIG CU dpinterface.cpp static-initialization block
// (__static_initialization_and_destruction_0 at 0x000c09d0, reached from
// _GLOBAL__I__ZN11DPInterface8instanceEv at 0x000c0dc0).  Decoded ORIG order:
//   ios_base::Init::Init(&__ioinit) + atexit(__tcf_0)
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (ids 0x238d..0x239c, 0x5c bytes each)
//     + atexit(__tcf_1) for the array
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() / __cxa_guard_release
//     + atexit(__tcf_2), with __cxa_guard_abort + _Unwind_Resume unwind pad
// (ORIG init block: 26 calls, target size 996.)
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

DPInterface::~DPInterface() {}

DPInterface *DPInterface::instance() {
    static DPInterface dp_interface_;
    return &dp_interface_;
}

// ORIG .bss 0x0029d800 (weak, 1 byte):
//   Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator
// Its constructor calls instance(), so the comm-service singleton is created
// during this TU's dynamic initialization, exactly as the ORIG does.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
