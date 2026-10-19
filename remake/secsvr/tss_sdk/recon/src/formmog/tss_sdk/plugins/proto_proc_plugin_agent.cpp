// Semantic reconstruction of tss_sdk_impl::TssSdkProtoProcPlugin from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG translation unit:
//   src/formmog/tss_sdk/plugins/proto_proc/proto_proc_plugin.cpp
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   fini()                                          0x000c1150-0x000c1157
//   handle_notify_pkg(NotifyPkg*)                   0x000c1160-0x000c116f
//   proc_special_antibot_pkg(u8*,u16,u32)           0x000c1250-0x000c145d
//   handle_decrypt_pkg_old(DpPkg*)                  0x000c1460-0x000c14cc
//   recv_buffer_for_encrypt(DownPkg*)               0x000c14d0-0x000c1543
//   recv_buffer_for_encrypt_old(DownPkg*)           0x000c1550-0x000c15bc
//   is_need_encrypt(DownListPkg*)                   0x000c15c0-0x000c161b
//   is_need_decrypt(DecryptListPkg*)                0x000c1620-0x000c167c
//   TssSdkProtoProcPlugin() C1                      0x000c1680-0x000c16f0
//   get_obj_TssSdkProtoProcPlugin()                 0x000c1710-0x000c17a5
//   TssSdkProtoProcPlugin() C2                      0x000c17b0-0x000c1820
//   proc_forward_data_to_client_pkg(void*)          0x000c1e00-0x000c216b
//   proc(void*)                                     0x000c2170-0x000c2237
//   is_cheating_pkg(AntiCheatingPkg*)               0x000c2240-0x000c2582
//   handle_antibot_pkg(u32,u8 const*,u32)           0x000c2590-0x000c3538
//   handle_decrypt_pkg(DpPkg*)                      0x000c3540-0x000c3715
//   notify(void*)                                   0x000c3720-0x000c3c0f
//   init_dpsdk()                                    0x000c3c10-0x000c42ef
//   init(void*)                                     0x000c42f0-0x000c43af
//   ~TssSdkProtoProcPlugin() D0                     0x000c43b0-0x000c43ee
//   ~TssSdkProtoProcPlugin() D1                     0x000c43f0-0x000c4420
//
// Class layout from DWARF (sizeof == 32):
//   +0x00 vptr, +0x04 callback_proc_, +0x08 error_info_,
//   +0x0c ETssSdkPluginId
//   +0x10 unsigned int m_channel_id
//   +0x14 TssSdkCommSvc* m_sdk_comm_svc
//   +0x18 unsigned int m_recv_buf_seq
//   +0x1c unsigned int m_notify_svr_seq
// Static DPInterface* m_dp_if lives at 0x002c22a0.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <new>
#include <sstream>
#include <string>
// Canonical tag-394 declarations.  The former local ZEN_Time_Value,
// DPInterface, UserInfo, DP_CONFIG, SERVICES_ID, Comm_App_Frame,
// ZEN_Server_Status/Comm_Stat_Monitor, sec_proto packet and plugin wrapper
// models are replaced by these single shared definitions.
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

