// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_msg_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_MSG_HANDLER_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_MSG_HANDLER_H_H_

#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_restrict.h"

// sizeof = 52
struct SecAgentMsgHandler { // line 28
public:
static const unsigned int MAX_FRAME_LEN;
static const unsigned int MAX_RPCODE_KEY_INFO_LEN;
private:
SecagentAntibotMsgHandler antibot_msg_handler_;
AntibotCrypter antibot_crypter_;
SecagentRestrict login_restrict_;
Comm_App_Frame *send_frame_;
public:
SecAgentMsgHandler(); // line 33
~SecAgentMsgHandler(); // line 34
int init(SecAgentDPsdkInfoMgr::LPCONFIG conf); // line 37
int reload(SecAgentDPsdkInfoMgr::LPCONFIG conf); // line 40
int proc_client_msg(Comm_App_Frame *recv_frame); // line 43
int proc_sever_msg(Comm_App_Frame *recv_frame); // line 46
static int heart_beat_to_secsvr(); // line 49
private:
int proc_add_user(unsigned int uin, unsigned int game_id, short unsigned int channel_id); // line 53
int proc_del_user(unsigned int uin, unsigned int game_id); // line 55
int proc_cs_sdk_pkg(Comm_App_Frame *recv_frame); // line 58
int proc_sc_sdk_pkg(Comm_App_Frame *recv_frame); // line 60
int proc_cs_sdk_antibot_pkg(Comm_App_Frame *recv_frame, /*anon struct*/ int *sdk_pkg); // line 63
int proc_sc_sdk_antibot_pkg(Comm_App_Frame *recv_frame, /*anon struct*/ int *sdk_pkg); // line 65
int punish_user(unsigned int uin, unsigned int game_id, short unsigned int channel_id, int punish_mode); // line 68
int proc_rpcode_info_pkg(Comm_App_Frame *recv_frame); // line 71
int proc_notify_is_change_svr(const Comm_App_Frame *recv_frame); // line 74
int create_rpcode_key_pkg(const SecAgentUserInfo *arg0, unsigned int arg1, short unsigned int arg2); // line 77
int send_pkg_to_secsvr<sec_proto::ConfirmRpcodeInfo>(unsigned int cmd, const SecAgentUserInfo *user_info, const /*anon struct*/ int &pkg); // line 100
};
extern const unsigned int MAX_FRAME_LEN; // line 30


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_MSG_HANDLER_H_H_
