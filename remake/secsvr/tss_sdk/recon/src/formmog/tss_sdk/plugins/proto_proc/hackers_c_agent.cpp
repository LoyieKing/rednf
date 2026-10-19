// Semantic reconstruction of three tss_dpsdk hacker classes from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC):
//
//   tss_dpsdk::DPCfRoomQtalkHacker
//     notify_client  0x000a5680..0x000a5786 (0x106)
//     on_login       0x000a5790..0x000a5875 (0x0e5)
//     hack           0x000a5880..0x000a5fba (0x73a)
//     init           0x000a6550..0x000a65c5 (0x075, weak inline)
//     ~D1/~D0        0x000a65d0 / 0x000a63d0 (weak inline)
//
//   tss_dpsdk::DPPropertyTransHacker  (thin facade over propety_impl)
//     init           0x000b87a0..0x000b8815 (0x075)
//     hack           0x000b9f20..0x000b9f6b (0x04b)
//     ~D1/~D0        0x000ba8b0 / 0x000ba870 (weak inline)
//
//   tss_dpsdk::DPPkgDelayHacker       (abstract packet-delay queue)
//     init             0x000a99f0..0x000a9b13 (0x123, weak inline)
//     on_timer         0x000b4eb0..0x000b4ff7 (0x147)
//     insert(buf)      0x000b5000..0x000b50ff (0x0ff)
//     remove_from_delay0x000b5100..0x000b517f (0x07f)
//     on_logout        0x000b5180..0x000b52f9 (0x179)
//     proc             0x000b5300..0x000b545a (0x15a)
//     insert(user,...) 0x000b5460..0x000b55db (0x17b)
//     on_special_ctrl  0x000b55e0..0x000b581a (0x23a)
//     ctor C1/C2       0x000b5c30 / 0x000b5da0 (0x16b each)
//     ~D1/~D0          0x000b6480 / 0x000b6530 (weak inline)
//
// ORIG DWARF source lines: dp_cf_room_qtalk.cpp/h,
// dp_property_trans_hacker.cpp/h, dp_pkg_delay_hacker.cpp/h.
//
// Everything this translation unit calls outside itself is a real ORIG
// symbol: ZEN_Trace_LogMsg::debug_output, DPMsgPacker::send_msg_to_client /
// send_special_control, Dp2QtMsg::pack, ClientPkgHead::pack,
// DPSafeModeHacker::hack, DPPropertyTransImpl::hack, ZEN_Thread_Mutex,
// the tss_dp_comm config singleton and the IDPHacker default virtuals.

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <list>
#include <map>
#include <vector>

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice is the primary reconstruction
// for two ORIG CUs:
//   dp_cf_room_qtalk.cpp, dp_pkg_delay_hacker.cpp
// (dp_delay_hacker.cpp carries the same block but its primary reconstruction
// is hackers_a_agent.cpp.)
// Each block is independent: ios_base::Init 1, __cxa_atexit 3,
// ZEN_STATUS_ITEM_WITHNAME ctor 16, guard acquire/release 1,
// TssSdkCommSvc::TssSdkCommSvc 1, ~ZEN_STATUS_ITEM_WITHNAME 1,
// __cxa_guard_abort 1, _Unwind_Resume 1.  Emit one real TU-local block per
// ORIG CU.  The weak Singleton<TssSdkCommSvc>::ObjectCreator pre-create
// below is one shared object in the shipped DSO.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID dp_cf_room_qtalk
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_pkg_delay_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"


template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_trace_log_msg.h"
// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo comes from the canonical layout/user_info.h
// (user_info_recon.h).  DPHackBanModeInfo/DPHackCfUserInfo/DPHackDelayCmd are
// the same ORIG nested aggregates.
// ---------------------------------------------------------------------------
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

// ---------------------------------------------------------------------------
// tss_dp_comm configuration comes from the shared ORIG-DWARF header
// (src/origsrc/formmog_conf_tsssdk_dp.h via src/closing/dp_config.h).
// The delay hacker reads cmd_hack.cf_room_qtalk_enable and
// cmd_hack.delay_cmd.safemode_drop_cmdlist.
// ---------------------------------------------------------------------------
#include "src/closing/dp_config.h"

