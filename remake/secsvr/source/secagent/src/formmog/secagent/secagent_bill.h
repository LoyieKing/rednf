// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_bill.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_

#include <stdint.h>
#include "output/commlib/zenlib/release/include/zen_server_status.h"

// sizeof = 1
struct SecAgentBillHandler { // line 230
private:
static SecAgentBillHandler *instance_;
SecAgentBillHandler(); // line 232
~SecAgentBillHandler(); // line 233
public:
static SecAgentBillHandler * instance(); // line 236
static void clean_instance(); // line 237
static void add_statistic_bill(unsigned int statistic_id, unsigned int incre); // line 240
static void write_bill_add_user_from_sdk(unsigned int game_id, unsigned int uin, unsigned int cmd, unsigned int ret, short unsigned int channel_id); // line 247
static void write_bill_del_user_from_sdk(unsigned int game_id, unsigned int uin, unsigned int cmd, unsigned int ret, short unsigned int channel_id); // line 254
static void write_bill_expire_user(unsigned int uin, unsigned int appid); // line 257
static void write_bill_send_secsvr_error(unsigned int uin, unsigned int appid, unsigned int error_id); // line 262
static void write_bill_antibot_test_decypt_fail(unsigned int uin, unsigned int appid, unsigned int pkg_crc, unsigned int tmp_crc, unsigned int content1, unsigned int content2); // line 270
static void write_bill_rpcode_check_fail(unsigned int uin, unsigned int appid, unsigned int local_hash, unsigned int client_hash, unsigned int dib_id); // line 276
static void write_bill_antibot_ready_pkg(unsigned int uin, unsigned int appid, unsigned int pkg_dibid, unsigned int local_dibid, unsigned int pkg_crc, unsigned int local_crc, uint8_t is_ready); // line 285
static void write_bill_antibot_decrypt_fail(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_offset, unsigned int key_len); // line 292
static void write_bill_antibot_nocrypt_fail(unsigned int uin, unsigned int appid, unsigned int cmd_id); // line 296
static void write_bill_antibot_de_log(unsigned int uin, unsigned int appid, unsigned int error_id, unsigned int error, unsigned int pad_size); // line 302
static void write_bill_notify_is_change_svr(unsigned int uin, unsigned int appid, unsigned int is_change_sv); // line 307
static void write_bill_notify_rpcode_info(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_offset); // line 312
static void write_bill_antibot_crypt_info(unsigned int uin, unsigned int appid, unsigned int dib_id, unsigned int key_len, unsigned int key_offset, unsigned int crc); // line 319
static void write_bill_antibot_cmd_list(unsigned int uin, unsigned int appid, uint8_t cmd_count); // line 324
static void write_bill_send_to_secsvr(unsigned int uin, unsigned int appid, unsigned int cmd); // line 329
};
static const unsigned int SECAGENT_NUMBER_OF_COUNTER = 38; // line 183
// line 184：每个包含本头的 TU 各持有一份（.bss，动态构造），
// 与二进制 STB_LOCAL 3496 字节数组（38 x 92）的观测一致。
static ZEN_STATUS_ITEM_WITHNAME STR_SECAGENT_STAT[SECAGENT_NUMBER_OF_COUNTER] = {
    ZEN_STATUS_ITEM_WITHNAME(0x493e1, STATICS_PER_FIVE_MINTUES, "RECV_SDK_ADD_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e2, STATICS_PER_FIVE_MINTUES, "ADD_USER_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e3, STATICS_PER_FIVE_MINTUES, "ADD_USER_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e4, STATICS_PER_FIVE_MINTUES, "RECV_SDK_DEL_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e5, STATICS_PER_FIVE_MINTUES, "DEL_USER_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e6, STATICS_PER_FIVE_MINTUES, "DEL_USER_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e7, STATICS_PER_FIVE_MINTUES, "EXPIRE_USER_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e8, STATICS_PER_FIVE_MINTUES, "SDK_REQ_RECV_DATA_NOT_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493e9, STATICS_PER_FIVE_MINTUES, "SDK_REQ_RECV_DATA_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ea, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_OTHER_NO_PROC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493eb, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_INIT_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ec, STATICS_PER_FIVE_MINTUES, "UPDATE_DIB_FROM_INIT_PKG_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ed, STATICS_PER_FIVE_MINTUES, "UPDATE_KEY_FROM_INIT_PKG_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ee, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_ENCRYPT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ef, STATICS_PER_FIVE_MINTUES, "ANTIBOT_DECRYPT_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f0, STATICS_PER_FIVE_MINTUES, "ANTIBOT_DECRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f1, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_READY_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f2, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_TEST_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f3, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_CLIENT_ERROR_LOG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f4, STATICS_PER_FIVE_MINTUES, "RECV_ANTIBOT_RPCODE_PKG_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f5, STATICS_PER_FIVE_MINTUES, "SECSVR_FORWARD_NOT_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f6, STATICS_PER_FIVE_MINTUES, "SECSVR_FORWARD_ANTIBOT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f7, STATICS_PER_FIVE_MINTUES, "ANTIBOT_NEED_ENCRYPT_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f8, STATICS_PER_FIVE_MINTUES, "ANTIBOT_ENCRYPT_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493f9, STATICS_PER_FIVE_MINTUES, "ANTIBOT_ENCRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fa, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_ENCRYPT_INFO_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fb, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_DECRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fc, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_UNENCRYPT_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fd, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_DECRYPT_TEST_PKG_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493fe, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_CHECK_DELOG_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(0x493ff, STATICS_PER_FIVE_MINTUES, "ONLINE_USER_NUM_FEATURE_ID"),
    ZEN_STATUS_ITEM_WITHNAME(0x49400, STATICS_PER_FIVE_MINTUES, "RPCODE_DECRYPT_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49401, STATICS_PER_FIVE_MINTUES, "RPCODE_KEY_NOT_READY_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49402, STATICS_PER_FIVE_MINTUES, "RESTRICT_PUNISH_USER_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49403, STATICS_PER_FIVE_MINTUES, "GET_GAME_EXE_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49404, STATICS_PER_FIVE_MINTUES, "SEND_SECSVR_ERROR_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49405, STATICS_PER_FIVE_MINTUES, "PROC_CLIENT_PKG_FAIL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x49406, STATICS_PER_FIVE_MINTUES, "PROC_SERVER_PKG_FAIL_COUNT"),
};


// inferred

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_
