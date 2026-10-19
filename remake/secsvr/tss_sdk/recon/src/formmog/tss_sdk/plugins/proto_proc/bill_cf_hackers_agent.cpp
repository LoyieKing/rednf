// Semantic reconstruction of tss_dpsdk bill/CF units from
// libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1, -O2 -fPIC).
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   tss_dpsdk::DPBillRecorder::write_to_logsvr(LogSvrCommBillRecord const&)
//       0x000a3120..0x000a33e5 (0x2c5)
//   tss_dpsdk::DPBillRecorder::~DPBillRecorder()  D1 0x000a3430 (0x2e)
//                                                 D0 0x000a33f0 (0x3f)
//   tss_dpsdk::DPCfExtractUserInfo::hack(...)        0x000a37c0 (0xb93)
//   tss_dpsdk::DPCfForwardRoomMasterMsg::hack(...)   0x000a4970 (0x97)
//   tss_dpsdk::DPCfRecordUserInfo::hack(...)         0x000a4f50 (0x20d)
//   corresponding weak D1/D0 destructors at 0xa4860/0xa4890,
//   0xa4e20/0xa4e60 and 0xa5570/0xa55a0.
//
// This is a semantic reconstruction, not a byte-identical one.  External
// symbols named below are the exact targets referenced by the original
// instruction sequences and remain external here.

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
// ---------------------------------------------------------------------------
// ZEN trace boundary (zen_trace_log_basic.h).  The target mangling is
// _ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz.
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo (DWARF byte_size 400).
//
// base_info_ +0x000
//   uin_ +0x00, game_id_ +0x04, room_id_ +0x08, client_ip_ +0x0c,
//   server_ip_ +0x10, game_usr_id_ +0x14, client_version_ +0x18,
//   client_user_id_ +0x1c, flags_ +0x20, role_name_[64] +0x24
// user_code_info_ +0x064, user_timer_ +0x094, user_stat_ +0x0a8,
// hack_data_ +0x0dc (anonymous struct ._486, 180 bytes)
//   cf_user_info_ +0x0a8  => absolute UserInfo +0x184
//
// Only the fields touched by the reconstructed units are named; the untouched
// regions preserve the exact offsets.
// ---------------------------------------------------------------------------
// Canonical tag-394 declarations: UserInfo, bill record, frame/service ids,
// hacker hierarchy and the shared DPConfig/DP_CONFIG.
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"
#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/user_info.h"

namespace sec_proto {
// LogSvrCommBillRecord is only read as a 2048-byte payload here.  The
// canonical header pulls the vendor TdrBuf runtime, which is mutually
// exclusive with src/closing/dp_config.h's TDR model in one TU, so this
// declaration keeps the ORIG byte layout and the base-class forward name.
struct LogSvrCommBillRecord {
    char string_buffer_[2048];   // +0x0000
    int pack(char *buf, unsigned int buf_len,
             unsigned int *used_len, unsigned int version) const;
};
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;
}  // namespace sec_proto

// The out-of-line weak instance() is owned by tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice merges four ORIG CUs:
//   dp_bill_recorder.cpp
//   dp_cf_extract_user_info.cpp
//   dp_cf_forward_room_master_msg.cpp
//   dp_cf_record_user_info.cpp
// Each carried an independent 996-byte block: ios_base::Init 1,
// __cxa_atexit 3, ZEN_STATUS_ITEM_WITHNAME ctor 16, guard acquire/release 1,
// TssSdkCommSvc::TssSdkCommSvc 1, ~ZEN_STATUS_ITEM_WITHNAME 1,
// __cxa_guard_abort 1, _Unwind_Resume 1 (audit/report/init_brief.md).
// The 16-entry status array and __ioinit are TU-local objects, so emit one
// real block per ORIG CU.  The weak
// Singleton<TssSdkCommSvc>::ObjectCreator pre-create below is one shared
// object in the shipped DSO and stays one definition.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID dp_bill_recorder
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_cf_extract_user_info
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_cf_forward_room_master_msg
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_cf_record_user_info
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID


