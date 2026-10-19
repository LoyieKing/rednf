// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/adapter/release/include/antibotinterface.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_ANTIBOTINTERFACE_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_ANTIBOTINTERFACE_H_H_

#include "src/formmog/tss_sdk/adapter/iantibotinterface.h"

// sizeof = 4
struct AntibotInterface : public IAntibotInterface { // line 12
public:
AntibotInterface(const AntibotInterface &arg0);
AntibotInterface(); // line 282
virtual ~AntibotInterface(); // line 285
virtual int Initialize(IBusiness *busi, char *ipc_path_name); // line 290
virtual int AddClient(int channel_id, int room_id, unsigned int uin, unsigned int cli_ver, TUserInfo *user_info); // line 365
virtual int DeleteClient(int channel_id, int room_id, unsigned int uin); // line 423
virtual int OnRecvBufferFromClient(int channel_id, int room_id, unsigned int uin, unsigned char *buffer, short unsigned int buffer_size); // line 464
virtual int ProcessAntiBotInput(); // line 599
virtual int CheckTimer(int channel_id, int room_id, unsigned int uin); // line 494
virtual void SetLogPath(char *log_base_path); // line 718
virtual void ShowInfo(); // line 724
virtual int P2PAddClient(unsigned int uin, int user_id, unsigned int cli_ver); // line 443
virtual int P2PDeleteClient(unsigned int uin); // line 449
virtual int RecvBufferForEncrypt(unsigned int key, unsigned char *input_buffer, unsigned int input_buffer_size, unsigned char *output_buffer, unsigned int &output_buffer_size); // line 505
virtual int RecvBufferForDecrypt(unsigned int key, unsigned char *input_buffer, short unsigned int input_buffer_size, unsigned char *output_buffer, short unsigned int &output_buffer_size); // line 555
virtual int IsCmdInUpList(unsigned int key, int cmd_id); // line 612
virtual int IsCmdInDownList(unsigned int key, int cmd_id); // line 652
virtual int OnNotifyServer(int channel_id, int room_id, unsigned int main_id, unsigned char *buffer, short int buffer_size); // line 683
virtual int AntiCheatPacket(unsigned int key, int cmd_id); // line 730
virtual int QueryProtectPolicy(int channel_id, int room_id, unsigned int uin, SSafeModePkg *safemode_pkg); // line 759
virtual int ForwardData(int game_id, int room_id, unsigned int uin, unsigned int dest_svr_id, unsigned int data_len, const unsigned char *data); // line 793
};


extern IAntibotInterface * GetAntiBotInterfaceAll(); // line 27 (inferred)
extern int process_safemode_cb_(void *sec_data); // line 241 (inferred)
extern int process_safemode_mba_set_safe_status_cb_(void *sec_data); // line 182 (inferred)
extern int process_safemode_mba_set_verify_options_cb_(void *sec_data); // line 197 (inferred)
extern int process_safemode_mba_set_verify_status_cb_(void *sec_data); // line 219 (inferred)
extern int process_safemode_msg_cb_(void *sec_data); // line 143 (inferred)
extern int process_safemode_set_limit_status_cb_(void *sec_data); // line 156 (inferred)
extern int punish_user_cb_(void *sec_data); // line 106 (inferred)
extern int send_buffer_to_client_cb_(void *sec_data); // line 94 (inferred)

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_ANTIBOTINTERFACE_H_H_