#include "src/commlib/framework/comm_cfgsvr_sdk.h"  // SERVICES_ID, Comm_App_Frame
#include "src/commlib/zenlib/zen_recon_support.h"   // ZEN_Time_Value
#include "src/commlib/zenlib/zen_singleton.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/comm_stat_monitor.h"
#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"  // CommBillRecord
#include "src/formmog/tss_sdk/layout/dp_bill.h"           // DPBillAdapter
#include "src/formmog/tss_sdk/layout/dp_interface.h"
#include "src/formmog/tss_sdk/layout/tss_sdk_pkgs.h"
#include "src/formmog/tss_sdk/layout/user_info.h"
#include "src/protocol/formmog/formmog_proto_antibot_client.h"
#include "src/protocol/formmog/formmog_proto_public_secsvr.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// Channel stat table carried by this CU as well (same five named counters and
// four default-constructed entries as channel_plugin_agent.cpp; the ORIG init
// block has 21 named + 1 default ZEN_STATUS_ITEM_WITHNAME constructions).
static ZEN_STATUS_ITEM_WITHNAME STR_TSSSDK_STAT[9] = {
    ZEN_STATUS_ITEM_WITHNAME(0x68fb1, STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_RECV_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x68fb2, STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_RECV_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x68fb3, STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_SEND_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x68fb4, STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_SEND_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x68fb9, STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_ANTIBOT_LOST_PKG_NUM")
};


// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// DPBillAdapter is the canonical declaration of layout/dp_bill.h (sizeof 4,
// its static DPBillRecorder bill_ at 0x002b6e00 defined by
// dp_bill_monitor_agent.cpp).  The local 1-member/CommBillRecord model this TU
// used to carry is gone.
unsigned int Crc32(unsigned char *buf, unsigned int len);

// ORIG exports these Comm_App_Frame template instantiations weakly; the
// canonical header defines the body inline but the host build uses
// -fno-implement-inlines, so this TU (which uses the packets) emits them.
template int Comm_App_Frame::appdata_encode<sec_proto::RecvDataFromSDKReq>(
    unsigned int, const sec_proto::RecvDataFromSDKReq &, unsigned int,
    size_t *);
template int Comm_App_Frame::appdata_encode<sec_proto::ReportPkgTime>(
    unsigned int, const sec_proto::ReportPkgTime &, unsigned int, size_t *);
template int Comm_App_Frame::appdata_encode<sec_proto::ReportLostAntibotPkg>(
    unsigned int, const sec_proto::ReportLostAntibotPkg &, unsigned int,
    size_t *);
template int Comm_App_Frame::appdata_encode<sec_proto::AddUserReq>(
    unsigned int, const sec_proto::AddUserReq &, unsigned int, size_t *);
template int Comm_App_Frame::appdata_encode<sec_proto::DelUserReq>(
    unsigned int, const sec_proto::DelUserReq &, unsigned int, size_t *);
template int Comm_App_Frame::appdata_decode<sec_proto::SendToSDK>(
    sec_proto::SendToSDK &, unsigned int, size_t *) const;
// tss_sdk_plugin_proto_proc.h message ids (the canonical layout header only
// declares the packet types, not this enum).
enum ETssSdkPluginProtoProcMsgId {
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG = 201,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG = 202,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG = 203,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST = 304,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST = 305,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ON_NOTIFY_SERVER = 306,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD = 307,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD = 308,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG = 309
};

// DPInterDataHead (5 bytes) is declared by recon/src/origsrc/
// formmog_proto_tss_sdk_client.h, but that header's private TDR runtime
// conflicts with src/closing/dp_config.h's copy.  This TU only reads the
// three fields, so the DWARF-exact declaration lives here; reported as a
// header-integration gap rather than pulling two TDR runtimes together.
namespace sec_proto {
struct DPInterDataHead {
    uint16_t packet_size_;   // +0
    uint16_t protocol_type_; // +2
    uint8_t data_type_;      // +4, sizeof 5

    int unpack(const char *buf, unsigned int buf_len,
               unsigned int *used_len, unsigned int cut_version);
} __attribute__((packed));
}  // namespace sec_proto

// Frame limits are the canonical Comm_App_Frame constants (comm_app_frame.h):
// MAX_LEN_OF_APPFRAME (0x10000) sizes the trans_send_buffer,
// MAX_LEN_OF_APPFRAME_DATA (0xFFBD) is the appdata budget.



// ---------------------------------------------------------------------------
// TssSdkProtoProcPlugin, sizeof == 32.
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {

class TssSdkProtoProcPlugin : public ITssSdkPlugin {
public:
    TssSdkProtoProcPlugin();
    virtual ~TssSdkProtoProcPlugin();

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *proc_data);
    virtual int notify(void *notify_data);

    int init_dpsdk();
    int is_need_decrypt(TssSdkPluginProtoProcDecryptListPkg *pkg);
    int is_need_encrypt(TssSdkPluginProtoProcDownListPkg *pkg);
    int recv_buffer_for_encrypt(TssSdkPluginProtoProcDownPkg *pkg);
    int recv_buffer_for_encrypt_old(TssSdkPluginProtoProcDownPkg *pkg);
    int handle_notify_pkg(TssSdkPluginProtoProcNotifyPkg *pkg);
    int is_cheating_pkg(TssSdkPluginProtoProcAntiCheatingPkg *pkg);
    int handle_antibot_pkg(unsigned int uin, const unsigned char *input_buf,
                           unsigned int input_buf_size);
    int handle_decrypt_pkg(TssSdkPluginProtoProcDpPkg *pkg);
    int handle_decrypt_pkg_old(TssSdkPluginProtoProcDpPkg *pkg);
    int proc_forward_data_to_client_pkg(void *proc_data);
    int proc_special_antibot_pkg(unsigned char *antibot_data,
                                 unsigned short data_len,
                                 unsigned int buf_len);

    static DPInterface *m_dp_if;  // 0x002c22a0

private:
    unsigned int m_channel_id;          // +0x10
    TssSdkCommSvc *m_sdk_comm_svc;      // +0x14
    unsigned int m_recv_buf_seq;        // +0x18
    unsigned int m_notify_svr_seq;      // +0x1c
};

DPInterface *TssSdkProtoProcPlugin::m_dp_if = 0;

TssSdkProtoProcPlugin *get_obj_TssSdkProtoProcPlugin();

// ===========================================================================
// Implementation.
// ===========================================================================

TssSdkProtoProcPlugin::TssSdkProtoProcPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC) {
    m_channel_id = 0;
    m_sdk_comm_svc = 0;
    m_recv_buf_seq = 0;
    m_notify_svr_seq = 0;
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkProtoProcPlugin ctor.");
}