template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// ---------------------------------------------------------------------------
// ORIG weak D0/D1 (vtable 0x00292ed0 is 0x14 bytes); this TU owns the key
// function write_to_logsvr, so it also emits the destructor/vtable.  The
// definition is strong here: a weak out-of-line definition is only emitted
// when odr-used, and the vtable reference from another TU is not an odr-use.
tss_dpsdk::DPBillRecorder::~DPBillRecorder() {}

// DPBillRecorder::write_to_logsvr
// ORIG 0x000a3120..0x000a33e5.
//
// Line 19: trace priority is RS_INFO when enbale_bill_to_log_ is true and
// RS_DEBUG otherwise (cmpb $1 / sbb); format "%s" prints pkg.string_buffer_,
// i.e. the LogSvrCommBillRecord payload at offset 0.
//
// Line 21 inlines Singleton<TssSdkCommSvc>::instance() and
// send_ipc_pkg<LogSvrCommBillRecord>: a 65536-byte Comm_App_Frame stack
// buffer, init_framehead(0x10000, 0, 0x4e21), services send=0x2b / recv=2 /
// proxy=0, frame_uin = get_userid(), app_id = glb_dp_config_head_->game_id,
// appdata_encode(0xffbd, pkg, 0, 0), then
// send_buf_to_backend(frame, frame_length_, true, 0).
//
// Returns 0 on success, 0x139f when appdata_encode fails (the encoder's own
// 0x13a0 is replaced), and 0xfffffffe (-2) when send_buf_to_backend fails
// (after debug_errorex("[%s]send to backend fail. ret=%d",
// __PRETTY_FUNCTION__, ret)).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPBillRecorder::write_to_logsvr(
    const sec_proto::LogSvrCommBillRecord &pkg) {
    ZEN_Trace_LogMsg::debug_output(
        enbale_bill_to_log_ ? RS_INFO : RS_DEBUG, "%s",
        pkg.string_buffer_);

    // ORIG evaluates get_userid() and acquires the singleton before the
    // inlined frame construction; keep that side-effect order.
    const unsigned int uin = user_id_;
    tss_sdk_impl::TssSdkCommSvc *comm_svc =
        Singleton<tss_sdk_impl::TssSdkCommSvc>::instance();

    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg =
        reinterpret_cast<Comm_App_Frame *>(trans_send_buffer);

    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4e21u);

    SERVICES_ID rcvsvc(2, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x2b, 0);

    rsp_msg->frame_uin_ = uin;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;
    rsp_msg->backfill_trans_id_ = 0;
    rsp_msg->app_id_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    // ORIG tests raw bit 16 of the option word (inner_option_ bit 0x10000);
    // init_framehead leaves it clear, so this dump is normally skipped.
    if (rsp_msg->inner_option_.inner_option_ & 0x10000u)
        Comm_App_Frame::dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, pkg, 0, 0);
    if (ret != 0)
        return 0x139f;

    ret = comm_svc->send_buf_to_backend(
        reinterpret_cast<const char *>(trans_send_buffer),
        rsp_msg->frame_length_, true, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]send to backend fail. ret=%d",
            "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
            "unsigned int, unsigned int, short unsigned int) [with PKG = "
            "sec_proto::LogSvrCommBillRecord]",
            ret);
        return -2;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Hacker hierarchy (DWARF / vtable shapes).
