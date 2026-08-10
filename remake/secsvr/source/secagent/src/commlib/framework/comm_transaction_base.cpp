// Reconstructed from secagent binary (DWARF + 反汇编, 2026-08-10)
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/framework/comm_transaction_base.cpp
// 语义与 secagent 二进制逐一对照（符号/偏移/字符串/返回码一致）。

#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_predefine.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_debug.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_security_check.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_transaction_base.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include <iomanip>
#include <sstream>
#include <typeinfo>

const int Transaction_Base::INVALID_PHASE_ID = -1;
const int Transaction_Base::TRANSACTION_TIME_ID[] = {1, 2};

Transaction_Base::Transaction_Base(ZEN_Timer_Queue *arg0,
                                   Transaction_Manager *arg1, bool arg2)
    : ZEN_Timer_Handler(arg0),
      trans_manager_(arg1),
      transaction_id_(0),
      trans_locker_(arg2),
      req_qq_uin_(0),
      trans_command_(0),
      trans_run_state_(INIT_RUN_STATE),
      trans_phase_(0),
      req_snd_service_(0, 0),
      req_rcv_service_(0, 0),
      req_proxy_service_(0, 0),
      req_trans_id_(0),
      req_session_id_(0),
      req_game_app_id_(0),
      req_ip_address_(0),
      req_frame_option_(0),
      tmp_recv_frame_(0),
      trans_timeout_id_(-1),
      trans_touchtimer_id_(-1),
      wait_cmd_(0),
      trans_create_time_(0),
      trace_log_pri_(RS_DEBUG),
      process_errno_(0) {
}

Transaction_Base::~Transaction_Base() {
}

void Transaction_Base::re_init() {
    transaction_id_ = 0;
    req_qq_uin_ = 0;
    trans_command_ = 0;
    trans_run_state_ = INIT_RUN_STATE;
    trans_phase_ = 0;
    req_snd_service_.set_serviceid(0, 0);
    req_rcv_service_.set_serviceid(0, 0);
    req_proxy_service_.set_serviceid(0, 0);
    req_trans_id_ = 0;
    req_session_id_ = 0;
    req_game_app_id_ = 0;
    req_ip_address_ = 0;
    req_frame_option_ = 0;
    tmp_recv_frame_ = 0;
    trans_timeout_id_ = -1;
    trans_touchtimer_id_ = -1;
    wait_cmd_ = 0;
    trans_create_time_ = 0;
    trace_log_pri_ = RS_DEBUG;
    process_errno_ = 0;
}

void Transaction_Base::finish() {
}

Transaction_Base::TRANSACTION_PROCESS Transaction_Base::on_timeout() {
    return EXIT_PROCESS_SUCC;
}

Transaction_Base::TRANSACTION_PROCESS Transaction_Base::on_exception() {
    return EXIT_PROCESS_SUCC;
}

int Transaction_Base::on_exit() {
    return 0;
}

int Transaction_Base::sendbuf_to_service(unsigned int cmd, unsigned int qquin,
                                         unsigned int transaction_id,
                                         unsigned int backfill_trans_id,
                                         const SERVICES_ID &rcv_svc,
                                         const SERVICES_ID &proxy_svc,
                                         const SERVICES_ID &snd_svc,
                                         const unsigned char *buf,
                                         size_t buf_len, unsigned int app_id,
                                         unsigned int option) {
    if (req_frame_option_ & 0x10000) {
        option |= 0x10000;
    }
    return trans_manager_->mgr_sendbuf_to_service(
        cmd, qquin, transaction_id, backfill_trans_id, rcv_svc, proxy_svc,
        snd_svc, buf, buf_len, app_id, option);
}

int Transaction_Base::response_buf_sendback(unsigned int cmd, unsigned int uin,
                                            const unsigned char *buf,
                                            unsigned int buf_len,
                                            unsigned int option) {
    if (req_frame_option_ & 0x80) {
        option |= 0x80;
    }
    return sendbuf_to_service(cmd, uin, transaction_id_, req_trans_id_,
                              req_snd_service_, req_rcv_service_,
                              req_proxy_service_, buf, buf_len,
                              req_game_app_id_, option);
}

int Transaction_Base::request_send_buf_to_peer(unsigned int cmd,
                                               unsigned int qquin,
                                               const SERVICES_ID &rcv_svc,
                                               const unsigned char *buf,
                                               unsigned int buf_len,
                                               unsigned int app_id,
                                               unsigned int option) {
    SERVICES_ID proxy_svc(0, 0);
    return sendbuf_to_service(cmd, qquin, transaction_id_, 0, rcv_svc,
                              proxy_svc, trans_manager_->self_services_id_,
                              buf, buf_len, app_id, option);
}

