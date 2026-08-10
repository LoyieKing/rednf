// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_svrd_application.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_predefine.h"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml.hpp"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml_utils.hpp"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml_print.hpp"
#include "import/include/opensource/mysqlclient/mysql.h"
#include "import/include/opensource/mysqlclient/mysql_version.h"
#include "import/include/opensource/mysqlclient/mysql_com.h"
#include "import/include/opensource/mysqlclient/mysql_time.h"
#include "import/include/opensource/mysqlclient/typelib.h"
#include "import/include/opensource/mysqlclient/my_alloc.h"
#include "import/include/opensource/mysqlclient/my_list.h"
#include "output/commlib/zenlib/release/include/zen_id_to_string.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_mysql_connect.h"
#include "output/commlib/zenlib/release/include/zen_mysql_predefine.h"
#include "output/commlib/zenlib/release/include/zen_mysql_command.h"
#include "output/commlib/zenlib/release/include/zen_mysql_result.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "output/commlib/zenlib/release/include/zen_mysql_field.h"
#include "output/commlib/zenlib/release/include/zen_mysql_process.h"
#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_shm_cache_chunk.h"
#include "output/commlib/zenlib/release/include/zen_shm_lockfree_deque.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_spin.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_thread.h"
#include "output/commlib/zenlib/release/include/zen_server_toolkit.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_process.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_sysinfo.h"
#include "output/commlib/zenlib/release/include/zen_shm_hash_table.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_socket.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_error.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_getopt.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_file.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_dirent.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_debug.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_base.h"
#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include "output/commlib/zenlib/release/include/zen_lock_null_lock.h"
#include "output/commlib/zenlib/release/include/zen_config_property_tree.h"
#include "output/commlib/zenlib/release/include/zen_config_ini_implement.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_sema.h"
#include "output/commlib/zenlib/release/include/zen_lock_synch_traits.h"
#include "output/commlib/zenlib/release/include/zen_lock_process_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_rw_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_semaphore.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_condi.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_condi.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_nonlock.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_wheel.h"
#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_event_handle_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_select.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_epoll.h"
#include "output/commlib/zenlib/release/include/zen_thread_wait_mgr.h"
#include "output/commlib/zenlib/release/include/zen_config_getopt.h"
#include "output/commlib/zenlib/release/include/zen_string_util.h"
#include "output/commlib/zenlib/release/include/zen_server_status.h"
#include "output/commlib/zenlib/release/include/zen_shm_vector.h"
#include "output/commlib/zenlib/release/include/zen_thread_bus_pipe.h"
#include "output/protocol/common/release/include/comm_proto_cfgsvr.h"
#include "output/protocol/common/release/include/TdrBuf.h"
#include "output/protocol/common/release/include/TdrPal.h"
#include "output/protocol/common/release/include/TdrError.h"
#include "output/protocol/common/release/include/TdrBufUtil.h"
#include "output/protocol/common/release/include/TdrTypeUtil.h"
#include "output/protocol/common/release/include/TdrTime.h"
#include "output/protocol/common/release/include/comm_proto_public_head.h"
#include "output/protocol/common/release/include/comm_proto_public_cmd.h"
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_monitorsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"
#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "output/protocol/common/release/include/TdrXml.h"
#include "output/protocol/common/release/include/TdrIO.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "output/protocol/common/release/include/comm_conf_cfgsdk.h"
#include "import/include/tencore/tencrypt/TenHash.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_external.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_types.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_define.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_ctypes_info.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_error.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_init.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_XMLtags.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_XMLMetaLib.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_to_hpp.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_data_io.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_operate_data.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_data_sort.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_net.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_manage.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_sql.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_meta_entries_index.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus_macros.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus_error.h"
#include "src/commlib/framework/comm_svrd_application.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/commlib/framework/comm_timer_handler.h"
#include "src/commlib/framework/comm_bill_record.h"
#include "src/commlib/framework/comm_time_provider.h"
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

