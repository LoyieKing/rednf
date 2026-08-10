// Reconstructed from secagent disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_antibot_msg_handler.cpp
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

// 按二进制还原的字节序交换（GCC 4.1 编译为 ror $8）
static inline unsigned short secagent_bswap16(unsigned short v) {
    return (unsigned short)((v << 8) | (v >> 8));
}

// 本文件使用的 secagent antibot 返回码（app_anti_err_code.h 枚举名未知，值按二进制还原）
static const int SECAGENT_ANTIBOT_GET_USER_FAIL = 0x493e7;
static const int SECAGENT_ANTIBOT_UNPACK_PKG_FAIL = 0x493ea;
static const int SECAGENT_ANTIBOT_PACK_PKG_FAIL = 0x493e9;
static const int SECAGENT_ANTIBOT_DECRYPT_FAIL = 0x493e8;
static const int SECAGENT_ANTIBOT_READY_FAIL = 0x493f1;
static const int SECAGENT_ANTIBOT_RPCODE_KEY_NOT_READY = 0x49400;
static const int SECAGENT_ANTIBOT_RPCODE_KEY_LEN_ERR = 0x49401;
static const int SECAGENT_ANTIBOT_GAME_EXE_FAIL = 0x4940a;
static const int SECAGENT_ANTIBOT_SANLIX_DATA_FAIL = 0x4940d;
static const int SECAGENT_ANTIBOT_GET_SANLIX_FAIL = 0x4940e;

// 32 位字节序读取（rpcode pkg 的 size/hash 字段跨 u16 边界读取，按二进制复现）
static inline unsigned int secagent_read32(const void *p) {
    const unsigned char *b = (const unsigned char *)p;
    return (unsigned int)b[0] | ((unsigned int)b[1] << 8) |
           ((unsigned int)b[2] << 16) | ((unsigned int)b[3] << 24);
}

// line 12
SecagentAntibotMsgHandler::SecagentAntibotMsgHandler() {
    user_info_ = NULL;
}

// line 16
SecagentAntibotMsgHandler::~SecagentAntibotMsgHandler() {
}

// line 22
int SecagentAntibotMsgHandler::proc_tsssdk_antibot_msg(Comm_App_Frame *recv_frame,
                                                       sec_proto::RecvDataFromSDKReq *sdk_pkg) {
    user_info_ = SecAgentUserInfoMgr::instance()->get_user(recv_frame->frame_uin_,
                                                           recv_frame->app_id_);
    if (user_info_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], get user fail, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_tsssdk_antibot_msg(Comm_App_Frame*, sec_proto::RecvDataFromSDKReq*)",
                                        recv_frame->frame_uin_);
        return SECAGENT_ANTIBOT_GET_USER_FAIL;
    }
    uint16_t antibot_cmd = secagent_bswap16(*(const uint16_t *)sdk_pkg->data_);
    ZEN_Trace_LogMsg::debug_debugEx("recv tsssdk antibot pkg, antibot_cmd=%d, uin=%u",
                                    antibot_cmd, user_info_->base_info_.uin_);
    SecAgentBillHandler::add_statistic_bill(0x493e9, 1); // SDK_REQ_RECV_DATA_ANTIBOT_NUM
    is_encrypt_pkg_ = false;
    int ret = 0;
    switch (antibot_cmd) {
    case 0x26:
        ret = decrypt(sdk_pkg, &antibot_cmd);
        if (ret != 0) {
            return ret;
        }
        is_encrypt_pkg_ = true;
        // fall through：用解密后的真实 cmd 继续分发
    case 0x20:
        ret = proc_ab_de_testpkg(*sdk_pkg);
        break;
    case 0x3:
        ret = proc_ab_init_pkg(*sdk_pkg);
        break;
    case 0x22:
        ret = proc_ab_de_log(*sdk_pkg);
        break;
    case 0x1f:
        ret = proc_ab_ready_pkg(*sdk_pkg);
        break;
    case 0x65:
        ret = proc_ab_rpcode_pkg(sdk_pkg);
        is_encrypt_pkg_ = true;
        break;
    default:
        ret = proc_tsssdk_ab_other_msg(antibot_cmd, *sdk_pkg);
        break;
    }
    if (ret != 0) {
        return ret;
    }
    if (is_encrypt_pkg_) {
        ret = recv_frame->appdata_encode<sec_proto::RecvDataFromSDKReq>(0xffbd, *sdk_pkg, 0, 0);
    }
    return ret;
}

// line 112
int SecagentAntibotMsgHandler::proc_secsvr_antibot_msg(Comm_App_Frame *recv_frame,
                                                       sec_proto::SendToSDK *sdk_pkg) {
    user_info_ = SecAgentUserInfoMgr::instance()->get_user(recv_frame->frame_uin_,
                                                           recv_frame->app_id_);
    if (user_info_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], get user fail, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_secsvr_antibot_msg(Comm_App_Frame*, sec_proto::SendToSDK*)",
                                        recv_frame->frame_uin_);
        return SECAGENT_ANTIBOT_GET_USER_FAIL;
    }
    uint16_t antibot_cmd = secagent_bswap16(*(const uint16_t *)sdk_pkg->data_);
    ZEN_Trace_LogMsg::debug_debugEx(
        "SecagentAntibotMsgHandler::proc_secsvr_antibot_msg recv secsvr antibot pkg, antibot_cmd=%d, uin=%u",
        antibot_cmd, recv_frame->frame_uin_);
    SecAgentBillHandler::add_statistic_bill(0x493f6, 1); // SECSVR_FORWARD_ANTIBOT_NUM
    switch (antibot_cmd) {
    case 0x15:
    case 0x32:
    case 0x33:
    case 0x35:
        return proc_sanlix_data_msg(antibot_cmd, recv_frame, sdk_pkg);
    case 0x1d:
    case 0x1e:
        ZEN_Trace_LogMsg::debug_errorex("[%s], should not recv the pkg, cmd=%u, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_secsvr_antibot_msg(Comm_App_Frame*, sec_proto::SendToSDK*)",
                                        antibot_cmd, recv_frame->frame_uin_);
        return 0;
    default:
        return proc_secsvr_ab_other_msg(antibot_cmd, recv_frame, sdk_pkg);
    }
}

// line 171
bool SecagentAntibotMsgHandler::is_down_cmd_list(uint16_t cmd) {
    conf_ = SecAgentApp::instance()->get_config();
    ZEN_Trace_LogMsg::debug_debugEx("check is down cmd list, cmd=%u", cmd);
    for (unsigned int i = 0;
         i < conf_->crypt_checker_conf_.down_cmdlist_info_.cmd_list_count_; ++i) {
        if (conf_->crypt_checker_conf_.down_cmdlist_info_.cmd_[i] == cmd) {
            return true;
        }
    }
    return false;
}

// line 191
bool SecagentAntibotMsgHandler::is_up_cmd_list(uint16_t cmd) {
    conf_ = SecAgentApp::instance()->get_config();
    ZEN_Trace_LogMsg::debug_debugEx("check is down cmd list, cmd=%u", cmd);
    for (unsigned int i = 0;
         i < conf_->crypt_checker_conf_.up_cmdlist_info_.cmd_list_count_; ++i) {
        if (conf_->crypt_checker_conf_.up_cmdlist_info_.cmd_[i] == cmd) {
            return true;
        }
    }
    return false;
}

