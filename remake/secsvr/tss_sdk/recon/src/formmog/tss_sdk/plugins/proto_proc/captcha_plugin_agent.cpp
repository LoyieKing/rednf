// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/plugins/captcha/captcha_plugin.cpp
//
// Reconstructed definitions (addresses are shipped symbol starts; the end
// excludes inter-symbol padding):
//   TssSdkCaptchaPlugin::TssSdkCaptchaPlugin() (C1)  0x0009b6d0-0x0009b722
//   TssSdkCaptchaPlugin::TssSdkCaptchaPlugin() (C2)  0x0009b7e0-0x0009b832
//   get_obj_TssSdkCaptchaPlugin()                    0x0009b740-0x0009b7d6
//   TssSdkCaptchaPlugin::fini()                      0x0009b620-0x0009b626
//   TssSdkCaptchaPlugin::proc(void*)                 0x0009b890-0x0009bc15
//   TssSdkCaptchaPlugin::init(void*)                 0x0009c030-0x0009c1ad
//   TssSdkCaptchaPlugin::notify(void*)               0x0009c1b0-0x0009c6af
//   TssSdkCaptchaPlugin::~TssSdkCaptchaPlugin() D0   0x0009c6b0-0x0009c6ee
//   TssSdkCaptchaPlugin::~TssSdkCaptchaPlugin() D1   0x0009c6f0-0x0009c71d
//
// Class layout (DWARF, sizeof == 20):
//   +0x00  vptr
//   +0x04  TssSdkPluginCallbackProc callback_proc_
//   +0x08  CThreadErrorInfo error_info_ (4-byte pthread key)
//   +0x0c  tss_sdk_if::ETssSdkPluginId sdk_plugin_id_ (CAPTCHA == 5)
//   +0x10  TssSdkCommSvc *sdk_comm_svc_
//
// The constructor does NOT initialise sdk_comm_svc_.  The shipped object is
// a function-local static in BSS (zero before init), so init() is the only
// writer of +0x10.  ctor/fini/init/notify return 0 on success.
//
// The plugin speaks two directions:
//   proc   : Comm_App_Frame* from the channel plugin.  Command 0x493e1
//            (300001) carries appdata<SendToSDK>, whose data_ is a packed
//            sec_proto::TSSCaptchaPkg handed to callback_proc_.
//   notify : sec_proto::TSSCaptchaPkg* from the SDK API.  It is packed into
//            RecvDataFromSDKReq.data_ and sent to the secsvr through
//            TssSdkCommSvc::send_buf_to_backend.
//
// No socket/IPC primitive is touched here.  send_buf_to_backend is the only
// backend boundary; get_last_error supplies the message for the combined
// encode/send failure code 0x49511 (300305).

#include <cstring>
#include <ctime>
#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// ---------------------------------------------------------------------------
// External ZEN/plugin boundary: the canonical tag-394 declaration, shared with
// every other reconstructed TU (the ORIG class is declared by
// zen_trace_log_basic.h / zen_trace_log_msg.h, not by this CU).
// ---------------------------------------------------------------------------

#include "src/commlib/zenlib/zen_trace_log_msg.h"


// zen_base64.cpp exports C++-mangled symbols; extern "C" here produced plain
// C imports that do not exist in ORIG.
int base64_encode(const unsigned char *input, unsigned int input_len,
                  char *output, unsigned int *output_len);
int base64_decode(const char *input, unsigned int input_len,
                  unsigned char *output, unsigned int *output_len);

// ---------------------------------------------------------------------------
// DPInterface / DP_CONFIG.  Both are canonical shared declarations:
//   src/formmog/tss_sdk/layout/dp_interface.h  (12 bytes, ORIG dpinterface.cpp)
//   src/closing/dp_config.h                    (280 bytes, ORIG dp_config.cpp)
// The reconstructed definitions live in those TUs; this CU only reads the
// mapped config head.
// ---------------------------------------------------------------------------

