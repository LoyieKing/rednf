// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG translation unit:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_property_trans_hacker.cpp
//   (DWARF decl_file 1)
//
// ORIG symbols implemented here (addresses/sizes from nm -S):
//   tss_dpsdk::DPPropertyTransImpl::on_logout(UserInfo*)
//       @0x000b8820..0x000b88af  (0x90)  T
//   tss_dpsdk::DPPropertyTransImpl::forward_trade_data_to_safemode(
//       UserInfo*, UserInfo*, int, unsigned int, unsigned char const*,
//       unsigned int)
//       @0x000b8fb0..0x000b96d4  (0x725) T
//   tss_dpsdk::DPPropertyTransImpl::hack(
//       UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)
//       @0x000b96e0..0x000b9e83  (0x7a4) T
//   tss_dpsdk::DPPropertyTransImpl::init(IDPHackerEventNotify*)
//       @0x000ba8e0..0x000ba954  (0x75)  W (inline in the class in ORIG)
//   tss_dpsdk::DPPropertyTransImpl::~DPPropertyTransImpl() D0
//       @0x000ba960..0x000ba9fe  (0x9f)  W (inline)
//   tss_dpsdk::DPPropertyTransImpl::~DPPropertyTransImpl() D1
//       @0x000baa00..0x000baa96  (0x97)  W (inline)
//
// This is a semantic reconstruction, not a byte-identical one.
//
// The original forward_trade_data_to_safemode inlines
//   DPMsgPacker::generate_comm_frame<ReqSafemodeTransAdaptTradeNotify>
//   DPMsgPacker::send_ipc_pkg<ReqSecsvrForwardData>
//   DPMsgPacker::send_datatrans_pkg<ReqSafemodeTransAdaptTradeNotify>
// (the complete frame construction/send sequence was observed at
// 0xb9097..0xb9625).  The reconstruction calls the shipped out-of-line weak
// instantiation of send_datatrans_pkg at 0xbac20 with the same arguments
// (user, 0x26, trade_pkg, 0x5cc65); that weak instantiation is defined in the
// dp_msg_packer TU and performs the same generate_comm_frame -> send_ipc_pkg
// chain.  No fake send/allow path exists.

#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <map>
#include <vector>

#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
// Canonical hacker hierarchy/state declarations.  IDPHackerSafe::locker_ is
// protected there now, so this TU no longer needs local copies.
#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"
#include "src/formmog/tss_sdk/layout/dp_property_trans.h"
#include "src/formmog/tss_sdk/layout/dp_hack_dnf.h"
#include "src/closing/dp_config.h"

// ---------------------------------------------------------------------------
// ORIG static-initialization block (dp_property_trans_hacker.cpp, init target
// 1174 bytes / 29 calls): ios_base::Init 1, __cxa_atexit 4,
// ZEN_STATUS_ITEM_WITHNAME ctor 16, ZEN_Thread_Mutex ctor 1 (the file-scope
// propety_impl object), guard acquire/release 1, TssSdkCommSvc::TssSdkCommSvc
// 1, ~ZEN_STATUS_ITEM_WITHNAME 1, __cxa_guard_abort 1, _Unwind_Resume 2
// (audit/report/init_brief.md).  The ORIG header-inlined instance() puts the
// guard and singleton ctor inside this block; the template definition below
// reproduces it (weak copies merge in the DSO).
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"
#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"


template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_guard.h"

// ---------------------------------------------------------------------------
// sec_proto protocol structures.  All packed; sizes/offsets are DWARF-exact.
// ---------------------------------------------------------------------------
namespace sec_proto {

// Forward declarations only; the IDPHacker default virtuals take these by
// const reference, and the vtable must reference the shipped mangled names.
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;

struct ReqSafemodeTransAdaptTradeNotify {
    unsigned short world_id_;       // +0
    unsigned int src_id_;           // +2
    unsigned char type_;            // +6
    unsigned int send_uin_;         // +7
    unsigned int target_uin_;       // +11
    unsigned int send_ip_;          // +15
    unsigned int target_ip_;        // +19
    unsigned short buf_len_;        // +23
    unsigned char trade_buf_[2048]; // +25
} __attribute__((packed));

}  // namespace sec_proto