// line 210
int SecagentAntibotMsgHandler::decrypt(sec_proto::RecvDataFromSDKReq *sdk_pkg,
                                       uint16_t *real_antibot_cmd) {
    ZEN_Trace_LogMsg::debug_debugEx("recv antibot encrypt pkg, uin=%u, appid=%u",
                                    user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    SecAgentBillHandler::add_statistic_bill(0x493ee, 1); // RECV_ANTIBOT_ENCRYPT_NUM
    if (!user_info_->antibot_crypt_info_.antibot_ready_encrypt_ ||
        user_info_->antibot_crypt_info_.key_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("recv encrypt pkg, antibot crypt not ready, uin=%u",
                                        user_info_->base_info_.uin_);
        return 0;
    }
    unsigned int dib_id = user_info_->antibot_crypt_info_.dib_id_;
    const unsigned char *crypt_key = user_info_->antibot_crypt_info_.key_data_;
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg->data_), sdk_pkg->data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::decrypt(sec_proto::RecvDataFromSDKReq*, uint16_t*)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    ret = AntibotCrypter::decrypt(dib_id, crypt_key, &antibot_pkg);
    if (ret != 0) {
        user_info_->antibot_error_code_num_.decrypt_fail_num_++;
        if (user_info_->antibot_error_code_num_.decrypt_fail_num_ >
            conf_->crypt_checker_conf_.antibot_error_thread_) {
            ZEN_Trace_LogMsg::debug_debugEx("decrypt fail num bigger than thread, uin=%u, decrypt fail num=%u",
                                            user_info_->base_info_.uin_,
                                            user_info_->antibot_error_code_num_.decrypt_fail_num_);
            return SECAGENT_ANTIBOT_DECRYPT_FAIL;
        }
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], decrypt fail, ret=%u, uin=%d, funcid=%d, key_offset=%u, key_len=%u",
            "int SecagentAntibotMsgHandler::decrypt(sec_proto::RecvDataFromSDKReq*, uint16_t*)",
            ret, user_info_->base_info_.uin_, dib_id,
            user_info_->antibot_crypt_info_.key_offset_,
            user_info_->antibot_crypt_info_.key_len_);
        SecAgentBillHandler::add_statistic_bill(0x493f0, 1); // ANTIBOT_DECRYPT_FAIL_NUM
        SecAgentBillHandler::write_bill_antibot_decrypt_fail(
            user_info_->base_info_.uin_, user_info_->base_info_.appid_, dib_id,
            user_info_->antibot_crypt_info_.key_offset_,
            user_info_->antibot_crypt_info_.key_len_);
        send_error_pkg_to_secsvr(*sdk_pkg, 0x66);
        SecAgentBillHandler::add_statistic_bill(0x493fb, 1); // SEND_SECSVR_CHECK_DECRYPT_FAIL_NUM
        return SECAGENT_ANTIBOT_DECRYPT_FAIL;
    }
    *real_antibot_cmd = secagent_bswap16(
        *(const uint16_t *)&antibot_pkg.body_.sc_encrypt_.pkg_data_[0]);
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::decrypt, antibot decrypt succ, antibot cmd=%d, uin=%u, dib_id=%d, key_offset=%u, key_len=%u",
        *real_antibot_cmd, user_info_->base_info_.uin_, dib_id,
        user_info_->antibot_crypt_info_.key_offset_,
        user_info_->antibot_crypt_info_.key_len_);
    SecAgentBillHandler::add_statistic_bill(0x493ef, 1); // ANTIBOT_DECRYPT_SUCC_NUM
    memcpy(sdk_pkg->data_, antibot_pkg.body_.sc_encrypt_.pkg_data_,
           antibot_pkg.body_.sc_encrypt_.pkg_size_);
    sdk_pkg->data_len_ = antibot_pkg.body_.sc_encrypt_.pkg_size_;
    ZEN_Trace_LogMsg::debug_infoex("after decrypt, copy antibot data to sdk pkg succ, uin=%u",
                                   user_info_->base_info_.uin_);
    return 0;
}

// line 302
int SecagentAntibotMsgHandler::encrypt(Comm_App_Frame *recv_frame, sec_proto::SendToSDK *sdk_pkg) {
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = AntibotCrypter::encrypt(user_info_->antibot_crypt_info_.dib_id_,
                                      user_info_->antibot_crypt_info_.key_data_,
                                      sdk_pkg->data_, sdk_pkg->data_len_, &antibot_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], encrypt fail, ret=%d, uin=%u, funcid=%d, key_offset=%u, key_len=%u",
            "int SecagentAntibotMsgHandler::encrypt(Comm_App_Frame*, sec_proto::SendToSDK*)",
            ret, user_info_->base_info_.uin_, user_info_->antibot_crypt_info_.dib_id_,
            user_info_->antibot_crypt_info_.key_offset_,
            user_info_->antibot_crypt_info_.key_len_);
        SecAgentBillHandler::add_statistic_bill(0x493f9, 1); // ANTIBOT_ENCRYPT_FAIL_NUM
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL; // 0x493e8
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::encrypt, encrypt antibot pkg succ, uin=%u, funcid=%d, key_offset=%u, key_len=%u",
        user_info_->base_info_.uin_, user_info_->antibot_crypt_info_.dib_id_,
        user_info_->antibot_crypt_info_.key_offset_,
        user_info_->antibot_crypt_info_.key_len_);
    ret = pack_secsvr_antibot_to_frame(recv_frame, sdk_pkg, antibot_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], pack antibot fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::encrypt(Comm_App_Frame*, sec_proto::SendToSDK*)",
                                        ret, user_info_->base_info_.uin_);
        return ret;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::encrypt, pack antibot succ, uin=%u, antibot_data_len=%u",
        user_info_->base_info_.uin_, sdk_pkg->data_len_);
    SecAgentBillHandler::add_statistic_bill(0x493f8, 1); // ANTIBOT_ENCRYPT_SUCC_NUM
    return 0;
}

// line 358
int SecagentAntibotMsgHandler::pack_secsvr_antibot_to_frame(Comm_App_Frame *recv_frame,
                                                            sec_proto::SendToSDK *sdk_pkg,
                                                            const sec_proto::AntibotClientPkg &antibot_pkg) {
    size_t use_len = 0;
    int ret = antibot_pkg.pack(reinterpret_cast<char *>(sdk_pkg->data_), 0x7fff, &use_len, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], pack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::pack_secsvr_antibot_to_frame(Comm_App_Frame*, sec_proto::SendToSDK*, const sec_proto::AntibotClientPkg&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_PACK_PKG_FAIL;
    }
    sdk_pkg->data_len_ = (uint16_t)use_len;
    ZEN_Trace_LogMsg::debug_infoex("pack antibot succ, ret = %d, uin=%u", 0,
                                   user_info_->base_info_.uin_);
    ret = recv_frame->appdata_encode<sec_proto::SendToSDK>(0xffbd, *sdk_pkg, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], pack sdk pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::pack_secsvr_antibot_to_frame(Comm_App_Frame*, sec_proto::SendToSDK*, const sec_proto::AntibotClientPkg&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_PACK_PKG_FAIL; // 0x493eb
    }
    return 0;
}

