// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_zerg_mmappipe.cpp
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
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"

Zerg_MMAP_BusPipe::Zerg_MMAP_BusPipe()
    : zerg_svr_info_(0, 0),
      monitor_(0) {
}

Zerg_MMAP_BusPipe::~Zerg_MMAP_BusPipe() {
    zerg_svr_info_.~SERVICES_ID();
}

// line 159
void Zerg_MMAP_BusPipe::get_mmapfile_name(char *mmapfile, size_t buflen) {
    snprintf(mmapfile, buflen, "./ZERGPIPE.%u.%u.MMAP",
             zerg_svr_info_.services_type_, zerg_svr_info_.services_id_);
}

// line 85
int Zerg_MMAP_BusPipe::getpara_from_zergcfg(const Zerg_MMAP_BusPipe::ZERG_CONFIG &zerg_config) {
    int ret = 0;
    unsigned short svrtype = 0;
    unsigned int svrid = 0;

    bus_head_.size_of_pipe_[0] = zerg_config.comm_cfg.recv_pipe_len;
    if (bus_head_.size_of_pipe_[0] <= 0x200000
            || bus_head_.size_of_pipe_[0] > 0x31ffffff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "COMMCFG|RECVPIPELEN key error.");
        return 0x1398;
    }
    bus_head_.size_of_pipe_[1] = zerg_config.comm_cfg.send_pipe_len;
    if (bus_head_.size_of_pipe_[1] <= 0x200000
            || bus_head_.size_of_pipe_[1] > 0x31ffffff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "COMMCFG|SENDPIPELEN key error.");
        return 0x1398;
    }
    svrtype = zerg_config.self_cfg.self_svr_info.svr_type;
    if (svrtype == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRTYPE key error.");
        return 0x1398;
    }
    svrid = zerg_config.self_cfg.self_svr_info.svr_id;
    if (svrid == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRID key error.");
        return 0x1398;
    }
    zerg_svr_info_.set_serviceid(svrtype, svrid);
    if_check_pthrad_ = (zerg_config.comm_cfg.check_pthread != 0);
    return 0;
}

// line 66
int Zerg_MMAP_BusPipe::initialize(SERVICES_ID &svrinfo, size_t size_recv_pipe, size_t size_send_pipe, size_t max_frame_len, bool if_restore, bool if_check_pthread) {
    char bus_mmap_name[0x200] = { 0 };
    monitor_ = Comm_Stat_Monitor::instance();
    config_ = Comm_Svrd_Config::instance();
    zerg_svr_info_ = svrinfo;
    get_mmapfile_name(bus_mmap_name, sizeof(bus_mmap_name));
    return ZEN_BusPipe_TwoWay::initialize(bus_mmap_name, size_recv_pipe, size_send_pipe,
                                          max_frame_len, if_restore, if_check_pthread, 1);
}

// line 147
int Zerg_MMAP_BusPipe::init_after_getcfg(size_t max_frame_len, bool if_restore) {
    return initialize(zerg_svr_info_, bus_head_.size_of_pipe_[0],
                      bus_head_.size_of_pipe_[1], max_frame_len, if_restore,
                      if_check_pthrad_);
}

// line 185
void Zerg_MMAP_BusPipe::clean_instance() {
    if (zerg_bus_instance_) {
        delete zerg_bus_instance_;
        zerg_bus_instance_ = 0;
    }
}

// line 177
void Zerg_MMAP_BusPipe::instance(Zerg_MMAP_BusPipe *pinstatnce) {
    clean_instance();
    zerg_bus_instance_ = pinstatnce;
}

// line 166
Zerg_MMAP_BusPipe * Zerg_MMAP_BusPipe::instance() {
    if (!zerg_bus_instance_) {
        zerg_bus_instance_ = new Zerg_MMAP_BusPipe;
    }
    return zerg_bus_instance_;
}

