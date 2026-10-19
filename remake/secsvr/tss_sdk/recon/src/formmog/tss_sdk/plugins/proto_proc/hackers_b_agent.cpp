// Semantic reconstruction of four tss_dpsdk protocol hackers from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC):
//
//   tss_dpsdk::DPMailVerifyHacker    (src/.../dp_mail_verify_hacker.cpp)
//   tss_dpsdk::DPForbidModeHacker    (src/.../dp_forbid_mode_hacker.cpp)
//   tss_dpsdk::DPSafeModeHacker      (src/.../dp_safe_mode_hacker.cpp)
//   tss_dpsdk::DPUicTransHacker      (src/.../dp_uic_trans_hacker.cpp)
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   DPMailVerifyHacker::on_special_control      0x000ac110..0x000ac17a (0x6a)
//   DPMailVerifyHacker::cancel_mail_opt         0x000ac180..0x000ac1b9 (0x39)
//   DPMailVerifyHacker::on_timer                0x000ac1c0..0x000ac291 (0xd1)
//   DPMailVerifyHacker::is_permit_throught      0x000ac2a0..0x000ac412 (0x172)
//   DPMailVerifyHacker::on_verify_response      0x000ac420..0x000ac627 (0x207)
//   DPMailVerifyHacker::on_verify_msg           0x000ac630..0x000ac783 (0x153)
//   DPMailVerifyHacker::hack                    0x000acba0..0x000ad71e (0xb7e)
//   DPMailVerifyHacker::init                    0x000ad770..0x000ad95c (0x1ec)
//   DPForbidModeHacker::hack                    0x000aa3b0..0x000aa572 (0x1c2)
//   DPForbidModeHacker::on_ban_mode             0x000aa580..0x000aa6e4 (0x164)
//   DPForbidModeHacker::init                    0x000aaab30..0x000aaba5 (0x75)
//   DPSafeModeHacker::hack                      0x000bb2f0..0x000bb3cd (0xdd)
//   DPSafeModeHacker::on_safe_mode              0x000bb3d0..0x000bb471 (0xa1)
//   DPSafeModeHacker::init                      0x000bb8c0..0x000bb935 (0x75)
//   DPUicTransHacker::hack                      0x000bbe30..0x000bc556 (0x726)
//
// Class layouts (DWARF):
//   IDPHacker                      4   vptr
//   IDPHackerSafe                 32   IDPHacker + ZEN_Thread_Mutex(28) at +4
//   DPPkgDelayHacker              84   safe base + map<u32,DPCommandBuf2> @32,
//                                      list<DPCommandBuf> @56, timeval @64,
//                                      int @72, u32 node_wait_for_send_ @76,
//                                      u32 node_in_delay_ @80
//   DPMailVerifyHacker            96   delay base + captcha_no_response_ @84,
//                                      captcha_counts_ @88,
//                                      last_captcha_retry_ @92
//   DPForbidModeHacker/DPUicTransHacker/DPSafeModeHacker  4 (vptr only)
//
// DPMailVerifyHacker::on_timer is a virtual added by DPPkgDelayHacker after
// the inherited destructor slots; the four vtable shapes are
//   init/hack/on_login/on_logout/proc/on_ban_mode/on_drop_cmd/
//   on_verify_response/on_verify_msg/on_safe_mode/on_special_control/
//   ~D1/~D0[/on_timer].
//
// External boundaries (defined by libtss_sdk.so, intentionally only declared
// here): IDPHacker base implementation, DPPkgDelayHacker construction/
// destruction/insert/remove/on_timer, DPBillAdapter, CommBillRecord, the
// ZEN trace/lock/time primitives, TssSdkCommSvc, DPMsgPacker,
// DPMonitorReporter, Comm_App_Frame and the generated protocol codecs.

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <list>
#include <map>
#include <new>
#include <sstream>
#include <string>
#include <vector>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ---------------------------------------------------------------------------
// ZEN trace boundary.  Values from zen_trace_log_basic.h.
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_trace_log_msg.h"
// declared.  The target vtable is (vptr = _ZTV16ZEN_Thread_Mutex + 8):
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_guard.h"
// ORIG .data 0x297964, tss_dpsdk::cf_ban_mode_buf (11 bytes, symbol
// _ZN9tss_dpsdk15cf_ban_mode_bufE).  Used by DPForbidModeHacker.
namespace tss_dpsdk {
unsigned char cf_ban_mode_buf[11] = {
    0xf1, 0x04, 0x00, 0x01, 0x43, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf2
};
}  // namespace tss_dpsdk

