// Reconstructed from gunnersvr DWARF + disassembly (2026-08-10).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_cfgsvr_sdk.cpp
// 37 个 CfgSvrSdk 符号全部按二进制语义还原（串/命令字/返回码与反汇编一致）。

#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_thread.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_process.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_socket.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_error.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/protocol/common/release/include/comm_conf_cfgsdk.h"
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/TdrXml.h"
#include "import/include/tencore/tencrypt/TenHash.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_sndrcv_zulu.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace ZEN_OS {
ZEN_HANDLE open(const char *filename, int open_mode, mode_t perms);
int close(ZEN_HANDLE handle);
int read(ZEN_HANDLE handle, void *buffer, unsigned int buf_len);
int write(ZEN_HANDLE handle, const void *buffer, unsigned int buf_len);
ZEN_THREAD_ID pthread_self();
}

// line 22：与二进制 .data.rel.ro 中 DEFAULT_MASTER_CFGSVR_IP 指向的字符串一致。
const char * const DEFAULT_MASTER_CFGSVR_IP __attribute__((used)) = "";

// cfgsvr 交互命令字（来自二进制反汇编，comm_proto_public_cmd.h 为坏桩未含常量）。
namespace {
const unsigned int CFGSVR_QQUIN = 0x2710;
const unsigned int CMD_SVC_IP_INFO_REQ = 0x2713;
const unsigned int CMD_SVC_IP_INFO_RSP = 0x2714;
const unsigned int CMD_SVC_ID_REQ = 0x2715;
const unsigned int CMD_SVC_ID_RSP = 0x2716;
const unsigned int CMD_CFG_FILE_REQ = 0x2717;
const unsigned int CMD_CFG_FILE_RSP = 0x2718;
const unsigned int CMD_CHECK_CFG_FILE_UPDATE_REQ = 0x2729;
const unsigned int CMD_CHECK_CFG_FILE_UPDATE_RSP = 0x272a;
const unsigned int CMD_CFGSVR_INFO_REQ = 0x272b;
const unsigned int CMD_CFGSVR_INFO_RSP = 0x272c;
const unsigned int CMD_DEPLOY_INFO_REQ = 0x272d;
const unsigned int CMD_DEPLOY_INFO_RSP = 0x272e;
}

// 错误码（框架/通信约定，见二进制返回常量）。
enum {
    RET_GET_SVC_FAILED = 0x1428,   // 取 svc id / deploy info 失败
    RET_GET_IP_FAILED = 0x1429,    // 取 svc ip 失败
    RET_CFG_FILE_ERROR = 0x142a,   // 配置文件处理通用错误
    RET_CFGSVR_INFO_FAILED = 0x142b,
    RET_NO_SERVICE_TYPE = 0x142d,  // 取不到 service type
};

CfgSvrSdk::CfgSvrSdk()
    : Zen_Thread_Task(),
      self_info_(0, 0),
      master_cfgsvr_info_(0, 0),
      slave_cfgsvr_info_(0, 0),
      master_cfgsvr_ip_(),
      slave_cfgsvr_ip_(),
      cfg_name_list_(),
      config_update_list_(),
      file_buf_(0),
      curr_cfg_name_(),
      cfg_md5_map_(),
      is_pthread_start_(false),
      is_updade_config_(false) {
    cfgsdk_config_.construct();
    master_cfgsvr_ip_.set(DEFAULT_MASTER_CFGSVR_IP, MASTER_CFGSVR_PORT);
    svc_index_ = 0;
}

CfgSvrSdk::~CfgSvrSdk() {
    delete file_buf_;
    file_buf_ = 0;
}

CfgSvrSdk * CfgSvrSdk::instance() {
    if (instance_ == 0) {
        instance_ = new CfgSvrSdk();
    }
    return instance_;
}

void CfgSvrSdk::clean_instance() {
    if (instance_) {
        delete instance_;
        instance_ = 0;
    }
}