// line 207
int Zerg_MMAP_BusPipe::pipe_sendbuf_to_service(unsigned int cmd, unsigned int qquin, unsigned int transaction_id, unsigned int backfill_trans_id, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, const SERVICES_ID &sendsvc, const unsigned char *buf, size_t buf_len, unsigned int app_id, unsigned int option) {
    Comm_App_Frame *send_frame = (Comm_App_Frame *)send_buffer_;
    int ret = 0;

    send_frame->init_framehead(0x10000, option, cmd);
    send_frame->frame_uin_ = qquin;
    send_frame->app_id_ = app_id;
    send_frame->send_service_ = sendsvc;
    send_frame->proxy_service_ = proxysvc;
    send_frame->recv_service_ = rcvsvc;
    send_frame->transaction_id_ = transaction_id;
    send_frame->backfill_trans_id_ = backfill_trans_id;
    ret = send_frame->fill_appdata(buf_len, (const char *)buf);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] [%s]TDR encode fail.ret =%d,Please check your code and buffer len.",
            "int Zerg_MMAP_BusPipe::pipe_sendbuf_to_service(unsigned int, unsigned int, unsigned int, unsigned int, const SERVICES_ID",
            ret);
        return ret;
    }
    if (send_frame->frame_length_ - 0x32 > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            send_frame->frame_length_);
        return 0x13b1;
    }
    if (config_->is_monitor_uin(send_frame->frame_uin_)) {
        send_frame->frame_option_ |= 0x10000;
    }
    if (send_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(send_frame, "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    if (!bus_pipe_pointer_[1]->push_end((const ZEN_LIB::dequechunk_node *)send_frame)) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
            1, send_frame->frame_length_, bus_pipe_pointer_[1]->freesize());
        monitor_->increase_by_statid(0x238f, send_frame->app_id_,
                                     send_frame->frame_command_, 1);
        return 0x1396;
    }
    monitor_->increase_by_statid(0x238e, send_frame->app_id_,
                                 send_frame->frame_command_, 1);
    monitor_->increase_by_statid(0x239c, send_frame->app_id_,
                                 send_frame->frame_command_,
                                 send_frame->frame_length_);
    return 0;
}

// line 111
int Zerg_MMAP_BusPipe::getpara_from_zergcfg(const Zen_INI_PropertyTree &zerglingcfg) {
    int ret = 0;
    uint32_t tmp_value = 0;
    uint32_t tmpuint = 0;
    unsigned short svrtype = 0;
    unsigned int svrid = 0;

    ret = zerglingcfg.get_uint32_value(std::string("COMMCFG"),
                                       std::string("RECVPIPELEN"), tmp_value);
    bus_head_.size_of_pipe_[0] = tmp_value;
    if (ret != 0 || bus_head_.size_of_pipe_[0] <= 0x200000
            || bus_head_.size_of_pipe_[0] > 0x31ffffff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "COMMCFG|RECVPIPELEN key error.");
        return 0x1398;
    }
    ret = zerglingcfg.get_uint32_value(std::string("COMMCFG"),
                                       std::string("SENDPIPELEN"), tmp_value);
    bus_head_.size_of_pipe_[1] = tmp_value;
    if (ret != 0 || bus_head_.size_of_pipe_[1] <= 0x200000
            || bus_head_.size_of_pipe_[1] > 0x31ffffff) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "COMMCFG|SENDPIPELEN key error.");
        return 0x1398;
    }
    ret = zerglingcfg.get_uint32_value(std::string("SELFCFG"),
                                       std::string("SELFSVRTYPE"), tmpuint);
    svrtype = (unsigned short)tmpuint;
    if (ret != 0 || svrtype == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRTYPE key error.");
        return 0x1398;
    }
    ret = zerglingcfg.get_uint32_value(std::string("SELFCFG"),
                                       std::string("SELFSVRID"), tmpuint);
    svrid = tmpuint;
    if (ret != 0 || svrid == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.",
            "SELFCFG|SELFSVRID key error.");
        return 0x1398;
    }
    zerg_svr_info_.set_serviceid(svrtype, svrid);
    return 0;
}

char Zerg_MMAP_BusPipe::send_buffer_[0x10000];
Zerg_MMAP_BusPipe *Zerg_MMAP_BusPipe::zerg_bus_instance_ = NULL;