// ---------------------------------------------------------------------------
// sec_proto wire structures used by DPCfRoomQtalkHacker.  They are packed
// aggregates, exactly as the tsf4g protocol generator emitted them.
// ---------------------------------------------------------------------------
namespace sec_proto {

struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;

struct ClientPkgHead {
    unsigned short protocol_id_;     // +0
    unsigned int seq_;               // +2 (left uninitialised by ORIG)
    unsigned short sln_id_;          // +6
    unsigned short sub_sln_id_;      // +8
    unsigned short body_len_;        // +10

    int pack(char *buf, unsigned int cut_size, unsigned int *used,
             unsigned int protocol_version) const;
} __attribute__((packed));

struct Login {
    unsigned int uin;                // +0
    unsigned int server_ip;          // +4
    unsigned char name_len;          // +8
    char name[256];                  // +9
} __attribute__((packed));

struct EnterChannel {
    unsigned char channel_id;        // +0
} __attribute__((packed));

struct CreateRoom {
    unsigned char room_type;         // +0
    unsigned char visitor_count;     // +1
} __attribute__((packed));

struct CreateRoomRes {
    unsigned short room_id;          // +0
} __attribute__((packed));

struct EnterRoom {
    unsigned short room_id;          // +0
    unsigned char room_type;         // +2
    unsigned char visitor_count;     // +3
    unsigned char role_type;         // +4
    unsigned char role_position;     // +5
} __attribute__((packed));

struct ChangeRole {
    unsigned char role_position;     // +0
} __attribute__((packed));

struct ChangePosition {
    unsigned char role_position;     // +0
    unsigned char role_type;         // +1
} __attribute__((packed));

struct RoleAction {
    unsigned char role_action;       // +0
    unsigned int revert;             // +1
} __attribute__((packed));

struct RoomMaster {
    unsigned char master_role_type;      // +0
    unsigned char master_role_position;  // +1
} __attribute__((packed));

struct ChangeRoomType {
    unsigned char room_type;         // +0
} __attribute__((packed));

union Dp2QtMsgBody {
    Login login;
    EnterChannel enter_channel;
    CreateRoom create_room;
    CreateRoomRes create_room_res;
    EnterRoom enter_room;
    ChangeRole change_role;
    ChangePosition change_position;
    RoleAction role_action;
    RoomMaster room_master;
    ChangeRoomType room_type;
} __attribute__((packed));

struct Dp2QtMsg {
    unsigned char cmd_id;            // +0
    Dp2QtMsgBody body;               // +1
    int pack(char *buf, unsigned int cut_size, unsigned int *used,
             unsigned int protocol_version) const;
} __attribute__((packed));

}  // namespace sec_proto

// zen_lock_thread_mutex.h: 28 bytes, virtual dtor + lock/try_lock/unlock
// (global namespace; ZEN_Lock_Base base).
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
namespace tss_dpsdk {

}  // namespace tss_dpsdk

// Canonical hacker hierarchy.  IDPHackerSafe::locker_ is protected there and
// DPPkgDelayHacker() is declared, so the former local copies are gone.
#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"

