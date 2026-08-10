// Reconstructed from zergsvr binary (DWARF 布局 + 反汇编逐函数还原)
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_application.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/commsvr/zergsvr/zerg_predefine.h"
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
#include "output/commlib/zenlib/release/include/zen_os_adapt_error.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_file.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_socket.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_getopt.h"
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
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_rw_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_semaphore.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_condi.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_condi.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_nonlock.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_wheel.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_event_handle_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_base.h"
#include "output/commlib/zenlib/release/include/zen_config_getopt.h"
#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_acceptor.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "output/commlib/framework/release/include/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_dirent.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_select.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_epoll.h"
#include "output/commlib/zenlib/release/include/zen_thread_wait_mgr.h"
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
#include "output/commlib/framework/release/include/comm_error_code.h"
#include "output/commlib/framework/release/include/comm_random_number.h"
#include "output/commlib/framework/release/include/comm_service_config.h"
#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/framework/release/include/comm_enum_define.h"
#include "output/commlib/framework/release/include/comm_app_frame.h"
#include "output/commlib/framework/release/include/comm_frame_command.h"
#include "output/commlib/framework/release/include/comm_zerg_mmappipe.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/framework/release/include/comm_stat_define.h"
#include "output/commlib/framework/release/include/comm_svrd_config.h"
#include "output/commlib/framework/release/include/comm_mml_command.h"
#include "output/commlib/framework/release/include/comm_mml_console_handler.h"
#include "output/commlib/framework/release/include/comm_cfgsvr_sdk.h"
#include "output/commlib/framework/release/include/comm_encrypt_arithmetic.h"
#include "output/commlib/framework/release/include/comm_cmd_statistic.h"
#include "output/commlib/framework/release/include/comm_svrd_application.h"
#include "output/commlib/framework/release/include/comm_svrd_app_main.h"
#include "output/commlib/framework/release/include/comm_bill_record.h"
#include "output/commlib/framework/release/include/comm_time_provider.h"
#include "output/commlib/framework/release/include/comm_xml_config.h"
#include "src/commsvr/zergsvr/zerg_application.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"
#include "src/commsvr/zergsvr/zerg_app_handler.h"
#include "src/commsvr/zergsvr/zerg_console_handler.h"
#include "src/commsvr/zergsvr/zerg_udp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_ip_restrict.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
#include "src/commsvr/zergsvr/zerg_stat_define.h"
#include <algorithm>
#include <alloca.h>
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <assert.h>
#include <cassert>
#include <cctype>
#include <climits>
#include <clocale>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <cwchar>
#include <cwctype>
#include <debug/debug.h>
#include <deque>
#include <dirent.h>
#include <dlfcn.h>
#include <endian.h>
#include <errno.h>
#include <exception>
#include <exception_defines.h>
#include <execinfo.h>
#include <ext/hash_fun.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/hashtable.h>
#include <ext/new_allocator.h>
#include <fcntl.h>
#include <features.h>
#include <fstream>
#include <functional>
#include <gconv.h>
#include <getopt.h>
#include <gnu/stubs.h>
#include <iconv.h>
#include <inttypes.h>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <langinfo.h>
#include <libintl.h>
#include <libio.h>
#include <limits.h>
#include <limits>
#include <list>
#include <locale.h>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <new>
#include <nl_types.h>
#include <ostream>
#include <pthread.h>
#include <queue>
#include <rpc/netdb.h>
#include <sched.h>
#include <semaphore.h>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdarg.h>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>
#include <string>
#include <sys/cdefs.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/io.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <sys/uio.h>
#include <time.h>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <vector>
#include <wchar.h>
#include <wctype.h>

// line 34
Zerg_Service_App *Zerg_Service_App::instance_ = NULL;

// line 37
Zerg_Service_App::Zerg_Service_App()
    : timer_queue_(NULL), zerg_comm_mgr_(NULL), conf_timestamp_(0), config_(NULL) {
    config_ = new Zerg_MMAP_BusPipe::ZERG_CONFIG();
}