int Transaction_Base::request_send_buf_to_proxy(
    unsigned int cmd, unsigned int qquin, const SERVICES_ID &proxy_svc,
    const SERVICES_ID &recv_svc, const unsigned char *buf,
    unsigned int buf_len, unsigned int app_id, unsigned int option) {
    return sendbuf_to_service(cmd, qquin, transaction_id_, 0, recv_svc,
                              proxy_svc, trans_manager_->self_services_id_,
                              buf, buf_len, app_id, option);
}

void Transaction_Base::unlock_qquin_cmd() {
    trans_manager_->unlock_qquin_trans_cmd(req_qq_uin_, trans_command_);
}

void Transaction_Base::unlock_qquin_key(unsigned int one_key) {
    trans_manager_->unlock_qquin_trans_cmd(req_qq_uin_, one_key);
}

int Transaction_Base::lock_qquin_cmd() {
    return trans_manager_->lock_qquin_trnas_cmd(req_qq_uin_, trans_command_,
                                                trans_command_);
}

int Transaction_Base::lock_qquin_key(unsigned int one_key) {
    return trans_manager_->lock_qquin_trnas_cmd(req_qq_uin_, one_key,
                                                trans_command_);
}

int Transaction_Base::check_receive_frame(const Comm_App_Frame *recv_frame) {
    if (wait_cmd_ != 0 && wait_cmd_ != recv_frame->frame_command_) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] check_receive_frame error,Transaction need Cmd error!Wait command[%u],Recieve command[%u] Transaction ID:[%u].",
            wait_cmd_, recv_frame->frame_command_,
            recv_frame->transaction_id_);
        return 0x13ea;
    }
    wait_cmd_ = 0;
    return 0;
}

void Transaction_Base::cancel_timeout_timer() {
    if (trans_timeout_id_ != -1) {
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }
}

void Transaction_Base::cancel_touch_timer() {
    if (trans_touchtimer_id_ != -1) {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }
}

int Transaction_Base::close_request_service() const {
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] close_request_service() at trans_command_=%u,trans_phase_=%d,qq_uin_=%u.",
        trans_command_, trans_phase_, req_qq_uin_);
    return trans_manager_->mgr_sendmsghead_to_service(
        6, req_qq_uin_, req_rcv_service_, req_proxy_service_, 0, 0, 0);
}

int Transaction_Base::set_timetouch_timer(int sec, int usec) {
    if (trans_touchtimer_id_ != -1) {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }
    ZEN_Time_Value delay(sec, usec);
    trans_touchtimer_id_ = timer_queue()->schedule_timer(
        this, TRANSACTION_TIME_ID + 1, delay, ZEN_Time_Value::ZERO_TIME_VALUE);
    return 0;
}

int Transaction_Base::set_timeout_timer(int sec, int usec) {
    if (trans_timeout_id_ != -1) {
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }
    ZEN_Time_Value delay(sec, usec);
    trans_timeout_id_ = timer_queue()->schedule_timer(
        this, TRANSACTION_TIME_ID + 1, delay, ZEN_Time_Value::ZERO_TIME_VALUE);
    return 0;
}

int Transaction_Base::handle_close() {
    if (trans_timeout_id_ != -1) {
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }
    if (trans_touchtimer_id_ != -1) {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }
    return trans_manager_->unregiester_trans_id(
        transaction_id_, trans_command_, trans_run_state_, trans_create_time_);
}

