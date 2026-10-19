// Canonical layout of tss_dpsdk::DPCodeMgr.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::DPCodeMgr'
//   struct tss_dpsdk::DPCodeMgr size=4 bases=0 vfuncs=0
//     +0   code_infos_   tss_dpsdk::DPCodeMgr::DpCodeInfos*   size=4
//   The DWARF definition DIE (CU dp_code_mgr.cpp) resolves the member type to
//   the nested `DpCodeInfos` structure (9604936 bytes) through a pointer, so
//   only the nested name is needed here.
//
// The out-of-line members (reload_code/reload/init) are defined by
// dp_code_mgr_agent.cpp; they are deliberately not re-declared here because
// that TU owns the nested DpCodeInfos/DpCodeInfo/CodeInfo/CodeItems<T>
// hierarchy and would otherwise have to be migrated at the same time.
#ifndef TSS_SDK_LAYOUT_DP_CODE_MGR_H_
#define TSS_SDK_LAYOUT_DP_CODE_MGR_H_

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_dpsdk {

class DPCodeMgr {
public:
    class DpCodeInfos;   // dp_code_mgr.h, nested definition

private:
    DpCodeInfos *code_infos_;   // +0x00

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPCodeMgr, code_infos_, 0,
                                dpcodemgr_code_infos_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPCodeMgr) == 4, dpcodemgr_size_must_be_4);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_CODE_MGR_H_