//
// IDPHacker vptr slots:
//   +0x00 init, +0x04 hack, +0x08 on_login, +0x0c on_logout, +0x10 proc,
//   +0x14 on_ban_mode, +0x18 on_drop_cmd, +0x1c on_verify_response,
//   +0x20 on_verify_msg, +0x24 on_safe_mode, +0x28 on_special_control,
//   +0x2c D1, +0x30 D0.
// DPCfExtractUserInfo/DPCfRecordUserInfo derive IDPHacker directly;
// DPCfForwardRoomMasterMsg derives DPPkgForwardLaHacker.  All four-byte
// classes have 0x3c-byte vtables (15 slots, no new virtuals).
// ---------------------------------------------------------------------------
namespace tss_dpsdk {


class DPPkgForwardLaHacker : public IDPHacker {
public:
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual ~DPPkgForwardLaHacker() {}
};

class DPCfExtractUserInfo : public IDPHacker {
public:
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual ~DPCfExtractUserInfo() {}
};

class DPCfForwardRoomMasterMsg : public DPPkgForwardLaHacker {
public:
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual ~DPCfForwardRoomMasterMsg() {}
};

class DPCfRecordUserInfo : public IDPHacker {
public:
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
    virtual ~DPCfRecordUserInfo() {}
};

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// DPCfExtractUserInfo::hack
// ORIG 0x000a37c0..0x000a4353.
//
// The command/type dispatch below is the exact original control flow.  The
// first command test is hoisted before the type==1 test in ORIG, therefore
// 0x13e00 reaches the first group for every type; 0x53e00 only for type==1.
// All length comparisons are unsigned (jbe), all packet reads are unaligned.
//
// Field writes address UserInfo+0x184..0x18a, i.e.
//   game_mode_   UserInfo+0x184
//   role_posi_   UserInfo+0x185
//   role_type_   UserInfo+0x186
//   game_map_    UserInfo+0x187
//   channel_id_  UserInfo+0x188
//   room_id_     UserInfo+0x18a (unsigned short)
//
// Return values: 0 on handled/success, -1 on malformed packet or enter-room
// lookup failure.  Unknown command/type returns 0.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPCfExtractUserInfo::hack(tss_sdk_impl::UserInfo *user,
                                         int type, unsigned int cmd_id,
                                         unsigned char *&buf,
                                         unsigned int &len) {
    const unsigned char *data = buf;
    tss_sdk_impl::DPHackCfUserInfo &cf = user->hack_data_.cf_user_info_;

    // ORIG: cmp $0x13e00 before the type==1 check (see header comment).
    if (cmd_id == 0x13e00u || (type == 1 && cmd_id == 0x53e00u)) {
        // Source line 21: expected packet length 14.
        if (len <= 0x0du) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", type,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x0e));
            return -1;
        }

        // Lines 23/24.  game_map_ = data[0x0c], game_mode_ = data[0x0e].
        cf.game_map_ = data[0x0c];
        cf.game_mode_ = data[0x0e];

        // Line 65.
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], game_map_=%u, game_mode=%u",
            user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.game_map_),
            static_cast<unsigned int>(cf.game_mode_));
        return 0;
    }

    if (type == 1) {
        if (cmd_id == 0x11f00u || cmd_id == 0x56600u) {
            // Line 38: expected packet length 6.
            if (len <= 0x05u) {
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    (user ? user->base_info_.uin_ : 0u), "hack", 1,
                cmd_id,
                    static_cast<unsigned int>(len),
                    static_cast<unsigned int>(0x06));
                return -1;
            }

            // Line 40: channel_id_ = data[6] + 1 (unsigned char wrap).
            cf.channel_id_ = static_cast<unsigned char>(data[6] + 1);

            // Line 42.
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], channel_id_=%u", user->base_info_.uin_, "hack",
                static_cast<unsigned int>(cf.channel_id_));
            return 0;
        }

        if (cmd_id == 0x13400u || cmd_id == 0x56e00u) {
            // Line 49: expected packet length 0x22.
            if (len <= 0x21u) {
                ZEN_Trace_LogMsg::debug_output(
                    RS_ERROR,
                    "[%u][%s], game data is wrong, type=%d, cmd=%u, "
                    "len=%u, expected_len=%u",
                    (user ? user->base_info_.uin_ : 0u), "hack", 1,
                cmd_id,
                    static_cast<unsigned int>(len),
                    static_cast<unsigned int>(0x22));
                return -1;
            }

            // Lines 53-63.  Bit 1 is set; role fields are cleared.
            user->set_user_flag(2);
            cf.role_type_ = 0;
            cf.role_posi_ = 0;
            if (cmd_id == 0x56e00u)
                cf.game_map_ = data[0x1e];      // line 57
            else
                cf.game_map_ = data[0x20];      // line 61
            cf.game_mode_ = data[0x22];         // line 63

            // Line 65.
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], game_map_=%u, game_mode=%u",
                user->base_info_.uin_, "hack",
                static_cast<unsigned int>(cf.game_map_),
                static_cast<unsigned int>(cf.game_mode_));
            return 0;
        }

        return 0;
    }

    if (type != 2)
        return 0;

    if (cmd_id == 0x13500u || cmd_id == 0x56f00u) {
        // Line 77: expected payload length 0x0c.
        if (len <= 0x0bu) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x0c));
            return -1;
        }

        // Line 79/81: status byte must be 0x9c.
        const unsigned char status = data[6];
        if (status != 0x9cu) {
            ZEN_Trace_LogMsg::debug_output(
                (user && (user->base_info_.flags_ & 1u)) ? RS_INFO
                                                         : RS_DEBUG,
                "[%u][%s], res=%u", (user ? user->base_info_.uin_ : 0u),
                "hack",
                static_cast<unsigned int>(status));
            return -1;
        }

        // Lines 85-89.
        user->clear_user_flag(0x0c);
        cf.room_id_ = static_cast<unsigned short>(
            static_cast<unsigned short>(data[0x0a] |
                                        (static_cast<unsigned short>(data[0x0b])
                                         << 8)) +
            1);
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], room_id_=%u", user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.room_id_));
        return 0;
    }

    if (cmd_id == 0x13700u || cmd_id == 0x14a00u ||
        cmd_id == 0x50900u) {
        // Lines 95-102: the uin table starts at data+0x4a for
        // 0x13700/0x14a00 and at data+0x46 for 0x50900.  Entries are
        // 0x124 bytes apart; entry 0 is the 32-bit uin at offset 0.
        const bool short_frame = (cmd_id == 0x50900u);
        const unsigned char *role_info =
            data + (short_frame ? 0x46 : 0x4a);
        const unsigned char *end = data + len;

        if (end <= role_info + 4) {
            // Line 102: enter room fail, i=0.
            ZEN_Trace_LogMsg::debug_output(
                (user && (user->base_info_.flags_ & 1u)) ? RS_INFO
                                                         : RS_DEBUG,
                "[%u][%s], enter room fail, i=%d",
                (user ? user->base_info_.uin_ : 0u), "hack", 0);
            return -1;
        }

        // Lines 97/105/106: scan up to 16 slots.  The first slot is tested
        // before the loop in the shipped code; this loop is equivalent.
        int i = 0;
        for (; i < 16; ++i) {
            if (end <= role_info + i * 0x124 + 4) {
                ZEN_Trace_LogMsg::debug_output(
                    (user && (user->base_info_.flags_ & 1u)) ? RS_INFO
                                                            : RS_DEBUG,
                    "[%u][%s], enter room fail, i=%d",
                    (user ? user->base_info_.uin_ : 0u), "hack", i);
                return -1;
            }
            unsigned int slot_uin;
            const unsigned char *slot = role_info + i * 0x124;
            slot_uin = static_cast<unsigned int>(slot[0]) |
                       (static_cast<unsigned int>(slot[1]) << 8) |
                       (static_cast<unsigned int>(slot[2]) << 16) |
                       (static_cast<unsigned int>(slot[3]) << 24);
            if (slot_uin == user->base_info_.uin_)
                break;
        }
        if (i == 16) {
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], enter room fail, i=%d",
                user->base_info_.uin_, "hack", i);
            return -1;
        }

        // Lines 108/109 and 113/114/118/119.
        cf.role_type_ = static_cast<unsigned char>(i > 7);
        cf.role_posi_ = static_cast<unsigned char>(i % 8);
        if (short_frame) {
            cf.game_map_ = data[0x0e];
            cf.game_mode_ = data[0x12];
        } else {
            cf.game_map_ = data[0x10];
            cf.game_mode_ = data[0x16];
        }

        // Line 121: room_id_ = u16(data+0x0a) + 1; clear flags 0x0e.
        user->clear_user_flag(0x0e);
        cf.room_id_ = static_cast<unsigned short>(
            static_cast<unsigned short>(data[0x0a] |
                                        (static_cast<unsigned short>(data[0x0b])
                                         << 8)) +
            1);

        // Lines 127/180 format: role_type, role_posi, game_map, game_mode,
        // room_id (the room id is printed with %d).
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], enter room, role_type_=%u, role_posi_=%u, "
            "game_map_=%u, game_mode=%u, room_id_=%d",
            user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.role_type_),
            static_cast<unsigned int>(cf.role_posi_),
            static_cast<unsigned int>(cf.game_map_),
            static_cast<unsigned int>(cf.game_mode_),
            static_cast<int>(cf.room_id_));
        return 0;
    }

    if (cmd_id == 0x13d00u) {
        // Line 142: expected packet length 10.
        if (len <= 0x09u) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x0a));
            return -1;
        }

        // Line 143/174: res byte must be zero.
        const unsigned char status = data[6];
        if (status != 0) {
            ZEN_Trace_LogMsg::debug_output(
                (user && (user->base_info_.flags_ & 1u)) ? RS_INFO
                                                         : RS_DEBUG,
                "[%u][%s], res=%u", (user ? user->base_info_.uin_ : 0u),
                "hack",
                static_cast<unsigned int>(status));
            return -1;
        }

        // Lines 149/150/178: toggle role_type_, set posi from data[0x0a].
        cf.role_type_ = static_cast<unsigned char>(cf.role_type_ == 0);
        cf.role_posi_ = data[0x0a];

        // Line 180.
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], update, type=%u, posi=%u",
            user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.role_type_),
            static_cast<unsigned int>(cf.role_posi_));
        return 0;
    }

    if (cmd_id == 0x51300u) {
        // Lines 159/161: toggle role_type_; no packet reads (the packet may
        // be shorter than 7 bytes - ORIG has no length test here).
        cf.role_type_ = static_cast<unsigned char>(cf.role_type_ == 0);

        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], update, type=%u, posi=%u",
            user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.role_type_),
            static_cast<unsigned int>(cf.role_posi_));
        return 0;
    }

    if (cmd_id == 0x12800u) {
        // Line 166: expected packet length 12.
        if (len <= 0x0bu) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x0c));
            return -1;
        }

        // Lines 171/174: res byte must be zero.
        const unsigned char status = data[6];
        if (status != 0) {
            ZEN_Trace_LogMsg::debug_output(
                (user && (user->base_info_.flags_ & 1u)) ? RS_INFO
                                                         : RS_DEBUG,
                "[%u][%s], res=%u", (user ? user->base_info_.uin_ : 0u),
                "hack",
                static_cast<unsigned int>(status));
            return -1;
        }

        // Lines 177/178.
        cf.role_type_ = data[0x0a];
        cf.role_posi_ = data[0x0c];

        // Line 180.
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], update, type=%u, posi=%u",
            user->base_info_.uin_, "hack",
            static_cast<unsigned int>(cf.role_type_),
            static_cast<unsigned int>(cf.role_posi_));
        return 0;
    }

    if (cmd_id == 0x14f00u) {
        // Line 185: expected packet length 10.
        if (len <= 0x09u) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x0a));
            return -1;
        }

        // Line 191: action selector at data[7].  0 and 0xff take the
        // start-play branch; 1..254 take the master check.
        const unsigned char action = data[7];
        if (action != 0 && action != 0xffu) {
            // Lines 206/210/214.
            if (cf.role_type_ == data[6] && cf.role_posi_ == data[0x0a]) {
                user->set_user_flag(2);
                ZEN_Trace_LogMsg::debug_output(
                    (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                    "[%u][%s], I am matser", user->base_info_.uin_,
                    "hack");
            } else {
                ZEN_Trace_LogMsg::debug_output(
                    (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                    "[%u][%s], I am not matser", user->base_info_.uin_,
                    "hack");
            }
            return 0;
        }

        // Lines 193/197/201: start play only if bit 1 or bit 2 is set.
        if (user->check_user_flag(4) || user->check_user_flag(2)) {
            user->set_user_flag(8);
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], start play", user->base_info_.uin_, "hack");
        } else {
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], not start play", user->base_info_.uin_, "hack");
        }
        return 0;
    }

    if (cmd_id == 0x14e00u) {
        // Line 222: expected packet length 0x12.
        if (len <= 0x11u) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x12));
            return -1;
        }

        // Line 223/225.
        const unsigned char revert = data[0x12];
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], join game, revert=%u", user->base_info_.uin_,
            "hack", static_cast<unsigned int>(revert));
        if (revert == 0 || revert == 5)
            user->set_user_flag(8);
        return 0;
    }

    if (cmd_id == 0x14800u) {
        // Line 234: expected packet length 6.
        if (len <= 0x05u) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", 2,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x06));
            return -1;
        }

        // Lines 236/238/243.
        if (data[6] == 0) {
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], I am not ready now", user->base_info_.uin_,
                "hack");
            user->clear_user_flag(4);
        } else {
            ZEN_Trace_LogMsg::debug_output(
                (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
                "[%u][%s], I am ready now", user->base_info_.uin_, "hack");
            user->set_user_flag(4);
        }
        return 0;
    }

    if (cmd_id == 0x15b00u) {
        // Lines 248/253.
        user->clear_user_flag(0x0c);
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], stop play", user->base_info_.uin_, "hack");
        return 0;
    }

    if (cmd_id == 0x14100u || cmd_id == 0x50b00u) {
        // Lines 256/262.
        user->clear_user_flag(0x0a);
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], exit room", user->base_info_.uin_, "hack");
        return 0;
    }

    // Line 262/267: unhandled command.
    return 0;
}

