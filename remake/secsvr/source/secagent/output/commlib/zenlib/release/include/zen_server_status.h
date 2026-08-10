// ZEN_Server_Status 还原头文件（基于 gunnersvr DWARF + 二进制反汇编，2026-08-10）
// 布局保持 DWARF 原样（sizeof = 76）：
//   vptr(4) + stat_lock_(4) + clear_time_(4) + stat_file_(16) + stat_file_head_(4)
//   + status_stat_sandy_(4) + status_copy_mandy_(4) + conf_stat_ary_(12)
//   + statid_to_index_(20) + multi_thread_guard_(1) + initialized_(1) = 76
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#include <ext/hash_map>
#include <sstream>
#include <vector>

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_lock_base.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"
#include "src/commlib/zenlib/zen_shm_vector.h"

enum ZEN_STATUS_STATICS_TYPE {
    STATICS_INVALID_TYPE = 0,
    STATICS_PER_FIVE_MINTUES = 1,
    STATICS_PER_HOUR = 2,
    STATICS_PER_DAYS = 3,
    STATICS_ABSOLUTE_VALUE = 11,
};

// 共享内存文件头（sizeof = 16）
struct ZEN_STATUS_HEAD {
    time_t monitor_start_time_;    // 0x00
    time_t copy_time_;             // 0x04
    uint32_t report_monitor_time_; // 0x08
    time_t active_time_;           // 0x0c
};

