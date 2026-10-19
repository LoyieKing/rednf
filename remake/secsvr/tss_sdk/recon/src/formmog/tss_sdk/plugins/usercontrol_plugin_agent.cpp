// Semantic reconstruction of tss_sdk_impl::TssSdkUserControlPlugin from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// ORIG translation unit:
//   src/formmog/tss_sdk/plugins/user_control/user_control_plugin.cpp
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   fini()                                        0x000ca130-0x000ca140
//   check_timer(long)                             0x000ca1f0-0x000ca26c
//   get_user_info_by_uin(GetUserInfo&)            0x000ca270-0x000ca2c4
//   get_user_info(GetUserInfo&)                   0x000ca2d0-0x000ca33b
//   TssSdkUserControlPlugin() C1                  0x000ca340-0x000ca3f1
//   get_obj_TssSdkUserControlPlugin()             0x000ca400-0x000ca495
//   TssSdkUserControlPlugin() C2                  0x000ca4a0-0x000ca551
//   ProcessCheckTimer(UserInfo*, void*, time_t)   0x000ca560-0x000ca590
//   punish_user(PunishUser&)                      0x000ca9f0-0x000caca7
//   proc(void*)                                   0x000cacb0-0x000cae49
//   delete_user(DeleteUser&)                      0x000cae50-0x000cb878
//   add_user(AddUser&)                            0x000cb880-0x000cc3d4
//   notify(void*)                                 0x000cc3e0-0x000cc63e
//   init(void*)                                   0x000cc640-0x000cd10f
//   ~TssSdkUserControlPlugin() D0                 0x000cd110-0x000cd14e
//   ~TssSdkUserControlPlugin() D1                 0x000cd150-0x000cd17d
//
// Class layout from DWARF (sizeof == 40, base ITssSdkPlugin is 16 bytes):
//   +0x00 vptr
//   +0x04 TssSdkPluginCallbackProc callback_proc_
//   +0x08 CThreadErrorInfo error_info_ (4-byte pthread_key_t)
//   +0x0c ETssSdkPluginId sdk_plugin_id_
//   +0x10 DPInterface* m_dp_if_
//   +0x14 unsigned int channel_id_
//   +0x18 unsigned int m_add_user_seq_
//   +0x1c unsigned int m_delete_user_seq_
//   +0x20 UserMgr* user_manager_
//   +0x24 time_t last_check_timer_
//
// The destructor does NOT delete user_manager_ in ORIG; that leak is preserved.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <new>
#include <sstream>
#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// Real sec_proto::PunishClientReq (formmog_proto_public_tsssdk.cpp, CU
// 0x00154d50): class with a non-const unpack(char const*, ...).  The local
// const-member copy produced _ZNK9sec_proto15PunishClientReq6unpack... which
// does not exist in ORIG.
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

// ---------------------------------------------------------------------------
// zenlib boundary types: canonical tag-394 declarations.
// ---------------------------------------------------------------------------

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG
// (audit/report/init_brief.md).

#include "src/commlib/zenlib/zen_singleton.h"


// DPInterface (12 bytes, ORIG dpinterface.cpp) is the canonical shared
// declaration in src/formmog/tss_sdk/layout/dp_interface.h.
#include "src/formmog/tss_sdk/layout/dp_interface.h"

// Canonical tag-394 declarations.  UserInfo, DP_CONFIG, SERVICES_ID,
// Comm_App_Frame, the plugin packet wrappers and the process-wide config are
// all declared once by shared headers; the local copies are gone.
#include "src/commlib/framework/comm_cfgsvr_sdk.h"  // SERVICES_ID, Comm_App_Frame
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/tss_sdk_pkgs.h"
#include "src/formmog/tss_sdk/layout/user_info.h"
#include "src/formmog/tss_sdk/plugins/tss_sdk_config_shared.h"
#include "src/protocol/formmog/formmog_proto_public_secsvr.h"

// Frame limits are the canonical Comm_App_Frame constants (comm_app_frame.h):
// MAX_LEN_OF_APPFRAME (0x10000) sizes the trans_send_buffer,
// MAX_LEN_OF_APPFRAME_DATA (0xFFBD) is the appdata budget.

namespace tss_sdk_impl {

// user_control/user_mgr.h — sizeof == 40.  layout/user_mgr.h declares the
// same members but omits UserMgr::check(), which this TU calls; keeping the
// declaration here preserves the ORIG mangled name
// (_ZN12tss_sdk_impl7UserMgr5checkEPF...).  Member names/types/offsets match
// the canonical header exactly.
class UserMgr {
public:
    UserMgr();
    ~UserMgr();