// Canonical tag-394 declarations: UserInfo, the hacker hierarchy, the DNF
// packet views, CommBillRecord, Comm_App_Frame/SERVICES_ID and the shared
// DPConfig/DP_CONFIG.  The former local models are gone.
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"  // CommBillRecord
#include "src/formmog/tss_sdk/layout/dp_hack_dnf.h"
#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"
#include "src/formmog/tss_sdk/layout/user_info.h"
// ---------------------------------------------------------------------------
namespace sec_proto {

struct DpBanModeDetail {
    uint32_t mode_id_;                  // +0
    uint32_t keep_time_;                // +4
};

struct DpBanModeReq {
    uint16_t channel_id_;               // +0
    uint16_t room_id_;                  // +2
    uint32_t count_;                    // +4
    DpBanModeDetail modes_[32];         // +8
} __attribute__((packed));

struct DPCaptchaResultRsp {
    uint16_t channel_id_;               // +0
    uint16_t room_id_;                  // +2
    uint8_t result_;                    // +4
} __attribute__((packed));

struct DpDropCmdlistReq;

struct ClientPkgHead {
    uint16_t protocol_id_;              // +0
    uint32_t seq_;                      // +2
    uint16_t sln_id_;                   // +6
    uint16_t sub_sln_id_;               // +8
    uint16_t body_len_;                 // +10
    int unpack(const char *buf, unsigned int buf_len,
               unsigned int *used_len, unsigned int version);
} __attribute__((packed));

struct ReqUICTransAdaptMsgForwardPkg {
    uint16_t world_id_;                     // +0x0000
    uint32_t src_id_;                       // +0x0002
    uint32_t cmd_id_;                       // +0x0006
    uint8_t need_encrypt_;                  // +0x000a
    uint8_t is_encrypted_;                  // +0x000b
    uint16_t cmd_buf_len_;                  // +0x000c
    unsigned char cmd_buf_[0x8000];         // +0x000e
    uint16_t rolename_len_;                 // +0x800e
    unsigned char rolename_[0x100];         // +0x8010
    uint32_t client_ip_;                    // +0x8110
    int pack(char *buf, unsigned int buf_len,
             unsigned int *used_len, unsigned int version) const;
} __attribute__((packed));

struct ReqSecsvrForwardData {
    uint16_t channel_id_;                   // +0
    uint16_t room_id_;                      // +2
    uint16_t data_len_;                     // +4
    unsigned char data_[0x7fff];            // +6
    int pack(char *buf, unsigned int buf_len,
             unsigned int *used_len, unsigned int version) const;
} __attribute__((packed));

}  // namespace sec_proto

// ---------------------------------------------------------------------------
// tss_dp_comm configuration.  Sizes/offsets from DWARF:
//   DPConfig     22842 (0x593a); cmd_hack at 0x3d7e
//   DPCmdHack     7100 (0x1bbc)
//     safemode_to_client  +0x23
//     safemode_hint_client +0x24
//     captch_method       +0x182c (absolute DPConfig +0x55aa)
//     ban_mode            +0x1ae0 (absolute DPConfig +0x585e)
//   DPCaptchaMethod 16, DPBanMode 130 (u16 length + u32 values[32]).
// ---------------------------------------------------------------------------
// Global in the shipped library (DWARF/nM: CommBillRecord::operator<<).

namespace tss_dpsdk {

class DPBillAdapter {
public:
    DPBillAdapter(const char *bill_type, unsigned int line,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();

    static CommBillRecord bill_;

private:
    tss_sdk_impl::UserInfo *user_;  // +0x00
};

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// tss_sdk_impl::TssSdkCommSvc singleton (DWARF size 40).  Only channel_id_
// (+0) and send_buf_to_backend are used here.
// ---------------------------------------------------------------------------

// zen_singleton.h: canonical declaration shared by every reconstructed TU.
// The out-of-line weak instance() is owned by tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice is the primary reconstruction
// for four ORIG CUs:
//   dp_forbid_mode_hacker.cpp, dp_mail_verify_hacker.cpp,
//   dp_safe_mode_hacker.cpp, dp_uic_trans_hacker.cpp
// Each carries the independent 996-byte block:
//   ios_base::Init 1, __cxa_atexit 3, ZEN_STATUS_ITEM_WITHNAME ctor 16,
//   guard acquire/release 1, TssSdkCommSvc::TssSdkCommSvc 1,
//   ~ZEN_STATUS_ITEM_WITHNAME 1, __cxa_guard_abort 1, _Unwind_Resume 1.
// audit/report/init_brief.md.  The 16-entry status array and __ioinit are
// TU-local objects: emit one real block per ORIG CU.  The weak
// Singleton<TssSdkCommSvc>::ObjectCreator pre-create below is one shared
// object in the shipped DSO.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID dp_forbid_mode_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_mail_verify_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_safe_mode_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_uic_trans_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"


template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// ---------------------------------------------------------------------------


namespace tss_dpsdk {

class DPMonitorReporter {
public:
    static int report_data(unsigned int id, unsigned int value);
};

class DPMsgPacker {
public:
    enum E_TIPS_WINDOWS {
        E_TIPS_NONE = 0,
        E_TIPS_SAFEMODE = 1,
        E_TIPS_SEND_SUCCESS = 2,
    };

    static int revive_client_mouse(tss_sdk_impl::UserInfo *user,
                                   unsigned int cmd_id,
                                   E_TIPS_WINDOWS tips);
    static int send_captcha_finish(tss_sdk_impl::UserInfo *user);
    static int send_captcha_request(tss_sdk_impl::UserInfo *user,
                                    const char *url);
    static int send_safe_mode_notify(tss_sdk_impl::UserInfo *user,
                                     int safe_mode_level);
};

// ---------------------------------------------------------------------------
// tss_dpsdk hacker interface hierarchy.
// ---------------------------------------------------------------------------

class DPMailVerifyHacker : public DPPkgDelayHacker {
public:
    DPMailVerifyHacker()
        : captcha_no_response_(0), captcha_counts_(0),
          last_captcha_retry_(0) {}

    virtual int init(IDPHackerEventNotify *event_notify);
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual int on_verify_response(tss_sdk_impl::UserInfo *user,
                                   unsigned char *data,
                                   unsigned int data_len);
    virtual int on_verify_msg(tss_sdk_impl::UserInfo *user,
                              const sec_proto::DPCaptchaResultRsp &pkg);
    virtual int on_special_control(tss_sdk_impl::UserInfo *user,
                                   unsigned char *buf, unsigned int &buf_len,
                                   unsigned char *out, unsigned int &out_len);
    virtual int on_timer(DPCommandBuf &buf);