namespace tss_dpsdk {

// ORIG flag bits of DPPkgDelayHacker::flag_position_ (private nested enum in
// the shipped header; the canonical layout header keeps only the data
// members, so the two values live at namespace scope here).
enum {
    E_IN_DELAY = 1,
    E_IN_SPECIAL_CONTROL = 2
};

class DPSafeModeHacker : public IDPHacker {
public:
    DPSafeModeHacker() {}
    virtual ~DPSafeModeHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// zen_lock_thread_mutex.h: 28 bytes, virtual dtor + lock/try_lock/unlock.
// The ORIG calls lock through vptr+0x08 and unlock through vptr+0x10.
class DPMsgPacker {
public:
    static int send_msg_to_client(int channel, tss_sdk_impl::UserInfo *user,
                                  unsigned char *buf, unsigned short len);
    static int send_special_control(tss_sdk_impl::UserInfo *user, int cmd_id);
};

// DPPkgDelayHacker, DWARF 84 bytes:
//   +0x00 vptr                         (IDPHacker/IDPHackerSafe base)
//   +0x04 ZEN_Thread_Mutex locker_     (28)
//   +0x20 map<unsigned, DPCommandBuf2> waitting_for_send_
//   +0x38 list<DPCommandBuf>           cmd_delay_
//   +0x40 timeval                      next_timer_
//   +0x48 int                          flag_position_
//   +0x4c unsigned                     node_wait_for_send_
//   +0x50 unsigned                     node_in_delay_
// The class is abstract in ORIG: its hack slot is __cxa_pure_virtual.
class DPCfRoomQtalkHacker : public IDPHacker {
public:
    DPCfRoomQtalkHacker() {}
    virtual ~DPCfRoomQtalkHacker() {}
    virtual int init(IDPHackerEventNotify *event_notify) {
        event_notify->register_event(E_LOGIN, this);
        return 0;
    }
    virtual int on_login(tss_sdk_impl::UserInfo *user);
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);

private:
    int notify_client(tss_sdk_impl::UserInfo *user,
                      const sec_proto::Dp2QtMsg &msg);
};


}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Layout checks using only public/standard-layout types.  The ORIG 32-bit
// GCC 4.1 std::map/std::list sizes must be 24/8 for sizeof(DPPkgDelayHacker)
// to be 0x54; those checks are in the class-size typedef at the bottom.
// ---------------------------------------------------------------------------
typedef char Check_ClientPkgHead_size[
    sizeof(sec_proto::ClientPkgHead) == 12 ? 1 : -1];
typedef char Check_Login_size[sizeof(sec_proto::Login) == 265 ? 1 : -1];
typedef char Check_Dp2QtMsgBody_size[
    sizeof(sec_proto::Dp2QtMsgBody) == 265 ? 1 : -1];
typedef char Check_Dp2QtMsg_size[sizeof(sec_proto::Dp2QtMsg) == 266 ? 1 : -1];
typedef char Check_Dp2QtMsg_body_offset[
    __builtin_offsetof(sec_proto::Dp2QtMsg, body) == 1 ? 1 : -1];
typedef char Check_EnterRoom_size[sizeof(sec_proto::EnterRoom) == 6 ? 1 : -1];
typedef char Check_RoleAction_size[
    sizeof(sec_proto::RoleAction) == 5 ? 1 : -1];
typedef char Check_DPValueArray_8_size[
    sizeof(tss_dp_comm::DPValueArray_8) == 34 ? 1 : -1];
typedef char Check_DPDelayCmd_size[
    sizeof(tss_dp_comm::DPDelayCmd) == 35 ? 1 : -1];
typedef char Check_DPCmdHack_flag_offset[
    __builtin_offsetof(tss_dp_comm::DPCmdHack, cf_room_qtalk_enable) == 0x25
        ? 1 : -1];
typedef char Check_DPConfig_cmd_hack_offset[
    __builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack) == 0x3d7e
        ? 1 : -1];
typedef char Check_UserInfo_size[
    sizeof(tss_sdk_impl::UserInfo) == 400 ? 1 : -1];
typedef char Check_UserInfo_hack_data_offset[
    __builtin_offsetof(tss_sdk_impl::UserInfo, hack_data_) == 220 ? 1 : -1];
typedef char Check_UserInfo_cf_offset[
    __builtin_offsetof(tss_sdk_impl::UserInfo,
                       hack_data_.cf_user_info_) == 388 ? 1 : -1];
typedef char Check_UserInfo_delay_offset[
    __builtin_offsetof(tss_sdk_impl::UserInfo,
                       hack_data_.delay_cmd_) == 396 ? 1 : -1];
typedef char Check_DPCommandBuf_size[
    sizeof(tss_dpsdk::DPPkgDelayHacker::DPCommandBuf) == 24 ? 1 : -1];
typedef char Check_DPCommandBuf2_size[
    sizeof(tss_dpsdk::DPPkgDelayHacker::DPCommandBuf2) == 12 ? 1 : -1];
typedef char Check_ZEN_Thread_Mutex_size[
    sizeof(ZEN_Thread_Mutex) == 28 ? 1 : -1];
typedef char Check_IDPHackerSafe_size[
    sizeof(tss_dpsdk::IDPHackerSafe) == 32 ? 1 : -1];