    int init(const char *shm_name, unsigned int count, int use_idx);
    int add_user(unsigned int uin, unsigned short game_id,
                 unsigned short room_id);
    int del_user(UserInfo *user);
    UserInfo *get_user(unsigned int uin, unsigned short game_id,
                       unsigned short room_id);
    UserInfo *get_user_info(unsigned int &uin, unsigned short &game_id,
                            unsigned short &room_id, unsigned int idx);
    int check(int (*func)(UserInfo *, void *, time_t), void *userdata,
              time_t now);

private:
    void *user_info_hash_table_;                  // +0x00
    time_t last_check_time_;                      // +0x04
    int use_idx_;                                 // +0x08
    ZEN_Thread_Mutex user_info_hash_table_mutex_;  // +0x0c, sizeof 28
};

// user_control/user_control_plugin.h.
class TssSdkUserControlPlugin : public ITssSdkPlugin {
public:
    TssSdkUserControlPlugin();
    virtual ~TssSdkUserControlPlugin();

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *proc_data);
    virtual int notify(void *notify_data);

    int add_user(TssSdkPluginUserControlAddUser &pkg);
    int delete_user(TssSdkPluginUserControlDeleteUser &pkg);
    int punish_user(TssSdkPluginUserControlPunishUser &pkg);
    int get_user_info(TssSdkPluginUserControlGetUserInfo &pkg);
    int get_user_info_by_uin(TssSdkPluginUserControlGetUserInfo &pkg);
    int check_timer(time_t now);

private:
    DPInterface *m_dp_if_;          // +0x10
    unsigned int channel_id_;       // +0x14
    unsigned int m_add_user_seq_;   // +0x18
    unsigned int m_delete_user_seq_;// +0x1c
    UserMgr *user_manager_;         // +0x20
    time_t last_check_timer_;       // +0x24
};

int ProcessCheckTimer(UserInfo *user, void *instance, time_t now);
TssSdkUserControlPlugin *get_obj_TssSdkUserControlPlugin();

}  // namespace tss_sdk_impl


namespace tss_sdk_impl {

// ===========================================================================
// Implementation.
// ===========================================================================

int ProcessCheckTimer(UserInfo *user, void *instance, time_t now) {
    DPInterface *proc = (DPInterface *)instance;
    return proc->check_timer(user, now);
}

TssSdkUserControlPlugin::TssSdkUserControlPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL) {
    m_dp_if_ = 0;
    channel_id_ = 0;
    m_add_user_seq_ = 0;
    m_delete_user_seq_ = 0;
    last_check_timer_ = 0;

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkUserControlPlugin ctor.");

    user_manager_ = new UserMgr();
}

TssSdkUserControlPlugin::~TssSdkUserControlPlugin() {
    // ORIG leaks user_manager_.
}

int TssSdkUserControlPlugin::fini() {
    channel_id_ = 0;
    return 0;
}

int TssSdkUserControlPlugin::check_timer(time_t now) {
    m_dp_if_->check_timer((long)now);

    tss_dp_comm::DPConfig *config = tss_dp_comm::DP_CONFIG::glb_dp_config_;
    if (config->dp_base_conf.is_use_dp == 0)
        return 0;

    unsigned int elapsed = (unsigned int)(now - last_check_timer_);
    if (elapsed > config->dp_time_confg.check_time) {
        last_check_timer_ = now;
        user_manager_->check(ProcessCheckTimer, m_dp_if_, now);
    }
    return 0;
}

int TssSdkUserControlPlugin::get_user_info_by_uin(
    TssSdkPluginUserControlGetUserInfo &pkg) {
    pkg.user_info_ = user_manager_->get_user(
        pkg.uin_, (unsigned short)channel_id_,
        (unsigned short)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id);
    return 0;
}

int TssSdkUserControlPlugin::get_user_info(
    TssSdkPluginUserControlGetUserInfo &pkg) {
    unsigned int uin = pkg.uin_;
    unsigned short game_id = (unsigned short)channel_id_;
    unsigned short room_id =
        (unsigned short)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;

    pkg.user_info_ = user_manager_->get_user_info(uin, game_id, room_id,
                                                  pkg.idx_);
    pkg.uin_ = uin;
    return 0;
}

