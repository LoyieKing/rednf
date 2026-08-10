// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_dpsdk_info_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_DPSDK_INFO_MGR_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_DPSDK_INFO_MGR_H_H_

#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"

// sizeof = 44
struct SecAgentDPsdkInfoMgr { // line 46
public:
ZEN_Thread_Mutex thread_mutex_;
ZEN_ShareMem_Mmap dp_info_mmap_;
SecAgentDPsdkInfoMgr(); // line 48
~SecAgentDPsdkInfoMgr(); // line 49
static int init(); // line 52
int update_config(SecAgentDPsdkInfoMgr::LPCONFIG arg0); // line 54
void update_time(unsigned int arg0); // line 55
private:
SecAgentDPsdkInfoMgr(const SecAgentDPsdkInfoMgr &arg0); // line 65
SecAgentDPsdkInfoMgr & operator=(const SecAgentDPsdkInfoMgr &arg0); // line 66
};
// sizeof = 280
struct SecAgentDPInfo { // line 22
public:
static const unsigned int MAX_PATH_LEN;
unsigned int config_update_time_;
unsigned int sdk_update_time_;
unsigned int game_id_;
unsigned int world_id_;
unsigned int secagent_id_;
char dp_config_path_[];
unsigned int now_time_;
};
extern const unsigned int MAX_PATH_LEN; // line 24


// inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_DPSDK_INFO_MGR_H_H_
