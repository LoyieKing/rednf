// Semantic reconstruction of the TSS SDK UIC dirty-check and user-input
// manager from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0, -O2 -fPIC).
//
// Original translation units:
//   src/formmog/tss_sdk/plugins/uic/uic_tdirty.cpp
//   src/formmog/tss_sdk/plugins/uic/uic_user_input_info.cpp
//   src/formmog/tss_sdk/plugins/user_control/user_info.cpp
//
// ORIG symbol ranges (half-open; alignment padding ends the preceding range):
//   UicTdirty::UicTdirty() C2                    0x000c83a0..0x000c83b2
//   UicTdirty::UicTdirty() C1                    0x000c83c0..0x000c83d2
//   UicTdirty::~UicTdirty() D2                   0x000c83e0..0x000c83e5
//   UicTdirty::~UicTdirty() D1                   0x000c83f0..0x000c83f5
//   UicTdirty::uic_dirty_replace(int, uchar*, int*)  0x000c8810..0x000c88d6
//   UicTdirty::uic_dirty_replace(uchar*)         0x000c88e0..0x000c8939
//   UicTdirty::uic_dirty_check(int, uchar*, int*, MsgResultFlag*)
//                                                0x000c8940..0x000c8a1c
//   UicTdirty::uic_dirty_check(uchar*, MsgResultFlag*)
//                                                0x000c8a20..0x000c8a89
//   UicTdirty::init(char*, unsigned, UserInputCode)
//                                                0x000c8a90..0x000c8afc
//   UicTdirty::instance()                        0x000c8b00..0x000c8b5d
//
//   UicUserInputInfoMgr::UicUserInputInfoMgr() C2      0x000c8b60..0x000c8b9f
//   UicUserInputInfoMgr::UicUserInputInfoMgr() C1      0x000c8ba0..0x000c8bdf
//   UicUserInputInfoMgr::~UicUserInputInfoMgr() D2     0x000c8be0..0x000c8be5
//   UicUserInputInfoMgr::~UicUserInputInfoMgr() D1     0x000c8bf0..0x000c8bf5
//   UicUserInputInfoMgr::reset_count()                 0x000c8c00..0x000c8c34
//   UicUserInputInfoMgr::print_info()                  0x000c9050..0x000c90a3
//   UicUserInputInfoMgr::get_now_time()                0x000c90b0..0x000c9103
//   UicUserInputInfoMgr::get_user_num()                0x000c9110..0x000c9169
//   UicUserInputInfoMgr::fini()                        0x000c9170..0x000c91dd
//   UicUserInputInfoMgr::on_recv_check_result(
//       sec_proto::UserInputCheckPkg const*)           0x000c91e0..0x000c95ed
//   UicUserInputInfoMgr::init(unsigned, callback)      0x000c95f0..0x000c9975
//   UicUserInputInfoMgr::add_user_input(
//       UserMessage const*, unsigned*)                 0x000c9980..0x000c9e09
//   UicUserInputInfoMgr::expire_user_input()           0x000c9e10..0x000ca0eb
//
//   UserInfo::init(unsigned, unsigned short, unsigned short, long)
//                                                      0x000cdb80..0x000cdc04
//
// Object layouts (DWARF)
// ---------------------------------------------------------------------------
// UicTdirty, size 8:
//   +0x00 sec_proto::UserInputCode code_
//   +0x04 bool                    is_inited_
//
// UicUserInputInfoMgr, size 32:
//   +0x00 unsigned char*                     uic_user_info_hashmap_buf_
//   +0x04 shm_hashtable_expire<...>*         uic_user_info_hashmap_
//   +0x08 BuddyAlloc*                        user_info_mem_pool_
//   +0x0c TssSdkPluginCallbackProc           callback_proc_
//   +0x10 volatile unsigned int              sequence_
//   +0x14 volatile unsigned int              add_input_num_
//   +0x18 volatile unsigned int              recv_input_total_num_
//   +0x1c volatile unsigned int              recv_input_valid_num_
//
// UicUserInputInfo, size 36:
//   +0x00 unsigned int        uin_
//   +0x04 unsigned int        seq_
//   +0x08 int                 input_type_
//   +0x0c int                 msg_result_ (E_MSG_RESULT_FLAG)
//   +0x10 char                if_replace_
//   +0x14 unsigned char*      message_
//   +0x18 int                 message_len_
//   +0x1c unsigned char*      callback_data_
//   +0x20 int                 callback_data_len_
//
// Shared-memory hash container, allocated as 0x24 + 60*P bytes:
//   base + 0x00  _hashtable_expire_head { size_of_mmap_, num_of_node_,
//                                         sz_freenode_, sz_usenode_,
//                                         sz_useindex_ }             (20 bytes)
//   base + 0x14  int               hash_factor_[P]   bucket heads, -1 = empty
//   base + 0x14+4P  int            hash_index_[P]    chain next, -1 = end
//   base + 0x14+8P  _shm_list_index lst_[P+2]        [P]   = use sentinel,
//                                                    [P+1] = free sentinel
//   base + 0x24+16P unsigned int   priority_[P]
//   base + 0x24+20P UicUserInputInfo value_[P]       stride 36
// The allocator adds 4*P bytes of unused tail slack (60*P = 36*P values +
// 24*P index overhead).
//
// External boundaries (same mangled references as the original):
//   tInitializeDirtyCheck(char*, unsigned int)
//   tDirtyCheck(int, char*, int*) / tDirtyReplace(int, char*, int*)
//   utf8DirtyCheck(int, char*, int*) / utf8DirtyReplace(int, char*, int*)
//   ZEN_OS::get_uptime()
//   ZEN_Trace_LogMsg::debug_errorex(char const*, ...)
//   ZEN_Trace_LogMsg::debug_debugEx(char const*, ...)
//   BuddyAlloc::BuddyAlloc(), ~BuddyAlloc(), init, fini, alloc, free,
//   print_info
//
// Semantic notes established from the disassembly:
//   * UicTdirty::init returns 0 without ever setting is_inited_; the guard is
//     dead in this build.  This is reproduced (not "fixed").
//   * uic_dirty_replace(uchar*) and uic_dirty_check(uchar*) pass the literal
//     size 1, not strlen(), to the int-taking overload.
//   * A negative dirty-check result is reported as an error but still writes
//     MSG_NORMAL_FLAG (0) to the result out-parameter.
//   * get_now_time returns 100 ms ticks: 10 * tv_sec + tv_usec / 100000.
//     USER_INFO_EXPIRE_TIME == 10, i.e. one second.
//   * add_user_input starts a record at EMSG_NOT_OPERATE_FLAG (3) and only
//     the result packet changes it.
//   * Counters are reset when the incremented value equals 0xffffffff, not
//     when it wraps to zero.
//   * on_recv_check_result returns 0 even when the node is not found; only
//     callback/erase failures return -1.
//   * expire_user_input uses the UicTimeOutProc static callback/pool, while
//     on_recv_check_result uses this->callback_proc_/mem_pool_.

