// Canonical layout of tss_dpsdk::DPPropertyTransImpl.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::DPPropertyTransImpl'
//   struct DPPropertyTransImpl size=56 bases=1
//     base +0    tss_dpsdk::IDPHackerSafe (32)
//     +32        uid2usr_  std::map<unsigned int, tss_sdk_impl::UserInfo*>
//                          (24 bytes: _Rb_tree)
//   DWARF definition DIE: CU dp_property_trans_hacker.cpp.  ORIG lists
//   uid2usr_ directly as the std::map member; the model shows the flattened
//   `._M_t._M_impl` only because GCC 4.1 emitted the map instance unnamed.
//   Out-of-line members (orig_model 'funcs'):
//     init(IDPHackerEventNotify*), hack(UserInfo*, int, unsigned, u8*&, u32&),
//     on_logout(UserInfo*), on_timer/insert-style helpers stay with the TU.
//
// TODO(migrate): property_trans_impl_agent.cpp still declares its own
// DPPropertyTransImpl / IDPHackerSafe model.
#ifndef TSS_SDK_LAYOUT_DP_PROPERTY_TRANS_H_
#define TSS_SDK_LAYOUT_DP_PROPERTY_TRANS_H_

#include <map>

#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl { struct UserInfo; }

namespace tss_dpsdk {

class DPPropertyTransImpl : public IDPHackerSafe {
public:
    // ORIG emits weak _ZN9tss_dpsdk19DPPropertyTransImplD1Ev (0x000baa00,
    // 0x97) and D0Ev (0x000ba960, 0x9f); vtable 0x293500 (0x3c) carries them
    // in slots +0x34 / +0x38.  Without this declaration GCC 13 zeroes those
    // two slots.
    virtual ~DPPropertyTransImpl();

    // ORIG constructs the file-scope DPPropertyTransImpl object inside the
    // ORIG TU; the base IDPHackerSafe is not default-constructible because
    // ZEN_Thread_Mutex requires a name, so the owning TU provides this.
    DPPropertyTransImpl();

    int init(IDPHackerEventNotify *event_notify);
    int hack(tss_sdk_impl::UserInfo *user, int hack_type,
             unsigned int cmd_id, unsigned char *&buf,
             unsigned int &buf_len);

    // ORIG: _ZN9tss_dpsdk19DPPropertyTransImpl30forward_trade_data_to_safemodeEPN12tss_sdk_impl8UserInfoES3_ijPKhj
    // (0x000b8fb0, 1829 bytes in the shipped object), called from hack().
    // ORIG does not inline it (the call goes through the PLT at 0x84da4), so
    // the declaration carries the same noinline attribute the reconstructed
    // TU uses.
    __attribute__((noinline))
    int forward_trade_data_to_safemode(tss_sdk_impl::UserInfo *user,
                                       tss_sdk_impl::UserInfo *dst_user,
                                       int type, unsigned int cmd_id,
                                       const unsigned char *buf,
                                       unsigned int len);
    int on_logout(tss_sdk_impl::UserInfo *user);

private:
    std::map<unsigned int, tss_sdk_impl::UserInfo *> uid2usr_;   // +0x20

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPPropertyTransImpl, uid2usr_, 32,
                                dppropertytransimpl_uid2usr_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPPropertyTransImpl) == 56,
                      dppropertytransimpl_size_must_be_56);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_PROPERTY_TRANS_H_
