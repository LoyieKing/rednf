// Reconstructed from secagent binary DWARF + disassembly (2026-08-10).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_app.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/formmog/secagent/secagent_predefine.h"
#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/framework/comm_svrd_app_non_ctrl.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "src/protocol/common/TdrXml.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_app.h"
#include "src/formmog/secagent/secagent_timer_handler.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include <arpa/inet.h>
#include <memory>
#include <stddef.h>
#include <string.h>

// 文件内静态状态项数组（与二进制静态构造顺序一致：先框架项，后 secagent 项）。
static ZEN_STATUS_ITEM_WITHNAME g_comm_stat_item[] = {
    ZEN_STATUS_ITEM_WITHNAME(0x238d, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x238e, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x238f, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2390, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x2391, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2392, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_TIMEOUT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2393, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_ERRNO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2394, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_USE_TIME"),
    ZEN_STATUS_ITEM_WITHNAME(0x2395, STATICS_PER_FIVE_MINTUES, "COMM_STAT_APP_ALIVE"),
    ZEN_STATUS_ITEM_WITHNAME(0x2396, STATICS_PER_FIVE_MINTUES, "COMM_STAT_BILL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2397, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2398, STATICS_PER_FIVE_MINTUES, "COMM_STAT_PROCESS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2399, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_MEM_USE_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x239a, STATICS_PER_FIVE_MINTUES, "COMM_STAT_AVAILABLE_MEM_SIZE"),
    ZEN_STATUS_ITEM_WITHNAME(0x239b, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_BYTES"),
    ZEN_STATUS_ITEM_WITHNAME(0x239c, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_BYTES"),
};

static ZEN_STATUS_ITEM_WITHNAME g_secagent_stat_item[] = {
    ZEN_STATUS_ITEM_WITHNAME(0x493e1, STATICS_PER_FIVE_MINTUES, "RECV_SDK_ADD_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e2, STATICS_PER_FIVE_MINTUES, "ADD_USER_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e3, STATICS_PER_FIVE_MINTUES, "ADD_USER_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e4, STATICS_PER_FIVE_MINTUES, "RECV_SDK_DEL_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e5, STATICS_PER_FIVE_MINTUES, "DEL_USER_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e6, STATICS_PER_FIVE_MINTUES, "DEL_USER_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e7, STATICS_PER_FIVE_MINTUES, "EXPIRE_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e8, STATICS_PER_FIVE_MINTUES, "SDK_REQ_RECV_DATA_NOT_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e9, STATICS_PER_FIVE_MINTUES, "SDK_REQ_RECV_DATA_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ea, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_OTHER_NO_PROC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493eb, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_INIT_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ec, STATICS_PER_FIVE_MINTUES, "UPDATE_DIB_FROM_INIT_PKG_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ed, STATICS_PER_FIVE_MINTUES, "UPDATE_KEY_FROM_INIT_PKG_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ee, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_ENCRYPT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ef, STATICS_PER_FIVE_MINTUES, "ANTIBOT_DECRYPT_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f0, STATICS_PER_FIVE_MINTUES, "ANTIBOT_DECRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f1, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_READY_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f2, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_TEST_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f3, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_CLIENT_ERROR_LOG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f4, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_RPCODE_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f5, STATICS_PER_FIVE_MINTUES, "SECSVR_FORWARD_NOT_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f6, STATICS_PER_FIVE_MINTUES, "SECSVR_FORWARD_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f7, STATICS_PER_FIVE_MINTUES, "ANTIBOT_NEED_ENCRYPT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f8, STATICS_PER_FIVE_MINTUES, "ANTIBOT_ENCRYPT_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f9, STATICS_PER_FIVE_MINTUES, "ANTIBOT_ENCRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fa, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_ENCRYPT_INFO_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fb, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_DECRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fc, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_UNENCRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fd, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_DECRYPT_TEST_PKG_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fe, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_DELOG_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ff, STATICS_PER_FIVE_MINTUES, "ONLINE_USER_NUM_FEATURE_ID"),
    ZEN_STATUS_ITEM_WITHNAME(0x49400, STATICS_PER_FIVE_MINTUES, "RPCODE_DECRYPT_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49401, STATICS_PER_FIVE_MINTUES, "RPCODE_KEY_NOT_READY_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49402, STATICS_PER_FIVE_MINTUES, "RESTRICT_PUNISH_USER_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49403, STATICS_PER_FIVE_MINTUES, "GET_GAME_EXE_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49404, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_ERROR_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49405, STATICS_PER_FIVE_MINTUES, "PROC_CLIENT_PKG_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49406, STATICS_PER_FIVE_MINTUES, "PROC_SERVER_PKG_FAIL_COUNT"),
};

SecAgentApp * SecAgentApp::instance_ = NULL;

// line 10
SecAgentApp * SecAgentApp::instance() {
    if (instance_ == NULL) {
        instance_ = new SecAgentApp();
    }
    return instance_;
}

// line 20
void SecAgentApp::clean_instance() {
    Comm_Stat_Monitor::instance();
    Comm_Stat_Monitor::clean_instance();
    if (instance_ != NULL) {
        delete instance_;
    }
    instance_ = NULL;
}

// line 28
SecAgentApp::SecAgentApp()
    : gamesvr_channel_(), dpsdk_channel_(), msg_handler_(), svr_start_time_(),
      dib_data_(), sanlix_data_(), game_exe_mgr_(8) {
    conf_ = new secagent_config::config();
    nonctrl_recv_buffer_ = new (0x10000) Comm_App_Frame(0, 0x32, 0x1000000);
}

// line 34
SecAgentApp::~SecAgentApp() {
    delete conf_;
    conf_ = NULL;
}

// line 41
int SecAgentApp::init() {
    svr_start_time_.gettimeofday();
    register_timer();
    int ret = gamesvr_channel_.open();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], gamesvr channel open error, ret=%d", "virtual int SecAgentApp::init()", ret);
        return 0x493e2;
    }
    ZEN_Trace_LogMsg::debug_infoex("init gamesvr channel succ");
    ret = dpsdk_channel_.init(conf_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], init dpsdk config info error, ret=%d", "virtual int SecAgentApp::init()", ret);
        return 0x493e3;
    }
    ZEN_Trace_LogMsg::debug_infoex("init dpsdk config info succ");
    ret = SecAgentUserInfoMgr::instance()->init(conf_->table_node_num_, conf_->if_restore_user_info_ != 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], user mngr init error, ret=%d", "virtual int SecAgentApp::init()", ret);
        return 0x493e4;
    }
    ZEN_Trace_LogMsg::debug_infoex("init user info mngr succ");
    Transaction_Manager *p_trans_mgr_ = new Transaction_Manager();
    p_trans_mgr_->initialize(Comm_Svrd_Config::instance()->framework_config_.trans_info_.trans_cmd_num_,
                             Comm_Svrd_Config::instance()->framework_config_.trans_info_.trans_num_,
                             self_services_id_,
                             ZEN_Timer_Queue::instance(),
                             Zerg_MMAP_BusPipe::instance(),
                             0x10000, false, false);
    Transaction_Manager::instance(p_trans_mgr_);
    ret = Comm_Stat_Monitor::instance()->initialize(false,
                                                    Comm_Svrd_Config::instance()->self_svr_id_,
                                                    0x26, g_secagent_stat_item, false);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], Comm_Stat_Monitor init fail. ret=%d", "virtual int SecAgentApp::init()", ret);
        return 0x493e6;
    }
    ZEN_Trace_LogMsg::debug_infoex("init stat monitor st succ");
    msg_handler_.init(conf_);
    return load_conf();
}