#include <new>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h); the local stub is gone.
#include "src/commlib/zenlib/zen_trace_log_msg.h"
// ZEN_LIB shared-memory container base (canonical tag-394 head).
#include "src/commlib/zenlib/zen_shm_models.h"
// BuddyAlloc (476 bytes, canonical tag-394 layout and signatures): the local
// opaque 476-byte copy is gone; init()/fini()/alloc()/free() are the ORIG
// out-of-line symbols.
#include "src/commlib/comm_buddy_alloc.h"
// tss_sdk_impl::UserInfo (400 bytes): canonical layout header.  The
// UserInfoBaseInfo/UserInfoCodeInfo/UserInfoTimer/UserInfoStat/
// UserInfoHackData models this TU used to carry are gone.
#include "src/formmog/tss_sdk/layout/user_info.h"

// ---------------------------------------------------------------------------
// Public protocol / callback types
// ---------------------------------------------------------------------------

// DWARF ._485, file formmog_proto_tss_sdk_uic.h.
enum MsgResultFlag {
    MSG_NORMAL_FLAG = 0,
    MSG_EVIL_FLAG = 1,
    MSG_DIRTY_FLAG = 2
};

// DWARF ._489, stored in UicUserInputInfo::msg_result_ and in UserMessage.
enum E_MSG_RESULT_FLAG {
    EMSG_NORMAL_FLAG = 0,
    EMSG_EVIL_FLAG = 1,
    EMSG_DIRTY_FLAG = 2,
    EMSG_NOT_OPERATE_FLAG = 3
};

// DWARF ._488 / ._491.
enum EUITYPE {
    EUITYPE_CHAT = 0,
    EUITYPE_NAME = 1,
    EUITYPE_IMAGE = 2,
    EUITYPE_VIDEO = 3
};

typedef int (*TssSdkPluginCallbackProc)(void *);

// Tencent tdirty C API (plain C linkage; symbols are not mangled).
extern "C" {
int tInitializeDirtyCheck(char *file, unsigned int size);
int tDirtyCheck(int size, char *data, int *out_len);
int tDirtyReplace(int size, char *data, int *out_len);
int utf8DirtyCheck(int size, char *data, int *out_len);
int utf8DirtyReplace(int size, char *data, int *out_len);
}

namespace sec_proto {

enum UserInputCode {
    USER_INPUT_CODE_GBK = 0,
    USER_INPUT_CODE_UTF8 = 1
};

// DWARF: UserInputCheckPkgHead is 10 bytes; the request body is 1169 bytes.
struct UserInputCheckPkgHead {
    unsigned short proto_id_;  // +0
    unsigned short version_;   // +2
    unsigned short pkg_len_;   // +4
    unsigned short cmd_;       // +6
    unsigned short game_id_;   // +8
};

struct ReqUserInputCheck {
    unsigned int uin_;                  // +0
    unsigned int session_id_;           // +4
    unsigned int session_seq_;          // +8
    signed char user_input_code_;       // +12
    unsigned short user_input_data_len_; // +13
    unsigned char user_input_data_[1024]; // +15
    unsigned short user_game_data_len_; // +1039
    unsigned char user_game_data_[128]; // +1041
} __attribute__((packed));              // 1169 bytes

struct RspUserInputCheck {
    unsigned int uin_;          // +0
    unsigned int session_id_;   // +4
    unsigned int session_seq_;  // +8
    unsigned int check_result_; // +12
};

// ORIG DWARF: the arm union has no DW_AT_name (anonymous) and no third
// `raw_` member; both arms promote into the class scope, which is how the
// decoded packet is read below.
struct UserInputCheckPkgBody {
    union {
        ReqUserInputCheck req_user_input_check_;
        RspUserInputCheck rsp_user_input_check_;
    } __attribute__((packed));
} __attribute__((packed));

struct UserInputCheckPkg {
    UserInputCheckPkgHead head_;  // +0, 10 bytes
    UserInputCheckPkgBody body_;  // +10
} __attribute__((packed));        // 1179 bytes, body at +0x0a

}  // namespace sec_proto

