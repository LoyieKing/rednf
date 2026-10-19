// Canonical layout of tss_sdk_impl::TssSdkFramework.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_sdk_impl::TssSdkFramework'
//   struct tss_sdk_impl::TssSdkFramework size=1 bases=0 vfuncs=0
//   no members; the DWARF definition DIE in CU tss_sdk_framework.cpp carries
//   only member functions (the framework entry points are file-scope symbols).
//
// The remaining framework types are already declared once in the shared
// framework headers and are deliberately not repeated here:
//   tss_sdk_if::ETssSdkPluginId, tss_sdk_impl::ITssSdkPlugin (16 bytes)
//     -> src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h
//   tss_sdk_impl::TssSdkPluginMgr (56 bytes), TssSdkCommSvc (40 bytes),
//   TssSdkChannelStatus (24 bytes)
//     -> src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h
//   tss_sdk_impl::TssSdkChannelPlugin (75908) / CIpcChannel (75856) and
//   tss_sdk_impl::TssSdkUicPlugin (76) / UicUserInputInfo (36) are still
//   declared inside their plugin .cpp files (see the report for the exact
//   migration list and the missing ZEN_BusPipe_TwoWay prerequisite).
//
// tss_sdk_impl::UicUserInputInfoMgr / UicUserInputInfo / TssSdkUicPlugin are
// declared in layout/tss_sdk_uic.h.
#ifndef TSS_SDK_LAYOUT_TSS_SDK_FRAMEWORK_H_
#define TSS_SDK_LAYOUT_TSS_SDK_FRAMEWORK_H_

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl {

// sizeof = 1 (empty class, no data members).
class TssSdkFramework {
public:
    // ORIG defines the framework entry points as file-scope symbols
    // (tss_sdk_framework.cpp); no virtual and no non-static data members.
private:
    static void tss_sdk_layout_contract_() {
        TSS_SDK_STATIC_ASSERT(sizeof(TssSdkFramework) == 1,
                              tsssdkframework_size_must_be_1);
    }
};

}  // namespace tss_sdk_impl

#endif  // TSS_SDK_LAYOUT_TSS_SDK_FRAMEWORK_H_
