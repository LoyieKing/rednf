// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_user_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_

#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_shm_hash_expire.h"
#include "src/formmog/secagent/secagent_user_info.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

// sizeof = 320
struct SecAgentUserInfo { // line 27
    // sizeof = 16
    struct AntibotErrorCodeNum {
    public:
    unsigned int decrypt_fail_num_;
    unsigned int unecncrypt_fail_num_;
    unsigned int test_pkg_fail_num_;
    unsigned int de_log_fail_num_;
    };
    // sizeof = 136
    struct RpcodeCryptInfo {
    public:
    unsigned int dib_id_;
    unsigned int key_size;
    unsigned char key_[];
    };
    // sizeof = 152
    struct AntibotCryptInfo {
    public:
    unsigned int dib_id_;
    unsigned int key_len_;
    unsigned int key_offset_;
    unsigned char key_data_[];
    time_t test_pkg_time_;
    bool antibot_ready_encrypt_;
    unsigned int client_ver_;
    };
public:
static const unsigned int MAX_KEY_LEN;
static const unsigned int MAX_CRYPT_KEY_LENGTH;
UserBaseInfo base_info_;
unsigned int login_continue_times_;
SecAgentUserInfo::AntibotCryptInfo antibot_crypt_info_;
SecAgentUserInfo::RpcodeCryptInfo rpcode_crypt_info_;
SecAgentUserInfo::AntibotErrorCodeNum antibot_error_code_num_;
SecAgentUserInfo(); // line 69
SecAgentUserInfo(unsigned int arg0, unsigned int arg1); // line 70
~SecAgentUserInfo();
};
// sizeof = 1
struct ExtractSecAgentUserKey { // line 88
public:
uint64_t operator()(const SecAgentUserInfo &arg0); // line 90
};
// sizeof = 1
struct TimeOutProc { // line 97
public:
void operator()(const SecAgentUserInfo &arg0); // line 99
};
// sizeof = 28
struct SecAgentUserInfoMgr { // line 113
private:
shm_hashtable_expire<SecAgentUserInfo,long long unsigned int,ZEN_LIB::smem_hash<long long unsigned int>,ExtractSecAgentUserKey,std::equal_to<long long unsigned int>,TimeOutProc> *hash_tab_user_;
// anonymous struct, sizeof = 8
struct {
    protected:
    size_t serial_;
    shm_hashtable_expire<SecAgentUserInfo,long long unsigned int,ZEN_LIB::smem_hash<long long unsigned int>,ExtractSecAgentUserKey,std::equal_to<long long unsigned int>,TimeOutProc> *lruht_instance_;
    public:
    void _hashtable_expire_iterator(); // line 72
    void _hashtable_expire_iterator(unsigned int arg0, shm_hashtable_expire<SecAgentUserInfo,long long unsigned int,ZEN_LIB::smem_hash<long long unsigned int>,ExtractSecAgentUserKey,std::equal_to<long long unsigned int>,TimeOutProc> *arg1); // line 78
    void ~_hashtable_expire_iterator(); // line 84
    SecAgentUserInfo & operator*() const; // line 88
    SecAgentUserInfo * operator->() const; // line 93
    /*anon struct*/ int & operator++(); // line 101
    /*anon struct*/ int & goto_next_equal(); // line 126
    /*anon struct*/ int operator++(int arg0); // line 151
    bool operator==(const /*anon struct*/ int &arg0) const; // line 158
    bool operator!=(const /*anon struct*/ int &arg0) const; // line 169
    size_t getserial() const; // line 175
} user_info_itr_;
ZEN_ShareMem_Mmap mmap_file_;
static SecAgentUserInfoMgr *instance_;
public:
SecAgentUserInfoMgr(); // line 124
~SecAgentUserInfoMgr(); // line 125
static SecAgentUserInfoMgr * instance(); // line 127
static void clean_instance(); // line 128
int init(size_t data_num, bool if_restore); // line 132
int add_user(unsigned int uin, unsigned int game_id, short unsigned int channel_id); // line 137
int del_user(unsigned int uin, unsigned int game_id); // line 139
SecAgentUserInfo * get_user(unsigned int uin, unsigned int game_id); // line 141
int expire(const ZEN_Time_Value &now); // line 144
void active(unsigned int uin, unsigned int appid); // line 147
SecAgentUserInfo * get_first_node(); // line 150
SecAgentUserInfo * get_next_node(); // line 153
size_t get_online_user_num(); // line 155
};
extern const unsigned int MAX_KEY_LEN; // line 30
namespace SecAgentUserInfo {
    // sizeof = 16
    struct AntibotErrorCodeNum { // line 32
    public:
    unsigned int decrypt_fail_num_;
    unsigned int unecncrypt_fail_num_;
    unsigned int test_pkg_fail_num_;
    unsigned int de_log_fail_num_;
    };
    // sizeof = 136
    struct RpcodeCryptInfo { // line 43
    public:
    unsigned int dib_id_;
    unsigned int key_size;
    unsigned char key_[];
    };
    // sizeof = 152
    struct AntibotCryptInfo { // line 51
    public:
    unsigned int dib_id_;
    unsigned int key_len_;
    unsigned int key_offset_;
    unsigned char key_data_[];
    time_t test_pkg_time_;
    bool antibot_ready_encrypt_;
    unsigned int client_ver_;
    };
} // namespace SecAgentUserInfo


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_USER_MGR_H_H_
