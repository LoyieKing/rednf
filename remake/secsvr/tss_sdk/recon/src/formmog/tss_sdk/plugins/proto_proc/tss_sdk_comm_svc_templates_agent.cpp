// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI,
// -O2 -fPIC).  Semantic reconstruction, not a byte-identical one.
//
// Translation unit: src/formmog/tss_sdk/framework/tss_sdk_comm_svc.cpp
// class header:     tss_sdk_comm_svc.h
//
// ORIG weak symbols implemented (start..end, size from nm -S):
//   send_ipc_pkg<sec_proto::RecvDataFromSDKReq>
//       0x0009c7b0..0x0009c9d2 (0x222)
//   send_ipc_pkg<sec_proto::LogSvrCommBillRecord>
//       0x000a34f0..0x000a3712 (0x222)
//   send_ipc_pkg<sec_proto::SecsvrCaptchaReq>
//       0x000b3450..0x000b3672 (0x222)
//   send_ipc_pkg<sec_proto::ReqSecsvrForwardData>
//       0x000b3960..0x000b3b82 (0x222)
//   send_ipc_pkg<sec_proto::ReportErrReq>
//       0x000b3c20..0x000b3e42 (0x222)
//   send_ipc_pkg<sec_proto::ReportLostAntibotPkg>
//       0x000c44b0..0x000c46d2 (0x222)
//   send_ipc_pkg<sec_proto::ReportPkgTime>
//       0x000c4770..0x000c4992 (0x222)
//   send_ipc_pkg<sec_proto::DelUserReq>
//       0x000cd210..0x000cd432 (0x222)
//   send_ipc_pkg<sec_proto::AddUserReq>
//       0x000cd4d0..0x000cd6f2 (0x222)
//   send_socket_pkg<sec_proto::UserInputCheckPkg>
//       0x000c8180..0x000c839c (0x21c)
//
// Object shape and constants (DWARF + disassembly):
//   sizeof(Comm_App_Frame)      = 50
//   MAX_LEN_OF_APPFRAME         = 65536 (0x10000)
//   MAX_LEN_OF_APPFRAME_DATA    = 65469 (0xffbd)
//   encode-failure replacement  = 0x139f
//   transport-failure return    = -2 (0xfffffffe)
//
// Source-level shape, identical for both templates:
//   - build a Comm_App_Frame over a 65536-byte stack buffer,
//   - init_framehead(MAX_LEN_OF_APPFRAME, 0, cmd),
//   - fill services (sndsvc = 0x2b, proxy = 0, recv = rcv or 0),
//   - encode the protocol body through Comm_App_Frame::appdata_encode<>,
//   - hand the raw frame to TssSdkCommSvc::send_buf_to_backend().
// send_ipc_pkg selects the IPC channel (use_ipc = true); send_socket_pkg
// selects the socket channel (use_ipc = false, business_id passed through).

// ---------------------------------------------------------------------------
// Shared ABI declarations.  The local SERVICES_ID / _APP_FRAME_OPTION /
// Comm_App_Frame / logging / DP_CONFIG models are gone: the canonical tag-394
// headers are the single definitions in the build (Comm_App_Frame 50 with the
// flexible frame_appdata_, packed SERVICES_ID 6, tss_dp_comm::DP_CONFIG 280).
// ---------------------------------------------------------------------------
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ---------------------------------------------------------------------------
// The ten wire packages.  The templates only take them by reference and hand
// them to Comm_App_Frame::appdata_encode<>; their internal TDR layouts are
// owned by the generated sec_proto pack() implementations, so no local
// payload fields are invented here.
// ---------------------------------------------------------------------------

namespace sec_proto {
class RecvDataFromSDKReq;
class LogSvrCommBillRecord;
class SecsvrCaptchaReq;
class ReqSecsvrForwardData;
class ReportErrReq;
class ReportLostAntibotPkg;
class ReportPkgTime;
class DelUserReq;
class AddUserReq;
class UserInputCheckPkg;
}  // namespace sec_proto

