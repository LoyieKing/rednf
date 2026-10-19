// Semantic reconstruction of tss_sdk_impl::UserMgr from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0, -O2 -fPIC).
//
// Original translation unit:
//   src/formmog/tss_sdk/plugins/user_control/user_mgr.cpp
// Original header:
//   src/formmog/tss_sdk/plugins/user_control/user_mgr.h
//
// ORIG ranges (half-open, from the DWARF subprogram high_pc values):
//   UserMgr::~UserMgr() D2                 0x000ce310..0x000ce34d
//   UserMgr::UserMgr() C1                  0x000ce350..0x000ce38b
//   UserMgr::UserMgr() C2                  0x000ce390..0x000ce3cb
//   UserMgr::~UserMgr() D1                 0x000ce900..0x000ce93d
//   UserMgr::add_user(uint, ushort, ushort)  0x000ce3d0..0x000ce8f8
//   UserMgr::del_user(UserInfo*)             0x000ce070..0x000ce307
//   UserMgr::check(on_user_check, void*, long) 0x000ce940..0x000ceaaa
//   UserMgr::get_user(uint, ushort, ushort)   0x000ceab0..0x000cebe1
//   UserMgr::get_user_info(uint&, ushort&, ushort&, uint)
//                                             0x000cebf0..0x000cec88
//   UserMgr::init(char const*, uint, int)    0x000cec90..0x000cf20f
//
// UserMgr object (DWARF, 40 bytes):
//   +0x00 ZEN_LIB::shm_hashtable_expire<...>* user_info_hash_table_
//   +0x04 time_t                              last_check_time_
//   +0x08 int                                 use_idx_
//   +0x0c ZEN_Thread_Mutex                    user_info_hash_table_mutex_
// The constructor zeroes +0x00/+0x04 and constructs the mutex; it does NOT
// initialize use_idx_ (+0x08).  init() is the only writer of +0x08.
// No method below locks the mutex; the original only constructs/destroys it.
//
// The user table is the ZEN shared-memory hash container.  Its memory layout
// is reproduced field-for-field:
//   base + 0x00  _hashtable_expire_head { size_of_mmap_, num_of_node_,
//                                         sz_freenode_, sz_usenode_,
//                                         sz_useindex_ }        (20 bytes)
//   base + 0x14  int  hash_factor_[P]        bucket heads, -1 = empty
//   base + 0x14+4P int hash_index_[P]        bucket chain, -1 = end
//   base + 0x14+8P _shm_list_index lst_[P+2] node links; [P] = use sentinel,
//                                             [P+1] = free sentinel
//   base + 0x24+16P int priority_[P]
//   base + 0x24+20P UserInfo value_[P]       stride 400
//   allocated size  0x24 + 424*P             (4*P bytes beyond value_[P])
// The out-of-line wrapper object is 36 bytes and stores raw pointers into the
// mapping; its constructor is inlined in init and its destructor is simply
// operator delete().
//
// External boundaries (declarations emit the same mangled references):
//   ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)
//   ZEN_Trace_LogMsg::debug_errorex(char const*, ...)
//   ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)
//   ZEN_Thread_Mutex::~ZEN_Thread_Mutex()
//   ZEN_ShareMem_Mmap (16 bytes) and static s_dp_user_info_
//   tss_dp_comm::get_comm_mem(ZEN_ShareMem_Mmap&, char const*, uint)
//   tss_dp_comm::DP_CONFIG::glb_dp_config_
//   tss_sdk_impl::UserInfo::init(uint, ushort, ushort, long)
// ---------------------------------------------------------------------------

#include <new>
#include <stddef.h>
#include <string.h>
#include <time.h>

// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h); the local stub is gone.
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/closing/dp_config.h"
// ZEN_LIB shared-memory + mutex models (canonical tag-394 heads).
#include "src/commlib/zenlib/zen_shm_models.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

// ---------------------------------------------------------------------------
// ZEN_ShareMem_Mmap (DWARF 16 bytes).  Only its address is passed to
// tss_dp_comm::get_comm_mem, so the exact layout is an integration boundary;
// no canonical tag-394 header models it yet.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// tss_dp_comm shared configuration.  Only the members read by UserMgr are
// named; the offsets come from DWARF.
// ---------------------------------------------------------------------------
namespace tss_dp_comm {



// Size 22842 (0x593a).  check_time lands at +0x148.


void *get_comm_mem(ZEN_ShareMem_Mmap &mem, const char *name,
                   unsigned int size);

}  // namespace tss_dp_comm