int TssSdkUserControlPlugin::punish_user(
    TssSdkPluginUserControlPunishUser &pkg) {
    TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();

    if (comm_svc->is_need_trans_uin(pkg.game_id_)) {
        UserInfo *user = comm_svc->get_user_info_by_uin(pkg.uin_);
        if (user == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "in TssSdkUserControlPlugin::punish_user, "
                "get_user_info_by_uin error, uin = %u", pkg.uin_);
            return -1;
        }
        pkg.uin_ = user->base_info_.game_usr_id_;
        ZEN_Trace_LogMsg::debug_debugEx(
            "in TssSdkUserControlPlugin::punish_user, "
            "get_user_info_by_uin success, uin = %u", pkg.uin_);
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkUserControlPlugin::punish_user, uin/userid: %u, "
        "game_id: %u, room_id: %u, punish_mode: %d, punish_time: %d, "
        "punish_info_len: %u, punish_info: %s, punish_reason_len: %u",
        pkg.uin_, (unsigned int)pkg.game_id_, (unsigned int)pkg.room_id_,
        pkg.punish_mode_, pkg.punish_time_, pkg.punish_info_len_,
        (const char *)pkg.punish_info_, pkg.punish_reason_len_);

    int rc = callback_proc_(&pkg);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In CTssSdkUserControlPlugin::punish_user, "
            "callback_proc_ failed. rc=%d.", rc);
        error_info_.set_last_error_msg(
            "In TssSdkUserControlPlugin::proc, punish user failed.");
        error_info_.last_error_info()->error_code = 0x49766;
    }
    return 0;
}

int TssSdkUserControlPlugin::proc(void *proc_data) {
    check_timer(time(0));

    if (proc_data == 0)
        return 0;

    Comm_App_Frame *app_frame = (Comm_App_Frame *)proc_data;
    char err_str[256];

    if (app_frame->frame_command_ != 0x493e3) {
        snprintf(err_str, 0xff, "recv invalid cmd %u, expect %u.",
                 app_frame->frame_command_, 0x493e3);
        error_info_.set_last_error_msg("%s", err_str);
        error_info_.last_error_info()->error_code = 0x497ca;
        return error_info_.last_error_info()->error_code;
    }

    sec_proto::PunishClientReq punish_client;
    if (app_frame->appdata_decode(punish_client, 0, 0) != 0) {
        snprintf(err_str, 0xff,
                 "unpack punish client pkg failed. uin %u|appid %u.",
                 app_frame->frame_uin_, app_frame->app_id_ & 0xffff);
        error_info_.set_last_error_msg("%s", err_str);
        error_info_.last_error_info()->error_code = 0x494b3;
        return error_info_.last_error_info()->error_code;
    }

    TssSdkPluginUserControlPunishUser pkg;
    pkg.uin_ = app_frame->frame_uin_;
    pkg.game_id_ = (unsigned short)(app_frame->app_id_ & 0xffff);
    pkg.room_id_ = punish_client.room_id_;
    pkg.punish_mode_ = (int)punish_client.punish_type_;
    pkg.punish_time_ = (int)punish_client.punish_duration_;
    pkg.punish_info_len_ = punish_client.punish_info_len_;
    pkg.punish_info_ = punish_client.punish_info_;
    pkg.punish_reason_len_ = punish_client.punish_reason_len_;
    pkg.punish_reason_ = punish_client.punish_reason_;

    return punish_user(pkg);
}