namespace ZEN_OS {
pid_t fork();
pid_t setsid();
mode_t umask(mode_t cmask);
sighandler_t signal(int signum, sighandler_t func);
int chdir(const char *dirname);
const char *basename(const char *path_name, char *file_name, size_t buf_len);
}

static const char * const FRAMEWORK_LOG_SEPARATOR =
    "------------------------------------------------------------------------------------------";

Comm_Svrd_Appliction::Comm_Svrd_Appliction()
    : app_run_(true),
      app_pause_(false),
      self_services_id_(0, 0),
      run_as_win_serivces_(false),
      max_msg_num_(0x400),
      zerg_mmap_pipe_(0),
      argc_(0),
      argv_(0),
      timer_handler_(0) {
    app_author_.assign("TSS Platform Server Dev Team.", 29);
}

Comm_Svrd_Appliction::~Comm_Svrd_Appliction() {
    if (timer_handler_) {
        delete timer_handler_;
        timer_handler_ = 0;
    }
}

int Comm_Svrd_Appliction::load_app_conf() {
}

void Comm_Svrd_Appliction::exit() {
}

void Comm_Svrd_Appliction::set_pause_sign(bool app_pause) {
    app_pause_ = app_pause;
}

void Comm_Svrd_Appliction::set_run_sign(bool app_run) {
    app_run_ = app_run;
}

void Comm_Svrd_Appliction::exit_signal(int arg0) {
    base_instance_->set_run_sign(false);
}

void Comm_Svrd_Appliction::reload_config_signal(int arg0) {
    base_instance_->reload_instance();
    base_instance_->reload();
}

Comm_Svrd_Appliction * Comm_Svrd_Appliction::instance() {
    return base_instance_;
}

int Comm_Svrd_Appliction::reload_config() {
    int ret = reload_instance();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "framework reload config error:ret=%d", ret);
        return ret;
    }
    ret = load_app_conf();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "load app config error, ret=%d", ret);
        return ret;
    }
    ret = reload();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "app reload config error, ret=%d", ret);
        return ret;
    }
    return 0;
}

int Comm_Svrd_Appliction::do_run() {
    int ret = init_instance();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "application: init_instance fail. ret=%d", ret);
        return ret;
    }
    ret = init();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "application: init_app fail. ret=%d", ret);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework]application: init succ. start run");
    ret = run_instance();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "application: run_instance fail. ret=%d", ret);
    }
    exit();
    exit_instance();
    ZEN_Trace_LogMsg::debug_infoex("[framework] application exit.");
    return ret;
}

int Comm_Svrd_Appliction::register_comm_timer() {
    ZEN_Timer_Queue *timer_queue = ZEN_Timer_Queue::instance();
    timer_handler_ = new Comm_Timer_Handler(timer_queue);
    timer_handler_->init();
    ZEN_Time_Value delay;
    ZEN_Time_Value interval;
    delay.set(0, 0);
    interval.set(0, 0x186a0);
    timer_queue->schedule_timer(timer_handler_, 0, delay, interval);
    return 0;
}

ZEN_LOG_PRIORITY Comm_Svrd_Appliction::get_log_priority() {
    return ZEN_Trace_LogMsg::instance()->get_log_priority();
}

void Comm_Svrd_Appliction::set_log_priority(ZEN_LOG_PRIORITY log_prio) {
    ZEN_Trace_LogMsg::instance()->set_log_priority(log_prio);
}

int Comm_Svrd_Appliction::daemon_init() {
    pid_t pid = ZEN_OS::fork();
    if (pid < 0) {
        return 0x13b4;
    }
    if (pid > 0) {
        ::exit(0);
    }
    ZEN_OS::setsid();
    ZEN_OS::umask(0);
    return 0;
}

int Comm_Svrd_Appliction::process_signal() {
    ZEN_OS::signal(SIGHUP, (sighandler_t)1);
    ZEN_OS::signal(SIGPIPE, (sighandler_t)1);
    ZEN_OS::signal(SIGCHLD, (sighandler_t)1);
    ZEN_OS::signal(SIGINT, exit_signal);
    ZEN_OS::signal(SIGQUIT, exit_signal);
    ZEN_OS::signal(SIGTERM, exit_signal);
    ZEN_OS::signal(SIGUSR1, reload_config_signal);
    return 0;
}

