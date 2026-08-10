// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_sndrcv_zulu.cpp
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
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_sndrcv_zulu.h"
#include "src/commlib/framework/comm_sndrcv_base.h"

Zulu_SendRecv_Package::Zulu_SendRecv_Package()
    : zulu_connected_(false) {
    zulu_svc_ip_.set("127.0.0.1", 8080);
}

Zulu_SendRecv_Package::~Zulu_SendRecv_Package() {
    if (zulu_connected_) {
        zulu_stream_.close();
    }
}

void Zulu_SendRecv_Package::close() {
    if (zulu_connected_) {
        zulu_stream_.close();
        zulu_connected_ = false;
    }
}

int Zulu_SendRecv_Package::send_svc_package(ZEN_Time_Value *time_wait) {
    int len = 0;
    unsigned int cmd = 0;
    ssize_t socket_ret = 0;

    tibetan_send_appframe_->send_service_ = tibetan_send_service_;
    tibetan_send_appframe_->recv_service_ = tibetan_recv_service_;
    tibetan_send_appframe_->proxy_service_ = tibetan_proxy_service_;

    trans_id_builder_ += 2;
    if (trans_id_builder_ == 0) {
        trans_id_builder_ = 1;
    }
    tibetan_send_appframe_->transaction_id_ = trans_id_builder_;

    cmd = tibetan_send_appframe_->frame_command_;
    len = tibetan_send_appframe_->frame_length_;
    tibetan_send_appframe_->framehead_encode();
    socket_ret = zulu_stream_.send_n(tibetan_send_appframe_, len, time_wait, 0);
    if (socket_ret <= 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] SEND cmd [%u] bytes[%u] frame To Svr fail ret =%u. ",
            cmd, len, socket_ret);
        return 0x13e5;
    }
    return 0;
}

int Zulu_SendRecv_Package::connect_zulu_server(ZEN_Time_Value *time_wait) {
    int ret = 0;
    if (zulu_connected_) {
        zulu_connected_ = true;
        return 0;
    }
    ZEN_Socket_Connector tmp_connector;
    ZEN_Time_Value real_time_wait(10, 0);
    if (time_wait) {
        real_time_wait = *time_wait;
    }
    ret = tmp_connector.connect(zulu_stream_, &zulu_svc_ip_, real_time_wait, false, 0, 0);
    if (ret != 0) {
        return 0x13e4;
    }
    zulu_connected_ = true;
    return 0;
}

int Zulu_SendRecv_Package::set_zulu_svcinfo(const char *svc_ip, short unsigned int svc_port, const SERVICES_ID &recv_service, const SERVICES_ID &send_service, const SERVICES_ID &proxy_service, size_t frame_len) {
    set_services_id(recv_service, send_service, proxy_service, frame_len);
    zulu_svc_ip_.set(svc_ip, svc_port);
    return 0;
}

int Zulu_SendRecv_Package::receive_svc_package(ZEN_Time_Value *time_wait) {
    ssize_t socket_ret = 0;
    int data_len = 0;
    int last_error = 0;
    for (;;) {
        socket_ret = zulu_stream_.recv_n(tibetan_recv_appframe_, 0x32, time_wait, 0);
        if (socket_ret == 0) {
            last_error = errno;
            ZEN_Trace_LogMsg::debug_infoex(
                "[framework] Link is disconnect recv ret =%d, error[%u|%s].",
                0, last_error, strerror(last_error));
            return 0x1422;
        }
        if (socket_ret < 0) {
            last_error = errno;
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] RECV Comm_App_Frame head error or time out. Ret:%d, error[%u|%s].",
                socket_ret, last_error, strerror(last_error));
            if (last_error == 4) {
                continue;
            }
            return 0x13e6;
        }
        tibetan_recv_appframe_->framehead_decode();
        recv_trans_id_ = tibetan_recv_appframe_->transaction_id_;
        data_len = tibetan_recv_appframe_->frame_length_ - 0x32;
        if (data_len < 0) {
            last_error = errno;
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Receive Comm_App_Frame head len error ,frame len:%d,error[%u|%s].",
                tibetan_recv_appframe_->frame_length_, last_error, strerror(last_error));
            return 0x13e6;
        }
        if (data_len > 0) {
            for (;;) {
                socket_ret = zulu_stream_.recv_n(tibetan_recv_appframe_->frame_appdata_, data_len, time_wait, 0);
                if (socket_ret < 0) {
                    last_error = errno;
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[framework] RECV Comm_App_Frame body data error. Ret:%d, error[%u|%s].",
                        socket_ret, last_error, strerror(last_error));
                    if (last_error == 4) {
                        continue;
                    }
                    return 0x13e6;
                }
                break;
            }
            if (socket_ret == 0) {
                last_error = errno;
                ZEN_Trace_LogMsg::debug_infoex(
                    "[framework] Link is disconnect recv ret =%d, error[%u|%s].",
                    0, last_error, strerror(last_error));
                return 0x1422;
            }
            if (socket_ret != data_len) {
                last_error = errno;
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] Receive Comm_App_Frame body data error or time out ,ret:%d,error[%u|%s].",
                    socket_ret, last_error, strerror(last_error));
                return 0x13e6;
            }
        }
        ZEN_Trace_LogMsg::debug_debugEx(
            "[framework] Recv cmd [%u]  bytes [%u]  Frame From Svr Succ. ",
            tibetan_recv_appframe_->frame_command_, tibetan_recv_appframe_->frame_length_);
        if (tibetan_send_service_.services_id_ == SERVICES_ID::DYNAMIC_ALLOC_SERVICES_ID) {
            tibetan_send_service_.services_id_ = tibetan_recv_appframe_->recv_service_.services_id_;
        }
        if (tibetan_send_service_ != tibetan_recv_appframe_->recv_service_) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] zulu recv a error or unexpect frame,cmd %u. snd svc id [%u|%u] recv svc id[%u|%u].",
                tibetan_recv_appframe_->frame_command_, tibetan_send_service_.services_id_,
                tibetan_send_service_.services_id_, tibetan_recv_appframe_->recv_service_.services_type_,
                tibetan_recv_appframe_->recv_service_.services_id_);
            return 0x13e6;
        }
        return 0;
    }
}

int Zulu_SendRecv_Package::receive_svc_package(unsigned int &recv_cmd, ZEN_Time_Value *time_out) {
    int ret = receive_svc_package(time_out);
    if (ret != 0) {
        zulu_stream_.close();
        zulu_connected_ = false;
        return ret;
    }
    recv_cmd = tibetan_recv_appframe_->frame_command_;
    return 0;
}