// Global UserMessage, 40 bytes.  add_user_input() reads only the fields below;
// game_data_ is present in the ABI but not copied or freed by this unit.
struct UserMessage {
    unsigned int uin_;              // +0
    int input_type_;                // +4
    int msg_result_flag_;           // +8
    char if_replace_;               // +12
    unsigned int message_len_;      // +16
    unsigned char *message_;        // +20
    unsigned short callback_data_len_; // +24
    unsigned char *callback_data_;  // +28
    unsigned short game_data_len_;  // +32
    unsigned char *game_data_;      // +36
};

// ---------------------------------------------------------------------------
// External classes and helpers
// ---------------------------------------------------------------------------

// Buddy allocator used for the per-message buffers: canonical tag-394 class
// (src/commlib/comm_buddy_alloc.h, 476 bytes), included above.

namespace ZEN_OS {

// Returns an 8-byte timeval by hidden sret pointer under the i386 ABI.
struct ZEN_TimeVal {
    long tv_sec;   // +0
    long tv_usec;  // +4
};

ZEN_TimeVal get_uptime();

}  // namespace ZEN_OS
namespace tss_sdk_impl {

struct UicUserInputInfo;
struct UicUserInputInfoMgr;
class UicTdirty;
struct UserInfo;

// The timeout callback state is a set of three globals in .bss; the inlined
// UIC code assigns them directly instead of calling the setters.
struct UicTimeOutProc {
    static TssSdkPluginCallbackProc callback_proc_;
    static BuddyAlloc *user_info_mem_pool_;
    static unsigned int timeout_nodes_num_;
};

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// ZEN shared-memory hash table: the base/head types come from
// src/commlib/zenlib/zen_shm_models.h; only the tag-394 instantiation this
// unit consumes is spelled out here.
// ---------------------------------------------------------------------------
namespace ZEN_LIB {
struct _hashtable_expire_head {
    unsigned int size_of_mmap_;  // +0
    unsigned int num_of_node_;   // +4
    unsigned int sz_freenode_;   // +8
    unsigned int sz_usenode_;    // +0xc
    unsigned int sz_useindex_;   // +0x10
};

struct _shm_list_index {
    int idx_next_;  // +0
    int idx_prev_;  // +4
};

struct shm_hashtable_expire : public _shm_memory_base {
    _hashtable_expire_head *lru_hash_head_;       // +0x04 (points at base)
    int *hash_factor_base_;                       // +0x08
    int *hash_index_base_;                        // +0x0c
    _shm_list_index *lst_index_base_;             // +0x10
    _shm_list_index *lst_free_node_;              // +0x14 (index P+1)
    _shm_list_index *lst_use_node_;               // +0x18 (index P)
    unsigned int *priority_base_;                 // +0x1c
    tss_sdk_impl::UicUserInputInfo *value_base_;  // +0x20
};

}  // namespace ZEN_LIB

namespace tss_sdk_impl {

// DWARF: 36 bytes.
struct UicUserInputInfo {
    unsigned int uin_;          // +0x00
    unsigned int seq_;          // +0x04
    int input_type_;            // +0x08
    int msg_result_;            // +0x0c
    char if_replace_;           // +0x10
    unsigned char *message_;    // +0x14
    int message_len_;           // +0x18
    unsigned char *callback_data_;  // +0x1c
    int callback_data_len_;     // +0x20
};

// Callback payload passed to TssSdkPluginCallbackProc by on_recv/expire.
// DWARF ._490, size 28 (file formmog_proto_tss_sdk_uic.h).
struct UicChatJudgeResult {
    unsigned int uin_;              // +0
    int msg_result_flag_;           // +4
    unsigned int message_len_;      // +8
    unsigned char *message_;        // +12
    unsigned short callback_data_len_; // +16
    unsigned char *callback_data_;  // +20
    int input_type_;                // +24
};

// tss_sdk_impl::UserInfo (400 bytes) and its nested aggregates come from
// layout/user_info.h (included above); the local models are gone.
// UserInfo::init(unsigned int, unsigned short, unsigned short, long) is
// declared there (this TU defines the ORIG out-of-line symbol).

// ---------------------------------------------------------------------------
// tss_sdk_impl::UicTdirty
// ---------------------------------------------------------------------------
class UicTdirty {
public:
    ~UicTdirty();

    static UicTdirty *instance();
    int init(char *file, unsigned int size, sec_proto::UserInputCode code);

    int uic_dirty_check(int size, unsigned char *data, int *out_len,
                        MsgResultFlag *result);
    int uic_dirty_check(unsigned char *data, MsgResultFlag *result);
    int uic_dirty_replace(int size, unsigned char *data, int *out_len);
    int uic_dirty_replace(unsigned char *data);

private:
    UicTdirty();

    static UicTdirty *instance_;       // .bss 0x002c3860
    sec_proto::UserInputCode code_;    // +0
    bool is_inited_;                   // +4
};

// ---------------------------------------------------------------------------
// tss_sdk_impl::UicUserInputInfoMgr
// ---------------------------------------------------------------------------
struct UicUserInputInfoMgr {
    UicUserInputInfoMgr();
    ~UicUserInputInfoMgr();

    int init(unsigned int user_count, TssSdkPluginCallbackProc callback);
    int fini();
    int add_user_input(const UserMessage *message, unsigned int *out_seq);
    int on_recv_check_result(const sec_proto::UserInputCheckPkg *pkg);
    int expire_user_input();
    size_t get_user_num();
    unsigned int get_now_time();

private:
    static const unsigned int USER_INFO_MEM_POOL_MAX_SIZE = 0x200000;
    static const int USER_INFO_EXPIRE_TIME = 10;
    static const unsigned int USER_INFO_UNSIGNED_INT_MAX = 0xffffffffu;

