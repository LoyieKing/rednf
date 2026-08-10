// Reconstructed from zergsvr disassembly (2026-08-10).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_configure.cpp

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_service_config.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_socket_addr_in.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_udp_ctrl_handler.h"

using std::endl;

// 静态状态项数组（二进制 __static_initialization 逐个构造，本文件未消费）
static ZEN_STATUS_ITEM_WITHNAME zerg_comm_stat_items_[16] = {
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

ZERG_SERVICES_INFO Zerg_Server_Config::self_svc_info_;
SELF_SERVICESINFO_LIST Zerg_Server_Config::slave_svc_ary_;
Zerg_Server_Config *Zerg_Server_Config::instance_ = NULL;

// line 37
ZERG_SERVICES_INFO::ZERG_SERVICES_INFO()
    : zerg_svc_info_(0, 0), zerg_sessionkey_(true),
      zerg_ip_addr_("1.1.1.1", 0x1ad4) {
}

// line 44
ZERG_SERVICES_INFO::~ZERG_SERVICES_INFO() {
}

// line 53
Zerg_Server_Config::Zerg_Server_Config() : services_info_cfg_(0x800) {
    zerg_insurance_ = true;
    get_svc_ip_way_ = FROM_SERVICES_INFO_FILE;
    zerg_need_opkey_ = false;
    zerg_udp_session_ = false;
    zerg_stat_file_.assign("./ZERGLING.STAT", 15);
    cmd_stat_file_.assign("./ZERG.CMD.STAT", 15);
    svcid_info_.construct();
}

// line 66
Zerg_Server_Config::~Zerg_Server_Config() {
}

// line 137
void Zerg_Server_Config::dump_status_info(ostringstream &ostr_stream) {
    unsigned int i;

    ostr_stream << "Dump Zerg_Server_Config StatusInfo:" << endl;
    ostr_stream.width(24);
    ostr_stream << "SELF_SVR_INFO :"
                << self_svc_info_.zerg_svc_info_.services_type_ << "|"
                << self_svc_info_.zerg_svc_info_.services_id_ << " SEESIONKEY:"
                << self_svc_info_.zerg_sessionkey_ << " IP:"
                << self_svc_info_.zerg_ip_addr_.get_host_addr() << "|"
                << self_svc_info_.zerg_ip_addr_.get_port_number() << endl;
    for (i = 0; i < slave_svc_ary_.size(); i++) {
        ostr_stream << "SLAVE_SVR_INFO :"
                    << slave_svc_ary_[i].zerg_svc_info_.services_type_ << "|"
                    << slave_svc_ary_[i].zerg_svc_info_.services_id_
                    << " SEESIONKEY:" << slave_svc_ary_[i].zerg_sessionkey_
                    << " IP:"
                    << slave_svc_ary_[i].zerg_ip_addr_.get_host_addr() << "|"
                    << slave_svc_ary_[i].zerg_ip_addr_.get_port_number() << endl;
    }
    ostr_stream.width(24);
    ostr_stream << "RESTORE_PIPE:"
                << Comm_Svrd_Config::instance()->if_restore_pipe_ << endl;
    ostr_stream.width(24);
    ostr_stream << "SERVICES_CONFG_PATH:"
                << Comm_Svrd_Config::instance()->svcid_cfg_file_ << endl;
    ostr_stream.width(24);
    ostr_stream << "ZERG_CONFG_PATH:"
                << Comm_Svrd_Config::instance()->zerg_cfg_file_ << endl;
    ostr_stream.width(24);
    ostr_stream << "ZERG_STAT_PATH:" << zerg_stat_file_ << endl;
    ostr_stream.width(24);
    ostr_stream << "LOG_FILE_PREFIX :"
                << Comm_Svrd_Config::instance()->log_file_prefix_ << endl;
}

// line 73
int Zerg_Server_Config::GetServicesIPInfo(const SERVICES_ID &svc_info,
                                          ZEN_Sockaddr_In &ipaddr) {
    int ret = 0;
    unsigned int idc_no = 0;
    char ip_info[0x28];
    unsigned int server_guid_no = 0;
    int num;

    if (get_svc_ip_way_ == FROM_SVC_CONFIG_CENTER) {
        ret = CfgSvrSdk::instance()->get_ip_info(svc_info, ipaddr,
                                                 server_guid_no);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Can't get svc [%u|%u] ip info from config center .",
                svc_info.services_type_, svc_info.services_id_);
            return ret;
        }
        ipaddr.get_host_addr_port(ip_info, 0x28);
        num = svcid_info_.service_num_;
        svcid_info_.service_info_[num].svr_type_ = svc_info.services_type_;
        svcid_info_.service_info_[num].svr_id_ = svc_info.services_id_;
        ipaddr.get_host_addr(svcid_info_.service_info_[num].svr_ip_, 0x10);
        svcid_info_.service_info_[num].svr_port_ = ipaddr.get_port_number();
        svcid_info_.service_info_[num].svr_idc_ = server_guid_no;
        svcid_info_.service_num_++;
    } else if (get_svc_ip_way_ == FROM_SERVICES_INFO_FILE) {
        idc_no = 0;
        ret = services_info_cfg_.FindServicesIPConf(svc_info, ipaddr, idc_no,
                                                    server_guid_no, NULL);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Can't get svc [%u|%u] ip info from services information file. .",
                svc_info.services_type_, svc_info.services_id_);
            return ret;
        }
    } else {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Svc_info [%u|%u] ipaddress info [%s|%u].",
            svc_info.services_type_, svc_info.services_id_,
            ipaddr.get_host_addr(), ipaddr.get_port_number());
        return 0;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Svc_info [%u|%u] ipaddress info [%s|%u].",
        svc_info.services_type_, svc_info.services_id_,
        ipaddr.get_host_addr(), ipaddr.get_port_number());
    return 0;
}

