// Canonical layout of ::Comm_Stat_Monitor (comm_stat_monitor.h).
//
// ORIG evidence: python3 audit/tu_report.py --type 'Comm_Stat_Monitor'
//   struct Comm_Stat_Monitor size=140 bases=1 vfuncs=0
//     base +0   ZEN_Server_Status (76)
//     +74       stat_mmap_filename_  char[65]   (74 + 65 = 139, tail padding
//                                               to 140 for the 4-byte vptr
//                                               alignment)
//   The base subobject keeps ZEN_Server_Status's vptr at +0, so the derived
//   class adds no virtual of its own (vfuncs=0).
//
// ZEN_Server_Status (76), ZEN_STATUS_ITEM (24), ZEN_STATUS_ITEM_WITHNAME (92)
// and ZEN_ShareMem_Posix are declared once in
// commlib/zenlib/zen_server_status.h; this header includes it instead of
// repeating them.
//
// Verified mangled forms (readelf -sW on the shipped object):
//   _ZN17Comm_Stat_MonitorC1Ev / C2Ev / D0Ev / D1Ev / D2Ev
//   _ZN17Comm_Stat_Monitor10initializeEbRK11SERVICES_IDjPK24ZEN_STATUS_ITEM_WITHNAMEb
//   _ZN17Comm_Stat_Monitor28get_svr_info_from_stat_fnameEPKcP11SERVICES_IDPb
//   _ZN17Comm_Stat_Monitor17create_stat_fnameEbRK11SERVICES_ID
//   _ZN17Comm_Stat_Monitor8instanceEv
//   _ZN17Comm_Stat_Monitor14clean_instanceEv
//   _ZN17Comm_Stat_Monitor9instance_E   (singleton pointer object)
//
// TODO(migrate): the following TUs still carry their own local
// Comm_Stat_Monitor / ZEN_Server_Status models and are the wave-2 input:
//   plugins/channel_plugin_agent.cpp, plugins/proto_proc/*_agent.cpp,
//   commlib/zenlib/zen_server_status.cpp
#ifndef TSS_SDK_LAYOUT_COMM_STAT_MONITOR_H_
#define TSS_SDK_LAYOUT_COMM_STAT_MONITOR_H_

#include <stddef.h>

// SERVICES_ID is only used through references and pointers here, so a forward
// declaration is enough; that avoids pulling
// src/commlib/framework/comm_cfgsvr_sdk.h, which cannot be co-included with
// the src/origsrc TDR chain (both define tsf4g_tdr::TdrXmlFormat).  Its
// canonical declaration lives in that commlib header.
struct SERVICES_ID;
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

class Comm_Stat_Monitor : public ZEN_Server_Status {
public:
    Comm_Stat_Monitor();
    virtual ~Comm_Stat_Monitor();

    bool initialize(bool is_svr_info_valid, const SERVICES_ID &svr_info,
                    std::size_t stat_item_num,
                    const ZEN_STATUS_ITEM_WITHNAME *static_items,
                    bool create_if_absent);
    bool get_svr_info_from_stat_fname(const char *stat_fname,
                                      SERVICES_ID *svr_info, bool *is_valid);
    bool create_stat_fname(bool use_svr_info, const SERVICES_ID &svr_info);

    static Comm_Stat_Monitor *instance();
    static void clean_instance();

private:
    char stat_mmap_filename_[65];   // +0x4a

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(Comm_Stat_Monitor, stat_mmap_filename_, 74,
                                comm_stat_monitor_filename_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(Comm_Stat_Monitor) == 140,
                      comm_stat_monitor_size_must_be_140);

#endif  // TSS_SDK_LAYOUT_COMM_STAT_MONITOR_H_