    void reset_count();
    void print_info();

    unsigned char *uic_user_info_hashmap_buf_;          // +0x00
    ZEN_LIB::shm_hashtable_expire *uic_user_info_hashmap_; // +0x04
    BuddyAlloc *user_info_mem_pool_;                    // +0x08
    TssSdkPluginCallbackProc callback_proc_;            // +0x0c
    volatile unsigned int sequence_;                    // +0x10
    volatile unsigned int add_input_num_;               // +0x14
    volatile unsigned int recv_input_total_num_;        // +0x18
    volatile unsigned int recv_input_valid_num_;        // +0x1c
};

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Static storage
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {

UicTdirty *UicTdirty::instance_ = 0;

TssSdkPluginCallbackProc UicTimeOutProc::callback_proc_ = 0;
BuddyAlloc *UicTimeOutProc::user_info_mem_pool_ = 0;
unsigned int UicTimeOutProc::timeout_nodes_num_ = 0;

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Layout checks (the original DWARF sizes are part of the ABI).
// ---------------------------------------------------------------------------
namespace {

typedef char check_uic_tdirty_size
    [sizeof(tss_sdk_impl::UicTdirty) == 8 ? 1 : -1];
typedef char check_uic_mgr_size
    [sizeof(tss_sdk_impl::UicUserInputInfoMgr) == 32 ? 1 : -1];
typedef char check_uic_info_size
    [sizeof(tss_sdk_impl::UicUserInputInfo) == 36 ? 1 : -1];
typedef char check_uic_payload_size
    [sizeof(tss_sdk_impl::UicChatJudgeResult) == 28 ? 1 : -1];
typedef char check_userinfo_size
    [sizeof(tss_sdk_impl::UserInfo) == 400 ? 1 : -1];
typedef char check_userinfo_base_size
    [sizeof(tss_sdk_impl::UserInfoBaseInfo) == 100 ? 1 : -1];
typedef char check_userinfo_code_size
    [sizeof(tss_sdk_impl::UserInfoCodeInfo) == 48 ? 1 : -1];
typedef char check_userinfo_timer_size
    [sizeof(tss_sdk_impl::UserInfoTimer) == 20 ? 1 : -1];
typedef char check_userinfo_stat_size
    [sizeof(tss_sdk_impl::UserInfoStat) == 52 ? 1 : -1];
typedef char check_userinfo_hack_size
    [sizeof(tss_sdk_impl::UserInfoHackData) == 180 ? 1 : -1];
typedef char check_usermsg_size[sizeof(UserMessage) == 40 ? 1 : -1];
typedef char check_check_pkg_head_size
    [sizeof(sec_proto::UserInputCheckPkgHead) == 10 ? 1 : -1];
typedef char check_check_pkg_req_size
    [sizeof(sec_proto::ReqUserInputCheck) == 1169 ? 1 : -1];
typedef char check_check_pkg_size
    [sizeof(sec_proto::UserInputCheckPkg) == 1179 ? 1 : -1];
typedef char check_check_pkg_body_offset
    [offsetof(sec_proto::UserInputCheckPkg, body_) == 0x0a ? 1 : -1];
typedef char check_check_pkg_rsp_result_offset
    [offsetof(sec_proto::UserInputCheckPkg, body_) +
             offsetof(sec_proto::RspUserInputCheck, check_result_) == 0x16
     ? 1
     : -1];
typedef char check_hash_table_size
    [sizeof(ZEN_LIB::shm_hashtable_expire) == 36 ? 1 : -1];
typedef char check_head_size
    [sizeof(ZEN_LIB::_hashtable_expire_head) == 20 ? 1 : -1];
typedef char check_list_index_size
    [sizeof(ZEN_LIB::_shm_list_index) == 8 ? 1 : -1];

// Offsets used by UserInfo::init().
typedef char check_userinfo_up_pos
    [offsetof(tss_sdk_impl::UserInfo, user_code_info_) +
             offsetof(tss_sdk_impl::UserInfoCodeInfo, up_cmd_list_pos_) ==
         0x90
     ? 1
     : -1];
typedef char check_userinfo_down_pos
    [offsetof(tss_sdk_impl::UserInfo, user_code_info_) +
             offsetof(tss_sdk_impl::UserInfoCodeInfo, down_cmd_list_pos_) ==
         0x92
     ? 1
     : -1];
typedef char check_userinfo_timer_offset
    [offsetof(tss_sdk_impl::UserInfo, user_timer_) == 0x94 ? 1 : -1];
typedef char check_userinfo_first_timer
    [offsetof(tss_sdk_impl::UserInfo, user_timer_) +
             offsetof(tss_sdk_impl::UserInfoTimer,
                      last_check_init_pkg_time_) ==
         0x94
     ? 1
     : -1];
typedef char check_userinfo_third_timer
    [offsetof(tss_sdk_impl::UserInfo, user_timer_) +
             offsetof(tss_sdk_impl::UserInfoTimer, last_recv_stat_pkg_time_) ==
         0x9c
     ? 1
     : -1];

// First prime >= count; the table has 70 entries and the original falls back
// to 0xfffffffb when count exceeds every entry.
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

unsigned int next_prime(unsigned int count) {
    for (int i = 0; i < 70; ++i) {
        if (count <= kShmPrimes[i])
            return kShmPrimes[i];
    }
    return 0xffffffffu - 4u;  // 0xfffffffb, the original fallback
}

// ExtractUserInfoKey: ((uint64)uin << 32) | seq.
// smem_hash<unsigned long long>: fold the halves with a wrapping 32-bit add.
unsigned int uic_bucket(const unsigned int uin, const unsigned int seq,
                        const unsigned int node_count) {
    return (uin + seq) % node_count;
}

int uic_find_index(const ZEN_LIB::shm_hashtable_expire *table,
                   const unsigned int uin, const unsigned int seq) {
    const unsigned int bucket =
        uic_bucket(uin, seq, table->lru_hash_head_->num_of_node_);
    for (int index = table->hash_factor_base_[bucket]; index != -1;
         index = table->hash_index_base_[index]) {
        const tss_sdk_impl::UicUserInputInfo &node = table->value_base_[index];
        if (node.uin_ == uin && node.seq_ == seq)
            return index;
    }
    return -1;
}

// Inlined shm_hashtable_expire::erase(iterator) used by on_recv/expire.
int uic_erase_index(ZEN_LIB::shm_hashtable_expire *table,
                    const int node_index) {
    ZEN_LIB::_hashtable_expire_head *const head = table->lru_hash_head_;
    const tss_sdk_impl::UicUserInputInfo &node =
        table->value_base_[node_index];
    const unsigned int bucket =
        uic_bucket(node.uin_, node.seq_, head->num_of_node_);

    int *const chain_head = &table->hash_factor_base_[bucket];
    int previous = -1;
    int current = *chain_head;
    while (current != node_index && current != -1) {
        previous = current;
        current = table->hash_index_base_[current];
    }
    if (current == -1)
        return -1;

    if (previous == -1)
        *chain_head = table->hash_index_base_[node_index];
    else
        table->hash_index_base_[previous] =
            table->hash_index_base_[node_index];

    // Detach from the use list.
    ZEN_LIB::_shm_list_index *const list = table->lst_index_base_;
    const int next = list[node_index].idx_next_;
    const int prev = list[node_index].idx_prev_;
    list[prev].idx_next_ = next;
    list[next].idx_prev_ = prev;

    // Insert at the head of the free list.
    ZEN_LIB::_shm_list_index *const free_head = table->lst_free_node_;
    const int old_free = free_head->idx_next_;
    list[node_index].idx_next_ = old_free;
    list[node_index].idx_prev_ = list[old_free].idx_prev_;
    list[old_free].idx_prev_ = node_index;
    free_head->idx_next_ = node_index;

    --head->sz_usenode_;
    ++head->sz_freenode_;
    table->hash_index_base_[node_index] = -1;
    table->priority_base_[node_index] = 0;
    return 0;
}

// Inlined create_node(): take a node from the free list and insert it at the
// head of the use list.  Returns -1 when the free list is empty.
int uic_create_node(ZEN_LIB::shm_hashtable_expire *table) {
    ZEN_LIB::_hashtable_expire_head *const head = table->lru_hash_head_;
    if (head->sz_freenode_ == 0)
        return -1;

    ZEN_LIB::_shm_list_index *const list = table->lst_index_base_;
    ZEN_LIB::_shm_list_index *const free_head = table->lst_free_node_;
    const int node_index = free_head->idx_next_;

    // Pop the free-list head.
    const int new_free = list[node_index].idx_next_;
    free_head->idx_next_ = new_free;
    list[new_free].idx_prev_ = list[node_index].idx_prev_;

    // Push onto the use-list head.
    ZEN_LIB::_shm_list_index *const use_head = table->lst_use_node_;
    const int old_use = use_head->idx_next_;
    list[node_index].idx_next_ = old_use;
    list[node_index].idx_prev_ = list[old_use].idx_prev_;
    list[old_use].idx_prev_ = node_index;
    use_head->idx_next_ = node_index;

    ++head->sz_usenode_;
    --head->sz_freenode_;
    return node_index;
}

}  // namespace

// ---------------------------------------------------------------------------
// tss_sdk_impl::UicTdirty
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {

UicTdirty::UicTdirty() : code_(sec_proto::USER_INPUT_CODE_GBK), is_inited_(false) {
}

UicTdirty::~UicTdirty() {
}

UicTdirty *UicTdirty::instance() {
    if (instance_ == 0)
        instance_ = new (std::nothrow) UicTdirty();
    return instance_;
}

int UicTdirty::init(char *file, unsigned int size,
                    sec_proto::UserInputCode code) {
    if (is_inited_)
        return 0;

    if (tInitializeDirtyCheck(file, size) < 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::init tInitializeDirtyCheck failed");
        return -1;
    }

    // The original stores the code here but never sets is_inited_ after a
    // successful initialization (0xc8ad6..0xc8ae6).
    code_ = code;
    return 0;
}

int UicTdirty::uic_dirty_check(int size, unsigned char *data, int *out_len,
                               MsgResultFlag *result) {
    if (data == 0 || out_len == 0 || result == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_check invalid args");
        return -1;
    }

    int check_result;
    switch (code_) {
    case sec_proto::USER_INPUT_CODE_GBK:
        check_result = tDirtyCheck(size, reinterpret_cast<char *>(data),
                                   out_len);
        break;
    case sec_proto::USER_INPUT_CODE_UTF8:
        check_result = utf8DirtyCheck(size, reinterpret_cast<char *>(data),
                                      out_len);
        break;
    default:
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_check unknown user input code");
        return -1;
    }

    if (check_result > 0) {
        *result = MSG_DIRTY_FLAG;
        return 0;
    }
    if (check_result == 0) {
        *result = MSG_NORMAL_FLAG;
        return 0;
    }

    // Failure is still reported as "normal" in the out-parameter, matching
    // 0xc8a01; both GBK and UTF-8 use the tDirtyCheck error text.
    *result = MSG_NORMAL_FLAG;
    ZEN_Trace_LogMsg::debug_errorex(
        "[uic sdk]UicTdirty::uic_dirty_check tDirtyCheck failed");
    return -1;
}

int UicTdirty::uic_dirty_check(unsigned char *data, MsgResultFlag *result) {
    if (data == 0 || result == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_check invalid args");
        return -1;
    }

    int message_len = 0;
    return uic_dirty_check(1, data, &message_len, result);
}

int UicTdirty::uic_dirty_replace(int size, unsigned char *data,
                                 int *out_len) {
    if (data == 0 || out_len == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_replace invalid args");
        return -1;
    }

    int replace_result;
    switch (code_) {
    case sec_proto::USER_INPUT_CODE_GBK:
        replace_result = tDirtyReplace(size, reinterpret_cast<char *>(data),
                                       out_len);
        break;
    case sec_proto::USER_INPUT_CODE_UTF8:
        replace_result = utf8DirtyReplace(size,
                                          reinterpret_cast<char *>(data),
                                          out_len);
        break;
    default:
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_replace unknown user input code");
        return -1;
    }

    if (replace_result >= 0)
        return 0;

    ZEN_Trace_LogMsg::debug_errorex(
        code_ == sec_proto::USER_INPUT_CODE_GBK
            ? "[uic sdk]UicTdirty::uic_dirty_replace tDirtyReplace failed"
            : "[uic sdk]UicTdirty::uic_dirty_replace utf8DirtyReplace failed");
    return -1;
}

int UicTdirty::uic_dirty_replace(unsigned char *data) {
    if (data == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicTdirty::uic_dirty_replace invalid args");
        return -1;
    }

    int message_len = 0;
    return uic_dirty_replace(1, data, &message_len);
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {

int UserInfo::init(unsigned int uin, unsigned short game_id,
                   unsigned short room_id, long now) {
    ::memset(this, 0, sizeof(UserInfo));

    base_info_.game_id_ = game_id;
    base_info_.uin_ = uin;
    base_info_.room_id_ = room_id;

    user_code_info_.up_cmd_list_pos_ = -1;
    user_code_info_.down_cmd_list_pos_ = -1;

    user_timer_.last_check_init_pkg_time_ = now;
    user_timer_.last_recv_stat_pkg_time_ = now;
    user_timer_.last_send_test_pkg_time_ = now;

    return 0;
}

// ---------------------------------------------------------------------------
// tss_sdk_impl::UicUserInputInfoMgr
// ---------------------------------------------------------------------------
UicUserInputInfoMgr::UicUserInputInfoMgr()
    : uic_user_info_hashmap_buf_(0),
      uic_user_info_hashmap_(0),
      user_info_mem_pool_(0),
      callback_proc_(0),
      sequence_(0),
      add_input_num_(0),
      recv_input_total_num_(0),
      recv_input_valid_num_(0) {
}

UicUserInputInfoMgr::~UicUserInputInfoMgr() {
}

int UicUserInputInfoMgr::init(unsigned int user_count,
                              TssSdkPluginCallbackProc callback) {
    if (callback == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::init call_back == NULL");
        return -1;
    }

    const unsigned int prime = next_prime(user_count);
    // 0x24 header + 60 bytes per node (36-byte value + 24-byte index entry).
    const unsigned int alloc_size = 0x24u + 60u * prime;

    unsigned char *const base =
        new (std::nothrow) unsigned char[alloc_size];
    uic_user_info_hashmap_buf_ = base;
    if (base == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::init "
            "uic_user_info_hashmap_buf_ == NULL");
        fini();
        return -1;
    }

    ZEN_LIB::_hashtable_expire_head *const head =
        reinterpret_cast<ZEN_LIB::_hashtable_expire_head *>(base);
    head->size_of_mmap_ = alloc_size;
    head->num_of_node_ = prime;

    ZEN_LIB::shm_hashtable_expire *const table =
        static_cast<ZEN_LIB::shm_hashtable_expire *>(
            ::operator new(sizeof(ZEN_LIB::shm_hashtable_expire)));
    table->smem_base_ = base;
    table->lru_hash_head_ = head;
    table->hash_factor_base_ =
        reinterpret_cast<int *>(base + 0x14);
    table->hash_index_base_ = table->hash_factor_base_ + prime;
    table->lst_index_base_ = reinterpret_cast<ZEN_LIB::_shm_list_index *>(
        table->hash_index_base_ + prime);
    table->lst_use_node_ = table->lst_index_base_ + prime;
    table->lst_free_node_ = table->lst_index_base_ + prime + 1;
    table->priority_base_ = reinterpret_cast<unsigned int *>(
        table->lst_index_base_ + prime + 2);
    table->value_base_ = reinterpret_cast<UicUserInputInfo *>(
        table->priority_base_ + prime);

    uic_user_info_hashmap_ = table;

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

        table->lst_index_base_[i].idx_next_ = static_cast<int>(i + 1);
        table->lst_index_base_[i].idx_prev_ = static_cast<int>(i - 1);
        if (i == 0) {
            table->lst_index_base_[0].idx_prev_ =
                table->lst_free_node_->idx_next_;
            table->lst_free_node_->idx_next_ = 0;
        }
        if (i == prime - 1) {
            table->lst_index_base_[i].idx_next_ =
                table->lst_free_node_->idx_prev_;
            table->lst_free_node_->idx_prev_ = static_cast<int>(prime - 1);
        }
    }

    BuddyAlloc *const pool = new (std::nothrow) BuddyAlloc();
    user_info_mem_pool_ = pool;
    if (pool == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::init "
            "user_info_mem_pool_ == NULL");
        fini();
        return -1;
    }

    unsigned int pool_size = USER_INFO_MEM_POOL_MAX_SIZE;
    if (pool->init(&pool_size, 0x10, true) != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::init "
            "user_info_mem_pool_->init failed");
        fini();
        return -1;
    }

    UicTimeOutProc::callback_proc_ = callback;
    UicTimeOutProc::user_info_mem_pool_ = user_info_mem_pool_;
    callback_proc_ = callback;
    return 0;
}

int UicUserInputInfoMgr::fini() {
    if (user_info_mem_pool_ != 0) {
        user_info_mem_pool_->fini();
        delete user_info_mem_pool_;
        user_info_mem_pool_ = 0;
    }

    if (uic_user_info_hashmap_buf_ != 0) {
        delete[] uic_user_info_hashmap_buf_;
        uic_user_info_hashmap_buf_ = 0;
    }

    // The 36-byte shm_hashtable_expire wrapper (uic_user_info_hashmap_) is
    // intentionally not deleted or cleared here; the original leaves it.
    return 0;
}

int UicUserInputInfoMgr::add_user_input(const UserMessage *message,
                                        unsigned int *out_seq) {
    if (message == 0 || message->message_ == 0 || out_seq == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::add_user_input invalid args");
        return -2;
    }

    const unsigned int seq = ++sequence_;
    *out_seq = seq;

    const char if_replace = message->if_replace_;
    const int input_type = message->input_type_;
    const unsigned int message_len = message->message_len_;
    const unsigned short callback_data_len = message->callback_data_len_;

    unsigned char *buffer = 0;
    unsigned char *callback_data = 0;

    // Signed 32-bit check, exactly as the original add/message_len sequence.
    const int alloc_size =
        static_cast<int>(message_len + callback_data_len + 2u);
    if (alloc_size > 0) {
        buffer = static_cast<unsigned char *>(
            user_info_mem_pool_->alloc(static_cast<unsigned int>(alloc_size)));
        if (buffer == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]UicUserInputInfoMgr::add_user_input "
                "user_info_mem_pool_->alloc failed");
            return -1;
        }