TssSdkProtoProcPlugin::~TssSdkProtoProcPlugin() {
    // ORIG performs no member cleanup.
}

int TssSdkProtoProcPlugin::fini() {
    return 0;
}

int TssSdkProtoProcPlugin::init(void *init_data) {
    (void)init_data;

    TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();
    m_sdk_comm_svc = comm_svc;
    m_channel_id = comm_svc->channel_id_;
    m_dp_if = comm_svc->dp_if_;

    int rc = init_dpsdk();
    return rc == 0 ? 0 : -1;
}

int TssSdkProtoProcPlugin::init_dpsdk() {
    std::ostringstream channel_id_str;
    channel_id_str << m_channel_id;

    std::string ipc_name = channel_id_str.str();
    int rc = m_dp_if->init(ipc_name.c_str());
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "in TssSdkProtoProcPlugin::init_dpsdk, m_dp_if->init failed");
        return -1;
    }
    return 0;
}

int TssSdkProtoProcPlugin::handle_notify_pkg(
    TssSdkPluginProtoProcNotifyPkg *pkg) {
    pkg->notify_result_ = 0;
    return 0;
}

int TssSdkProtoProcPlugin::handle_decrypt_pkg_old(
    TssSdkPluginProtoProcDpPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);
    pkg->decrypt_result_ = m_dp_if->recv_buffer_for_decrypt(
        user, (unsigned char *)pkg->input_buf_, pkg->input_buf_size_,
        pkg->output_buf_, pkg->output_buf_size_);
    return 0;
}

int TssSdkProtoProcPlugin::recv_buffer_for_encrypt(
    TssSdkPluginProtoProcDownPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);
    pkg->encrypt_result_ = m_dp_if->recv_buffer_for_encrypt(
        user, pkg->cmd_id_, (unsigned char *)pkg->input_buf_,
        pkg->input_buf_size_, pkg->output_buf_, pkg->output_buf_size_);
    return 0;
}

int TssSdkProtoProcPlugin::recv_buffer_for_encrypt_old(
    TssSdkPluginProtoProcDownPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);
    pkg->encrypt_result_ = m_dp_if->recv_buffer_for_encrypt(
        user, (unsigned char *)pkg->input_buf_, pkg->input_buf_size_,
        pkg->output_buf_, pkg->output_buf_size_);
    return 0;
}

int TssSdkProtoProcPlugin::is_need_encrypt(
    TssSdkPluginProtoProcDownListPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);
    pkg->is_in_down_list_ =
        m_dp_if->is_need_encrypt(user, pkg->cmd_id_) ? 1 : 0;
    return 0;
}

int TssSdkProtoProcPlugin::is_need_decrypt(
    TssSdkPluginProtoProcDecryptListPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);
    bool result = m_dp_if->is_need_decrypt(user, pkg->cmd_id_);
    // ORIG computes `cmp al,1; sbb eax,eax`: false -> -1, true -> 0.
    pkg->check_result_ = result ? 0 : -1;
    return 0;
}

