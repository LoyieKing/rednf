// Reconstructed from secagent disassembly + DWARF (2026-08-10)
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_msg_handler.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体按二进制反汇编逐条还原；错误码/统计 ID 与助记符对照。

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
#include <arpa/inet.h>
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
// zen_shm_hash_expire.h 已定义 ZEN_LIB::_shm_memory_base，但未设置
// zen_shm_lockfree_deque.h 依赖的守卫宏；此处先补定义，避免重复定义。
#ifndef ZEN_SHM_PREDEFINE_H_SHM_MEMORY_BASE_DEFINED_
#define ZEN_SHM_PREDEFINE_H_SHM_MEMORY_BASE_DEFINED_
#endif
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
#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/formmog/secagent/secagent_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/formmog/secagent/secagent_restrict.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_static_config_dec.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include "src/formmog/secagent/secagent_user_info.h"
#include "src/formmog/secagent/secagent_bill.h"
#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/formmog/secagent/secagent_app.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_game_exe.h"

// 命令常量（原形位于 formmog_proto_public_cmd.h，该头尚未还原；数值按二进制立即数还原）
namespace {
const unsigned int SECAGENT_CMD_CLIENT_DEL_USER = 0x4baf3;      // SDK 通知删除用户
const unsigned int SECAGENT_CMD_CLIENT_ADD_USER = 0x4baf1;      // SDK 通知添加用户
const unsigned int SECAGENT_CMD_CLIENT_CS_SDK_PKG = 0x4c6ab;    // 客户端->secsvr SDK 数据
const unsigned int SECAGENT_CMD_SVR_SEND_TO_SDK = 0x493e1;      // secsvr->客户端 SDK 数据
const unsigned int SECAGENT_CMD_SVR_RPCODE_KEYOFFSET_RSP = 0x4ca98; // rpcode keyoffset 应答
const unsigned int SECAGENT_CMD_SVR_NOTIFY_IS_CHANGE_SVR = 0x4ca9a; // 通知切换 secsvr
const unsigned int SECAGENT_CMD_HEART_BEAT = 0x4baf9;           // 心跳
const unsigned int SECAGENT_CMD_CONFIRM_RPCODE = 0x4bb00;       // 上报 rpcode 确认
const unsigned int SECAGENT_CMD_PUNISH_USER = 0x493e3;          // 封禁玩家（gamesvr 通道）
const unsigned short SECSVR_SERVICE_TYPE = 0x1f;                // secsvr 服务类型
}

// 头文件模板显式特化（二进制弱符号，DWARF decl_line=None，与头文件模板体一致）