// zen_singleton.h: canonical declaration shared by every reconstructed TU.
// The out-of-line weak instance() is owned by tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/dp_interface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/sdk_recv_pkg.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// The ORIG header defines instance() inline, so every plugin CU contains the
// inlined guard sequence (audit/report/init_brief.md: __cxa_guard_acquire,
// TssSdkCommSvc::TssSdkCommSvc(), __cxa_guard_release, __cxa_atexit).
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG.

// ---------------------------------------------------------------------------
// Frame / protocol types.  All wire structures use the DWARF byte layout and
// are packed (the generated TDR pack/unpack implementations are external).
// ---------------------------------------------------------------------------


#include "src/commlib/framework/comm_app_frame.h"

// sec_proto::TSSCaptchaHead/Body/Pkg: the single canonical declaration of the
// wire type (ORIG src/protocol/formmog/formmog_proto_captcha_client.cpp); the
// local copy that split this type away from the protocol TU is gone.
#include "src/origsrc/formmog_proto_captcha_client.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

namespace sec_proto {

// Channel -> SDK direction.  sizeof = 32781; data_ is the wire payload.


// RecvDataFromSDKReq (32785 B) is the shared declaration of
// src/formmog/tss_sdk/plugins/proto_proc/sdk_recv_pkg.h (included above).

}  // namespace sec_proto

namespace tss_sdk_impl {

class TssSdkCaptchaPlugin : public ITssSdkPlugin {
public:
    TssSdkCaptchaPlugin();
    virtual ~TssSdkCaptchaPlugin() {}

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *data);
    virtual int notify(void *data);

private:
    TssSdkCommSvc *sdk_comm_svc_;   // +0x10
};

TssSdkCaptchaPlugin *get_obj_TssSdkCaptchaPlugin();

}  // namespace tss_sdk_impl

using tss_sdk_impl::TssSdkCaptchaPlugin;
using tss_sdk_impl::TssSdkCommSvc;

// ---------------------------------------------------------------------------
// Constructor / destructor / singleton accessor.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

TssSdkCaptchaPlugin::TssSdkCaptchaPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA) {
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkCaptchaPlugin ctor.");
}

TssSdkCaptchaPlugin *get_obj_TssSdkCaptchaPlugin() {
    static TssSdkCaptchaPlugin s_obj_TssSdkCaptchaPlugin;
    return &s_obj_TssSdkCaptchaPlugin;
}

// ---------------------------------------------------------------------------
// init / fini.
// ---------------------------------------------------------------------------

