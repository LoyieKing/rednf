// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG symbols/ranges implemented (half-open, addresses/sizes from nm -S):
//   DPMsgPacker::generate_comm_frame<LATransAdaptDPCmdMonitor>
//       @0x000b3710..0x000b38c2  (0x1b2)
//   DPMsgPacker::generate_comm_frame<LATransAdaptDPCmdBufferMonitor>
//       @0x000b7ed0..0x000b8082  (0x1b2)
//   DPMsgPacker::generate_comm_frame<ReqSafemodeTransAdaptTradeNotify>
//       @0x000ba290..0x000ba442  (0x1b2)
//   DPMsgPacker::generate_comm_frame<ReqUICTransAdaptMsgForwardPkg>
//       @0x000bc620..0x000bc7d2  (0x1b2)
//   DPMsgPacker::send_ipc_pkg<SecsvrCaptchaReq>
//       @0x000b3e50..0x000b4171  (0x321)
//   DPMsgPacker::send_ipc_pkg<ReportErrReq>
//       @0x000b4180..0x000b44a1  (0x321)
//   DPMsgPacker::send_ipc_pkg<ReqSecsvrForwardData>
//       @0x000b44b0..0x000b47d1  (0x321)
//   DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdMonitor>
//       @0x000b47e0..0x000b4e02  (0x622)
//   DPMsgPacker::send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor>
//       @0x000b80d0..0x000b86f2  (0x622)
//   DPMsgPacker::send_datatrans_pkg<ReqSafemodeTransAdaptTradeNotify>
//       @0x000bac20..0x000bb242  (0x622)
//   DPMsgPacker::send_datatrans_pkg<ReqUICTransAdaptMsgForwardPkg>
//       @0x000bc820..0x000bce42  (0x622)
//
// This is a semantic reconstruction, not a byte-identical one.
//
// Source-level shape established from DWARF abstract instances:
//   - generate_comm_frame(rsp_msg, cmd, entire, user, forward_pkg) builds and
//     encodes one Comm_App_Frame.
//   - send_ipc_pkg(user, pkg, cmd) wraps the inlined
//     Singleton<TssSdkCommSvc>::instance()->send_ipc_pkg(pkg, user->uin, cmd,
//     0x1f) call and logs success/failure.  The ORIG inlined the callee; this
//     reconstruction performs the same source-level call to the library's
//     out-of-line weak instantiation (same arguments and call target).
//   - send_datatrans_pkg(user, entire, forward_pkg, cmd) calls
//     generate_comm_frame, wraps the resulting frame into a
//     ReqSecsvrForwardData whose channel_id_ comes from the TssSdkCommSvc
//     singleton and whose room_id_ is glb_dp_config_head_->world_id, then calls
//     send_ipc_pkg<ReqSecsvrForwardData>(..., 0x4c6ad).
//
// ORIG log strings (addresses in .rodata):
//   0x21a4dc  "[TRACK MONITOR][TRANS SEND]"
//   0x21c45c  "[%u][%s], app data encode fail. ret=%d"
//   0x21c484  "[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u"
//   0x21c4b0  "[%u][%s], send_ipc_pkg success, cmd=%u"
//   0x21c5a0  "[%u][%s], generate_comm_frame fail, ret=%d, entire=%d, cmd=%u"
//   __func__ values: "generate_comm_frame" (0x21c7b6), "send_ipc_pkg"
//   (0x21c7ca/0x21caac), "send_datatrans_pkg" (0x21c7a3/0x21ca85).

#include <string.h>

// Canonical tag-394 framework declarations: Comm_App_Frame (50 bytes), the
// packed SERVICES_ID, the ZEN log boundary and the tsf4g_tdr enums.  The
// local frame/SERVICES_ID/logging models are gone; this TU keeps only the
// out-of-line appdata_encode<> references, declared as explicit
// specializations below (the ORIG calls the weak instantiations emitted by
// the packet-defining CUs, not an inlined body).
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
// tss_dp_comm::DP_CONFIG::glb_dp_config_head_ (DWARF size 280): only game_id
// +8 and world_id +12 are read here.
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// zen_singleton.h: canonical declaration shared by every reconstructed TU.
// The out-of-line weak instance() is owned by tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"