    int is_permit_throught(tss_sdk_impl::UserInfo *user);
    int cancel_mail_opt(tss_sdk_impl::UserInfo *user, unsigned int cmd_id);

private:
    unsigned int captcha_no_response_;  // +84 (0x54)
    unsigned int captcha_counts_;       // +88 (0x58)
    time_t last_captcha_retry_;         // +92 (0x5c)
};

class DPForbidModeHacker : public IDPHacker {
public:
    DPForbidModeHacker() {}

    virtual int init(IDPHackerEventNotify *event_notify) {
        event_notify->register_event(E_BAN_MODE, this);
        return 0;
    }

    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual int on_ban_mode(tss_sdk_impl::UserInfo *user,
                            const sec_proto::DpBanModeReq &pkg);
};

class DPSafeModeHacker : public IDPHacker {
public:
    DPSafeModeHacker() {}

    virtual int init(IDPHackerEventNotify *event_notify) {
        event_notify->register_event(E_SAFE_MODE, this);
        return 0;
    }

    virtual int on_safe_mode(tss_sdk_impl::UserInfo *user,
                             int safe_mode_level);
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

class DPUicTransHacker : public IDPHacker {
public:
    DPUicTransHacker() {}

    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// ---------------------------------------------------------------------------
// DPMailVerifyHacker::init -- weak, 0x000ad770.
//
// Event registration order observed in the shipped body:
//   E_MSG2CLIENT, E_VERIFY_MSG, E_LOGOUT, E_PROC, E_SPECIAL_CONTROL.
// (E_LOGOUT/E_PROC dispatch to DPPkgDelayHacker::on_logout/proc.)
// ---------------------------------------------------------------------------
inline int DPMailVerifyHacker::init(IDPHackerEventNotify *event_notify) {
    event_notify->register_event(E_MSG2CLIENT, this);
    event_notify->register_event(E_VERIFY_MSG, this);
    event_notify->register_event(E_LOGOUT, this);
    event_notify->register_event(E_PROC, this);
    event_notify->register_event(E_SPECIAL_CONTROL, this);
    return 0;
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::on_special_control(...)  @ 0x000ac110
//
// Forwards to DPPkgDelayHacker::on_special_control with all arguments.
// A return value of exactly 1 additionally sends the captcha-finish packet;
// the forwarded return value is what this override returns.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::on_special_control(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int &buf_len,
    unsigned char *out, unsigned int &out_len) {
    int ret = DPPkgDelayHacker::on_special_control(user, buf, buf_len, out,
                                                   out_len);
    if (ret == 1)
        DPMsgPacker::send_captcha_finish(user);
    return ret;
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::cancel_mail_opt(UserInfo*, unsigned int)
//                                                          @ 0x000ac180
//
// Clears hack_data_.captcha_state_ (UserInfo +0x18f) and forwards to
// revive_client_mouse(user, cmd_id, E_TIPS_NONE).  The target returns the
// helper's result directly.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::cancel_mail_opt(tss_sdk_impl::UserInfo *user,
                                        unsigned int cmd_id) {
    user->hack_data_.captcha_state_ = 0;
    return DPMsgPacker::revive_client_mouse(user, cmd_id,
                                            DPMsgPacker::E_TIPS_NONE);
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::on_timer(DPCommandBuf&)  @ 0x000ac1c0
//
// If the owning user is still in captcha state 1: count one more missing
// response, report 0x3f4, bill "captcha no response" with the updated count,
// then cancel the pending operation.  Otherwise cancel immediately.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::on_timer(DPCommandBuf &buf) {
    tss_sdk_impl::UserInfo *user = buf.user_;
    if (user->hack_data_.captcha_state_ == 1) {
        ++captcha_no_response_;
        DPMonitorReporter::report_data(0x3f4, 1);
        {
            DPBillAdapter bill("captcha no response", 0x3d, user);
            DPBillAdapter::bill_ << (long long)captcha_no_response_;
        }
    }
    return cancel_mail_opt(user, buf.cmd_id_);
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::is_permit_throught(UserInfo*) @ 0x000ac2a0
//
// method = glb_dp_config_->cmd_hack.captch_method (DPConfig +0x55aa)
//   (max_timeout_count +0, min_try_count +4, min_try_time +8)
//
// Return 0 (permit) when max_timeout_count is zero, when the counter has not
// reached it, when min_try_count is zero, when the incremented counter is a
// multiple of min_try_count, or when min_try_time > 0 and
// now - last_captcha_retry_ >= min_try_time.  Those non-reporting paths update
// last_captcha_retry_ to now (except when max/count short-circuits).
//
// Otherwise bill "no captcha verify" with
// {max_timeout_count, min_try_count, min_try_time, captcha_counts_}, report
// 0x3f5 and return 1.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::is_permit_throught(tss_sdk_impl::UserInfo *user) {
    const tss_dp_comm::DPCaptchaMethod &method =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.captch_method;

    if (method.max_timeout_count == 0 ||
        method.max_timeout_count > captcha_no_response_)
        return 0;

    ++captcha_counts_;

    time_t now = time(NULL);

    if (method.min_try_count == 0 ||
        captcha_counts_ % method.min_try_count == 0) {
        last_captcha_retry_ = now;
        return 0;
    }

    if (method.min_try_time != 0) {
        if ((unsigned int)(now - last_captcha_retry_) >=
            method.min_try_time) {
            last_captcha_retry_ = now;
            return 0;
        }
    }

    {
        DPBillAdapter bill("no captcha verify", 0x3b, user);
        DPBillAdapter::bill_
            << (long long)method.max_timeout_count
            << (long long)method.min_try_count
            << (long long)method.min_try_time
            << (long long)captcha_counts_;
    }
    DPMonitorReporter::report_data(0x3f5, 1);
    return 1;
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::on_verify_response(UserInfo*, u8*, u32)
//                                                          @ 0x000ac420
//
// Requires captcha_state_ == 1.  A ClientPkgHead must unpack and carry
// protocol_id_ == 0x33 and sln_id_ == 0xc.  In that case, under locker_, the
// first cmd_delay_ entry for this user is removed from the list, the
// node_in_delay_ count is decremented, captcha_state_ becomes 2, its expiry is
// extended by 120 seconds and it is re-inserted through the delay sorted
// insert (which increments node_in_delay_ again).  Monitor id 0x406.
//
// Return -1 when the state guard or unpack fails, 0 otherwise.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::on_verify_response(tss_sdk_impl::UserInfo *user,
                                           unsigned char *data,
                                           unsigned int data_len) {
    const ZEN_LOG_PRIORITY priority =
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG;

    ZEN_Trace_LogMsg::debug_output(
        priority, "[%u][%s], captcha_state_=%d", user->base_info_.uin_,
        __func__, (int)user->hack_data_.captcha_state_);

    if (user->hack_data_.captcha_state_ != 1)
        return -1;

    sec_proto::ClientPkgHead frame;
    int ret = frame.unpack((const char *)data, data_len, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], ClientPkgHead unpack fail, ret=%d, len=%u",
            user->base_info_.uin_, __func__, ret, data_len);
        return -1;
    }