// line 100 (secagent_msg_handler.h 模板体；本文件提供特化)
template<>
int Comm_App_Frame::appdata_encode<sec_proto::ReportHeartbeatReq>(
    unsigned int szframe_appdata, const sec_proto::ReportHeartbeatReq &info,
    unsigned int data_start, size_t *sz_code) {
    size_t use_len = 0;
    int ret = info.pack(frame_appdata_ + data_start,
                        szframe_appdata - data_start, &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    frame_length_ = data_start + use_len + LEN_OF_APPFRAME_HEAD;
    return 0;
}

template<>
int Comm_App_Frame::appdata_decode<sec_proto::SecsvrNotifyIsChangeSvrRsp>(
    sec_proto::SecsvrNotifyIsChangeSvrRsp &info, unsigned int data_start,
    size_t *sz_code) const {
    size_t use_len = 0;
    int ret = info.unpack(frame_appdata_ + data_start,
                          frame_length_ - data_start - LEN_OF_APPFRAME_HEAD,
                          &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    return 0;
}

template<>
int Comm_App_Frame::appdata_decode<sec_proto::SecsvrNotifyKeyOffsetRsp>(
    sec_proto::SecsvrNotifyKeyOffsetRsp &info, unsigned int data_start,
    size_t *sz_code) const {
    size_t use_len = 0;
    int ret = info.unpack(frame_appdata_ + data_start,
                          frame_length_ - data_start - LEN_OF_APPFRAME_HEAD,
                          &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    return 0;
}

template<>
int Comm_App_Frame::appdata_encode<sec_proto::ConfirmRpcodeInfo>(
    unsigned int szframe_appdata, const sec_proto::ConfirmRpcodeInfo &info,
    unsigned int data_start, size_t *sz_code) {
    size_t use_len = 0;
    int ret = info.pack(frame_appdata_ + data_start,
                        szframe_appdata - data_start, &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    frame_length_ = data_start + use_len + LEN_OF_APPFRAME_HEAD;
    return 0;
}

template<>
int Comm_App_Frame::appdata_encode<sec_proto::PunishClientReq>(
    unsigned int szframe_appdata, const sec_proto::PunishClientReq &info,
    unsigned int data_start, size_t *sz_code) {
    size_t use_len = 0;
    int ret = info.pack(frame_appdata_ + data_start,
                        szframe_appdata - data_start, &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    frame_length_ = data_start + use_len + LEN_OF_APPFRAME_HEAD;
    return 0;
}

// line 69 (secagent_gamesvr_channel.h 模板体；本文件提供特化)
template<>
int SecAgentGamesvrChannel::send<sec_proto::PunishClientReq>(
    unsigned int uin, unsigned int cmd, unsigned int appid,
    unsigned int channel_id, const sec_proto::PunishClientReq &pkg) {
    Comm_App_Frame *frame = reinterpret_cast<Comm_App_Frame *>(send_buf_);
    frame->init_framehead(0x10000, 0, cmd);
    frame->frame_uin_ = uin;
    frame->app_id_ = appid;
    frame->appdata_encode(0x10000, pkg, 0, NULL);
    return send(channel_id, send_buf_, frame->frame_length_);
}

template<>
int Comm_App_Frame::appdata_decode<sec_proto::SendToSDK>(
    sec_proto::SendToSDK &info, unsigned int data_start,
    size_t *sz_code) const {
    size_t use_len = 0;
    int ret = info.unpack(frame_appdata_ + data_start,
                          frame_length_ - data_start - LEN_OF_APPFRAME_HEAD,
                          &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    return 0;
}

template<>
int Comm_App_Frame::appdata_decode<sec_proto::RecvDataFromSDKReq>(
    sec_proto::RecvDataFromSDKReq &info, unsigned int data_start,
    size_t *sz_code) const {
    size_t use_len = 0;
    int ret = info.unpack(frame_appdata_ + data_start,
                          frame_length_ - data_start - LEN_OF_APPFRAME_HEAD,
                          &use_len, 0);
    if (ret != 0) {
        return 0x13a0;
    }
    if (sz_code) {
        *sz_code = use_len;
    }
    return 0;
}

// line 100 (secagent_msg_handler.h 模板体；本文件提供特化)
template<>
int SecAgentMsgHandler::send_pkg_to_secsvr<sec_proto::ConfirmRpcodeInfo>(
    unsigned int cmd, const SecAgentUserInfo *user_info,
    const sec_proto::ConfirmRpcodeInfo &pkg) {
    int ret = 0;
    SERVICES_ID recv_info(SECSVR_SERVICE_TYPE, 0);
    SERVICES_ID proxy_info(0, 0);
    char send_buf[0x10000];
    Comm_App_Frame *send_frame = reinterpret_cast<Comm_App_Frame *>(send_buf);

    ZEN_Trace_LogMsg::debug_debugEx("send pkg to secsvr, uin=%u, cmd=%d",
                                    user_info->base_info_.uin_, cmd);
    SecAgentBillHandler::write_bill_send_to_secsvr(user_info->base_info_.uin_,
                                                   user_info->base_info_.appid_,
                                                   cmd);
    send_frame->init_framehead(0x10000, 0, cmd);
    send_frame->frame_uin_ = user_info->base_info_.uin_;
    send_frame->app_id_ = user_info->base_info_.appid_;
    send_frame->send_service_ = Comm_Svrd_Config::instance()->self_svr_id_;
    send_frame->recv_service_ = recv_info;
    send_frame->proxy_service_ = proxy_info;
    send_frame->transaction_id_ = 0;
    send_frame->backfill_trans_id_ = 0;
    ret = send_frame->appdata_encode<sec_proto::ConfirmRpcodeInfo>(
        0xffbd, pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]TDR encode fail.ret =%d,Please check your code and buffer len.",
            "int SecAgentMsgHandler::send_pkg_to_secsvr(unsigned int, const SecAgentUserInfo*, const T&) [with T = sec_proto::ConfirmRpcodeInfo]",
            ret);
        return ret;
    }
    Zerg_MMAP_BusPipe *bus = Zerg_MMAP_BusPipe::instance();
    if (send_frame->frame_length_ - Comm_App_Frame::LEN_OF_APPFRAME_HEAD > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            send_frame->frame_length_);
        return 0x13b1;
    }
    if (bus->config_->is_monitor_uin(send_frame->frame_uin_)) {
        send_frame->frame_option_ |= 0x10000;
    }
    if (send_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(send_frame, "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    ZEN_LIB::shm_dequechunk *send_pipe = reinterpret_cast<ZEN_LIB::shm_dequechunk *>(
        reinterpret_cast<char *>(bus) + 0x2010);
    if (send_pipe->push_end((const ZEN_LIB::dequechunk_node *)send_frame)) {
        bus->monitor_->increase_by_statid(0x238e, send_frame->app_id_,
                                          send_frame->frame_command_, 1);
        bus->monitor_->increase_by_statid(0x239c, send_frame->app_id_,
                                          send_frame->frame_command_,
                                          send_frame->frame_length_);
        return 0;
    }
    size_t free_size = send_pipe->freesize();
    ZEN_Trace_LogMsg::debug_alertex(
        "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
        1, send_frame->frame_length_, free_size);
    bus->monitor_->increase_by_statid(0x238f, send_frame->app_id_,
                                      send_frame->frame_command_, 1);
    return 0x1396;
}

// line 10
SecAgentMsgHandler::SecAgentMsgHandler() {
    send_frame_ = new (0x10020) Comm_App_Frame(0, 0x10000, 0x1000000);
}

// line 14
SecAgentMsgHandler::~SecAgentMsgHandler() {
    if (send_frame_ != NULL) {
        send_frame_->~Comm_App_Frame();
        Comm_App_Frame::operator delete(send_frame_, 0x32);
    }
    login_restrict_.~SecagentRestrict();
    antibot_crypter_.~AntibotCrypter();
    antibot_msg_handler_.~SecagentAntibotMsgHandler();
}

// line 20
int SecAgentMsgHandler::proc_client_msg(Comm_App_Frame *recv_frame) {
    int ret = 0;
    char *recv_data = recv_frame->frame_appdata_;
    unsigned short channel_id;

    ZEN_Trace_LogMsg::debug_infoex("recv client pkg, uin=%u, cmd=%u",
                                   recv_frame->frame_uin_,
                                   recv_frame->frame_command_);
    switch (recv_frame->frame_command_) {
    case SECAGENT_CMD_CLIENT_DEL_USER:
        channel_id = ntohs(*(unsigned short *)recv_data);
        ret = proc_del_user(recv_frame->frame_uin_, recv_frame->app_id_);
        SecAgentBillHandler::write_bill_del_user_from_sdk(
            recv_frame->app_id_, recv_frame->frame_uin_,
            recv_frame->frame_command_, ret, channel_id);
        break;
    case SECAGENT_CMD_CLIENT_CS_SDK_PKG:
        ret = proc_cs_sdk_pkg(recv_frame);
        break;
    case SECAGENT_CMD_CLIENT_ADD_USER:
        channel_id = ntohs(*(unsigned short *)recv_data);
        ret = proc_add_user(recv_frame->frame_uin_, recv_frame->app_id_,
                            channel_id);
        SecAgentBillHandler::write_bill_add_user_from_sdk(
            recv_frame->app_id_, recv_frame->frame_uin_,
            recv_frame->frame_command_, ret, channel_id);
        break;
    default:
        ZEN_Trace_LogMsg::debug_debugEx("no need to proc client pkg, cmd=%u, uin=%u",
                                        recv_frame->frame_command_,
                                        recv_frame->frame_uin_);
        break;
    }
    SecAgentUserInfoMgr::instance()->active(recv_frame->frame_uin_,
                                            recv_frame->app_id_);
    if (ret != 0) {
        Comm_Stat_Monitor::instance()->increase_by_statid(
            0x49405, CfgSvrSdk::instance()->get_game_id(), ret, 1);
        if (ret != 0x49408) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[%s], proc client msg error, cmd=%u, ret=%d, uin=%u",
                __PRETTY_FUNCTION__, recv_frame->frame_command_, ret,
                recv_frame->frame_uin_);
        }
    }
    Zerg_MMAP_BusPipe *bus = Zerg_MMAP_BusPipe::instance();
    if (recv_frame->frame_length_ - Comm_App_Frame::LEN_OF_APPFRAME_HEAD > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            recv_frame->frame_length_);
        ZEN_Trace_LogMsg::debug_errorex("[%s], send sdk pkg to secsvr fail, ret=%d",
                                        __PRETTY_FUNCTION__, 0x13b1);
        return 0x13b1;
    }
    if (bus->config_->is_monitor_uin(recv_frame->frame_uin_)) {
        recv_frame->frame_option_ |= 0x10000;
    }
    if (recv_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(recv_frame, "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    ZEN_LIB::shm_dequechunk *send_pipe = reinterpret_cast<ZEN_LIB::shm_dequechunk *>(
        reinterpret_cast<char *>(bus) + 0x2010);
    if (send_pipe->push_end((const ZEN_LIB::dequechunk_node *)recv_frame)) {
        bus->monitor_->increase_by_statid(0x238e, recv_frame->app_id_,
                                          recv_frame->frame_command_, 1);
        bus->monitor_->increase_by_statid(0x239c, recv_frame->app_id_,
                                          recv_frame->frame_command_,
                                          recv_frame->frame_length_);
        return 0;
    }
    size_t free_size = send_pipe->freesize();
    ZEN_Trace_LogMsg::debug_alertex(
        "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
        1, recv_frame->frame_length_, free_size);
    bus->monitor_->increase_by_statid(0x238f, recv_frame->app_id_,
                                      recv_frame->frame_command_, 1);
    ZEN_Trace_LogMsg::debug_errorex("[%s], send sdk pkg to secsvr fail, ret=%d",
                                    __PRETTY_FUNCTION__, 0x1396);
    return 0x1396;
}

// line 116
int SecAgentMsgHandler::proc_sever_msg(Comm_App_Frame *recv_frame) {
    int ret = 0;
    ZEN_Trace_LogMsg::debug_debugEx("recv pkg from secsvr, cmd=%u, uin=%u",
                                    recv_frame->frame_command_,
                                    recv_frame->frame_uin_);
    switch (recv_frame->frame_command_) {
    case SECAGENT_CMD_SVR_RPCODE_KEYOFFSET_RSP:
        ZEN_Trace_LogMsg::debug_debugEx("recv secsvr rsp rpcode keyoffset, uin=%u",
                                        recv_frame->frame_uin_);
        ret = proc_rpcode_info_pkg(recv_frame);
        if (ret != 0) {
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x49406, CfgSvrSdk::instance()->get_game_id(), ret, 1);
        }
        return 0x493fd;
    case SECAGENT_CMD_SVR_NOTIFY_IS_CHANGE_SVR:
        ZEN_Trace_LogMsg::debug_debugEx("recv secsvr notify user change svr or not, uin=%u",
                                        recv_frame->frame_uin_);
        ret = proc_notify_is_change_svr(recv_frame);
        if (ret != 0) {
            Comm_Stat_Monitor::instance()->increase_by_statid(
                0x49406, CfgSvrSdk::instance()->get_game_id(), ret, 1);
        }
        return 0x493fd;
    case SECAGENT_CMD_SVR_SEND_TO_SDK:
        ZEN_Trace_LogMsg::debug_debugEx("recv pkg of send to sdk, uin=%u",
                                        recv_frame->frame_uin_);
        proc_sc_sdk_pkg(recv_frame);
        return 0;
    default:
        ZEN_Trace_LogMsg::debug_debugEx("no need proc svr pkg, cmd=%u",
                                        recv_frame->frame_command_);
        return 0;
    }
}

// line 184
int SecAgentMsgHandler::proc_add_user(unsigned int uin, unsigned int game_id,
                                      unsigned short channel_id) {
    int ret = 0;
    SecAgentBillHandler::add_statistic_bill(0x493e1, 1);
    if (!login_restrict_.is_allow_login(uin)) {
        ZEN_Trace_LogMsg::debug_infoex(
            "user not allowed login, no need add, uin=%u, appid=%u, channel_id=%u",
            uin, game_id, channel_id);
        ret = punish_user(uin, game_id, channel_id, 2);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("punish user fail. uin=%u ret=%d",
                                            uin, ret);
        }
        SecAgentBillHandler::add_statistic_bill(0x49402, 1);
        return 0x493fc;
    }
    ret = SecAgentUserInfoMgr::instance()->add_user(uin, game_id, channel_id);
    if (ret != 0) {
        SecAgentBillHandler::add_statistic_bill(0x493e3, 1);
        ZEN_Trace_LogMsg::debug_errorex("[%s], add user fail, ret=%d, uin=%u",
                                        __PRETTY_FUNCTION__, ret, uin);
        return 0x493ee;
    }
    SecAgentBillHandler::add_statistic_bill(0x493e2, 1);
    ZEN_Trace_LogMsg::debug_infoex("add user succ, uin=%u", uin);
    return 0;
}