int TssSdkUserControlPlugin::delete_user(
    TssSdkPluginUserControlDeleteUser &pkg) {
    TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();
    pkg.idx_ = 0xffffffff;

    UserInfo *user = comm_svc->get_user_info_by_uin(pkg.uin_);
    if (user != 0) {
        int rc = m_dp_if_->del_user(user);
        if (rc < 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "In CTssSdkUserControlPlugin::delete_user, "
                "m_dp_if_->del_user failed. rc=%d.", rc);
        }

        rc = user_manager_->del_user(user);
        if (rc == 0) {
            ZEN_Trace_LogMsg::debug_debugEx(
                "In CTssSdkUserControlPlugin::delete_user, "
                "delete user ok. idx=%d.", 0);
            pkg.idx_ = 0;
        } else {
            ZEN_Trace_LogMsg::debug_errorex(
                "In CTssSdkUserControlPlugin::delete_user, "
                "delete user failed. rc=%d.", rc);
        }
    } else {
        ZEN_Trace_LogMsg::debug_errorex(
            "In CTssSdkUserControlPlugin::delete_user, "
            "delete user failed. rc=%d, uin=%u.", 0, pkg.uin_);
    }

    // IPC DelUserReq: channel/room/timestamp.
    sec_proto::DelUserReq del_user_req;
    del_user_req.channel_id_ = (uint16_t)channel_id_;
    del_user_req.room_id_ =
        (uint16_t)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
    del_user_req.time_stamp_ = (uint32_t)time(0);

    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg = (Comm_App_Frame *)trans_send_buffer;

    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4baf3);
    SERVICES_ID rcvsvc(0x1f, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x2b, 0);

    rsp_msg->frame_uin_ = pkg.uin_;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;
    rsp_msg->backfill_trans_id_ = 0;
    rsp_msg->app_id_ = tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if ((rsp_msg->frame_option_ & 0x10000) != 0)
        rsp_msg->dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, del_user_req, 0, 0);
    if (ret == 0) {
        ret = comm_svc->send_buf_to_backend(
            (const char *)trans_send_buffer, rsp_msg->frame_length_, true, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s]send to backend fail. ret=%d",
                "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
                "unsigned int, unsigned int, short unsigned int) "
                "[with PKG = sec_proto::DelUserReq]", ret);
            ret = -2;
        }
    } else {
        ret = 0x139f;
    }

    if (ret != 0) {
        std::ostringstream err_str_stream;
        err_str_stream
            << "In CTssSdkUserControlPlugin::delete_user, "
               "send IPC pkg failed. "
            << comm_svc->get_last_error();
        error_info_.set_last_error_msg("%s", err_str_stream.str().c_str());
        error_info_.last_error_info()->error_code = 0x49768;
        return error_info_.last_error_info()->error_code;
    }
    return 0;
}

int TssSdkUserControlPlugin::add_user(
    TssSdkPluginUserControlAddUser &pkg) {
    TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();
    pkg.idx_ = 0xffffffff;

    int obj_id = user_manager_->add_user(
        pkg.uin_, (unsigned short)channel_id_,
        (unsigned short)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id);
    if (obj_id > 0)
        pkg.idx_ = obj_id;
    else
        ZEN_Trace_LogMsg::debug_errorex(
            "in TssSdkUserControlPlugin::add_user, "
            "user_manager_->add_user error, uin = %u", pkg.uin_);

    UserInfo *user = comm_svc->get_user_info(pkg.uin_, pkg.idx_);
    if (user == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In TssSdkUserControlPlugin::add_user, "
            "get_user_info failed. uin=%u, obj_id=%d.",
            pkg.uin_, (int)pkg.idx_);
        pkg.idx_ = 0xffffffff;
    } else {
        user->base_info_.game_id_ = channel_id_;
        user->base_info_.room_id_ =
            tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
        user->base_info_.game_usr_id_ = pkg.user_id_;
        user->base_info_.client_version_ = pkg.client_ver_;
        user->base_info_.client_ip_ = pkg.client_ip_;
        user->base_info_.server_ip_ = pkg.server_ip_;
        if (pkg.player_name_ != 0)
            strncpy(user->base_info_.role_name_, pkg.player_name_, 0x3f);
        ZEN_Trace_LogMsg::debug_debugEx(
            "In TssSdkUserControlPlugin::add_user, "
            "add user ok. uin=%u, idx=%d", pkg.uin_, (int)pkg.idx_);
    }

    int rc = m_dp_if_->add_user(user);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "in TssSdkUserControlPlugin::add_user, "
            "m_dp_if_->add_user failed, rc = %d", rc);
    }

    // IPC AddUserReq.
    sec_proto::AddUserReq add_user_req;
    add_user_req.channel_id_ = (uint16_t)channel_id_;
    add_user_req.room_id_ =
        (uint16_t)tss_dp_comm::DP_CONFIG::glb_dp_config_head_->world_id;
    add_user_req.time_stamp_ = (uint32_t)time(0);
    add_user_req.client_version_ = pkg.client_ver_;

    unsigned int play_name_len;
    if (pkg.player_name_ == 0) {
        play_name_len = 0;
        add_user_req.player_name_[0] = '\0';
    } else {
        play_name_len = (unsigned int)strlen(pkg.player_name_);
        if (play_name_len > 0xff)
            play_name_len = 0x100;
        if (play_name_len != 0)
            strncpy(add_user_req.player_name_, pkg.player_name_,
                    play_name_len);
        add_user_req.player_name_[play_name_len] = '\0';
    }

    add_user_req.client_ip_ = pkg.client_ip_;
    add_user_req.server_ip_ = pkg.server_ip_;

    unsigned char trans_send_buffer[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *rsp_msg = (Comm_App_Frame *)trans_send_buffer;

    rsp_msg->init_framehead(Comm_App_Frame::MAX_LEN_OF_APPFRAME, 0, 0x4baf1);
    SERVICES_ID rcvsvc(0x1f, 0);
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc(0x2b, 0);

    rsp_msg->frame_uin_ = pkg.uin_;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;
    rsp_msg->backfill_trans_id_ = 0;
    rsp_msg->app_id_ = tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;

    if ((rsp_msg->frame_option_ & 0x10000) != 0)
        rsp_msg->dumpoutput_framehead(
            rsp_msg, "[TRACK MONITOR][TRANS SEND]", RS_INFO);

    int ret = rsp_msg->appdata_encode(
        Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA, add_user_req, 0, 0);
    if (ret == 0) {
        ret = comm_svc->send_buf_to_backend(
            (const char *)trans_send_buffer, rsp_msg->frame_length_, true, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s]send to backend fail. ret=%d",
                "int tss_sdk_impl::TssSdkCommSvc::send_ipc_pkg(const PKG&, "
                "unsigned int, unsigned int, short unsigned int) "
                "[with PKG = sec_proto::AddUserReq]", ret);
            ret = -2;
        }
    } else {
        ret = 0x139f;
    }

    if (ret != 0) {
        std::ostringstream err_str_stream;
        err_str_stream
            << "In CTssSdkUserControlPlugin::add_user, "
               "send IPC pkg failed. "
            << comm_svc->get_last_error();
        error_info_.set_last_error_msg("%s", err_str_stream.str().c_str());
        error_info_.last_error_info()->error_code = 0x49768;
        return error_info_.last_error_info()->error_code;
    }
    return 0;
}

