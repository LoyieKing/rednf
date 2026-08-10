// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_channel_mgr.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"

#define GUNNER_MONITOR_REPORT_CMD 0x7531

// 原 TU 显式实例化的头文件内联模板（二进制 W 符号）
template int Comm_App_Frame::appdata_encode<sec_proto::MonitorSvrReport>(
    unsigned int szframe_appdata, const sec_proto::MonitorSvrReport &info,
    unsigned int data_start, size_t *sz_code);
template int Zulu_SendRecv_Package::send_svc_package<sec_proto::MonitorSvrReport>(
    unsigned int qq_uin, unsigned int cmd, const sec_proto::MonitorSvrReport &info,
    ZEN_Time_Value *time_wait, unsigned int app_id, unsigned int backfill_trans_id);

GunnerChannelMgr::GunnerChannelMgr() {
    channel_num_ = 0;
}

GunnerChannelMgr::~GunnerChannelMgr() {
}

unsigned int GunnerChannel::get_appid() {
    return monitor_info_.app_id_;
}

GunnerChannel *GunnerChannelMgr::get_channel(unsigned int appid) {
    if (channel_num_ == 0) {
        return NULL;
    }
    for (unsigned int i = 0; i < channel_num_; ++i) {
        if (gunner_channel_[i].get_appid() == appid) {
            return &gunner_channel_[i];
        }
    }
    return NULL;
}

void GunnerChannel::close() {
    zulu_.close();
}

void GunnerChannelMgr::close_channels() {
    default_channel_.close();
    for (unsigned int i = 0; i < channel_num_; ++i) {
        gunner_channel_[i].close();
    }
    channel_num_ = 0;
}

int GunnerChannel::send_to_monitor(sec_proto::MonitorSvrReport pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("[%s] send pkg to monitor, monitor ip:%s port:%hu",
                                    "int GunnerChannel::send_to_monitor(sec_proto::MonitorSvrReport)",
                                    monitor_info_.net_addr_.ip_, monitor_info_.net_addr_.port_);
    return zulu_.send_svc_package<sec_proto::MonitorSvrReport>(
        0, GUNNER_MONITOR_REPORT_CMD, pkg, NULL, 0, 0);
}

int GunnerChannel::connect_monitor() {
    SERVICES_ID selfsvr(7, 0xffffffff);
    SERVICES_ID recvsvc(3, 1);
    SERVICES_ID proxysvc(0, 0);
    ZEN_Time_Value time_out(2, 0);
    int ret;

    zulu_.set_zulu_svcinfo(monitor_info_.net_addr_.ip_, monitor_info_.net_addr_.port_,
                           recvsvc, selfsvr, proxysvc, 0x10000);
    ret = zulu_.connect_zulu_server(&time_out);
    if (ret == 0) {
        ZEN_Trace_LogMsg::debug_debugEx("[%s] connect monitor succ, monitor ip:%s port:%hu ",
                                        "int GunnerChannel::connect_monitor()",
                                        monitor_info_.net_addr_.ip_, monitor_info_.net_addr_.port_);
    } else {
        ZEN_Trace_LogMsg::debug_errorex("[%s] connect monitor failed, monitor ip:%s port:%hu ",
                                        "int GunnerChannel::connect_monitor()",
                                        monitor_info_.net_addr_.ip_, monitor_info_.net_addr_.port_);
    }
    return ret;
}

int GunnerChannel::init(const conf_gunnersvr::MonitorInfo &monitor_info) {
    monitor_info_ = monitor_info;
    return connect_monitor();
}

int GunnerChannelMgr::init_channels(LPCONFIG config) {
    int ret;
    unsigned int i;

    ret = default_channel_.init(config->default_monitor_addr_);
    if (ret != 0) {
        return ret;
    }
    if (config->monitor_info_count_ == 0) {
        return 0;
    }
    for (i = 0; i < config->monitor_info_count_; ++i) {
        ret = gunner_channel_[i].init(config->monitor_info_[i]);
        if (ret != 0) {
            break;
        }
        ++channel_num_;
    }
    return ret;
}
