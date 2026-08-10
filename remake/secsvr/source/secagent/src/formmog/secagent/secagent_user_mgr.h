// Reconstructed from secagent disassembly + DWARF (2026-08-10)
// Original path: src/formmog/secagent/secagent_user_mgr.h
//
// SecAgentUserInfo 320 字节：
//   base_info_(10) login_continue_times_(4) antibot_crypt_info_(152)
//   rpcode_crypt_info_(136) antibot_error_code_num_(16)
// SecAgentUserInfoMgr 28 字节：
//   hash_tab_user_@0  user_info_itr_(8)@4  mmap_file_(16)@12
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_

#include "src/commlib/zenlib/zen_share_mem_mmap.h"
#include "src/commlib/zenlib/zen_shm_hash_expire.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_user_info.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

struct SecAgentUserInfo {
public:
    static const unsigned int MAX_KEY_LEN = 128;
    static const unsigned int MAX_CRYPT_KEY_LENGTH = 128;

    // 16 字节
    struct AntibotErrorCodeNum {
        unsigned int decrypt_fail_num_;
        unsigned int unecncrypt_fail_num_;
        unsigned int test_pkg_fail_num_;
        unsigned int de_log_fail_num_;
    };

    // 136 字节
    struct RpcodeCryptInfo {
        unsigned int dib_id_;
        unsigned int key_size;
        unsigned char key_[MAX_KEY_LEN];
    };

    // 152 字节
    struct AntibotCryptInfo {
        unsigned int dib_id_;
        unsigned int key_len_;
        unsigned int key_offset_;
        unsigned char key_data_[MAX_CRYPT_KEY_LENGTH];
        time_t test_pkg_time_;
        bool antibot_ready_encrypt_;
        unsigned int client_ver_;
    };

    UserBaseInfo base_info_;                       // 0
    unsigned int login_continue_times_;            // 12
    SecAgentUserInfo::AntibotCryptInfo antibot_crypt_info_;   // 16
    SecAgentUserInfo::RpcodeCryptInfo rpcode_crypt_info_;     // 168
    SecAgentUserInfo::AntibotErrorCodeNum antibot_error_code_num_; // 304

    SecAgentUserInfo();
    SecAgentUserInfo(unsigned int uin, unsigned int appid);
};

// key = (uint64)uin_ << 32 | appid_
struct ExtractSecAgentUserKey {
    uint64_t operator()(const SecAgentUserInfo &info) {
        return ((uint64_t)info.base_info_.uin_ << 32) |
               info.base_info_.appid_;
    }
};

// 超时清洗回调（secagent 内联为空实现）
struct TimeOutProc {
    void operator()(const SecAgentUserInfo &) {}
};

struct SecAgentUserInfoMgr {
private:
    typedef ZEN_LIB::shm_hashtable_expire<
        SecAgentUserInfo, unsigned long long, ZEN_LIB::smem_hash<unsigned long long>,
        ExtractSecAgentUserKey, std::equal_to<unsigned long long>, TimeOutProc>
        user_hash_table;
    typedef ZEN_LIB::_hashtable_expire_iterator<
        SecAgentUserInfo, unsigned long long, ZEN_LIB::smem_hash<unsigned long long>,
        ExtractSecAgentUserKey, std::equal_to<unsigned long long>, TimeOutProc>
        user_hash_iterator;

    user_hash_table *hash_tab_user_;       // 0
    user_hash_iterator user_info_itr_;     // 4 (8 字节)
    ZEN_ShareMem_Mmap mmap_file_;          // 12
    static SecAgentUserInfoMgr *instance_; // .bss
public:
    SecAgentUserInfoMgr();
    ~SecAgentUserInfoMgr();
    static SecAgentUserInfoMgr *instance();
    static void clean_instance();
    int init(size_t data_num, bool if_restore);
    int add_user(unsigned int uin, unsigned int game_id,
                 unsigned short channel_id);
    int del_user(unsigned int uin, unsigned int game_id);
    SecAgentUserInfo *get_user(unsigned int uin, unsigned int game_id);
    int expire(const ZEN_Time_Value &now);
    void active(unsigned int uin, unsigned int appid);
    SecAgentUserInfo *get_first_node();
    SecAgentUserInfo *get_next_node();
    size_t get_online_user_num();
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_
