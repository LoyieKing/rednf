// Reconstructed from gunnersvr disassembly (DWARF + .rodata + mnemonic check).
#include "src/commsvr/gunnersvr/gunner_reporter.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"

#include <time.h>
#include <vector>

GunnerInfoReporter *GunnerInfoReporter::instance_ = NULL;

GunnerInfoReporter::GunnerInfoReporter() {
    config_ = new conf_gunnersvr::config();
}

GunnerInfoReporter::~GunnerInfoReporter() {
    delete config_;
    config_ = NULL;
}

GunnerInfoReporter *GunnerInfoReporter::instance() {
    if (instance_ == NULL) {
        instance_ = new GunnerInfoReporter;
    }
    return instance_;
}

void GunnerInfoReporter::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

int GunnerInfoReporter::init(GunnerChannelMgr::LPCONFIG config) {
    config_ = config;
    int ret = channel_mgr_.init_channels(config);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s] init monitor channel error",
            "int GunnerInfoReporter::init(conf_gunnersvr::config*)");
    }
    return ret;
}

int GunnerInfoReporter::reload(GunnerChannelMgr::LPCONFIG config) {
    channel_mgr_.close_channels();
    return init(config);
}

int GunnerInfoReporter::report(time_t report_time) {
    std::vector<std::string> stat_files;
    int ret = file_scanner_.scan_stat_file("/dev/shm/", stat_files);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("scan stat file error");
        return ret;
    }

    unsigned int size = stat_files.size();
    GunnerStatInfo stat_info;
    unsigned int i;
    for (i = 0; i < size; ++i) {
        if (statinfo_fetcher_.get_info(stat_files[i].c_str(), stat_info) < 0) {
            ZEN_Trace_LogMsg::debug_errorex("get stat file %s statinfo failed",
                                            stat_files[i].c_str());
            continue;
        }
        time_t cost_time = report_time - stat_info.copy_time_;
        if (cost_time > 300) {
            ZEN_Trace_LogMsg::debug_debugEx("shm file : %s is time out",
                                            stat_files[i].c_str());
            continue;
        }
        report_statinfo_to_monitor(stat_info);
    }
    return 0;
}

int GunnerInfoReporter::report_statinfo_to_monitor(const GunnerStatInfo &stat_info) {
    int ret = 0;
    const int APPID_NUM = 32;
    unsigned int appid_num = 0;
    unsigned int appid[APPID_NUM];
    unsigned int i, j;

    for (i = 0; i < stat_info.statinfo_array_.size(); ++i) {
        for (j = 0; j < appid_num; ++j) {
            if (appid[j] == stat_info.statinfo_array_[i].statics_item_.item_id_.app_id_) {
                break;
            }
        }
        if (j == appid_num) {
            appid[appid_num++] = stat_info.statinfo_array_[i].statics_item_.item_id_.app_id_;
        }
    }

    std::vector<sec_proto::MonitorSvrReport> report_pkgs(appid_num);
    pack_by_appid(stat_info, appid_num, appid, report_pkgs);

    for (i = 0; i < report_pkgs.size(); ++i) {
        sec_proto::MonitorSvrReport pkg = report_pkgs[i];
        GunnerChannel *channel = channel_mgr_.get_channel(appid[i]);
        ret = channel->send_to_monitor(pkg);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s] send pkg to monitor fail",
                "int GunnerInfoReporter::report_statinfo_to_monitor(const GunnerStatInfo&)");
        }
    }
    return 0;
}

int GunnerInfoReporter::pack_by_appid(const GunnerStatInfo &stat_info,
                                      unsigned int appid_num,
                                      const unsigned int *appid,
                                      std::vector<sec_proto::MonitorSvrReport> &report_pkgs) {
    time_t tmp_remain_time = stat_info.copy_time_ % 300;
    uint32_t report_time = 0;
    unsigned int array_size = stat_info.statinfo_array_.size();
    unsigned int i, j;
    int item_count;

    if (tmp_remain_time > 150) {
        report_time = stat_info.copy_time_ - tmp_remain_time + 300;
    } else {
        report_time = stat_info.copy_time_ - tmp_remain_time;
    }

    if (appid_num > 0) {
        for (i = 0; i < appid_num; ++i) {
            report_pkgs[i].report_time_ = report_time;
            report_pkgs[i].service_type_ = stat_info.svr_info_.services_type_;
            report_pkgs[i].service_id_ = stat_info.svr_info_.services_id_;
            report_pkgs[i].count_ = 0;
        }
    }

    for (i = 0; i < array_size; ++i) {
        if (appid_num == 0) {
            continue;
        }
        for (j = 0; j < appid_num; ++j) {
            if (appid[j] == stat_info.statinfo_array_[i].statics_item_.item_id_.app_id_) {
                sec_proto::MonitorSvrReport &report_pkg = report_pkgs[j];
                item_count = report_pkg.count_;
                report_pkg.item_[item_count].app_id_ = appid[j];
                report_pkg.item_[item_count].classify_id_ =
                    stat_info.statinfo_array_[i].statics_item_.item_id_.classify_id_;
                report_pkg.item_[item_count].feature_id_ =
                    stat_info.statinfo_array_[i].statics_item_.item_id_.statics_id_;
                report_pkg.item_[item_count].feature_value_ =
                    stat_info.statinfo_array_[i].statics_item_.counter_;
                ZEN_Trace_LogMsg::debug_infoex(
                    "[server_type:%hu]|[server_id:%u]|[feature_id:%u]|[app_id:%u]|[classify_id:%u]------|feature_value=%llu",
                    stat_info.svr_info_.services_type_, stat_info.svr_info_.services_id_,
                    stat_info.statinfo_array_[i].statics_item_.item_id_.statics_id_,
                    stat_info.statinfo_array_[i].statics_item_.item_id_.app_id_,
                    stat_info.statinfo_array_[i].statics_item_.item_id_.classify_id_,
                    stat_info.statinfo_array_[i].statics_item_.counter_);
                ++report_pkg.count_;
                break;
            }
        }
    }

    for (i = 0; i < appid_num; ++i) {
        if (report_pkgs[i].count_ > 2048) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s] too much features, please check it",
                "int GunnerInfoReporter::pack_by_appid(const GunnerStatInfo&, unsigned int, const unsigned int*, std::vector<sec_proto::MonitorSvrReport, std::allocator<sec_proto::MonitorSvrReport> >&)");
            report_pkgs[i].count_ = 2048;
        }
    }
    return 0;
}
