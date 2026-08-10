// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_timer_handler.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_timer_handler.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_app.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include "src/formmog/secagent/secagent_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_restrict.h"

// line 44
const uint64_t SecAgentTimerHandler::NO_EXPIRE_TIME_ = 300;

// line 47
const unsigned int SecAgentTimerHandler::HEARTBEAT_TIME_INTERVAL = 60;

// line 9
SecAgentTimerHandler::SecAgentTimerHandler(ZEN_Timer_Queue *timer_queue)
    : ZEN_Timer_Handler(timer_queue) {
    cur_index_ = 0;
    conf_ = SecAgentApp::instance()->get_config();
    last_heartbeat_time_.gettimeofday();
    last_check_user_time_.gettimeofday();
}

// line 19
SecAgentTimerHandler::~SecAgentTimerHandler() {
}

// line 23
int SecAgentTimerHandler::handle_timeout(const ZEN_Time_Value &now, const void *act) {
    SecAgentApp::instance()->update_sdk_time(now.sec());

    if ((unsigned int)(now.sec() - SecAgentApp::instance()->get_svr_start_time().sec()) > (unsigned int)NO_EXPIRE_TIME_) {
        int expire_num = SecAgentUserInfoMgr::instance()->expire(now);
        ZEN_Trace_LogMsg::debug_infoex("expire num %d", expire_num);
    }

    if (now.sec() - last_check_user_time_.sec() > conf_->crypt_checker_conf_.crypt_test_gap_) {
        ZEN_Trace_LogMsg::debug_debugEx("begin check user");
        SecagentAntibotMsgHandler antibot_check;
        for (SecAgentUserInfo *user_info = SecAgentUserInfoMgr::instance()->get_first_node();
             user_info != NULL;
             user_info = SecAgentUserInfoMgr::instance()->get_next_node()) {
            antibot_check.on_check_timer(now.sec(), user_info);
        }
        last_check_user_time_ = now;
        ZEN_Trace_LogMsg::debug_debugEx("over check user");
    }

    if ((unsigned int)(now.sec() - last_heartbeat_time_.sec()) > (unsigned int)HEARTBEAT_TIME_INTERVAL) {
        SecAgentMsgHandler::heart_beat_to_secsvr();
        last_heartbeat_time_ = now;
        unsigned int online_num = SecAgentUserInfoMgr::instance()->get_online_user_num();
        unsigned int game_id = CfgSvrSdk::instance()->get_game_id();
        ZEN_Server_Status::instance()->set_by_statid(0x493ff, game_id, 0, online_num);
        ZEN_Trace_LogMsg::debug_infoex("SecAgentTimerHandler::handle_timeout, now online user number:%u",
                                       SecAgentUserInfoMgr::instance()->get_online_user_num());
    }
    return 0;
}
