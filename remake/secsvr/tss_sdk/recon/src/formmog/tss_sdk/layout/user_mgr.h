// Canonical layout of tss_sdk_impl::UserMgr.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_sdk_impl::UserMgr'
//   struct tss_sdk_impl::UserMgr size=40 bases=0 vfuncs=0
//     +0   user_info_hash_table_  ZEN_LIB::shm_hashtable_expire<
//                                   tss_sdk_impl::UserInfo, unsigned long long,
//                                   ZEN_LIB::smem_hash<unsigned long long>,
//                                   tss_sdk_impl::ExtractUserInfo,
//                                   std::equal_to<unsigned long long>,
//                                   tss_sdk_impl::TimeOutProc>*   size=4
//     +4   last_check_time_       time_t        size=4
//     +8   use_idx_               int           size=4
//     +12  user_info_hash_table_mutex_  ZEN_Thread_Mutex  size=28
//   The model prints size 0 for the mutex because that CU references a
//   declaration DIE; the model's own ZEN_Thread_Mutex is 28 bytes, and
//   12 + 28 = 40 fixes the extent.
//
// ZEN_Thread_Mutex comes from the shared zenlib declaration; the hash table is
// only ever used through a pointer, so the template is forward declared with
// its exact ORIG argument list.
#ifndef TSS_SDK_LAYOUT_USER_MGR_H_
#define TSS_SDK_LAYOUT_USER_MGR_H_

#include <ext/functional>
#include <functional>
#include <time.h>

#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl {
struct UserInfo;
struct ExtractUserInfo;   // 1-byte key extractor
struct TimeOutProc;       // 1-byte timeout predicate
}  // namespace tss_sdk_impl

namespace ZEN_LIB {
template <class KEY>
struct smem_hash;
template <class T, class KEY, class HASH, class KEY_EXTRACT, class EQUAL,
          class TIMEOUT>
class shm_hashtable_expire;
}  // namespace ZEN_LIB

namespace tss_sdk_impl {

// sizeof = 40.
// ORIG: typedef int (*on_user_check)(UserInfo*, void*, long);
// _ZN12tss_sdk_impl7UserMgr5checkEPFiPNS_8UserInfoEPvlES3_l
typedef int (*on_user_check)(UserInfo *user, void *userdata, long now);

struct UserMgr {
    typedef ZEN_LIB::shm_hashtable_expire<
        tss_sdk_impl::UserInfo, unsigned long long,
        ZEN_LIB::smem_hash<unsigned long long>, tss_sdk_impl::ExtractUserInfo,
        std::equal_to<unsigned long long>, tss_sdk_impl::TimeOutProc>
        UserInfoHashTable;

    UserMgr();
    ~UserMgr();

    int init(const char *shm_name, unsigned int count, int use_idx);
    int add_user(unsigned int uin, unsigned short game_id,
                 unsigned short room_id);
    int del_user(UserInfo *user);
    UserInfo *get_user(unsigned int uin, unsigned short game_id,
                       unsigned short room_id);
    UserInfo *get_user_info(unsigned int &uin, unsigned short &game_id,
                            unsigned short &room_id, unsigned int idx);
    int check(on_user_check func, void *userdata, long now);

private:
    UserInfoHashTable *user_info_hash_table_;    // +0x00
    time_t last_check_time_;                     // +0x04
    int use_idx_;                                // +0x08
    ZEN_Thread_Mutex user_info_hash_table_mutex_;  // +0x0c
};

TSS_SDK_STATIC_ASSERT(sizeof(UserMgr) == 40, usermgr_size_must_be_40);

}  // namespace tss_sdk_impl

#endif  // TSS_SDK_LAYOUT_USER_MGR_H_
