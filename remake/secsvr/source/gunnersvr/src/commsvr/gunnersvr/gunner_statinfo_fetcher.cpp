// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_statinfo_fetcher.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"

unsigned int GunnerStatinfoFetcher::dump_timeinfo(const char *file_name) {
    int ret = ZEN_Server_Status::instance()->initialize(file_name);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("init mmap file %s failed", file_name);
        return ret;
    }
    return ZEN_Server_Status::instance()->get_copy_time();
}

int GunnerStatinfoFetcher::fetch_svrinfo_from_filename(const char *file_name,
                                                       GunnerStatInfo &stat_info) {
    return Comm_Stat_Monitor::get_svr_info_from_stat_fname(file_name,
                                                           &stat_info.svr_info_, NULL);
}

int GunnerStatinfoFetcher::dump(const char *file_name) {
    int ret = ZEN_Server_Status::instance()->initialize(file_name);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("init mmap file %s failed", file_name);
        return -1;
    }
    ZEN_Server_Status::instance()->dump_status_info(false);
    return 0;
}

int GunnerStatinfoFetcher::get_info(const char *file_name, GunnerStatInfo &stat_info) {
    ZEN_Server_Status server_status;
    int ret = server_status.initialize(file_name);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("init mmap file %s failed", file_name);
        return -1;
    }

    this->fetch_svrinfo_from_filename(file_name, stat_info);
    server_status.dump_all(stat_info.statinfo_array_, true);
    stat_info.copy_time_ = server_status.get_copy_time();
    return 0;
}
