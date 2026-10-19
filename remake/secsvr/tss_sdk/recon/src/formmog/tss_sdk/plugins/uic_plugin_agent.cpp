// Semantic reconstruction of tss_sdk_impl::TssSdkUicPlugin from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG translation unit:
//   src/formmog/tss_sdk/plugins/uic/uic_plugin.cpp
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   handle_chat_user_input_local(UserMessage*)  0x000c6fb0-0x000c70e3
//   handle_name_user_input(UserMessage*)        0x000c70f0-0x000c71c4
//   TssSdkUicPlugin() C1                        0x000c71d0-0x000c7276
//   get_obj_TssSdkUicPlugin()                   0x000c7280-0x000c7318
//   TssSdkUicPlugin() C2                        0x000c7320-0x000c73c6
//   proc(void*)                                 0x000c73d0-0x000c74e3
//   fini()                                      0x000c74f0-0x000c7556
//   init(void*)                                 0x000c7560-0x000c7659
//   handle_chat_user_input(UserMessage*)        0x000c7b00-0x000c7f06
//   notify(void*)                               0x000c7f10-0x000c7fee
//   ~TssSdkUicPlugin() D0                       0x000c7ff0-0x000c806e
//   ~TssSdkUicPlugin() D1                       0x000c8070-0x000c80ee
//
// Class layout from DWARF (sizeof == 76):
//   +0x00 vptr
//   +0x04 TssSdkPluginCallbackProc callback_proc_
//   +0x08 CThreadErrorInfo error_info_
//   +0x0c ETssSdkPluginId sdk_plugin_id_
//   +0x10 UicUserInputInfoMgr user_input_mgr_ (sizeof 32)
//   +0x30 ZEN_Thread_Mutex thread_mutex_       (sizeof 28)
//
// UserMessage is a global struct (mangled as P11UserMessage), sizeof == 40:
//   +0x00 uin_                    +0x14 message_ (unsigned char*)
//   +0x04 input_type_ (enum)      +0x18 callback_data_len_ (u16)
//   +0x08 msg_result_flag (enum)  +0x1c callback_data_ (unsigned char*)
//   +0x0c if_replace_ (char)      +0x20 game_data_len_ (u16)
//   +0x10 message_len_ (u32)      +0x24 game_data_ (unsigned char*)

#include <stdint.h>
#include <string.h>
#include <new>
#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
// Canonical tag-394 frame/service and config declarations.
#include "src/commlib/framework/comm_cfgsvr_sdk.h"  // SERVICES_ID, Comm_App_Frame
#include "src/formmog/tss_sdk/plugins/tss_sdk_config_shared.h"
// Canonical tag-394 UIC layouts: TssSdkUicPlugin (76), UicUserInputInfoMgr
// (32), UicUserInputInfo (36), UicTdirty (8), the global MsgResultFlag enum,
// the EUI_INPUT_TYPE enum and the 28-byte UicChatJudgeResult callback payload.
// The local models this TU used to carry are gone.
#include "src/formmog/tss_sdk/layout/tss_sdk_uic.h"



// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// External ZEN/plugin boundary: canonical tag-394 declarations.
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG
// (audit/report/init_brief.md).

#include "src/commlib/zenlib/zen_singleton.h"



// Global UserMessage (uic_plugin.h), 40 bytes.  input_type_ carries the
// canonical EUI_INPUT_TYPE enum and msg_result_flag the global MsgResultFlag;
// both are 4-byte enums, so the layout is unchanged.
struct UserMessage {
    unsigned int uin_;                            // +0
    tss_sdk_impl::EUI_INPUT_TYPE input_type_;     // +4
    MsgResultFlag msg_result_flag;                // +8
    char if_replace_;                             // +12
    unsigned int message_len_;                    // +16
    unsigned char *message_;                      // +20
    unsigned short callback_data_len_;            // +24
    unsigned char *callback_data_;                // +28
    unsigned short game_data_len_;                // +32
    unsigned char *game_data_;                    // +36
};

// sec_proto::UserInputCode / UserInputCheckPkg*: the single canonical
// declaration (ORIG src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp); the
// local copy that split these wire types away from the protocol TU is gone.
#include "src/origsrc/formmog_proto_tss_sdk_uic.h"

// ORIG exports this Comm_App_Frame template instantiation weakly.
template int Comm_App_Frame::appdata_encode<sec_proto::UserInputCheckPkg>(
    unsigned int, const sec_proto::UserInputCheckPkg &, unsigned int,
    size_t *);