typedef char Check_DPPkgDelayHacker_size[
    sizeof(tss_dpsdk::DPPkgDelayHacker) == 0x54 ? 1 : -1];
typedef char Check_DPCfRoomQtalkHacker_size[
    sizeof(tss_dpsdk::DPCfRoomQtalkHacker) == 4 ? 1 : -1];
typedef char Check_DPSafeModeHacker_size[
    sizeof(tss_dpsdk::DPSafeModeHacker) == 4 ? 1 : -1];

// ===========================================================================
// DPCfRoomQtalkHacker
// ===========================================================================

int tss_dpsdk::DPCfRoomQtalkHacker::notify_client(
    tss_sdk_impl::UserInfo *user, const sec_proto::Dp2QtMsg &msg) {
    unsigned char send_buf[0x10000];
    unsigned int send_buf_len = 0;

    // 0xfff3 + 12 = 0xffff; the head is packed immediately before the body.
    int res = msg.pack(reinterpret_cast<char *>(send_buf + 0x0c), 0xfff3,
                       &send_buf_len, 0);
    if (res != 0) {
        // ORIG: uin is only dereferenced when user != NULL on this path.
        unsigned int uin = 0;
        if (user != 0)
            uin = user->base_info_.uin_;
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], pack fail, res=%d", uin, __func__, res);
        return res;
    }

    sec_proto::ClientPkgHead head;  // seq_ deliberately left uninitialised
    head.protocol_id_ = 0x33;
    head.sln_id_ = 0x0d;
    head.sub_sln_id_ = 0;
    head.body_len_ = static_cast<unsigned short>(send_buf_len);
    head.pack(reinterpret_cast<char *>(send_buf), 0xffff, 0, 0);

    return DPMsgPacker::send_msg_to_client(
        0, user, send_buf,
        static_cast<unsigned short>(send_buf_len + 0x0c));
}

int tss_dpsdk::DPCfRoomQtalkHacker::on_login(
    tss_sdk_impl::UserInfo *user) {
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_
             ->cmd_hack.cf_room_qtalk_enable)
        return 0;

    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], login info, role_name_=%s, server_ip_=%u",
        user->base_info_.uin_, __func__, user->base_info_.role_name_,
        user->base_info_.server_ip_);

    sec_proto::Dp2QtMsg msg;
    msg.cmd_id = 1;  // Login
    msg.body.login.uin = user->base_info_.uin_;
    msg.body.login.server_ip = user->base_info_.server_ip_;
    // ORIG truncates the length byte and copies exactly that byte count.
    unsigned char name_len =
        static_cast<unsigned char>(strlen(user->base_info_.role_name_) + 1);
    msg.body.login.name_len = name_len;
    memcpy(msg.body.login.name, user->base_info_.role_name_, name_len);

    notify_client(user, msg);
    return 0;
}