        ::memcpy(buffer, message->message_, message_len);
        buffer[message_len] = 0;

        callback_data = buffer + message_len + 1;
        ::memcpy(callback_data, message->callback_data_, callback_data_len);
        buffer[message_len + 1 + callback_data_len] = 0;
    }

    const unsigned int uin = message->uin_;
    if (uic_find_index(uic_user_info_hashmap_, uin, seq) != -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::add_user_input "
            "find node before insert");
        user_info_mem_pool_->free(buffer);
        return -1;
    }

    // The original reads the timestamp after the duplicate scan and before
    // the free-list check; preserve the call target and ordering.
    const unsigned int priority = get_now_time();

    const int node_index = uic_create_node(uic_user_info_hashmap_);
    if (node_index == -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::add_user_input "
            "result.first == uic_user_info_hashmap_->end()");
        user_info_mem_pool_->free(buffer);
        return -1;
    }

    UicUserInputInfo &node =
        uic_user_info_hashmap_->value_base_[node_index];
    node.callback_data_len_ = callback_data_len;
    node.msg_result_ = EMSG_NOT_OPERATE_FLAG;
    node.message_len_ = static_cast<int>(message_len);
    node.message_ = buffer;
    node.callback_data_ = callback_data;
    node.input_type_ = input_type;
    node.if_replace_ = if_replace;
    node.seq_ = seq;
    node.uin_ = uin;

    uic_user_info_hashmap_->priority_base_[node_index] = priority;

    const unsigned int bucket =
        uic_bucket(uin, seq,
                   uic_user_info_hashmap_->lru_hash_head_->num_of_node_);
    uic_user_info_hashmap_->hash_index_base_[node_index] =
        uic_user_info_hashmap_->hash_factor_base_[bucket];
    uic_user_info_hashmap_->hash_factor_base_[bucket] = node_index;

    if (++add_input_num_ == USER_INFO_UNSIGNED_INT_MAX)
        reset_count();

    return 0;
}