// line 223
int SecAgentMsgHandler::proc_del_user(unsigned int uin, unsigned int game_id) {
    int ret = 0;
    SecAgentBillHandler::add_statistic_bill(0x493e4, 1);
    if (!login_restrict_.is_allow_login(uin)) {
        ZEN_Trace_LogMsg::debug_infoex("user not allowed login, no need del, uin=%u, appid=%u",
                                       uin, game_id);
        return 0x493fc;
    }
    ret = SecAgentUserInfoMgr::instance()->del_user(uin, game_id);
    if (ret != 0) {
        SecAgentBillHandler::add_statistic_bill(0x493e6, 1);
        ZEN_Trace_LogMsg::debug_errorex("[%s], del user fail, ret=%d, uin=%u",
                                        __PRETTY_FUNCTION__, ret, uin);
        return 0x49408;
    }
    SecAgentBillHandler::add_statistic_bill(0x493e5, 1);
    ZEN_Trace_LogMsg::debug_infoex("del user succ, uin=%u", uin);
    return 0;
}

// line 262
int SecAgentMsgHandler::proc_cs_sdk_pkg(Comm_App_Frame *recv_frame) {
    int ret = 0;
    sec_proto::RecvDataFromSDKReq sdk_pkg;
    ret = recv_frame->appdata_decode<sec_proto::RecvDataFromSDKReq>(sdk_pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("antibot pkg decode fail. ret=%d uin=%u",
                                        ret, recv_frame->frame_uin_);
        return ret;
    }
    if (sdk_pkg.to_busi_id_ != 1) {
        SecAgentBillHandler::add_statistic_bill(0x493e8, 1);
        ZEN_Trace_LogMsg::debug_debugEx("proc cs sdk pkg, not antibot, to_busi_id=%d, uin=%u",
                                        sdk_pkg.to_busi_id_, recv_frame->frame_uin_);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_debugEx("proc cs sdk pkg, antibot, check is_need_decrypt, uin=%u",
                                    recv_frame->frame_uin_);
    return proc_cs_sdk_antibot_pkg(recv_frame, &sdk_pkg);
}

// line 297
int SecAgentMsgHandler::proc_cs_sdk_antibot_pkg(Comm_App_Frame *recv_frame,
                                                sec_proto::RecvDataFromSDKReq *sdk_pkg) {
    return antibot_msg_handler_.proc_tsssdk_antibot_msg(recv_frame, sdk_pkg);
}

// line 310
int SecAgentMsgHandler::proc_sc_sdk_pkg(Comm_App_Frame *recv_frame) {
    int ret = 0;
    sec_proto::SendToSDK sdk_pkg;
    ret = recv_frame->appdata_decode<sec_proto::SendToSDK>(sdk_pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], proc sc sdk pkg, unpack decode fail, ret=%d, uin=%u",
            __PRETTY_FUNCTION__, ret, recv_frame->frame_uin_);
        return 0x493ec;
    }
    if (sdk_pkg.to_busi_id_ != 1) {
        SecAgentBillHandler::add_statistic_bill(0x493f5, 1);
        ZEN_Trace_LogMsg::debug_debugEx(
            "enter proc sc sdk pkg, not antibot pkg, no need proc, uin=%u, to_busi_id_=%d",
            recv_frame->frame_uin_, sdk_pkg.to_busi_id_);
        return 0;
    }
    ZEN_Trace_LogMsg::debug_debugEx("enter proc sc sdk pkg, antibot pkg, will proc, uin=%u",
                                    recv_frame->frame_uin_);
    ret = proc_sc_sdk_antibot_pkg(recv_frame, &sdk_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], proc_sc_sdk_pkg fail, ret=%u, uin=%u",
                                        __PRETTY_FUNCTION__, ret,
                                        recv_frame->frame_uin_);
        return 0x493ec;
    }
    ZEN_Trace_LogMsg::debug_infoex("proc_sc_sdk_pkg succ, uin=%u",
                                   recv_frame->frame_uin_);
    return 0;
}