// line 45
Zerg_Service_App::~Zerg_Service_App() {
    if (config_ != NULL) {
        delete config_;
        config_ = NULL;
    }
    if (zerg_comm_mgr_ != NULL) {
        delete zerg_comm_mgr_;
        zerg_comm_mgr_ = NULL;
    }
}

// line 78
int Zerg_Service_App::init_instance() {
    int ret = 0;
    size_t max_accept = 0;
    size_t max_connect = 0;
    size_t max_peer = 0;
    unsigned int i = 0;

    ret = Comm_Svrd_Appliction::init_instance();
    if (ret != 0) {
        return ret;
    }

    ret = Comm_Stat_Monitor::instance()->initialize(
        true, Comm_Svrd_Config::instance()->self_svr_id_,
        sizeof(ZERGLING_STAT_ITEM_WITH_NAME) / sizeof(ZERGLING_STAT_ITEM_WITH_NAME[0]),
        ZERGLING_STAT_ITEM_WITH_NAME, false);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[zergsvr] Zen_Server_Status init fail. ret=%d", ret);
        return ret;
    }

    config_ = &Comm_Svrd_Config::instance()->zerg_config_;
    ret = Zerg_Server_Config::instance()->init(config_);
    if (ret != 0) {
        return ret;
    }

    TCP_Svc_Handler::get_max_peer_num(max_accept, max_connect);
    max_peer = max_accept + 0x10 + max_connect;
    ZEN_Reactor::instance(new ZEN_Epoll_Reactor(max_peer, false, 1024));
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] ZEN_Reactor and ZEN_Epoll_Reactor initialized.");

    new Zerg_App_Timer_Handler(ZEN_Timer_Queue::instance());
    ZBuffer_Storage::instance()->init_buflist_by_hdlnum(max_peer);
    Zerg_IPRestrict_Mgr::instance();
    zerg_comm_mgr_ = Zerg_Comm_Manager::instance();
    MML_Console_Handler::instance(new Zerg_Console_Handler(ZEN_Reactor::instance()));

    ret = reload();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_infoex("[zergsvr] ReloadDynamicConfig fail,ret = %d.", ret);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] ReloadDynamicConfig Succ.Ooooo!Some people believe that God created the world,but.");

    ret = TCP_Svc_Handler::init_all_static_data();
    if (ret != 0) {
        return ret;
    }
    ret = UDP_Svc_Handler::init_all_static_data();
    if (ret != 0) {
        return ret;
    }

    ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::self_svc_info_);
    if (ret != 0) {
        return ret;
    }
    for (i = 0; i < config_->self_cfg.slave_svr_count; i++) {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::slave_svc_ary_[i]);
        if (ret != 0) {
            return ret;
        }
    }

    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] init_instance Succ.Have Fun.!!!");
    return ret;
}

// line 199
int Zerg_Service_App::reload() {
    int ret = 0;

    ret = Zerg_IPRestrict_Mgr::instance()->get_iprestrict_conf(*config_);
    if (ret != 0) {
        return ret;
    }
    ret = zerg_comm_mgr_->get_config(*config_);
    if (ret != 0) {
        return ret;
    }

    ret = MML_Console_Handler::instance()->initialize(*config_);
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Console close .ret = %u", ret);
    if (ret != 0) {
        if (ret == 0x13ad) {
            ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Console close .ret = %u", 0x13ad);
        } else {
            ZEN_Trace_LogMsg::debug_errorex("[zergsvr] Console initialize fail.ret = %u", ret);
        }
    }

    set_log_priority(ZEN_LogTrace_Basic::log_priorities(config_->log_cfg.log_level));
    ret = Zerg_Server_Config::instance()->init(config_);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[zergsvr] zerg reload config fail. ret=%d", ret);
        return ret;
    }
    return ret;
}

// line 266
int Zerg_Service_App::exit_instance() {
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] exit_instance Succ.Have Fun.!!!");
    TCP_Svc_Handler::uninit_all_staticdata();
    Zerg_Comm_Manager::clean_instance();
    Zerg_IPRestrict_Mgr::clean_instance();
    MML_Console_Handler::clean_instance();
    Comm_Stat_Monitor::clean_instance();
    if (ZEN_Reactor::instance() != NULL) {
        ZEN_Reactor::instance()->close();
    }
    if (ZEN_Timer_Queue::instance() != NULL) {
        ZEN_Timer_Queue::instance()->close();
    }
    ZBuffer_Storage::instance()->uninit_buffer_list();
    return 0;
}