int tss_dpsdk::DPCfRoomQtalkHacker::hack(
    tss_sdk_impl::UserInfo *user, int type, unsigned int cmd_id,
    unsigned char *&buf, unsigned int &len) {
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_
             ->cmd_hack.cf_room_qtalk_enable)
        return 0;

    // The default path in ORIG reaches notify_client without initialising
    // msg; keep the same POD local and the same call order.
    sec_proto::Dp2QtMsg msg;

    if (type == 1) {
        switch (cmd_id) {
        case 0x13e00:
        case 0x53e00: {
            msg.cmd_id = 10;  // ChangeRoomType
            msg.body.room_type.room_type =
                user->hack_data_.cf_user_info_.game_mode_;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], change_room_type, room_type=%u",
                user->base_info_.uin_, __func__,
                user->hack_data_.cf_user_info_.game_mode_);
            break;
        }
        case 0x11f00:
        case 0x56600: {
            msg.cmd_id = 2;  // EnterChannel
            msg.body.enter_channel.channel_id =
                user->hack_data_.cf_user_info_.channel_id_;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], enter channel, channel_id=%u",
                user->base_info_.uin_, __func__,
                user->hack_data_.cf_user_info_.channel_id_);
            break;
        }
        case 0x13400: {
            if (len <= 0x1d) {
                unsigned int uin = user != 0 ? user->base_info_.uin_ : 0;
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    uin, __func__, 1, cmd_id, len, 0x1e);
                return -1;
            }
            msg.cmd_id = 3;  // CreateRoom
            msg.body.create_room.room_type =
                user->hack_data_.cf_user_info_.game_mode_;
            msg.body.create_room.visitor_count =
                static_cast<unsigned char>(buf[0x1e] >> 1);
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], create_room, room_type=%u, visitor_count=%u",
                user->base_info_.uin_, __func__,
                user->hack_data_.cf_user_info_.game_mode_,
                static_cast<unsigned int>(buf[0x1e] >> 1));
            break;
        }
        case 0x56e00: {
            if (len <= 0x35) {
                unsigned int uin = user != 0 ? user->base_info_.uin_ : 0;
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    uin, __func__, 1, cmd_id, len, 0x36);
                return -1;
            }
            msg.cmd_id = 3;  // CreateRoom, logged as create_team_room
            msg.body.create_room.room_type =
                user->hack_data_.cf_user_info_.game_mode_;
            msg.body.create_room.visitor_count =
                static_cast<unsigned char>(buf[0x36] >> 1);
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], create_team_room, room_type=%u, "
                "visitor_count=%u",
                user->base_info_.uin_, __func__,
                user->hack_data_.cf_user_info_.game_mode_,
                static_cast<unsigned int>(buf[0x36] >> 1));
            break;
        }
        default:
            return 0;
        }
    } else if (type == 2) {
        switch (cmd_id) {
        case 0x12800:
        case 0x51300:
        case 0x13d00: {
            msg.cmd_id = 7;  // ChangePosition
            msg.body.change_position.role_position =
                user->hack_data_.cf_user_info_.role_posi_;
            msg.body.change_position.role_type =
                user->hack_data_.cf_user_info_.role_type_;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], change_position, role_type=%u, "
                "role_position=%u",
                user->base_info_.uin_, __func__,
                user->hack_data_.cf_user_info_.role_type_,
                user->hack_data_.cf_user_info_.role_posi_);
            break;
        }
        case 0x13500:
        case 0x56f00: {
            msg.cmd_id = 4;  // CreateRoomRes
            msg.body.create_room_res.room_id =
                user->hack_data_.cf_user_info_.room_id_;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], create_room_res, room_id=%u",
                user->base_info_.uin_, __func__,
                static_cast<unsigned int>(
                    user->hack_data_.cf_user_info_.room_id_));
            break;
        }
        case 0x13700:
        case 0x50900:
        case 0x14a00: {
            if (len <= 0x0d) {
                unsigned int uin = user != 0 ? user->base_info_.uin_ : 0;
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    uin, __func__, 2, cmd_id, len, 0x0e);
                return -1;
            }
            unsigned char visitor_count;
            if (cmd_id == 0x50900) {
                // ORIG uses an arithmetic shift (floor) of the signed
                // difference, not C division truncation.
                int diff = 0x10 - static_cast<int>(buf[0x0c]);
                visitor_count = static_cast<unsigned char>(diff >> 1);
            } else {
                visitor_count = static_cast<unsigned char>(buf[0x0e] >> 1);
            }
            msg.cmd_id = 5;  // EnterRoom
            msg.body.enter_room.room_id =
                user->hack_data_.cf_user_info_.room_id_;
            msg.body.enter_room.room_type =
                user->hack_data_.cf_user_info_.game_mode_;
            msg.body.enter_room.visitor_count = visitor_count;
            msg.body.enter_room.role_type =
                user->hack_data_.cf_user_info_.role_type_;
            msg.body.enter_room.role_position =
                user->hack_data_.cf_user_info_.role_posi_;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], enter_room, room_id=%u, room_type=%u, "
                "visitor_count=%u, role_type=%u, role_position=%u",
                user->base_info_.uin_, __func__,
                static_cast<unsigned int>(
                    user->hack_data_.cf_user_info_.room_id_),
                user->hack_data_.cf_user_info_.game_mode_,
                static_cast<unsigned int>(visitor_count),
                user->hack_data_.cf_user_info_.role_type_,
                user->hack_data_.cf_user_info_.role_posi_);
            break;
        }
        case 0x14e00: {  // JOIN_GAME
            if (!(user->base_info_.flags_ & 8))
                return 0;
            msg.cmd_id = 8;  // RoleAction
            msg.body.role_action.role_action = 6;
            msg.body.role_action.revert = 0;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], JOIN_GAME", user->base_info_.uin_, __func__);
            break;
        }
        case 0x14f00: {  // START_PLAY
            if (len <= 6) {
                unsigned int uin = user != 0 ? user->base_info_.uin_ : 0;
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    uin, __func__, 2, cmd_id, len, 7);
                return -1;
            }
            if (static_cast<unsigned char>(buf[7] - 1) <= 0xfd)
                return 0;
            if (!(user->base_info_.flags_ & 8))
                return 0;
            msg.cmd_id = 8;  // RoleAction
            msg.body.role_action.role_action = 6;
            msg.body.role_action.revert = 0;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], START_PLAY", user->base_info_.uin_, __func__);
            break;
        }
        case 0x15b00: {  // STOP_PLAY
            msg.cmd_id = 8;  // RoleAction
            msg.body.role_action.role_action = 4;
            msg.body.role_action.revert = 0;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], STOP_PLAY", user->base_info_.uin_, __func__);
            break;
        }
        case 0x14100:
        case 0x50b00: {  // EXIT_ROOM
            msg.cmd_id = 8;  // RoleAction
            msg.body.role_action.role_action = 5;
            msg.body.role_action.revert = 0;
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
                "[%u][%s], EXIT_ROOM", user->base_info_.uin_, __func__);
            break;
        }
        default: {
            unsigned int uin = user != 0 ? user->base_info_.uin_ : 0;
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR, "[%u][%s], unknown msg, cmd_id=%u, type=%d",
                uin, __func__, cmd_id, 2);
            return 0;
        }
        }
    }

    // Reached for type 1/2 after a message was built, and for every other
    // type value (where msg is uninitialised in ORIG).
    notify_client(user, msg);
    return 0;
}