// .bss 0x002c5620, 16 bytes, local symbol "s_dp_user_info_".
static ZEN_ShareMem_Mmap s_dp_user_info_;

namespace tss_sdk_impl {
struct UserInfo;
}

// ---------------------------------------------------------------------------
// ZEN_LIB shared-memory hash container: the base/head types come from
// src/commlib/zenlib/zen_shm_models.h; only the tag-394 instantiation this
// unit consumes is spelled out here.
// ---------------------------------------------------------------------------
namespace ZEN_LIB {
// DWARF: 20 bytes.
struct _hashtable_expire_head {
    unsigned int size_of_mmap_;  // +0
    unsigned int num_of_node_;   // +4
    unsigned int sz_freenode_;   // +8
    unsigned int sz_usenode_;    // +0xc
    unsigned int sz_useindex_;   // +0x10
};

// DWARF: 8 bytes.
struct _shm_list_index {
    int idx_next_;  // +0
    int idx_prev_;  // +4
};

// Partial model of
//   shm_hashtable_expire<UserInfo, unsigned long long,
//                        smem_hash<unsigned long long>,
//                        ExtractUserInfo, std::equal_to<...>, TimeOutProc>
// sizeof = 36 (DWARF byte_size).
struct shm_hashtable_expire : public _shm_memory_base {
    _hashtable_expire_head *lru_hash_head_;  // +0x04 (points at base)
    int *hash_factor_base_;                  // +0x08
    int *hash_index_base_;                   // +0x0c
    _shm_list_index *lst_index_base_;        // +0x10
    _shm_list_index *lst_free_node_;         // +0x14 (index P+1)
    _shm_list_index *lst_use_node_;          // +0x18 (index P)
    int *priority_base_;                          // +0x1c
    tss_sdk_impl::UserInfo *value_base_;          // +0x20
};

}  // namespace ZEN_LIB

#include "src/formmog/tss_sdk/layout/user_info.h"

namespace tss_sdk_impl {

typedef int (*on_user_check)(UserInfo *user, void *userdata, long now);

// sizeof = 40 (DWARF).
struct UserMgr {
    UserMgr();   // C1 0x000ce350, C2 0x000ce390
    ~UserMgr();  // D1 0x000ce900, D2 0x000ce310