// ---------------------------------------------------------------------------
// tss_dp_comm configuration prefix.  DPConfig is 22842 bytes; only the
// property command block read by hack() is named.  The byte arrays preserve
// the exact absolute member offsets:
//   cmd_hack                @ DPConfig + 0x3d7e
//   cmd_hack.property_cmd   @ DPConfig + 0x590a (= 0x3d7e + 0x1b8c)
// ---------------------------------------------------------------------------
// tss_dp_comm::DPConfig/DPCmdHack/DPPropertyCmd/DP_CONFIG come from the shared
// ORIG-DWARF header (src/origsrc/formmog_conf_tsssdk_dp.h via
// src/closing/dp_config.h); property_cmd is at DPConfig+0x590a.

// ---------------------------------------------------------------------------
// tss_dpsdk hacker framework.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

// ORIG emits these three as weak inline copies from this CU.
inline __attribute__((weak))
DPPropertyTransImpl::DPPropertyTransImpl() {}

__attribute__((weak))
DPPropertyTransImpl::~DPPropertyTransImpl() {}

__attribute__((weak))
int DPPropertyTransImpl::init(IDPHackerEventNotify *event_notify) {
    std::vector<IDPHacker *> &hackers = event_notify->hackers_[E_LOGOUT];
    for (std::vector<IDPHacker *>::iterator it = hackers.begin();
         it != hackers.end(); ++it) {
        if (*it == this)
            return 0;
    }
    hackers.push_back(this);
    return 0;
}

// cannot be preempted, but both halves of the original TU share one instance.
__attribute__((visibility("hidden"))) DPPropertyTransImpl propety_impl;

