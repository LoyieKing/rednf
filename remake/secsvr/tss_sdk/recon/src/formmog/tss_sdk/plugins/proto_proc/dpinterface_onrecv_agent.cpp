// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
//   DPInterface::on_recv_msg(tss_sdk_impl::UserInfo*, void*)
//       DWARF low_pc 0x000c04f0, high_pc 0x000c09c2
//       next symbol __static_initialization_and_destruction_0 at 0x000c09d0
//       mangled: _ZN11DPInterface11on_recv_msgEPN12tss_sdk_impl8UserInfoEPv
//
// The out-of-line symbol above is the real definition.  All helper types in
// this file are partial ABI-accurate models used to emit the exact calls and
// vtable offsets observed in the binary; they are marked INTEGRATION
// BOUNDARY and must be replaced by the real SDK headers when this TU is wired
// into the full build.  The sec_proto decode templates are deliberately left
// undefined: the original calls the out-of-line/instantiated
// Comm_App_Frame::appdata_decode<T> symbols, which are dependencies.
//
// Recovered control flow (source line numbers from .debug_line):
//   448  entry
//   450  user / proc_data / is_inited_ / glb_dp_config_->is_use_dp guard -> -1
//   456  res
//   457  app_frame = (const Comm_App_Frame *)proc_data
//   458  bill dump gate: user->base_info_.flags_ & 1
//   461  DPBillAdapter::dump("on recv msg", uin, app_frame, frame_length_)
//   464  switch (app_frame->frame_command_)
//   469  DpBanModeReq decode -> DPHackMgr::on_ban_mode
//   482  DpDropCmdlistReq decode -> DPHackMgr::on_drop_cmd
//   495  SafemodeStatusRsp decode -> DPHackMgr::on_safe_mode
//   507  DPCaptchaResultRsp decode -> DPHackMgr::on_verify_msg
//   519  SendToSDK forward_pkg decode -> DPHackMgr::on_verify_response
//        (only when forward_pkg.to_busi_id_ == 2)
//   536  entry-failure return -1 and the default/unknown-command block are
//        both attributed to this line by the line table
//   542  final `return res`
//
// Frame command jump table at 0x0021d964 (EBX delta base 0x00294534):
//   0x493e1 -> 0x0c05a5  0x493e2 -> default  0x493e3 -> default
//   0x493e4 -> default   0x493e5 -> 0x0c060e  0x493e6 -> default
//   0x493e7 -> 0x0c067d  0x493e8 -> default  0x493e9 -> default
//   0x493ea -> 0x0c06ec  0x493eb -> default  0x493ec -> 0x0c075b
//
// No DPMsgHandler method is called by this function; the packet handling goes
// through the five inlined DPHackMgr / IDPHackerEventNotify loops.

#include "src/closing/dp_config.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

#include <stddef.h>

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"

template <>
int Comm_App_Frame::appdata_decode<sec_proto::SendToSDK>(
    sec_proto::SendToSDK &info, unsigned int data_start, size_t *sz_code) const;

template <>
int Comm_App_Frame::appdata_decode<sec_proto::DpBanModeReq>(
    sec_proto::DpBanModeReq &info, unsigned int data_start, size_t *sz_code) const;

template <>
int Comm_App_Frame::appdata_decode<sec_proto::DpDropCmdlistReq>(
    sec_proto::DpDropCmdlistReq &info, unsigned int data_start,
    size_t *sz_code) const;

template <>
int Comm_App_Frame::appdata_decode<sec_proto::SafemodeStatusRsp>(
    sec_proto::SafemodeStatusRsp &info, unsigned int data_start,
    size_t *sz_code) const;

template <>
int Comm_App_Frame::appdata_decode<sec_proto::DPCaptchaResultRsp>(
    sec_proto::DPCaptchaResultRsp &info, unsigned int data_start,
    size_t *sz_code) const;



// ---------------------------------------------------------------------------
// DPInterface::on_recv_msg
// ---------------------------------------------------------------------------
int DPInterface::on_recv_msg(tss_sdk_impl::UserInfo *user, void *proc_data) {
    if (user == 0 || proc_data == 0 || !is_inited_ ||
        !tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return -1;

    int res = 0;
    const Comm_App_Frame *app_frame =
        reinterpret_cast<const Comm_App_Frame *>(proc_data);

    if ((user->base_info_.flags_ & 1) != 0)
        tss_dpsdk::DPBillAdapter::dump(
            "on recv msg", user->base_info_.uin_,
            reinterpret_cast<const unsigned char *>(app_frame),
            app_frame->frame_length_);

    switch (app_frame->frame_command_) {
    case 300007: {  // sec_proto::DpBanModeReq (raw command id 0x493e7)
        sec_proto::DpBanModeReq pkg;
        res = app_frame->appdata_decode(pkg, 0, 0);
        if (res != 0) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], unpack dp ban mode pkg failed, len=%u",
                user != 0 ? user->base_info_.uin_ : 0u, __func__,
                app_frame->frame_length_);
            break;
        }
        res = reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_)->on_ban_mode(user, pkg);
        break;
    }
    case 300005: {  // sec_proto::DpDropCmdlistReq (raw command id 0x493e5)
        sec_proto::DpDropCmdlistReq pkg;
        res = app_frame->appdata_decode(pkg, 0, 0);
        if (res != 0) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], unpack dp drop cmdlist pkg failed, len=%u",
                user != 0 ? user->base_info_.uin_ : 0u, __func__,
                app_frame->frame_length_);
            break;
        }
        res = reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_)->on_drop_cmd(user, pkg);
        break;
    }
    case 300012: {  // sec_proto::SafemodeStatusRsp (raw command id 0x493ec)
        sec_proto::SafemodeStatusRsp pkg;
        res = app_frame->appdata_decode(pkg, 0, 0);
        if (res != 0) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], unpack safemode status pkg failed, len=%u",
                user != 0 ? user->base_info_.uin_ : 0u, __func__,
                app_frame->frame_length_);
            break;
        }
        res = reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_)->on_safe_mode(
            user, static_cast<int>(pkg.stat_level_));
        break;
    }
    case 300010: {  // sec_proto::DPCaptchaResultRsp (raw command id 0x493ea)
        sec_proto::DPCaptchaResultRsp pkg;
        res = app_frame->appdata_decode(pkg, 0, 0);
        if (res != 0) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], unpack dp catpcha verify pkg failed,len=%u",
                user != 0 ? user->base_info_.uin_ : 0u, __func__,
                app_frame->frame_length_);
            break;
        }
        res = reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_)->on_verify_msg(user, pkg);
        break;
    }
    case 300001: {  // sec_proto::SendToSDK (raw command id 0x493e1)
        sec_proto::SendToSDK forward_pkg;
        res = app_frame->appdata_decode(forward_pkg, 0, 0);
        if (res != 0) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR, "[%u][%s], unpack forward data failed,len=%u",
                user != 0 ? user->base_info_.uin_ : 0u, __func__,
                app_frame->frame_length_);
            break;
        }
        if (forward_pkg.to_busi_id_ != 2)
            break;
        res = reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_)->on_verify_response(
            user, forward_pkg.data_, forward_pkg.data_len_);
        break;
    }
    default:
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR, "[%u][%s], unknow cmd=%d",
            user != 0 ? user->base_info_.uin_ : 0u, __func__,
            app_frame->frame_command_);
        break;
    }

    return res;
}