// Comm_App_Frame::appdata_encode<T> for the ten packets below.  The bodies are
// the ORIG weak instantiations defined by the packet-owning CUs (this TU only
// ever held the declaration); these explicit specializations keep the calls
// external references instead of pulling the inline header body into the
// instantiations of send_ipc_pkg<>/send_socket_pkg<>.
template <>
int Comm_App_Frame::appdata_encode<sec_proto::RecvDataFromSDKReq>(
    unsigned int szframe_appdata, const sec_proto::RecvDataFromSDKReq &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::LogSvrCommBillRecord>(
    unsigned int szframe_appdata,
    const sec_proto::LogSvrCommBillRecord &info, unsigned int data_start,
    size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::SecsvrCaptchaReq>(
    unsigned int szframe_appdata, const sec_proto::SecsvrCaptchaReq &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::ReqSecsvrForwardData>(
    unsigned int szframe_appdata,
    const sec_proto::ReqSecsvrForwardData &info, unsigned int data_start,
    size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::ReportErrReq>(
    unsigned int szframe_appdata, const sec_proto::ReportErrReq &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::ReportLostAntibotPkg>(
    unsigned int szframe_appdata,
    const sec_proto::ReportLostAntibotPkg &info, unsigned int data_start,
    size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::ReportPkgTime>(
    unsigned int szframe_appdata, const sec_proto::ReportPkgTime &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::DelUserReq>(
    unsigned int szframe_appdata, const sec_proto::DelUserReq &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::AddUserReq>(
    unsigned int szframe_appdata, const sec_proto::AddUserReq &info,
    unsigned int data_start, size_t *sz_code);
template <>
int Comm_App_Frame::appdata_encode<sec_proto::UserInputCheckPkg>(
    unsigned int szframe_appdata, const sec_proto::UserInputCheckPkg &info,
    unsigned int data_start, size_t *sz_code);

// ---------------------------------------------------------------------------
// tss_sdk_impl::TssSdkCommSvc (DWARF size 40).  Only this pointer and its
// send_buf_to_backend() entry point are used by the templates.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

template <class PKG>
int TssSdkCommSvc::send_ipc_pkg(PKG const &pkg, unsigned int uin,
                                unsigned int cmd, unsigned short rcv) {
    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg = (Comm_App_Frame *)trans_send_buffer;

    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, cmd);

    SERVICES_ID rcvsvc(rcv, 0);
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

    if (rsp_msg->inner_option_.inner_option_ & 0x10000)
        Comm_App_Frame::dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, pkg, 0, 0);
    if (ret != 0)
        return 0x139f;

    ret = send_buf_to_backend((char const *)trans_send_buffer,
                              rsp_msg->frame_length_, true, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]send to backend fail. ret=%d", __PRETTY_FUNCTION__, ret);
        return -2;
    }
    return 0;
}

template <class PKG>
int TssSdkCommSvc::send_socket_pkg(int business_id, PKG const &pkg) {
    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg = (Comm_App_Frame *)trans_send_buffer;

    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0);

    SERVICES_ID rcvsvc(0, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x2b, 0);

    rsp_msg->frame_uin_ = 0;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;
    rsp_msg->backfill_trans_id_ = 0;
    rsp_msg->app_id_ = 0;

    if (rsp_msg->inner_option_.inner_option_ & 0x10000)
        Comm_App_Frame::dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, pkg, 0, 0);
    if (ret != 0)
        return 0x139f;

    ret = send_buf_to_backend((char const *)trans_send_buffer,
                              rsp_msg->frame_length_, false, business_id);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]send to backend fail. ret=%d", __PRETTY_FUNCTION__, ret);
        return -2;
    }
    return 0;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Explicit instantiations: these produce the ten weak ORIG symbols.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

template int TssSdkCommSvc::send_ipc_pkg<sec_proto::RecvDataFromSDKReq>(
    sec_proto::RecvDataFromSDKReq const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::LogSvrCommBillRecord>(
    sec_proto::LogSvrCommBillRecord const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::SecsvrCaptchaReq>(
    sec_proto::SecsvrCaptchaReq const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReqSecsvrForwardData>(
    sec_proto::ReqSecsvrForwardData const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportErrReq>(
    sec_proto::ReportErrReq const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportLostAntibotPkg>(
    sec_proto::ReportLostAntibotPkg const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::ReportPkgTime>(
    sec_proto::ReportPkgTime const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::DelUserReq>(
    sec_proto::DelUserReq const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_ipc_pkg<sec_proto::AddUserReq>(
    sec_proto::AddUserReq const&, unsigned int, unsigned int,
    unsigned short);
template int TssSdkCommSvc::send_socket_pkg<sec_proto::UserInputCheckPkg>(
    int, sec_proto::UserInputCheckPkg const&);

}  // namespace tss_sdk_impl
