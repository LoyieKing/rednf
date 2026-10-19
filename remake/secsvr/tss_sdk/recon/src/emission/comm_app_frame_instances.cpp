// Explicit instantiations of Comm_App_Frame::appdata_encode<T> /
// appdata_decode<T> for the 25 sec_proto types that ORIG
// (libtss_sdk.so.1.0.394) exports as weak symbols but the reconstructed tree
// did not emit.
//
// The template body is the one from the reused comm_app_frame.h
// (source/gunnersvr|secagent/src/commlib/framework/comm_app_frame.h, identical
// in both trees; the reuse object reuse/obj/src__commlib__framework__comm_app_frame.o
// was built from the gunnersvr copy).  The body only reads
// LEN_OF_APPFRAME_HEAD (0x32), frame_length_ (+0x00) and frame_appdata_ (+0x32),
// and calls T::pack/T::unpack, so the instantiation is byte-identical for both
// copies of the header.
//
// Header choice for the protocol types (the ORIG headers are not in any
// reconstructed tree, so the closest reconstructions were used and every layout
// was checked against ORIG DWARF):
//
//   CfgFileReq, CfgFileRsp, CfgsvrInfoRsp, CheckCfgFileUpdateReq,
//   CheckCfgFileUpdateRsp, DeployInfoReq, DeployInfoRsp, SvcIdReq, SvcIdRsp,
//   SvcIpInfoReq, SvcIpInfoRsp
//       src/protocol/common/comm_proto_public_cfgsvr.h
//   LogSvrCommBillRecord
//       src/protocol/common/comm_proto_public_logsvr.h
//   ReportErrReq, ReportLostAntibotPkg, ReqSecsvrForwardData
//       src/protocol/formmog/formmog_proto_public_secsvr.h
//   DPCaptchaResultRsp, DpBanModeReq, DpDropCmdlistReq,
//   SafemodeLimitStatusRsp, SafemodeStatusRsp
//       src/protocol/formmog/formmog_proto_public_tsssdk.h
//   LATransAdaptDPCmdMonitor, LATransAdaptDPCmdBufferMonitor,
//   ReqSafemodeTransAdaptTradeNotify, ReqUICTransAdaptMsgForwardPkg,
//   SecsvrCaptchaReq
//       sec_proto_surface_models.h in this directory (these five have no header
//       in any tree: the four formmog ones exist only as the per-CU
//       reconstructions recon/src/origsrc/formmog_proto_public_{latransadapt,
//       safemodetransadapt,uictransadapt}.h, whose family-private tsf4g_tdr
//       compatibility headers are mutually exclusive inside one TU, and
//       SecsvrCaptchaReq exists only inside formmog_proto_public_secsvr_extra.cpp;
//       the models copy those layouts verbatim and assert their ORIG sizes).
//
// The four headers above all resolve against the reconstructed dnf_secsvr tree
// source/secagent (they are the same reconstruction as the CU
// src/commlib/framework/comm_cfgsvr_sdk.cpp uses); no type is declared twice in
// this TU.
//
// Build:
//   sh c5build.sh recon/src/emission/comm_app_frame_instances.cpp <out.o> \
//      -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent
//
// ORIG addresses of the emitted weak symbols are listed per instantiation.

#include "src/commlib/framework/comm_app_frame.h"

#include "src/protocol/common/comm_proto_public_cfgsvr.h"
#include "src/protocol/common/comm_proto_public_logsvr.h"
#include "src/protocol/formmog/formmog_proto_public_secsvr.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

#include "sec_proto_surface_models.h"

// ---- appdata_encode (0x0009c720 .. 0x000e5640 in ORIG) ---------------------

template int Comm_App_Frame::appdata_encode<sec_proto::CfgFileReq>(          // 0x000e4d60
    unsigned int, const sec_proto::CfgFileReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::CheckCfgFileUpdateReq>(  // 0x000e49c0
    unsigned int, const sec_proto::CheckCfgFileUpdateReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::DeployInfoReq>(       // 0x000e5140
    unsigned int, const sec_proto::DeployInfoReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::SvcIdReq>(            // 0x000e53c0
    unsigned int, const sec_proto::SvcIdReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::SvcIpInfoReq>(        // 0x000e4ec0
    unsigned int, const sec_proto::SvcIpInfoReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::LATransAdaptDPCmdBufferMonitor>(  // 0x000b7e40
    unsigned int, const sec_proto::LATransAdaptDPCmdBufferMonitor &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::LATransAdaptDPCmdMonitor>(  // 0x000b3680
    unsigned int, const sec_proto::LATransAdaptDPCmdMonitor &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::LogSvrCommBillRecord>(  // 0x000a3460
    unsigned int, const sec_proto::LogSvrCommBillRecord &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::ReportErrReq>(        // 0x000b3b90
    unsigned int, const sec_proto::ReportErrReq &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::ReportLostAntibotPkg>(  // 0x000c4420
    unsigned int, const sec_proto::ReportLostAntibotPkg &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::ReqSafemodeTransAdaptTradeNotify>(  // 0x000ba200
    unsigned int, const sec_proto::ReqSafemodeTransAdaptTradeNotify &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::ReqSecsvrForwardData>(  // 0x000b38d0
    unsigned int, const sec_proto::ReqSecsvrForwardData &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::ReqUICTransAdaptMsgForwardPkg>(  // 0x000bc590
    unsigned int, const sec_proto::ReqUICTransAdaptMsgForwardPkg &, unsigned int, size_t *);

template int Comm_App_Frame::appdata_encode<sec_proto::SecsvrCaptchaReq>(    // 0x000b33c0
    unsigned int, const sec_proto::SecsvrCaptchaReq &, unsigned int, size_t *);

// ---- appdata_decode (0x0009c9e0 .. 0x000e5640 in ORIG) ---------------------

template int Comm_App_Frame::appdata_decode<sec_proto::CfgFileRsp>(          // 0x000e4c40
    sec_proto::CfgFileRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::CfgsvrInfoRsp>(       // 0x000e5640
    sec_proto::CfgsvrInfoRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::CheckCfgFileUpdateRsp>(  // 0x000e4b20
    sec_proto::CheckCfgFileUpdateRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::DeployInfoRsp>(       // 0x000e52a0
    sec_proto::DeployInfoRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::SvcIdRsp>(            // 0x000e5520
    sec_proto::SvcIdRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::SvcIpInfoRsp>(        // 0x000e5020
    sec_proto::SvcIpInfoRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::DPCaptchaResultRsp>(  // 0x000c10e0
    sec_proto::DPCaptchaResultRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::DpBanModeReq>(        // 0x000c0f90
    sec_proto::DpBanModeReq &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::DpDropCmdlistReq>(    // 0x000c1000
    sec_proto::DpDropCmdlistReq &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::SafemodeLimitStatusRsp>(  // 0x000c6ea0
    sec_proto::SafemodeLimitStatusRsp &, unsigned int, size_t *) const;

template int Comm_App_Frame::appdata_decode<sec_proto::SafemodeStatusRsp>(   // 0x000c1070
    sec_proto::SafemodeStatusRsp &, unsigned int, size_t *) const;