// line 388
int SecagentAntibotMsgHandler::proc_ab_ready_pkg(const sec_proto::RecvDataFromSDKReq &sdk_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("recv antibot decrypt ready pkg, uin=%u, appid=%u",
                                    user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    SecAgentBillHandler::add_statistic_bill(0x493f1, 1); // RECV_ANTIBOT_READY_PKG_NUM
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg.data_), sdk_pkg.data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_ab_ready_pkg(const sec_proto::RecvDataFromSDKReq&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    unsigned int temp_crc = Crc32((unsigned char *)user_info_->antibot_crypt_info_.key_data_,
                                  user_info_->antibot_crypt_info_.key_len_);
    if (antibot_pkg.body_.cs_de_ready_.id_ == user_info_->antibot_crypt_info_.dib_id_ &&
        temp_crc == antibot_pkg.body_.cs_de_ready_.key_crc_) {
        user_info_->antibot_crypt_info_.antibot_ready_encrypt_ = true;
        ZEN_Trace_LogMsg::debug_infoex("proc antibot decrypt ready pkg succ, uin=%u, appid=%u",
                                       user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    } else {
        user_info_->antibot_crypt_info_.antibot_ready_encrypt_ = false;
        ZEN_Trace_LogMsg::debug_errorex("[%s], report error to secsvr, uin=%u, err_id=%d",
                                        "int SecagentAntibotMsgHandler::proc_ab_ready_pkg(const sec_proto::RecvDataFromSDKReq&)",
                                        user_info_->base_info_.uin_, 0x1c);
        SecAgentBillHandler::add_statistic_bill(0x493fa, 1); // SEND_SECSVR_CHECK_ENCRYPT_INFO_FAIL_NUM
        send_error_pkg_to_secsvr(sdk_pkg, 0x1c);
    }
    SecAgentBillHandler::write_bill_antibot_ready_pkg(
        user_info_->base_info_.uin_, user_info_->base_info_.appid_,
        antibot_pkg.body_.cs_de_ready_.id_, user_info_->antibot_crypt_info_.dib_id_,
        antibot_pkg.body_.cs_de_ready_.key_crc_, temp_crc,
        user_info_->antibot_crypt_info_.antibot_ready_encrypt_);
    return 0;
}

// line 452
int SecagentAntibotMsgHandler::proc_secsvr_ab_other_msg(uint16_t antibot_cmd,
                                                        Comm_App_Frame *recv_frame,
                                                        sec_proto::SendToSDK *sdk_pkg) {
    if (!is_down_cmd_list(antibot_cmd)) {
        return 0;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::proc_secsvr_ab_other_msg, cmd in down cmd list, uin=%u, appid=%u, antibotcmd=%d, ready_encrypt=%d",
        user_info_->base_info_.uin_, user_info_->base_info_.appid_, antibot_cmd,
        user_info_->antibot_crypt_info_.antibot_ready_encrypt_);
    if (!user_info_->antibot_crypt_info_.antibot_ready_encrypt_) {
        return 0;
    }
    SecAgentBillHandler::add_statistic_bill(0x493f7, 1); // ANTIBOT_NEED_ENCRYPT_NUM
    return encrypt(recv_frame, sdk_pkg);
}

// line 476
int SecagentAntibotMsgHandler::proc_tsssdk_ab_other_msg(uint16_t antibot_cmd,
                                                        const sec_proto::RecvDataFromSDKReq &sdk_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("recv tsssdk antibot other pkg, uin=%u, appid=%u",
                                    user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    SecAgentBillHandler::add_statistic_bill(0x493ea, 1); // RECV_ANTIBOT_OTHER_NO_PROC_NUM
    if (!is_up_cmd_list(antibot_cmd) || is_encrypt_pkg_) {
        ZEN_Trace_LogMsg::debug_debugEx(
            "proc_tsssdk_ab_other_msg, antibot, not in up cmd list, no need proc, cmd=%u, uin=%u",
            antibot_cmd, user_info_->base_info_.uin_);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
        "proc_tsssdk_ab_other_msg, antibot, in up cmd list, cmd=%u, uin=%u, read_encrypt=%d",
        antibot_cmd, user_info_->base_info_.uin_,
        user_info_->antibot_crypt_info_.antibot_ready_encrypt_);
    if (!user_info_->antibot_crypt_info_.antibot_ready_encrypt_) {
        return 0;
    }
    user_info_->antibot_error_code_num_.unecncrypt_fail_num_++;
    if (user_info_->antibot_error_code_num_.unecncrypt_fail_num_ >
        conf_->crypt_checker_conf_.antibot_error_thread_) {
        ZEN_Trace_LogMsg::debug_debugEx(
            "unencrypt fail num bigger than thread, uin=%u, unencrypt fail num=%u",
            user_info_->base_info_.uin_,
            user_info_->antibot_error_code_num_.unecncrypt_fail_num_);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex("[%s], report error to secsvr, err_id=%d",
                                    "int SecagentAntibotMsgHandler::proc_tsssdk_ab_other_msg(uint16_t, const sec_proto::RecvDataFromSDKReq&)",
                                    0x69);
    SecAgentBillHandler::add_statistic_bill(0x493fc, 1); // SEND_SECSVR_UNENCRYPT_FAIL_NUM
    SecAgentBillHandler::write_bill_antibot_nocrypt_fail(user_info_->base_info_.uin_,
                                                         user_info_->base_info_.appid_,
                                                         antibot_cmd);
    send_error_pkg_to_secsvr(sdk_pkg, 0x69);
    return 0;
}

// line 530
int SecagentAntibotMsgHandler::send_error_pkg_to_secsvr(const sec_proto::RecvDataFromSDKReq &sdk_pkg,
                                                        unsigned int error_id) {
    ZEN_Trace_LogMsg::debug_debugEx("send error pkg to secsvr, uin=%u, errorid=%d",
                                    user_info_->base_info_.uin_, error_id);
    SecAgentBillHandler::write_bill_send_secsvr_error(user_info_->base_info_.uin_,
                                                      user_info_->base_info_.appid_, error_id);
    sec_proto::ReportErrReq report_err_pkg;
    report_err_pkg.channel_id_ = sdk_pkg.channel_id_;
    report_err_pkg.room_id_ = sdk_pkg.room_id_;
    report_err_pkg.error_id_ = error_id;
    char send_buf[Comm_App_Frame::MAX_LEN_OF_APPFRAME];
    Comm_App_Frame *send_frame = (Comm_App_Frame *)send_buf;
    send_frame->init_framehead(0x10000, 0, 0x4baf7);
    send_frame->frame_uin_ = user_info_->base_info_.uin_;
    send_frame->app_id_ = user_info_->base_info_.appid_;
    send_frame->send_service_ = Comm_Svrd_Config::instance()->self_svr_id_;
    SERVICES_ID recv_info(0x1f, 0);
    SERVICES_ID proxy_info(0, 0);
    send_frame->recv_service_ = recv_info;
    send_frame->proxy_service_ = proxy_info;
    send_frame->transaction_id_ = 0;
    send_frame->backfill_trans_id_ = 0;
    int ret = send_frame->appdata_encode<sec_proto::ReportErrReq>(0xffbd, report_err_pkg, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]TDR encode fail.ret =%d,Please check your code and buffer len.",
            "int SecagentAntibotMsgHandler::send_error_pkg_to_secsvr(const sec_proto::RecvDataFromSDKReq&, unsigned int)",
            ret);
        return ret;
    }
    unsigned int game_id = CfgSvrSdk::instance()->get_game_id();
    Comm_Stat_Monitor::instance()->increase_by_statid(0x49404, game_id, error_id, 1);
    Zerg_MMAP_BusPipe *bus = Zerg_MMAP_BusPipe::instance();
    if (send_frame->frame_length_ - Comm_App_Frame::LEN_OF_APPFRAME_HEAD > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            send_frame->frame_length_);
        return 0x13b1;
    }
    if (Comm_Svrd_Config::instance()->is_monitor_uin(send_frame->frame_uin_)) {
        send_frame->frame_option_ |= 0x10000;
    }
    if (send_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(send_frame, "[TRACK MONITOR][Send]", RS_ERROR);
    }
    if (!bus->push_back_sendpipe(send_frame)) {
        size_t freesize = 0;
        size_t size = 0;
        bus->get_bus_freesize(1, freesize, size);
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
            1, send_frame->frame_length_, freesize);
        bus->monitor_->increase_by_statid(0x238f, send_frame->app_id_,
                                          send_frame->frame_command_, 1);
        return 0x1396;
    }
    bus->monitor_->increase_by_statid(0x238e, send_frame->app_id_,
                                      send_frame->frame_command_, 1);
    bus->monitor_->increase_by_statid(0x239c, send_frame->app_id_,
                                      send_frame->frame_command_,
                                      send_frame->frame_length_);
    return 0;
}