int CfgSvrSdk::init() {
    if (self_info_.services_type_ == SERVICES_ID::INVALID_SERVICES_TYPE) {
        if (Comm_Svrd_Config::instance()->self_svr_id_.services_type_ ==
            SERVICES_ID::INVALID_SERVICES_TYPE) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] can't get service type. use -t to set service type");
            return RET_NO_SERVICE_TYPE;
        }
        set_service_type(Comm_Svrd_Config::instance()->self_svr_id_.services_type_);
    }
    svc_index_ = Comm_Svrd_Config::instance()->instance_id_;
    master_cfgsvr_ip_.set(Comm_Svrd_Config::instance()->master_cfgsvr_ip_.c_str(),
                          MASTER_CFGSVR_PORT);
    int ret = 0;
    if (Comm_Svrd_Config::instance()->is_use_cfgsvr_) {
        master_cfgsvr_info_.set_serviceid(1, MASTER_CFGSVR_ID);
        ZEN_Trace_LogMsg::debug_infoex("[framework] try to get config from %s",
            master_cfgsvr_ip_.get_host_addr());
        ret = get_slave_cfgsvr_info();
        ZEN_Trace_LogMsg::debug_infoex("[framework] get slave cfgsvr info, ret=%d", ret);
        if (ret != 0) {
            ret = cfgsdk_config_.fromXmlFile("cfg/cfgsdk_config.xml",
                                             tsf4g_tdr::LIST_ENTRY, 0);
            if (ret != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] Get slave cfgsvr info error and load cfgsdk config error");
                return ret;
            }
            ZEN_Trace_LogMsg::debug_infoex(
                "[framework] Get slave cfgsvr info from cfgsdk_config.xml");
            slave_cfgsvr_info_.set_serviceid(cfgsdk_config_.slave_cfgsvr_type_,
                                             cfgsdk_config_.slave_cfgsvr_id_);
            slave_cfgsvr_ip_.set(cfgsdk_config_.slave_cfgsvr_ip_,
                                 cfgsdk_config_.slave_cfgsvr_port_);
        }
        ret = get_svc_id();
        if (ret != 0) {
            return ret;
        }
        ret = get_svc_deploy_info();
        if (ret != 0) {
            return ret;
        }
        if (cfgsdk_config_.toXmlFile("cfg/cfgsdk_config.xml",
                                     tsf4g_tdr::LIST_ENTRY, 0) != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[framework] Write cfgsdk_config.xml error");
        }
        file_buf_ = new unsigned char[MAX_FILE_LENGTH];
        ret = get_config_file();
        return ret;
    }
    ret = cfgsdk_config_.fromXmlFile("cfg/cfgsdk_config.xml",
                                     tsf4g_tdr::LIST_ENTRY, 0);
    if (ret == 0) {
        return 0;
    }
    ZEN_Trace_LogMsg::debug_infoex("[framework]Not find cfgsdk_config.xml");
    return ret;
}

int CfgSvrSdk::start_task() {
    int ret = 0;
    if (Comm_Svrd_Config::instance()->is_use_cfgsvr_) {
        const size_t ACTIVATE_TASK_GROUP = 0x1eb4a2;
        const size_t THREAD_STACK_SIZE = 0x7800000;
        ZEN_THREAD_ID threadid = 0;
        ret = Zen_Thread_Task::activate(ACTIVATE_TASK_GROUP, &threadid,
                                        0, THREAD_STACK_SIZE, 0);
    }
    return ret;
}

bool CfgSvrSdk::is_need_update_file() {
    return is_updade_config_;
}

void CfgSvrSdk::clear_file_update_flags() {
    is_updade_config_ = false;
}

unsigned int CfgSvrSdk::get_game_id() {
    return cfgsdk_config_.game_id_;
}

int CfgSvrSdk::get_idc_no() {
    return cfgsdk_config_.idc_no_;
}

unsigned short CfgSvrSdk::get_world_id() {
    return cfgsdk_config_.world_;
}

void CfgSvrSdk::set_service_type(unsigned short svc_type) {
    self_info_.services_type_ = svc_type;
}

void CfgSvrSdk::get_deploy_world_list(std::vector<unsigned short> &world_list) {
    world_list.clear();
    for (unsigned int i = 0; i < cfgsdk_config_.deploy_info_num_; ++i) {
        world_list.push_back(cfgsdk_config_.deploy_info_[i].world_);
    }
}

void CfgSvrSdk::get_config_md5(const string &cfg_name, CFG_MD5 &cfg_md5) {
    memset(&cfg_md5, 0, sizeof(cfg_md5));
    std::map<string, CFG_MD5>::const_iterator iter = cfg_md5_map_.find(cfg_name);
    if (iter != cfg_md5_map_.end()) {
        cfg_md5 = iter->second;
    }
}

