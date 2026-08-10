// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_dpsdk_info_mgr.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_os_adapt_file.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"

// line 24
const unsigned int SecAgentDPInfo::MAX_PATH_LEN;

// line 48
SecAgentDPsdkInfoMgr::SecAgentDPsdkInfoMgr() {
}

// line 49
SecAgentDPsdkInfoMgr::~SecAgentDPsdkInfoMgr() {
}

// line 88
void SecAgentDPsdkInfoMgr::update_time(unsigned int now_time) {
    SecAgentDPInfo *info = (SecAgentDPInfo *)dp_info_mmap_.addr();
    info->now_time_ = now_time;
}

// line 14
int SecAgentDPsdkInfoMgr::init(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
    char bus_path[4096];
    bool if_restore;
    zen_os_stat mmapfile_stat;
    snprintf(bus_path, sizeof(bus_path) - 1, "/tmp/dp_comm_config_head");
    if (conf->if_restore_dpsdk_channel_ == 1) {
        if_restore = (ZEN_OS::stat(bus_path, &mmapfile_stat) == 0);
    } else {
        ZEN_OS::unlink(bus_path);
        if_restore = false;
    }
    int ret = dp_info_mmap_.open(bus_path, sizeof(SecAgentDPInfo), if_restore, false, true, NULL, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], open mmap error, path=%s ret=%d",
                                        "int SecAgentDPsdkInfoMgr::init(secagent_config::config*)",
                                        bus_path, ret);
        return 0x493f3;
    }
    return 0;
}

// line 59
int SecAgentDPsdkInfoMgr::update_config(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
    SecAgentDPInfo *info = (SecAgentDPInfo *)dp_info_mmap_.addr();
    info->config_update_time_ = time(NULL);
    info->game_id_ = CfgSvrSdk::instance()->get_game_id();
    info->world_id_ = CfgSvrSdk::instance()->get_world_id();
    info->secagent_id_ = Comm_Svrd_Config::instance()->self_svr_id_.services_id_;

    std::string path(Comm_Svrd_Config::instance()->app_run_dir_);
    path.append("/cfg/", 5);
    std::string dp_path(path);
    dp_path.append(conf->dp_config_path_, strlen(conf->dp_config_path_));

    ZEN_Trace_LogMsg::debug_debugEx("dp config path is: %s", dp_path.c_str());
    if (dp_path.length() > SecAgentDPInfo::MAX_PATH_LEN) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], dpconfig path too long, len=%u",
                                        "int SecAgentDPsdkInfoMgr::update_config(secagent_config::config*)",
                                        dp_path.length());
        return 0x493fb;
    }
    memcpy(info->dp_config_path_, dp_path.c_str(), dp_path.length());
    info->now_time_ = time(NULL);
    return 0;
}
