// Reconstructed from secagent binary DWARF + disassembly (2026-08-10)
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_user_mgr.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/formmog/secagent/secagent_predefine.h"
#include "src/commlib/zenlib/zen_predefine.h"
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
#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_shm_hash_expire.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_crc32.h"
#include "src/commlib/zenlib/zen_digit_list.h"
#include "src/commlib/zenlib/zen_key_value_config.h"
#include "src/commlib/zenlib/zen_data_with_version.h"
#include "src/commlib/zenlib/zen_code_engine.h"
#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_id_to_string.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_os_adapt_time.h"
#include "src/commlib/zenlib/zen_mysql_connect.h"
#include "src/commlib/zenlib/zen_mysql_predefine.h"
#include "src/commlib/zenlib/zen_mysql_command.h"
#include "src/commlib/zenlib/zen_mysql_result.h"
#include "src/commlib/zenlib/zen_os_adapt_string.h"
#include "src/commlib/zenlib/zen_trace_log_debug.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_base.h"
#include "src/commlib/zenlib/zen_lock_guard.h"
#include "src/commlib/zenlib/zen_mysql_field.h"
#include "src/commlib/zenlib/zen_mysql_process.h"
#include "src/commlib/zenlib/zen_shm_cache_chunk.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_server_toolkit.h"
#include "src/commlib/zenlib/zen_os_adapt_process.h"
#include "src/commlib/zenlib/zen_os_adapt_sysinfo.h"
#include "src/commlib/zenlib/zen_shm_hash_table.h"
#include "src/commlib/zenlib/zen_boost_lord_rings.h"
#include "src/commlib/zenlib/zen_os_adapt_thread.h"
#include "src/commlib/zenlib/zen_os_adapt_socket.h"
#include "src/commlib/zenlib/zen_os_adapt_error.h"
#include "src/commlib/zenlib/zen_os_adapt_getopt.h"
#include "src/commlib/zenlib/zen_os_adapt_file.h"
#include "src/commlib/zenlib/zen_os_adapt_dirent.h"
#include "src/commlib/zenlib/zen_share_mem_mmap.h"
#include "src/commlib/zenlib/zen_lock_null_lock.h"
#include "src/commlib/zenlib/zen_config_property_tree.h"
#include "src/commlib/zenlib/zen_config_ini_implement.h"
#include "src/commlib/zenlib/zen_thread_msgque_sema.h"
#include "src/commlib/zenlib/zen_lock_synch_traits.h"
#include "src/commlib/zenlib/zen_lock_process_mutex.h"
#include "src/commlib/zenlib/zen_lock_thread_rw_mutex.h"
#include "src/commlib/zenlib/zen_lock_thread_semaphore.h"
#include "src/commlib/zenlib/zen_lock_thread_condi.h"
#include "src/commlib/zenlib/zen_os_adapt_condi.h"
#include "src/commlib/zenlib/zen_thread_msgque_template.h"
#include "src/commlib/zenlib/zen_thread_msgque_nonlock.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/zen_timer_queue_wheel.h"
#include "src/commlib/zenlib/zen_bus_two_way.h"
#include "src/commlib/zenlib/zen_bus_mmap_pipe.h"
#include "src/commlib/zenlib/zen_thread_task.h"
#include "src/commlib/zenlib/zen_socket_addr_base.h"
#include "src/commlib/zenlib/zen_socket_addr_in.h"
#include "src/commlib/zenlib/zen_socket_base.h"
#include "src/commlib/zenlib/zen_socket_stream.h"
#include "src/commlib/zenlib/zen_socket_datagram.h"
#include "src/commlib/zenlib/zen_socket_connector.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include "src/commlib/zenlib/zen_event_reactor_select.h"
#include "src/commlib/zenlib/zen_event_reactor_epoll.h"
#include "src/commlib/zenlib/zen_thread_wait_mgr.h"
#include "src/commlib/zenlib/zen_config_getopt.h"
#include "src/commlib/zenlib/zen_string_util.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_shm_vector.h"
#include "src/commlib/zenlib/zen_thread_bus_pipe.h"
#include "output/protocol/common/release/include/comm_proto_cfgsvr.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"
#include "output/protocol/common/release/include/comm_proto_public_head.h"
#include "output/protocol/common/release/include/comm_proto_public_cmd.h"
#include "src/protocol/common/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_logsvr.h"
#include "src/protocol/common/comm_proto_public_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_monitorsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"
#include "src/protocol/common/comm_conf_framework.h"
#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrIO.h"
#include "src/protocol/common/comm_conf_svcid.h"
#include "src/protocol/common/comm_conf_zerg.h"
#include "src/protocol/common/comm_conf_cfgsdk.h"
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
#include "src/commlib/framework/comm_svrd_app_non_ctrl.h"
#include "src/commlib/framework/comm_svrd_application.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_bill_record.h"
#include "src/commlib/framework/comm_time_provider.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "output/formmog/appcomm/release/include/app_punish_mode.h"
#include "output/formmog/appcomm/release/include/app_svr_type.h"
#include "output/formmog/appcomm/release/include/app_error_code.h"
#include "output/formmog/appcomm/release/include/app_anti_err_code.h"
#include "import/include/tencore/tencrypt/TenCrypt.h"
#include "src/protocol/formmog/formmog_proto_antibot_client.h"
#include "src/protocol/formmog/formmog_proto_public_secsvr.h"
#include "src/protocol/formmog/formmog_proto_public_cmd.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"
#include "output/protocol/formmog/release/include/formmog_proto_tss_sdk_client.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include "src/formmog/secagent/secagent_user_info.h"
#include "src/formmog/secagent/secagent_bill.h"
#include "src/formmog/secagent/secagent_app.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/formmog/secagent/secagent_restrict.h"
#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/formmog/secagent/secagent_static_config_dec.h"
#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/formmog/secagent/secagent_game_exe.h"