// line 585
int SecagentAntibotMsgHandler::proc_ab_rpcode_pkg(sec_proto::RecvDataFromSDKReq *sdk_pkg) {
    sec_proto::AntibotClientPkg rpcode_pkg;
    int ret = rpcode_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg->data_), sdk_pkg->data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_ab_rpcode_pkg(sec_proto::RecvDataFromSDKReq*)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    SecAgentBillHandler::add_statistic_bill(0x493f4, 1); // RECV_ANTIBOT_RPCODE_PKG_NUM
    ZEN_Trace_LogMsg::debug_debugEx("rpcode decrypt with local key, uin=%u, dib_id=%u",
                                    user_info_->base_info_.uin_,
                                    user_info_->rpcode_crypt_info_.dib_id_);
    ret = AntibotCrypter::decrypt(user_info_->rpcode_crypt_info_.dib_id_,
                                  user_info_->rpcode_crypt_info_.key_,
                                  rpcode_pkg.body_.sc_pkg_code_.rpcode_data_ + 2,
                                  rpcode_pkg.body_.sc_pkg_code_.rp_id_);
    if (user_info_->antibot_crypt_info_.antibot_ready_encrypt_) {
        unsigned int hash_val = rpcode_hash(rpcode_pkg.body_.sc_pkg_code_.rpcode_data_ + 2,
                                            rpcode_pkg.body_.sc_pkg_code_.rp_id_);
        if (hash_val != secagent_read32(&rpcode_pkg.body_.sc_pkg_code_.rpcode_data_size_)) {
            ZEN_Trace_LogMsg::debug_errorex(
                "rpcode decrypt fail, hash not right, uin=%u, local_hash=%u, cli_hash=%u",
                user_info_->base_info_.uin_, hash_val,
                secagent_read32(&rpcode_pkg.body_.sc_pkg_code_.rpcode_data_size_));
            SecAgentBillHandler::add_statistic_bill(0x49400, 1); // RPCODE_DECRYPT_FAIL_COUNT
            return SECAGENT_ANTIBOT_RPCODE_KEY_NOT_READY;
        }
    }
    size_t use_len = 0;
    ret = rpcode_pkg.pack(reinterpret_cast<char *>(sdk_pkg->data_), 0x7fff, &use_len, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], rpcode pkg pack fail, rc=%d",
                                        "int SecagentAntibotMsgHandler::proc_ab_rpcode_pkg(sec_proto::RecvDataFromSDKReq*)",
                                        ret);
        return ret;
    }
    sdk_pkg->data_len_ = (uint16_t)use_len;
    ZEN_Trace_LogMsg::debug_infoex("rpcode pkg decrypt succ, uin=%u, dib_id=%u",
                                   user_info_->base_info_.uin_,
                                   user_info_->rpcode_crypt_info_.dib_id_);
    return 0;
}

// line 708
int SecagentAntibotMsgHandler::proc_ab_init_pkg(const sec_proto::RecvDataFromSDKReq &sdk_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("recv antibot init pkg, uin=%u, appid=%u",
                                    user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    SecAgentBillHandler::add_statistic_bill(0x493eb, 1); // RECV_ANTIBOT_INIT_PKG_NUM
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg.data_), sdk_pkg.data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_ab_init_pkg(const sec_proto::RecvDataFromSDKReq&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    user_info_->antibot_crypt_info_.client_ver_ = antibot_pkg.body_.cs_init_info_.client_ver_;
    const SecagentGameExe *game_exe =
        SecAgentApp::instance()->get_game_exe(user_info_->antibot_crypt_info_.client_ver_);
    if (game_exe == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("get game exe fail, uin=%u, client version=%u",
                                        user_info_->base_info_.uin_,
                                        user_info_->antibot_crypt_info_.client_ver_);
        send_error_pkg_to_secsvr(sdk_pkg, 5);
        return SECAGENT_ANTIBOT_GAME_EXE_FAIL;
    }
    ZEN_Trace_LogMsg::debug_debugEx("get game exe succ, uin=%u, client version=%u",
                                    user_info_->base_info_.uin_,
                                    user_info_->antibot_crypt_info_.client_ver_);
    ZEN_Trace_LogMsg::debug_debugEx("proc cs sdk pkg, antibot init pkg, need update decypt info, cmd=%u, uin=%u, client_ver=%u",
                                    antibot_pkg.head_.cmd_id_, user_info_->base_info_.uin_,
                                    user_info_->antibot_crypt_info_.client_ver_);
    if (antibot_pkg.body_.cs_init_info_.dib_id_ == 0) {
        ZEN_Trace_LogMsg::debug_infoex("init dib_id equal 0, no need update, uin=%u",
                                       user_info_->base_info_.uin_);
        return 0;
    }
    user_info_->antibot_crypt_info_.dib_id_ = antibot_pkg.body_.cs_init_info_.dib_id_;
    SecAgentBillHandler::add_statistic_bill(0x493ec, 1); // UPDATE_DIB_FROM_INIT_PKG_SUCC_NUM
    user_info_->antibot_crypt_info_.key_len_ = antibot_pkg.body_.cs_init_info_.key_len_;
    user_info_->antibot_crypt_info_.key_offset_ = antibot_pkg.body_.cs_init_info_.key_offset_;
    memcpy(user_info_->antibot_crypt_info_.key_data_,
           game_exe->exe_data_ + user_info_->antibot_crypt_info_.key_offset_,
           user_info_->antibot_crypt_info_.key_len_);
    g_bill << ConstDataSource(0x1e) << UserID(user_info_->base_info_.uin_)
           << LogType(3) << GameID(user_info_->base_info_.appid_)
           << LogComment("antibot init update func_id from sdk")
           << (int64_t)antibot_pkg.head_.cmd_id_
           << (int64_t)antibot_pkg.body_.cs_init_info_.dib_id_
           << (int64_t)antibot_pkg.body_.cs_init_info_.key_len_
           << (int64_t)antibot_pkg.body_.cs_init_info_.key_offset_
           << (int64_t)antibot_pkg.body_.cs_init_info_.client_ver_ << endl;
    SecAgentBillHandler::add_statistic_bill(0x493ed, 1); // UPDATE_KEY_FROM_INIT_PKG_SUCC_NUM
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::proc_antibot_init_msg, update_user_key_info succ, keylen=%d, key_offset=%d, dib_id=%u, uin=%u",
        user_info_->antibot_crypt_info_.key_len_, user_info_->antibot_crypt_info_.key_offset_,
        user_info_->antibot_crypt_info_.dib_id_, user_info_->base_info_.uin_);
    return 0;
}

