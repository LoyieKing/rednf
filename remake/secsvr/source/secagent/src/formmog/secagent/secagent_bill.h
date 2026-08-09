// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_bill.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_

#include <stdint.h>

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
extern const int SECAGENT_NUMBER_OF_COUNTER; // line 183
extern ZEN_STATUS_ITEM_WITHNAME STR_SECAGENT_STAT[]; // line 184


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_BILL_H_H_
