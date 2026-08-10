// Reconstructed from secagent binary (DWARF layout + vtable + disassembly, 2026-08-10)
// Transaction_Manager: sizeof=120。hash_map/hash_set 用 __gnu_cxx 标准容器（GCC 4.1，
// hashtable 20 字节：vector(12)+num_elements(4)+allocator/hash/equals/get_key(4×1)）。
// 二进制使用自定义 find_or_insert（返回 value_type&），此处用 find+insert 等价实现。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_MANAGER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_MANAGER_H_H_

#include "src/commlib/framework/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_lock_null_lock.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

struct Transaction_Base;
struct Comm_App_Frame;
class ZEN_Timer_Queue;
struct Zerg_MMAP_BusPipe;

typedef ZEN_Null_Mutex ZEN_NULL_SYNCH;
typedef ZEN_LIB::lordrings<Transaction_Base*> POOL_OF_REGISTERTRANS;

// sizeof = 68
struct CREATE_TRANS_RECORD { // line 68
public:
    unsigned int trans_command_;       // 0x00
    POOL_OF_REGISTERTRANS crttrs_cmd_pool_; // 0x04 (16)
    bool if_auto_trans_lock_;          // 0x14
    unsigned int trans_lock_cmd_;      // 0x18
    uint64_t create_trans_num_;        // 0x1c
    uint64_t destroy_right_num_;       // 0x24
    uint64_t destroy_timeout_num_;     // 0x2c
    uint64_t destroy_exception_num_;   // 0x34
    uint64_t trans_consume_time_;      // 0x3c
    CREATE_TRANS_RECORD(unsigned int arg0); // line 71
    ~CREATE_TRANS_RECORD(); // line 72
};

// sizeof = 8
struct TRANS_LOCK_RECORD { // line 112
public:
    unsigned int lock_qq_uin_;         // 0x00
    unsigned int lock_trans_cmd_;      // 0x04
    TRANS_LOCK_RECORD(unsigned int arg0, unsigned int arg1); // line 121
    TRANS_LOCK_RECORD(); // line 122
    ~TRANS_LOCK_RECORD(); // line 123
};

// sizeof = 1
struct HASH_OF_TRANS_LOCK { // line 128
public:
    size_t operator()(const TRANS_LOCK_RECORD &arg0) const { // line 130
        // 二进制：index = (lock_qq_uin_ + lock_trans_cmd_) % bucket_count
        return arg0.lock_qq_uin_ + arg0.lock_trans_cmd_;
    }
};

// sizeof = 1
struct EQUAL_OF_TRANS_LOCK { // line 138
public:
    bool operator()(const TRANS_LOCK_RECORD &arg0, const TRANS_LOCK_RECORD &arg1) const { // line 140
        return arg0.lock_qq_uin_ == arg1.lock_qq_uin_
            && arg0.lock_trans_cmd_ == arg1.lock_trans_cmd_;
    }
};