// line 798
int SecagentAntibotMsgHandler::send_cmd_list(SecAgentUserInfo *user_info) {
    if (user_info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], user_info is null",
                                        "int SecagentAntibotMsgHandler::send_cmd_list(SecAgentUserInfo*)");
        return SECAGENT_ANTIBOT_READY_FAIL;
    }
    user_info_ = user_info;
    sec_proto::AntibotClientPkg send_msg;
    send_msg.head_.cmd_id_ = 0x1c; // TAB_SC_DE_CMDID_LIST
    send_msg.head_.seq_ = 0;
    conf_ = SecAgentApp::instance()->get_config();
    send_msg.body_.sc_de_cmd_id_list_.cmd_count_ =
        (uint8_t)conf_->crypt_checker_conf_.up_cmdlist_info_.cmd_list_count_;
    memcpy(send_msg.body_.sc_de_cmd_id_list_.cmd_,
           conf_->crypt_checker_conf_.up_cmdlist_info_.cmd_,
           send_msg.body_.sc_de_cmd_id_list_.cmd_count_ * sizeof(uint16_t));
    SecAgentBillHandler::write_bill_antibot_cmd_list(user_info_->base_info_.uin_,
                                                     user_info_->base_info_.appid_,
                                                     send_msg.body_.sc_de_cmd_id_list_.cmd_count_);
    return send_antibot_msg(send_msg, user_info_->base_info_.channel_id_, 0);
}

// line 828
int SecagentAntibotMsgHandler::send_cmd_dib_key(SecAgentUserInfo *user_info) {
    if (user_info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], user_info is null",
                                        "int SecagentAntibotMsgHandler::send_cmd_dib_key(SecAgentUserInfo*)");
        return SECAGENT_ANTIBOT_READY_FAIL;
    }
    user_info_ = user_info;
    const SecagentGameExe *game_exe =
        SecAgentApp::instance()->get_game_exe(user_info_->antibot_crypt_info_.client_ver_);
    if (game_exe == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("get newest game exe fail, uin=%u, client_ver=%u",
                                        user_info_->base_info_.uin_,
                                        user_info_->antibot_crypt_info_.client_ver_);
        return SECAGENT_ANTIBOT_GAME_EXE_FAIL;
    }
    unsigned int exe_data_size = game_exe->exe_data_size_;
    const unsigned char *exe_data = game_exe->exe_data_;
    const DibData *cdib_data = SecAgentApp::instance()->get_dib_data();
    int dib_index = get_crypt_id();
    const DibItem *dib_item = &cdib_data->dib_item_array_[dib_index];
    user_info_->antibot_crypt_info_.dib_id_ = dib_item->type_;
    SecAgentBillHandler::add_statistic_bill(0x493ec, 1); // UPDATE_DIB_FROM_INIT_PKG_SUCC_NUM
    sec_proto::AntibotClientPkg send_msg;
    send_msg.head_.cmd_id_ = 0x1d; // TAB_SC_DEKEY
    send_msg.head_.seq_ = 0;
    send_msg.body_.sc_de_key_.pad_data_size_ = (uint8_t)dib_item->type_;
    if (dib_item->type_ == 1) {
        send_msg.body_.sc_de_key_.pad_data_[0] = 0;
        unsigned int rand_start = (rand() % 0x400) + 0x400;
        *(uint16_t *)&send_msg.body_.sc_de_key_.pad_data_[1] = (uint16_t)rand_start;
        memcpy(&send_msg.body_.sc_de_key_.pad_data_[3],
               exe_data + rand() % (exe_data_size - 0x800), rand_start);
    } else {
        send_msg.body_.sc_de_key_.pad_data_[0] = 1;
        *(uint16_t *)&send_msg.body_.sc_de_key_.pad_data_[1] =
            (uint16_t)dib_item->dib_data_len_;
        if (dib_item->dib_data_len_ > 0x7fd7) {
            ZEN_Trace_LogMsg::debug_errorex("SendCmdDibKey: bad dibdatalen=%d dibid=%d",
                                            dib_item->dib_data_len_, dib_item->type_);
            return SECAGENT_ANTIBOT_RPCODE_KEY_NOT_READY;
        }
        memcpy(&send_msg.body_.sc_de_key_.pad_data_[3], dib_item->dib_data_ + 4,
               dib_item->dib_data_len_);
    }
    send_antibot_msg(send_msg, user_info_->base_info_.channel_id_, 0);
    // 第二条：cmd 0x1e（TAB_SC_DEKEY 复用）
    send_msg.head_.cmd_id_ = 0x1e;
    send_msg.head_.seq_ = 0;
    send_msg.body_.sc_de_key_.pad_data_size_ = rand() & 0x7f;
    memcpy(send_msg.body_.sc_de_key_.pad_data_,
           exe_data + rand() % (exe_data_size - 0x800),
           send_msg.body_.sc_de_key_.pad_data_size_);
    send_msg.body_.sc_de_key_.key_len_ = (uint8_t)dib_item->key_len_;
    unsigned int rang_index = rand() % cdib_data->key_info_count_;
    const DibKeyInfo *key_range_item = &cdib_data->key_info_array_[rang_index];
    int offset = rand() % (key_range_item->len_ - user_info_->antibot_crypt_info_.key_offset_);
    offset += key_range_item->vstart_;
    user_info_->antibot_crypt_info_.key_offset_ = offset;
    if (user_info_->antibot_crypt_info_.key_len_ > 0x80) {
        ZEN_Trace_LogMsg::debug_errorex("SendCmdDibKey: bad keylen=%d dibid=%d",
                                        user_info_->antibot_crypt_info_.key_len_,
                                        dib_item->type_);
        return SECAGENT_ANTIBOT_RPCODE_KEY_LEN_ERR;
    }
    memcpy(user_info_->antibot_crypt_info_.key_data_,
           exe_data + key_range_item->fstart_ + offset,
           user_info_->antibot_crypt_info_.key_len_);
    SecAgentBillHandler::add_statistic_bill(0x493ed, 1); // UPDATE_KEY_FROM_INIT_PKG_SUCC_NUM
    send_msg.body_.sc_de_key_.key_crc_ =
        Crc32((unsigned char *)user_info_->antibot_crypt_info_.key_data_,
              user_info_->antibot_crypt_info_.key_len_);
    send_antibot_msg(send_msg, user_info_->base_info_.channel_id_, 0);
    send_cmd_list(user_info);
    SecAgentBillHandler::write_bill_antibot_crypt_info(
        user_info_->base_info_.uin_, user_info_->base_info_.appid_,
        user_info_->antibot_crypt_info_.dib_id_, user_info_->antibot_crypt_info_.key_len_,
        user_info_->antibot_crypt_info_.key_offset_, send_msg.body_.sc_de_key_.key_crc_);
    return 0;
}

