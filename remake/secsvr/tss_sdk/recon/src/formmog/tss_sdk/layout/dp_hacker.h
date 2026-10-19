// Canonical layout and interface of tss_dpsdk::IDPHacker.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_dpsdk::IDPHacker'
//     struct tss_dpsdk::IDPHacker size=4 bases=0
//       +0  <vptr>  func**  size=4
//   DWARF definition DIE (CU dp_cf_extract_user_info.cpp) lists every slot
//   with DW_AT_virtuality=1: init, hack, on_login, on_logout, proc,
//   on_ban_mode, on_drop_cmd, on_verify_response, on_verify_msg,
//   on_safe_mode, on_special_control, ~IDPHacker.  ORIG vtable
//   _ZTVN9tss_dpsdk9IDPHackerE (0x00292f60, 0x3c bytes) has the same order;
//   slot 1 (hack) is __cxa_pure_virtual.
//
// Signatures from /tmp/orig_model.json 'funcs' (mangled names):
//   _ZN9tss_dpsdk9IDPHacker4initEPNS_20IDPHackerEventNotifyE
//   _ZN9tss_dpsdk9IDPHacker8on_loginEPN12tss_sdk_impl8UserInfoE
//   _ZN9tss_dpsdk9IDPHacker9on_logoutEPN12tss_sdk_impl8UserInfoE
//   _ZN9tss_dpsdk9IDPHacker4procEl
//   _ZN9tss_dpsdk9IDPHacker11on_ban_modeEPN12tss_sdk_impl8UserInfoERKN9sec_proto12DpBanModeReqE
//   _ZN9tss_dpsdk9IDPHacker11on_drop_cmdEPN12tss_sdk_impl8UserInfoERKN9sec_proto16DpDropCmdlistReqE
//   _ZN9tss_dpsdk9IDPHacker18on_verify_responseEPN12tss_sdk_impl8UserInfoEPhj
//   _ZN9tss_dpsdk9IDPHacker13on_verify_msgEPN12tss_sdk_impl8UserInfoERKN9sec_proto18DPCaptchaResultRspE
//   _ZN9tss_dpsdk9IDPHacker12on_safe_modeEPN12tss_sdk_impl8UserInfoEi
//   _ZN9tss_dpsdk9IDPHacker18on_special_controlEPN12tss_sdk_impl8UserInfoEPhRjS4_S5_
//   hack has no symbol (pure virtual); its shape is fixed by
//   _ZN9tss_dpsdk9DPHackMgr4hackEPN12tss_sdk_impl8UserInfoEijRPhRj and by the
//   derived DPPropertyTransImpl/DPDelayHacker overrides.
//
// This header is opt-in: dpinterface_lifecycle_agent.cpp and
// dpinterface_onrecv_agent.cpp still declare their own IDPHacker model, so it
// is not included by plugins/proto_proc/dpinterface.h.
#ifndef TSS_SDK_LAYOUT_DP_HACKER_H_
#define TSS_SDK_LAYOUT_DP_HACKER_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/dp_hacker_if.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl { struct UserInfo; }

namespace sec_proto {
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;
}  // namespace sec_proto

namespace tss_dpsdk {

// ORIG dp_hacker_if.h: indexes the per-event hacker vectors.  ORIG emits no
// enum symbol; the values are the E_* dispatch order of DPHackMgr.
enum E_EVENT {
    E_LOGIN = 0,
    E_LOGOUT = 1,
    E_PROC = 2,
    E_BAN_MODE = 3,
    E_DROP_CMD = 4,
    E_MSG2CLIENT = 5,
    E_VERIFY_MSG = 6,
    E_SAFE_MODE = 7,
    E_SPECIAL_CONTROL = 8,
    E_EVENT_COUNT = 9
};

class IDPHacker {
public:
    virtual int init(IDPHackerEventNotify *event_notify);
    virtual int hack(tss_sdk_impl::UserInfo *user, int hack_type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &buf_len) = 0;
    virtual int on_login(tss_sdk_impl::UserInfo *user);
    virtual int on_logout(tss_sdk_impl::UserInfo *user);
    virtual int proc(time_t now);
    virtual int on_ban_mode(tss_sdk_impl::UserInfo *user,
                            const sec_proto::DpBanModeReq &req);
    virtual int on_drop_cmd(tss_sdk_impl::UserInfo *user,
                            const sec_proto::DpDropCmdlistReq &req);
    virtual int on_verify_response(tss_sdk_impl::UserInfo *user,
                                   unsigned char *buf, unsigned int len);
    virtual int on_verify_msg(tss_sdk_impl::UserInfo *user,
                              const sec_proto::DPCaptchaResultRsp &rsp);
    virtual int on_safe_mode(tss_sdk_impl::UserInfo *user, int mode);
    virtual int on_special_control(tss_sdk_impl::UserInfo *user,
                                   unsigned char *buf, unsigned int &buf_len,
                                   unsigned char *out, unsigned int &out_len);
    virtual ~IDPHacker();
};

TSS_SDK_STATIC_ASSERT(sizeof(IDPHacker) == 4, idphacker_size_must_be_4);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_HACKER_H_