int Comm_Svrd_Appliction::reload_instance() {
    return Comm_Svrd_Config::instance()->reload();
}

int Comm_Svrd_Appliction::init_log() {
    Comm_Svrd_Config *config = Comm_Svrd_Config::instance();
    int ret = 0;
    ret = g_bill.init("bill", 3, 0, 1);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] init bill fail. ret=%d", ret);
        return ret;
    }
    ret = g_stat.init("stat", 3, 0, 1);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] init stat fail. ret=%d", ret);
        return ret;
    }
    g_bill.set_logway(config->framework_config_.log_info_.bill_output_);
    g_bill.set_bill_num(config->framework_config_.log_info_.max_bill_file_num_);
    g_stat.set_logway(config->framework_config_.log_info_.bill_output_);
    g_stat.set_bill_num(config->framework_config_.log_info_.max_bill_file_num_);
    ZEN_Trace_LogMsg::debug_debugEx("log instance finalize .");
    ZEN_Trace_LogMsg::instance()->finalize();
    ret = ZEN_Trace_LogMsg::instance()->initialize(
        (ZEN_LOGFILE_DEVIDE)config->framework_config_.log_info_.log_div_type_,
        config->log_file_prefix_.c_str(), false, true,
        config->framework_config_.log_info_.max_log_file_size_,
        config->framework_config_.log_info_.max_log_file_num_,
        config->framework_config_.log_info_.log_output_, 3);
    ZEN_Trace_LogMsg::debug_debugEx("log instance reinit .");
    return 0;
}

const char * Comm_Svrd_Appliction::get_app_basename() {
    return app_base_name_.c_str();
}

const char * Comm_Svrd_Appliction::get_app_runname() {
    return app_run_name_.c_str();
}

int Comm_Svrd_Appliction::exit_instance() {
    Zen_Thread_Wait_Manager::instance()->wait_all();
    Zen_Thread_Wait_Manager::clean_instance();
    ZEN_Reactor::instance()->close();
    Zerg_MMAP_BusPipe::clean_instance();
    Comm_Svrd_Config::clean_instance();
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] %s exit_instance Succ.Have Fun.!!!", get_app_runname());
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    return 0;
}

void Comm_Svrd_Appliction::set_service_info(const char *svc_name, const char *svc_desc) {
    if (svc_name) {
        service_name_.assign(svc_name, strlen(svc_name));
    }
    if (svc_desc) {
        service_desc_.assign(svc_desc, strlen(svc_desc));
    }
}

int Comm_Svrd_Appliction::create_app_name(const char *argv_0) {
    char str_base_name[0x1000];
    app_run_name_.assign(argv_0, strlen(argv_0));
    str_base_name[0] = 0;
    ZEN_OS::basename(argv_0, str_base_name, sizeof(str_base_name));
    app_base_name_.assign(str_base_name, strlen(str_base_name));
    return 0;
}

int Comm_Svrd_Appliction::proc_start_args(int argc, const char **argv) {
    int ret = 0;
    argc_ = argc;
    argv_ = argv;
    base_instance_ = this;
    ret = create_app_name(argv[0]);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "svr create_app_base_name init fail. ret=%d", ret);
        return ret;
    }
    ret = Comm_Svrd_Config::instance()->proc_start_arg(argc, argv);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "load config error:ret=%d", ret);
        return ret;
    }
    return 0;
}

int Comm_Svrd_Appliction::run(int argc, const char **argv) {
    int ret = proc_start_args(argc, argv);
    if (ret != 0) {
        return ret;
    }
    if (Comm_Svrd_Config::instance()->app_run_daemon_) {
        daemon_init();
    }
    return do_run();
}

