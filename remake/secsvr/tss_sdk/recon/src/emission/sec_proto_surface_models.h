// Protocol-struct models for the five sec_proto types whose real header is not
// available in any reconstructed tree, needed by comm_app_frame_instances.cpp to
// instantiate Comm_App_Frame::appdata_encode/decode.
//
// Provenance of each layout (all are byte-packed, #pragma pack(1)):
//
//   LATransAdaptDPCmdMonitor / LATransAdaptDPCmdBufferMonitor
//       recon/src/origsrc/formmog_proto_public_latransadapt.h (DWARF-derived
//       reconstruction of the ORIG CU formmog_proto_public_latransadapt.cpp;
//       sizes 14 / 32786 cross-checked against ORIG DWARF here).
//   ReqSafemodeTransAdaptTradeNotify
//       recon/src/origsrc/formmog_proto_public_safemodetransadapt.h (size 2073).
//   ReqUICTransAdaptMsgForwardPkg
//       recon/src/origsrc/formmog_proto_public_uictransadapt.h (size 33044).
//   SecsvrCaptchaReq
//       recon/src/origsrc/formmog_proto_public_secsvr_extra.cpp, whose layout
//       comment records the ORIG construct() memset sizes 2+2+4+2+4+4+2+2+4+1+2+512
//       = 541 (size 541 here).
//
// The ORIG byte sizes come from the DWARF definition DIEs of
// libtss_sdk.so.1.0.394 (typedefs.py): 14, 32786, 2073, 33044, 541.  The
// STATIC_ASSERT_EQ lines below fail the build if any of these layouts drifts.
//
// pack()/unpack() are declared with the ORIG's exact parameter and const
// qualification so the instantiated template calls the ORIG symbol
// (sec_proto::<T>::pack(char*, unsigned int, unsigned int*, unsigned int) const
// and ::unpack(char const*, unsigned int, unsigned int*, unsigned int)); the
// empty exception specification the family headers carry does not take part in
// the Itanium mangling, so it is omitted here.
#ifndef RECON_SRC_EMISSION_SEC_PROTO_SURFACE_MODELS_H_
#define RECON_SRC_EMISSION_SEC_PROTO_SURFACE_MODELS_H_

#include <stdint.h>
#include <stddef.h>

#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

struct LATransAdaptDPCmdMonitor {
    uint32_t room_id_;
    uint32_t time_stamp_;
    int32_t monitor_cmd_id_;
    uint8_t is_need_encrypt_;
    uint8_t is_encrypted_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
};

struct LATransAdaptDPCmdBufferMonitor {
    uint32_t room_id_;
    uint32_t time_stamp_;
    int32_t monitor_cmd_id_;
    uint8_t is_need_encrypt_;
    uint8_t is_encrypted_;
    uint32_t data_len_;
    uint8_t data_[32768];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
};

struct ReqSafemodeTransAdaptTradeNotify {
    uint16_t world_id_;
    uint32_t src_id_;
    uint8_t type_;
    uint32_t send_uin_;
    uint32_t target_uin_;
    uint32_t send_ip_;
    uint32_t target_ip_;
    uint16_t buf_len_;
    uint8_t trade_buf_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
};

struct ReqUICTransAdaptMsgForwardPkg {
    uint16_t world_id_;
    uint32_t src_id_;
    uint32_t cmd_id_;
    uint8_t need_encrypt_;
    uint8_t is_encrypted_;
    uint16_t cmd_buf_len_;
    uint8_t cmd_buf_[32768];
    uint16_t rolename_len_;
    uint8_t rolename_[256];
    uint32_t client_ip_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
};

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

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
};

#pragma pack(pop)

}  // namespace sec_proto

#define SA_CAT2(a, b) a##b
#define SA_CAT(a, b) SA_CAT2(a, b)
#define STATIC_ASSERT_EQ(T, N)                                                 \
    typedef char SA_CAT(assert_size_, __LINE__)[(sizeof(T) == (size_t)(N)) ? 1 : -1]

STATIC_ASSERT_EQ(sec_proto::LATransAdaptDPCmdMonitor, 14);
STATIC_ASSERT_EQ(sec_proto::LATransAdaptDPCmdBufferMonitor, 32786);
STATIC_ASSERT_EQ(sec_proto::ReqSafemodeTransAdaptTradeNotify, 2073);
STATIC_ASSERT_EQ(sec_proto::ReqUICTransAdaptMsgForwardPkg, 33044);
STATIC_ASSERT_EQ(sec_proto::SecsvrCaptchaReq, 541);

#endif  // RECON_SRC_EMISSION_SEC_PROTO_SURFACE_MODELS_H_