    int init(const char *shm_name, unsigned int count, int use_idx);
    int add_user(unsigned int uin, unsigned short game_id,
                 unsigned short room_id);
    int del_user(UserInfo *user);
    int check(on_user_check func, void *userdata, long now);
    UserInfo *get_user(unsigned int uin, unsigned short game_id,
                       unsigned short room_id);
    UserInfo *get_user_info(unsigned int &uin, unsigned short &game_id,
                            unsigned short &room_id, unsigned int idx);

private:
    ZEN_LIB::shm_hashtable_expire *user_info_hash_table_;  // +0x00
    time_t last_check_time_;                                // +0x04
    int use_idx_;                                           // +0x08
    ZEN_Thread_Mutex user_info_hash_table_mutex_;           // +0x0c
};

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Inlined key/hash helpers of the ZEN container.
//
// ExtractUserInfo builds the 64-bit key as
//   (uin << 32) + ((game_id & 0xffff) << 16) + room_id
// and smem_hash folds the two 32-bit halves with a wrapping add.  Both are
// inlined at every call site in the original; the static helpers below are
// file-local and have no out-of-line counterpart in libtss_sdk.
// ---------------------------------------------------------------------------
namespace {

using tss_sdk_impl::UserInfo;

// First prime >= count.  The original list has 70 entries and falls back to
// the last one when count exceeds 0xfffffffb.  The array is the .rodata list
// at 0x00220080.
const unsigned int kShmPrimes[70] = {
    53u, 97u, 193u, 389u, 769u, 1543u, 3079u, 6151u, 12289u, 21101u,
    31957u, 42953u, 53657u, 63761u, 73009u, 83207u, 93199u, 101051u,
    111187u, 120851u, 130699u, 141283u, 150383u, 161503u, 170899u,
    181639u, 191657u, 203431u, 252817u, 310823u, 408869u, 510989u,
    603881u, 710531u, 803171u, 909613u, 1008853u, 1220027u, 1572869u,
    1820009u, 2120009u, 3145739u, 4220039u, 4620001u, 5120029u,
    6291469u, 7320011u, 8120023u, 9320009u, 12582917u, 21200021u,
    31200007u, 40200071u, 50331653u, 65200013u, 74200013u, 83200009u,
    90200017u, 100663319u, 201326611u, 312000019u, 402653189u,
    512000009u, 622000003u, 732000011u, 805306457u, 922000007u,
    1610612741u, 3221225473u, 4294967291u};

// Layout checks (compile-time).
typedef char check_userinfo_size[sizeof(tss_sdk_impl::UserInfo) == 400 ? 1
                                                                        : -1];
typedef char check_baseinfo_size[sizeof(tss_sdk_impl::UserInfoBaseInfo) ==
                                         100
                                     ? 1
                                     : -1];
typedef char check_usermgr_size[sizeof(tss_sdk_impl::UserMgr) == 40 ? 1 : -1];
typedef char check_table_size[sizeof(ZEN_LIB::shm_hashtable_expire) == 36 ? 1
                                                                          : -1];
typedef char check_dp_time_offset
    [offsetof(tss_dp_comm::DPConfig, dp_time_confg) +
             offsetof(tss_dp_comm::DPTimeConfig, check_time) ==
         0x148
     ? 1
     : -1];
typedef char check_dp_base_offset
    [offsetof(tss_dp_comm::DPConfig, dp_base_conf) +
             offsetof(tss_dp_comm::DPConfigBase, is_restore_usr_space) ==
         1
     ? 1
     : -1];

}  // namespace

namespace tss_sdk_impl {

UserMgr::UserMgr()
    : user_info_hash_table_(0),
      last_check_time_(0),
      user_info_hash_table_mutex_(0) {}

UserMgr::~UserMgr() {
    if (user_info_hash_table_ != 0)
        ::operator delete(user_info_hash_table_);
    // ~ZEN_Thread_Mutex() for user_info_hash_table_mutex_ follows.
}

int UserMgr::init(const char *shm_name, unsigned int count, int use_idx) {
    char shm_path[256];
    ::memset(shm_path, 0, sizeof(shm_path));
    ::memcpy(shm_path, "/tmp/tss_sdk_shm_user",
             sizeof("/tmp/tss_sdk_shm_user"));
    ::strncat(shm_path, shm_name, 0x100);

    unsigned int prime = kShmPrimes[69];
    for (int i = 0; i < 70; ++i) {
        if (count <= kShmPrimes[i]) {
            prime = kShmPrimes[i];
            break;
        }
    }
    // 0x24 + 400*P + 8*P + 16*P.
    const unsigned int shm_size = 0x24u + 424u * prime;

    void *base =
        tss_dp_comm::get_comm_mem(s_dp_user_info_, shm_path, shm_size);
    if (base == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "open user info shm file fail. filepath=%s, user_count=%u, "
            "shm_size=%lu.",
            shm_path, prime, static_cast<unsigned long>(shm_size));
        return -2;
    }

    ZEN_LIB::_hashtable_expire_head *head =
        static_cast<ZEN_LIB::_hashtable_expire_head *>(base);
    bool restore_ok = false;

    if (tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf
            .is_restore_usr_space != 0) {
        // The original recomputes the same prime/size before validating.
        if (head->size_of_mmap_ == shm_size &&
            head->num_of_node_ == prime) {
            head->size_of_mmap_ = shm_size;
            head->num_of_node_ = prime;
            restore_ok = true;
        } else {
            // Drop the old wrapper without deleting it, then initialize the
            // mapping from scratch (matching 0xcee4a).
            user_info_hash_table_ = 0;
        }
    } else if (user_info_hash_table_ != 0) {
        // Mapping is always attempted first; an already-created table only
        // updates the index mode.
        use_idx_ = use_idx;
        return 0;
    }

    if (!restore_ok) {
        // Fresh mapping: the header is stamped before the wrapper allocation.
        head->size_of_mmap_ = shm_size;
        head->num_of_node_ = prime;
    }