// ---------------------------------------------------------------------------
// DPCfForwardRoomMasterMsg::hack
// ORIG 0x000a4970..0x000a4a07.
//
// If user->flags_ bit 1 is clear, return 0.  Otherwise trace
// "[%u][%s], forward master msg to LA" and forward the unchanged arguments to
// DPPkgForwardLaHacker::hack() (same this pointer, since the class derives
// from it).  The trace priority is RS_INFO when flags_ bit 0 is set, else
// RS_DEBUG.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPCfForwardRoomMasterMsg::hack(
    tss_sdk_impl::UserInfo *user, int type, unsigned int cmd_id,
    unsigned char *&buf, unsigned int &len) {
    int ret = 0;
    if (user->check_user_flag(2)) {
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1u) ? RS_INFO : RS_DEBUG,
            "[%u][%s], forward master msg to LA", user->base_info_.uin_,
            "hack");
        ret = DPPkgForwardLaHacker::hack(user, type, cmd_id, buf, len);
    }
    return ret;
}

// ---------------------------------------------------------------------------
// DPCfRecordUserInfo::hack
// ORIG 0x000a4f50..0x000a515d.
//
// Gate: user->flags_ bit 3 (0x8); clear => return 0.
//
// Stop-play commands 0x15b00, 0x14100, 0x50b00 create
//   DPBillAdapter bill("cf user stop play", 0x43, user);
// and return 0.
//
// 0x14f00 (master/start packet) requires len > 6; if len <= 6 it logs the
// wrong-data error with expected_len=7 and returns -1.  Selector data[7]:
//   0 or 0xff -> fall through to the start-play bill below;
//   1..254    -> return 0 without billing.
//
// All remaining commands, plus the 0/0xff selector case, create
//   DPBillAdapter bill("cf user start play", 0x42, user);
// and stream, through CommBillRecord::operator<<(long long) on the global
// DPBillAdapter::bill_, exactly:
//   channel_id_ (+0x188), room_id_ (+0x18a, u16), role_type_ (+0x186),
//   role_posi_ (+0x185), game_mode_ (+0x184), game_map_ (+0x187), cmd_id.
// Return 0.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