int CfgSvrSdk::init_zulu_ex(Zulu_SendRecv_Package &zulu_ex,
                            const ZEN_Sockaddr_In &ip_info,
                            const SERVICES_ID &recv_info) {
    SERVICES_ID send_info(0, 0);
    // 6 = 配置服务器服务类型（见 get_slave_cfgsvr_info 的 set_serviceid(1, 1) 同族）
    send_info.set_serviceid(6, SERVICES_ID::DYNAMIC_ALLOC_SERVICES_ID);
    SERVICES_ID proxy_info(0, 0);
    int ret = zulu_ex.set_zulu_svcinfo(ip_info.get_host_addr(),
                                       ip_info.get_port_number(),
                                       recv_info, send_info, proxy_info, 0x10000);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::init_zulu_ex, set zulu svcinfo error, ret=%d", ret);
        return ret;
    }
    ZEN_Time_Value over_time(10, 0);
    ret = zulu_ex.connect_zulu_server(&over_time);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::init_zulu_ex, connect error, |%s|%d|, error:%s",
            ip_info.get_host_addr(), ip_info.get_port_number(), strerror(errno));
        return ret;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] CfgSvrSdk::init_zulu_ex, connect succ, |%s|%d|",
        ip_info.get_host_addr(), ip_info.get_port_number());
    return 0;
}

int CfgSvrSdk::get_svc_deploy_info() {
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_deploy_info, init zulu error, ret=%d", ret);
        return ret;
    }
    sec_proto::DeployInfoReq deploy_info_req;
    deploy_info_req.svc_type_ = self_info_.services_type_;
    deploy_info_req.svc_id_ = self_info_.services_id_;
    ret = zulu_ex.send_svc_package<sec_proto::DeployInfoReq>(
        CFGSVR_QQUIN, CMD_DEPLOY_INFO_REQ, deploy_info_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_deploy_info, send_svc_package failed, |%u|%u|, error:%s",
            self_info_.services_type_, self_info_.services_id_, strerror(errno));
        return ret;
    }
    sec_proto::DeployInfoRsp deploy_info_rsp;
    ret = zulu_ex.receive_svc_package<sec_proto::DeployInfoRsp>(
        CMD_DEPLOY_INFO_RSP, deploy_info_rsp, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_deploy_info, receive_svc_package failed, |%u|%u|, error:%s",
            self_info_.services_type_, self_info_.services_id_, strerror(errno));
        return ret;
    }
    if (deploy_info_rsp.result_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_deploy_info, get failed, |%u|%u|, result=%d",
            self_info_.services_type_, self_info_.services_id_, deploy_info_rsp.result_);
        return RET_GET_SVC_FAILED;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] CfgSvrSdk::get_svc_deploy_info, deploy_info_num=%u",
        deploy_info_rsp.deploy_info_num_);
    cfgsdk_config_.deploy_info_num_ = deploy_info_rsp.deploy_info_num_;
    memcpy(cfgsdk_config_.deploy_info_, deploy_info_rsp.deploy_info_,
           deploy_info_rsp.deploy_info_num_ * sizeof(conf_cfgsdk::DeployInfo));
    zulu_ex.close();
    return 0;
}

int CfgSvrSdk::get_svc_id() {
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_id, init zulu error, ret=%d", ret);
        return ret;
    }
    sec_proto::SvcIdReq svc_id_req;
    svc_id_req.svc_type_ = self_info_.services_type_;
    svc_id_req.svc_index_ = svc_index_;
    ret = zulu_ex.send_svc_package<sec_proto::SvcIdReq>(
        CFGSVR_QQUIN, CMD_SVC_ID_REQ, svc_id_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_id, send_svc_package failed, |%u|%u|, error:%s",
            self_info_.services_type_, svc_index_, strerror(errno));
        return ret;
    }
    sec_proto::SvcIdRsp svc_id_rsp;
    ret = zulu_ex.receive_svc_package<sec_proto::SvcIdRsp>(
        CMD_SVC_ID_RSP, svc_id_rsp, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_id, receive_svc_package failed, |%u|%u|, error:%s",
            self_info_.services_type_, svc_index_, strerror(errno));
        return ret;
    }
    if (svc_id_rsp.result_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svc_id, get svcid failed, |%u|%u|, result=%d",
            self_info_.services_type_, svc_index_, svc_id_rsp.result_);
        return RET_GET_SVC_FAILED;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] CfgSvrSdk::get_svc_id, get svcid succ, svc_id=%u, game_id=%u, idc_no=%u, world=%u",
        svc_id_rsp.svc_id_, svc_id_rsp.game_id_, svc_id_rsp.idc_no_, svc_id_rsp.world_id_);
    self_info_.services_id_ = svc_id_rsp.svc_id_;
    cfgsdk_config_.game_id_ = svc_id_rsp.game_id_;
    cfgsdk_config_.idc_no_ = svc_id_rsp.idc_no_;
    cfgsdk_config_.world_ = svc_id_rsp.world_id_;
    zulu_ex.close();
    return 0;
}

