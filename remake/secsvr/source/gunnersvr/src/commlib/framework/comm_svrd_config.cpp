// Reconstructed from gunnersvr disassembly + DWARF (GCC 4.1.0 SUSE, -O2).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/framework/comm_svrd_config.cpp

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
#include "src/commlib/framework/comm_server_ver_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_bill_record.h"
#include "src/commlib/framework/comm_time_provider.h"
#include "src/commlib/framework/comm_xml_config.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <strings.h>

// comm_svrd_application.h is still a broken stub (owned by another agent);
// declare only the interface used here. Semantics taken from the binary
// (_ZN20Comm_Svrd_Appliction8instanceEv / 16get_app_basenameEv).
struct Comm_Svrd_Appliction {
    static Comm_Svrd_Appliction *instance();
    const char *get_app_basename();
};

// Replaces every occurrence of range [fo, lo) in s with range [fn, ln).
// Matches the binary symbol:
//   replace_all<char, char_traits<char>, allocator<char>, const char*, const char*, equal_to<char>>
template <typename Ch, typename Tr, typename A, typename I1, typename I2, typename Pred>
int replace_all(std::basic_string<Ch, Tr, A> &s, I1 fo, I1 lo, I2 fn, I2 ln, Pred pr)
{
    if (fo == lo) {
        return 0;
    }
    typedef typename std::basic_string<Ch, Tr, A>::iterator iterator_type;
    iterator_type p = std::search(s.begin(), s.end(), fo, lo, pr);
    if (p == s.end()) {
        return 0;
    }
    std::basic_string<Ch, Tr, A> tmp;
    tmp.reserve(s.size());
    iterator_type old_pos = s.begin();
    int count = 0;
    while (p != s.end()) {
        tmp.append(old_pos, p);
        tmp.append(fn, ln);
        ++count;
        old_pos = p + (lo - fo);
        p = std::search(old_pos, s.end(), fo, lo, pr);
    }
    tmp.append(old_pos, s.end());
    s.swap(tmp);
    return count;
}

Comm_Svrd_Config *Comm_Svrd_Config::instance_ = NULL;

void Comm_Svrd_Config::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

bool Comm_Svrd_Config::is_monitor_uin(unsigned int uin) {
    return monitor_uin_set_.find(uin) != monitor_uin_set_.end();
}

int Comm_Svrd_Config::usage(const char *program_name) {
    std::cout << "usage: " << program_name << std::endl;
    std::cout << "   -z [zergling cfg path]" << std::endl;
    std::cout << "   -c [services cfg file]" << std::endl;
    std::cout << "   -d run as daemon" << std::endl;
    std::cout << "   -n reset channel mmp" << std::endl;
    std::cout << "   -v show version" << std::endl;
    std::cout << "   -t service type" << std::endl;
    std::cout << "   -i service index" << std::endl;
    std::cout << "   -p pull config from cfgsvr" << std::endl;
    std::cout << "   -m install app as windows servcie" << std::endl;
    std::cout << "   -u uninstall app as windows servcie" << std::endl;
    std::cout << "   -h show help info." << std::endl;
    std::cout << "Server Version :1.0.387  .\n"
                 "Tss is compiled by release version, NDEBUG is defined.\n"
                 "Compilation Timestamp :Jan 15 2013 18:40:12 .\n"
                 "Copyright :(C) 2008-2012 . Tencent TSS platform Team (We are best!). All rights reserved. \n"
                 "Author :charliedeng chweiling djiang errayzhao fieldsxie gagacui jiangchen junhuanie kelvinpang"
                 "Author :kliu liangfeng longma sailzeng sasukeliu senlyzhang sriverxiao stefzhou wilsonliu.\n"
                 "Author :pascalshen yunfeiyang derrickhu\n\n"
                 "Have fun. My brother.  You Can't Write Perfect Software !  Free as in Freedom .\n"
              << std::endl;
    return 0;
}

Comm_Svrd_Config::Comm_Svrd_Config()
    : self_svr_id_(0, 0),
      app_run_dir_(),
      if_restore_pipe_(true),
      app_run_daemon_(false),
      app_install_service_(false),
      app_uninstall_service_(false),
      log_file_prefix_(),
      zerg_cfg_file_(),
      app_cfg_file_(),
      framework_cfg_file_(),
      svcid_cfg_file_(),
      master_cfgsvr_ip_(),
      instance_id_(1),
      is_use_cfgsvr_(false)
{
}

Comm_Svrd_Config::~Comm_Svrd_Config() {
}

Comm_Svrd_Config *Comm_Svrd_Config::instance() {
    if (instance_ == NULL) {
        instance_ = new Comm_Svrd_Config();
    }
    return instance_;
}

int Comm_Svrd_Config::init(int argc, const char **argv) {
    int ret = proc_start_arg(argc, argv);
    if (ret != 0) {
        return ret;
    }
    return load_config();
}

