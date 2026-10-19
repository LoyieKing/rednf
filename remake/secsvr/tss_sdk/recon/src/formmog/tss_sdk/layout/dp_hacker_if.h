// Canonical layout of the tss_dpsdk hacker interface layer.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::IDPHackerEventNotify'
//   struct tss_dpsdk::IDPHackerEventNotify size=112 bases=0 vfuncs=0
//     +0    <vptr>      func**       size=4
//     +4    hackers_    DPHACKERS[9] size=108
//   DWARF definition DIE (CU dp_cf_extract_user_info.cpp) additionally holds
//   the nested `typedef DPHACKERS`, whose target DIE is the unnamed 12-byte
//   std::vector<IDPHacker*> instantiation GCC 4.1 emitted for the typedef
//   (its 43 members are the std::vector member set and it inherits
//   _Vector_base<IDPHacker*, allocator<IDPHacker*> >).
//   _ZTVN9tss_dpsdk20IDPHackerEventNotifyE is 16 bytes: the offset-to-top /
//   typeinfo pair plus exactly two slots, i.e. one virtual destructor
//   (_ZN9tss_dpsdk20IDPHackerEventNotifyD1Ev @0x000abfc0; D0 is inlined).
//
// E_EVENT is deliberately NOT declared here: this header is on the
// plugins/proto_proc/dpinterface.h include path, and
// dpinterface_lifecycle_agent.cpp declares its own `tss_dpsdk::E_EVENT`.
// The named enum lives in layout/dp_hacker.h and converts implicitly into
// register_event()'s `int` parameter, whose mangling is unobservable because
// ORIG emits no register_event symbol at all.
//
// IDPHacker itself is only forward declared here because nothing in this
// layer needs its size; the full interface is layout/dp_hacker.h.
#ifndef TSS_SDK_LAYOUT_DP_HACKER_IF_H_
#define TSS_SDK_LAYOUT_DP_HACKER_IF_H_

#include <stddef.h>
#include <vector>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_dpsdk {

class IDPHacker;

// DWARF: `typedef DPHACKERS` nested in IDPHackerEventNotify, 12 bytes.
typedef std::vector<IDPHacker *> DPHACKERS;

class IDPHackerEventNotify {
public:
    // ORIG dp_hack_event_notify.h: weak inline virtual destructor
    // (_ZN9tss_dpsdk20IDPHackerEventNotifyD2Ev @0x000abfc0, D1 @0x000ac010):
    // the body destroys the nine vectors, so it is empty here; the weak
    // linkage comes from the in-class definition, exactly as ORIG emits it.
    virtual ~IDPHackerEventNotify() {}

    // ORIG dp_hack_event_notify.h:60..63.  It emits no symbol, so every
    // IDPHacker::init() inlines this de-duplicating push_back loop
    // (reconstructed at hackers_a_agent.cpp:343, hackers_b_agent.cpp:466,
    // dp_hack_mgr_agent.cpp:175).
    void register_event(int event, IDPHacker *hacker) {
        std::vector<IDPHacker *> &hackers = hackers_[event];
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            if (hackers[i] == hacker)
                return;
        }
        hackers.push_back(hacker);
    }

    DPHACKERS hackers_[9];   // +0x04, 9 * 12 = 108
};

TSS_SDK_STATIC_ASSERT(sizeof(DPHACKERS) == 12, dphackers_size_must_be_12);
TSS_SDK_STATIC_ASSERT(sizeof(IDPHackerEventNotify) == 112,
                      idphackereventnotify_size_must_be_112);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(IDPHackerEventNotify, hackers_) == 4,
                      idphackereventnotify_hackers_offset);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_HACKER_IF_H_