// ---------------------------------------------------------------------------
// sec_proto wire structures.  Sizes/offsets from DWARF; all packed because
// the protocol encoders use the exact byte layout.
// ---------------------------------------------------------------------------
namespace sec_proto {

struct ReportErrReq {
    unsigned short channel_id_;  // +0
    unsigned short room_id_;     // +2
    unsigned int error_id_;      // +4
} __attribute__((packed));

struct SecsvrCaptchaReq {
    unsigned short channel_id_;         // +0
    unsigned short room_id_;            // +2
    unsigned int version_;              // +4
    unsigned short bus_id_;             // +8
    unsigned int time_stamp_;           // +10
    unsigned int client_ip_;            // +14
    unsigned short max_pic_change_num_; // +18
    unsigned short max_verify_num_;     // +20
    unsigned int timeout_;              // +22
    unsigned char busi_id_;             // +26
    unsigned short busi_info_len_;      // +27
    unsigned char busi_info_[512];      // +29
} __attribute__((packed));

struct ReqSecsvrForwardData {
    unsigned short channel_id_;   // +0
    unsigned short room_id_;      // +2
    unsigned short data_len_;     // +4
    unsigned char data_[32767];   // +6
} __attribute__((packed));

struct LATransAdaptDPCmdMonitor {
    unsigned int room_id_;        // +0
    unsigned int time_stamp_;     // +4
    int monitor_cmd_id_;          // +8
    unsigned char is_need_encrypt_;  // +12
    unsigned char is_encrypted_;     // +13
} __attribute__((packed));

struct LATransAdaptDPCmdBufferMonitor {
    unsigned int room_id_;        // +0
    unsigned int time_stamp_;     // +4
    int monitor_cmd_id_;          // +8
    unsigned char is_need_encrypt_;  // +12
    unsigned char is_encrypted_;     // +13
    unsigned int data_len_;          // +14 (u32, unaligned)
    unsigned char data_[32768];      // +18
} __attribute__((packed));

struct ReqSafemodeTransAdaptTradeNotify {
    unsigned short world_id_;     // +0
    unsigned int src_id_;         // +2
    unsigned char type_;          // +6
    unsigned int send_uin_;       // +7
    unsigned int target_uin_;     // +11
    unsigned int send_ip_;        // +15
    unsigned int target_ip_;      // +19
    unsigned short buf_len_;      // +23
    unsigned char trade_buf_[2048];  // +25
} __attribute__((packed));

struct ReqUICTransAdaptMsgForwardPkg {
    unsigned short world_id_;     // +0
    unsigned int src_id_;         // +2
    unsigned int cmd_id_;         // +6
    unsigned char need_encrypt_;  // +10
    unsigned char is_encrypted_;  // +11
    unsigned short cmd_buf_len_;  // +12
    unsigned char cmd_buf_[32768];   // +14
    unsigned short rolename_len_;    // +32782
    unsigned char rolename_[256];    // +32784
    unsigned int client_ip_;         // +33040
} __attribute__((packed));

}  // namespace sec_proto
// Comm_App_Frame::appdata_encode<T> for the four packets of this TU.  ORIG
// calls the out-of-line weak instantiations emitted by the CUs that define
// the packet bodies (nm: 0x000b3680 LATransAdaptDPCmdMonitor, 0x000b7e40
// LATransAdaptDPCmdBufferMonitor, 0x000ba200 ReqSafemodeTransAdaptTradeNotify,
// 0x000bc590 ReqUICTransAdaptMsgForwardPkg); these explicit specializations
// keep every call an external reference instead of an inlined body.  They are
// emitted by src/emission/comm_app_frame_instances.cpp.
template <>
int Comm_App_Frame::appdata_encode<sec_proto::LATransAdaptDPCmdMonitor>(
    unsigned int szframe_appdata,
    const sec_proto::LATransAdaptDPCmdMonitor &info, unsigned int data_start,
    size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::LATransAdaptDPCmdBufferMonitor>(
    unsigned int szframe_appdata,
    const sec_proto::LATransAdaptDPCmdBufferMonitor &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<
    sec_proto::ReqSafemodeTransAdaptTradeNotify>(
    unsigned int szframe_appdata,
    const sec_proto::ReqSafemodeTransAdaptTradeNotify &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<
    sec_proto::ReqUICTransAdaptMsgForwardPkg>(
    unsigned int szframe_appdata,
    const sec_proto::ReqUICTransAdaptMsgForwardPkg &info,
    unsigned int data_start, size_t *sz_code);

// ---------------------------------------------------------------------------
// DPMsgPacker: DWARF size 1, all methods static at the ABI level.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

class DPMsgPacker {
public:
    template <class PKG>
    static int generate_comm_frame(Comm_App_Frame* rsp_msg, unsigned int cmd,
                                   int entire, tss_sdk_impl::UserInfo* user,
                                   const PKG& forward_pkg);

    template <class PKG>
    static int send_ipc_pkg(tss_sdk_impl::UserInfo* user, const PKG& pkg,
                            unsigned int cmd);

    template <class PKG>
    static int send_datatrans_pkg(tss_sdk_impl::UserInfo* user, int entire,
                                  const PKG& forward_pkg, unsigned int cmd);
};

// ---------------------------------------------------------------------------
// generate_comm_frame @0xb3710 (and the three identical instantiations).
//
// ORIG sequence:
//   init_framehead(MAX_LEN_OF_APPFRAME, 0, cmd)
//   SERVICES_ID rcvsvc((uint16)entire,0), proxysvc(0,0), sndsvc(0x1f,0)
//   frame_uin_ = user->uin; recv_service_=rcvsvc; proxy_service_=proxysvc;
//   send_service_=sndsvc; transaction_id_=0; backfill_trans_id_=0;
//   app_id_ = glb_dp_config_head_->game_id
//   if (frame_option_ bit 16) dumpoutput_framehead(..., RS_INFO)
//   ret = appdata_encode(MAX_LEN_OF_APPFRAME_DATA, forward_pkg, 0, NULL)
//   on ret != 0: ERROR log, return 0x139f; else return 0.
// The ORIG tests BYTE PTR [frame+6],0x1, i.e. bit 16 of frame_option_.
// ---------------------------------------------------------------------------
template <class PKG>
int DPMsgPacker::generate_comm_frame(Comm_App_Frame* rsp_msg,
                                     unsigned int cmd, int entire,
                                     tss_sdk_impl::UserInfo* user,
                                     const PKG& forward_pkg) {
    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, cmd);

    SERVICES_ID rcvsvc((unsigned short)entire, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x1f, 0);

    rsp_msg->frame_uin_ = user->base_info_.uin_;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->backfill_trans_id_ = 0;
    rsp_msg->app_id_ = tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if ((rsp_msg->frame_option_ & 0x00010000u) != 0) {
        Comm_App_Frame::dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);
    }

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, forward_pkg, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], app data encode fail. ret=%d",
            user->base_info_.uin_, __func__, ret);
        ret = 0x139f;
    }
    return ret;
}