    ZEN_LIB::shm_hashtable_expire *table =
        static_cast<ZEN_LIB::shm_hashtable_expire *>(
            ::operator new(sizeof(ZEN_LIB::shm_hashtable_expire)));
    table->smem_base_ = base;
    table->lru_hash_head_ = head;
    table->hash_factor_base_ =
        reinterpret_cast<int *>(static_cast<char *>(base) + 0x14);
    table->hash_index_base_ = table->hash_factor_base_ + prime;
    table->lst_index_base_ = reinterpret_cast<ZEN_LIB::_shm_list_index *>(
        table->hash_index_base_ + prime);
    table->lst_use_node_ = table->lst_index_base_ + prime;
    table->lst_free_node_ = table->lst_index_base_ + prime + 1;
    table->priority_base_ =
        reinterpret_cast<int *>(table->lst_index_base_ + prime + 2);
    table->value_base_ =
        reinterpret_cast<UserInfo *>(table->priority_base_ + prime);

    if (!restore_ok) {
        head->sz_freenode_ = prime;
        head->sz_usenode_ = 0;
        head->sz_useindex_ = 0;

        table->lst_use_node_->idx_next_ = static_cast<int>(prime);
        table->lst_use_node_->idx_prev_ = static_cast<int>(prime);
        table->lst_free_node_->idx_next_ = static_cast<int>(prime + 1);
        table->lst_free_node_->idx_prev_ = static_cast<int>(prime + 1);

        for (unsigned int i = 0; i < prime; ++i) {
            table->hash_factor_base_[i] = -1;
            table->hash_index_base_[i] = -1;
            table->priority_base_[i] = 0;
            table->lst_index_base_[i].idx_next_ =
                static_cast<int>(i + 1);
            table->lst_index_base_[i].idx_prev_ =
                static_cast<int>(i - 1);
            if (i == 0) {
                table->lst_index_base_[0].idx_prev_ =
                    table->lst_free_node_->idx_next_;
                table->lst_free_node_->idx_next_ = 0;
            }
            if (i == prime - 1) {
                table->lst_index_base_[i].idx_next_ =
                    table->lst_free_node_->idx_prev_;
                table->lst_free_node_->idx_prev_ =
                    static_cast<int>(prime - 1);
            }
        }
    }

