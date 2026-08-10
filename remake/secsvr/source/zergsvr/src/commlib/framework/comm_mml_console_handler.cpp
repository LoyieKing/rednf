// Restored from zergsvr binary disassembly (2026-08-10).
// All 15 comm_mml_console_handler.cpp symbols reconstructed:
//   instance x2, clean_instance, handle_close, initialize, read_config,
//   get_handle, D0/D1/D2, C1/C2(ZEN_Reactor*), C1/C2(ZEN_Reactor*, ZEN_Sockaddr_In&),
//   handle_input.
// Error codes observed: read_config -> 0x1398, console disabled -> 0x13ad,
// datagram open fail -> 0x13ae.
#include "src/commlib/framework/comm_predefine.h"
#include <errno.h>
#include <string.h>
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_base.h"
#include "src/commlib/framework/comm_mml_console_handler.h"

MML_Console_Handler * MML_Console_Handler::instance_ = 0;

// line 244
void MML_Console_Handler::instance(MML_Console_Handler *instance) {
    instance_ = instance;
}

// line 249
MML_Console_Handler * MML_Console_Handler::instance() {
    return instance_;
}

// line 254
void MML_Console_Handler::clean_instance() {
    if (instance_ != 0) {
        instance_->handle_close();
        if (instance_ != 0)
            delete instance_;
        instance_ = 0;
    }
}

// line 146
int MML_Console_Handler::handle_close() {
    reactor()->remove_handler(this, false);
    dgram_socket_peer_.close();
    return 0;
}

// line 110
int MML_Console_Handler::initialize(const conf_zerg::zerg_config &config) {
    int ret = 0;

    dgram_socket_peer_.close();
    ret = read_config(config);
    if (ret != 0) {
        handle_close();
        return ret;
    }
    if (if_console_ == false) {
        handle_close();
        return 0x13ad;
    }
    ret = dgram_socket_peer_.open(&dgram_addr_, 2, 0, false);
    if (ret != 0) {
        handle_close();
        return 0x13ae;
    }
    reactor()->register_handler(this, 2);
    dgram_socket_peer_.sock_enable(0x800);
    return 0;
}

// line 52
int MML_Console_Handler::read_config(const conf_zerg::zerg_config &config) {
    int ret = 0;
    const unsigned int TMP_BUF_LEN = 0x200;
    char tmp_tip[TMP_BUF_LEN];

    if_console_ = (config.console_cfg.use_console == 1);
    if (if_console_ == false) {
        ZEN_Trace_LogMsg::debug_infoex("[framework] MML Console is close.");
        return 0;
    }
    snprintf(tmp_tip, TMP_BUF_LEN, "Get key console_cfg console_ip/port error.");
    ret = dgram_addr_.set(config.console_cfg.console_ip, config.console_cfg.console_port);
    if (ret != 0 || config.console_cfg.console_port <= 0x400) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.", tmp_tip);
        return 0x1398;
    }
    return 0;
}

// line 45
ZEN_SOCKET MML_Console_Handler::get_handle() const {
    return dgram_socket_peer_.get_handle();
}

// line 35
MML_Console_Handler::~MML_Console_Handler() {
    if (console_data_buf_ != 0) {
        delete console_data_buf_;
        console_data_buf_ = 0;
    }
}

// line 26
MML_Console_Handler::MML_Console_Handler(ZEN_Reactor *reactor)
    : ZEN_Event_Handler(reactor),
      if_console_(true),
      if_internal_(false),
      console_data_buf_(0),
      dgram_addr_(),
      dgram_socket_peer_(),
      remote_addr_(),
      console_command_() {
    console_data_buf_ = new char[MAX_DATA_BUFFER_LEN];
}

// line 15
MML_Console_Handler::MML_Console_Handler(ZEN_Reactor *reactor, ZEN_Sockaddr_In &dgram_addr)
    : ZEN_Event_Handler(reactor),
      if_console_(true),
      if_internal_(true),
      console_data_buf_(0),
      dgram_addr_(dgram_addr),
      dgram_socket_peer_(),
      remote_addr_(),
      console_command_() {
    console_data_buf_ = new char[MAX_DATA_BUFFER_LEN];
}

// line 160
int MML_Console_Handler::handle_input() {
    int ret = 0;
    ssize_t ssret = 0;
    size_t str_len = 0;
    bool bret = false;

    ssret = sendto(dgram_socket_peer_.socket_handle_, console_data_buf_, 0x10000, 0,
                   remote_addr_.get_addr(), remote_addr_.get_size());
    if (ssret <= 0) {
        if (errno == 11 || errno == 22)
            return 0;
        char *tmp_tip = (char *)dgram_addr_.get_host_addr();
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Recv a console command fail IP[%s|%u] ZEN_OS::last_error()=%d|%s.",
            tmp_tip, dgram_addr_.get_port_number(), errno, strerror(errno));
        return 0;
    }

    console_data_buf_[ssret] = 0;
    char *tmp_tip = (char *)remote_addr_.get_host_addr();
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] Recv a console command succ,remote dgram_addr_ %s|%u.",
        tmp_tip, remote_addr_.get_port_number());
    if (if_internal_ == true && dgram_addr_.is_internal() == false) {
        char *tmp_tip2 = (char *)dgram_addr_.get_host_addr();
        ZEN_Trace_LogMsg::debug_debugEx(
            "[framework] Recv a console command from internet,not internal,remote dgram_addr_ [%s|%u].",
            tmp_tip2, dgram_addr_.get_port_number());
        return 0;
    }

    str_len = 0x10000;
    ret = console_command_.ParseMMLCommandString2(console_data_buf_);
    if (ret == 0) {
        process_mml_command(console_data_buf_, str_len);
    } else {
        str_len = snprintf(console_data_buf_, 0x10000, "MML command error...");
    }
    console_data_buf_[0x10000] = 0;
    if (str_len != 0) {
        sendto(dgram_socket_peer_.socket_handle_, console_data_buf_, str_len, 0,
               remote_addr_.get_addr(), remote_addr_.get_size());
    }
    return 0;
}