    if (frame.protocol_id_ != 0x33 || frame.sln_id_ != 0x0c)
        return 0;

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);

        for (std::list<DPCommandBuf>::iterator IT = cmd_delay_.begin();
             IT != cmd_delay_.end(); ++IT) {
            DPCommandBuf &buf = *IT;
            if (buf.user_ != user)
                continue;

            DPCommandBuf new_buf = buf;
            cmd_delay_.erase(IT);
            --node_in_delay_;

            user->hack_data_.captcha_state_ = 2;
            new_buf.expire_time_.tv_sec += 0x78;
            insert(new_buf);

            ZEN_Trace_LogMsg::debug_output(
                priority, "[%u][%s], get captcha resonece",
                user->base_info_.uin_, __func__);
            DPMonitorReporter::report_data(0x406, 1);
            break;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::on_verify_msg(UserInfo*, DPCaptchaResultRsp const&)
//                                                          @ 0x000ac630
//
// Resets captcha_no_response_.  If a delayed command for this user exists and
// captcha_state_ is non-zero, under locker_:
//   result_ != 0: cancel_mail_opt(user, cmd_id), send_captcha_finish(user)
//   result_ == 0: DPPkgDelayHacker::on_timer(cmd)
// then remove_from_delay(user, iterator) and captcha_state_ = 0.
// Always returns 0.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::on_verify_msg(
    tss_sdk_impl::UserInfo *user,
    const sec_proto::DPCaptchaResultRsp &pkg) {
    const ZEN_LOG_PRIORITY priority =
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG;

    ZEN_Trace_LogMsg::debug_output(
        priority,
        "[%u][%s], captcha_no_response_=%d, state=%d, result=%u",
        user->base_info_.uin_, __func__, (int)captcha_no_response_,
        (int)user->hack_data_.captcha_state_, (unsigned int)pkg.result_);

    captcha_no_response_ = 0;

    if (cmd_delay_.empty())
        return 0;
    if (user->hack_data_.captcha_state_ == 0)
        return 0;

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);

        for (std::list<DPCommandBuf>::iterator IT = cmd_delay_.begin();
             IT != cmd_delay_.end(); ++IT) {
            DPCommandBuf &buf = *IT;
            if (buf.user_ != user)
                continue;

            if (pkg.result_ != 0) {
                cancel_mail_opt(user, buf.cmd_id_);
                DPMsgPacker::send_captcha_finish(user);
            } else {
                DPPkgDelayHacker::on_timer(buf);
            }

            remove_from_delay(user, IT);
            user->hack_data_.captcha_state_ = 0;
            break;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// int DPMailVerifyHacker::hack(UserInfo*, int, u32, u8*&, u32&)
//                                                          @ 0x000acba0
//
// Only up packets (type == 1) from users outside safemode (safemode_level_ ==
// 0) and not already in captcha state are considered.  Parses the DNF mail
// packet view:
//   mail_header = (DPHackDnfMailHead *)buf
//   len must be > 18 and >= role_name_len_ + 17 + 16
//   mail_body = (DPHackDnfMailBody *)(buf + role_name_len_ + 17)
// A mail with no gold, no items and no item_posi_ is ignored.
//
// If is_permit_throught() fires, the packet is dropped (return 0).  Otherwise
// a GBK description is assembled, send_captcha_request() is called with it,
// and on success the packet is delayed for captch_method.waitting_response
// seconds under cmd id 0x61 (captcha_state_ becomes 1 when that delay insert
// succeeds).  Finally a bill record is emitted and the packet is reported to
// monitor 0x404.  Returns 1 after the captcha path, 0 on every guard.
// ---------------------------------------------------------------------------
int DPMailVerifyHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                             unsigned int cmd_id, unsigned char *&buf,
                             unsigned int &len) {
    if (type != 1 || user->hack_data_.safemode_level_ != 0) {
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
            "[%u][%s], safemode_level_=%u", user->base_info_.uin_, __func__,
            user->hack_data_.safemode_level_);
        return 0;
    }