int TssSdkProtoProcPlugin::proc_special_antibot_pkg(
    unsigned char *antibot_data, unsigned short data_len,
    unsigned int buf_len) {
    uint16_t antibot_cmd =
        (uint16_t)(((uint16_t)antibot_data[0] << 8) | antibot_data[1]);

    if ((uint16_t)(antibot_cmd - 0x78) > 1)
        return 0;

    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack((const char *)antibot_data, data_len, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], unpack antibot pkg fail, ret=%d, cmd=%d",
            "int tss_sdk_impl::TssSdkProtoProcPlugin::"
            "proc_special_antibot_pkg(unsigned char*, short unsigned int, "
            "unsigned int)", ret, (int)antibot_cmd);
        return ret;
    }

    ZEN_Time_Value now;
    now.gettimeofday();

    if (antibot_cmd != 0x79) {
        unsigned long long now_msec = (unsigned long long)now.total_msec();
        ZEN_Trace_LogMsg::debug_debugEx(
            "proc special antibot sc pkg succ, cmd=%d, "
            "secsvr_down_time=%llu, sdk_down_time=%llu",
            (int)antibot_cmd,
            (unsigned long long)antibot_pkg.body_.sc_channel_delay_check_.
                secsvr_down_time_,
            (unsigned long long)antibot_pkg.body_.sc_channel_delay_check_.
                sdk_down_time_);
        (void)now_msec;
    } else {
        unsigned long long now_msec = (unsigned long long)now.total_msec();
        antibot_pkg.body_.cs_channel_delay_check_.sdk_up_time_ = now_msec;
        ZEN_Trace_LogMsg::debug_debugEx(
            "proc special antibot cs pkg succ, cmd=%d, "
            "secsvr_down_time=%llu, sdk_down_time=%llu, "
            "client_up_time=%u, sdk_up_time=%llu",
            (int)antibot_cmd,
            (unsigned long long)antibot_pkg.body_.cs_channel_delay_check_.
                secsvr_down_time_,
            (unsigned long long)antibot_pkg.body_.cs_channel_delay_check_.
                sdk_down_time_,
            antibot_pkg.body_.cs_channel_delay_check_.client_up_time_,
            now_msec);
    }

    ret = antibot_pkg.pack((char *)antibot_data, buf_len, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], unpack antibot pkg fail, ret=%d, cmd=%d",
            "int tss_sdk_impl::TssSdkProtoProcPlugin::"
            "proc_special_antibot_pkg(unsigned char*, short unsigned int, "
            "unsigned int)", ret, (int)antibot_cmd);
        return ret;
    }
    return 0;
}

int TssSdkProtoProcPlugin::proc(void *proc_data) {
    if (proc_data == 0)
        return 0;

    Comm_App_Frame *app_frame = (Comm_App_Frame *)proc_data;
    unsigned int cmd = app_frame->frame_command_;

    if (cmd == 0x493e5 || cmd == 0x493e7 || cmd == 0x493ea) {
        UserInfo *user = m_sdk_comm_svc->get_user_info_by_uin(
            app_frame->frame_uin_);
        if (user != 0) {
            m_dp_if->on_recv_msg(user, app_frame);
        } else {
            ZEN_Trace_LogMsg::debug_errorex(
                "in RSP_TSSSDK_DP_*, user is not exist, uin = %u",
                app_frame->frame_uin_);
        }
        return 0;
    }

    if (cmd == 0x493e1) {
        proc_forward_data_to_client_pkg(app_frame);
        return 0;
    }

    ZEN_Trace_LogMsg::debug_errorex(
        "%s, invalid cmd %u",
        "virtual int tss_sdk_impl::TssSdkProtoProcPlugin::proc(void*)", cmd);
    return 0;
}