// line 354
int SecAgentMsgHandler::proc_sc_sdk_antibot_pkg(Comm_App_Frame *recv_frame,
                                                sec_proto::SendToSDK *sdk_pkg) {
    return antibot_msg_handler_.proc_secsvr_antibot_msg(recv_frame, sdk_pkg);
}

// line 360
int SecAgentMsgHandler::init(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
    return login_restrict_.init(conf->restrict_file_);
}

// line 366
int SecAgentMsgHandler::reload(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
    return login_restrict_.reload(conf->restrict_file_);
}

// line 375
int SecAgentMsgHandler::punish_user(unsigned int uin, unsigned int game_id,
                                    unsigned short channel_id, int punish_mode) {
    sec_proto::PunishClientReq punish_req;
    punish_req.channel_id_ = channel_id;
    punish_req.punish_type_ = punish_mode;
    punish_req.punish_duration_ = 0;
    punish_req.punish_info_len_ = 0;
    punish_req.punish_reason_len_ = 0;
    SecAgentApp *app = SecAgentApp::instance();
    return app->gamesvr_channel_.send<sec_proto::PunishClientReq>(
        uin, SECAGENT_CMD_PUNISH_USER, game_id, channel_id, punish_req);
}

// line 395
int SecAgentMsgHandler::proc_rpcode_info_pkg(Comm_App_Frame *recv_frame) {
    int ret = 0;
    sec_proto::SecsvrNotifyKeyOffsetRsp rpcode_info_pkg;
    SecAgentUserInfo *user_info = NULL;
    const SecagentGameExe *game_exe = NULL;
    sec_proto::ConfirmRpcodeInfo rsp_pkg;

    ret = recv_frame->appdata_decode<sec_proto::SecsvrNotifyKeyOffsetRsp>(
        rpcode_info_pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], proc rpcode info pkg, unpack decode fail, ret=%d, uin=%u",
            __PRETTY_FUNCTION__, ret, recv_frame->frame_uin_);
        return 0x493ec;
    }
    user_info = SecAgentUserInfoMgr::instance()->get_user(recv_frame->frame_uin_,
                                                          recv_frame->app_id_);
    if (user_info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], get user fail, uin=%u",
                                        __PRETTY_FUNCTION__,
                                        recv_frame->frame_uin_);
        return 0x493e7;
    }
    game_exe = SecAgentApp::instance()->get_game_exe(
        user_info->antibot_crypt_info_.client_ver_);
    if (game_exe == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("get game exe fail, uin=%u, client_ver=%u",
                                        user_info->base_info_.uin_,
                                        user_info->antibot_crypt_info_.client_ver_);
        SecAgentBillHandler::add_statistic_bill(0x49403, 1);
        return 0x4940a;
    }
    if (rpcode_info_pkg.key_size_ > SecAgentUserInfo::MAX_KEY_LEN) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], rpcode key size error, uin=%u, send_key_size=%u, key_size=%u",
            __PRETTY_FUNCTION__, user_info->base_info_.uin_,
            rpcode_info_pkg.key_size_, SecAgentUserInfo::MAX_KEY_LEN);
        return 0x493fe;
    }
    if (rpcode_info_pkg.key_offset_ + rpcode_info_pkg.key_size_ >=
        game_exe->exe_data_size_) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], rpcode key offset error, uin=%u, keyoffset=%u, key_size=%u, exe_size=%u",
            __PRETTY_FUNCTION__, user_info->base_info_.uin_,
            rpcode_info_pkg.key_offset_, rpcode_info_pkg.key_size_,
            game_exe->exe_data_size_);
        return 0x493fe;
    }
    if (game_exe->exe_data_size_ <=
        rpcode_info_pkg.key_offset_ + SecAgentUserInfo::MAX_KEY_LEN) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s], rpcode key offset error, uin=%u, keyoffset=%u, exe_size=%u",
            __PRETTY_FUNCTION__, user_info->base_info_.uin_,
            rpcode_info_pkg.key_offset_, game_exe->exe_data_size_);
        return 0x493fe;
    }
    rsp_pkg.world_id_ = rpcode_info_pkg.world_id_;
    rsp_pkg.dib_id_ = rpcode_info_pkg.dib_id_;
    rsp_pkg.key_offset_ = rpcode_info_pkg.key_offset_;
    ret = send_pkg_to_secsvr<sec_proto::ConfirmRpcodeInfo>(
        SECAGENT_CMD_CONFIRM_RPCODE, user_info, rsp_pkg);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("rsp to secsvr fail, ret=%d, uin=%u",
                                        user_info->base_info_.uin_);
        return ret;
    }
    user_info->rpcode_crypt_info_.dib_id_ = rpcode_info_pkg.dib_id_;
    user_info->rpcode_crypt_info_.key_size = rpcode_info_pkg.key_size_;
    memcpy(user_info->rpcode_crypt_info_.key_,
           game_exe->exe_data_ + rpcode_info_pkg.key_offset_,
           rpcode_info_pkg.key_size_);
    ZEN_Trace_LogMsg::debug_infoex(
        "update user rpcode crypt info succ, uin=%u, dib_id=%u, key_offset=%u, key_size=%u",
        user_info->base_info_.uin_, user_info->rpcode_crypt_info_.dib_id_,
        rpcode_info_pkg.key_offset_, user_info->rpcode_crypt_info_.key_size);
    user_info->rpcode_crypt_info_.dib_id_ = rpcode_info_pkg.dib_id_;
    memcpy(user_info->rpcode_crypt_info_.key_,
           game_exe->exe_data_ + rpcode_info_pkg.key_offset_,
           SecAgentUserInfo::MAX_KEY_LEN);
    ZEN_Trace_LogMsg::debug_infoex(
        "update user rpcode crypt info succ, uin=%u, dib_id=%u, key_offset=%u",
        user_info->base_info_.uin_, user_info->rpcode_crypt_info_.dib_id_,
        rpcode_info_pkg.key_offset_);
    SecAgentBillHandler::write_bill_notify_rpcode_info(
        user_info->base_info_.uin_, user_info->base_info_.appid_,
        user_info->rpcode_crypt_info_.dib_id_, rpcode_info_pkg.key_offset_);
    SecAgentApp::instance();
    return 0;
}