int CfgSvrSdk::get_ip_info(const SERVICES_ID &req_info, ZEN_Sockaddr_In &svr_inetadd,
                           unsigned int &svr_idc_no) {
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_ip_info, init zulu error, ret=%d", ret);
        return ret;
    }
    sec_proto::SvcIpInfoReq svc_ip_info_req;
    svc_ip_info_req.svc_type_ = req_info.services_type_;
    svc_ip_info_req.svc_id_ = req_info.services_id_;
    ret = zulu_ex.send_svc_package<sec_proto::SvcIpInfoReq>(
        CFGSVR_QQUIN, CMD_SVC_IP_INFO_REQ, svc_ip_info_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_ip_info, send_svc_package failed, error:%s",
            strerror(errno));
        return ret;
    }
    sec_proto::SvcIpInfoRsp svc_ip_info_rsp;
    ret = zulu_ex.receive_svc_package<sec_proto::SvcIpInfoRsp>(
        CMD_SVC_IP_INFO_RSP, svc_ip_info_rsp, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_ip_info, receive_svc_package failed, error:%s",
            strerror(errno));
        return ret;
    }
    if (svc_ip_info_rsp.result_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_ip_info, get ip failed, result=%d",
            svc_ip_info_rsp.result_);
        return RET_GET_IP_FAILED;
    }
    svr_inetadd.set(ntohl(svc_ip_info_rsp.bind_ip_), ntohs(svc_ip_info_rsp.bind_port_));
    svr_idc_no = svc_ip_info_rsp.idc_no_;
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] CfgSvrSdk::get_ip_info, get ip succ, |%s|%u|%u|%u|",
        svr_inetadd.get_host_addr(), svr_inetadd.get_port_number(),
        req_info.services_type_, req_info.services_id_);
    zulu_ex.close();
    return 0;
}

int CfgSvrSdk::get_slave_cfgsvr_info() {
    ZEN_Trace_LogMsg::debug_infoex("try to get slave cfgsvr info from %s:%d",
        master_cfgsvr_ip_.get_host_addr(), master_cfgsvr_ip_.get_port_number());
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, master_cfgsvr_ip_, master_cfgsvr_info_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_cfgsvr_info, init zulu error, ret=%d", ret);
        return ret;
    }
    sec_proto::SvcIdReq cfgsvr_info_req;
    cfgsvr_info_req.svc_type_ = self_info_.services_type_;
    cfgsvr_info_req.svc_index_ = svc_index_;
    ret = zulu_ex.send_svc_package<sec_proto::SvcIdReq>(
        CFGSVR_QQUIN, CMD_CFGSVR_INFO_REQ, cfgsvr_info_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_cfgsvr_info, send_svc_package failed, type=%u, index=%u, error:%s",
            self_info_.services_type_, svc_index_, strerror(errno));
        return ret;
    }
    sec_proto::CfgsvrInfoRsp cfgsvr_info_rsp;
    ret = zulu_ex.receive_svc_package<sec_proto::CfgsvrInfoRsp>(
        CMD_CFGSVR_INFO_RSP, cfgsvr_info_rsp, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_cfgsvr_info, receive_svc_package failed, type=%u, index=%u, error:%s",
            self_info_.services_type_, svc_index_, strerror(errno));
        return ret;
    }
    if (cfgsvr_info_rsp.result_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_cfgsvr_info, get failed, type=%u, index=%u, result=%d",
            self_info_.services_type_, svc_index_, cfgsvr_info_rsp.result_);
        return RET_CFGSVR_INFO_FAILED;
    }
    slave_cfgsvr_ip_.set(ntohl(cfgsvr_info_rsp.bind_ip_),
                         ntohs(cfgsvr_info_rsp.bind_port_));
    slave_cfgsvr_info_.set_serviceid(cfgsvr_info_rsp.svc_type_, cfgsvr_info_rsp.svc_id_);
    cfgsdk_config_.slave_cfgsvr_type_ = slave_cfgsvr_info_.services_type_;
    cfgsdk_config_.slave_cfgsvr_id_ = slave_cfgsvr_info_.services_id_;
    snprintf(cfgsdk_config_.slave_cfgsvr_ip_,
             sizeof(cfgsdk_config_.slave_cfgsvr_ip_) - 1,
             "%s", slave_cfgsvr_ip_.get_host_addr());
    cfgsdk_config_.slave_cfgsvr_ip_[sizeof(cfgsdk_config_.slave_cfgsvr_ip_) - 1] = 0;
    cfgsdk_config_.slave_cfgsvr_port_ = slave_cfgsvr_ip_.get_port_number();
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] CfgSvrSdk::get_cfgsvr_info, get succ, |%u|%u|%s|%u|",
        slave_cfgsvr_info_.services_type_, slave_cfgsvr_info_.services_id_,
        slave_cfgsvr_ip_.get_host_addr(), slave_cfgsvr_ip_.get_port_number());
    zulu_ex.close();
    return 0;
}