#include <string.h>

// line 8
SecAgentUserInfo::SecAgentUserInfo() {
    base_info_.uin_ = 0;
    base_info_.appid_ = 0;
    base_info_.channel_id_ = 0;
    memset(&antibot_crypt_info_, 0, sizeof(antibot_crypt_info_));
    memset(&rpcode_crypt_info_, 0, sizeof(rpcode_crypt_info_));
    antibot_error_code_num_.decrypt_fail_num_ = 0;
    antibot_error_code_num_.unecncrypt_fail_num_ = 0;
    antibot_error_code_num_.test_pkg_fail_num_ = 0;
    antibot_error_code_num_.de_log_fail_num_ = 0;
    login_continue_times_ = 0;
}

// line 17
SecAgentUserInfo::SecAgentUserInfo(unsigned int uin, unsigned int appid) {
    base_info_.uin_ = uin;
    base_info_.appid_ = appid;
    base_info_.channel_id_ = 0;
    memset(&antibot_crypt_info_, 0, sizeof(antibot_crypt_info_));
    memset(&rpcode_crypt_info_, 0, sizeof(rpcode_crypt_info_));
    antibot_error_code_num_.decrypt_fail_num_ = 0;
    antibot_error_code_num_.unecncrypt_fail_num_ = 0;
    antibot_error_code_num_.test_pkg_fail_num_ = 0;
    antibot_error_code_num_.de_log_fail_num_ = 0;
    login_continue_times_ = 0;
}

SecAgentUserInfoMgr * SecAgentUserInfoMgr::instance_ = NULL;

// line 29
SecAgentUserInfoMgr::SecAgentUserInfoMgr() : user_info_itr_(0, NULL) {
}

// line 33
SecAgentUserInfoMgr::~SecAgentUserInfoMgr() {
}

// line 37
SecAgentUserInfoMgr * SecAgentUserInfoMgr::instance() {
    if (instance_ == NULL) {
        instance_ = new SecAgentUserInfoMgr();
    }
    return instance_;
}

// line 47
void SecAgentUserInfoMgr::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

