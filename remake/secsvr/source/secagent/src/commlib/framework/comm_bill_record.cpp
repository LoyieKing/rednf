// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_bill_record.cpp
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
#include "src/commlib/framework/comm_bill_record.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_time_provider.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_svrd_application.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"

CommBillRecord::CommBillRecord()
    : data_source_need_reset_(true),
      game_id_need_reset_(true),
      file_log_(0),
      time_provider_(0),
      zerg_mmap_buspipe_(0),
      if_thread_sync_(false),
      self_svr_(0, 0),
      use_app_framework_(false),
      monitor_(0) {
    log_comment_[LEN_OF_LOGGER_COMMENT_INFO - 1] = 0;
    reset();
}

CommBillRecord::~CommBillRecord() {
    if (file_log_) {
        delete file_log_;
        file_log_ = 0;
    }
}

void CommBillRecord::reset() {
    if (data_source_need_reset_) {
        data_source_ = 0;
    }
    user_id_ = 0;
    log_type_ = 0;
    if (game_id_need_reset_) {
        game_id_ = 0;
    }
    client_ip_ = 0;
    server_ip_ = 0;
    log_comment_[0] = 0;
    num_of_char_data_ = 0;
    num_of_int64_data_ = 0;
}

CommBillRecord & CommBillRecord::operator<<(const ConstDataSource &data_source) {
    data_source_need_reset_ = false;
    data_source_ = data_source.data_source_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const UserID &user_id) {
    user_id_ = user_id.uin_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const LogType &log_type) {
    log_type_ = log_type.log_type_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const GameID &game_id) {
    game_id_need_reset_ = true;
    game_id_ = game_id.game_id_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const ConstGameID &game_id) {
    game_id_need_reset_ = false;
    game_id_ = game_id.game_id_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const ClientIP &client_ip) {
    client_ip_ = client_ip.client_ip_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(int64_t i) {
    int64_data_array_[num_of_int64_data_] = i;
    ++num_of_int64_data_;
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(CommBillRecord & (*op)(CommBillRecord &)) {
    return (*op)(*this);
}

void CommBillRecord::set_logway(int log_way) {
    log_way_ = log_way;
}

void CommBillRecord::set_bill_num(unsigned int bill_num) {
    if (file_log_) {
        file_log_->set_reserve_file_num(bill_num);
    }
}

CommBillRecord & CommBillRecord::operator<<(const char *s) {
    if (num_of_char_data_ <= 3) {
        strncpy(char_data_array_[num_of_char_data_++], s, 0x40);
    }
    return *this;
}

CommBillRecord & CommBillRecord::operator<<(const LogComment &log_comment) {
    strncpy(log_comment_, log_comment.comment_, 0x400);
    return *this;
}

int CommBillRecord::write_to_logsvr(const sec_proto::LogSvrCommBillRecord &pkg) {
    static SERVICES_ID rcvsvc(2, 0);
    static SERVICES_ID proxysvc(0, 0);
    return zerg_mmap_buspipe_->pipe_sendmsg_to_service<sec_proto::LogSvrCommBillRecord>(
        0x4e21, user_id_, 0, 0, rcvsvc, proxysvc, self_svr_, pkg, game_id_, 0);
}

const char * CommBillRecord::make_string() {
    time_t cur_time = use_app_framework_ ? time_provider_->time() : time(0);
    struct tm *tm_info = localtime(&cur_time);
    char str_time[0x31];
    size_t out_len = 0;
    unsigned int i = 0;
    strftime(str_time, 0x1f, "%Y-%m-%d %H:%M:%S", tm_info);
    log_buffer_[0] = 0;
    out_len = snprintf(log_buffer_, 0x1000, "%u|%s|%u|%u|%u|%u|%u|%s",
                       data_source_, str_time, user_id_, log_type_, game_id_,
                       client_ip_, server_ip_, log_comment_);
    for (i = 0; i < num_of_char_data_; ++i) {
        out_len += snprintf(log_buffer_ + out_len, 0x1000 - out_len, "|%s",
                            char_data_array_[i]);
    }
    {
        const unsigned int char_fill_len = 4 - num_of_char_data_;
        char char_fill_string[0x7d];
        memset(char_fill_string, 0x7c, char_fill_len);
        char_fill_string[char_fill_len] = 0;
        out_len += snprintf(log_buffer_ + out_len, 0x1000 - out_len, "%s",
                            char_fill_string);
    }
    for (i = 0; i < num_of_int64_data_; ++i) {
        out_len += snprintf(log_buffer_ + out_len, 0x1000 - out_len, "|%lld",
                            int64_data_array_[i]);
    }
    {
        const unsigned int int64_fill_len = 0x40 - num_of_int64_data_;
        char int64_fill_string[0x7d];
        memset(int64_fill_string, 0x7c, int64_fill_len);
        int64_fill_string[int64_fill_len] = 0;
        out_len += snprintf(log_buffer_ + out_len, 0x1000 - out_len, "%s\n",
                            int64_fill_string);
    }
    log_buffer_[out_len] = 0;
    log_used_buffer_len_ = out_len;
    return log_buffer_;
}

CommBillRecord & CommBillRecord::output() {
    timeval tv;
    sec_proto::LogSvrCommBillRecord pkg;
    int ret = 0;
    if (log_way_ == 0) {
        reset();
        return *this;
    }
    make_string();
    if (monitor_) {
        monitor_->increase_by_statid(0x2396, game_id_, log_type_, 1);
    }
    if (log_way_ & 2) {
        tv.tv_sec = use_app_framework_ ? time_provider_->time() : time(0);
        file_log_->output_log_info(tv, log_buffer_, log_used_buffer_len_);
    }
    if (log_way_ & 1) {
        if (log_used_buffer_len_ > 0x7ff) {
            ZEN_Trace_LogMsg::debug_errorex(
                "bill output fail: log is too long, log_len=%u",
                log_used_buffer_len_ + 1);
        } else {
            memcpy(&pkg, log_buffer_, log_used_buffer_len_ + 1);
        }
        ret = write_to_logsvr(pkg);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "bill output net fail: send to logsvr fail. ret=%d", ret);
        }
    }
    reset();
    return *this;
}