int TssSdkUserControlPlugin::notify(void *notify_data) {
    if (notify_data == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "in TssSdkUserControlPlugin::notify, invalid args");
        error_info_.set_last_error_msg(
            "In CTssSdkUserControlPlugin::notify, notify_data is NULL.");
        error_info_.last_error_info()->error_code = 0x49765;
        return error_info_.last_error_info()->error_code;
    }

    TssSdkPluginUserControlPkg *pkg =
        (TssSdkPluginUserControlPkg *)notify_data;

    switch (pkg->head_.msg_id_) {
    case TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER:
        return add_user(pkg->body_.add_user_);
    case TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER:
        return delete_user(pkg->body_.delete_user_);
    case TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER:
        return punish_user(pkg->body_.punish_user_);
    case TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO:
        return get_user_info(pkg->body_.user_info_);
    case TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN:
        return get_user_info_by_uin(pkg->body_.user_info_);
    default:
        ZEN_Trace_LogMsg::debug_errorex(
            "in TssSdkUserControlPlugin::notify, unknown msg_id_");
        error_info_.set_last_error_msg("Unknown pkg.");
        error_info_.last_error_info()->error_code = 0x49767;
        return error_info_.last_error_info()->error_code;
    }
}

int TssSdkUserControlPlugin::init(void *init_data) {
    (void)init_data;

    TssSdkCommSvc *comm_svc = Singleton<TssSdkCommSvc>::instance();
    m_dp_if_ = comm_svc->dp_if_;
    channel_id_ = comm_svc->channel_id_;

    std::ostringstream channel_id_str;
    channel_id_str << channel_id_;

    std::string ipc_name = channel_id_str.str();
    int res = user_manager_->init(
        ipc_name.c_str(), (unsigned int)g_config.usercontrol_config_.
                              max_client_obj_count_,
        g_config.usercontrol_config_.use_idx_);

    if (res != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "UserMgr init fail, res=%d, ipc_name=%s, max_client_count=%d",
            res, ipc_name.c_str(),
            g_config.usercontrol_config_.max_client_obj_count_);
        return -1;
    }
    return 0;
}

TssSdkUserControlPlugin *get_obj_TssSdkUserControlPlugin() {
    static TssSdkUserControlPlugin s_obj_TssSdkUserControlPlugin;
    return &s_obj_TssSdkUserControlPlugin;
}

}  // namespace tss_sdk_impl

// ORIG static-init block, part 2 (definition above the first use): the weak
// object_creator template static member (ORIG .bss 0x0029d800).
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