// line 952
int SecagentAntibotMsgHandler::send_antibot_msg(const sec_proto::AntibotClientPkg &antibot_pkg,
                                                unsigned int channel_id, unsigned int room_id) {
    char buffer[MAX_CHECK_DATA_LEN];
    unsigned int buf_len = MAX_CHECK_DATA_LEN;
    size_t use_len = 0;
    int ret = antibot_pkg.pack(buffer, MAX_CHECK_DATA_LEN, &use_len, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("encode antibot msg fail.ret=%d, antibot cmd=%d, uin=%u",
                                        ret, antibot_pkg.head_.cmd_id_,
                                        user_info_->base_info_.uin_);
        return ret;
    }
    buf_len = use_len;
    unsigned int max_package_size = conf_->max_package_size_;
    if (max_package_size == 0 || max_package_size >= buf_len) {
        return send_buff_to_client(buffer, buf_len, channel_id, room_id);
    }
    char piece_data[0xc800];
    size_t piece_used_len = 0;
    int piece_size = 0;
    char *temp_ptr = piece_data;
    char *temp_piece_ptr = piece_data;
    sec_proto::AntibotPkgHead head;
    head.cmd_id_ = antibot_pkg.head_.cmd_id_;
    head.seq_ = 0;
    piece_size = head.pack(piece_data, MAX_PKG_LEN / 2, &use_len, 0);
    if (piece_size != 0) {
        ZEN_Trace_LogMsg::debug_errorex("pack antibotpkghead fail, |uin=%d|game_id=%d|room_id=%d|ret=%d",
                                        user_info_->base_info_.uin_,
                                        user_info_->base_info_.appid_, room_id, piece_size);
        return piece_size;
    }
    piece_used_len = use_len;
    temp_ptr = piece_data + piece_used_len;
    uint16_t piece_count = (uint16_t)(buf_len / max_package_size +
                                      ((buf_len % max_package_size) ? 1 : 0));
    piece_size = AB_NS_CODEENGINE::EncodeShort(&temp_ptr, (uint16_t)buf_len);
    ZEN_Trace_LogMsg::debug_infoex(
        "antibot pkg too big, will send piece data,             pkg_size=%d, max_size=%d, piece_count=%u, cmd=%d, uin=%u",
        buf_len, max_package_size, piece_count, antibot_pkg.head_.cmd_id_,
        user_info_->base_info_.uin_);
    piece_used_len += AB_NS_CODEENGINE::EncodeShort(&temp_ptr, piece_count);
    uint32_t crc32_code = Crc32((unsigned char *)buffer, buf_len);
    if (piece_count == 0) {
        return send_buff_to_client(buffer, buf_len, channel_id, room_id);
    }
    for (uint16_t i = 0; i < piece_count; ++i) {
        temp_piece_ptr = temp_ptr;
        int temp_piece_size = AB_NS_CODEENGINE::EncodeShort(&temp_piece_ptr, (uint16_t)(i + 1));
        temp_piece_size += AB_NS_CODEENGINE::EncodeInt(&temp_piece_ptr, crc32_code);
        if (max_package_size < buf_len) {
            temp_piece_size +=
                AB_NS_CODEENGINE::EncodeShort(&temp_piece_ptr, (uint16_t)max_package_size);
            temp_piece_size += AB_NS_CODEENGINE::EncodeMem(
                &temp_piece_ptr, buffer + i * max_package_size, (short)max_package_size);
            buf_len -= max_package_size;
        } else {
            temp_piece_size +=
                AB_NS_CODEENGINE::EncodeShort(&temp_piece_ptr, (uint16_t)buf_len);
            temp_piece_size += AB_NS_CODEENGINE::EncodeMem(&temp_piece_ptr,
                                                           buffer + i * max_package_size,
                                                           (short)buf_len);
        }
        piece_size = piece_used_len + piece_size + temp_piece_size;
        ret = send_buff_to_client(piece_data, piece_size, channel_id, room_id);
        ZEN_Trace_LogMsg::debug_debugEx(
            "send piece msg fail|game_id=%d|room_id=%d|uin=%d|piece_count=%d|buf_len=%d",
            user_info_->base_info_.appid_, room_id, user_info_->base_info_.uin_,
            max_package_size, buf_len);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "send piece msg fail|game_id=%d|room_id=%d|uin=%d|piece_count=%d|buf_len=%d",
                user_info_->base_info_.appid_, room_id, user_info_->base_info_.uin_,
                max_package_size, buf_len);
            return ret;
        }
    }
    return send_buff_to_client(buffer, buf_len, channel_id, room_id);
}

// line 1072
int SecagentAntibotMsgHandler::send_buff_to_client(const char *buffer, size_t buf_len,
                                                   unsigned int channel_id,
                                                   unsigned int room_id) {
    if (buffer == NULL || buf_len == 0) {
        ZEN_Trace_LogMsg::debug_errorex("SecagentAntibotMsgHandler invalid param|buf_len=%d",
                                        buf_len);
        return SECAGENT_ANTIBOT_READY_FAIL;
    }
    sec_proto::SendToSDK send_to_sdk;
    send_to_sdk.channel_id_ = (uint16_t)channel_id;
    send_to_sdk.room_id_ = (uint16_t)room_id;
    send_to_sdk.time_stamp_ = (uint32_t)time(NULL);
    send_to_sdk.to_busi_id_ = 1;
    send_to_sdk.from_busi_id_ = 1;
    send_to_sdk.data_len_ = (uint16_t)buf_len;
    if (buf_len > 0x7fff) {
        ZEN_Trace_LogMsg::debug_errorex("SecagentAntibotMsgHandler invalid param|buf_len=%d",
                                        buf_len);
        return SECAGENT_ANTIBOT_READY_FAIL;
    }
    memcpy(send_to_sdk.data_, buffer, buf_len);
    ZEN_Trace_LogMsg::debug_debugEx(
        "SecagentAntibotMsgHandler::send_buff_to_client: uin=%d game_id=%d room_id=%d msg_len=%d",
        user_info_->base_info_.uin_, user_info_->base_info_.appid_, room_id, buf_len);
    return SecAgentApp::instance()->get_gamesvr_channel().send<sec_proto::SendToSDK>(
        user_info_->base_info_.uin_, 0x493e1, user_info_->base_info_.appid_, channel_id,
        send_to_sdk);
}

// line 1106
void get_quickly_func(unsigned int *idx, const DibData *cdib_data) {
    static const unsigned int s_quickly_tencrypt_ids[] = { 8, 19, 23, 24 };
    static unsigned int i = 0;
    static unsigned int dib_count = cdib_data->dib_item_count_;
    static unsigned int quickly_idx = rand() & 3;
    i = 0;
    if (dib_count == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotCryptChecker::get_crypt_id, get quickly func fail.");
        return;
    }
    if (cdib_data->dib_item_array_[0].type_ == s_quickly_tencrypt_ids[quickly_idx]) {
        *idx = 0;
    } else {
        for (i = 1; i < dib_count; ++i) {
            if (cdib_data->dib_item_array_[i].type_ == s_quickly_tencrypt_ids[quickly_idx]) {
                *idx = i;
                return;
            }
        }
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotCryptChecker::get_crypt_id, get quickly func fail.");
    }
}

// line 1140
int SecagentAntibotMsgHandler::get_crypt_id() const {
    unsigned int idx = 0;
    get_quickly_func(&idx, SecAgentApp::instance()->get_dib_data());
    return idx;
}

