// Reconstructed from gunnersvr disassembly (DWARF + .rodata + mnemonic check).
#include "src/commsvr/gunnersvr/gunner_reporter.h"
#include "src/commsvr/gunnersvr/gunner_tool.h"
#include "src/commlib/zenlib/zen_config_getopt.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>

GunnerTool *GunnerTool::instance_ = NULL;

GunnerTool::GunnerTool() {
    server_type_ = 0;
    server_id_ = 0;
    app_id_ = 0;
    feature_id_ = 0;
    feature_vaule_ = 0;
}

GunnerTool::~GunnerTool() {
}

GunnerTool *GunnerTool::instance() {
    if (instance_ == NULL) {
        instance_ = new GunnerTool;
    }
    return instance_;
}

void GunnerTool::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

void GunnerTool::init() {
    ZEN_Trace_LogMsg::instance()->set_log_priority(RS_INFO);
}

int GunnerTool::proc_cmd(int argc, const char **argv) {
    int cmd;
    int ret = 0;

    init();
    cmd = analyze_cmd(argc, argv);
    switch (cmd) {
    case 1:
        ret = statinfo_fetcher_.dump(file_name_);
        if (ret < 0) {
            ZEN_Trace_LogMsg::debug_errorex("print statinfo error");
        }
        break;
    case 2:
        ret = print_monitor_info();
        if (ret < 0) {
            ZEN_Trace_LogMsg::debug_errorex("print monitor info error");
        }
        break;
    case 3:
        print_update_time();
        break;
    case 5:
        report_gunner_info();
        break;
    default:
        usage();
        break;
    }
    return ret;
}

int GunnerTool::analyze_cmd(int argc, const char **argv) {
    ZEN_Get_Option get_opt(argc, (char **)argv, "v::r::h::m::s::l::t:i:q:e:f:a:", 1, 0, 3, 0);
    char c;
    int cmd = 0;

    while ((c = get_opt()) != (char)0xff) {
        switch (c) {
        case 'a':
            app_id_ = strtoul(get_opt.opt_arg(), NULL, 0);
            break;
        case 'e': {
            std::stringstream str_value;
            str_value << get_opt.opt_arg();
            str_value >> feature_vaule_;
            break;
        }
        case 'f':
            snprintf(file_name_, MAX_SHM_FILE_NAME_LEN, "%s", get_opt.opt_arg());
            file_name_[MAX_SHM_FILE_NAME_LEN] = '\0';
            break;
        case 'h':
            cmd = 4;
            break;
        case 'i':
            server_id_ = strtoul(get_opt.opt_arg(), NULL, 0);
            break;
        case 'l':
            cmd = 3;
            break;
        case 'm':
            cmd = 2;
            break;
        case 'q':
            feature_id_ = strtoul(get_opt.opt_arg(), NULL, 0);
            break;
        case 'r':
            cmd = 5;
            break;
        case 's':
            cmd = 1;
            break;
        case 't':
            server_type_ = strtoul(get_opt.opt_arg(), NULL, 0);
            break;
        case 'v':
            ZEN_Trace_LogMsg::debug_infoex("version : %d, date : %d", 1,
                                           "Jan 15 2013");
            exit(0);
            break;
        default:
            break;
        }
    }

    if (memcmp(file_name_, "ZERG", 4) == 0) {
        snprintf(file_name_, MAX_SHM_FILE_NAME_LEN, "ZERG_%d_%d.STAT",
                 server_type_, server_id_);
    } else {
        snprintf(file_name_, MAX_SHM_FILE_NAME_LEN, "TSS_%d_%d.STAT",
                 server_type_, server_id_);
    }
    file_name_[MAX_SHM_FILE_NAME_LEN] = '\0';
    ZEN_OS::strupr(file_name_);
    return cmd;
}

void GunnerTool::usage() {
    std::cout << "usage of shm monitor-upload tool" << std::endl;
    std::cout << "   -d run the gunner app" << std::endl;
    std::cout << "   -f [name of shm-file prefix]" << std::endl;
    std::cout << "   -t [server type]" << std::endl;
    std::cout << "   -i [server id]" << std::endl;
    std::cout << "   -q [feature id]" << std::endl;
    std::cout << "   -e [feature value]" << std::endl;
    std::cout << "   -a [app id]" << std::endl;
    std::cout << "   -m show monitor info" << std::endl;
    std::cout << "   -s display shm-file info" << std::endl;
    std::cout << "   -l show shm-file update time" << std::endl;
    std::cout << "   -v show version" << std::endl;
    std::cout << "   -h show help info." << std::endl;
    std::cout << "   -r report stat info" << std::endl;
}

void GunnerTool::print_update_time() {
    unsigned int report_time = statinfo_fetcher_.dump_timeinfo(file_name_);
    timeval tv;
    char time_str[MAX_TIME_STR_LEN];

    tv.tv_sec = report_time;
    tv.tv_usec = 0;
    ZEN_Trace_LogMsg::debug_infoex("shm-file head info");
    ZEN_OS::timestamp(&tv, time_str, MAX_TIME_STR_LEN);
    ZEN_Trace_LogMsg::debug_infoex("shm-file time = %s", time_str);

    tv.tv_sec = time(NULL);
    tv.tv_usec = 0;
    ZEN_OS::timestamp(&tv, time_str, MAX_TIME_STR_LEN);
    ZEN_Trace_LogMsg::debug_infoex("current  time = %s", time_str);
}

int GunnerTool::print_monitor_info() {
    ZEN_Trace_LogMsg::debug_infoex("not support, go to see the config file");
    return 0;
}

int GunnerTool::report_gunner_info() {
    GunnerStatInfo gunner_info;
    ZEN_STATUS_ITEM_WITHNAME report_item;
    timeval now;
    GunnerChannelMgr::LPCONFIG config;
    int ret;

    gettimeofday(&now, NULL);
    gunner_info.copy_time_ = now.tv_sec;
    gunner_info.svr_info_.services_id_ = server_id_;
    gunner_info.svr_info_.services_type_ = server_type_;

    report_item.statics_item_.counter_ = feature_vaule_;
    report_item.statics_item_.item_id_.statics_id_ = feature_id_;
    report_item.statics_item_.item_id_.classify_id_ = 0;
    report_item.statics_item_.item_id_.app_id_ = app_id_;
    gunner_info.statinfo_array_.push_back(report_item);

    config = new conf_gunnersvr::config();
    ret = config->fromXmlFile("cfg/gunnersvr_config.xml", tsf4g_tdr::LIST_ENTRY, 0);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("config init error, ret = %d", ret);
        return ret;
    }

    GunnerInfoReporter::instance()->init(config);
    GunnerInfoReporter::instance()->report_statinfo_to_monitor(gunner_info);
    delete config;
    return 0;
}
