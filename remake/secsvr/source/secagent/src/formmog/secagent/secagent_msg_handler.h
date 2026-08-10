// Reconstructed from secagent disassembly + DWARF (2026-08-10)
// Original path: src/formmog/secagent/secagent_msg_handler.h
// sizeof = 52: antibot_msg_handler_@0(16) antibot_crypter_@16(4)
// login_restrict_@20(28) send_frame_@48(4)
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_MSG_HANDLER_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_MSG_HANDLER_H_H_

#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/formmog/secagent/secagent_restrict.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include "src/protocol/formmog/formmog_proto_public_secsvr.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"
#include "src/commlib/framework/comm_app_frame.h"

struct SecAgentMsgHandler {
public:
    static const unsigned int MAX_FRAME_LEN = 0x10000;
    static const unsigned int MAX_RPCODE_KEY_INFO_LEN = 128;

private:
    SecagentAntibotMsgHandler antibot_msg_handler_; // 0
    AntibotCrypter antibot_crypter_;                // 16
    SecagentRestrict login_restrict_;               // 20
    Comm_App_Frame *send_frame_;                    // 48

public:
    SecAgentMsgHandler();
    ~SecAgentMsgHandler();
    int init(SecAgentDPsdkInfoMgr::LPCONFIG conf);
    int reload(SecAgentDPsdkInfoMgr::LPCONFIG conf);
    int proc_client_msg(Comm_App_Frame *recv_frame);
    int proc_sever_msg(Comm_App_Frame *recv_frame);
    static int heart_beat_to_secsvr();

private:
    int proc_add_user(unsigned int uin, unsigned int game_id,
                      unsigned short channel_id);
    int proc_del_user(unsigned int uin, unsigned int game_id);
    int proc_cs_sdk_pkg(Comm_App_Frame *recv_frame);
    int proc_sc_sdk_pkg(Comm_App_Frame *recv_frame);
    int proc_cs_sdk_antibot_pkg(Comm_App_Frame *recv_frame,
                                sec_proto::RecvDataFromSDKReq *sdk_pkg);
    int proc_sc_sdk_antibot_pkg(Comm_App_Frame *recv_frame,
                                sec_proto::SendToSDK *sdk_pkg);
    int punish_user(unsigned int uin, unsigned int game_id,
                    unsigned short channel_id, int punish_mode);
    int proc_rpcode_info_pkg(Comm_App_Frame *recv_frame);
    int proc_notify_is_change_svr(const Comm_App_Frame *recv_frame);
    int create_rpcode_key_pkg(const SecAgentUserInfo *user_info,
                              unsigned int dib_id, unsigned short channel_id);

    template<typename T>
    int send_pkg_to_secsvr(unsigned int cmd, const SecAgentUserInfo *user_info,
                           const T &pkg);
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_MSG_HANDLER_H_H_