// ---------------------------------------------------------------------------
// send_ipc_pkg @0xb3e50 (and the two identical instantiations).
//
// ORIG is a wrapper around the inlined
//   Singleton<TssSdkCommSvc>::instance()->send_ipc_pkg(pkg, uin, cmd, 0x1f)
// (the callee supplies send service 0x2b and recv service 0x1f), followed by
// a success log at user priority or an ERROR failure log.
// ---------------------------------------------------------------------------
template <class PKG>
int DPMsgPacker::send_ipc_pkg(tss_sdk_impl::UserInfo* user, const PKG& pkg,
                              unsigned int cmd) {
    int res = Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()->send_ipc_pkg(
        pkg, user->base_info_.uin_, cmd, 0x1f);

    if (res == 0) {
        const ZEN_LOG_PRIORITY priority =
            (user->base_info_.flags_ & 1u) == 1u ? RS_INFO : RS_DEBUG;
        ZEN_Trace_LogMsg::debug_output(
            priority, "[%u][%s], send_ipc_pkg success, cmd=%u",
            user->base_info_.uin_, __func__, cmd);
    } else {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], send_ipc_pkg fail, res=%d, cmd=%u",
            user->base_info_.uin_, __func__, res, cmd);
    }
    return res;
}

// ---------------------------------------------------------------------------
// send_datatrans_pkg @0xb47e0 (and the three identical instantiations).
//
// ORIG:
//   unsigned char trans_send_buffer[65536];  (DWARF array type, count 65536)
//   Comm_App_Frame* rsp_msg = (Comm_App_Frame*)trans_send_buffer;
//   ret = generate_comm_frame(rsp_msg, cmd, entire, user, forward_pkg);
//   on failure: ERROR generate_comm_frame-fail log and return ret.
//   ReqSecsvrForwardData recv_from_sdk;
//   channel_id_ = (uint16)TssSdkCommSvc::instance()->channel_id_;
//   room_id_    = (uint16)glb_dp_config_head_->world_id;
//   memcpy(data_, rsp_msg, rsp_msg->frame_length_);   // unclamped, as ORIG
//   data_len_   = (uint16)rsp_msg->frame_length_;
//   return send_ipc_pkg<ReqSecsvrForwardData>(user, recv_from_sdk, 0x4c6ad);
// ---------------------------------------------------------------------------
template <class PKG>
int DPMsgPacker::send_datatrans_pkg(tss_sdk_impl::UserInfo* user, int entire,
                                    const PKG& forward_pkg,
                                    unsigned int cmd) {
    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame* rsp_msg =
        reinterpret_cast<Comm_App_Frame*>(trans_send_buffer);

    int ret =
        generate_comm_frame(rsp_msg, cmd, entire, user, forward_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], generate_comm_frame fail, ret=%d, entire=%d, cmd=%u",
            user->base_info_.uin_, __func__, ret, entire, cmd);
        return ret;
    }

    sec_proto::ReqSecsvrForwardData recv_from_sdk;
    recv_from_sdk.channel_id_ =
        (unsigned short)Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()
            ->channel_id_;
    recv_from_sdk.room_id_ =
        (unsigned short)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
    memcpy(recv_from_sdk.data_, rsp_msg, rsp_msg->frame_length_);
    recv_from_sdk.data_len_ = (unsigned short)rsp_msg->frame_length_;

    return send_ipc_pkg<sec_proto::ReqSecsvrForwardData>(user, recv_from_sdk,
                                                        0x4c6ad);
}