// line 109
void SecAgentApp::register_timer() {
    ZEN_Timer_Queue *timer_queue = ZEN_Timer_Queue::instance();
    ZEN_Time_Value delay;
    ZEN_Time_Value interval;
    delay.set(0, 0);
    interval.set(1, 0);
    const unsigned int TIME_WHEEL_INTERVAL = 1;
    SecAgentTimerHandler *timer_handler = new SecAgentTimerHandler(timer_queue);
    timer_queue->schedule_timer(timer_handler, NULL, delay, interval);
}

// line 126
void SecAgentApp::exit() {
}

// line 131
int SecAgentApp::reload() {
    ZEN_Trace_LogMsg::debug_infoex("reload start");
    msg_handler_.reload(conf_);
    int ret = load_conf();
    ZEN_Trace_LogMsg::debug_infoex("reload ret=%d", ret);
    return ret;
}

// line 141
int SecAgentApp::process_recv_appframe(Comm_App_Frame *recv_frame) {
    if (recv_frame->frame_length_ == Comm_App_Frame::LEN_OF_APPFRAME_HEAD) {
        ZEN_Trace_LogMsg::debug_debugEx("ack pkg, no need proc");
        return 0;
    }
    int ret = msg_handler_.proc_sever_msg(recv_frame);
    if (ret == 0x493fd) {
        ZEN_Trace_LogMsg::debug_infoex("msg handler proc sever msg, no need send to sdk, ret=%u", 0x493fd);
        return 0;
    }
    unsigned short channel_id = ntohs(*(unsigned short *)recv_frame->frame_appdata_);
    unsigned short room_id = ntohs(*(unsigned short *)(recv_frame->frame_appdata_ + 2));
    ret = gamesvr_channel_.send(channel_id, recv_frame, recv_frame->frame_length_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], send to gamesvr channel fail,ret=%d, channel_id=%d",
                                        "virtual int SecAgentApp::process_recv_appframe(Comm_App_Frame*)", ret, channel_id);
    } else {
        ZEN_Trace_LogMsg::debug_infoex("SecAgentApp::process_recv_appframe, send over, channel_id=%u, room_id=%u",
                                       channel_id, room_id);
    }
    return ret;
}