// line 55
int SecAgentUserInfoMgr::init(size_t data_num, bool if_restore) {
    int rc;
    size_t malloc_size;
    char file_path[] = "./PLAT_USER_INFO_.MMAP";

    size_t num_of_node = user_hash_table::get_next_prime(data_num);
    malloc_size = user_hash_table::getallocsize(num_of_node);

    if (ZEN_OS::access(file_path, 0) != 0) {
        if_restore = false;
    }

    rc = mmap_file_.open(file_path, malloc_size, if_restore, false, true,
                         NULL, 0);
    if (rc != 0) {
        return 0x493f3;
    }

    char *base_addr = static_cast<char *>(mmap_file_.addr());
    if (if_restore) {
        size_t check_num = user_hash_table::get_next_prime(num_of_node);
        if (*(size_t *)base_addr != user_hash_table::getallocsize(check_num) ||
            ((size_t *)base_addr)[1] != check_num) {
            hash_tab_user_ = NULL;
            return 0x493f4;
        }
    }

    ((size_t *)base_addr)[0] = malloc_size;
    ((size_t *)base_addr)[1] = num_of_node;
    hash_tab_user_ = new user_hash_table(base_addr);
    if (!if_restore) {
        hash_tab_user_->lru_hash_head_->sz_freenode_ = num_of_node;
        hash_tab_user_->lru_hash_head_->sz_usenode_ = 0;
        hash_tab_user_->lru_hash_head_->sz_useindex_ = 0;
        hash_tab_user_->lst_use_node_[0].idx_next_ = num_of_node;
        hash_tab_user_->lst_use_node_[0].idx_prev_ = num_of_node;
        hash_tab_user_->lst_free_node_[0].idx_next_ = num_of_node + 1;
        hash_tab_user_->lst_free_node_[0].idx_prev_ = num_of_node + 1;
        if (num_of_node != 0) {
            for (size_t i = 0; i < num_of_node; ++i) {
                hash_tab_user_->hash_factor_base_[i] = (size_t)-1;
                hash_tab_user_->hash_index_base_[i] = (size_t)-1;
                hash_tab_user_->priority_base_[i] = 0;
                hash_tab_user_->lst_index_base_[i].idx_next_ = i + 1;
                hash_tab_user_->lst_index_base_[i].idx_prev_ = i - 1;
                if (i == 0) {
                    hash_tab_user_->lst_index_base_[0].idx_prev_ =
                        hash_tab_user_->lst_free_node_[0].idx_next_;
                    hash_tab_user_->lst_free_node_[0].idx_next_ = 0;
                }
            }
            hash_tab_user_->lst_index_base_[num_of_node - 1].idx_next_ =
                hash_tab_user_->lst_free_node_[0].idx_prev_;
            hash_tab_user_->lst_free_node_[0].idx_prev_ = num_of_node - 1;
        }
    }
    return 0;
}

// line 92
int SecAgentUserInfoMgr::add_user(unsigned int uin, unsigned int game_id,
                                  unsigned short channel_id) {
    SecAgentUserInfo temp_p(uin, game_id);
    temp_p.base_info_.channel_id_ = channel_id;
    ExtractSecAgentUserKey extract;
    uint64_t key = extract(temp_p);

    user_hash_iterator itr = hash_tab_user_->find(key);
    if (itr != hash_tab_user_->end()) {
        SecAgentUserInfo *user_info = &*itr;
        ++user_info->login_continue_times_;
        g_bill << ConstDataSource(0x1e) << UserID(uin) << LogType(15)
               << GameID(game_id) << LogComment("user continue login")
               << (int64_t)user_info->login_continue_times_ << endl;
        ZEN_Trace_LogMsg::debug_debugEx("add user succ, uin=%u", uin);
        return 0;
    }

    time_t now = time(NULL);
    itr = hash_tab_user_->find(key);
    if (itr != hash_tab_user_->end()) {
        return 0;
    }

    if (hash_tab_user_->full()) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], no free node",
            "int SecAgentUserInfoMgr::add_user(unsigned int, unsigned int, short unsigned int)");
        return 0x493f5;
    }

    hash_tab_user_->insert(temp_p, (unsigned int)now);
    ZEN_Trace_LogMsg::debug_debugEx("add user succ, uin=%u", uin);
    return 0;
}