int TssSdkCaptchaPlugin::init(void *init_data) {
    (void)init_data;

    sdk_comm_svc_ = ::Singleton<TssSdkCommSvc>::instance();
    if (sdk_comm_svc_ == 0) {
        std::string message("In CTssSdkCaptchaPlugin::init, invalid args.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4950d;  // 300301
        return error_info_.last_error_info()->error_code;
    }
    return 0;
}

int TssSdkCaptchaPlugin::fini() {
    return 0;
}

// ---------------------------------------------------------------------------
// proc: Comm_App_Frame from the channel plugin.
// ---------------------------------------------------------------------------

int TssSdkCaptchaPlugin::proc(void *data) {
    if (data == 0) {
        std::string message(
            "In CTssSdkCaptchaPlugin::proc, invalid args.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4950d;  // 300301
        return error_info_.last_error_info()->error_code;
    }

    Comm_App_Frame *frame = static_cast<Comm_App_Frame *>(data);
    if (frame->frame_command_ != 0x493e1) {  // 300001
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], invalid cmd=%d, uin %u|appid %u",
            "virtual int tss_sdk_impl::TssSdkCaptchaPlugin::proc(void*)",
            frame->frame_command_, frame->frame_uin_, frame->app_id_);
        return 0x497ca;  // 301002
    }

    sec_proto::SendToSDK send_to_sdk;
    int ret = frame->appdata_decode(send_to_sdk, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "unpack send to sdk pkg failed,uin=%u", frame->frame_uin_);
        return ret;
    }

    sec_proto::TSSCaptchaPkg captcha_pkg;
    ret = captcha_pkg.unpack(
        reinterpret_cast<const char *>(send_to_sdk.data_),
        send_to_sdk.data_len_, 0, 0);
    if (ret != 0) {
        std::string message("Captcha pkg unpack failed.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4950e;  // 300302
        return error_info_.last_error_info()->error_code;
    }

    if (callback_proc_ == 0) {
        // The shipped literal at 0x0021a60c is the suffix of the adjacent
        // "...::proc, m_callback_proc is NULL." string.
        std::string message(
            "TssSdkCaptchaPlugin::proc, m_callback_proc is NULL.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4950d;  // 300301
        return error_info_.last_error_info()->error_code;
    }

    callback_proc_(&captcha_pkg);
    return 0;
}

// ---------------------------------------------------------------------------
// notify: TSSCaptchaPkg from the SDK API -> RecvDataFromSDKReq -> backend.
// ---------------------------------------------------------------------------

int TssSdkCaptchaPlugin::notify(void *data) {
    if (data == 0) {
        error_info_.set_last_error_msg(
            "invalid args. notify_data=%p", (void *)0);
        error_info_.last_error_info()->error_code = 0x4950d;  // 300301
        return error_info_.last_error_info()->error_code;
    }

    sec_proto::TSSCaptchaPkg *captcha_pkg =
        static_cast<sec_proto::TSSCaptchaPkg *>(data);

    // Stack layout matches the shipped function: a 65536-byte frame buffer
    // followed immediately by RecvDataFromSDKReq; the package is packed
    // directly into req.data_ (32767 bytes).
    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    sec_proto::RecvDataFromSDKReq req;

    unsigned int packed_len = 0;
    int ret = captcha_pkg->pack(
        reinterpret_cast<char *>(req.data_), 0x7fff, &packed_len, 0);
    if (ret != 0) {
        std::string message("Captcha pkg pack failed.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4950f;  // 300303
        return error_info_.last_error_info()->error_code;
    }

    req.channel_id_ =
        static_cast<unsigned short>(sdk_comm_svc_->channel_id_);
    req.room_id_ = static_cast<unsigned short>(
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id);
    req.time_stamp_ = static_cast<unsigned int>(time(0));
    req.to_busi_id_ = 6;
    req.from_busi_id_ = 3;
    // req.to_secsvr_seq_ is not written by the shipped function; the local
    // object reaches appdata_encode with this field left as stack garbage.
    req.data_len_ = static_cast<unsigned short>(packed_len);

    Comm_App_Frame *frame =
        reinterpret_cast<Comm_App_Frame *>(trans_send_buffer);
    frame->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4c6ab);
    frame->frame_uin_ = captcha_pkg->head_.uin_;

    SERVICES_ID rcvsvc(0x1f, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x2b, 0);
    frame->recv_service_ = rcvsvc;
    frame->proxy_service_ = proxysvc;
    frame->send_service_ = sndsvc;
    frame->transaction_id_ = 0;
    frame->backfill_trans_id_ = 0;
    frame->app_id_ = tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if (frame->inner_option_.inner_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_framehead(
            frame, "[TRACK MONITOR][TRANS SEND]", RS_INFO);
    }

    ret = frame->appdata_encode(Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA,
                                req, 0, 0);
    int result = 0;
    if (ret != 0) {
        result = 0x139f;  // 5023, app-data encode failure
    } else {
        ret = sdk_comm_svc_->send_buf_to_backend(
            reinterpret_cast<const char *>(frame), frame->frame_length_,
            true, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s]send to backend fail. ret=%d",
                "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
                "unsigned int, unsigned int, short unsigned int) [with PKG = "
                "sec_proto::RecvDataFromSDKReq]",
                ret);
            result = -2;
        }
    }

    if (result != 0) {
        std::string err = sdk_comm_svc_->get_last_error();
        error_info_.set_last_error_msg(err.c_str());
        error_info_.last_error_info()->error_code = 0x49511;  // 300305
        return error_info_.last_error_info()->error_code;
    }
    return 0;
}

}  // namespace tss_sdk_impl

// ORIG static-init block, part 2 (definition above the first use): the weak
// object_creator template static member (ORIG .bss 0x0029d800).  Its
// constructor calls instance(), whose body is then inlined into this TU's
// initializer exactly like the shipped build.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