int Transaction_Base::process_trans_event() {
    int ret;
    Transaction_Base::TRANSACTION_PROCESS txprocess;
    do {
        if (trans_timeout_id_ != -1) {
            timer_queue()->cancel_timer(trans_timeout_id_);
            trans_timeout_id_ = -1;
        }
        switch (trans_run_state_) {
        case INIT_RUN_STATE:
            ZEN_Trace_LogMsg::debug_debugEx(
                "[framework] %s::on_init start ,requst trans id:%u .",
                typeid(*this).name(), req_trans_id_);
            txprocess = on_init();
            ZEN_Trace_LogMsg::debug_debugEx(
                "[framework] %s::on_init end,new transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                txprocess, trans_phase_);
            if (txprocess != EXIT_PROCESS_SUCC
                && txprocess != EXIT_PROCESS_FAIL) {
                trans_run_state_ = RIGHT_RUN_STATE;
            }
            break;
        case RIGHT_RUN_STATE:
            ZEN_Trace_LogMsg::debug_debugEx(
                "[framework] %s::on_active start,transaction id:%u,requst trans id:%u,trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                trans_phase_);
            txprocess = on_active();
            ZEN_Trace_LogMsg::debug_debugEx(
                "[framework] %s::on_active end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d,.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                txprocess, trans_phase_);
            break;
        case TIMEOUT_RUN_STATE:
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] %s::on_timeout start,transaction id:%u,requst trans id:%u,trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                trans_phase_);
            txprocess = on_timeout();
            ZEN_Trace_LogMsg::debug_debugEx(
                "[framework] %s::on_timeout end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                txprocess, trans_phase_);
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x2392, req_game_app_id_, trans_command_, 1);
            break;
        case EXCEPTION_RUN_STATE:
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] %s::on_exception start,transaction id:%u,requst trans id:%u,trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                trans_phase_);
            txprocess = on_exception();
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] %s::on_exception end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                typeid(*this).name(), transaction_id_, req_trans_id_,
                txprocess, trans_phase_);
            break;
        default:
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x2390, req_game_app_id_, trans_command_, 1);
            return 0x13de;
        }
        switch (txprocess) {
        case WAIT_PROCESS:
            return 0;
        case NEXT_PROCESS:
            continue;
        case EXIT_PROCESS_SUCC:
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x2390, req_game_app_id_, trans_command_, 1);
            return 0x13de;
        case EXIT_PROCESS_FAIL:
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x2391, req_game_app_id_, trans_command_, 1);
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x2393, req_game_app_id_, process_errno_, 1);
            return 0x13de;
        default:
            return 0x13de;
        }
    } while (true);
}

int Transaction_Base::handle_timeout(const ZEN_Time_Value &arg0,
                                     const void *arg) {
    int ret;
    const int timeid = *(const int *)arg;
    if (timeid == TRANSACTION_TIME_ID[0]) {
        trans_timeout_id_ = -1;
        trans_run_state_ = TIMEOUT_RUN_STATE;
    } else if (timeid == TRANSACTION_TIME_ID[1]) {
        trans_touchtimer_id_ = -1;
    }
    ret = process_trans_event();
    if (ret != 0) {
        handle_close();
    }
    return 0;
}

int Transaction_Base::receive_trans_msg(Comm_App_Frame *proc_frame) {
    tmp_recv_frame_ = proc_frame;
    int ret = process_trans_event();
    if (ret == 0) {
        tmp_recv_frame_ = 0;
    }
    return ret;
}

int Transaction_Base::initialize_trans(Comm_App_Frame *proc_frame,
                                       unsigned int transid) {
    transaction_id_ = transid;
    trans_command_ = proc_frame->frame_command_;
    req_snd_service_.services_id_ = proc_frame->send_service_.services_id_;
    req_snd_service_.services_type_ = proc_frame->send_service_.services_type_;
    req_rcv_service_.services_id_ = proc_frame->recv_service_.services_id_;
    req_rcv_service_.services_type_ = proc_frame->recv_service_.services_type_;
    req_proxy_service_.services_id_ = proc_frame->proxy_service_.services_id_;
    req_proxy_service_.services_type_ = proc_frame->proxy_service_.services_type_;
    req_trans_id_ = proc_frame->transaction_id_;
    req_session_id_ = proc_frame->backfill_trans_id_;
    req_game_app_id_ = proc_frame->app_id_;
    req_qq_uin_ = proc_frame->frame_uin_;
    req_ip_address_ = proc_frame->send_ip_address_;
    req_frame_option_ = proc_frame->frame_option_;
    tmp_recv_frame_ = proc_frame;
    if (proc_frame->frame_option_ & 0x10000) {
        trace_log_pri_ = RS_INFO;
    }
    int ret = process_trans_event();
    if (ret == 0) {
        tmp_recv_frame_ = 0;
    }
    return ret;
}

void Transaction_Base::dump_transa_info(std::ostringstream &strstream) const {
    strstream << "ID:" << transaction_id_ << " Uin:" << req_qq_uin_
              << " Cmd:" << trans_command_ << " State:" << std::left
              << trans_run_state_ << " Phase:" << std::dec << trans_phase_
              << " "
              << "ReqSndSvr:" << req_snd_service_.services_type_ << " "
              << req_snd_service_.services_id_
              << " ReqRcvSvr:" << req_rcv_service_.services_type_ << " "
              << req_rcv_service_.services_id_
              << " Reqproxy:" << req_proxy_service_.services_type_ << " "
              << req_proxy_service_.services_id_ << " "
              << "ReqtransID:" << req_trans_id_
              << " TimeoutID:" << trans_timeout_id_
              << " TouchID:" << trans_touchtimer_id_ << " ";
}

void Transaction_Base::output_trans_info(const char *outstr) const {
    std::ostringstream strstream;
    dump_transa_info(strstream);
    ZEN_Trace_LogMsg::debug_infoex("[framework] %s:%s", outstr,
                                   strstream.str().c_str());
}
