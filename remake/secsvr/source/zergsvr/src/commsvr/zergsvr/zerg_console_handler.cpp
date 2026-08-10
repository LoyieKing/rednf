// Reconstructed from zergsvr disassembly (2026-08-10).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_console_handler.cpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#include "src/commlib/framework/comm_mml_command.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_os_adapt_time.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commsvr/zergsvr/zerg_console_handler.h"
#include "src/commsvr/zergsvr/zerg_application.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"

// line 24
Zerg_Console_Handler::Zerg_Console_Handler(ZEN_Reactor *reactor)
    : MML_Console_Handler(reactor) {
}

// line 29
Zerg_Console_Handler::~Zerg_Console_Handler() {
}

// line 228
int Zerg_Console_Handler::cmd_close_services(char *ret_string, size_t &str_len) {
    size_t tmp_size;

    Zerg_Service_App::instance()->set_run_sign(false);
    tmp_size = snprintf(
        ret_string, str_len,
        "Console close services. Server exit at immediately,Please wait.");
    str_len = tmp_size;
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] %s", ret_string);
    return 0;
}

// line 292
int Zerg_Console_Handler::cmd_modify_logpriority(char *ret_string,
                                                 size_t &str_len) {
    int ret;
    string tmpstr("PRIORITY");
    string param_str;
    ZEN_LOG_PRIORITY new_log_priority;
    ZEN_LOG_PRIORITY old_log_priority;
    size_t tmp_size;

    ret = console_command_.GetCmdParameter(tmpstr, param_str);
    if (ret == 0) {
        new_log_priority = ZEN_LogTrace_Basic::log_priorities(param_str.c_str());
        old_log_priority = Zerg_Service_App::instance()->get_log_priority();
        Zerg_Service_App::instance()->set_log_priority(new_log_priority);
        tmp_size = snprintf(
            ret_string, str_len,
            "Console modify log priority.Old log priority %u, New log priority %u .",
            old_log_priority, new_log_priority);
        str_len = tmp_size;
        ZEN_Trace_LogMsg::debug_infoex("[zergsvr] %s", ret_string);
    }
    return ret;
}

// line 240
int Zerg_Console_Handler::cmd_close_socketpeer(char *ret_string,
                                               size_t &str_len) {
    int ret;
    size_t tmp_size;
    SERVICES_ID svr_info(0, 0);
    string svc_type_cmd("SVCTYPE");
    string svc_type_param;
    string svc_id_cmd("SVCID");
    string svc_id_param;

    ret = console_command_.GetCmdParameter(svc_type_cmd, svc_type_param);
    if (ret != 0) {
        return ret;
    }
    svr_info.services_type_ =
        (unsigned short)strtol(svc_type_param.c_str(), NULL, 10);
    ret = console_command_.GetCmdParameter(svc_id_cmd, svc_id_param);
    if (ret != 0) {
        return ret;
    }
    svr_info.services_id_ = (unsigned int)strtol(svc_id_param.c_str(), NULL, 10);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Console To close One Services:%u|%u",
        svr_info.services_id_, svr_info.services_type_);
    ret = TCP_Svc_Handler::close_services_peer(svr_info);
    tmp_size = snprintf(ret_string, str_len,
                        "Console To close One Services: %u|%u %s.",
                        svr_info.services_type_, svr_info.services_id_,
                        ret == 0 ? "Success" : "Fail");
    str_len = tmp_size;
    return 0;
}

// line 137
int Zerg_Console_Handler::cmd_get_zergstats(char *ret_string, size_t &str_len) {
    size_t tmp_size;
    char day_and_time[0x23];
    ostringstream tmp_ostrstm;

    ZEN_OS::timestamp(day_and_time, 0x23);
    tmp_size = snprintf(ret_string, str_len,
                        "Console Get Zerg(Tcpsvrd) services info.");
    tmp_size += snprintf(ret_string + tmp_size, str_len - tmp_size,
                         "Server Time %s.", day_and_time);
    Zerg_Server_Config::instance()->dump_status_info(tmp_ostrstm);
    Comm_Stat_Monitor::instance()->dump_status_info(tmp_ostrstm, false);
    TCP_Svc_Handler::dump_status_staticinfo(tmp_ostrstm);
    string str_tmp = tmp_ostrstm.str();
    tmp_size += snprintf(ret_string + tmp_size, str_len - tmp_size, "%s",
                         str_tmp.c_str());
    tmp_size += snprintf(ret_string + tmp_size, str_len - tmp_size,
                         "Have fun my brother.Author: Pet2 Team .");
    str_len = tmp_size;
    return 0;
}

// line 180
int Zerg_Console_Handler::cmd_get_peer_stats(char *ret_string,
                                             size_t &str_len) {
    int ret;
    size_t tmp_size;
    size_t startno;
    size_t numquery;
    string tmpstr;
    const size_t MAX_QUERY_NUMBER = 0x400;
    ostringstream ostr_stream;

    tmp_size = snprintf(ret_string, str_len,
                        "Console Get Zerg(Tcpsvrd) services info.");
    string startno_cmd("STARTNO");
    string startno_param;
    ret = console_command_.GetCmdParameter(startno_cmd, startno_param);
    if (ret != 0) {
        return ret;
    }
    startno = (size_t)strtol(startno_param.c_str(), NULL, 10);
    string numquery_cmd("NUMQUERY");
    string numquery_param;
    ret = console_command_.GetCmdParameter(numquery_cmd, numquery_param);
    if (ret != 0) {
        return ret;
    }
    numquery = (size_t)strtol(numquery_param.c_str(), NULL, 10);
    if (numquery - 1 > MAX_QUERY_NUMBER - 1) {
        numquery = MAX_QUERY_NUMBER;
    }
    TCP_Svc_Handler::dump_svcpeer_info(ostr_stream, startno, numquery);
    tmpstr = ostr_stream.str();
    tmp_size += snprintf(ret_string + tmp_size, str_len - tmp_size, "%s",
                         tmpstr.c_str());
    str_len = tmp_size;
    return 0;
}

// line 46
int Zerg_Console_Handler::process_mml_command(char *ret_string,
                                              size_t &str_len) {
    int ret;
    string mml_cmd;

    ret = console_command_.GetMMLCommand(mml_cmd);
    if (ret == 0) {
        if (strcasecmp(mml_cmd.c_str(), "QUERY_STATS") == 0) {
            ret = cmd_get_zergstats(ret_string, str_len);
        } else if (strcasecmp(mml_cmd.c_str(), "QUERY_PEER") == 0) {
            ret = cmd_get_peer_stats(ret_string, str_len);
        } else if (strcasecmp(mml_cmd.c_str(), "CLOSE_PEER") == 0) {
            ret = cmd_close_socketpeer(ret_string, str_len);
        } else if (strcasecmp(mml_cmd.c_str(), "MODIFY_LOG") == 0) {
            ret = cmd_modify_logpriority(ret_string, str_len);
        } else if (strcasecmp(mml_cmd.c_str(), "CLOSE_SERVICE") == 0) {
            ret = cmd_close_services(ret_string, str_len);
        } else {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Can't Process this command :%s.", mml_cmd.c_str());
            str_len = 0;
        }
        if (ret == 0) {
            ZEN_Trace_LogMsg::debug_infoex(
                "[zergsvr] Process Command %s, Ret:%u.", mml_cmd.c_str(), ret);
        }
    }
    return ret;
}