// ZEN_Thread_Mutex (28) comes from zen_lock_thread_mutex.h; UicTdirty,
// UicUserInputInfoMgr, UicUserInputInfo, UicChatJudgeResult and
// TssSdkUicPlugin are the canonical declarations of layout/tss_sdk_uic.h.
// The local UicUserInputInfoMgr/UicUserInputInfo/UicTdirty/TssSdkUicPlugin
// models (28-byte UicUserInputInfo, 1-byte UicTdirty) are gone.

// Frame limits are the canonical Comm_App_Frame constants (comm_app_frame.h):
// MAX_LEN_OF_APPFRAME (0x10000) sizes the trans_send_buffer,
// MAX_LEN_OF_APPFRAME_DATA (0xFFBD) is the appdata budget.


namespace tss_sdk_impl {
TssSdkUicPlugin *get_obj_TssSdkUicPlugin();

// ===========================================================================
// Implementation.
// ===========================================================================

TssSdkUicPlugin::TssSdkUicPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_UIC),
      thread_mutex_(0) {
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkUicPlugin ctor.");
}

TssSdkUicPlugin::~TssSdkUicPlugin() {
    // thread_mutex_ then user_input_mgr_ are destroyed by member teardown;
    // ORIG order is mutex (+0x30) -> manager (+0x10) -> base (+0x00).
}

int TssSdkUicPlugin::handle_chat_user_input_local(UserMessage *user_message) {
    if (user_message == 0 || user_message->message_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "TssSdkUicPlugin::handle_chat_user_input_local invalid args\n");
        return -1;
    }

    MsgResultFlag result = EMSG_NORMAL_FLAG;
    if (UicTdirty::instance()->uic_dirty_check(user_message->message_,
                                               &result) != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::handle_chat_user_input_local "
            "uic_dirty_check failed");
        // ORIG logs and continues.
    }

    if (result == EMSG_DIRTY_FLAG) {
        if (UicTdirty::instance()->uic_dirty_replace(
                user_message->message_) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]TssSdkUicPlugin::handle_chat_user_input_local "
                "uic_dirty_replace failed");
            // ORIG logs and continues.
        }
    }

    // The 28-byte UicChatJudgeResult (canonical declaration in
    // layout/tss_sdk_uic.h) is the payload the shipped code hands to
    // callback_proc_; it is not the 36-byte shared-memory UicUserInputInfo.
    UicChatJudgeResult info;
    info.uin_ = user_message->uin_;
    info.msg_result_flag_ = result;
    info.message_len_ = user_message->message_len_;
    info.message_ = user_message->message_;
    info.callback_data_len_ = user_message->callback_data_len_;
    info.callback_data_ = user_message->callback_data_;
    info.input_type_ = user_message->input_type_;

    if (callback_proc_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::handle_chat_user_input_local "
            "callback_proc_ == NULL");
        return 0;
    }

    int rc = callback_proc_(&info);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::handle_chat_user_input_local "
            "callback_proc_ failed");
    }
    return 0;
}

int TssSdkUicPlugin::handle_name_user_input(UserMessage *user_message) {
    if (user_message == 0 || user_message->message_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "TssSdkUicPlugin::handle_name_user_input invalid args\n");
        return -1;
    }

    MsgResultFlag result = EMSG_NORMAL_FLAG;
    if (UicTdirty::instance()->uic_dirty_check(user_message->message_,
                                               &result) != 0) {
        ZEN_Trace_LogMsg::debug_errorex("uic_dirty_check failed\n");
        return -1;
    }

    if (result == EMSG_DIRTY_FLAG && user_message->if_replace_ != 0) {
        if (UicTdirty::instance()->uic_dirty_replace(
                user_message->message_) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]TssSdkUicPlugin::handle_name_user_input "
                "UicTdirty::instance()->uic_dirty_replace failed");
            return -1;
        }
    }

    user_message->msg_result_flag = result;
    return 0;
}

int TssSdkUicPlugin::init(void *init_data) {
    (void)init_data;

    thread_mutex_.lock();

    unsigned char *dirty_file = 0;
    if ((unsigned int)g_config.uic_config_.use_dirty_file_flag_ >= 1)
        dirty_file = (unsigned char *)g_config.uic_config_.dirty_file_;

    int ret = UicTdirty::instance()->init(
        (char *)dirty_file, g_config.uic_config_.bus_id_,
        (sec_proto::UserInputCode)g_config.uic_config_.user_input_code_);

    int result;
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::init "
            "UicTdirty::instance()->init failed");
        result = -1;
    } else {
        result = 0;
        ret = user_input_mgr_.init(0x4b0, callback_proc_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]TssSdkUicPlugin::init "
                "user_input_mgr_.init failed");
            result = -1;
        }
    }

    thread_mutex_.unlock();
    return result;
}

int TssSdkUicPlugin::fini() {
    thread_mutex_.lock();
    user_input_mgr_.fini();
    thread_mutex_.unlock();
    return 0;
}

