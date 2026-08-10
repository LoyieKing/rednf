// Reconstructed from secagent binary DWARF + disassembly.
// Original path: src/formmog/secagent/secagent_dpsdk_info_mgr.h
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_DPSDK_INFO_MGR_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_DPSDK_INFO_MGR_H_H_

#include "output/commlib/zenlib/release/include/zen_lock_thread_mutex.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"

// sizeof = 280
struct SecAgentDPInfo { // line 22
public:
    static const unsigned int MAX_PATH_LEN = 256; // line 24
    unsigned int config_update_time_; // line 26
    unsigned int sdk_update_time_;    // line 29
    unsigned int game_id_;            // line 32
    unsigned int world_id_;           // line 35
    unsigned int secagent_id_;        // line 37
    char dp_config_path_[MAX_PATH_LEN]; // line 39
    unsigned int now_time_;           // line 42
};

// sizeof = 44
struct SecAgentDPsdkInfoMgr { // line 46
public:
    typedef secagent_config::config *LPCONFIG; // line 225 (formmog_conf_secagent.h)
    ZEN_Thread_Mutex thread_mutex_;   // line 60 @0
    ZEN_ShareMem_Mmap dp_info_mmap_;  // line 62 @28
    SecAgentDPsdkInfoMgr();           // line 48
    ~SecAgentDPsdkInfoMgr();          // line 49
    int init(LPCONFIG conf);          // line 52
    int update_config(LPCONFIG conf); // line 54
    void update_time(unsigned int now_time); // line 55
private:
    SecAgentDPsdkInfoMgr(const SecAgentDPsdkInfoMgr &arg0); // line 65
    SecAgentDPsdkInfoMgr & operator=(const SecAgentDPsdkInfoMgr &arg0); // line 66
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_DPSDK_INFO_MGR_H_H_