// ---------------------------------------------------------------------------
// Explicit instantiations, exactly the eleven ORIG weak symbols.
// ---------------------------------------------------------------------------
template int DPMsgPacker::generate_comm_frame<sec_proto::LATransAdaptDPCmdMonitor>(
    Comm_App_Frame*, unsigned int, int, tss_sdk_impl::UserInfo*,
    const sec_proto::LATransAdaptDPCmdMonitor&);
template int DPMsgPacker::generate_comm_frame<
    sec_proto::LATransAdaptDPCmdBufferMonitor>(
    Comm_App_Frame*, unsigned int, int, tss_sdk_impl::UserInfo*,
    const sec_proto::LATransAdaptDPCmdBufferMonitor&);
template int DPMsgPacker::generate_comm_frame<
    sec_proto::ReqSafemodeTransAdaptTradeNotify>(
    Comm_App_Frame*, unsigned int, int, tss_sdk_impl::UserInfo*,
    const sec_proto::ReqSafemodeTransAdaptTradeNotify&);
template int DPMsgPacker::generate_comm_frame<
    sec_proto::ReqUICTransAdaptMsgForwardPkg>(
    Comm_App_Frame*, unsigned int, int, tss_sdk_impl::UserInfo*,
    const sec_proto::ReqUICTransAdaptMsgForwardPkg&);