// mangled: _Z4endlR14CommBillRecord
// line 423
CommBillRecord & endl(CommBillRecord &os) {
    return os.output();
}

void CommBillRecord::init_log_file(unsigned int reserve_file_num,
                                   ZEN_LOGFILE_DEVIDE div_log_file) {
    std::string bill_prefix;
    if (!(log_way_ & 2)) {
        return;
    }
    if (strchr(bill_name_.c_str(), '/') != NULL
            || strchr(bill_name_.c_str(), '\\') != NULL) {
        bill_prefix = bill_name_;
    } else {
        if (use_app_framework_) {
            std::string tmp(Comm_Svrd_Config::instance()->app_run_dir_);
            tmp.append("/log/", 5);
            bill_prefix = tmp;
            const char *app_basename =
                Comm_Svrd_Appliction::instance()->get_app_basename();
            bill_prefix.append(app_basename, strlen(app_basename));
            bill_prefix.append("_", 1);
        }
        bill_prefix.append(bill_name_);
        if (bill_prefix.empty()) {
            log_way_ &= ~2;
        }
    }
    if (log_way_ & 2) {
        if (file_log_) {
            delete file_log_;
            file_log_ = 0;
        }
        file_log_ = new ZEN_LogTrace_Basic;
        file_log_->init_time_log(div_log_file, bill_prefix.c_str(),
                                 if_thread_sync_, true, reserve_file_num, 1, 3);
    }
}

int CommBillRecord::init(const char *bill_name, unsigned int reserve_file_num,
                         ZEN_LOGFILE_DEVIDE div_log_file, int log_way,
                         bool if_thread_sync, bool arg4) {
    if (!bill_name) {
        return 0x13a2;
    }
    bill_name_.assign(bill_name, strlen(bill_name));
    log_way_ = log_way;
    if_thread_sync_ = if_thread_sync;
    use_app_framework_ = (Comm_Svrd_Appliction::instance() != NULL);
    init_log_file(reserve_file_num, div_log_file);
    if (use_app_framework_) {
        time_provider_ = Comm_Time_Provider::instance();
        zerg_mmap_buspipe_ = Zerg_MMAP_BusPipe::instance();
        monitor_ = Comm_Stat_Monitor::instance();
    }
    self_svr_ = Comm_Svrd_Config::instance()->self_svr_id_;
    return 0;
}

CommBillRecord g_bill;
CommBillRecord g_stat;