    if (user->hack_data_.captcha_state_ != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], captcha_state_=%u", user->base_info_.uin_,
            __func__, (unsigned int)user->hack_data_.captcha_state_);
        return 0;
    }

    DPHackDnfMailHead *mail_header = (DPHackDnfMailHead *)buf;

    if (len <= 0x12) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], game pkg is wrong, len=%u, role_name_len_=%u",
            user->base_info_.uin_, __func__, len,
            mail_header->role_name_len_);
        return 0;
    }

    if (len <= mail_header->role_name_len_) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], game pkg is wrong, len=%u, role_name_len_=%u",
            user->base_info_.uin_, __func__, len,
            mail_header->role_name_len_);
        return 0;
    }

    unsigned int length_require = mail_header->role_name_len_ + 0x11;
    if (len < length_require + 0x10) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], game pkg is wrong, len=%u, role_name_len_=%u",
            user->base_info_.uin_, __func__, len, length_require);
        return 0;
    }

    DPHackDnfMailBody *mail_body =
        (DPHackDnfMailBody *)(buf + length_require);

    if (mail_body->gold_num_ == 0 && mail_body->items_num_ == 0 &&
        mail_body->item_.item_posi_ == 0) {
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
            "[%u][%s], mail has nothing", user->base_info_.uin_, __func__);
        return 0;
    }

    if (is_permit_throught(user))
        return 0;

    std::ostringstream info;
    info << "\304\372\325\375\315\250\271\375\323\312\274\376\270\370"
            "\001role_name=";
    info.write((const char *)mail_header->role_name_,
               mail_header->role_name_len_);
    info << "\001\267\242\313\315";

    if (mail_body->gold_num_ != 0) {
        info << "\001gold_num=" << mail_body->gold_num_
             << "\001\275\360\261\322";
    }

    if (mail_body->items_num_ != 0) {
        if (mail_body->gold_num_ != 0)
            info << "\241\242";

        info << "\001item_id=";
        if (mail_body->items_num_ > 0) {
            info << mail_body->items_[0].item_.item_id_;
        } else {
            info << mail_body->item_.item_id_;
        }
        info << "\001";

        if (mail_body->items_num_ > 1) {
            // The shipped code reads items_[1].item_.item_id_, i.e. the
            // 32-bit value at body + 0x13 (the declared array has one item).
            unsigned int second_item_id;
            memcpy(&second_item_id,
                   (const unsigned char *)mail_body + 0x13,
                   sizeof(second_item_id));
            info << "\241\242\001item_id=" << second_item_id
                 << "\001\265\310\316\357\306\267";
        }
    }

    info << "\243\254\310\347\310\267\310\317\264\313\262\331\327\367"
            "\307\353\325\375\310\267\312\344\310\353\321\351\326\244"
            "\302\353\243\254\310\347\262\273\310\267\310\317\264\313"
            "\262\331\327\367\307\353\326\261\275\323\265\343\273\367"
            "\310\241\317\373\260\264\305\245\241\243";

    std::string url = info.str();
    int res = DPMsgPacker::send_captcha_request(user, url.c_str());

    if (res == 0) {
        DPMonitorReporter::report_data(0x405, 1);
        res = DPPkgDelayHacker::insert(
            user,
            (int)tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                .captch_method.waitting_response,
            0x61, buf, len);
        if (res == 0)
            user->hack_data_.captcha_state_ = 1;
    }

    if (res != 0)
        cancel_mail_opt(user, 0x61);

    {
        DPBillAdapter bill(res == 0 ? "captcha verify"
                                    : "captcha verify fail",
                           0x3a, user);
        DPBillAdapter::bill_
            << (long long)res
            << (long long)cmd_id
            << (long long)len
            << (long long)node_wait_for_send_
            << (long long)node_in_delay_
            << (long long)tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                   .captch_method.waitting_response;
    }

    return 1;
}

// ---------------------------------------------------------------------------
// Static packet substituted for banned game-mode commands (ORIG .data
// 0x00297964, 11 bytes; *len is set to 11).
// ---------------------------------------------------------------------------
static unsigned char s_forbid_mode_pkg[11] = {
    0xf1, 0x04, 0x00, 0x01, 0x43, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf2
};

// ---------------------------------------------------------------------------
// int DPForbidModeHacker::hack(UserInfo*, int, u32, u8*&, u32&)
//                                                          @ 0x000aa3b0
//
// Applies only to type 2 (down) commands 0x14e00/0x14f00.  The user's
// cf_user_info_.game_mode_ (UserInfo +0x184) is mapped to an index via
// glb_dp_config_->cmd_hack.ban_mode (+0x585e): exact position in values[32]
// when present, otherwise game_mode_ & 0x1f.
//
// keep_time = ban_mode.keep_time_[index] (UserInfo +0x104 + index*4):
//   zero                        -> return 0
//   <= time(NULL)               -> clear it, return 0
//   still active                -> bill "ban mode" {cmd_id, game_mode,
//                                  index, keep_time, len}, replace *buf with
//                                  the 11-byte packet, set *len = 11,
//                                  report 0x3ee, return 0.
// ---------------------------------------------------------------------------
int DPForbidModeHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                             unsigned int cmd_id, unsigned char *&buf,
                             unsigned int &len) {
    if (type != 2)
        return 0;
    if (cmd_id != 0x14f00 && cmd_id != 0x14e00)
        return 0;

    unsigned char game_mode = user->hack_data_.cf_user_info_.game_mode_;
    const tss_dp_comm::DPBanMode &ban_mode =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.ban_mode;

    unsigned short mode_id;
    unsigned int i;
    for (i = 0; i < ban_mode.length; ++i) {
        if (ban_mode.values[i] == (unsigned int)game_mode) {
            mode_id = (unsigned short)i;
            break;
        }
    }
    if (!(i < ban_mode.length))
        mode_id = (unsigned short)(game_mode & 0x1f);

    time_t &keep_time =
        user->hack_data_.ban_mode_.keep_time_[mode_id];

    if (keep_time == 0)
        return 0;

    if (keep_time <= time(NULL)) {
        keep_time = 0;
        return 0;
    }

    {
        DPBillAdapter bill("ban mode", 0x2b, user);
        DPBillAdapter::bill_
            << (long long)cmd_id
            << (long long)game_mode
            << (long long)mode_id
            << (long long)keep_time
            << (long long)len;
    }

    buf = s_forbid_mode_pkg;
    len = 0x0b;
    DPMonitorReporter::report_data(0x3ee, 1);
    return 0;
}