// line 533
int SecAgentMsgHandler::proc_notify_is_change_svr(const Comm_App_Frame *recv_frame) {
    int ret = 0;
    sec_proto::SecsvrNotifyIsChangeSvrRsp notify_pkg;
    SecAgentUserInfo *user_info = NULL;
    SecAgentDPsdkInfoMgr::LPCONFIG conf = NULL;

    ret = recv_frame->appdata_decode<sec_proto::SecsvrNotifyIsChangeSvrRsp>(
        notify_pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], unpack notify pkg fail, ret=%d, uin=%u",
                                        __PRETTY_FUNCTION__, ret,
                                        recv_frame->frame_uin_);
        return 0x493ec;
    }
    user_info = SecAgentUserInfoMgr::instance()->get_user(recv_frame->frame_uin_,
                                                          recv_frame->app_id_);
    if (user_info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], get user fail, uin=%u",
                                        __PRETTY_FUNCTION__,
                                        recv_frame->frame_uin_);
        return 0x493e7;
    }
    SecAgentBillHandler::write_bill_notify_is_change_svr(
        recv_frame->frame_uin_, recv_frame->app_id_, notify_pkg.is_change_svr_);
    conf = SecAgentApp::instance()->get_config();
    if (conf->crypt_checker_conf_.is_use_) {
        if (notify_pkg.is_change_svr_) {
            antibot_msg_handler_.send_cmd_list(user_info);
            ZEN_Trace_LogMsg::debug_infoex(
                "SecagentAntibotMsgHandler::proc_notify_is_change_svr, send cmd list to client, uin=%u",
                user_info->base_info_.uin_);
        } else {
            antibot_msg_handler_.send_cmd_dib_key(user_info);
            ZEN_Trace_LogMsg::debug_infoex(
                "SecagentAntibotMsgHandler::proc_notify_is_change_svr, send cmd dib key to client, uin=%u, dibid=%d, keyoffset=%u, key_len=%u",
                user_info->base_info_.uin_,
                user_info->antibot_crypt_info_.dib_id_,
                user_info->antibot_crypt_info_.key_offset_,
                user_info->antibot_crypt_info_.key_len_);
        }
    }
    return 0;
}