// ORIG 0x000b9f20: thin facade over propety_impl.  sizeof 4.
class DPPropertyTransHacker : public IDPHacker {
public:
    DPPropertyTransHacker() {}
    virtual ~DPPropertyTransHacker() {}
    virtual int init(IDPHackerEventNotify *event_notify);
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// ---------------------------------------------------------------------------
// DPMsgPacker boundary.  Only the weak instantiation used by
// forward_trade_data_to_safemode is referenced.
// ---------------------------------------------------------------------------
class DPMsgPacker {
public:
    template <class PKG>
    static int send_datatrans_pkg(tss_sdk_impl::UserInfo *user, int entire,
                                  const PKG &forward_pkg, unsigned int cmd);
};

template <>
int DPMsgPacker::send_datatrans_pkg<
    sec_proto::ReqSafemodeTransAdaptTradeNotify>(
    tss_sdk_impl::UserInfo *user, int entire,
    const sec_proto::ReqSafemodeTransAdaptTradeNotify &forward_pkg,
    unsigned int cmd);

// DPMonitorReporter::report_data(unsigned int, unsigned int) boundary.
class DPMonitorReporter {
public:
    static int report_data(unsigned int id, unsigned int value);
};

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// dp_hacker.h / dp_hnf_cmd.h packet layouts.  Sizes and field offsets are
// DWARF-exact.  Names follow the class member names emitted by DWARF.
// ---------------------------------------------------------------------------
// DPHackDnf* packet layouts come from the canonical layout/dp_hack_dnf.h
// (sizes 13/16/14/25/14/18/10/11/18/16, DWARF-exact); the anonymous union of
// DPHackDnfMailBody is flattened there, so items_/item_ are direct members.

// ---------------------------------------------------------------------------
// Logging helpers used by hack().  The ORIG source repeats these expressions
// inline at every log site; the macros expand to the same calls (no helper
// symbols are introduced).
// ---------------------------------------------------------------------------
#define TRADE_LOG_PRIORITY(user) \
    ((((user) != 0) && (((user)->base_info_.flags_ & 1u) == 1u)) ? RS_INFO \
                                                                : RS_DEBUG)
#define TRADE_LOG_UIN(user) ((user) != 0 ? (user)->base_info_.uin_ : 0u)

#define TRADE_DEBUG_LOG(user, fmt, ...)                                      \
    ZEN_Trace_LogMsg::debug_output(TRADE_LOG_PRIORITY(user), fmt,           \
                                   TRADE_LOG_UIN(user), __func__,            \
                                   ##__VA_ARGS__)

#define TRADE_LEN_ERROR(user, type, cmd_id, len, expected)                    \
    ZEN_Trace_LogMsg::debug_output(                                          \
        RS_ERROR,                                                            \
        "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "            \
        "expected_len=%u",                                                   \
        TRADE_LOG_UIN(user), __func__, (int)(type), (unsigned int)(cmd_id),  \
        (unsigned int)(len), (unsigned int)(expected))

namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// on_logout @0xb8820.
//
// if (user->hack_data_.dnf_client_uid_ == 0) return 0;
// lock; uid2usr_.erase(dnf_client_uid_); dnf_client_uid_ = 0; unlock;
// return 0;
// ---------------------------------------------------------------------------
int DPPropertyTransImpl::on_logout(tss_sdk_impl::UserInfo *user) {
    if (user->hack_data_.dnf_client_uid_ == 0)
        return 0;

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);
        uid2usr_.erase(user->hack_data_.dnf_client_uid_);
        user->hack_data_.dnf_client_uid_ = 0;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// forward_trade_data_to_safemode @0xb8fb0.
//
// Builds a ReqSafemodeTransAdaptTradeNotify and hands it to
// DPMsgPacker::send_datatrans_pkg with command 0x5cc65 and entire=0x26.
// The "cmd_id" argument is not consumed by the shipped function body (no
// read of [ebp+0x18] exists in 0xb8fb0..0xb96d4); it is kept for signature
// compatibility.
// ---------------------------------------------------------------------------
int DPPropertyTransImpl::forward_trade_data_to_safemode(
    tss_sdk_impl::UserInfo *user, tss_sdk_impl::UserInfo *dst_user, int type,
    unsigned int cmd_id, const unsigned char *buf, unsigned int len) {
    (void)cmd_id;

    sec_proto::ReqSafemodeTransAdaptTradeNotify trade_pkg;

    trade_pkg.type_ = (unsigned char)type;                       // 0xb8fd3
    trade_pkg.send_uin_ = user->base_info_.uin_;                 // 0xb8fdb
    trade_pkg.send_ip_ = htonl(user->base_info_.client_ip_);     // 0xb8fe1
    trade_pkg.world_id_ = (unsigned short)user->base_info_.room_id_;  // 0xb8ff5
    trade_pkg.src_id_ = 0;                                       // 0xb8ffa

    if (dst_user != 0) {
        trade_pkg.target_uin_ = dst_user->base_info_.uin_;       // 0xb9013
        trade_pkg.target_ip_ = dst_user->base_info_.client_ip_;  // 0xb901c
    } else {
        trade_pkg.target_uin_ = 0;                               // 0xb9632
        trade_pkg.target_ip_ = 0;                                // 0xb963c
    }

    if (len > sizeof(trade_pkg.trade_buf_)) {                    // 0xb9022
        trade_pkg.buf_len_ = (unsigned short)sizeof(trade_pkg.trade_buf_);
    } else {
        trade_pkg.buf_len_ = (unsigned short)len;                // 0xb9470
    }

    memcpy(trade_pkg.trade_buf_, buf, trade_pkg.buf_len_);       // 0xb9047

    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1u) == 1u ? RS_INFO : RS_DEBUG,
        "[%u][%s], forward to trans, target_uin_=%u",
        user->base_info_.uin_, __func__, trade_pkg.target_uin_); // 0xb9092

    // ORIG inlined this weak instantiation (0xbac20); the observable send
    // sequence is generate_comm_frame(cmd=0x5cc65, entire=0x26) followed by
    // send_ipc_pkg<ReqSecsvrForwardData>(..., 0x4c6ad).
    return DPMsgPacker::send_datatrans_pkg<
        sec_proto::ReqSafemodeTransAdaptTradeNotify>(
        user, 0x26, trade_pkg, 0x5cc65);                         // 0xb9646
}