// ---------------------------------------------------------------------------
// int DPForbidModeHacker::on_ban_mode(UserInfo*, DpBanModeReq const&)
//                                                          @ 0x000aa580
//
// current_time = time(NULL).  For every entry in pkg.modes_[0..count_):
//   index = position of info.mode_id_ in glb_dp_config_->cmd_hack.ban_mode
//           .values, otherwise info.mode_id_ & 0x1f
//   keep  = info.keep_time_ ? current_time + info.keep_time_ : 0
//   ban_mode.keep_time_[index] = keep
//   bill "recv ban mode" {info.mode_id_, info.keep_time_}
// Afterwards report 0x3ef with pkg.count_ and return 0.
// ---------------------------------------------------------------------------
int DPForbidModeHacker::on_ban_mode(
    tss_sdk_impl::UserInfo *user,
    const sec_proto::DpBanModeReq &pkg) {
    time_t current_time;
    time(&current_time);

    DPBillAdapter bill("recv ban mode", 0x2c, user);
    tss_sdk_impl::DPHackBanModeInfo &ban_mode =
        user->hack_data_.ban_mode_;
    const tss_dp_comm::DPBanMode &config_ban_mode =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.ban_mode;

    for (unsigned int i = 0; i < pkg.count_; ++i) {
        const sec_proto::DpBanModeDetail &info = pkg.modes_[i];

        unsigned int j;
        unsigned short mode_id;
        for (j = 0; j < config_ban_mode.length; ++j) {
            if (config_ban_mode.values[j] == info.mode_id_) {
                mode_id = (unsigned short)j;
                break;
            }
        }
        if (!(j < config_ban_mode.length))
            mode_id = (unsigned short)(info.mode_id_ & 0x1f);

        time_t keep_time =
            info.keep_time_ ? current_time + (time_t)info.keep_time_ : 0;
        ban_mode.keep_time_[mode_id] = keep_time;

        DPBillAdapter::bill_
            << (long long)info.mode_id_
            << (long long)info.keep_time_;
    }

    DPMonitorReporter::report_data(0x3ef, pkg.count_);
    return 0;
}