int UicUserInputInfoMgr::on_recv_check_result(
    const sec_proto::UserInputCheckPkg *pkg) {
    // Request body offsets: uin +0x00, session_seq +0x08, check_result +0x0c
    // relative to body_, which starts at pkg + 0x0a.
    const unsigned int uin = pkg->body_.rsp_user_input_check_.uin_;
    const unsigned int seq = pkg->body_.rsp_user_input_check_.session_seq_;
    const unsigned int check_result =
        pkg->body_.rsp_user_input_check_.check_result_;

    if (++recv_input_total_num_ == USER_INFO_UNSIGNED_INT_MAX)
        reset_count();

    const int node_index =
        uic_find_index(uic_user_info_hashmap_, uin, seq);
    if (node_index == -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
            "can not found node");
        return 0;
    }

    if (++recv_input_valid_num_ == USER_INFO_UNSIGNED_INT_MAX)
        reset_count();

    UicUserInputInfo &node =
        uic_user_info_hashmap_->value_base_[node_index];
    node.msg_result_ = static_cast<int>(check_result);

    if (node.msg_result_ != EMSG_EVIL_FLAG) {
        MsgResultFlag dirty_result = MSG_NORMAL_FLAG;
        if (UicTdirty::instance()->uic_dirty_check(node.message_,
                                                   &dirty_result) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
                "uic_dirty_check failed");
        }
        node.msg_result_ = dirty_result;

        if (dirty_result == MSG_DIRTY_FLAG && node.if_replace_ != 0) {
            if (UicTdirty::instance()->uic_dirty_replace(node.message_) != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
                    "uic_dirty_replace failed");
            }
        }
    }

    UicChatJudgeResult payload;
    payload.uin_ = node.uin_;
    payload.msg_result_flag_ = node.msg_result_;
    payload.message_len_ = static_cast<unsigned int>(node.message_len_);
    payload.message_ = node.message_;
    payload.callback_data_len_ =
        static_cast<unsigned short>(node.callback_data_len_);
    payload.callback_data_ = node.callback_data_;
    payload.input_type_ = node.input_type_;

    if (callback_proc_ != 0) {
        const int callback_ret = callback_proc_(&payload);
        if (callback_ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
                "callback_proc_ failed, ret = %d",
                callback_ret);
        }
    } else {
        ::puts("callback_proc_ == NULL");
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
            "callback_proc_ == NULL");
    }

    user_info_mem_pool_->free(node.message_);

    if (uic_erase_index(uic_user_info_hashmap_, node_index) != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[uic sdk]UicUserInputInfoMgr::on_recv_check_result "
            "uic_user_info_hashmap_->erase failed");
        return -1;
    }

    return 0;
}