int TssSdkProtoProcPlugin::proc_forward_data_to_client_pkg(void *proc_data) {
    Comm_App_Frame *app_frame = (Comm_App_Frame *)proc_data;
    sec_proto::SendToSDK to_sdk_pkg;
    char err_str[256];

    if (app_frame->appdata_decode(to_sdk_pkg, 0, 0) != 0) {
        snprintf(err_str, 0xff,
                 "unpack forward data to client pkg failed. uin %u|appid %u.",
                 app_frame->frame_uin_, app_frame->app_id_ & 0xffff);
        error_info_.set_last_error_msg("%s", err_str);
        error_info_.last_error_info()->error_code = 0x494b3;
        return error_info_.last_error_info()->error_code;
    }

    unsigned short busi_id = to_sdk_pkg.to_busi_id_;
    if (busi_id == 2) {
        UserInfo *user = m_sdk_comm_svc->get_user_info_by_uin(
            app_frame->frame_uin_);
        if (user != 0)
            m_dp_if->on_recv_msg(user, app_frame);
        else
            ZEN_Trace_LogMsg::debug_errorex(
                "in TssSdkProtoProcPlugin::proc_forward_data_to_client_pkg, "
                "get_user_info_by_uin failed, uin = %u",
                app_frame->frame_uin_);
    } else if (busi_id != 0xa && busi_id != 1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], invalid busi id %u, cmd %u, uin %u|appid %u.",
            "int tss_sdk_impl::TssSdkProtoProcPlugin::"
            "proc_forward_data_to_client_pkg(void*)",
            (unsigned int)busi_id, app_frame->frame_command_,
            app_frame->frame_uin_, app_frame->app_id_ & 0xffff);
        return 0;
    }

    unsigned int transed_uin = app_frame->frame_uin_;
    unsigned int game_id =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
    if (m_sdk_comm_svc->is_need_trans_uin(game_id)) {
        UserInfo *user = m_sdk_comm_svc->get_user_info_by_uin(
            app_frame->frame_uin_);
        if (user == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "in TssSdkProtoProcPlugin::proc_forward_data_to_client_pkg, "
                "get_user_info_by_uin failed, uin = %u", transed_uin);
            return -1;
        }
        transed_uin = user->base_info_.game_usr_id_;
        ZEN_Trace_LogMsg::debug_debugEx(
            "in TssSdkProtoProcPlugin::proc_forward_data_to_client_pkg, "
            "get_user_info_by_uin success, uin = %u", app_frame->frame_uin_);
    }

    proc_special_antibot_pkg(to_sdk_pkg.data_, to_sdk_pkg.data_len_,
                             0x7fff);

    int rc = m_sdk_comm_svc->send_buf_to_client(
        0, transed_uin, to_sdk_pkg.data_, (short)to_sdk_pkg.data_len_);
    if (rc != 0) {
        snprintf(err_str, 0xff,
                 "In CTssSdkProtoProcPlugin::proc, "
                 "proc send msg to client pkg failed. uin %u|appid %u.",
                 app_frame->frame_uin_, app_frame->app_id_ & 0xffff);
        error_info_.set_last_error_msg("%s", err_str);
        error_info_.last_error_info()->error_code = 0x4963c;
        return error_info_.last_error_info()->error_code;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "in TssSdkProtoProcPlugin::proc_forward_data_to_client_pkg, "
        "uin = %u, frame_command_ = %u, frame_length_ = %u, app_id_ = %u, "
        "send_service(%u, %u)",
        transed_uin, app_frame->frame_command_, app_frame->frame_length_,
        app_frame->app_id_ & 0xffff,
        (unsigned int)app_frame->send_service_.services_type_,
        app_frame->send_service_.services_id_);
    return 0;
}