// ---------------------------------------------------------------------------
// int DPSafeModeHacker::on_safe_mode(UserInfo*, int)  @ 0x000bb3d0
//
// Logs the old/new level; on change stores hack_data_.safemode_level_
// (UserInfo +0xdc) and, when cmd_hack.safemode_to_client (+0x3da1) is set,
// sends send_safe_mode_notify(user, new_level).  Always returns 0.
// ---------------------------------------------------------------------------
int DPSafeModeHacker::on_safe_mode(tss_sdk_impl::UserInfo *user,
                                   int safe_mode_level) {
    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], update safe mode, old=%u, new=%u",
        user->base_info_.uin_, __func__,
        user->hack_data_.safemode_level_, (unsigned int)safe_mode_level);

    if (user->hack_data_.safemode_level_ != (unsigned int)safe_mode_level) {
        user->hack_data_.safemode_level_ = safe_mode_level;
        if (tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                .safemode_to_client != 0) {
            DPMsgPacker::send_safe_mode_notify(user, safe_mode_level);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// int DPSafeModeHacker::hack(UserInfo*, int, u32, u8*&, u32&)
//                                                          @ 0x000bb2f0
//
// Returns 0 for users not in safe mode.  In safe mode, if
// cmd_hack.safemode_hint_client (+0x3da2) is set, revives the client mouse
// with cmd_id and E_TIPS_SAFEMODE.  Bills "safe mode drop" with cmd_id,
// reports 0x408 and returns 1 (packet swallowed).
// ---------------------------------------------------------------------------
int DPSafeModeHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                           unsigned int cmd_id, unsigned char *&buf,
                           unsigned int &len) {
    (void)type;
    (void)buf;
    (void)len;

    if (user->hack_data_.safemode_level_ == 0)
        return 0;

    if (tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
            .safemode_hint_client != 0) {
        DPMsgPacker::revive_client_mouse(user, cmd_id,
                                         DPMsgPacker::E_TIPS_SAFEMODE);
    }

    {
        DPBillAdapter bill("safe mode drop", 0x30, user);
        DPBillAdapter::bill_ << (long long)cmd_id;
    }

    DPMonitorReporter::report_data(0x408, 1);
    return 1;
}

// ---------------------------------------------------------------------------
// int DPUicTransHacker::hack(UserInfo*, int, u32, u8*&, u32&)
//                                                          @ 0x000bbe30
//
// Builds a ReqUICTransAdaptMsgForwardPkg from the game packet:
//   world_id_    = user->base_info_.room_id_
//   src_id_      = 0
//   cmd_id_      = cmd_id
//   need_encrypt_= 1
//   is_encrypted_= (type != 4)
//   cmd_buf_len_ = min(len, 0x8000); cmd_buf_ = first cmd_buf_len_ bytes
//   rolename_len_= 0x41; rolename_ = 0x41 bytes from user role_name_
//   client_ip_   = user->base_info_.client_ip_
//
// With trace logging "[%u][%s], forward to uic", it then:
// 1. builds an inner Comm_App_Frame (command 0x57e41, send 0x1f, recv 0x24,
//    proxy 0, app_id = glb_dp_config_head_->game_id, uin, no transaction),
//    encodes appdata_encode<ReqUICTransAdaptMsgForwardPkg>(0xffbd);
// 2. wraps that frame in ReqSecsvrForwardData
//    {channel_id = TssSdkCommSvc::instance()->channel_id_,
//     room_id = glb_dp_config_head_->world_id, data_len = inner length,
//     data = inner frame bytes};
// 3. builds the outer Comm_App_Frame (command 0x4c6ad, send 0x2b, recv 0x1f,
//    proxy 0, same uin/app_id) and encodes
//    appdata_encode<ReqSecsvrForwardData>(0xffbd);
// 4. sends it through TssSdkCommSvc::send_buf_to_backend(frame, length, true,
//    0).
//
// Error paths log (RS_ERROR) and report monitor 0x404; the method always
// returns 0 and leaves *buf/*len untouched.
//
// The shipped code conditionally dumps each frame on
// (inner_option_ & 0x10000), but init_framehead clears the option word and
// sets only bit 24, so that dump is unreachable; it is reproduced as the same
// bit test.
// ---------------------------------------------------------------------------
int DPUicTransHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                           unsigned int cmd_id, unsigned char *&buf,
                           unsigned int &len) {
    sec_proto::ReqUICTransAdaptMsgForwardPkg forward_pkg;
    memset(&forward_pkg, 0, sizeof(forward_pkg));

    forward_pkg.world_id_ = user->base_info_.room_id_;
    forward_pkg.src_id_ = 0;
    forward_pkg.cmd_id_ = cmd_id;
    forward_pkg.need_encrypt_ = 1;
    forward_pkg.is_encrypted_ = (type != 4) ? 1 : 0;
    forward_pkg.client_ip_ = user->base_info_.client_ip_;
    forward_pkg.cmd_buf_len_ =
        len > 0x8000 ? (uint16_t)0x8000 : (uint16_t)len;
    memcpy(forward_pkg.cmd_buf_, buf, forward_pkg.cmd_buf_len_);
    forward_pkg.rolename_len_ = 0x41;
    memcpy(forward_pkg.rolename_, user->base_info_.role_name_, 0x41);

    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], forward to uic", user->base_info_.uin_, __func__);

    // Inner frame: ReqUICTransAdaptMsgForwardPkg.
    unsigned char inner_storage[0x10000];
    Comm_App_Frame *inner = (Comm_App_Frame *)inner_storage;
    inner->init_framehead(0x10000, 0, 0x57e41);
    inner->frame_uin_ = user->base_info_.uin_;
    {
        SERVICES_ID send_service(0x1f, 0);
        SERVICES_ID recv_service(0x24, 0);
        SERVICES_ID proxy_service(0, 0);
        inner->send_service_ = send_service;
        inner->recv_service_ = recv_service;
        inner->proxy_service_ = proxy_service;
    }
    inner->transaction_id_ = 0;
    inner->backfill_trans_id_ = 0;
    inner->app_id_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if (inner->inner_option_.inner_option_ & 0x00010000)
        inner->dumpoutput_framehead(
            inner, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = inner->appdata_encode<sec_proto::ReqUICTransAdaptMsgForwardPkg>(
        0xffbd, forward_pkg, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], app data encode fail. ret=%d",
            user->base_info_.uin_, "generate_comm_frame", ret);
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], generate_comm_frame fail, ret=%d, entire=%d, cmd=%u",
            user->base_info_.uin_, "send_datatrans_pkg", 0x139f, 0x24,
            0x57e41);
        DPMonitorReporter::report_data(0x404, 1);
        return 0;
    }

    // ReqSecsvrForwardData wrapper around the inner frame.
    sec_proto::ReqSecsvrForwardData forward_data;
    forward_data.channel_id_ =
        Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()->channel_id_;
    forward_data.room_id_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
    forward_data.data_len_ = (uint16_t)inner->frame_length_;
    memcpy(forward_data.data_, inner, forward_data.data_len_);

    // Outer frame: ReqSecsvrForwardData.
    unsigned char outer_storage[0x10000];
    Comm_App_Frame *outer = (Comm_App_Frame *)outer_storage;
    outer->init_framehead(0x10000, 0, 0x4c6ad);
    outer->frame_uin_ = user->base_info_.uin_;
    {
        SERVICES_ID send_service(0x2b, 0);
        SERVICES_ID recv_service(0x1f, 0);
        SERVICES_ID proxy_service(0, 0);
        outer->send_service_ = send_service;
        outer->recv_service_ = recv_service;
        outer->proxy_service_ = proxy_service;
    }
    outer->transaction_id_ = 0;
    outer->backfill_trans_id_ = 0;
    outer->app_id_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if (outer->inner_option_.inner_option_ & 0x00010000)
        outer->dumpoutput_framehead(
            outer, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    ret = outer->appdata_encode<sec_proto::ReqSecsvrForwardData>(
        0xffbd, forward_data, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u",
            user->base_info_.uin_, "send_ipc_pkg", 0x139f, 0x4c6ad);
        DPMonitorReporter::report_data(0x404, 1);
        return 0;
    }

    tss_sdk_impl::TssSdkCommSvc *comm_svc =
        Singleton<tss_sdk_impl::TssSdkCommSvc>::instance();
    int send_res = comm_svc->send_buf_to_backend(
        (const char *)outer, outer->frame_length_, true, 0);
    if (send_res != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]send to backend fail. ret=%d",
            "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
            "unsigned int, unsigned int, short unsigned int) [with PKG = "
            "sec_proto::ReqSecsvrForwardData]",
            send_res);
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u",
            user->base_info_.uin_, "send_ipc_pkg", -2, 0x4c6ad);
    } else {
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
            "[%u][%s], send_ipc_pkg success, cmd=%u",
            user->base_info_.uin_, "send_ipc_pkg", 0x4c6ad);
    }

    DPMonitorReporter::report_data(0x404, 1);
    return 0;
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Compile-time layout checks (DWARF values).
// ---------------------------------------------------------------------------
typedef char check_user_info_size[
    sizeof(tss_sdk_impl::UserInfo) == 400 ? 1 : -1];
