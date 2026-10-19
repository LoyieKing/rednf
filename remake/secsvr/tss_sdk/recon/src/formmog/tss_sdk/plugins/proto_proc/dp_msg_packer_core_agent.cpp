// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG symbols and ranges (half-open, sizes from nm -S):
//   tss_dpsdk::DPMsgPacker::send_msg_to_client(int, UserInfo*, unsigned char*, unsigned short)
//       @0x000b1920..0x000b1a9d
//   tss_dpsdk::DPMsgPacker::revive_client_mouse(UserInfo*, unsigned int, E_TIPS_WINDOWS)
//       @0x000b1aa0..0x000b1b83
//   tss_dpsdk::DPMsgPacker::send_captcha_finish(UserInfo*)
//       @0x000b1b90..0x000b1d06
//   tss_dpsdk::DPMsgPacker::send_special_control(UserInfo*, int)
//       @0x000b1d10..0x000b1dfd
//   tss_dpsdk::DPMsgPacker::send_safe_mode_notify(UserInfo*, int)
//       @0x000b1e00..0x000b1edd
//   tss_dpsdk::DPMsgPacker::send_dp_msg_to_client(UserInfo*, unsigned char*, unsigned short)
//       @0x000b1ee0..0x000b20ed
//   tss_dpsdk::DPMsgPacker::send_captcha_request(UserInfo*, char const*)
//       @0x000b2500..0x000b2911
//   tss_dpsdk::DPMsgPacker::send_error_report_msg(UserInfo*, int, int)
//       @0x000b2920..0x000b2d0f
//   tss_dpsdk::DPMsgPacker::send_cmd_monitor(UserInfo*, unsigned int, bool, bool)
//       @0x000b2d10..0x000b33bc
//
// This is a semantic reconstruction, not a byte-identical one.  Templates whose
// bodies were inlined into the three *_pkg users (send_ipc_pkg<>,
// send_datatrans_pkg<>) are left as declarations only, as requested; the ORIG
// instantiation addresses are recorded in AGENT_REPORT_packer_core.md.
// The sec_proto types below are partial but byte-exact models of the DWARF
// layouts (sizes and offsets asserted at the bottom of this file).

#include <string.h>
#include <time.h>

#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
#include "src/origsrc/formmog_proto_captcha_client_old.h"
// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h).  The local enum/struct stubs this TU used to carry
// are gone; every call site below is unchanged.
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ---------------------------------------------------------------------------
// Logging / monitoring entry points.
// ---------------------------------------------------------------------------


namespace tss_dpsdk {
class DPMonitorReporter {
public:
    static int report_data(unsigned int type, unsigned int value);
};
}

// ---------------------------------------------------------------------------
// tss_sdk_impl::TssSdkCommSvc.  DWARF size 40:
//   channel_id_ +0, plugin_mgr_ +4, dp_if_ +8,
//   sdk_channel_status_ +12 (24 bytes), error_info_ +36.
// Only channel_id_ is read by this TU; the rest preserves the exact size so
// that the function-local singleton object is allocated correctly.
// ---------------------------------------------------------------------------

// zen_singleton.h: canonical declaration shared by every reconstructed TU.
// The out-of-line weak instance() is owned by tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"