// ---------------------------------------------------------------------------
// hack @0xb96e0.
//
// type == 2 is the client->server (encrypt) command set:
//   role_uid, create_team, drop_item, create_role, enter_private_store.
// any other type is the server->client command set:
//   trade_accept, trade_confirm, trade_put_item, enchant_put_item,
//   enchant_confirm, simple_mail, complex_mail.
//
// Every command that passes its filter is forwarded to
// forward_trade_data_to_safemode(), then monitor id 0x407 is reported and 1
// is returned.  Length failures return -1.  Filter failures and the local
// role_uid bookkeeping return 0.
// ---------------------------------------------------------------------------
int DPPropertyTransImpl::hack(tss_sdk_impl::UserInfo *user, int type,
                              unsigned int cmd_id, unsigned char *&buf,
                              unsigned int &len) {
    // Every path that reads dest_uid assigns it first; ORIG has no
    // function-entry initialisation for it.
    unsigned int dest_uid;
    tss_sdk_impl::UserInfo *dest_user = 0;

    if (type == 2) {
        // ---- client -> server command set -------------------------------
        if (cmd_id ==
            tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.property_cmd
                .cmd_role_uid) {
            if (len < sizeof(DPHackDnfCmdUid)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdUid));
                return -1;
            }

            DPHackDnfCmdUid *pkg = (DPHackDnfCmdUid *)buf;
            dest_uid = pkg->role_uid_;

            TRADE_DEBUG_LOG(user, "[%u][%s], uid=%u", dest_uid);

            {
                ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);
                uid2usr_[dest_uid] = user;
                user->hack_data_.dnf_client_uid_ = dest_uid;
            }
            return 0;
        } else if (cmd_id ==
                       tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                           .property_cmd.cmd_create_team ||
                   cmd_id ==
                       tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                           .property_cmd.cmd_drop_item) {
            if (len < sizeof(DPHackDnfCmdHead)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdHead));
                return -1;
            }

            DPHackDnfCmdHead *pkg = (DPHackDnfCmdHead *)buf;
            if (pkg->type_ != 0) {
                TRADE_DEBUG_LOG(user, "[%u][%s], type_=%u", pkg->type_);
                return 0;
            }
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_create_role) {
            if (len < sizeof(DPHackDnfCmdHead)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdHead));
                return -1;
            }

            DPHackDnfCmdHead *pkg = (DPHackDnfCmdHead *)buf;
            if (pkg->type_ != 1) {
                TRADE_DEBUG_LOG(user, "[%u][%s], type_=%u", pkg->type_);
                return 0;
            }
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_enter_private_store) {
            if (len < sizeof(DPHackStoreResponse)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackStoreResponse));
                return -1;
            }

            DPHackStoreResponse *pkg = (DPHackStoreResponse *)buf;
            if (pkg->header_.type_ != 1) {
                TRADE_DEBUG_LOG(user, "[%u][%s], type_=%u",
                                pkg->header_.type_);
                return 0;
            }
            dest_uid = pkg->role_uid_;
            goto find_dest_user;
        } else {
            goto forward_to_safemode;
        }
    } else {
        // ---- server -> client command set -------------------------------
        if (cmd_id ==
            tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.property_cmd
                .cmd_trade_accept) {
            if (len < sizeof(DPHackDnfCmdRequest)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdRequest));
                return -1;
            }

            DPHackDnfCmdRequest *pkg = (DPHackDnfCmdRequest *)buf;
            if (pkg->sub_id_ != 1 && pkg->sub_id_ != 6 &&
                pkg->sub_id_ != 7 && pkg->sub_id_ != 8) {
                TRADE_DEBUG_LOG(user, "[%u][%s], sub_id_=%u", pkg->sub_id_);
                return 0;
            }
            dest_uid = pkg->role_uid_;
            goto find_dest_user;
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_trade_confirm) {
            if (len < sizeof(DPHackDnfTradeConfirm)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfTradeConfirm));
                return -1;
            }

            DPHackDnfTradeConfirm *pkg = (DPHackDnfTradeConfirm *)buf;
            if (pkg->sub_id_ != 3) {
                TRADE_DEBUG_LOG(user, "[%u][%s], sub_id_=%u", pkg->sub_id_);
                return 0;
            }
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_trade_put_item) {
            if (len < sizeof(DPHackDnfItemMove)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfItemMove));
                return -1;
            }

            DPHackDnfItemMove *pkg = (DPHackDnfItemMove *)buf;
            if (pkg->sub_id_ != 4 && pkg->flag_ != 4) {
                TRADE_DEBUG_LOG(user, "[%u][%s], sub_id_=%u, flag_=%u",
                                pkg->sub_id_, pkg->flag_);
                return 0;
            }
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_enchant_put_item) {
            if (len < sizeof(DPHackDnfCmdEnchant)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdEnchant));
                return -1;
            }

            DPHackDnfCmdEnchant *pkg = (DPHackDnfCmdEnchant *)buf;
            if (pkg->flag_ != 0) {
                TRADE_DEBUG_LOG(user, "[%u][%s], flag_=%u", pkg->flag_);
                return 0;
            }
        } else if (cmd_id ==
                   tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                       .property_cmd.cmd_enchant_confirm) {
            if (len < sizeof(DPHackDnfCmdEnchant)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                sizeof(DPHackDnfCmdEnchant));
                return -1;
            }

            DPHackDnfCmdEnchant *pkg = (DPHackDnfCmdEnchant *)buf;
            if (pkg->flag_ != 1) {
                TRADE_DEBUG_LOG(user, "[%u][%s], flag_=%u", pkg->flag_);
                return 0;
            }
        } else if (cmd_id ==
                       tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                           .property_cmd.cmd_simple_mail ||
                   cmd_id ==
                       tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack
                           .property_cmd.cmd_complex_mail) {
            if (user->hack_data_.safemode_level_ != 0)
                return 0;

            DPHackDnfMailHead *mail_header = (DPHackDnfMailHead *)buf;
            unsigned int length_require =
                mail_header->role_name_len_ + (unsigned int)0x11;
            DPHackDnfMailBody *mail_body =
                (DPHackDnfMailBody *)(buf + length_require);

            if (len < length_require + sizeof(DPHackDnfMailBody)) {
                TRADE_LEN_ERROR(user, type, cmd_id, len,
                                length_require + sizeof(DPHackDnfMailBody));
                return -1;
            }

            if (mail_body->gold_num_ != 0 || mail_body->items_num_ != 0 ||
                mail_body->item_.item_posi_ != 0) {
                goto forward_to_safemode;
            }

            TRADE_DEBUG_LOG(user, "[%u][%s], no gold or item");
            return 0;
        } else {
            goto forward_to_safemode;
        }
    }

    // All non-local command handlers reach this point.
    goto forward_to_safemode;