    user_info_hash_table_ = table;
    use_idx_ = use_idx;
    return 0;
}

int UserMgr::add_user(unsigned int uin, unsigned short game_id,
                      unsigned short room_id) {
    ZEN_Trace_LogMsg::debug_debugEx(
        "UserMgr::AddClient, uin=%u game_id=%u room_id=%u", uin, game_id,
        room_id);

    const time_t now = ::time(0);
    UserInfo user_info;
    user_info.init(uin, game_id, room_id, now);

    ZEN_LIB::shm_hashtable_expire *const table = user_info_hash_table_;

    // ExtractUserInfo key of the freshly initialized record: the full 32-bit
    // game_id_ field is shifted 16 bits into the 64-bit key.  UserInfo::init
    // stores game_id_ zero-extended from unsigned short, so its high half is
    // normally zero.
    const unsigned long long key =
        (static_cast<unsigned long long>(user_info.base_info_.uin_) << 32) +
        (static_cast<unsigned long long>(user_info.base_info_.game_id_)
         << 16) +
        static_cast<unsigned long long>(user_info.base_info_.room_id_);
    const unsigned int bucket =
        (static_cast<unsigned int>(key >> 32) +
         static_cast<unsigned int>(key)) %
        table->lru_hash_head_->num_of_node_;

    // Inlined shm_hashtable_expire::insert duplicate scan.
    int found = -1;
    for (int node = table->hash_factor_base_[bucket]; node != -1;
         node = table->hash_index_base_[node]) {
        const UserInfo &stored = table->value_base_[node];
        const unsigned long long stored_key =
            (static_cast<unsigned long long>(stored.base_info_.uin_) << 32) +
            (static_cast<unsigned long long>(stored.base_info_.game_id_)
             << 16) +
            static_cast<unsigned long long>(stored.base_info_.room_id_);
        if (key == stored_key) {
            found = node;
            break;
        }
    }

    // insert() returned second == false: either the key already exists or
    // create_node() failed because the free list is empty.  The user code
    // then calls find(key) again, rebuilding the key from the arguments.
    if (found != -1 || table->lru_hash_head_->sz_freenode_ == 0) {
        const unsigned long long arg_key =
            (static_cast<unsigned long long>(uin) << 32) +
            (static_cast<unsigned long long>(game_id) << 16) +
            static_cast<unsigned long long>(room_id);
        const unsigned int find_bucket =
            (static_cast<unsigned int>(arg_key >> 32) +
             static_cast<unsigned int>(arg_key)) %
            table->lru_hash_head_->num_of_node_;

        found = -1;
        for (int node = table->hash_factor_base_[find_bucket]; node != -1;
             node = table->hash_index_base_[node]) {
            const UserInfo &stored = table->value_base_[node];
            const unsigned long long stored_key =
                (static_cast<unsigned long long>(stored.base_info_.uin_)
                 << 32) +
                (static_cast<unsigned long long>(
                     stored.base_info_.game_id_)
                 << 16) +
                static_cast<unsigned long long>(
                    stored.base_info_.room_id_);
            if (arg_key == stored_key) {
                found = node;
                break;
            }
        }
        if (found == -1)
            return -1;

        // User already exists: overwrite the 400-byte record in place.
        // Counters, priority and LRU position are not touched.
        ::memcpy(&table->value_base_[found], &user_info, sizeof(UserInfo));
        if (found > 200000000)
            return -2;
        return found + 1;
    }

    const int newnode = table->lst_free_node_->idx_next_;
    ZEN_LIB::_shm_list_index *const lst = table->lst_index_base_;

    // Remove newnode from the free list.
    const int next_free = lst[newnode].idx_next_;
    table->lst_free_node_->idx_next_ = next_free;
    lst[next_free].idx_prev_ = lst[newnode].idx_prev_;

    // Insert newnode at the head of the use list.
    const int first_use = table->lst_use_node_->idx_next_;
    lst[newnode].idx_next_ = first_use;
    lst[newnode].idx_prev_ = lst[first_use].idx_prev_;
    lst[first_use].idx_prev_ = newnode;
    table->lst_use_node_->idx_next_ = newnode;

    UserInfo *dst = table->value_base_ + newnode;
    if (dst != 0)
        ::memcpy(dst, &user_info, sizeof(UserInfo));

    table->priority_base_[newnode] = now;
    table->lru_hash_head_->sz_usenode_ += 1;
    table->lru_hash_head_->sz_freenode_ -= 1;

    if (newnode != -1) {
        // Insert at the head of the bucket chain.
        table->hash_index_base_[newnode] =
            table->hash_factor_base_[bucket];
        table->hash_factor_base_[bucket] = newnode;
        if (newnode > 200000000)
            return -2;
        return newnode + 1;
    }

    // Unreachable while sz_freenode_ != 0 (the free sentinel always yields a
    // valid node).  The original routes this iterator-invalid case back
    // through find(key), which fails and returns -1.
    return -1;
}

int UserMgr::del_user(UserInfo *user) {
    if (user == 0)
        return -1;

    // The original narrows both ids to 16 bits before building the key.
    const unsigned int uin = user->base_info_.uin_;
    const unsigned short game_id =
        static_cast<unsigned short>(user->base_info_.game_id_);
    const unsigned short room_id =
        static_cast<unsigned short>(user->base_info_.room_id_);

    ZEN_Trace_LogMsg::debug_debugEx(
        "UserMgr::DeleteClient, uin=%u, game_id=%u, room_id=%u", uin,
        game_id, room_id);

    const unsigned long long key =
        (static_cast<unsigned long long>(uin) << 32) +
        (static_cast<unsigned long long>(game_id) << 16) +
        static_cast<unsigned long long>(room_id);
    ZEN_LIB::shm_hashtable_expire *const table = user_info_hash_table_;
    const unsigned int bucket =
        (static_cast<unsigned int>(key >> 32) +
         static_cast<unsigned int>(key)) %
        table->lru_hash_head_->num_of_node_;

    const int first = table->hash_factor_base_[bucket];
    if (first == -1)
        return -2;

    int node = first;
    int prev = -1;
    while (node != -1) {
        const UserInfo &stored = table->value_base_[node];
        const unsigned long long stored_key =
            (static_cast<unsigned long long>(stored.base_info_.uin_) << 32) +
            (static_cast<unsigned long long>(stored.base_info_.game_id_)
             << 16) +
            static_cast<unsigned long long>(stored.base_info_.room_id_);
        if (key == stored_key)
            break;
        prev = node;
        node = table->hash_index_base_[node];
    }
    if (node == -1)
        return -2;

    // Unlink from the hash bucket.
    if (node == first)
        table->hash_factor_base_[bucket] =
            table->hash_index_base_[node];
    else
        table->hash_index_base_[prev] = table->hash_index_base_[node];

    // Unlink from the use list.
    ZEN_LIB::_shm_list_index *const lst = table->lst_index_base_;
    const int next_use = lst[node].idx_next_;
    const int prev_use = lst[node].idx_prev_;
    lst[next_use].idx_prev_ = prev_use;
    lst[prev_use].idx_next_ = next_use;

    // Push onto the free-list head.
    const int first_free = table->lst_free_node_->idx_next_;
    lst[node].idx_next_ = first_free;
    lst[node].idx_prev_ = lst[first_free].idx_prev_;
    table->lst_free_node_->idx_next_ = node;
    lst[first_free].idx_prev_ = node;

    table->lru_hash_head_->sz_usenode_ -= 1;
    table->lru_hash_head_->sz_freenode_ += 1;
    table->priority_base_[node] = -1;
    table->hash_index_base_[node] = 0;
    return 0;
}

int UserMgr::check(on_user_check func, void *userdata, long now) {
    // Unsigned compare of the elapsed interval against check_time.
    if (static_cast<unsigned int>(now - last_check_time_) <
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.check_time)
        return 0;

    ZEN_LIB::shm_hashtable_expire *const table = user_info_hash_table_;
    const unsigned int num = table->lru_hash_head_->num_of_node_;
    for (unsigned int bucket = 0; bucket < num; ++bucket) {
        for (int node = table->hash_factor_base_[bucket]; node != -1;
             node = table->hash_index_base_[node]) {
            // The callback's int result is ignored by the original.
            func(table->value_base_ + node, userdata, now);
        }
    }

    last_check_time_ = now;
    return 0;
}

UserInfo *UserMgr::get_user(unsigned int uin, unsigned short game_id,
                            unsigned short room_id) {
    const unsigned long long key =
        (static_cast<unsigned long long>(uin) << 32) +
        (static_cast<unsigned long long>(game_id) << 16) +
        static_cast<unsigned long long>(room_id);
    ZEN_LIB::shm_hashtable_expire *const table = user_info_hash_table_;
    const unsigned int bucket =
        (static_cast<unsigned int>(key >> 32) +
         static_cast<unsigned int>(key)) %
        table->lru_hash_head_->num_of_node_;

    for (int node = table->hash_factor_base_[bucket]; node != -1;
         node = table->hash_index_base_[node]) {
        const UserInfo &stored = table->value_base_[node];
        const unsigned long long stored_key =
            (static_cast<unsigned long long>(stored.base_info_.uin_) << 32) +
            (static_cast<unsigned long long>(stored.base_info_.game_id_)
             << 16) +
            static_cast<unsigned long long>(stored.base_info_.room_id_);
        if (key == stored_key)
            return &table->value_base_[node];
    }
    return 0;
}

UserInfo *UserMgr::get_user_info(unsigned int &uin, unsigned short &game_id,
                                 unsigned short &room_id,
                                 unsigned int idx) {
    if (use_idx_ == 0)
        return get_user(uin, game_id, room_id);

    ZEN_LIB::shm_hashtable_expire *const table = user_info_hash_table_;
    if (static_cast<int>(idx) <= 0)
        return 0;
    if (idx > table->lru_hash_head_->num_of_node_)
        return 0;

    UserInfo *user = table->value_base_ + (idx - 1);
    if (user == 0)
        return 0;

    uin = user->base_info_.uin_;
    game_id = static_cast<unsigned short>(user->base_info_.game_id_);
    room_id = static_cast<unsigned short>(user->base_info_.room_id_);
    return user;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// ORIG CU user_mgr.cpp static-initialization block.
//
// Decoded ORIG order (audit/report/init_brief.md, 23 calls, target size 926):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   ZEN_ShareMem_Mmap::ZEN_ShareMem_Mmap()   (s_dp_user_info_, 0x002c5620)
//   + atexit, COMM_STAT dtor registration and the _Unwind_Resume pad.
// s_dp_user_info_ is already defined above with the ORIG name/type.
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"