int TssSdkUicPlugin::proc(void *proc_data) {
    thread_mutex_.lock();

    if (g_config.uic_config_.use_backend_check_ == 0) {
        thread_mutex_.unlock();
        return 0;
    }

    user_input_mgr_.expire_user_input();

    if (proc_data != 0) {
        Comm_App_Frame *head = (Comm_App_Frame *)proc_data;
        sec_proto::UserInputCheckPkg check_result;

        int ret = check_result.unpack(
            (const char *)head + 0x32, head->frame_length_ - 0x32, 0, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]TssSdkUicPlugin::proc "
                "check_result.unpack failed, ret = %d", ret);
            thread_mutex_.unlock();
            return -1;
        }

        ret = user_input_mgr_.on_recv_check_result(&check_result);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]TssSdkUicPlugin::proc "
                "user_input_mgr_.on_recv_check_result failed, ret = %d", ret);
            thread_mutex_.unlock();
            return -1;
        }
    }

    thread_mutex_.unlock();
    return 0;
}

int TssSdkUicPlugin::handle_chat_user_input(UserMessage *user_message) {
    if (user_message == 0 || user_message->message_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "TssSdkUicPlugin::handle_chat_user_input invalid args\n");
        return -1;
    }

    if (user_input_mgr_.get_user_num() > 0x4af ||
        g_config.uic_config_.use_backend_check_ == 0) {
        return handle_chat_user_input_local(user_message);
    }

    unsigned int seq = 0;
    int ret = user_input_mgr_.add_user_input(user_message, &seq);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::handle_chat_user_input "
            "user_input_mgr_.add_user_input failed, uin = %u",
            user_message->uin_);
        return handle_chat_user_input_local(user_message);
    }

    // NOTE: proto_id_, pkg_len_ and game_id_ are NOT initialized by ORIG;
    // only the fields below are written before appdata_encode<> packs the
    // whole structure.  The uninitialized bytes are preserved as-is.
    sec_proto::UserInputCheckPkg user_input_pkg;
    user_input_pkg.head_.version_ = 1;
    user_input_pkg.head_.cmd_ = 1;

    sec_proto::ReqUserInputCheck &req =
        user_input_pkg.body_.req_user_input_check_;
    req.uin_ = user_message->uin_;
    req.session_id_ = 0;
    req.session_seq_ = seq;
    req.user_input_code_ = g_config.uic_config_.user_input_code_;

    unsigned int message_len = user_message->message_len_;
    if (user_message->message_[message_len - 1] != 0) {
        unsigned short data_len = (unsigned short)(message_len + 1);
        req.user_input_data_len_ = data_len;
        memcpy(req.user_input_data_, user_message->message_, message_len);
        req.user_input_data_[message_len] = 0;
    } else {
        req.user_input_data_len_ = (unsigned short)message_len;
        memcpy(req.user_input_data_, user_message->message_, message_len);
    }

    req.user_game_data_len_ = user_message->game_data_len_;
    memcpy(req.user_game_data_, user_message->game_data_,
           user_message->game_data_len_);

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

    if ((rsp_msg->frame_option_ & 0x10000) != 0)
        rsp_msg->dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, user_input_pkg, 0, 0);
    if (ret == 0) {
        TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();
        ret = comm_svc->send_buf_to_backend(
            (const char *)trans_send_buffer, rsp_msg->frame_length_, false, 6);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s]send to backend fail. ret=%d",
                "int tss_sdk_impl::TssSdkCommSvc::send_socket_pkg(int, "
                "const PKG&) [with PKG = sec_proto::UserInputCheckPkg]", ret);
        }
    }

    return 0;
}

int TssSdkUicPlugin::notify(void *notify_data) {
    if (notify_data == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::notify invalid args");
        return -1;
    }

    UserMessage *user_message = (UserMessage *)notify_data;
    switch (user_message->input_type_) {
    case EUITYPE_CHAT: {
        thread_mutex_.lock();
        int rc = handle_chat_user_input(user_message);
        thread_mutex_.unlock();
        return rc;
    }
    case EUITYPE_NAME:
        return handle_name_user_input(user_message);
    case EUITYPE_IMAGE:
    case EUITYPE_VIDEO:
        return -1;
    default:
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]TssSdkUicPlugin::notify unknown input type");
        return -1;
    }
}

TssSdkUicPlugin *get_obj_TssSdkUicPlugin() {
    static TssSdkUicPlugin s_obj_TssSdkUicPlugin;
    return &s_obj_TssSdkUicPlugin;
}

}  // namespace tss_sdk_impl

// ORIG static-init block, part 2 (definition above the first use): the weak
// object_creator template static member (ORIG .bss 0x0029d800).
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