// line 183
int SecAgentApp::proc(size_t &proc_data_num) {
    unsigned int recv_buf_size = 0;
    int ret;
    unsigned int min_channel_id = gamesvr_channel_.getMinChannelID();
    unsigned int max_channel_id = gamesvr_channel_.getMaxChannelID();
    for (unsigned int i = min_channel_id; i < max_channel_id; ++i) {
        unsigned int recv_num = 0;
        while (recv_num < conf_->channel_recv_times_) {
            ret = gamesvr_channel_.recv(i, (void **)&recv_buf_, &recv_buf_size);
            if (ret != 0) {
                break;
            }
            if (recv_buf_size == 0) {
                continue;
            }
            ++recv_num;
            ZEN_Trace_LogMsg::debug_debugEx("channel_id[%d] recv data, num=%d", i, recv_num);
            ret = msg_handler_.proc_client_msg((Comm_App_Frame *)recv_buf_);
            if (ret != 0) {
                ZEN_Trace_LogMsg::debug_errorex("[%s], proc sdk msg error, ret = %d", "virtual int SecAgentApp::proc(size_t&)", ret);
            }
            ++proc_data_num;
        }
    }
    return proc_data_num;
}

// line 222
int SecAgentApp::load_game_exe() {
    std::auto_ptr<SecagentGameExe> game_exe(new SecagentGameExe());
    int ret = game_exe->load_version(conf_->crypt_checker_conf_.exe_version_path_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("load gamedata fail. load version fail. ret=%d", ret);
        return ret;
    }
    unsigned int game_exe_version = game_exe->get_version();
    if (game_exe_mgr_.get_data(game_exe_version) != NULL) {
        ZEN_Trace_LogMsg::debug_debugEx("muti_version_test, game exe, the new data existed, version=%u", game_exe_version);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_debugEx(" game exe, the new data not existed, will load, version=%u", game_exe_version);
    ret = game_exe->load(conf_->crypt_checker_conf_.exe_path_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("load game exe fail. ret=%d", ret);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_debugEx("game exe, the new data load succ, version=%u", game_exe_version);
    game_exe_mgr_.add_new_data(game_exe_version, game_exe);
    return 0;
}

// line 264
int SecAgentApp::load_dibdata() {
    if (conf_->crypt_checker_conf_.is_use_ == 0) {
        ZEN_Trace_LogMsg::debug_infoex("dibdata is not use");
        return 0;
    }
    int ret = dib_data_.load(conf_->crypt_checker_conf_.dib_base_path_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("dibdata load config fail|ret=%d", ret);
    }
    return ret;
}

// line 282
int SecAgentApp::load_app_conf() {
    int ret = 0;
    ret = conf_->fromXmlFile(Comm_Svrd_Config::instance()->app_cfg_file_.c_str(),
                             (tsf4g_tdr::TdrXmlFormat)0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], secagent app load config fail, ret=%d",
                                        "virtual int SecAgentApp::load_app_conf()", ret);
    }
    return ret;
}