find_dest_user:
    // 0xb9a04: the ORIG source compares the zero-extended 16-bit uid with
    // (unsigned int)-1, which can never be true; the guard is preserved.
    if (dest_uid != (unsigned int)-1) {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp(locker_);
        std::map<unsigned int, tss_sdk_impl::UserInfo *>::iterator it =
            uid2usr_.find(dest_uid);
        if (it != uid2usr_.end())
            dest_user = it->second;
    }

forward_to_safemode:
    forward_trade_data_to_safemode(user, dest_user, type, cmd_id, buf, len);
    DPMonitorReporter::report_data(0x407, 1);
    return 1;
}

// ===========================================================================
// DPPropertyTransHacker  (ORIG 0x000b87a0 / 0x000b9f20)
// ===========================================================================
int DPPropertyTransHacker::init(IDPHackerEventNotify *event_notify) {
    // ORIG registers the namespace-scope propety_impl object, not `this`.
    event_notify->register_event(
        E_LOGOUT, static_cast<IDPHacker *>(&propety_impl));
    return 0;
}

int DPPropertyTransHacker::hack(
    tss_sdk_impl::UserInfo *user, int type, unsigned int cmd_id,
    unsigned char *&buf, unsigned int &len) {
    // Direct (non-virtual) call to the out-of-line ORIG entry point.
    propety_impl.DPPropertyTransImpl::hack(user, type, cmd_id, buf, len);
    return 0;  // ORIG discards the implementation result
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Compile-time layout checks (DWARF / disassembly exact).
// ---------------------------------------------------------------------------
typedef char check_user_info_size[sizeof(tss_sdk_impl::UserInfo) == 400 ? 1
                                                                        : -1];
typedef char check_user_info_offsets[
    (__builtin_offsetof(tss_sdk_impl::UserInfo, base_info_) == 0x00 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_) == 0xdc &&
     __builtin_offsetof(tss_sdk_impl::UserInfoHackData, safemode_level_) ==
         0x00 &&
     __builtin_offsetof(tss_sdk_impl::UserInfoHackData, dnf_client_uid_) ==
         0x04)
        ? 1
        : -1];