int Comm_Svrd_Appliction::init_instance() {
    std::string log_file_prefix;
    std::string app_path;
    int ret = 0;
    CfgSvrSdk *cfgsvr_sdk = NULL;
    Comm_Svrd_Config *svd_config = NULL;

    process_signal();
    log_file_prefix = Comm_Svrd_Config::instance()->app_run_dir_;
    log_file_prefix.append("/log/", 5);
    log_file_prefix.append(app_base_name_);
    log_file_prefix.append("_init", 5);
    ZEN_Trace_LogMsg::instance()->init_size_log(
        log_file_prefix.c_str(), false, true, 0xa00000, 2, 5, 3);
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    ZEN_Trace_LogMsg::debug_infoex(FRAMEWORK_LOG_SEPARATOR);
    ZEN_Trace_LogMsg::debug_infoex("[framework] %s start init", get_app_basename());
    ret = ZEN_Server_Toolkit::socket_init();
    if (ret != 0) {
        return ret;
    }
    ret = ZEN_OS::chdir(Comm_Svrd_Config::instance()->app_run_dir_.c_str());
    if (ret != 0) {
        int last_error = errno;
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] change run directory to %s fail. err=%d",
            Comm_Svrd_Config::instance()->app_run_dir_.c_str(), last_error);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] change work dir to %s",
        Comm_Svrd_Config::instance()->app_run_dir_.c_str());
    app_path = Comm_Svrd_Config::instance()->app_run_dir_;
    app_path.append("/", 1);
    app_path.append(get_app_basename(), strlen(get_app_basename()));
    ret = ZEN_Server_Toolkit::out_pid_file(app_path.c_str(), true);
    if (ret != 0) {
        int last_error = errno;
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Create Pid file :%s.pid fail .last error =[%u|%s].",
            app_path.c_str(), last_error, strerror(last_error));
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] If last error == 16, could has a same process alread run in this directory.Please check ");
        return 0x13b7;
    }
    cfgsvr_sdk = CfgSvrSdk::instance();
    ret = cfgsvr_sdk->init();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] cfgsvrsdk init fail. ret=%d", ret);
        return ret;
    }
    cfgsvr_sdk->start_task();
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] cfgsdk init succ. start task succ");
    svd_config = Comm_Svrd_Config::instance();
    ret = svd_config->init(argc_, argv_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] framwork config init fail. ret=%d", ret);
        return ret;
    }
    self_services_id_ = svd_config->self_svr_id_;
    ret = load_app_conf();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] %s load app config fail. ret=%d",
            "virtual int Comm_Svrd_Appliction::init_instance()", ret);
        return ret;
    }
    ZEN_Timer_Wheel *timer_wheel = new ZEN_Timer_Wheel(
        svd_config->framework_config_.trans_info_.trans_num_ + 0x400,
        0xf731400, 0x64,
        ZEN_Timer_Queue::TRIGGER_MODE_SYSTEM_CLOCK, true);
    ZEN_Timer_Queue::instance(timer_wheel);
    Comm_Stat_Monitor::instance()->add_status_item(
        COMM_STAT_FRATURE_NUM, COMM_STAT_ITEM_WITH_NAME);
    ZEN_Epoll_Reactor *reactor = new ZEN_Epoll_Reactor(0x400, false, 0x400);
    ZEN_Reactor::instance(reactor);
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] ZEN_Reactor and ZEN_Epoll_Reactor initialized.");
    ret = Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg(
        svd_config->zerg_config_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg fail,ret = %d.",
            ret);
        return ret;
    }
    ret = Zerg_MMAP_BusPipe::instance()->init_after_getcfg(
        0x10000, svd_config->if_restore_pipe_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] Zerg_MMAP_BusPipe::instance()->init_by_cfg fail,ret = %d.",
            ret);
        return ret;
    }
    zerg_mmap_pipe_ = Zerg_MMAP_BusPipe::instance();
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] MMAP Pipe init success,gogogo.The more you have,the more you want. ");
    register_comm_timer();
    ret = init_log();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] init log fail. ret=%d", ret);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Comm_Svrd_Appliction::init_instance Success.");
    return 0;
}

Comm_Svrd_Appliction *Comm_Svrd_Appliction::base_instance_ = NULL;