// ---------------------------------------------------------------------------
// tss_dp_comm configuration objects read by this TU.
// ---------------------------------------------------------------------------
namespace tss_dp_comm {

// DPProtoConfig, DWARF size 10, offset inside DPConfig +6.
struct DPProtoConfig {
    unsigned short dp_fragmentation_size;  // +0
    unsigned short dp_max_encrypt_size;    // +2
    unsigned char use_sequence_check;      // +4
    unsigned char proto_check_switch;      // +5
    unsigned char seq_check_switch;        // +6
    unsigned char raw_check_switch;        // +7
    unsigned char dp_decrypt_fail_protect; // +8
    unsigned char explicit_use_dynamic_encrypt;  // +9
} __attribute__((packed));

// DPConfig, DWARF size 22842.  Only the dp_proto_conf prefix is modelled;
// the trailing pad preserves sizeof(DPConfig).
struct DPConfig {
    unsigned char dp_base_conf_[6];        // +0
    DPProtoConfig dp_proto_conf;           // +6
    unsigned char rest_[22842 - 16];       // +16
} __attribute__((packed));

// DP_CONFIG, DWARF size 280 (glb_dp_config_head_).
struct DPConfigHead {
    unsigned int update_time;              // +0
    unsigned int revert;                   // +4
    unsigned int game_id;                  // +8
    unsigned int world_id;                 // +12
    unsigned int secagent_id;              // +16
    char dp_config_path[256];              // +20
    unsigned int now_time;                 // +276
};

struct DP_CONFIG {
    static DPConfig* glb_dp_config_;       // 0x002bb4c0
    static DPConfigHead* glb_dp_config_head_;  // 0x002bb4c4
};

}  // namespace tss_dp_comm

// ---------------------------------------------------------------------------
// sec_proto wire structures.  Every struct below is byte-packed exactly as the
// DWARF layout: the protocol encoders access members at non-natural offsets.
// ---------------------------------------------------------------------------
namespace sec_proto {

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;

// ---- ClientPkg family (commands 0x135/0x136/0x137, protocol id 0x33) ------

struct ClientPkgHead {
    uint16_t protocol_id_;   // +0
    uint32_t seq_;           // +2
    uint16_t sln_id_;        // +6
    uint16_t sub_sln_id_;    // +8
    uint16_t body_len_;      // +10

    int pack(char* buf, unsigned int cut_size, unsigned int* used,
             unsigned int protocol_version) const;
} __attribute__((packed));

struct TReviveMouse {
    uint32_t cmdID;          // +0
    uint32_t isHint;         // +4
};

struct TSpecialControl {
    uint16_t shPacketSize;   // +0
    uint16_t shPacketType;   // +2
    uint8_t chDataType;      // +4
    uint32_t cmdid;          // +5
    uint32_t bufLen;         // +9
} __attribute__((packed));

struct TSafeMode {
    uint32_t iSafemodeLevel;  // +0
};

struct TClientPkgBody {
    union {
        TReviveMouse reviseMouse;
        TSpecialControl specialControl;
        TSafeMode safeMode;
    } __attribute__((packed));
} __attribute__((packed));

struct TClientPkg {
    ClientPkgHead head;      // +0
    TClientPkgBody body;     // +12

    int pack(char* buf, unsigned int cut_size, unsigned int* used,
             unsigned int protocol_version) const;
} __attribute__((packed));

// ---- CAPTCHA old package (msg_id 0xc14, CaptchaBusiFinish body) -----------
// TSSCaptchaOldHead / CaptchaBusiFinish come from the canonical
// src/origsrc/formmog_proto_captcha_client_old.h.

struct LATransAdaptDPCmdMonitor {
    uint32_t room_id_;         // +0
    uint32_t time_stamp_;      // +4
    int32_t monitor_cmd_id_;   // +8
    uint8_t is_need_encrypt_;  // +12
    uint8_t is_encrypted_;     // +13

    int pack(char* buf, unsigned int cut_size, unsigned int* used,
             unsigned int protocol_version) const;
} __attribute__((packed));

// secsvr captcha request (ORIG 541 B, packed).
struct SecsvrCaptchaReq {
    uint16_t channel_id_;
    uint16_t room_id_;
    uint32_t version_;
    uint16_t bus_id_;
    uint32_t time_stamp_;
    uint32_t client_ip_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
    uint32_t timeout_;
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    uint8_t busi_info_[512];
} __attribute__((packed));

struct ReportErrReq {
    unsigned short channel_id_;  // +0
    unsigned short room_id_;     // +2
    unsigned int error_id_;      // +4
} __attribute__((packed));

// ---- fragmented client frame (protoId 7) ---------------------------------

struct TClientFrameHead {
    uint32_t packetLen;        // +0
    uint16_t totalSize;        // +4
    uint16_t protoId;          // +6
};

struct TClientFramePiece {
    uint16_t totalPiece;       // +0
    uint16_t pieceSerial;      // +2
    uint32_t totalCrc;         // +4
    uint16_t pieceDataSize;    // +8
    uint8_t pieceData[51200];  // +10
} __attribute__((packed));

struct TClientFrameBody {
    union {
        TClientFramePiece framePiece;
    };
} __attribute__((packed));

struct TClientFrame {
    TClientFrameHead head;     // +0
    TClientFrameBody body;     // +8