int TssSdkProtoProcPlugin::is_cheating_pkg(
    TssSdkPluginProtoProcAntiCheatingPkg *pkg) {
    UserInfo *user = m_sdk_comm_svc->get_user_info(pkg->uin_, pkg->idx_);

    if (user != 0) {
        unsigned int now_time =
            tss_dp_comm::DP_CONFIG::glb_dp_config_head_->now_time;
        ZEN_Trace_LogMsg::debug_debugEx(
            "is cheating pkg uin=%u,now_time=%u", pkg->uin_, now_time);

        const unsigned int REPORT_TIME_GAP = 60;
        unsigned int elapsed =
            (unsigned int)(now_time - user->user_stat_.last_report_time_);
        if (elapsed > REPORT_TIME_GAP - 1) {
            sec_proto::ReportPkgTime report_pkg;
            report_pkg.channel_id_ = (uint16_t)m_channel_id;
            report_pkg.room_id_ = (uint16_t)
                tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
            report_pkg.last_report_time_ =
                user->user_stat_.last_report_time_;
            report_pkg.now_report_time_ = now_time;
            report_pkg.now_cmd_ = pkg->cmd_id_;

            user->user_stat_.last_report_time_ = now_time;

            unsigned char trans_send_buffer[
                Comm_App_Frame::MAX_LEN_OF_APPFRAME];
            Comm_App_Frame *rsp_msg =
                (Comm_App_Frame *)trans_send_buffer;

            rsp_msg->init_framehead(
                Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4bafd);
            SERVICES_ID rcvsvc(0x1f, 0);
            SERVICES_ID proxysvc(0, 0);
            SERVICES_ID sndsvc(0x2b, 0);

            rsp_msg->frame_uin_ = pkg->uin_;
            rsp_msg->transaction_id_ = 0;
            rsp_msg->recv_service_ = rcvsvc;
            rsp_msg->proxy_service_ = proxysvc;
            rsp_msg->send_service_ = sndsvc;
            rsp_msg->backfill_trans_id_ = 0;
            rsp_msg->app_id_ =
                tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

            if ((rsp_msg->frame_option_ & 0x10000) != 0)
                rsp_msg->dumpoutput_framehead(
                    rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

            int rc = rsp_msg->appdata_encode(
                Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, report_pkg, 0, 0);
            if (rc == 0) {
                rc = m_sdk_comm_svc->send_buf_to_backend(
                    (const char *)trans_send_buffer,
                    rsp_msg->frame_length_, true, 0);
                if (rc != 0) {
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[%s]send to backend fail. ret=%d",
                        "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg("
                        "const PKG&, unsigned int, unsigned int, "
                        "short unsigned int) "
                        "[with PKG = sec_proto::ReportPkgTime]", rc);
                    rc = -2;
                }
            } else {
                rc = 0x139f;
            }

            ZEN_Trace_LogMsg::debug_debugEx(
                "send report pkg time fail, rc=%d, uin=%u", rc, pkg->uin_);
        }
    }

    pkg->is_cheating_pkg_ = m_dp_if->anticheck_pkg(
        user, pkg->cmd_id_, (unsigned char *)pkg->game_pkg_,
        pkg->game_pkg_len_);
    return 0;
}

int TssSdkProtoProcPlugin::handle_decrypt_pkg(
    TssSdkPluginProtoProcDpPkg *pkg) {
    unsigned int use_len = 0;
    sec_proto::DPInterDataHead head;

    int ret = head.unpack((const char *)pkg->input_buf_,
                          pkg->input_buf_size_, &use_len, 0);
    if (ret != 0) {
        pkg->decrypt_result_ = -1;
        if (ret < 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "in TssSdkProtoProcPlugin::handle_dp_pkg, "
                "head.unpack failed, uin = %u", pkg->uin_);
            error_info_.set_last_error_msg(
                "In CTssSdkProtoProcPlugin::handle_dp_pkg, invalid dp pkg.");
            error_info_.last_error_info()->error_code = 0x49645;
            return error_info_.last_error_info()->error_code;
        }
    }

    if (head.data_type_ == 1) {
        handle_antibot_pkg(pkg->uin_,
                           (const unsigned char *)pkg->input_buf_ + use_len,
                           pkg->input_buf_size_ - use_len);
        pkg->decrypt_result_ = 1;
        error_info_.last_error_info()->error_code = 0;
        return error_info_.last_error_info()->error_code;
    }

    ZEN_Trace_LogMsg::debug_debugEx("recv dp pkg. cmd=%d uin=%u",
                                    (int)head.protocol_type_, pkg->uin_);
    return handle_decrypt_pkg_old(pkg);
}