template int DPMsgPacker::send_ipc_pkg<sec_proto::SecsvrCaptchaReq>(
    tss_sdk_impl::UserInfo*, const sec_proto::SecsvrCaptchaReq&, unsigned int);
template int DPMsgPacker::send_ipc_pkg<sec_proto::ReportErrReq>(
    tss_sdk_impl::UserInfo*, const sec_proto::ReportErrReq&, unsigned int);
template int DPMsgPacker::send_ipc_pkg<sec_proto::ReqSecsvrForwardData>(
    tss_sdk_impl::UserInfo*, const sec_proto::ReqSecsvrForwardData&,
    unsigned int);

template int DPMsgPacker::send_datatrans_pkg<sec_proto::LATransAdaptDPCmdMonitor>(
    tss_sdk_impl::UserInfo*, int, const sec_proto::LATransAdaptDPCmdMonitor&,
    unsigned int);
template int DPMsgPacker::send_datatrans_pkg<
    sec_proto::LATransAdaptDPCmdBufferMonitor>(
    tss_sdk_impl::UserInfo*, int,
    const sec_proto::LATransAdaptDPCmdBufferMonitor&, unsigned int);
template int DPMsgPacker::send_datatrans_pkg<
    sec_proto::ReqSafemodeTransAdaptTradeNotify>(
    tss_sdk_impl::UserInfo*, int,
    const sec_proto::ReqSafemodeTransAdaptTradeNotify&, unsigned int);
template int DPMsgPacker::send_datatrans_pkg<
    sec_proto::ReqUICTransAdaptMsgForwardPkg>(
    tss_sdk_impl::UserInfo*, int,
    const sec_proto::ReqUICTransAdaptMsgForwardPkg&, unsigned int);

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Compile-time layout checks against DWARF.
// ---------------------------------------------------------------------------
// SERVICES_ID and Comm_App_Frame have user-declared constructors, so GCC
// 4.1's __builtin_offsetof warns on their members.  Their packed layouts are
// fully determined by sizeof(SERVICES_ID)==6 and sizeof(Comm_App_Frame)==50:
// +0/+4/+8/+12/+16/+22/+28/+34/+38/+42/+46/+50.
typedef char Check_SERVICES_ID_size[sizeof(SERVICES_ID) == 6 ? 1 : -1];
typedef char Check_Comm_App_Frame_size[sizeof(Comm_App_Frame) == 50 ? 1 : -1];

typedef char Check_ReportErrReq_size[sizeof(sec_proto::ReportErrReq) == 8 ? 1 : -1];
typedef char Check_ReportErrReq_channel[
    __builtin_offsetof(sec_proto::ReportErrReq, channel_id_) == 0 ? 1 : -1];
typedef char Check_ReportErrReq_room[
    __builtin_offsetof(sec_proto::ReportErrReq, room_id_) == 2 ? 1 : -1];
typedef char Check_ReportErrReq_error[
    __builtin_offsetof(sec_proto::ReportErrReq, error_id_) == 4 ? 1 : -1];

typedef char Check_SecsvrCaptchaReq_size[
    sizeof(sec_proto::SecsvrCaptchaReq) == 541 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_id[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_id_) == 26 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_len[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_info_len_) == 27 ? 1 : -1];
typedef char Check_SecsvrCaptchaReq_busi_info[
    __builtin_offsetof(sec_proto::SecsvrCaptchaReq, busi_info_) == 29 ? 1 : -1];