    int pack(char* buf, unsigned int cut_size, unsigned int* used,
             unsigned int protocol_version) const;
} __attribute__((packed));

}  // namespace sec_proto

// CRC-32 over the complete unfragmented DP payload.
unsigned int Crc32(unsigned char* data, unsigned int len);

// ---------------------------------------------------------------------------
// DPMsgPacker.  DWARF size 1, all methods are static (ORIG call sites pass no
// object pointer).  Only the methods reconstructed in this TU are declared;
// send_antibot_msg_to_client/do_send_dp_msg_to_client belong to other slices.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

class DPMsgPacker {
public:
    enum E_TIPS_WINDOWS {
        E_TIPS_NONE = 0,
        E_TIPS_SAFEMODE = 1,
        E_TIPS_SEND_SUCCESS = 2
    };

    static int send_msg_to_client(int type, tss_sdk_impl::UserInfo* user,
                                  unsigned char* data, unsigned short len);
    static int send_dp_msg_to_client(tss_sdk_impl::UserInfo* user,
                                     unsigned char* data, unsigned short len);
    static int send_error_report_msg(tss_sdk_impl::UserInfo* user, int err_id,
                                     int err_num);
    static int send_cmd_monitor(tss_sdk_impl::UserInfo* user, unsigned int cmd,
                                bool is_need_encrypt, bool is_encrypted);
    static int revive_client_mouse(tss_sdk_impl::UserInfo* user, unsigned int id,
                                   E_TIPS_WINDOWS tips);
    static int send_safe_mode_notify(tss_sdk_impl::UserInfo* user, int level);
    static int send_special_control(tss_sdk_impl::UserInfo* user, int cmdid);
    static int send_captcha_request(tss_sdk_impl::UserInfo* user,
                                    const char* notify_data);
    static int send_captcha_finish(tss_sdk_impl::UserInfo* user);

    // ORIG instantiations used by the methods above.  Declarations only: the
    // real bodies are the library's weak template instantiations
    // send_ipc_pkg<...> @0x000b3e50/0x000b4180/0x000b44b0 and
    // send_datatrans_pkg<LATransAdaptDPCmdMonitor> @0x000b47e0.
    template <class PKG>
    static int send_ipc_pkg(tss_sdk_impl::UserInfo* user, const PKG& pkg,
                            unsigned int cmd);
    template <class PKG>
    static int send_datatrans_pkg(tss_sdk_impl::UserInfo* user, int entry,
                                  const PKG& pkg, unsigned int cmd);
};

// ---------------------------------------------------------------------------
// Implementations.
// ---------------------------------------------------------------------------
namespace {

// ORIG selects RS_DEBUG (2) for normal users and RS_INFO (3) when the low bit
// of base_info_.flags_ is set; uin is read only when user != NULL.
inline int user_log_priority(const tss_sdk_impl::UserInfo* user) {
    if (user != NULL && (user->base_info_.flags_ & 1u) == 1u) {
        return RS_INFO;
    }
    return RS_DEBUG;
}

inline unsigned int user_log_uin(const tss_sdk_impl::UserInfo* user) {
    return user != NULL ? user->base_info_.uin_ : 0u;
}

}  // namespace

int DPMsgPacker::send_msg_to_client(int type, tss_sdk_impl::UserInfo* user,
                                    unsigned char* data, unsigned short len) {
    const unsigned int uin = user_log_uin(user);
    const int priority = user_log_priority(user);

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)priority, "[%u][%s], type=%d, len=%d", uin, __func__,
        type, (int)len);

    // ORIG dereferences user unconditionally from here on.
    unsigned int send_id = user->base_info_.game_usr_id_;  // +0x14
    if (send_id == 0xffffffffu) {
        send_id = user->base_info_.uin_;                   // +0x00
    }

    const int ret = Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()
                        ->send_buf_to_client(type, send_id, data,
                                             (short)len);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], ret=%d, type=%d, len=%u",
            user->base_info_.uin_, __func__, ret, type, (unsigned int)len);
        DPMonitorReporter::report_data(0x3fb, 1);
    }
    return ret;
}

int DPMsgPacker::revive_client_mouse(tss_sdk_impl::UserInfo* user,
                                     unsigned int id, E_TIPS_WINDOWS tips) {
    unsigned char buf[0x10000];
    unsigned int len;

    sec_proto::TClientPkg pkg;  // ORIG leaves head.seq_ uninitialised
    pkg.head.protocol_id_ = 0x33;
    pkg.head.sln_id_ = 4;
    pkg.head.sub_sln_id_ = 0x135;
    pkg.body.reviseMouse.cmdID = id;
    pkg.body.reviseMouse.isHint = (unsigned int)tips;

    pkg.pack((char*)buf, 0xffff, &len, 0);

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], revive client mouse, id=%u, tips=%d", user_log_uin(user),
        __func__, id, (int)tips);

    return send_msg_to_client(0, user, buf, (unsigned short)len);
}

int DPMsgPacker::send_special_control(tss_sdk_impl::UserInfo* user, int cmdid) {
    unsigned char buf[0x10000];
    unsigned int len;

    sec_proto::TClientPkg pkg;  // ORIG leaves head.seq_ uninitialised
    pkg.head.protocol_id_ = 0x33;
    pkg.head.sln_id_ = 4;
    pkg.head.sub_sln_id_ = 0x136;
    pkg.body.specialControl.shPacketSize = 0;
    pkg.body.specialControl.shPacketType = 0x1a;
    pkg.body.specialControl.chDataType = 0;
    pkg.body.specialControl.cmdid = (unsigned int)cmdid;
    pkg.body.specialControl.bufLen = 0;

    pkg.pack((char*)buf, 0xffff, &len, 0);

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], special control, cmd_id=%u", user_log_uin(user), __func__,
        (unsigned int)cmdid);

    return send_msg_to_client(0, user, buf, (unsigned short)len);
}

int DPMsgPacker::send_safe_mode_notify(tss_sdk_impl::UserInfo* user, int level) {
    unsigned char buf[0x10000];
    unsigned int len = 0;

    sec_proto::TClientPkg pkg;  // ORIG leaves head.seq_ uninitialised
    pkg.head.protocol_id_ = 0x33;
    pkg.head.sln_id_ = 4;
    pkg.head.sub_sln_id_ = 0x137;
    pkg.body.safeMode.iSafemodeLevel = (unsigned int)level;

    pkg.pack((char*)buf, 0xffff, &len, 0);

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], safe mode notify, level=%d", user_log_uin(user), __func__,
        level);

    return send_msg_to_client(0, user, buf, (unsigned short)len);
}