int TssSdkProtoProcPlugin::handle_antibot_pkg(
    unsigned int uin, const unsigned char *input_buf,
    unsigned int input_buf_size) {
    TssSdkCommSvc *comm_svc = m_sdk_comm_svc;
    UserInfo *user = comm_svc->get_user_info_by_uin(uin);

    if (user == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], user %u is not exist",
            "int tss_sdk_impl::TssSdkProtoProcPlugin::handle_antibot_pkg("
            "unsigned int, const unsigned char*, unsigned int)", uin);
        return 0;
    }

    if (input_buf_size > 0x7fff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "recv antibot msg too long. recv_len=%u max_size=%u",
            input_buf_size, 0x7fff);
        return -1;
    }

    sec_proto::RecvDataFromSDKReq recv_pkg;
    recv_pkg.channel_id_ = (uint16_t)m_channel_id;
    recv_pkg.room_id_ = (uint16_t)
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
    recv_pkg.time_stamp_ = (uint32_t)time(0);
    recv_pkg.to_busi_id_ = 1;
    recv_pkg.from_busi_id_ = 1;
    recv_pkg.data_len_ = (uint16_t)input_buf_size;
    memcpy(recv_pkg.data_, input_buf, input_buf_size);

    unsigned int last_seq = user->user_stat_.antibot_last_seq_;
    unsigned int now_seq = ++user->user_stat_.send_to_secsvr_seq_;
    recv_pkg.to_secsvr_seq_ = now_seq;

    proc_special_antibot_pkg(recv_pkg.data_, recv_pkg.data_len_,
                             0x7fff);

    // ORIG updates antibot_last_seq_ to last_seq + 1 and computes the loss
    // count from that value, not from now_seq; as a result lost_num is
    // always zero.  The code is preserved exactly, including the
    // (normally unreachable) loss-report branch.
    unsigned int next_last = last_seq + 1;
    user->user_stat_.antibot_last_seq_ = next_last;
    int lost_num = (int)((next_last - last_seq) - 1);
    bool lost_pkg = false;

    if (lost_num != 0) {
        unsigned int crc32_value = Crc32(
            (unsigned char *)recv_pkg.data_, recv_pkg.data_len_);
        uint16_t antibot_cmd = (uint16_t)(
            ((uint16_t)recv_pkg.data_[0] << 8) | recv_pkg.data_[1]);

        tss_dpsdk::DPBillAdapter bill("antibot lost pkg", 0x41, user);
        tss_dpsdk::DPBillAdapter::bill_ << (long long)antibot_cmd;
        tss_dpsdk::DPBillAdapter::bill_ << (long long)last_seq;
        tss_dpsdk::DPBillAdapter::bill_ << (long long)next_last;
        tss_dpsdk::DPBillAdapter::bill_ << (long long)lost_num;
        tss_dpsdk::DPBillAdapter::bill_ << (long long)crc32_value;

        lost_pkg = true;

        if (lost_num > 0) {
            unsigned int game_id =
                tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x68fb9, game_id, 0, (long long)lost_num);
        }
    }

    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg = (Comm_App_Frame *)trans_send_buffer;

        rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4c6ab);
    SERVICES_ID rcvsvc(0x1f, 0);
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

    if ((rsp_msg->frame_option_ & 0x10000) != 0)
        rsp_msg->dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int rc = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, recv_pkg, 0, 0);
    if (rc == 0) {
        rc = comm_svc->send_buf_to_backend((const char *)trans_send_buffer,
                                           rsp_msg->frame_length_, true, 0);
        if (rc != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s]send to backend fail. ret=%d",
                "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
                "unsigned int, unsigned int, short unsigned int) "
                "[with PKG = sec_proto::RecvDataFromSDKReq]", rc);
            rc = -2;
        }
    } else {
        rc = 0x139f;
    }

    if (rc != 0) {
        std::ostringstream err_str_stream;
        err_str_stream
            << "In CTssSdkProtoProcPlugin::handle_antibot_pkg, "
               "send_ipc_pkg failed, rc is "
            << rc << "." << comm_svc->get_last_error();
        error_info_.set_last_error_msg("%s", err_str_stream.str().c_str());
        error_info_.last_error_info()->error_code = 0x4964b;
        return error_info_.last_error_info()->error_code;
    }

    if (lost_pkg) {
        sec_proto::ReportLostAntibotPkg lost_pkg_req;
        lost_pkg_req.channel_id_ = (uint16_t)m_channel_id;
        lost_pkg_req.room_id_ = (uint16_t)
            tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
        lost_pkg_req.last_seq_ = last_seq;
        lost_pkg_req.now_seq_ = next_last;

        Comm_App_Frame *lost_msg = (Comm_App_Frame *)trans_send_buffer;
        lost_msg->init_framehead(
            Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4bafb);

        SERVICES_ID rcvsvc2(0x1f, 0);
        SERVICES_ID proxysvc2(0, 0);
        SERVICES_ID sndsvc2(0x2b, 0);

        lost_msg->frame_uin_ = uin;
        lost_msg->transaction_id_ = 0;
        lost_msg->recv_service_ = rcvsvc2;
        lost_msg->proxy_service_ = proxysvc2;
        lost_msg->send_service_ = sndsvc2;
        lost_msg->backfill_trans_id_ = 0;
        lost_msg->app_id_ =
            tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

        if ((lost_msg->frame_option_ & 0x10000) != 0)
            lost_msg->dumpoutput_framehead(
                lost_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

        int lost_rc = lost_msg->appdata_encode(
            Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, lost_pkg_req, 0, 0);
        if (lost_rc == 0) {
            lost_rc = comm_svc->send_buf_to_backend(
                (const char *)trans_send_buffer, lost_msg->frame_length_,
                true, 0);
            if (lost_rc != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[%s]send to backend fail. ret=%d",
                    "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg("
                    "const PKG&, unsigned int, unsigned int, "
                    "short unsigned int) "
                    "[with PKG = sec_proto::ReportLostAntibotPkg]", lost_rc);
                lost_rc = -2;
            }
        } else {
            lost_rc = 0x139f;
        }

        if (lost_rc != 0) {
            std::ostringstream err_str_stream;
            err_str_stream
                << "In CTssSdkProtoProcPlugin::handle_antibot_pkg, "
                   "send_ipc_pkg failed, rc is "
                << lost_rc << "." << comm_svc->get_last_error();
            error_info_.set_last_error_msg(
                "%s", err_str_stream.str().c_str());
            error_info_.last_error_info()->error_code = 0x4964b;
            return error_info_.last_error_info()->error_code;
        }
    }

    return 0;
}