typedef char Check_ReqSecsvrForwardData_size[
    sizeof(sec_proto::ReqSecsvrForwardData) == 32773 ? 1 : -1];
typedef char Check_ReqSecsvrForwardData_channel[
    __builtin_offsetof(sec_proto::ReqSecsvrForwardData, channel_id_) == 0 ? 1 : -1];
typedef char Check_ReqSecsvrForwardData_room[
    __builtin_offsetof(sec_proto::ReqSecsvrForwardData, room_id_) == 2 ? 1 : -1];
typedef char Check_ReqSecsvrForwardData_len[
    __builtin_offsetof(sec_proto::ReqSecsvrForwardData, data_len_) == 4 ? 1 : -1];
typedef char Check_ReqSecsvrForwardData_data[
    __builtin_offsetof(sec_proto::ReqSecsvrForwardData, data_) == 6 ? 1 : -1];

typedef char Check_LATransAdaptDPCmdMonitor_size[
    sizeof(sec_proto::LATransAdaptDPCmdMonitor) == 14 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdMonitor_room[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdMonitor, room_id_) == 0 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdMonitor_cmd[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdMonitor, monitor_cmd_id_) == 8 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdMonitor_enc[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdMonitor, is_need_encrypt_) == 12 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdMonitor_encd[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdMonitor, is_encrypted_) == 13 ? 1 : -1];

typedef char Check_LATransAdaptDPCmdBufferMonitor_size[
    sizeof(sec_proto::LATransAdaptDPCmdBufferMonitor) == 32786 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdBufferMonitor_len[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor, data_len_) == 14 ? 1 : -1];
typedef char Check_LATransAdaptDPCmdBufferMonitor_data[
    __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor, data_) == 18 ? 1 : -1];

typedef char Check_ReqSafemodeTransAdaptTradeNotify_size[
    sizeof(sec_proto::ReqSafemodeTransAdaptTradeNotify) == 2073 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_world[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, world_id_) == 0 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_src[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, src_id_) == 2 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_type[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, type_) == 6 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_send_uin[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, send_uin_) == 7 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_target_uin[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, target_uin_) == 11 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_send_ip[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, send_ip_) == 15 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_target_ip[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, target_ip_) == 19 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_buf_len[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, buf_len_) == 23 ? 1 : -1];
typedef char Check_ReqSafemodeTransAdaptTradeNotify_buf[
    __builtin_offsetof(sec_proto::ReqSafemodeTransAdaptTradeNotify, trade_buf_) == 25 ? 1 : -1];

typedef char Check_ReqUICTransAdaptMsgForwardPkg_size[
    sizeof(sec_proto::ReqUICTransAdaptMsgForwardPkg) == 33044 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_world[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, world_id_) == 0 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_src[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, src_id_) == 2 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_cmd[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, cmd_id_) == 6 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_need[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, need_encrypt_) == 10 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_encd[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, is_encrypted_) == 11 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_len[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, cmd_buf_len_) == 12 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_buf[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, cmd_buf_) == 14 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_rolename_len[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, rolename_len_) == 32782 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_rolename[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, rolename_) == 32784 ? 1 : -1];
typedef char Check_ReqUICTransAdaptMsgForwardPkg_ip[
    __builtin_offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, client_ip_) == 33040 ? 1 : -1];

typedef char Check_DP_CONFIG_size[sizeof(tss_dp_comm::DP_CONFIG) == 280 ? 1 : -1];
typedef char Check_DP_CONFIG_game_id[
    __builtin_offsetof(tss_dp_comm::DP_CONFIG, game_id) == 8 ? 1 : -1];
typedef char Check_DP_CONFIG_world_id[
    __builtin_offsetof(tss_dp_comm::DP_CONFIG, world_id) == 12 ? 1 : -1];

typedef char Check_TssSdkCommSvc_size[
    sizeof(tss_sdk_impl::TssSdkCommSvc) == 40 ? 1 : -1];