int UicUserInputInfoMgr::expire_user_input() {
    const unsigned int now = get_now_time();
    ZEN_LIB::shm_hashtable_expire *const table = uic_user_info_hashmap_;
    ZEN_LIB::_hashtable_expire_head *const head = table->lru_hash_head_;
    const unsigned int expire_time = now - USER_INFO_EXPIRE_TIME;

    int expired_count = 0;
    int node_index = table->lst_use_node_->idx_prev_;

    while (node_index != static_cast<int>(head->num_of_node_)) {
        if (expire_time < table->priority_base_[node_index])
            break;

        UicUserInputInfo &node = table->value_base_[node_index];

        MsgResultFlag dirty_result = MSG_NORMAL_FLAG;
        if (UicTdirty::instance()->uic_dirty_check(node.message_,
                                                   &dirty_result) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]UicTimeOutProc uic_dirty_check failed");
        }

        if (dirty_result == MSG_DIRTY_FLAG && node.if_replace_ != 0) {
            if (UicTdirty::instance()->uic_dirty_replace(node.message_) != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[uic sdk]UicTimeOutProc uic_dirty_replace failed");
            }
        }

        UicChatJudgeResult payload;
        payload.uin_ = node.uin_;
        payload.msg_result_flag_ = dirty_result;
        payload.message_len_ = static_cast<unsigned int>(node.message_len_);
        payload.message_ = node.message_;
        payload.callback_data_len_ =
            static_cast<unsigned short>(node.callback_data_len_);
        payload.callback_data_ = node.callback_data_;
        payload.input_type_ = node.input_type_;

        if (UicTimeOutProc::callback_proc_ != 0) {
            if (UicTimeOutProc::callback_proc_(&payload) != 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[uic sdk]UicTimeOutProc callback_proc_ failed");
            }
        } else {
            ZEN_Trace_LogMsg::debug_errorex(
                "[uic sdk]UicTimeOutProc callback_proc_ == NULL");
        }

        UicTimeOutProc::user_info_mem_pool_->free(node.message_);
        ++UicTimeOutProc::timeout_nodes_num_;
        uic_erase_index(table, node_index);
        ++expired_count;

        node_index = table->lst_use_node_->idx_prev_;
    }

    return expired_count;
}

