// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_app.h"
#include "src/commsvr/gunnersvr/gunner_timer_handler.h"
#include "src/commsvr/gunnersvr/gunner_reporter.h"
#include "output/commlib/framework/release/include/comm_svrd_config.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"

#include <time.h>

GunnerSvrApp *GunnerSvrApp::instance_ = NULL;

// ORIG _ZN12GunnerSvrApp21process_recv_appframeEP14Comm_App_Frame
// @0x08070cc0..0x08070cc7 (size 0x7; DWARF low_pc/high_pc, decl_line 45):
//   push %ebp; mov %esp,%ebp; xor %eax,%eax; pop %ebp; ret
// => ORIG body is exactly "return 0;" (empty handler); nothing else to restore.
int GunnerSvrApp::process_recv_appframe(Comm_App_Frame *recv_frame) {
    return 0;
}

// ORIG _ZN12GunnerSvrApp4procERj @0x08070cd0..0x08070ce0 (size 0x10,
// DWARF low_pc/high_pc, param proc_data_num decl_line 62):
//   mov 0xc(%ebp),%eax; test %eax,%eax; jne 0x08070cdc;
//   0x08070cda: jmp 0x08070cda; 0x08070cdc: pop %ebp; xor %eax,%eax; ret
// => ORIG really contains this null-reference while loop; keep it verbatim.
int GunnerSvrApp::proc(size_t &proc_data_num) {
    while (&proc_data_num == NULL) {
    }
    return 0;
}

int GunnerSvrApp::load_app_conf() {
    int ret = 0;

    ret = config_->fromXmlFile(
        Comm_Svrd_Config::instance()->app_cfg_file_.c_str(), tsf4g_tdr::LIST_ENTRY, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], gunnersvrapp load config fail, ret=%d",
                                        "virtual int GunnerSvrApp::load_app_conf()", ret);
    }
    return ret;
}

int GunnerSvrApp::register_timer() {
    ZEN_Timer_Queue *timer_queue = ZEN_Timer_Queue::instance();
    ZEN_Time_Value delay;
    ZEN_Time_Value interval;

    if (timer_queue == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("ZEN timer queue init failed");
        return -1;
    }

    delay.set(0x131 - (int)time(NULL) % 0x12c, 0);
    interval.set(0x12c, 0);
    timer_queue->schedule_timer(
        new GunnerTimerHandler(timer_queue), NULL, delay, interval);
    return 0;
}

int GunnerSvrApp::init() {
    int ret = 0;

    ret = Comm_Stat_Monitor::instance()->initialize(
        false, Comm_Svrd_Config::instance()->self_svr_id_, 0, NULL, false);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("Zen_Server_Status init fail. ret=%d", ret);
        return ret;
    }

    ret = register_timer();
    ret = GunnerInfoReporter::instance()->init(config_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("monitor reporter init fail. ret=%d", ret);
        return ret;
    }
    set_log_priority((ZEN_LOG_PRIORITY)config_->log_priority_);
    return ret;
}

int GunnerSvrApp::reload() {
    GunnerInfoReporter::instance()->reload(config_);
    set_log_priority((ZEN_LOG_PRIORITY)config_->log_priority_);
    return 0;
}

GunnerSvrApp::~GunnerSvrApp() {
    delete config_;
    config_ = NULL;
}

GunnerSvrApp::GunnerSvrApp() {
    config_ = new conf_gunnersvr::config();
}

GunnerSvrApp *GunnerSvrApp::instance() {
    if (instance_ == NULL) {
        instance_ = new GunnerSvrApp;
    }
    return instance_;
}

// 显式实例化（原二进制中由服务 TU 发出的弱符号，含 comm_app_frame.h 模板）
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"

template int Comm_App_Frame::appdata_encode<sec_proto::CfgFileReq>(unsigned int, const sec_proto::CfgFileReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIpInfoReq>(unsigned int, const sec_proto::SvcIpInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::DeployInfoReq>(unsigned int, const sec_proto::DeployInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::MonitorSvrReport>(unsigned int, const sec_proto::MonitorSvrReport&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::LogSvrCommBillRecord>(unsigned int, const sec_proto::LogSvrCommBillRecord&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::CheckCfgFileUpdateReq>(unsigned int, const sec_proto::CheckCfgFileUpdateReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIdReq>(unsigned int, const sec_proto::SvcIdReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_decode<sec_proto::CfgFileRsp>(sec_proto::CfgFileRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIpInfoRsp>(sec_proto::SvcIpInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CfgsvrInfoRsp>(sec_proto::CfgsvrInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::DeployInfoRsp>(sec_proto::DeployInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CheckCfgFileUpdateRsp>(sec_proto::CheckCfgFileUpdateRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIdRsp>(sec_proto::SvcIdRsp&, unsigned int, size_t*) const;