// sizeof = 120
struct Transaction_Manager { // line 151
    typedef __gnu_cxx::hash_map<unsigned int, Transaction_Base *>
        transc_map_type;
    typedef __gnu_cxx::hash_map<unsigned int, CREATE_TRANS_RECORD>
        regtrans_pool_map_type;
    typedef __gnu_cxx::hash_set<TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK,
                                EQUAL_OF_TRANS_LOCK> trans_lock_pool_type;
    typedef transc_map_type::const_iterator transc_map_const_iterator;
    typedef regtrans_pool_map_type::iterator regtrans_pool_map_iterator;
    typedef regtrans_pool_map_type::const_iterator
        regtrans_pool_map_const_iterator;
protected:
    static const size_t MAX_ONCE_PROCESS_FRAME = 0x400;          // line 158
    static const size_t POOL_EXTEND_TRANSACTION_NUM = 0x400;     // line 161
    static const size_t RECYCLE_POOL_THRESHOLD_VALUE = 0x800;    // line 164
    static const size_t INIT_FRAME_MALLOC_NUMBER = 0x800;        // line 166
    static const size_t INNER_QUEUE_WATER_MARK = 0x4e2000;       // line 170
public:
    transc_map_type transc_map_;       // 0x04 (20)
    regtrans_pool_map_type regtrans_pool_map_; // 0x18 (20)
    trans_lock_pool_type trans_lock_pool_;    // 0x2c (20)
protected:
    friend struct Transaction_Base;
    size_t max_trans_;                     // 0x40
    unsigned int trans_id_builder_;        // 0x44
    SERVICES_ID self_services_id_;         // 0x48 (6)
    ZEN_Timer_Queue *timer_queue_;         // 0x50
    Zerg_MMAP_BusPipe *zerg_mmap_pipe_;    // 0x54
    const ZEN_Time_Value *statistics_clock_; // 0x58
    Comm_App_Frame *trans_send_buffer_;    // 0x5c
    Comm_App_Frame *trans_recv_buffer_;    // 0x60
    AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> *inner_frame_malloc_;   // 0x64
    ZEN_Message_Queue_Deque<ZEN_NULL_SYNCH, Comm_App_Frame*> *inner_message_queue_; // 0x68
    uint64_t gen_trans_counter_;           // 0x6c
    unsigned int cycle_gentrans_counter_;  // 0x74
public:
    Transaction_Manager(); // line 51
    ~Transaction_Manager(); // line 79
    int initialize(size_t szregtrans, size_t sztransmap, SERVICES_ID selfsvr,
                   ZEN_Timer_Queue *timer_queue, Zerg_MMAP_BusPipe *zerg_mmap_pipe,
                   unsigned int max_frame_len, bool init_inner_queue,
                   bool init_lock_pool); // line 158
    int register_trans_cmd(unsigned int cmd, Transaction_Base *ptxbase,
                           bool if_auto_lock_trans, unsigned int lock_trans_cmd); // line 211
    int regiester_trans_id(unsigned int transid, unsigned int trans_cmd,
                           Transaction_Base *ptxbase); // line 265
    int unregiester_trans_id(unsigned int transid, unsigned int trans_cmd,
                             int run_state, time_t trans_start); // line 308
    int process_pipe_frame(size_t &proc_frame, size_t &create_trans); // line 370
    int push_back_sendpipe(Comm_App_Frame *proc_frame); // line 412
    int get_clone_from_pool(unsigned int frame_cmd, unsigned int qq_uin,
                            Transaction_Base *&crt_trans); // line 435
    int return_clone_to_pool(unsigned int frame_cmd, Transaction_Base *&rt_txbase); // line 521
    void recycle_clone_from_pool(); // line 567
    int lock_qquin_trnas_cmd(unsigned int qq_uin, unsigned int trnas_lock_id,
                             unsigned int frame_cmd); // line 632
    int unlock_qquin_trans_cmd(unsigned int qq_uin, unsigned int lock_cmd); // line 651
    int process_appframe(Comm_App_Frame *ppetappframe, bool &bcrttx); // line 672
    int get_handler_by_transid(unsigned int transid, unsigned int trans_cmd,
                               Transaction_Base *&ptxbase); // line 783
    void dump_all_trans_info() const; // line 816
    void dump_trans_pool_info() const; // line 840
    void dump_statistics_info() const; // line 865
    void dump_all_debug_info() const; // line 893
    int mgr_sendmsghead_to_service(unsigned int cmd, unsigned int qquin,
                                   const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc,
                                   unsigned int backfill_trans_id, unsigned int app_id,
                                   unsigned int option); // line 907
    void enable_trans_statistics(const ZEN_Time_Value *stat_clock); // line 933
    int mgr_postframe_to_msgqueue(Comm_App_Frame *post_frame); // line 940
    int process_queue_frame(size_t &proc_frame, size_t &create_trans); // line 971
    void get_manager_load_foctor(unsigned int &load_max, unsigned int &load_cur); // line 1019
    void get_manager_load_foctor2(unsigned int &load_max, unsigned int &load_cur); // line 1033
    static Transaction_Manager * instance(); // line 1061
    static void instance(Transaction_Manager *pinstatnce); // line 1076
    static void clean_instance(); // line 1084
    int mgr_sendbuf_to_service(unsigned int cmd, unsigned int qquin,
                               unsigned int transaction_id, unsigned int backfill_trans_id,
                               const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc,
                               const SERVICES_ID &sndsvc, const unsigned char *buf,
                               size_t buf_len, unsigned int app_id,
                               unsigned int option); // line 1106
protected:
    static Transaction_Manager *instance_; // line 154
};

// 二进制 hashtable::find_or_insert 的等价实现（返回 value_type&；key 存在则返回既有值）
template<typename MAP>
static typename MAP::value_type & hash_map_find_or_insert(MAP &mp,
                                                          const typename MAP::key_type &key,
                                                          const typename MAP::mapped_type &val) {
    typename MAP::iterator it = mp.find(key);
    if (it == mp.end()) {
        it = mp.insert(std::make_pair(key, val)).first;
    }
    return *it;
}

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_MANAGER_H_H_