// line 1153
int SecagentAntibotMsgHandler::proc_ab_de_testpkg(const sec_proto::RecvDataFromSDKReq &sdk_pkg) {
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg.data_), sdk_pkg.data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_ab_de_testpkg(const sec_proto::RecvDataFromSDKReq&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    SecAgentBillHandler::add_statistic_bill(0x493f2, 1); // RECV_ANTIBOT_TEST_PKG_NUM
    unsigned int tmp_crc = Crc32(antibot_pkg.body_.cs_de_test_pkg_.content_,
                                 antibot_pkg.body_.cs_de_test_pkg_.content_size_);
    if (tmp_crc != antibot_pkg.body_.cs_de_test_pkg_.test_pkg_crc_) {
        user_info_->antibot_error_code_num_.test_pkg_fail_num_++;
        if (user_info_->antibot_error_code_num_.test_pkg_fail_num_ >
            conf_->crypt_checker_conf_.antibot_error_thread_) {
            ZEN_Trace_LogMsg::debug_debugEx(
                "test pkg fail num bigger than thread, uin=%u, test pkg fail num=%u",
                user_info_->base_info_.uin_,
                user_info_->antibot_error_code_num_.test_pkg_fail_num_);
            return 0;
        }
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], antibot test pkg crc error, uin=%u, crc=%x, tmpcrc=%x",
            "int SecagentAntibotMsgHandler::proc_ab_de_testpkg(const sec_proto::RecvDataFromSDKReq&)",
            user_info_->base_info_.uin_, antibot_pkg.body_.cs_de_test_pkg_.test_pkg_crc_,
            tmp_crc);
        SecAgentBillHandler::write_bill_antibot_test_decypt_fail(
            user_info_->base_info_.uin_, user_info_->base_info_.appid_,
            antibot_pkg.body_.cs_de_test_pkg_.test_pkg_crc_, tmp_crc,
            secagent_read32(&antibot_pkg.body_.cs_de_test_pkg_.content_[0]),
            secagent_read32(&antibot_pkg.body_.cs_de_test_pkg_.content_[16]));
        send_error_pkg_to_secsvr(sdk_pkg, 0x7d3);
        SecAgentBillHandler::add_statistic_bill(0x493fd, 1); // SEND_SECSVR_DECRYPT_TEST_PKG_FAIL_NUM
        return 0;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "SecagentAntibotMsgHandler::proc_ab_de_testpkg,antibot test pkg succ, uin=%u, appid=%u",
        user_info_->base_info_.uin_, user_info_->base_info_.appid_);
    return 0;
}

// line 1214
int SecagentAntibotMsgHandler::on_check_timer(time_t now, SecAgentUserInfo *user_info) {
    if (user_info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], user_info is null",
                                        "int SecagentAntibotMsgHandler::on_check_timer(time_t, SecAgentUserInfo*)");
        return SECAGENT_ANTIBOT_READY_FAIL;
    }
    user_info_ = user_info;
    conf_ = SecAgentApp::instance()->get_config();
    if (user_info_->antibot_crypt_info_.antibot_ready_encrypt_ &&
        now - user_info_->antibot_crypt_info_.test_pkg_time_ >
            conf_->crypt_checker_conf_.crypt_test_gap_) {
        send_testpkg();
        user_info_->antibot_crypt_info_.test_pkg_time_ = now;
    }
    return 0;
}

// line 1239
int SecagentAntibotMsgHandler::send_testpkg() {
    const SecagentGameExe *game_exe =
        SecAgentApp::instance()->get_game_exe(user_info_->antibot_crypt_info_.client_ver_);
    if (game_exe == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("get game exe fail, uin=%u, client_ver=%u",
                                        user_info_->base_info_.uin_,
                                        user_info_->antibot_crypt_info_.client_ver_);
        return SECAGENT_ANTIBOT_GAME_EXE_FAIL;
    }
    unsigned int exe_data_size = game_exe->exe_data_size_;
    sec_proto::AntibotClientPkg send_msg;
    send_msg.head_.cmd_id_ = 0x21; // TAB_SC_TESTPKG
    send_msg.head_.seq_ = 0;
    send_msg.body_.sc_test_pkg_.content_size_ = rand() & 0x3f;
    int rand_start = rand() % (exe_data_size - 0x800);
    memcpy(send_msg.body_.sc_test_pkg_.content_, game_exe->exe_data_ + 8 + rand_start,
           send_msg.body_.sc_test_pkg_.content_size_);
    send_msg.body_.sc_test_pkg_.test_pkg_crc_ =
        Crc32(send_msg.body_.sc_test_pkg_.content_, send_msg.body_.sc_test_pkg_.content_size_);
    send_antibot_msg(send_msg, user_info_->base_info_.channel_id_, 0);
    return 0;
}

// line 1270
int SecagentAntibotMsgHandler::proc_ab_de_log(const sec_proto::RecvDataFromSDKReq &sdk_pkg) {
    SecAgentBillHandler::add_statistic_bill(0x493f3, 1); // RECV_ANTIBOT_CLIENT_ERROR_LOG_NUM
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg.data_), sdk_pkg.data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_ab_de_log(const sec_proto::RecvDataFromSDKReq&)",
                                        ret, user_info_->base_info_.uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    user_info_->antibot_error_code_num_.de_log_fail_num_++;
    if (user_info_->antibot_error_code_num_.de_log_fail_num_ >
        conf_->crypt_checker_conf_.antibot_error_thread_) {
        ZEN_Trace_LogMsg::debug_debugEx("delog pkg fail num bigger than thread, uin=%u, delog pkg fail num=%u",
                                        user_info_->base_info_.uin_,
                                        user_info_->antibot_error_code_num_.de_log_fail_num_);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex(
        "[%s], recv client antibot error log, uin=%d, errorid=%u, errormsg=%u",
        "int SecagentAntibotMsgHandler::proc_ab_de_log(const sec_proto::RecvDataFromSDKReq&)",
        user_info_->base_info_.uin_, antibot_pkg.body_.cs_de_log_.error_id_,
        antibot_pkg.body_.cs_de_log_.error_);
    SecAgentBillHandler::write_bill_antibot_de_log(
        user_info_->base_info_.uin_, user_info_->base_info_.appid_,
        antibot_pkg.body_.cs_de_log_.error_id_, antibot_pkg.body_.cs_de_log_.error_,
        antibot_pkg.body_.cs_de_log_.pad_size_);
    send_error_pkg_to_secsvr(sdk_pkg, 0x65);
    SecAgentBillHandler::add_statistic_bill(0x493fe, 1); // SEND_SECSVR_CHECK_DELOG_FAIL_NUM
    return 0;
}

// line 1320
int SecagentAntibotMsgHandler::proc_tenparty_data_msg(const AntibotSanlixData *sanlix_data,
                                                      sec_proto::AntibotClientPkg *antibot_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("will proc sanlix tenparty data msg, cmd=%u",
                                    antibot_pkg->head_.cmd_id_);
    const AntibotSanlixDataInfo &info = sanlix_data->get_tenpartydata();
    if (info.data_len_ > 0x7fd6 || info.data_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], sanlix tenparty data too big, len=%u, size=%u",
                                        "int SecagentAntibotMsgHandler::proc_tenparty_data_msg(const AntibotSanlixData*, sec_proto::AntibotClientPkg*)",
                                        info.data_len_, 2);
        return SECAGENT_ANTIBOT_SANLIX_DATA_FAIL;
    }
    antibot_pkg->body_.sc_tenparty_data_.tenpary_data_len_ = (uint16_t)info.data_len_;
    memcpy(antibot_pkg->body_.sc_tenparty_data_.tenpary_data_, info.data_, info.data_len_);
    return 0;
}

// line 1345
int SecagentAntibotMsgHandler::proc_scheme_data_msg(const AntibotSanlixData *sanlix_data,
                                                    sec_proto::AntibotClientPkg *antibot_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("will proc sanlix scheme data msg, cmd=%u",
                                    antibot_pkg->head_.cmd_id_);
    const AntibotSanlixDataInfo &info = sanlix_data->get_tkdata();
    if (info.data_len_ > 0x7fd6 || info.data_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], sanlix key data too big, len=%u, size=%u",
                                        "int SecagentAntibotMsgHandler::proc_scheme_data_msg(const AntibotSanlixData*, sec_proto::AntibotClientPkg*)",
                                        info.data_len_, 0x7fd7);
        return SECAGENT_ANTIBOT_SANLIX_DATA_FAIL;
    }
    antibot_pkg->body_.sc_scheme_data_.scheme_data_len_ = (uint16_t)info.data_len_;
    memcpy(&antibot_pkg->body_.sc_scheme_data_.scheme_data_, info.data_, info.data_len_);
    return 0;
}

// line 1372
int SecagentAntibotMsgHandler::proc_slx_share_info_msg_te(const AntibotSanlixData *sanlix_data,
                                                          sec_proto::AntibotClientPkg *antibot_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("will proc sanlix te data msg, cmd=%u",
                                    antibot_pkg->head_.cmd_id_);
    const AntibotSanlixDataInfo &info = sanlix_data->get_tedata();
    if (info.data_len_ > 0x7fd6 || info.data_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], sanlix te data too big, len=%u, size=%u",
                                        "int SecagentAntibotMsgHandler::proc_slx_share_info_msg_te(const AntibotSanlixData*, sec_proto::AntibotClientPkg*)",
                                        info.data_len_, 0x7fd7);
        return SECAGENT_ANTIBOT_SANLIX_DATA_FAIL;
    }
    antibot_pkg->body_.sc_slx_te_share_info_.slx_data_len_ = (uint16_t)info.data_len_;
    memcpy(antibot_pkg->body_.sc_slx_te_share_info_.slx_data_, info.data_, info.data_len_);
    return 0;
}

// line 1398
int SecagentAntibotMsgHandler::proc_sanlix_data_msg(uint16_t antibot_cmd,
                                                    Comm_App_Frame *recv_frame,
                                                    sec_proto::SendToSDK *sdk_pkg) {
    conf_ = SecAgentApp::instance()->get_config();
    if (conf_->sanlix_checker_conf_.is_use_ == 0) {
        ZEN_Trace_LogMsg::debug_debugEx("no need proc sanlix msg, uin=%u, cmd=%d",
                                        recv_frame->frame_uin_, antibot_cmd);
        return 0;
    }
    if (antibot_cmd == 0x33) {
        uint16_t scheme_id = secagent_bswap16(*(const uint16_t *)(sdk_pkg->data_ + 6));
        uint16_t sub_proto_id = secagent_bswap16(*(const uint16_t *)(sdk_pkg->data_ + 8));
        if (scheme_id != 5 || sub_proto_id != 0x192) {
            ZEN_Trace_LogMsg::debug_debugEx("no need proc scheme data, id=%d", scheme_id);
            return 0;
        }
    }
    sec_proto::AntibotClientPkg antibot_pkg;
    int ret = antibot_pkg.unpack(reinterpret_cast<const char *>(sdk_pkg->data_), sdk_pkg->data_len_, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack antibot pkg fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_sanlix_data_msg(uint16_t, Comm_App_Frame*, sec_proto::SendToSDK*)",
                                        ret, recv_frame->frame_uin_);
        return SECAGENT_ANTIBOT_UNPACK_PKG_FAIL;
    }
    ZEN_Trace_LogMsg::debug_debugEx("will proc sanlix data msg, cmd=%u, uin=%u",
                                    antibot_pkg.head_.cmd_id_, recv_frame->frame_uin_);
    const AntibotSanlixData *sanlix_data = SecAgentApp::instance()->get_sanlix_data();
    if (sanlix_data == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], get sanlix data fail",
                                        "int SecagentAntibotMsgHandler::proc_sanlix_data_msg(uint16_t, Comm_App_Frame*, sec_proto::SendToSDK*)");
        return SECAGENT_ANTIBOT_GET_SANLIX_FAIL;
    }
    switch (antibot_cmd) {
    case 0x32:
        ret = proc_slx_share_info_msg_ts(sanlix_data, &antibot_pkg);
        break;
    case 0x15:
        ret = proc_tenparty_data_msg(sanlix_data, &antibot_pkg);
        break;
    case 0x33:
        ret = proc_scheme_data_msg(sanlix_data, &antibot_pkg);
        break;
    case 0x35:
        ret = proc_slx_share_info_msg_te(sanlix_data, &antibot_pkg);
        break;
    default:
        ZEN_Trace_LogMsg::debug_debugEx("unknown cmd=%d, uin=%u", antibot_cmd,
                                        recv_frame->frame_uin_);
        ret = 0;
        break;
    }
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("proc sanlix msg fail, ret=%d, cmd=%u, uin=%u",
                                        ret, antibot_cmd, recv_frame->frame_uin_);
        return ret;
    }
    ret = pack_secsvr_antibot_to_frame(recv_frame, sdk_pkg, antibot_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], pack antibot fail, ret=%d, uin=%u",
                                        "int SecagentAntibotMsgHandler::proc_sanlix_data_msg(uint16_t, Comm_App_Frame*, sec_proto::SendToSDK*)",
                                        ret, recv_frame->frame_uin_);
    }
    return ret;
}

// line 1494
int SecagentAntibotMsgHandler::proc_slx_share_info_msg_ts(const AntibotSanlixData *sanlix_data,
                                                          sec_proto::AntibotClientPkg *antibot_pkg) {
    ZEN_Trace_LogMsg::debug_debugEx("will proc sanlix ts data msg, cmd=%u",
                                    antibot_pkg->head_.cmd_id_);
    const AntibotSanlixDataInfo &info = sanlix_data->get_tsdata();
    if (info.data_len_ > 0x7fd7 || info.data_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], sanlix ts data too big, len=%u, size=%u",
                                        "int SecagentAntibotMsgHandler::proc_slx_share_info_msg_ts(const AntibotSanlixData*, sec_proto::AntibotClientPkg*)",
                                        info.data_len_, 0x7fd7);
        return SECAGENT_ANTIBOT_SANLIX_DATA_FAIL;
    }
    antibot_pkg->body_.sc_slx_share_info_.slx_data_len_ = (uint16_t)info.data_len_;
    memcpy(antibot_pkg->body_.sc_slx_share_info_.slx_data_, info.data_, info.data_len_);
    return 0;
}

// line 1520
unsigned int SecagentAntibotMsgHandler::rpcode_hash(const unsigned char *offset,
                                                    unsigned int len) {
    unsigned int tmp_hash = 0;
    const unsigned char *ptmp = offset;
    for (unsigned int i = 0; i < len; ++i) {
        if ((i & 1) == 0) {
            tmp_hash ^= ptmp[i] ^ ((tmp_hash << 7) ^ (tmp_hash >> 3));
        } else {
            tmp_hash ^= ~(ptmp[i] ^ ((tmp_hash << 11) ^ (tmp_hash >> 5)));
        }
    }
    return tmp_hash & 0x7fffffff;
}