int DPMsgPacker::send_captcha_finish(tss_sdk_impl::UserInfo* user) {
    unsigned char buf[0xffff];
    sec_proto::TSSCaptchaOldPkg pkg;
    unsigned int body_len = 0;

    memset(&pkg, 0, sizeof(pkg));  // ORIG memset size 0x12f3 = sizeof(pkg)

    pkg.head_.msg_id_ = 0xc14;
    pkg.head_.version_ = 0;
    pkg.head_.uin_ = user->base_info_.uin_;
    pkg.head_.send_time_ = (sec_proto::uint32_t)time(NULL);
    pkg.head_.head_len_ = 0x18;
    pkg.head_.body_len_ = 0x0f;  // rewritten by TSSCaptchaOldPkg::pack

    pkg.body_.captcha_busi_finish_.app_id_ = 0x6ad219;
    pkg.body_.captcha_busi_finish_.busi_id_ = 1;

    // CAPTCHA package starts at buf + 12 (after the 12-byte ClientPkgHead).
    pkg.pack((char*)buf + 12, 0xfff3, &body_len, 0);

    sec_proto::ClientPkgHead head;  // ORIG leaves head.seq_ uninitialised
    head.protocol_id_ = 0x33;
    head.sln_id_ = 0x0c;
    head.sub_sln_id_ = 0x271c;
    head.body_len_ = (sec_proto::uint16_t)body_len;

    unsigned int head_len = 0;
    head.pack((char*)buf, 0xffff, &head_len, 0);

    const unsigned int total_len = (unsigned int)head.body_len_ + head_len;

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], captcha finish, len=%u", user_log_uin(user), __func__,
        total_len);

    return send_msg_to_client(0, user, buf, (unsigned short)total_len);
}

int DPMsgPacker::send_dp_msg_to_client(tss_sdk_impl::UserInfo* user,
                                       unsigned char* data,
                                       unsigned short len) {
    const unsigned short frag_size =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_proto_conf
            .dp_fragmentation_size;

    if (frag_size == 0 || frag_size >= len) {
        const int ret = send_msg_to_client(1, user, data, len);
        if (ret == 0) {
            ++user->user_stat_.internal_msg_send_count_;  // +0xa8
        }
        return ret;
    }

    unsigned short total_pieces =
        (unsigned short)(len / frag_size);
    if ((len % frag_size) != 0) {
        ++total_pieces;
    }

    const unsigned int total_crc = Crc32(data, len);
    if (len == 0) {  // unreachable after the frag_size < len check; kept
        return 0;    // because the ORIG contains it
    }

    sec_proto::TClientFrame frame;
    frame.head.totalSize = len;
    frame.head.protoId = 7;
    frame.body.framePiece.totalPiece = total_pieces;
    frame.body.framePiece.pieceSerial = 0;
    frame.body.framePiece.totalCrc = total_crc;

    unsigned char buf[0xffff];
    unsigned int packed_len = 0;
    unsigned short remaining = len;
    unsigned char* src = data;

    do {
        unsigned short chunk = frag_size;
        if (chunk > remaining) {
            chunk = remaining;
        }
        frame.body.framePiece.pieceDataSize = chunk;
        frame.head.packetLen = (unsigned int)chunk + 18u;
        memcpy(frame.body.framePiece.pieceData, src, chunk);

        frame.pack((char*)buf, 0xffff, &packed_len, 0);

        // ORIG quirk: the packed frame in `buf` is never sent.  The call site
        // (0x000b2078..0x000b2091) advances `data`/`remaining` first and then
        // passes the advanced raw input pointer and the remaining count to
        // send_msg_to_client(1, ...).  This drops the first chunk and repeats
        // the tail; reproduced verbatim.
        remaining = (unsigned short)(remaining - chunk);
        src += chunk;

        const int ret = send_msg_to_client(1, user, src, remaining);
        if (ret != 0) {
            return ret;
        }
        ++user->user_stat_.internal_msg_send_count_;  // +0xa8

        if (remaining != 0) {
            ++frame.body.framePiece.pieceSerial;
        }
    } while (remaining != 0);

    return 0;
}

int DPMsgPacker::send_captcha_request(tss_sdk_impl::UserInfo* user,
                                      const char* notify_data) {
    sec_proto::SecsvrCaptchaReq req;  // ORIG leaves channel_id_ uninitialised

    req.version_ = 0;
    req.room_id_ = (sec_proto::uint16_t)
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;  // +12
    req.bus_id_ = 0;
    req.time_stamp_ = (sec_proto::uint32_t)time(NULL);
    req.client_ip_ = 0;
    req.max_pic_change_num_ = 3;
    req.max_verify_num_ = 1;
    req.timeout_ = 0x78;
    req.busi_id_ = 1;

    // ORIG truncates strlen to 16 bits before applying the 0x200 limit.
    unsigned short info_len = (unsigned short)strlen(notify_data);
    if (info_len > 0x200) {
        info_len = 0x200;
    }
    req.busi_info_len_ = info_len;
    memcpy(req.busi_info_, notify_data, info_len);

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], captcha request, notify=%s", user_log_uin(user), __func__,
        notify_data);

    return send_ipc_pkg<sec_proto::SecsvrCaptchaReq>(user, req, 0x4bed9);
}

int DPMsgPacker::send_error_report_msg(tss_sdk_impl::UserInfo* user,
                                       int err_id, int err_num) {
    sec_proto::ReportErrReq req;
    req.channel_id_ = (sec_proto::uint16_t)
        Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()->channel_id_;
    req.room_id_ = (sec_proto::uint16_t)user->base_info_.room_id_;
    req.error_id_ = (sec_proto::uint32_t)err_id;

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], err_id=%d, err_num=%d", user_log_uin(user), __func__,
        err_id, err_num);

    return send_ipc_pkg<sec_proto::ReportErrReq>(user, req, 0x4baf7);
}

int DPMsgPacker::send_cmd_monitor(tss_sdk_impl::UserInfo* user,
                                  unsigned int cmd, bool is_need_encrypt,
                                  bool is_encrypted) {
    sec_proto::LATransAdaptDPCmdMonitor monitor;
    monitor.room_id_ = user->base_info_.room_id_;
    monitor.time_stamp_ = (sec_proto::uint32_t)time(NULL);
    monitor.monitor_cmd_id_ = (sec_proto::int32_t)cmd;
    monitor.is_need_encrypt_ = is_need_encrypt ? 1 : 0;
    monitor.is_encrypted_ = is_encrypted ? 1 : 0;

    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)user_log_priority(user),
        "[%u][%s], cmd monitor, cmd=%u, is_need_encrypt=%u, is_encrypted=%u",
        user_log_uin(user), __func__, cmd, (unsigned int)is_need_encrypt,
        (unsigned int)is_encrypted);

    return send_datatrans_pkg<sec_proto::LATransAdaptDPCmdMonitor>(
        user, 0x23, monitor, 0x5573d);
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Compile-time layout checks against the DWARF sizes/offsets.
// ---------------------------------------------------------------------------
typedef char Check_ClientPkgHead_size[sizeof(sec_proto::ClientPkgHead) == 12 ? 1 : -1];
typedef char Check_TReviveMouse_size[sizeof(sec_proto::TReviveMouse) == 8 ? 1 : -1];
typedef char Check_TSpecialControl_size[sizeof(sec_proto::TSpecialControl) == 13 ? 1 : -1];
typedef char Check_TSafeMode_size[sizeof(sec_proto::TSafeMode) == 4 ? 1 : -1];
typedef char Check_TClientPkgBody_size[sizeof(sec_proto::TClientPkgBody) == 13 ? 1 : -1];
typedef char Check_TClientPkg_size[sizeof(sec_proto::TClientPkg) == 25 ? 1 : -1];
typedef char Check_TSSCaptchaOldHead_size[sizeof(sec_proto::TSSCaptchaOldHead) == 24 ? 1 : -1];
typedef char Check_CaptchaBusiFinish_size[sizeof(sec_proto::CaptchaBusiFinish) == 527 ? 1 : -1];
typedef char Check_TSSCaptchaOldBody_size[sizeof(sec_proto::TSSCaptchaOldBody) == 4827 ? 1 : -1];
typedef char Check_TSSCaptchaOldPkg_size[sizeof(sec_proto::TSSCaptchaOldPkg) == 4851 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_size[sizeof(sec_proto::SecsvrCaptchaReq) == 541 ? 1 : -1];
typedef char Check_ReportErrReq_size[sizeof(sec_proto::ReportErrReq) == 8 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdMonitor_size[
    sizeof(sec_proto::LATransAdaptDPCmdMonitor) == 14 ? 1 : -1];
typedef char Check_TClientFrameHead_size[sizeof(sec_proto::TClientFrameHead) == 8 ? 1 : -1];
typedef char Check_TClientFramePiece_size[sizeof(sec_proto::TClientFramePiece) == 51210 ? 1 : -1];
typedef char Check_TClientFrame_size[sizeof(sec_proto::TClientFrame) == 51218 ? 1 : -1];
typedef char Check_TssSdkCommSvc_size[sizeof(tss_sdk_impl::TssSdkCommSvc) == 40 ? 1 : -1];
typedef char Check_DPConfigHead_size[sizeof(tss_dp_comm::DPConfigHead) == 280 ? 1 : -1];

typedef char Check_ClientPkgHead_sln[
    __builtin_offsetof(sec_proto::ClientPkgHead, sln_id_) == 6 ? 1 : -1];
typedef char Check_ClientPkgHead_sub_sln[
    __builtin_offsetof(sec_proto::ClientPkgHead, sub_sln_id_) == 8 ? 1 : -1];
typedef char Check_ClientPkgHead_body_len[
    __builtin_offsetof(sec_proto::ClientPkgHead, body_len_) == 10 ? 1 : -1];
typedef char Check_TClientPkg_body[
    __builtin_offsetof(sec_proto::TClientPkg, body) == 12 ? 1 : -1];
typedef char Check_TSSCaptchaOldPkg_body[
    __builtin_offsetof(sec_proto::TSSCaptchaOldPkg, body_) == 24 ? 1 : -1];
typedef char Check_CaptchaBusiFinish_busi_id[
    __builtin_offsetof(sec_proto::CaptchaBusiFinish, busi_id_) == 12 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_id[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_id_) == 26 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_info_len[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_info_len_) == 27 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_info[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_info_) == 29 ? 1 : -1];
typedef char Check_TClientFrame_body[
    __builtin_offsetof(sec_proto::TClientFrame, body) == 8 ? 1 : -1];
typedef char Check_TClientFramePiece_data[
    __builtin_offsetof(sec_proto::TClientFramePiece, pieceData) == 10 ? 1 : -1];
typedef char Check_DPConfig_proto[
    __builtin_offsetof(tss_dp_comm::DPConfig, dp_proto_conf) == 6 ? 1 : -1];
typedef char Check_DPProtoConfig_frag[
    __builtin_offsetof(tss_dp_comm::DPProtoConfig, dp_fragmentation_size) == 0 ? 1 : -1];
typedef char Check_DPConfigHead_game_id[
    __builtin_offsetof(tss_dp_comm::DPConfigHead, game_id) == 8 ? 1 : -1];
typedef char Check_DPConfigHead_world_id[
    __builtin_offsetof(tss_dp_comm::DPConfigHead, world_id) == 12 ? 1 : -1];
// tss_sdk_impl::TssSdkCommSvc::channel_id_ is the first member (offset 0); a
// byteoffset check is avoided because the class has a user-declared ctor/dtor.
typedef char Check_UserInfo_uin[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.uin_) == 0 ? 1 : -1];
typedef char Check_UserInfo_room_id[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.room_id_) == 8 ? 1 : -1];
typedef char Check_UserInfo_game_usr_id[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.game_usr_id_) == 0x14 ? 1 : -1];
typedef char Check_UserInfo_flags[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.flags_) == 0x20 ? 1 : -1];
typedef char Check_UserInfo_internal_msg_send[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_stat_.internal_msg_send_count_) == 0xa8 ? 1 : -1];

// ---------------------------------------------------------------------------
// ORIG CU dp_msg_packer.cpp static-initialization block.
//
// This slice is the single owner of the block for the ORIG CU
// dp_msg_packer.cpp (dp_msg_packer_core_agent.cpp and
// dp_msg_packer_templates_agent.cpp both cover parts of it).
//
// Decoded ORIG order (audit/report/init_brief.md, 26 calls, target size 996):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() /
//     __cxa_guard_release + atexit; guard_abort + _Unwind_Resume pad
// The CU has no other module-specific object.
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG .bss 0x0029d800 (weak): Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