// ===========================================================================
// DPPkgDelayHacker
// ===========================================================================

tss_dpsdk::DPPkgDelayHacker::DPPkgDelayHacker()
    : next_timer_(),
      flag_position_(0),
      node_wait_for_send_(0),
      node_in_delay_(0) {
    static int flags = 0;
    flag_position_ = flags++;
    // Only three delay-hacker instances may exist process-wide; the fourth
    // construction throws the slot index as an int (GOT typeinfo is _ZTIi).
    if (flag_position_ > 2)
        throw flag_position_;
    // The map/list are default-constructed by the member initialisers; the
    // ORIG also zeroes next_timer_ and both node counters here.
    next_timer_.tv_sec = 0;
    next_timer_.tv_usec = 0;
}

int tss_dpsdk::DPPkgDelayHacker::on_timer(DPCommandBuf &buf) {
    int ret = DPMsgPacker::send_special_control(
        buf.user_, static_cast<int>(buf.cmd_id_));
    if (ret != 0)
        return ret;

    // operator[] performs ORIG's lower_bound + insert(hint, zeroed value).
    // The key is the UserInfo pointer bits (map<unsigned int, ...>).
    DPCommandBuf2 &pending =
        waitting_for_send_[reinterpret_cast<unsigned int>(buf.user_)];
    if (pending.buf_ != 0)
        delete pending.buf_;  // ORIG frees new[] storage with scalar delete
    else
        ++node_wait_for_send_;

    pending.buf_ = buf.buf_;
    buf.buf_ = 0;
    pending.buf_len_ = buf.buf_len_;
    buf.buf_len_ = 0;
    pending.cmd_id_ = buf.cmd_id_;

    if (flag_position_ <= 2) {
        unsigned char &flag = buf.user_->hack_data_.delay_cmd_
                                  .delay_mode_[flag_position_];
        if (!(flag & E_IN_SPECIAL_CONTROL))
            flag |= E_IN_SPECIAL_CONTROL;
    }
    return ret;
}