// line 162
Zerg_Server_Config * Zerg_Server_Config::instance() {
    if (instance_ == NULL) {
        instance_ = new Zerg_Server_Config();
    }
    return instance_;
}

// line 174
void Zerg_Server_Config::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

// line 185
int Zerg_Server_Config::init(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config) {
    int ret = 0;
    string tmpstr;
    const size_t LEN_TMP_BUFFER = 0x101;
    char key_buf[LEN_TMP_BUFFER];
    char err_buf[LEN_TMP_BUFFER];
    char out_buf[LEN_TMP_BUFFER];
    ZEN_Sockaddr_In inetadd;
    unsigned int slave_num;
    unsigned int i;
    ZERG_SERVICES_INFO slave_svc_info;

    config_ = config;
    memset(err_buf, 0, LEN_TMP_BUFFER);
    memset(key_buf, 0, LEN_TMP_BUFFER);
    memset(out_buf, 0, LEN_TMP_BUFFER);

    if (strcasecmp(config_->comm_cfg.get_svr_info_type, "cfgsvr") == 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Get IP define from config center.");
        get_svc_ip_way_ = FROM_SVC_CONFIG_CENTER;
        if (!Comm_Svrd_Config::instance()->is_use_cfgsvr_) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Not pull config but get ip from cfgsvr");
            return 0x114933b7;
        }
    } else if (strcasecmp(config_->comm_cfg.get_svr_info_type, "cfgfile") == 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Get IP define from services info config file.");
        get_svc_ip_way_ = FROM_SERVICES_INFO_FILE;
        ret = services_info_cfg_.LoadSvrConfig(
            &Comm_Svrd_Config::instance()->svcid_config_);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Get configure file error. %s.", "Load svr config.");
            return 0x1398;
        }
    }

    ret = TCP_Svc_Handler::get_tcpctrl_conf(config_);
    if (ret != 0) {
        return ret;
    }
    ret = UDP_Svc_Handler::get_udpctrl_conf(config_);
    if (ret != 0) {
        return ret;
    }
    self_svc_info_.zerg_svc_info_ = Comm_Svrd_Config::instance()->self_svr_id_;
    snprintf(err_buf, 0x100, "SELFCFG|SELFSVRTYPE|SELFSVRID key error.");
    ret = GetServicesIPInfo(self_svc_info_.zerg_svc_info_, inetadd);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Self svc id [%u|%u] ip address [%s] ",
        self_svc_info_.zerg_svc_info_.services_type_,
        self_svc_info_.zerg_svc_info_.services_id_,
        inetadd.get_host_addr_port(out_buf, 0x100));
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.", err_buf);
        return 0x1398;
    }
    self_svc_info_.zerg_ip_addr_ = inetadd;
    self_svc_info_.zerg_sessionkey_ =
        (config_->comm_cfg.insurance == 1);
    if (self_svc_info_.zerg_sessionkey_) {
        zerg_need_opkey_ = true;
        if (self_svc_info_.zerg_svc_info_.services_type_ > 0x7530) {
            zerg_udp_session_ = true;
        }
    }
    slave_num = config_->self_cfg.slave_svr_count;
    if (slave_num > 3) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.", err_buf);
        return 0x1398;
    }
    snprintf(key_buf, 0x100, "./ZERGLING.%u.%u.STAT",
             self_svc_info_.zerg_svc_info_.services_type_,
             self_svc_info_.zerg_svc_info_.services_id_);
    zerg_stat_file_.assign(key_buf, strlen(key_buf));
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Slave peer number [%u].",
                                   slave_num);
    for (i = 0; i < slave_num; i++) {
        slave_svc_info = ZERG_SERVICES_INFO();
        slave_svc_info.zerg_svc_info_.services_type_ =
            config_->self_cfg.slave_svrs[i].svr_type;
        if (slave_svc_info.zerg_svc_info_.services_type_ == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Get configure file error. %s.", err_buf);
            return 0x1398;
        }
        slave_svc_info.zerg_svc_info_.services_id_ =
            config_->self_cfg.slave_svrs[i].svr_id;
        if (slave_svc_info.zerg_svc_info_.services_id_ == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Get configure file error. %s.", err_buf);
            return 0x1398;
        }
        ret = GetServicesIPInfo(slave_svc_info.zerg_svc_info_, inetadd);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Get configure file error. %s.", err_buf);
            return 0x1398;
        }
        slave_svc_info.zerg_ip_addr_ = inetadd;
        slave_svc_info.zerg_sessionkey_ =
            (config_->self_cfg.slave_svrs[i].use_encrypt == 1);
        if (slave_svc_info.zerg_sessionkey_) {
            zerg_need_opkey_ = true;
            if (slave_svc_info.zerg_svc_info_.services_type_ > 0x7530) {
                zerg_udp_session_ = true;
            }
        }
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Slave [%u] svc id [%u|%u] [%s] ip address  [%s] . ",
            i, slave_svc_info.zerg_svc_info_.services_type_,
            slave_svc_info.zerg_svc_info_.services_id_,
            (slave_svc_info.zerg_svc_info_.services_type_ > 0x7530)
                ? "UDP"
                : "TCP",
            slave_svc_info.zerg_ip_addr_.get_host_addr_port(out_buf, 0x100));
        slave_svc_ary_.push_back(slave_svc_info);
    }
    if (strcasecmp(config_->comm_cfg.get_svr_info_type, "cfgfile") == 0) {
        ret = svcid_info_.toXmlFile("cfg/svcid.xml",
                                    (tsf4g_tdr::TdrXmlFormat)0, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[zergsvr] Write svcid.xml error");
        }
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Config session useful life %u.", 0x7080);
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Get File Configure Success.");
    return 0;
}