// line 589
int SecAgentMsgHandler::heart_beat_to_secsvr() {
    int ret = 0;
    sec_proto::ReportHeartbeatReq heart_beat_pkg;
    SERVICES_ID recv_info(SECSVR_SERVICE_TYPE, 0);
    SERVICES_ID proxy_info(0, 0);
    char send_buf[0x10000];
    Comm_App_Frame *send_frame = reinterpret_cast<Comm_App_Frame *>(send_buf);

    ZEN_Trace_LogMsg::debug_debugEx("send heart beat pkg to secsvr");
    heart_beat_pkg.room_id_ = CfgSvrSdk::instance()->get_world_id();
    send_frame->init_framehead(0x10000, 0, SECAGENT_CMD_HEART_BEAT);
    send_frame->frame_uin_ = 0;
    send_frame->app_id_ = CfgSvrSdk::instance()->get_game_id();
    send_frame->send_service_ = Comm_Svrd_Config::instance()->self_svr_id_;
    send_frame->recv_service_ = recv_info;
    send_frame->proxy_service_ = proxy_info;
    ret = send_frame->appdata_encode<sec_proto::ReportHeartbeatReq>(
        0xffbd, heart_beat_pkg, 0, NULL);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[%s]TDR encode fail.ret =%d,Please check your code and buffer len.",
            "static int SecAgentMsgHandler::heart_beat_to_secsvr()", ret);
        return ret;
    }
    Zerg_MMAP_BusPipe *bus = Zerg_MMAP_BusPipe::instance();
    if (send_frame->frame_length_ - Comm_App_Frame::LEN_OF_APPFRAME_HEAD > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            send_frame->frame_length_);
        return 0x13b1;
    }
    if (bus->config_->is_monitor_uin(send_frame->frame_uin_)) {
        send_frame->frame_option_ |= 0x10000;
    }
    if (send_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(send_frame, "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    ZEN_LIB::shm_dequechunk *send_pipe = reinterpret_cast<ZEN_LIB::shm_dequechunk *>(
        reinterpret_cast<char *>(bus) + 0x2010);
    if (send_pipe->push_end((const ZEN_LIB::dequechunk_node *)send_frame)) {
        bus->monitor_->increase_by_statid(0x238e, send_frame->app_id_,
                                          send_frame->frame_command_, 1);
        bus->monitor_->increase_by_statid(0x239c, send_frame->app_id_,
                                          send_frame->frame_command_,
                                          send_frame->frame_length_);
        return 0;
    }
    size_t free_size = send_pipe->freesize();
    ZEN_Trace_LogMsg::debug_alertex(
        "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
        1, send_frame->frame_length_, free_size);
    bus->monitor_->increase_by_statid(0x238f, send_frame->app_id_,
                                      send_frame->frame_command_, 1);
    return 0x1396;
}
