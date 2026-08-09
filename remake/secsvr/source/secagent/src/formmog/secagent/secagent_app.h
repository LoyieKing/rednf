// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_app.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_

#include "output/commlib/zenlib/release/include/zen_data_with_version.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_msg_handler.h"

// sizeof = 1903980
struct SecAgentApp : public Comm_SvrdApp_NonCtrl { // line 28
public:
static const unsigned int MAX_EXEDATA_LEN;
SecAgentGamesvrChannel gamesvr_channel_;
private:
SecAgentDPsdkInfoMgr::LPCONFIG conf_;
static const size_t MAX_ONCE_PROCESS_FRAME;
char *recv_buf_;
static SecAgentApp *instance_;
SecAgentDPsdkInfoMgr dpsdk_channel_;
SecAgentMsgHandler msg_handler_;
ZEN_Time_Value svr_start_time_;
DibData dib_data_;
AntibotSanlixData sanlix_data_;
ZenDataWithVersion<unsigned int,SecagentGameExe,std::equal_to<unsigned int> > game_exe_mgr_;
public:
SecAgentApp(const SecAgentApp &arg0);
private:
SecAgentApp(); // line 28
virtual ~SecAgentApp(); // line 34
public:
static SecAgentApp * instance(); // line 10
static void clean_instance(); // line 20
virtual int load_app_conf(); // line 282
virtual int init(); // line 41
virtual void exit(); // line 126
virtual int reload(); // line 131
void register_timer(); // line 109
virtual int process_recv_appframe(Comm_App_Frame *recv_frame); // line 141
virtual int proc(size_t &proc_data_num); // line 183
void update_sdk_time(unsigned int now_time); // line 354
const SecAgentDPsdkInfoMgr::LPCONFIG get_app_config() const; // line 54
SecAgentGamesvrChannel & get_gamesvr_channel(); // line 59
ZEN_Time_Value & get_svr_start_time(); // line 64
const SecagentGameExe * get_game_exe(unsigned int version) const; // line 342
const SecagentGameExe * get_game_exe_newest() const; // line 348
const DibData * get_dib_data(); // line 74
const AntibotSanlixData * get_sanlix_data(); // line 79
const SecAgentDPsdkInfoMgr::LPCONFIG get_config(); // line 84
private:
int load_conf(); // line 296
int load_game_exe(); // line 222
int load_dibdata(); // line 264
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_