typedef char check_user_hack_data_size[
    sizeof(tss_sdk_impl::UserInfoHackData) == 180 ? 1 : -1];
typedef char check_mutex_size[sizeof(ZEN_Thread_Mutex) == 28 ? 1 : -1];
typedef char check_hacker_safe_size[
    sizeof(tss_dpsdk::IDPHackerSafe) == 32 ? 1 : -1];
typedef char check_delay_hacker_size[
    sizeof(tss_dpsdk::DPPkgDelayHacker) == 0x54 ? 1 : -1];
typedef char check_mail_hacker_size[
    sizeof(tss_dpsdk::DPMailVerifyHacker) == 0x60 ? 1 : -1];
typedef char check_event_notify_size[
    sizeof(tss_dpsdk::IDPHackerEventNotify) == 112 ? 1 : -1];
typedef char check_services_id_size[sizeof(SERVICES_ID) == 6 ? 1 : -1];
typedef char check_comm_frame_size[sizeof(Comm_App_Frame) == 50 ? 1 : -1];
typedef char check_uic_forward_pkg_size[
    sizeof(sec_proto::ReqUICTransAdaptMsgForwardPkg) == 33044 ? 1 : -1];
typedef char check_secsvr_forward_data_size[
    sizeof(sec_proto::ReqSecsvrForwardData) == 32773 ? 1 : -1];
typedef char check_client_pkg_head_size[
    sizeof(sec_proto::ClientPkgHead) == 12 ? 1 : -1];
typedef char check_dp_ban_mode_req_size[
    sizeof(sec_proto::DpBanModeReq) == 264 ? 1 : -1];
typedef char check_mail_body_size[
    sizeof(tss_dpsdk::DPHackDnfMailBody) == 16 ? 1 : -1];
typedef char check_mail_head_offsets[
    (__builtin_offsetof(tss_dpsdk::DPHackDnfMailHead, role_name_len_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfMailBody, items_) == 5) ? 1 : -1];
typedef char check_dp_config_size[
    sizeof(tss_dp_comm::DPConfig) == 22842 ? 1 : -1];
typedef char check_dp_config_head_size[
    sizeof(tss_dp_comm::DP_CONFIG) == 280 ? 1 : -1];
typedef char check_dp_cmd_hack_size[
    sizeof(tss_dp_comm::DPCmdHack) == 7100 ? 1 : -1];
typedef char check_dp_ban_mode_size[
    sizeof(tss_dp_comm::DPBanMode) == 130 ? 1 : -1];
typedef char check_dp_config_offsets[
    (__builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack) == 0x3d7e &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, safemode_to_client) == 0x23 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, safemode_hint_client) == 0x24 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, captch_method) == 0x182c &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, ban_mode) == 0x1ae0) ? 1 : -1];
typedef char check_user_info_offsets[
    (__builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.flags_) == 0x20 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.role_name_) == 0x24 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_) == 0xdc &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_.ban_mode_) == 0x104 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_.cf_user_info_) == 0x184 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_.captcha_state_) == 0x18f)
        ? 1 : -1];
typedef char check_dp_command_buf_size[
    sizeof(tss_dpsdk::DPPkgDelayHacker::DPCommandBuf) == 24 ? 1 : -1];
typedef char check_vptr_only_hacker_sizes[
    (sizeof(tss_dpsdk::DPForbidModeHacker) == 4 &&
     sizeof(tss_dpsdk::DPSafeModeHacker) == 4 &&
     sizeof(tss_dpsdk::DPUicTransHacker) == 4) ? 1 : -1];
typedef char check_sec_proto_sizes[
    (sizeof(sec_proto::DpBanModeDetail) == 8 &&
     sizeof(sec_proto::DPCaptchaResultRsp) == 5) ? 1 : -1];
typedef char check_mail_packet_sizes[
    (sizeof(tss_dpsdk::DPHackDnfMailHead) == 18 &&
     sizeof(tss_dpsdk::DPHackDnfMailItem) == 10 &&
     sizeof(tss_dpsdk::DPHackDnfMailItemWrap) == 11) ? 1 : -1];
typedef char check_user_sub_sizes[
    (sizeof(tss_sdk_impl::UserInfoBaseInfo) == 100 &&
     sizeof(tss_sdk_impl::DPHackBanModeInfo) == 128 &&
     sizeof(tss_sdk_impl::DPHackCfUserInfo) == 8 &&
     sizeof(tss_sdk_impl::DPHackDelayCmd) == 3) ? 1 : -1];