// line 296
int SecAgentApp::load_conf() {
    if (conf_->is_for_mmog_ != 0) {
        int ret = load_game_exe();
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[%s], read game exe fail, ret=%d", "int SecAgentApp::load_conf()", ret);
            return 0x493e5;
        }
        ret = load_dibdata();
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[%s], load dibdata fail, ret=%d", "int SecAgentApp::load_conf()", ret);
            return 0x493ff;
        }
    }
    int ret = dpsdk_channel_.update_config(conf_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], update dpsdk config info error, ret=%d", "int SecAgentApp::load_conf()", ret);
        return 0x493e3;
    }
    ZEN_Trace_LogMsg::debug_infoex("update dpsdk config info succ");
    set_log_priority((ZEN_LOG_PRIORITY)conf_->log_priority_);
    if (conf_->sanlix_checker_conf_.is_use_ != 0) {
        int ret2 = sanlix_data_.load();
        ZEN_Trace_LogMsg::debug_infoex("load sanlix data , ret=%d", ret2);
        return ret2;
    }
    return 0;
}

// line 342
const SecagentGameExe * SecAgentApp::get_game_exe(unsigned int version) const {
    return game_exe_mgr_.get_data(version);
}

// line 348
const SecagentGameExe * SecAgentApp::get_game_exe_newest() const {
    return game_exe_mgr_.get_newest_data();
}

// line 354
void SecAgentApp::update_sdk_time(unsigned int now_time) {
    dpsdk_channel_.update_time(now_time);
}

// 显式实例化（原二进制弱符号：Comm_App_Frame 模板由服务 TU 发出）
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"
#include "output/protocol/formmog/release/include/formmog_proto_public_secsvr.h"

template int Comm_App_Frame::appdata_encode<sec_proto::CfgFileReq>(unsigned int, const sec_proto::CfgFileReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::ReportErrReq>(unsigned int, const sec_proto::ReportErrReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIpInfoReq>(unsigned int, const sec_proto::SvcIpInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::DeployInfoReq>(unsigned int, const sec_proto::DeployInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::RecvDataFromSDKReq>(unsigned int, const sec_proto::RecvDataFromSDKReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::LogSvrCommBillRecord>(unsigned int, const sec_proto::LogSvrCommBillRecord&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::CheckCfgFileUpdateReq>(unsigned int, const sec_proto::CheckCfgFileUpdateReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIdReq>(unsigned int, const sec_proto::SvcIdReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SendToSDK>(unsigned int, const sec_proto::SendToSDK&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_decode<sec_proto::CfgFileRsp>(sec_proto::CfgFileRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIpInfoRsp>(sec_proto::SvcIpInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CfgsvrInfoRsp>(sec_proto::CfgsvrInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::DeployInfoRsp>(sec_proto::DeployInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CheckCfgFileUpdateRsp>(sec_proto::CheckCfgFileUpdateRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIdRsp>(sec_proto::SvcIdRsp&, unsigned int, size_t*) const;