int CfgSvrSdk::uncompress_file(const char *cfg_name) {
    string cmd = "unzip -o cfg/";
    cmd.append(cfg_name, strlen(cfg_name));
    cmd.append(" -d cfg/ > log/uncompress.txt 2>&1", 0x22);
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] CfgSvrSdk::uncompress_file, cmd=%s", cmd.c_str());
    int ret = system(cmd.c_str());
    if (ret != 0) {
        if (ret == -1 && errno == ECHILD) {
            ret = 0;
        } else {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::uncompress_file, ret=%d, error=[%s] cmd=%s",
                ret, strerror(errno), cmd.c_str());
            ret = RET_CFG_FILE_ERROR;
        }
    }
    return ret;
}

int CfgSvrSdk::write_local_file() {
    string cfg_path = "cfg/";
    cfg_path.append(curr_cfg_name_);
    ZEN_HANDLE fd = ZEN_OS::open(cfg_path.c_str(), 0x242, 0x1b6);
    if (fd == -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::write_config_file, open error, %s, error:%s",
            cfg_path.c_str(), strerror(errno));
        return RET_CFG_FILE_ERROR;
    }
    int ret = ZEN_OS::write(fd, file_buf_, recv_len_);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::write_config_file, write error, %s, error:%s",
            cfg_path.c_str(), strerror(errno));
        return RET_CFG_FILE_ERROR;
    }
    ZEN_OS::close(fd);
    return 0;
}

int CfgSvrSdk::read_local_file(int &length) {
    string cfg_path = "cfg/";
    cfg_path.append(curr_cfg_name_);
    ZEN_HANDLE fd = ZEN_OS::open(cfg_path.c_str(), 0, 0x1b0);
    if (fd == -1) {
        return RET_CFG_FILE_ERROR;
    }
    int len = ZEN_OS::read(fd, file_buf_, MAX_FILE_LENGTH);
    if (len < 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::read_local_file, read error, %s, error:%s",
            cfg_path.c_str(), strerror(errno));
        return RET_CFG_FILE_ERROR;
    }
    ZEN_OS::close(fd);
    length = len;
    return 0;
}

void CfgSvrSdk::caculate_config_md5() {
    int length = 0;
    int ret = read_local_file(length);
    if (ret == 0) {
        TenMd5(file_buf_, length, local_cfg_md5_.md5_value_);
    }
}

void CfgSvrSdk::add_config(const char *cfg_name) {
    cfg_name_list_.push_back(string(cfg_name));
}