// dp_bill_adapter.h.  sizeof == 4 (single user_ pointer).  ORIG inlines
// operator<< as `bill_ << (long long)value` where bill_ is the process-global
// DPBillRecorder at 0x002b6e00; the call target is
// CommBillRecord::operator<<(long long).
class DPBillAdapter {
public:
    DPBillAdapter(const char *bill_type, unsigned int line,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();

    template <typename T>
    DPBillAdapter &operator<<(const T &value) {
        bill_ << static_cast<long long>(value);
        return *this;
    }

    static DPBillRecorder bill_;

private:
    tss_sdk_impl::UserInfo *user_;      // +0x00
};

}  // namespace tss_dpsdk

int tss_dpsdk::DPCfRecordUserInfo::hack(tss_sdk_impl::UserInfo *user,
                                        int type, unsigned int cmd_id,
                                        unsigned char *&buf,
                                        unsigned int &len) {
    const unsigned char *data = buf;

    // Line 17: cf recording is enabled only while flag 0x8 is set.
    if (!user->check_user_flag(8))
        return 0;

    // Line 22: stop-play triggers.
    if (cmd_id == 0x15b00u || cmd_id == 0x14100u || cmd_id == 0x50b00u) {
        // Line 26.
        DPBillAdapter bill("cf user stop play", 0x43u, user);
        return 0;
    }

    // Line 29: master/start packet.
    if (cmd_id == 0x14f00u) {
        // Line 33: expected length 7.
        if (len <= 0x06u) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], game data is wrong, type=%d, cmd=%u, len=%u, "
                "expected_len=%u",
                (user ? user->base_info_.uin_ : 0u), "hack", type,
                cmd_id,
                static_cast<unsigned int>(len),
                static_cast<unsigned int>(0x07));
            return -1;
        }

        // Line 35: only selector values 0 and 0xff reach the bill below.
        const unsigned char action = data[7];
        if (action != 0 && action != 0xffu)
            return 0;
    }

    // Lines 41-49: start-play bill and field record.
    {
        DPBillAdapter bill("cf user start play", 0x42u, user);
        tss_sdk_impl::DPHackCfUserInfo &cf =
            user->hack_data_.cf_user_info_;
        bill << cf.channel_id_          // UserInfo +0x188
             << cf.room_id_             // UserInfo +0x18a
             << cf.role_type_           // UserInfo +0x186
             << cf.role_posi_           // UserInfo +0x185
             << cf.game_mode_           // UserInfo +0x184
             << cf.game_map_            // UserInfo +0x187
             << cmd_id;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Static storage and out-of-line destructors.
// ---------------------------------------------------------------------------

// Defined by dp_bill_monitor_agent.cpp; use the single process-global.

// D1 (complete-object) and D0 (deleting) are emitted weak by the compiler,
// matching _ZN9tss_dpsdk14DPBillRecorderD1Ev/D0Ev.

// ---------------------------------------------------------------------------
// Layout guards for the ABI models.
// ---------------------------------------------------------------------------
typedef char comm_bill_record_size_must_be_5984[
    sizeof(CommBillRecord) == 5984 ? 1 : -1];
typedef char dp_bill_recorder_size_must_be_5988[
    sizeof(tss_dpsdk::DPBillRecorder) == 5988 ? 1 : -1];
typedef char dp_bill_adapter_size_must_be_4[
    sizeof(tss_dpsdk::DPBillAdapter) == 4 ? 1 : -1];
typedef char user_info_size_must_be_400[
    sizeof(tss_sdk_impl::UserInfo) == 400 ? 1 : -1];
typedef char hack_cf_user_info_size_must_be_8[
    sizeof(tss_sdk_impl::DPHackCfUserInfo) == 8 ? 1 : -1];
typedef char hack_cf_user_info_offset_must_be_0x184[
    offsetof(tss_sdk_impl::UserInfo, hack_data_) +
            offsetof(tss_sdk_impl::UserInfoHackData, cf_user_info_) ==
            0x184
        ? 1
        : -1];
typedef char log_svr_bill_record_size_must_be_2048[
    sizeof(sec_proto::LogSvrCommBillRecord) == 2048 ? 1 : -1];
typedef char services_id_size_must_be_6[
    sizeof(SERVICES_ID) == 6 ? 1 : -1];
typedef char comm_app_frame_size_must_be_50[
    sizeof(Comm_App_Frame) == 50 ? 1 : -1];
typedef char dp_config_head_size_must_be_280[
    sizeof(tss_dp_comm::DP_CONFIG) == 280 ? 1 : -1];
typedef char hacker_size_must_be_4[
    sizeof(tss_dpsdk::DPCfExtractUserInfo) == 4 &&
            sizeof(tss_dpsdk::DPCfForwardRoomMasterMsg) == 4 &&
            sizeof(tss_dpsdk::DPCfRecordUserInfo) == 4
        ? 1
        : -1];