int tss_dpsdk::DPPkgDelayHacker::insert(DPCommandBuf &buf) {
    ++node_in_delay_;

    bool insert_front = true;
    if (!cmd_delay_.empty()) {
        // Sorted by expire_time_ ascending.  A candidate at or after the
        // current head is inserted in order; an earlier one updates the head.
        if (buf.expire_time_.tv_sec > next_timer_.tv_sec ||
            (buf.expire_time_.tv_sec == next_timer_.tv_sec &&
             buf.expire_time_.tv_usec >= next_timer_.tv_usec)) {
            insert_front = false;
            CommandBufIterator pos = cmd_delay_.begin();
            for (; pos != cmd_delay_.end(); ++pos) {
                if (pos->expire_time_.tv_sec > buf.expire_time_.tv_sec)
                    break;
                if (pos->expire_time_.tv_sec < buf.expire_time_.tv_sec)
                    continue;
                if (pos->expire_time_.tv_usec > buf.expire_time_.tv_usec)
                    break;
            }
            cmd_delay_.insert(pos, buf);
        }
    }

    if (insert_front) {
        next_timer_ = buf.expire_time_;
        cmd_delay_.push_front(buf);
    }
    return 0;
}

int tss_dpsdk::DPPkgDelayHacker::remove_from_delay(
    tss_sdk_impl::UserInfo *user, CommandBufIterator &it) {
    delete (*it).buf_;  // scalar delete, matching ORIG _ZdlPv
    cmd_delay_.erase(it);
    --node_in_delay_;

    if (flag_position_ <= 2) {
        unsigned char &flag =
            user->hack_data_.delay_cmd_.delay_mode_[flag_position_];
        if (flag & E_IN_DELAY)
            flag &= ~E_IN_DELAY;
    }
    return 0;
}

int tss_dpsdk::DPPkgDelayHacker::on_logout(
    tss_sdk_impl::UserInfo *user) {
    if (flag_position_ > 2)
        return 0;

    unsigned char &flag =
        user->hack_data_.delay_cmd_.delay_mode_[flag_position_];
    bool in_delay = false;
    bool in_special_control = false;
    if (flag & E_IN_DELAY) {
        flag &= ~E_IN_DELAY;
        in_delay = true;
    }
    if (flag & E_IN_SPECIAL_CONTROL) {
        flag &= ~E_IN_SPECIAL_CONTROL;
        in_special_control = true;
    }
    if (!in_delay && !in_special_control)
        return 0;

    locker_.lock();
    try {
        if (in_delay) {
            for (CommandBufIterator it = cmd_delay_.begin();
                 it != cmd_delay_.end(); ++it) {
                if ((*it).user_ == user) {
                    remove_from_delay(user, it);
                    break;
                }
            }
        }
        if (in_special_control) {
            std::map<unsigned int, DPCommandBuf2>::iterator it =
                waitting_for_send_.find(
                    reinterpret_cast<unsigned int>(user));
            if (it != waitting_for_send_.end()) {
                delete it->second.buf_;
                waitting_for_send_.erase(it);
                --node_wait_for_send_;
            }
        }
    } catch (...) {
        locker_.unlock();
        throw;
    }
    locker_.unlock();
    return 0;
}

int tss_dpsdk::DPPkgDelayHacker::proc(time_t now) {
    if (next_timer_.tv_sec > now)
        return 0;
    if (cmd_delay_.empty())
        return 0;

    timeval tv;
    gettimeofday(&tv, 0);
    if (tv.tv_sec < next_timer_.tv_sec)
        return 0;
    if (tv.tv_sec == next_timer_.tv_sec &&
        tv.tv_usec < next_timer_.tv_usec)
        return 0;

    locker_.lock();
    try {
        while (!cmd_delay_.empty()) {
            CommandBufIterator it = cmd_delay_.begin();
            DPCommandBuf buf = *it;

            bool expired =
                tv.tv_sec > buf.expire_time_.tv_sec ||
                (tv.tv_sec == buf.expire_time_.tv_sec &&
                 tv.tv_usec >= buf.expire_time_.tv_usec);
            if (!expired) {
                // First node in the future becomes the new head deadline.
                next_timer_ = buf.expire_time_;
                break;
            }

            // ORIG ignores the virtual on_timer return value and always
            // releases the list node / flag afterwards.
            on_timer(buf);
            delete buf.buf_;
            cmd_delay_.erase(it);
            --node_in_delay_;

            if (flag_position_ <= 2) {
                unsigned char &flag = buf.user_->hack_data_.delay_cmd_
                                          .delay_mode_[flag_position_];
                if (flag & E_IN_DELAY)
                    flag &= ~E_IN_DELAY;
            }
        }
    } catch (...) {
        locker_.unlock();
        throw;
    }
    locker_.unlock();
    return 0;
}