// line 314
int Zerg_Service_App::run_instance() {
    size_t num_io_event = 0;
    size_t num_send_frame = 0;
    size_t want_send_frame = 0;
    size_t i = 0;
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Zerg_Service_App::run_instance start.");
    ZEN_Reactor *preactor = ZEN_Reactor::instance();
    ZEN_Timer_Queue *p_timer_queue = ZEN_Timer_Queue::instance();
    ZEN_Time_Value run_interval(0, IDLE_REACTOR_WAIT_USEC);

    while (app_run_) {
        size_t max_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;
        if (want_send_frame > SEND_BUSY_JUDGE_STANDARD) {
            if (num_send_frame == 0) {
                run_interval.usec(SEND_BUSY_REACTOR_WAIT_USEC);
            } else {
                run_interval.usec(BUSY_REACTOR_WAIT_USEC);
            }
            max_send_frame = SENDBUSY_MAX_ONCE_SEND_FRAME;
        } else if (want_send_frame > SEND_IDLE_JUDGE_STANDARD) {
            run_interval.usec(BUSY_REACTOR_WAIT_USEC);
        } else {
            if (num_send_frame == 0) {
                run_interval.usec(IDLE_REACTOR_WAIT_USEC);
            } else {
                run_interval.usec(NORMAL_REACTOR_WAIT_USEC);
            }
        }

        preactor->handle_events(&run_interval, &num_send_frame);
        zerg_comm_mgr_->popall_sendpipe_write(max_send_frame, want_send_frame);
        if (want_send_frame > SEND_BUSY_JUDGE_STANDARD && num_send_frame == 0) {
            zerg_comm_mgr_->popall_sendpipe_write(SENDBUSY_MAX_ONCE_SEND_FRAME, want_send_frame);
        } else if (want_send_frame >= NORMAL_MAX_ONCE_SEND_FRAME) {
            zerg_comm_mgr_->popall_sendpipe_write(SENDBUSY_MAX_ONCE_SEND_FRAME, want_send_frame);
        }

        if (i % DEFAULT_IO_FIRST_RATIO == 0) {
            p_timer_queue->expire();
        }
        i++;
    }

    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Zerg_Service_App::run_instance end.");
    return 0;
}

// line 390
Zerg_Service_App * Zerg_Service_App::instance() {
    if (instance_ == NULL) {
        instance_ = new Zerg_Service_App;
    }
    return instance_;
}

// line 402
void Zerg_Service_App::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

// 显式实例化（原二进制弱符号：Comm_App_Frame 模板由服务 TU 发出）
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"

template int Comm_App_Frame::appdata_encode<sec_proto::CfgFileReq>(unsigned int, const sec_proto::CfgFileReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::CheckCfgFileUpdateReq>(unsigned int, const sec_proto::CheckCfgFileUpdateReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::DeployInfoReq>(unsigned int, const sec_proto::DeployInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::LogSvrCommBillRecord>(unsigned int, const sec_proto::LogSvrCommBillRecord&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIdReq>(unsigned int, const sec_proto::SvcIdReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_encode<sec_proto::SvcIpInfoReq>(unsigned int, const sec_proto::SvcIpInfoReq&, unsigned int, size_t*);
template int Comm_App_Frame::appdata_decode<sec_proto::CfgFileRsp>(sec_proto::CfgFileRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CfgsvrInfoRsp>(sec_proto::CfgsvrInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::CheckCfgFileUpdateRsp>(sec_proto::CheckCfgFileUpdateRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::DeployInfoRsp>(sec_proto::DeployInfoRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIdRsp>(sec_proto::SvcIdRsp&, unsigned int, size_t*) const;
template int Comm_App_Frame::appdata_decode<sec_proto::SvcIpInfoRsp>(sec_proto::SvcIpInfoRsp&, unsigned int, size_t*) const;
