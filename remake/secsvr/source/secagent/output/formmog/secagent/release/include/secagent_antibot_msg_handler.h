// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_antibot_msg_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_MSG_HANDLER_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_MSG_HANDLER_H_H_

#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

// sizeof = 16
struct SecagentAntibotMsgHandler { // line 14
    typedef /*anon struct*/ int *LPCONFIG;
private:
AntibotCrypter antibot_crypter_;
SecAgentDPsdkInfoMgr::LPCONFIG conf_;
SecAgentUserInfo *user_info_;
bool is_encrypt_pkg_;
public:
SecagentAntibotMsgHandler(); // line 16
~SecagentAntibotMsgHandler(); // line 17
int proc_secsvr_antibot_msg(Comm_App_Frame *arg0, /*anon struct*/ int *arg1); // line 21
int proc_tsssdk_antibot_msg(Comm_App_Frame *arg0, /*anon struct*/ int *arg1); // line 24
int on_check_timer(time_t arg0, SecAgentUserInfo *arg1); // line 27
int send_cmd_list(SecAgentUserInfo *arg0); // line 30
int send_cmd_dib_key(SecAgentUserInfo *arg0); // line 32
private:
bool is_down_cmd_list(uint16_t arg0); // line 36
bool is_up_cmd_list(uint16_t arg0); // line 38
int encrypt(Comm_App_Frame *arg0, /*anon struct*/ int *arg1); // line 41
int decrypt(/*anon struct*/ int *arg0, uint16_t *arg1); // line 43
int proc_ab_init_pkg(const /*anon struct*/ int &arg0); // line 46
int proc_ab_ready_pkg(const /*anon struct*/ int &arg0); // line 49
int proc_ab_de_testpkg(const /*anon struct*/ int &arg0); // line 52
int proc_ab_de_log(const /*anon struct*/ int &arg0); // line 55
int proc_ab_rpcode_pkg(/*anon struct*/ int *arg0); // line 58
int proc_secsvr_ab_other_msg(uint16_t arg0, Comm_App_Frame *arg1, /*anon struct*/ int *arg2); // line 63
int proc_tsssdk_ab_other_msg(uint16_t arg0, const /*anon struct*/ int &arg1); // line 66
int pack_secsvr_antibot_to_frame(Comm_App_Frame *arg0, /*anon struct*/ int *arg1, const /*anon struct*/ int &arg2); // line 71
int send_error_pkg_to_secsvr(const /*anon struct*/ int &arg0, unsigned int arg1); // line 74
int send_testpkg(); // line 77
int send_antibot_msg(const /*anon struct*/ int &arg0, unsigned int arg1, unsigned int arg2); // line 82
int send_buff_to_client(const char *arg0, size_t arg1, unsigned int arg2, unsigned int arg3); // line 88
int get_crypt_id() const; // line 90
int proc_tenparty_data_msg(const AntibotSanlixData *arg0, /*anon struct*/ int *arg1); // line 94
int proc_scheme_data_msg(const AntibotSanlixData *arg0, /*anon struct*/ int *arg1); // line 97
int proc_slx_share_info_msg_ts(const AntibotSanlixData *arg0, /*anon struct*/ int *arg1); // line 100
int proc_slx_share_info_msg_te(const AntibotSanlixData *arg0, /*anon struct*/ int *arg1); // line 103
int proc_sanlix_data_msg(uint16_t arg0, Comm_App_Frame *arg1, /*anon struct*/ int *arg2); // line 107
unsigned int rpcode_hash(const unsigned char *arg0, unsigned int arg1); // line 109
};


// inferred
extern void get_quickly_func(unsigned int *idx, const DibData *cdib_data); // line 1106 (inferred)

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_MSG_HANDLER_H_H_