int CfgSvrSdk::get_config_update_list(Zulu_SendRecv_Package &zulu_ex) {
    config_update_list_.clear();
    sec_proto::CheckCfgFileUpdateReq check_update_req;
    check_update_req.svc_type_ = self_info_.services_type_;
    check_update_req.svc_id_ = self_info_.services_id_;
    check_update_req.cfg_num_ = 0;
    unsigned int cfg_num = 0;
    for (std::vector<string>::iterator iter = cfg_name_list_.begin();
         iter != cfg_name_list_.end(); ++iter) {
        CFG_MD5 cfg_md5;
        get_config_md5(*iter, cfg_md5);
        strncpy(check_update_req.cfg_md5_info_[cfg_num].cfg_name_,
                iter->c_str(), 0x80);
        memcpy(check_update_req.cfg_md5_info_[cfg_num].cfg_md5_,
               cfg_md5.md5_value_, 16);
        ++cfg_num;
    }
    check_update_req.cfg_num_ = cfg_num;
    int ret = zulu_ex.send_svc_package<sec_proto::CheckCfgFileUpdateReq>(
        CFGSVR_QQUIN, CMD_CHECK_CFG_FILE_UPDATE_REQ, check_update_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "CfgSvrSdk::get_config_update_list, send pkg error, error:%s",
            strerror(errno));
        return ret;
    }
    sec_proto::CheckCfgFileUpdateRsp check_update_rsp;
    ZEN_Time_Value time_out(10, 0);
    ret = zulu_ex.receive_svc_package<sec_proto::CheckCfgFileUpdateRsp>(
        CMD_CHECK_CFG_FILE_UPDATE_RSP, check_update_rsp, 0, &time_out);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "CfgSvrSdk::get_config_update_list, receive_svc_package failed, |%u|%u|, error:%s",
            self_info_.services_type_, self_info_.services_id_, strerror(errno));
        return ret;
    }
    for (unsigned int i = 0; i < check_update_rsp.cfg_num_; ++i) {
        string cfg_name(check_update_rsp.cfg_name_ + i * 128);
        config_update_list_.push_back(cfg_name);
    }
    return 0;
}

void CfgSvrSdk::insert_config_md5(const string &cfg_name, const CFG_MD5 &cfg_md5) {
    cfg_md5_map_.erase(cfg_name);
    cfg_md5_map_.insert(std::make_pair(cfg_name, cfg_md5));
}

int CfgSvrSdk::get_config_file(Zulu_SendRecv_Package &zulu_ex, const char *cfg_name) {
    recv_len_ = 0;
    curr_cfg_name_.assign(cfg_name, strlen(cfg_name));
    memset(&local_cfg_md5_, 0, sizeof(local_cfg_md5_));
    memset(&recv_cfg_md5_, 0, sizeof(recv_cfg_md5_));
    caculate_config_md5();
    int ret = 0;
    for (;;) {
        ret = send_config_file_req(zulu_ex);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, send pkg error,  error:%s",
                strerror(errno));
            return ret;
        }
        GET_CONFIG_RESULT get_config_result = CONFIG_IS_NEWEST;
        ret = recv_config_file_rsp(zulu_ex, get_config_result);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, recv pkg error, error:%s",
                strerror(errno));
            return ret;
        }
        if (get_config_result == CONFIG_IS_NEWEST) {
            insert_config_md5(curr_cfg_name_, local_cfg_md5_);
            return ret;
        }
        if (get_config_result == CONFIG_NEED_GET_MORE) {
            continue;
        }
        if (!check_config_md5()) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, md5 check error");
            return RET_CFG_FILE_ERROR;
        }
        insert_config_md5(curr_cfg_name_, recv_cfg_md5_);
        ret = write_local_file();
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, write file error, %s", cfg_name);
            return ret;
        }
        if (strcmp(cfg_name + strlen(cfg_name) - 4, ".zip") == 0) {
            ret = uncompress_file(cfg_name);
            if (ret != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] CfgSvrSdk::get_config_file, uncompress file error, %s",
                    cfg_name);
                return ret;
            }
        }
        return ret;
    }
}

int CfgSvrSdk::update_config_file(Zulu_SendRecv_Package &zulu_ex) {
    int ret = 0;
    for (std::vector<string>::iterator iter = config_update_list_.begin();
         iter != config_update_list_.end(); ++iter) {
        ret = get_config_file(zulu_ex, iter->c_str());
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}

int CfgSvrSdk::get_config_file() {
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_config_file, init zulu error, ret=%d", ret);
        return ret;
    }
    for (std::vector<string>::iterator iter = cfg_name_list_.begin();
         iter != cfg_name_list_.end(); ++iter) {
        ret = get_config_file(zulu_ex, iter->c_str());
        if (ret != 0) {
            return ret;
        }
    }
    zulu_ex.close();
    return 0;
}