typedef char check_trade_pkg_size[
    sizeof(sec_proto::ReqSafemodeTransAdaptTradeNotify) == 2073 ? 1 : -1];
typedef char check_trade_pkg_offsets[
    (__builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        world_id_) == 0 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, src_id_) ==
         2 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, type_) ==
         6 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        send_uin_) == 7 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        target_uin_) == 11 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        send_ip_) == 15 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        target_ip_) == 19 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        buf_len_) == 23 &&
     __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify,
                        trade_buf_) == 25)
        ? 1
        : -1];

typedef char check_packet_sizes[
    (sizeof(tss_dpsdk::DPHackDnfCmdHead) == 13 &&
     sizeof(tss_dpsdk::DPHackDnfCmdUid) == 26 &&
     sizeof(tss_dpsdk::DPHackDnfCmdRequest) == 16 &&
     sizeof(tss_dpsdk::DPHackDnfTradeConfirm) == 14 &&
     sizeof(tss_dpsdk::DPHackDnfItemMove) == 25 &&
     sizeof(tss_dpsdk::DPHackDnfCmdEnchant) == 14 &&
     sizeof(tss_dpsdk::DPHackStoreResponse) == 18 &&
     sizeof(tss_dpsdk::DPHackDnfMailItem) == 10 &&
     sizeof(tss_dpsdk::DPHackDnfMailItemWrap) == 11 &&
     sizeof(tss_dpsdk::DPHackDnfMailHead) == 18 &&
     sizeof(tss_dpsdk::DPHackDnfMailBody) == 16)
        ? 1
        : -1];
typedef char check_packet_offsets[
    (__builtin_offsetof(tss_dpsdk::DPHackDnfCmdUid, role_uid_) == 24 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfCmdRequest, role_uid_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfCmdRequest, sub_id_) == 15 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfTradeConfirm, sub_id_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfItemMove, flag_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfItemMove, sub_id_) == 24 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfCmdEnchant, flag_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackStoreResponse, role_uid_) == 16 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfMailHead, role_name_len_) == 13 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfMailBody, gold_num_) == 0 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfMailBody, items_num_) == 4 &&
     __builtin_offsetof(tss_dpsdk::DPHackDnfMailBody, item_) == 5)
        ? 1
        : -1];

typedef char check_config_offsets[
    (sizeof(tss_dp_comm::DPPropertyCmd) == 48 &&
     sizeof(tss_dp_comm::DPCmdHack) == 7100 &&
     sizeof(tss_dp_comm::DPConfig) == 22842 &&
     __builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack) == 0x3d7e &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, property_cmd) == 0x1b8c &&
     __builtin_offsetof(tss_dp_comm::DPPropertyCmd, cmd_role_uid) == 0 &&
     __builtin_offsetof(tss_dp_comm::DPPropertyCmd, cmd_trade_accept) == 20 &&
     __builtin_offsetof(tss_dp_comm::DPPropertyCmd, cmd_complex_mail) == 44)
        ? 1
        : -1];

typedef char check_hacker_layout[
    (sizeof(ZEN_Thread_Mutex) == 28 && sizeof(tss_dpsdk::IDPHacker) == 4 &&
     sizeof(tss_dpsdk::IDPHackerSafe) == 32 &&
     sizeof(tss_dpsdk::DPPropertyTransImpl) == 56 &&
     sizeof(tss_dpsdk::IDPHackerEventNotify) == 112)
        ? 1
        : -1];
// IDPHackerSafe is IDPHacker (4-byte vptr) + first member ZEN_Thread_Mutex
// (28), so locker_ is at +4 and DPPropertyTransImpl's sole member uid2usr_
// follows the 32-byte base at +32; the two sizeof checks above pin both
// offsets.  IDPHackerEventNotify's vptr is followed by hackers_ at +4.