int TssSdkProtoProcPlugin::notify(void *notify_data) {
    if (notify_data == 0) {
        error_info_.set_last_error_msg(
            "In CTssSdkProtoProcPlugin::notify, Invalid args.");
        error_info_.last_error_info()->error_code = 0x49639;
        return error_info_.last_error_info()->error_code;
    }

    TssSdkPluginProtoProcPkg *pkg =
        (TssSdkPluginProtoProcPkg *)notify_data;

    switch (pkg->head_.msg_id_) {
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG:
        return is_cheating_pkg(&pkg->body_.anti_cheating_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG:
        return recv_buffer_for_encrypt(&pkg->body_.down_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG: {
        int rc = handle_decrypt_pkg(&pkg->body_.dp_pkg_);
        if (rc != 0) {
            error_info_.set_last_error_msg(
                "In CTssSdkProtoProcPlugin::notify, "
                "handle_decrypt_pkg failed.");
            error_info_.last_error_info()->error_code = 0x49645;
            return error_info_.last_error_info()->error_code;
        }
        return 0;
    }
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST:
        return is_need_decrypt(&pkg->body_.decrypt_list_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST:
        return is_need_encrypt(&pkg->body_.down_list_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD:
        return recv_buffer_for_encrypt_old(&pkg->body_.down_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD:
        return handle_decrypt_pkg_old(&pkg->body_.dp_pkg_);
    case TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG: {
        int rc = handle_antibot_pkg(
            pkg->body_.antibot_pkg_.uin_, pkg->body_.antibot_pkg_.buf_,
            pkg->body_.antibot_pkg_.buf_size_);
        if (rc != 0) {
            error_info_.set_last_error_msg(
                "In CTssSdkProtoProcPlugin::notify, "
                "Handle antibot pkg failed.");
            error_info_.last_error_info()->error_code = 0x49644;
            return error_info_.last_error_info()->error_code;
        }
        return 0;
    }
    default:
        error_info_.set_last_error_msg(
            "In CTssSdkProtoProcPlugin::notify, unknown pkg.");
        error_info_.last_error_info()->error_code = 0x49649;
        return error_info_.last_error_info()->error_code;
    }
}

TssSdkProtoProcPlugin *get_obj_TssSdkProtoProcPlugin() {
    static TssSdkProtoProcPlugin s_obj_TssSdkProtoProcPlugin;
    return &s_obj_TssSdkProtoProcPlugin;
}

}  // namespace tss_sdk_impl
