// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_bill.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

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
#include "src/formmog/secagent/secagent_bill.h"

SecAgentBillHandler::SecAgentBillHandler() {
}

SecAgentBillHandler::~SecAgentBillHandler() {
}

// line 32
void SecAgentBillHandler::add_statistic_bill(unsigned int statistic_id, unsigned int incre) {
}

// line 17
void SecAgentBillHandler::clean_instance() {
}

// line 6
SecAgentBillHandler * SecAgentBillHandler::instance() {
}

// line 45
void SecAgentBillHandler::write_bill_add_user_from_sdk(unsigned int game_id, unsigned int uin, unsigned int cmd, unsigned int ret, short unsigned int channel_id) {
}

// line 63
void SecAgentBillHandler::write_bill_del_user_from_sdk(unsigned int game_id, unsigned int uin, unsigned int cmd, unsigned int ret, short unsigned int channel_id) {
}

// line 277
void SecAgentBillHandler::write_bill_antibot_cmd_list(unsigned int uin, unsigned int appid, uint8_t cmd_count) {
}

// line 260
void SecAgentBillHandler::write_bill_antibot_crypt_info(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_len, unsigned int key_offset, unsigned int crc) {
}

// line 243
void SecAgentBillHandler::write_bill_notify_rpcode_info(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_offset) {
}

// line 228
void SecAgentBillHandler::write_bill_notify_is_change_svr(unsigned int uin, unsigned int appid, unsigned int is_change_sv) {
}

// line 212
void SecAgentBillHandler::write_bill_antibot_de_log(unsigned int uin, unsigned int appid, unsigned int error_id, unsigned int error, unsigned int pad_size) {
}

// line 196
void SecAgentBillHandler::write_bill_antibot_nocrypt_fail(unsigned int uin, unsigned int appid, unsigned int cmd_id) {
}

// line 180
void SecAgentBillHandler::write_bill_antibot_decrypt_fail(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_offset, unsigned int key_len) {
}

// line 160
void SecAgentBillHandler::write_bill_antibot_ready_pkg(unsigned int uin, unsigned int appid, unsigned int pkg_dibid, unsigned int local_dibid, unsigned int pkg_crc, unsigned int local_crc, uint8_t is_ready) {
}

// line 140
void SecAgentBillHandler::write_bill_rpcode_check_fail(unsigned int uin, unsigned int appid, unsigned int local_hash, unsigned int client_hash, unsigned int dib_id) {
}

// line 121
void SecAgentBillHandler::write_bill_antibot_test_decypt_fail(unsigned int uin, unsigned int appid, unsigned int pkg_crc, unsigned int tmp_crc, unsigned int content1, unsigned int content2) {
}

// line 104
void SecAgentBillHandler::write_bill_send_to_secsvr(unsigned int uin, unsigned int appid, unsigned int cmd) {
}

// line 90
void SecAgentBillHandler::write_bill_send_secsvr_error(unsigned int uin, unsigned int appid, unsigned int error_id) {
}

// line 77
void SecAgentBillHandler::write_bill_expire_user(unsigned int uin, unsigned int appid) {
}

SecAgentBillHandler *SecAgentBillHandler::instance_ = NULL;