// sizeof = 12
struct ZEN_STATUS_ITEM_ID { // line 64
public:
    unsigned int statics_id_;
    unsigned int app_id_;
    unsigned int classify_id_;
    ZEN_STATUS_ITEM_ID(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 78
    ZEN_STATUS_ITEM_ID(); // line 79
    ~ZEN_STATUS_ITEM_ID(); // line 80
    bool operator==(const ZEN_STATUS_ITEM_ID &arg0) const; // line 82
};

// 状态项 ID 哈希（oracle 0x80949fd/0x8094914 内联：
//   hash = statics_id_ + classify_id_ + (app_id_ << 8)）
struct HASH_ZEN_STATUS_ITEM_ID {
    size_t operator()(const ZEN_STATUS_ITEM_ID &stat_item_id) const {
        return stat_item_id.statics_id_ + stat_item_id.classify_id_
               + (stat_item_id.app_id_ << 8);
    }
};

// sizeof = 24
struct ZEN_STATUS_ITEM { // line 107
public:
    ZEN_STATUS_ITEM_ID item_id_;         // 0x00 (12)
    ZEN_STATUS_STATICS_TYPE statics_type_; // 0x0c
    uint64_t counter_;                   // 0x10
    ZEN_STATUS_ITEM(); // line 111
    ZEN_STATUS_ITEM(unsigned int arg0, ZEN_STATUS_STATICS_TYPE arg1); // line 113
    ~ZEN_STATUS_ITEM(); // line 114
};

// sizeof = 92（item_name_ 64 字节 + 0x58 结束符 + 对齐填充）
struct ZEN_STATUS_ITEM_WITHNAME { // line 132
public:
    ZEN_STATUS_ITEM statics_item_;      // 0x00 (24)
    char item_name_[64];                // 0x18
    char item_name_end_;                // 0x58
    ZEN_STATUS_ITEM_WITHNAME(unsigned int arg0, ZEN_STATUS_STATICS_TYPE arg1, const char *arg2); // line 138
    ZEN_STATUS_ITEM_WITHNAME(); // line 139
    ~ZEN_STATUS_ITEM_WITHNAME(); // line 140
};

static const size_t MAX_COUNTER_NAME_LEN = 64; // line 144（oracle 无符号，编译期常量）

// sizeof = 76
class ZEN_Server_Status : public ZEN_NON_Copyable { // line 186
public:
    ZEN_Server_Status(const ZEN_Server_Status &arg0);
    ZEN_Server_Status(); // line 95
    virtual ~ZEN_Server_Status(); // line 107
    int initialize(const char *stat_filename); // line 297
    int initialize(const char *stat_filename, size_t num_stat_item,
                   const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool multi_thread); // line 321
    void add_status_item(size_t num_add_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary); // line 198
    void modify_multi_thread_guard(bool multi_thread); // line 211
    int increase_by_statid(unsigned int statics_id, unsigned int app_id,
                           unsigned int classify_id, int64_t incre_value); // line 354
    int increase_by_statid(unsigned int arg0, unsigned int arg1, int64_t arg2); // line 417
    int increase_by_statid(unsigned int arg0, int64_t arg1); // line 424
    int increase_once(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 395
    int increase_once(unsigned int arg0, unsigned int arg1); // line 402
    int increase_once(unsigned int arg0); // line 408
    int set_by_statid(unsigned int statics_id, unsigned int app_id,
                      unsigned int classify_id, uint64_t set_value); // line 385
    int set_by_statid(unsigned int arg0, unsigned int arg1, uint64_t arg2); // line 433
    int set_by_statid(unsigned int arg0, uint64_t arg1); // line 440
    uint64_t get_counter(unsigned int statics_id, unsigned int app_id,
                         unsigned int classify_id); // line 418
    size_t num_of_counter(); // line 436
    uint32_t get_copy_time(); // line 443
    void check_overtime(time_t now_time); // line 451
    void copy_stat_counter(); // line 543
    void dump_all(std::vector<ZEN_STATUS_ITEM_WITHNAME> &array_status, bool dump_copy); // line 508
    void dump_status_info(std::ostringstream &strstream, bool dump_copy); // line 558
    void dump_status_info(bool dump_copy); // line 608
    void get_stat_head(ZEN_STATUS_HEAD *stat_head); // line 662
    void report_monitor_time(uint32_t report_time); // line 668
    static ZEN_Server_Status * instance(); // line 677
    static void instance(ZEN_Server_Status *pinstatnce); // line 688
    static void clean_instance(); // line 696

protected:
    static const size_t MAX_MONITOR_STAT_ITEM = 0x10000; // line 190
    static const time_t FIVE_MINTUE_SECONDS = 300;       // line 192
    static const time_t ONE_HOURS_SECONDS = 3600;        // line 194
    static const time_t ONE_DAY_SECONDS = 86400;         // line 196
    static const uint64_t INCREASE_VALUE_ONCE = 1;       // line 347（oracle 无符号）
    ZEN_Lock_Base *stat_lock_;                            // 0x04
    time_t clear_time_;                                   // 0x08
    ZEN_ShareMem_Posix stat_file_;                        // 0x0c
    ZEN_STATUS_HEAD *stat_file_head_;                     // 0x1c
    ZEN_LIB::shm_vector<ZEN_STATUS_ITEM> *status_stat_sandy_; // 0x20
    ZEN_LIB::shm_vector<ZEN_STATUS_ITEM> *status_copy_mandy_; // 0x24
    std::vector<ZEN_STATUS_ITEM_WITHNAME> conf_stat_ary_; // 0x28 (12)
    __gnu_cxx::hash_map<ZEN_STATUS_ITEM_ID, unsigned int,
                        HASH_ZEN_STATUS_ITEM_ID> statid_to_index_; // 0x34 (20)
    bool multi_thread_guard_;                             // 0x48
    bool initialized_;                                    // 0x49
    int initialize(const char *stat_filename, bool restore_mmap,
                   bool multi_thread); // line 136
    int find_insert_idx(unsigned int statics_id, unsigned int app_id,
                        unsigned int classify_id, size_t *sandy_idx); // line 242
    static ZEN_Server_Status *instance_;
};

inline int ZEN_Server_Status::increase_by_statid(unsigned int arg0, unsigned int arg1,
                                                 int64_t arg2) {
    return increase_by_statid(arg0, arg1, 0, arg2);
}

inline int ZEN_Server_Status::increase_by_statid(unsigned int arg0, int64_t arg1) {
    return increase_by_statid(arg0, 0, 0, arg1);
}

inline int ZEN_Server_Status::increase_once(unsigned int arg0, unsigned int arg1,
                                            unsigned int arg2) {
    return increase_by_statid(arg0, arg1, arg2, INCREASE_VALUE_ONCE);
}

inline int ZEN_Server_Status::increase_once(unsigned int arg0, unsigned int arg1) {
    return increase_by_statid(arg0, arg1, 0, INCREASE_VALUE_ONCE);
}

inline int ZEN_Server_Status::increase_once(unsigned int arg0) {
    return increase_by_statid(arg0, 0, 0, INCREASE_VALUE_ONCE);
}

inline int ZEN_Server_Status::set_by_statid(unsigned int arg0, unsigned int arg1,
                                            uint64_t arg2) {
    return set_by_statid(arg0, arg1, 0, arg2);
}

inline int ZEN_Server_Status::set_by_statid(unsigned int arg0, uint64_t arg1) {
    return set_by_statid(arg0, 0, 0, arg1);
}

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_