int tss_dpsdk::DPPkgDelayHacker::insert(
    tss_sdk_impl::UserInfo *user, int delay_seconds, unsigned int cmd_id,
    unsigned char *buf, unsigned int &len) {
    // ORIG logs the old slot value before validating it, then again after
    // setting E_IN_DELAY.
    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], before set_flag %u", user->base_info_.uin_, __func__,
        static_cast<unsigned int>(
            user->hack_data_.delay_cmd_.delay_mode_[flag_position_]));

    if (flag_position_ > 2)
        return -1;
    unsigned char &flag =
        user->hack_data_.delay_cmd_.delay_mode_[flag_position_];
    if (flag & E_IN_DELAY)
        return -1;

    flag |= E_IN_DELAY;
    ZEN_Trace_LogMsg::debug_output(
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG,
        "[%u][%s], after set_flag %u", user->base_info_.uin_, __func__,
        static_cast<unsigned int>(flag));

    unsigned int copy_len = len;
    DPCommandBuf command;
    command.cmd_id_ = cmd_id;
    command.user_ = user;
    gettimeofday(&command.expire_time_, 0);
    command.expire_time_.tv_sec += delay_seconds;
    command.buf_len_ = copy_len;
    command.buf_ = new unsigned char[copy_len];
    if (copy_len != 0)
        memcpy(command.buf_, buf, copy_len);

    locker_.lock();
    int res;
    try {
        res = insert(command);
    } catch (...) {
        locker_.unlock();
        throw;
    }
    locker_.unlock();
    return res;
}

int tss_dpsdk::DPPkgDelayHacker::on_special_control(
    tss_sdk_impl::UserInfo *user, unsigned char *buf, unsigned int &buf_len,
    unsigned char *out, unsigned int &out_len) {
    if (flag_position_ > 2)
        return 0;
    unsigned char &flag =
        user->hack_data_.delay_cmd_.delay_mode_[flag_position_];
    if (!(flag & E_IN_SPECIAL_CONTROL))
        return 0;
    flag &= ~E_IN_SPECIAL_CONTROL;

    locker_.lock();
    try {
        std::map<unsigned int, DPCommandBuf2>::iterator it =
            waitting_for_send_.find(reinterpret_cast<unsigned int>(user));
        if (it == waitting_for_send_.end()) {
            locker_.unlock();
            return 0;
        }

        DPCommandBuf2 &pending = it->second;
        unsigned int copy_len = pending.buf_len_;
        if (copy_len > out_len) {
            locker_.unlock();
            return 0;
        }

        if (user->hack_data_.safemode_level_ != 0) {
            tss_dp_comm::DPValueArray_8 &cmd_list =
                tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.delay_cmd
                    .safemode_drop_cmdlist;
            for (unsigned short i = 0; i < cmd_list.length; ++i) {
                if (cmd_list.values[i] == pending.cmd_id_) {
                    // Scoped temporary: ORIG constructs a bare
                    // DPSafeModeHacker (4-byte vptr object) and calls the
                    // non-virtual hack entry directly.
                    DPSafeModeHacker safe_mode_hacker;
                    safe_mode_hacker.DPSafeModeHacker::hack(
                        user, 1, pending.cmd_id_, pending.buf_,
                        pending.buf_len_);
                    break;
                }
            }
        }

        out_len = copy_len;
        if (copy_len != 0)
            memcpy(out, pending.buf_, copy_len);
        delete pending.buf_;  // scalar delete, matching ORIG _ZdlPv
        waitting_for_send_.erase(it);
        --node_wait_for_send_;
    } catch (...) {
        locker_.unlock();
        throw;
    }
    locker_.unlock();
    return 1;  // packet consumed
}
