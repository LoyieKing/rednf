// Canonical layout of tss_dpsdk::IDPHackerSafe and tss_dpsdk::DPPkgDelayHacker.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_dpsdk::IDPHackerSafe'
//     struct IDPHackerSafe size=32 bases=1
//       base +0 tss_dpsdk::IDPHacker
//       +4      locker_  ZEN_Thread_Mutex  (the model prints size 0 because
//                                        that CU's DIE is a declaration;
//                                        the model's own ZEN_Thread_Mutex is
//                                        28 bytes, 4 + 28 = 32)
//   python3 audit/tu_report.py --type 'tss_dpsdk::DPPkgDelayHacker'
//     struct DPPkgDelayHacker size=84 bases=2
//       base +0  tss_dpsdk::IDPHackerSafe
//       base +56 <anon>  (the unnamed std::list instance, GCC 4.1 artifact)
//       +32      waitting_for_send_ std::map<unsigned, DPCommandBuf2>  24
//       +64      next_timer_        timeval   8
//       +72      flag_position_     int       4
//       +76      node_wait_for_send_ unsigned int 4
//       +80      node_in_delay_      unsigned int 4
//     DPCommandBuf 24: cmd_id_ u32 @0, buf_len_ u32 @4, buf_ u8* @8,
//                      user_ UserInfo* @12, expire_time_ timeval @16
//     DPCommandBuf2 12: cmd_id_ u32 @0, buf_len_ u32 @4, buf_ u8* @8
//   Out-of-line members (nm / orig_model 'funcs'):
//     DPPkgDelayHacker::init(IDPHackerEventNotify*), proc(time_t),
//     on_logout(UserInfo*), on_special_control(...), insert(...),
//     on_timer(DPCommandBuf&), remove_from_delay(UserInfo*, iterator&).
//
// ZEN_Thread_Mutex comes from the shared zenlib declaration; this header does
// not define it a second time.
#ifndef TSS_SDK_LAYOUT_DP_HACKER_SAFE_H_
#define TSS_SDK_LAYOUT_DP_HACKER_SAFE_H_

#include <list>
#include <map>
#include <sys/time.h>

#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_if.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl { struct UserInfo; }

namespace tss_dpsdk {

class IDPHackerSafe : public IDPHacker {
public:
    // ORIG emits no IDPHackerSafe constructor symbol: the class is built only
    // as the base subobject of its derived hackers, and ZEN_Thread_Mutex has
    // no default constructor (only ZEN_Thread_Mutex(const char *name)).  The
    // owning TU defines this with locker_((const char *)0), the same spelling
    // the other reconstructed zen call sites use.
    IDPHackerSafe();
    virtual ~IDPHackerSafe();

    // protected, not private: ORIG DPPkgDelayHacker::on_logout and
    // ::on_special_control lock/unlock through this member directly
    // (ZEN_Lock_Guard<ZEN_Thread_Mutex> on `this+4`) and no accessor symbol
    // exists.  Layout unchanged: sizeof(IDPHackerSafe) == 32, locker_ at +4.
protected:
    ZEN_Thread_Mutex locker_;   // +0x04

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(IDPHackerSafe, locker_, 4,
                                idphackersafe_locker_offset);
    }
};

class DPPkgDelayHacker : public IDPHackerSafe {
public:
    struct DPCommandBuf {          // 24 bytes
        unsigned int cmd_id_;              // +0x00
        unsigned int buf_len_;             // +0x04
        unsigned char *buf_;               // +0x08
        tss_sdk_impl::UserInfo *user_;     // +0x0c
        timeval expire_time_;              // +0x10
    };

    struct DPCommandBuf2 {         // 12 bytes
        unsigned int cmd_id_;              // +0x00
        unsigned int buf_len_;             // +0x04
        unsigned char *buf_;               // +0x08
    };

    typedef std::list<DPCommandBuf>::iterator CommandBufIterator;

    // ORIG emits weak _ZN9tss_dpsdk16DPPkgDelayHackerD1Ev (0x000b6530, 0xaa)
    // and D0Ev (0x000b6480, 0xa2), and vtable 0x293300 carries both in slots
    // +0x34 / +0x38.  Without this declaration GCC 13 leaves those two vtable
    // slots zeroed, so `delete` through IDPHacker* would jump to null.
    virtual ~DPPkgDelayHacker();

    int init(IDPHackerEventNotify *event_notify);
    // ORIG emits _ZN9tss_dpsdk16DPPkgDelayHackerC1Ev and C2Ev; the C2 out-of-
    // line constructor is what DPHackMgr::init() calls for the 0x54-byte
    // delay/mail hackers.
    DPPkgDelayHacker();
    int proc(time_t now);
    int on_logout(tss_sdk_impl::UserInfo *user);
    int on_special_control(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                           unsigned int &buf_len, unsigned char *out,
                           unsigned int &out_len);
    // ORIG vtable _ZTVN9tss_dpsdk16DPPkgDelayHackerE is 0x40 bytes = 8 + 13
    // inherited slots + this one; the relocation at vtable +0x3c is
    // _ZN9tss_dpsdk16DPPkgDelayHacker8on_timerERNS0_12DPCommandBufE, i.e. the
    // appended private virtual slot.  It must be virtual.
    virtual int on_timer(DPCommandBuf &buf);
    int insert(DPCommandBuf &buf);
    int insert(tss_sdk_impl::UserInfo *user, int delay_seconds,
               unsigned int cmd_id, unsigned char *buf, unsigned int &len);
    int remove_from_delay(tss_sdk_impl::UserInfo *user,
                          CommandBufIterator &it);

    // protected, not private: ORIG DPDelayHacker (dp_delay_hacker.h) derives
    // from DPPkgDelayHacker and reads node_wait_for_send_ (+0x4c) and
    // node_in_delay_ (+0x50) straight out of `this` (ORIG 0x0a9310..0x0a9488),
    // and no accessor symbol exists.  Layout and size are unchanged.
protected:
    std::map<unsigned int, DPCommandBuf2> waitting_for_send_;  // +0x20
    std::list<DPCommandBuf> cmd_delay_;                        // +0x38
    timeval next_timer_;                                       // +0x40
    int flag_position_;                                        // +0x48
    unsigned int node_wait_for_send_;                          // +0x4c
    unsigned int node_in_delay_;                               // +0x50

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, waitting_for_send_, 32,
                                dppkgdelayhacker_waitting_for_send_offset);
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, cmd_delay_, 56,
                                dppkgdelayhacker_cmd_delay_offset);
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, next_timer_, 64,
                                dppkgdelayhacker_next_timer_offset);
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, flag_position_, 72,
                                dppkgdelayhacker_flag_position_offset);
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, node_wait_for_send_, 76,
                                dppkgdelayhacker_node_wait_for_send_offset);
        TSS_SDK_OFFSET_CONTRACT(DPPkgDelayHacker, node_in_delay_, 80,
                                dppkgdelayhacker_node_in_delay_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(IDPHackerSafe) == 32,
                      idphackersafe_size_must_be_32);
TSS_SDK_STATIC_ASSERT(sizeof(DPPkgDelayHacker::DPCommandBuf) == 24,
                      dpcommandbuf_size_must_be_24);
TSS_SDK_STATIC_ASSERT(sizeof(DPPkgDelayHacker::DPCommandBuf2) == 12,
                      dpcommandbuf2_size_must_be_12);
TSS_SDK_STATIC_ASSERT(sizeof(DPPkgDelayHacker) == 84,
                      dppkgdelayhacker_size_must_be_84);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_HACKER_SAFE_H_