// line 139
int SecAgentUserInfoMgr::del_user(unsigned int uin, unsigned int game_id) {
    SecAgentUserInfo temp_p(uin, game_id);
    ExtractSecAgentUserKey extract;
    uint64_t key = extract(temp_p);

    user_hash_iterator itr = hash_tab_user_->find(key);
    if (itr == hash_tab_user_->end()) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], user not exist, del user fail, uin=%u",
            "int SecAgentUserInfoMgr::del_user(unsigned int, unsigned int)", uin);
        return 0x493ef;
    }
    SecAgentUserInfo *user_info = &*itr;
    if (user_info->login_continue_times_ != 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "user continue login, will not del, uin=%u, gameid=%u, login_continue_times_=%u",
            uin, game_id, user_info->login_continue_times_);
        --user_info->login_continue_times_;
        return 0;
    }
    user_info->antibot_crypt_info_.key_len_ = 0;

    itr = hash_tab_user_->find(key);
    if (itr == hash_tab_user_->end()) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], del user fail, uin=%u",
            "int SecAgentUserInfoMgr::del_user(unsigned int, unsigned int)", uin);
        return 0x493ef;
    }
    hash_tab_user_->erase(itr.getserial());
    return 0;
}

// line 188
SecAgentUserInfo * SecAgentUserInfoMgr::get_user(unsigned int uin,
                                                 unsigned int game_id) {
    const SecAgentUserInfo temp_p(uin, game_id);
    ExtractSecAgentUserKey extract;
    uint64_t key = extract(temp_p);

    user_hash_iterator itr = hash_tab_user_->find(key);
    if (itr == hash_tab_user_->end()) {
        return NULL;
    }
    return &*itr;
}

// line 204
int SecAgentUserInfoMgr::expire(const ZEN_Time_Value &now) {
    SecAgentDPsdkInfoMgr::LPCONFIG conf = SecAgentApp::instance()->get_config();
    time_t expire_time = now.sec() - conf->expire_time_;
    int expire_num = 0;

    size_t pos = hash_tab_user_->lst_use_node_[0].idx_prev_;
    while (pos != hash_tab_user_->lru_hash_head_->num_of_node_) {
        if (hash_tab_user_->priority_base_[pos] > (unsigned int)expire_time) {
            break;
        }
        SecAgentUserInfo *user_info = &hash_tab_user_->value_base_[pos];
        ZEN_Trace_LogMsg::debug_infoex("time out user, %u:%u",
            user_info->base_info_.uin_, user_info->base_info_.appid_);
        SecAgentBillHandler::write_bill_expire_user(user_info->base_info_.uin_,
                                                    user_info->base_info_.appid_);
        SecAgentBillHandler::instance()->add_statistic_bill(0x493e7, 1);
        ++expire_num;
        hash_tab_user_->erase(pos);
        pos = hash_tab_user_->lst_use_node_[0].idx_prev_;
    }
    return expire_num;
}

// line 216
void SecAgentUserInfoMgr::active(unsigned int uin, unsigned int appid) {
    uint64_t key = ((uint64_t)uin << 32) | appid;
    time_t now = time(NULL);

    user_hash_iterator itr = hash_tab_user_->find(key);
    if (itr != hash_tab_user_->end()) {
        hash_tab_user_->active(itr.getserial(), (unsigned int)now);
    }
}

// line 223
SecAgentUserInfo * SecAgentUserInfoMgr::get_first_node() {
    user_info_itr_ = hash_tab_user_->begin();
    if (user_info_itr_.getserial() == (size_t)-1) {
        return NULL;
    }
    return &*user_info_itr_;
}

// line 235
SecAgentUserInfo * SecAgentUserInfoMgr::get_next_node() {
    ++user_info_itr_;
    if (user_info_itr_.getserial() == (size_t)-1) {
        return NULL;
    }
    return &*user_info_itr_;
}

// line 248
size_t SecAgentUserInfoMgr::get_online_user_num() {
    return hash_tab_user_->size();
}