unsigned int UicUserInputInfoMgr::get_now_time() {
    const ZEN_OS::ZEN_TimeVal now = ZEN_OS::get_uptime();
    // 100 ms ticks: 10 seconds + microseconds / 100000.
    return static_cast<unsigned int>(now.tv_sec) * 10u +
           static_cast<unsigned int>(now.tv_usec) / 100000u;
}

size_t UicUserInputInfoMgr::get_user_num() {
    if (sequence_ % 8000u == 0) {
        user_info_mem_pool_->print_info();
        print_info();
    }

    return uic_user_info_hashmap_->lru_hash_head_->sz_usenode_;
}

void UicUserInputInfoMgr::print_info() {
    ZEN_Trace_LogMsg::debug_debugEx(
        "add_input_num_ = %u, recv_input_total_num_ = %u, "
        "recv_input_valid_num_ = %u, timeout_nodes_num_ = %u\n",
        add_input_num_, recv_input_total_num_, recv_input_valid_num_,
        UicTimeOutProc::timeout_nodes_num_);
}

void UicUserInputInfoMgr::reset_count() {
    add_input_num_ = 0;
    recv_input_total_num_ = 0;
    recv_input_valid_num_ = 0;
    UicTimeOutProc::timeout_nodes_num_ = 0;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice merges three ORIG CUs:
//   uic_tdirty.cpp, uic_user_input_info.cpp, user_info.cpp
// Each carries the independent 21-call block (target size 889):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   COMM_STAT dtor registration and the _Unwind_Resume pad.
// The module-specific objects of those CUs are already defined above with
// their ORIG names/types/initializers:
//   UicTdirty::instance_              (0x002c3860)
//   UicTimeOutProc::callback_proc_    (0x002c3e40)
//   UicTimeOutProc::user_info_mem_pool_ (0x002c3e44)
//   UicTimeOutProc::timeout_nodes_num_  (0x002c3e48)
// The status array and __ioinit are TU-local: emit one real block per ORIG CU.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID uic_tdirty
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID uic_user_input_info
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID user_info
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID
