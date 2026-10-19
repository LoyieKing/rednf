// Canonical layout of tss_dpsdk::DPBillAdapter and tss_dpsdk::DPMonitorReporter.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_dpsdk::DPBillAdapter'
//     struct tss_dpsdk::DPBillAdapter size=4 bases=0 vfuncs=0
//       +0   user_   <anon>*   size=4        (private)
//     DWARF definition DIE (CU dp_bill_adapter.cpp) additionally lists the
//     statics channel_id_ and bill_ (statics do not contribute to sizeof);
//     the pointer target is the UserInfo definition DIE (400 bytes), which
//     GCC 4.1 emitted unnamed, hence `<anon>*`.
//   python3 audit/tu_report.py --type 'tss_dpsdk::DPMonitorReporter'
//     struct tss_dpsdk::DPMonitorReporter size=1 bases=0 vfuncs=0 (no members)
//     DWARF definition DIE (CU dp_monitor_reporter.cpp) lists the statics
//     last_send_time_ / report_value_ and the members DPMonitorReporter,
//     init, reload, report_data, check_timer.
//
// The constructor/destructor/static declarations below are the ones the
// reconstruction already used; their mangled names are unchanged
// (_ZN9tss_dpsdk13DPBillAdapterC1EPKcjPN12tss_sdk_impl8UserInfoE,
//  _ZN9tss_dpsdk13DPBillAdapter4initEj, _ZN9tss_dpsdk17DPMonitorReporter4initEv).
// ORIG DWARF types DPBillAdapter::init and DPBillAdapter::reload as returning
// int; the shared declaration keeps the existing `void init` spelling because
// the return type is not part of the mangled name and no rebuilt TU consumes
// the result yet (see the migration note in dpinterface_deps.h).
#ifndef TSS_SDK_LAYOUT_DP_BILL_H_
#define TSS_SDK_LAYOUT_DP_BILL_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"
namespace tss_sdk_impl { struct UserInfo; }

namespace tss_dpsdk {

class DPBillAdapter {
public:
    DPBillAdapter(const char *msg, unsigned int type,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();
    // ORIG header-inline forwarders to the static recorder (`bill_`, typed
    // DPBillRecorder per DWARF; it is a static, so sizeof stays 4).
    template <typename T>
    DPBillAdapter &operator<<(T value) {
        bill_ << value;
        return *this;
    }
    template <typename T, int N>
    DPBillAdapter &operator<<(const T (&value)[N]) {
        bill_ << value;
        return *this;
    }

    // ORIG: int DPBillAdapter::init(unsigned int) / ::reload() / ::dump(...)
    // (_ZN9tss_dpsdk13DPBillAdapter4initEj / 6reloadEv / 4dumpEPKcjPKhj).
    // The return type is not part of the mangled name.
    static int init(unsigned int uin);
    static int reload();
    static int dump(const char *msg, unsigned int uin,
                    const unsigned char *buf, unsigned int len);
    static DPBillRecorder bill_;
private:
    tss_sdk_impl::UserInfo *user_;   // +0x00

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPBillAdapter, user_, 0,
                                dpbilladapter_user_offset);
    }
};

// ORIG has no data members (sizeof 1); all four entry points are out-of-line
// statics: _ZN9tss_dpsdk17DPMonitorReporter4initEv, 6reloadEv,
// 11report_dataEjj, 11check_timerEl.  dp_bill_monitor_agent.cpp defines them.
class DPMonitorReporter {
public:
    static int init();
    static int reload();
    static int report_data(unsigned int id, unsigned int value);
    static int check_timer(time_t now);
};

TSS_SDK_STATIC_ASSERT(sizeof(DPBillAdapter) == 4,
                      dpbilladapter_size_must_be_4);
TSS_SDK_STATIC_ASSERT(sizeof(DPMonitorReporter) == 1,
                      dpmonitorreporter_size_must_be_1);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_BILL_H_