int Comm_Svrd_Config::proc_start_arg(int argc, const char **argv) {
    char cur_dir[4096];
    ZEN_OS::getcwd(cur_dir, 0x1000);
    app_run_dir_.assign(cur_dir, strlen(cur_dir));

    ZEN_Get_Option get_opt(argc, (char **)argv, "umvndhpi:t:r:a:", 1, 0, 3, 0);
    int c;
    while ((c = get_opt()) != -1) {
        switch (c) {
        case 'u':
            app_uninstall_service_ = true;
            break;
        case 'm':
            app_install_service_ = true;
            break;
        case 'v':
            puts("Server Version :1.0.387  .\n"
                 "Tss is compiled by release version, NDEBUG is defined.\n"
                 "Compilation Timestamp :Jan 15 2013 18:40:12 .\n"
                 "Copyright :(C) 2008-2012 . Tencent TSS platform Team (We are best!). All rights reserved. \n"
                 "Author :charliedeng chweiling djiang errayzhao fieldsxie gagacui jiangchen junhuanie kelvinpang"
                 "Author :kliu liangfeng longma sailzeng sasukeliu senlyzhang sriverxiao stefzhou wilsonliu.\n"
                 "Author :pascalshen yunfeiyang derrickhu\n\n"
                 "Have fun. My brother.  You Can't Write Perfect Software !  Free as in Freedom .\n");
            exit(0);
            break;
        case 'n':
            if_restore_pipe_ = true;
            break;
        case 'd':
            app_run_daemon_ = true;
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'p':
            is_use_cfgsvr_ = true;
            break;
        case 'i':
            instance_id_ = (unsigned short)strtol(get_opt.opt_arg(), NULL, 10);
            break;
        case 't':
            self_svr_id_.services_type_ = (unsigned short)strtol(get_opt.opt_arg(), NULL, 10);
            break;
        case 'r':
            ZEN_Trace_LogMsg::debug_infoex("app run dir = %s", app_run_dir_.c_str());
            app_run_dir_.assign(get_opt.opt_arg(), strlen(get_opt.opt_arg()));
            replace_all(app_run_dir_, "\\", "\\" + 1, "/", "/" + 1, std::equal_to<char>());
            break;
        case 'a':
            is_use_cfgsvr_ = true;
            master_cfgsvr_ip_.assign(get_opt.opt_arg(), strlen(get_opt.opt_arg()));
            break;
        default:
            printf("unknow argu %c\n", c);
            usage(argv[0]);
            return 0x114933a2;
        }
    }

    std::string tmp_str = app_run_dir_ + "/log/";
    log_file_prefix_ = tmp_str;
    if (tmp_str.empty()) {
        log_file_prefix_ += Comm_Svrd_Appliction::instance()->get_app_basename();
        log_file_prefix_ += "_init";
    }

    tmp_str = app_run_dir_ + "/cfg/";
    app_cfg_file_ = tmp_str;
    if (tmp_str.empty()) {
        app_cfg_file_ += Comm_Svrd_Appliction::instance()->get_app_basename();
        app_cfg_file_ += "_config.xml";
    }

    zerg_cfg_file_ = app_run_dir_ + "/cfg/zergsvrd.xml";
    svcid_cfg_file_ = app_run_dir_ + "/cfg/svcid.xml";
    framework_cfg_file_ = app_run_dir_ + "/cfg/framework.xml";
    return 0;
}

int Comm_Svrd_Config::load_config() {
    int ret = zerg_config_.fromXmlFile(zerg_cfg_file_.c_str(), tsf4g_tdr::LIST_ENTRY, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("Comm_Svrd_Config: load zerg config fail. ret=%d, path=%s",
                                        ret, zerg_cfg_file_.c_str());
        return 0x1423;
    }

    self_svr_id_.services_type_ = zerg_config_.self_cfg.self_svr_info.svr_type;
    self_svr_id_.services_id_ = zerg_config_.self_cfg.self_svr_info.svr_id;

    ret = framework_config_.fromXmlFile(framework_cfg_file_.c_str(), tsf4g_tdr::LIST_ENTRY, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("Comm_Svrd_Config: load framework config fail.ret=%d", ret);
        return 0x1424;
    }

    monitor_uin_set_.clear();
    unsigned int monitor_uin_count = framework_config_.log_info_.monitor_uin_count_;
    for (unsigned int i = 0; i < monitor_uin_count; ++i) {
        monitor_uin_set_.insert(framework_config_.log_info_.monitor_uin_list_[i]);
    }

    if (strcasecmp(zerg_config_.comm_cfg.get_svr_info_type, "cfgfile") == 0) {
        ret = svcid_config_.fromXmlFile(svcid_cfg_file_.c_str(), tsf4g_tdr::LIST_ENTRY, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("Comm_Svrd_Config: load svcid config fail. ret=%d", ret);
            return 0x1425;
        }
    }

    std::string tmp_str = app_run_dir_ + "/log/";
    log_file_prefix_ = tmp_str;
    if (tmp_str.empty()) {
        log_file_prefix_ += Comm_Svrd_Appliction::instance()->get_app_basename();
    }

    ZEN_Trace_LogMsg::debug_infoex("Comm_Svrd_Config: load framework config succ.");
    return 0;
}

int Comm_Svrd_Config::reload() {
    ZEN_Trace_LogMsg::debug_infoex("app start reload");
    return load_config();
}
