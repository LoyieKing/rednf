// Reconstructed translation unit for the ORIG
// src/formmog/tss_sdk/busi_interf/busi_impl/tss_sdk_busi_impl.cpp CU.
//
// The ORIG CU contributes no sampled function symbols to the reconstruction;
// DWARF exposes only its namespace-scope objects.  Reproduce the real shipped
// static-initialization block instead of leaving the CU's effects absent:
//   - file-local `g_tss_sdk` (4 bytes) initialized by tss_get_sdk()
//   - Singleton<TssSdkCommSvc>::ObjectCreator weak pre-create (0x0029d800)
//   - __ioinit + COMM_STAT_ITEM_WITH_NAME[16] + status dtor registration
// audit/report/init_orig.json: status ctor 16, __ioinit 1, __cxa_atexit 3,
// status dtor 1, business {tss_get_sdk, TssSdkCommSvc::TssSdkCommSvc}.

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

namespace tss_sdk_impl {
struct TssSdkInterface;
}  // namespace tss_sdk_impl

extern "C" const tss_sdk_impl::TssSdkInterface *tss_get_sdk();

// ORIG file-local pointer (b, 4 bytes), set during dynamic initialization.
static const tss_sdk_impl::TssSdkInterface *g_tss_sdk
    __attribute__((used)) = tss_get_sdk();

#define TSS_SDK_STATIC_INIT_ID tss_sdk_busi_impl
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

// ORIG .bss 0x0029d800 (weak, shared with the other CommSvc pre-create TUs).
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
