// Reconstructed from secagent disassembly (behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_antibot_sanlixdata.cpp

#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/commlib/framework/comm_time_provider.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/framework/comm_stat_define.h"

#include <string.h>
#include <sys/stat.h>

// secagent_file_load.cpp（本 TU 直接声明，避免坏桩头文件）
extern int load_bin_file(const char *file_path, unsigned char *data, unsigned int *data_len);

#include "src/formmog/secagent/secagent_app.h"

#define SANLIX_ERROR_FILE_LOAD 0x4940e
#define SANLIX_DATA_MAX_LEN    0x7fd7

AntibotSanlixData::AntibotSanlixData() {
    memset(&te_data_, 0, sizeof(AntibotSanlixDataInfo));
    memset(&ts_data_, 0, sizeof(AntibotSanlixDataInfo));
    memset(&tk_data_, 0, sizeof(AntibotSanlixDataInfo));
}

AntibotSanlixData::~AntibotSanlixData() {
}

const AntibotSanlixDataInfo & AntibotSanlixData::get_tedata() const {
    return te_data_;
}

const AntibotSanlixDataInfo & AntibotSanlixData::get_tsdata() const {
    return ts_data_;
}

const AntibotSanlixDataInfo & AntibotSanlixData::get_tkdata() const {
    return tk_data_;
}

const AntibotSanlixDataInfo & AntibotSanlixData::get_tenpartydata() const {
    return tenparty_data_;
}

int AntibotSanlixData::load_sanlix_data(const char *file_path, AntibotSanlixDataInfo *data_info) {
    struct stat st;
    int ret;
    time_t modify_time;

    if (stat(file_path, &st) == 0) {
        modify_time = st.st_mtime;
    } else {
        modify_time = 0;
    }

    if (data_info->last_modify_time_ != modify_time) {
        data_info->last_modify_time_ = modify_time;
        data_info->load_time_ = Comm_Time_Provider::instance()->time();
        data_info->data_len_ = SANLIX_DATA_MAX_LEN;
        ret = load_bin_file(file_path, data_info->data_, &data_info->data_len_);
        if (ret == 0 && data_info->data_len_ != SANLIX_DATA_MAX_LEN) {
            data_info->is_read_succ_ = true;
            return 0;
        }
        ZEN_Trace_LogMsg::debug_errorex("load %s fail. ret=%d len=%d", file_path, ret, data_info->data_len_);
        return SANLIX_ERROR_FILE_LOAD;
    }
    return 0;
}

int AntibotSanlixData::load() {
    int ret = 0;
    const secagent_config::config *conf = SecAgentApp::instance()->get_app_config();

    if (!conf->sanlix_checker_conf_.is_use_) {
        ZEN_Trace_LogMsg::debug_debugEx("no use sanlix data");
        return 0;
    }
    if (conf->sanlix_checker_conf_.is_distribute_ts_data_) {
        ret = load_sanlix_data(conf->sanlix_checker_conf_.ts_data_path_, &ts_data_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("load ts fail. ret=%d", ret);
            return ret;
        }
    }
    if (conf->sanlix_checker_conf_.is_distribute_te_data_) {
        ret = load_sanlix_data(conf->sanlix_checker_conf_.te_data_path_, &te_data_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("load te fail. ret=%d", ret);
            return ret;
        }
    }
    if (conf->sanlix_checker_conf_.is_distribute_tk_data_) {
        ret = load_sanlix_data(conf->sanlix_checker_conf_.tk_data_path_, &tk_data_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("load tk fail. ret=%d", ret);
            return ret;
        }
    }
    if (conf->sanlix_checker_conf_.is_distribute_tenparty_data_) {
        ret = load_sanlix_data(conf->sanlix_checker_conf_.tp_data_path_, &tenparty_data_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("load tenparty fail. ret=%d path=%s", ret, conf->sanlix_checker_conf_.tp_data_path_);
            return ret;
        }
    }
    return ret;
}