int CfgSvrSdk::svc() {
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] [CfgSvrSdk] task start run, thread id=%u",
        ZEN_OS::pthread_self());
    is_pthread_start_ = true;
    Zulu_SendRecv_Package zulu_ex;
    int ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
    bool is_need_reconnect = false;
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::svc, init zulu error, ret=%d", ret);
        return ret;
    }
    for (;;) {
        ret = ZEN_OS::sleep(CHECK_UPDATE_TIME);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "CfgSvrSdk::svc, set timer error, ret=%d, errno=%d", ret, errno);
            return ret;
        }
        if (is_need_reconnect) {
            ret = init_zulu_ex(zulu_ex, slave_cfgsvr_ip_, slave_cfgsvr_info_);
            if (ret != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "CfgSvrSdk::svc, init zulu error, ret=%d", ret);
                continue;
            }
            is_need_reconnect = false;
        }
        if (is_updade_config_) {
            continue;
        }
        ret = get_config_update_list(zulu_ex);
        if (ret != 0) {
            if (ret == 0x13e5 || ret == 0x13e6 || ret == 0x1422) {
                is_need_reconnect = true;
            } else {
                ZEN_Trace_LogMsg::debug_errorex(
                    "CfgSvrSdk::svc, get update list error, ret=%d", ret);
            }
            continue;
        }
        if (config_update_list_.size() == 0) {
            continue;
        }
        ret = update_config_file(zulu_ex);
        if (ret != 0) {
            if (ret == 0x13e5 || ret == 0x13e6 || ret == 0x1422) {
                is_need_reconnect = true;
            } else {
                ZEN_Trace_LogMsg::debug_errorex(
                    "CfgSvrSdk::svc, update file error, ret=%d", ret);
            }
            continue;
        }
        config_update_list_.clear();
        is_updade_config_ = true;
    }
}

void CfgSvrSdk::update_config_name_list() {
    for (std::vector<string>::iterator update_iter = config_update_list_.begin();
         update_iter != config_update_list_.end(); ++update_iter) {
        bool is_exist = false;
        for (std::vector<string>::iterator iter = cfg_name_list_.begin();
             iter != cfg_name_list_.end(); ++iter) {
            if (*update_iter == *iter) {
                is_exist = true;
                break;
            }
        }
        if (!is_exist) {
            cfg_name_list_.push_back(*update_iter);
        }
    }
    config_update_list_.clear();
}

int CfgSvrSdk::send_config_file_req(Zulu_SendRecv_Package &zulu_ex) {
    sec_proto::CfgFileReq cfg_file_req;
    cfg_file_req.cfg_base_info_.svc_type_ = self_info_.services_type_;
    cfg_file_req.cfg_base_info_.svc_id_ = self_info_.services_id_;
    cfg_file_req.recv_len_ = recv_len_;
    snprintf(cfg_file_req.cfg_base_info_.cfg_name_,
             sizeof(cfg_file_req.cfg_base_info_.cfg_name_) - 1,
             "%s", curr_cfg_name_.c_str());
    memcpy(cfg_file_req.cfg_md5_, local_cfg_md5_.md5_value_, 16);
    int ret = zulu_ex.send_svc_package<sec_proto::CfgFileReq>(
        CFGSVR_QQUIN, CMD_CFG_FILE_REQ, cfg_file_req, 0, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_config_file, send pkg error, error:%s",
            strerror(errno));
    }
    return ret;
}

