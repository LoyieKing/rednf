// Reconstructed from secagent binary DWARF + disassembly (2026-08-10).
// 布局：Comm_SvrdApp_NonCtrl(0x224) + gamesvr_channel_(0x224,0x20024)
//   + conf_(0x20248) + recv_buf_(0x2024c) + dpsdk_channel_(0x20250,0x2c)
//   + msg_handler_(0x2027c,0x34) + svr_start_time_(0x202b0,8)
//   + dib_data_(0x202b8,0x190b08) + sanlix_data_(0x1b0fc0,0x1ffa0)
//   + game_exe_mgr_(0x1d0d60,12) = 0x1d0d6c
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_

#include "output/commlib/zenlib/release/include/zen_data_with_version.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/commlib/framework/comm_svrd_app_non_ctrl.h"
#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_game_exe.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_msg_handler.h"

// sizeof = 1903980 (0x1d0d6c)
struct SecAgentApp : public Comm_SvrdApp_NonCtrl { // line 28
public:
    static const unsigned int MAX_EXEDATA_LEN = 0x1400000; // line 36
    SecAgentGamesvrChannel gamesvr_channel_; // 0x224
private:
    SecAgentDPsdkInfoMgr::LPCONFIG conf_; // 0x20248
    static const size_t MAX_ONCE_PROCESS_FRAME = 2048; // line 105
    char *recv_buf_; // 0x2024c
    static SecAgentApp *instance_; // line 7
    SecAgentDPsdkInfoMgr dpsdk_channel_; // 0x20250
    SecAgentMsgHandler msg_handler_; // 0x2027c
    ZEN_Time_Value svr_start_time_; // 0x202b0
    DibData dib_data_; // 0x202b8
    AntibotSanlixData sanlix_data_; // 0x1b0fc0
    ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> > game_exe_mgr_; // 0x1d0d60
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
    const SecAgentDPsdkInfoMgr::LPCONFIG get_app_config() const { return conf_; } // line 54（内联，二进制无 out-of-line 符号）
    inline SecAgentGamesvrChannel & get_gamesvr_channel() { return gamesvr_channel_; } // line 59
    ZEN_Time_Value & get_svr_start_time() { return svr_start_time_; } // line 64（内联）
    const SecagentGameExe * get_game_exe(unsigned int version) const; // line 342
    const SecagentGameExe * get_game_exe_newest() const; // line 348
    inline const DibData * get_dib_data() { return &dib_data_; } // line 74
    inline const AntibotSanlixData * get_sanlix_data() { return &sanlix_data_; } // line 79
    inline const SecAgentDPsdkInfoMgr::LPCONFIG get_config() { return conf_; } // line 84
private:
    int load_conf(); // line 296
    int load_game_exe(); // line 222
    int load_dibdata(); // line 264
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_APP_H_H_
