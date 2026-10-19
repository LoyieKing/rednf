// Canonical layout of tss_dpsdk::DPHackMgr.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::DPHackMgr'
//   struct tss_dpsdk::DPHackMgr size=124 bases=0 vfuncs=0
//     +0    hackers_       DPHACKERS                 size=12   (private)
//     +12   event_notify_  tss_dpsdk::IDPHackerEventNotify size=112 (private)
//   DWARF definition DIE (CU dp_hack_mgr.cpp, spec dp_hack_mgr.h:24) lists
//   `typedef DPHACKERS`, member `hackers_` @0x00, member `event_notify_`
//   @0x0c and then the member functions.  The model reports event_notify_
//   with size 0 only because the referenced IDPHackerEventNotify DIE in that
//   CU is a declaration; 12 + 112 = 124 fixes the real extent.
//
// Out-of-line members in ORIG (only these three have symbols):
//   _ZN9tss_dpsdk9DPHackMgr4initEv
//   _ZN9tss_dpsdk9DPHackMgr4hackEPN12tss_sdk_impl8UserInfoEijRPhRj
//   _ZN9tss_dpsdk9DPHackMgr10get_filterEi   (returns tagDPHackFilter&)
// The nine event dispatchers are header-inline in ORIG (no symbols): each
// walks event_notify_.hackers_[E_*] and stops at the first non-zero result.
// They are defined inline below, exactly as ORIG emits them.
//
// This header is included from plugins/proto_proc/dpinterface_deps.h, not from
// dpinterface.h: the complete IDPHacker interface is needed for the inline
// dispatch bodies.
#ifndef TSS_SDK_LAYOUT_DP_HACK_MGR_H_
#define TSS_SDK_LAYOUT_DP_HACK_MGR_H_

#include <stddef.h>
#include <time.h>

#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_if.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace sec_proto {
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;
}  // namespace sec_proto

namespace tss_dp_comm { class DPHackFilter; }
typedef tss_dp_comm::DPHackFilter tagDPHackFilter;

namespace tss_sdk_impl { struct UserInfo; }

namespace tss_dpsdk {

class DPHackMgr {
public:
    // ORIG exports the implicit ctor/dtor pair
    //   _ZN9tss_dpsdk9DPHackMgrC1Ev @0x000aba70  C2 @0x000abb40
    //   _ZN9tss_dpsdk9DPHackMgrD2Ev @0x000abad0  D1 @0x000abba0
    // Declared so dp_hack_mgr_agent.cpp can define them out of line; no
    // layout change (hackers_ +0x00, event_notify_ +0x0c, size 124).
    DPHackMgr();
    ~DPHackMgr();

    int init();
    int hack(tss_sdk_impl::UserInfo *user, int hack_type,
             unsigned int cmd_id, unsigned char *&buf, unsigned int &buf_len);

    // ORIG dp_hack_mgr.h, header-inline.  ORIG shape of every dispatcher:
    //   int iRet = 0; for (i...) { iRet = hackers[i]-><slot>(...); if (iRet != 0)
    //   break; } return iRet;
    // The caller's return value therefore carries the first non-zero hacker
    // result (DPInterface::on_recv_msg returns it straight through; the
    // add_user/del_user/check_timer/recv_buffer_for_decrypt call sites discard
    // it).  Event vectors, from the inlined call sites:
    //   on_login             E_LOGIN            dp_hack_mgr_agent.cpp init
    //   on_logout            E_LOGOUT
    //   proc                 E_PROC
    //   on_ban_mode          E_BAN_MODE         DPInterface::on_recv_msg 0xc067d
    //   on_drop_cmd          E_DROP_CMD                          ...    0xc060e
    //   on_verify_response   E_MSG2CLIENT                        ...    0xc05a5
    //   on_verify_msg        E_VERIFY_MSG                        ...    0xc06ec
    //   on_safe_mode         E_SAFE_MODE                         ...    0xc075b
    //   on_special_control   E_SPECIAL_CONTROL  recv_buffer_for_decrypt 0xc02d5
    // (vector offset = event_notify_ +0x04 + E*12; on_verify_response's
    //  +0x4c == 0x10 + 5*12 pins E_MSG2CLIENT, not E_VERIFY_MSG.)
    int on_login(tss_sdk_impl::UserInfo *user) {
        DPHACKERS &hackers = event_notify_.hackers_[E_LOGIN];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_login(user);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_logout(tss_sdk_impl::UserInfo *user) {
        DPHACKERS &hackers = event_notify_.hackers_[E_LOGOUT];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_logout(user);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int proc(time_t now) {
        DPHACKERS &hackers = event_notify_.hackers_[E_PROC];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->proc(now);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_ban_mode(tss_sdk_impl::UserInfo *user,
                    const sec_proto::DpBanModeReq &req) {
        DPHACKERS &hackers = event_notify_.hackers_[E_BAN_MODE];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_ban_mode(user, req);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_drop_cmd(tss_sdk_impl::UserInfo *user,
                    const sec_proto::DpDropCmdlistReq &req) {
        DPHACKERS &hackers = event_notify_.hackers_[E_DROP_CMD];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_drop_cmd(user, req);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_verify_response(tss_sdk_impl::UserInfo *user, unsigned char *data,
                           unsigned int len) {
        DPHACKERS &hackers = event_notify_.hackers_[E_MSG2CLIENT];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_verify_response(user, data, len);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_verify_msg(tss_sdk_impl::UserInfo *user,
                      const sec_proto::DPCaptchaResultRsp &rsp) {
        DPHACKERS &hackers = event_notify_.hackers_[E_VERIFY_MSG];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_verify_msg(user, rsp);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_safe_mode(tss_sdk_impl::UserInfo *user, int safe_mode_level) {
        DPHACKERS &hackers = event_notify_.hackers_[E_SAFE_MODE];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_safe_mode(user, safe_mode_level);
            if (iRet != 0)
                break;
        }
        return iRet;
    }
    int on_special_control(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                           unsigned int &buf_len, unsigned char *out,
                           unsigned int &out_len) {
        DPHACKERS &hackers = event_notify_.hackers_[E_SPECIAL_CONTROL];
        int iRet = 0;
        for (std::size_t i = 0; i < hackers.size(); ++i) {
            iRet = hackers[i]->on_special_control(user, buf, buf_len, out, out_len);
            if (iRet != 0)
                break;
        }
        return iRet;
    }

    // type 1 -> up filter, 2 -> down filter, anything else -> raw filter (the
    // three DPHackFilter members ORIG lays out at DPConfig +0x3da4 / +0x45a6 /
    // +0x4da8).  Out-of-line in ORIG (it needs the DP_CONFIG global), so it is
    // only declared here; dp_hack_mgr_agent.cpp defines it.
    tagDPHackFilter &get_filter(int type);

private:
    DPHACKERS hackers_;                  // +0x00
    IDPHackerEventNotify event_notify_;  // +0x0c

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPHackMgr, hackers_, 0,
                                dphackmgr_hackers_offset);
        TSS_SDK_OFFSET_CONTRACT(DPHackMgr, event_notify_, 12,
                                dphackmgr_event_notify_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPHackMgr) == 124, dphackmgr_size_must_be_124);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_HACK_MGR_H_