int CfgSvrSdk::recv_config_file_rsp(Zulu_SendRecv_Package &zulu_ex,
                                    GET_CONFIG_RESULT &get_config_result) {
    sec_proto::CfgFileRsp cfg_file_rsp;
    int ret = zulu_ex.receive_svc_package<sec_proto::CfgFileRsp>(
        CMD_CFG_FILE_RSP, cfg_file_rsp, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_svr_config_file, recv pkg error, error:%s",
            strerror(errno));
        return ret;
    }
    if (cfg_file_rsp.result_ != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] CfgSvrSdk::get_config_file, get config error, %s, result=%d",
            curr_cfg_name_.c_str(), cfg_file_rsp.result_);
        return RET_CFG_FILE_ERROR;
    }
    if (cfg_file_rsp.cfg_len_ != 0) {
        if (cfg_file_rsp.cfg_base_info_.svc_type_ != self_info_.services_type_ ||
            cfg_file_rsp.cfg_base_info_.svc_id_ != self_info_.services_id_ ||
            strcmp(cfg_file_rsp.cfg_base_info_.cfg_name_, curr_cfg_name_.c_str()) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, recv error file, file_name %s, recv_name %s",
                curr_cfg_name_.c_str(), cfg_file_rsp.cfg_base_info_.cfg_name_);
            return RET_CFG_FILE_ERROR;
        }
        if (recv_len_ + cfg_file_rsp.data_len_ > MAX_FILE_LENGTH) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, recv file too large, recv_len=%u, data_len_=%u",
                recv_len_, cfg_file_rsp.data_len_);
            return RET_CFG_FILE_ERROR;
        }
        memcpy(file_buf_ + recv_len_, cfg_file_rsp.data_buf_, cfg_file_rsp.data_len_);
        recv_len_ += cfg_file_rsp.data_len_;
        if (recv_len_ == cfg_file_rsp.cfg_len_) {
            memcpy(&recv_cfg_md5_, cfg_file_rsp.cfg_md5_, sizeof(recv_cfg_md5_));
            ZEN_Trace_LogMsg::debug_infoex(
                "[framework] CfgSvrSdk::get_config_file, get config file, |%s|%d|",
                curr_cfg_name_.c_str(), cfg_file_rsp.cfg_len_);
            get_config_result = CONFIG_IS_GET_OVER;
            return 0;
        }
        if (recv_len_ > cfg_file_rsp.cfg_len_) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] CfgSvrSdk::get_config_file, failed, recv_len=%u, cfg_len=%d",
                recv_len_, cfg_file_rsp.cfg_len_);
            return RET_CFG_FILE_ERROR;
        }
        get_config_result = CONFIG_NEED_GET_MORE;
        return 0;
    }
    get_config_result = CONFIG_IS_NEWEST;
    return 0;
}

bool CfgSvrSdk::check_config_md5() {
    unsigned char md5[16];
    TenMd5(file_buf_, recv_len_, md5);
    for (int i = 0; i < 16; ++i) {
        if (md5[i] != recv_cfg_md5_.md5_value_[i]) {
            return false;
        }
    }
    return true;
}

CfgSvrSdk *CfgSvrSdk::instance_ = NULL;

// 显式实例化（原二进制弱符号：Zulu_SendRecv_Package 模板由本 TU 发出）
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"

template int Zulu_SendRecv_Package::send_svc_package<sec_proto::CfgFileReq>(unsigned int, unsigned int, const sec_proto::CfgFileReq&, ZEN_Time_Value*, unsigned int, unsigned int);
template int Zulu_SendRecv_Package::send_svc_package<sec_proto::CheckCfgFileUpdateReq>(unsigned int, unsigned int, const sec_proto::CheckCfgFileUpdateReq&, ZEN_Time_Value*, unsigned int, unsigned int);
template int Zulu_SendRecv_Package::send_svc_package<sec_proto::DeployInfoReq>(unsigned int, unsigned int, const sec_proto::DeployInfoReq&, ZEN_Time_Value*, unsigned int, unsigned int);
template int Zulu_SendRecv_Package::send_svc_package<sec_proto::SvcIdReq>(unsigned int, unsigned int, const sec_proto::SvcIdReq&, ZEN_Time_Value*, unsigned int, unsigned int);
template int Zulu_SendRecv_Package::send_svc_package<sec_proto::SvcIpInfoReq>(unsigned int, unsigned int, const sec_proto::SvcIpInfoReq&, ZEN_Time_Value*, unsigned int, unsigned int);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::CfgFileRsp>(unsigned int, sec_proto::CfgFileRsp&, bool, ZEN_Time_Value*);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::CfgsvrInfoRsp>(unsigned int, sec_proto::CfgsvrInfoRsp&, bool, ZEN_Time_Value*);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::CheckCfgFileUpdateRsp>(unsigned int, sec_proto::CheckCfgFileUpdateRsp&, bool, ZEN_Time_Value*);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::DeployInfoRsp>(unsigned int, sec_proto::DeployInfoRsp&, bool, ZEN_Time_Value*);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::SvcIdRsp>(unsigned int, sec_proto::SvcIdRsp&, bool, ZEN_Time_Value*);
template int Zulu_SendRecv_Package::receive_svc_package<sec_proto::SvcIpInfoRsp>(unsigned int, sec_proto::SvcIpInfoRsp&, bool, ZEN_Time_Value*);
