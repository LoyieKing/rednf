// ZEN_Server_Status, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_server_status.cpp
//
// ORIG DWARF layouts (sizeof):
//   ZEN_STATUS_ITEM_ID        12   statics_id_, app_id_, classify_id_
//   ZEN_STATUS_ITEM           24   item_id_ @0, statics_type_ @12, counter_ @16
//   ZEN_STATUS_ITEM_WITHNAME  92   statics_item_ @0, item_name_[65] @24
//   ZEN_STATUS_HEAD           16   four uint32_t
//   ZEN_LIB::_shm_vector_head 12   size_of_mmap_, num_of_node_, num_of_use_
//   ZEN_LIB::shm_vector<T>    12   (394: no vptr) smem_base_, vector_head_,
//                                  data_base_
//   ZEN_Server_Status         76   vptr @0, stat_lock_ @4, clear_time_ @8,
//                                  stat_file_ @12 (16), stat_file_head_ @28,
//                                  status_stat_sandy_ @32,
//                                  status_copy_mandy_ @36, conf_stat_ary_ @40,
//                                  statid_to_index_ @52 (20),
//                                  multi_thread_guard_ @72, initialized_ @73
#ifndef TSS_RECON_ZEN_SERVER_STATUS_H_
#define TSS_RECON_ZEN_SERVER_STATUS_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include <ext/hash_map>
#include <sstream>
#include <vector>

#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_null_lock.h"
#include "src/commlib/zenlib/zen_shm_models.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"


enum ZEN_STATUS_STATICS_TYPE {
    STATICS_INVALID_TYPE = 0,
    STATICS_PER_FIVE_MINTUES = 1,
    STATICS_PER_HOUR = 2,
    STATICS_PER_DAYS = 3,
    STATICS_ABSOLUTE_VALUE = 11
};

struct ZEN_STATUS_HEAD {
    uint32_t monitor_start_time_;   // +0x00
    uint32_t copy_time_;            // +0x04
    uint32_t report_monitor_time_;  // +0x08
    uint32_t active_time_;          // +0x0c
};

namespace ZEN_LIB {

struct _shm_vector_head { // sizeof 12 (394 naming from DWARF)
    size_t size_of_mmap_;     // +0x00, ORIG writes 0x18000c
    size_t num_of_node_;      // +0x04, ORIG writes 0x10000
    size_t num_of_use_;       // +0x08
};

// ORIG DWARF: shm_vector<ZEN_STATUS_ITEM> size 12 =
//   _shm_memory_base @0 (smem_base_), vector_head_ @4, data_base_ @8.
template <typename T>
struct shm_vector : public _shm_memory_base { // sizeof 12 in tag394 (no vptr)
    _shm_vector_head *vector_head_;   // +0x04
    T *data_base_;                    // +0x08
};


} // namespace ZEN_LIB

// ZEN_ShareMem_Posix   -> src/commlib/zenlib/zen_share_mem_posix.h
//   (ORIG size 16: ZEN_NON_Copyable base, shm_name_ @0, mmap_addr_ @4,
//    mmap_handle_ (ZEN_HANDLE) @8, shm_size_ (size_t) @12)
// ZEN_Thread_Mutex     -> src/commlib/zenlib/zen_lock_thread_mutex.h
//   (ORIG size 28: ZEN_Lock_Base + pthread_mutex_t lock_ @4)
// ZEN_Null_Mutex       -> src/commlib/zenlib/zen_lock_null_lock.h
//   (ORIG size 8: ZEN_Lock_Base + int lock_ @4)
// All three are included above instead of being re-declared here.


struct ZEN_STATUS_ITEM_ID {
    unsigned int statics_id_;
    unsigned int app_id_;
    unsigned int classify_id_;

    ZEN_STATUS_ITEM_ID(unsigned int statics_id, unsigned int app_id,
                       unsigned int classify_id);
    ZEN_STATUS_ITEM_ID();
    ~ZEN_STATUS_ITEM_ID();
    bool operator==(const ZEN_STATUS_ITEM_ID &other) const;
};

struct HASH_ZEN_STATUS_ITEM_ID {
    size_t operator()(const ZEN_STATUS_ITEM_ID &item) const {
        return item.statics_id_ + item.classify_id_ + (item.app_id_ << 8);
    }
};

struct ZEN_STATUS_ITEM {
    ZEN_STATUS_ITEM_ID item_id_;
    ZEN_STATUS_STATICS_TYPE statics_type_;
    uint64_t counter_;

    ZEN_STATUS_ITEM();
    ZEN_STATUS_ITEM(unsigned int statics_id, ZEN_STATUS_STATICS_TYPE type);
    ~ZEN_STATUS_ITEM();
};

struct ZEN_STATUS_ITEM_WITHNAME {
    ZEN_STATUS_ITEM statics_item_;
    char item_name_[65];   // +0x18, ORIG keeps item_name_[64] == '\0'

    ZEN_STATUS_ITEM_WITHNAME(unsigned int statics_id,
                             ZEN_STATUS_STATICS_TYPE type,
                             const char *name);
    ZEN_STATUS_ITEM_WITHNAME();
    ~ZEN_STATUS_ITEM_WITHNAME();
};

static const size_t MAX_COUNTER_NAME_LEN = 64;

typedef __gnu_cxx::hash_map<ZEN_STATUS_ITEM_ID, unsigned int,
                            HASH_ZEN_STATUS_ITEM_ID> ZEN_STATUS_ITEM_ID_MAP;

class ZEN_Server_Status : public ZEN_NON_Copyable {
public:
    ZEN_Server_Status(const ZEN_Server_Status &arg0);
    ZEN_Server_Status();
    virtual ~ZEN_Server_Status();

    int initialize(const char *stat_filename);
    int initialize(const char *stat_filename, size_t num_stat_item,
                   const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool multi_thread);
    void add_status_item(size_t num_add_stat_item,
                         const ZEN_STATUS_ITEM_WITHNAME *item_ary);
    void modify_multi_thread_guard(bool multi_thread);

    int increase_by_statid(unsigned int statics_id, unsigned int app_id,
                           unsigned int classify_id, int64_t incre_value);
    int increase_by_statid(unsigned int statics_id, unsigned int app_id,
                           int64_t incre_value) {
        return increase_by_statid(statics_id, app_id, 0, incre_value);
    }
    int increase_by_statid(unsigned int statics_id, int64_t incre_value) {
        return increase_by_statid(statics_id, 0, 0, incre_value);
    }
    int increase_once(unsigned int statics_id, unsigned int app_id,
                      unsigned int classify_id) {
        return increase_by_statid(statics_id, app_id, classify_id, 1);
    }
    int increase_once(unsigned int statics_id, unsigned int app_id) {
        return increase_by_statid(statics_id, app_id, 0, 1);
    }
    int increase_once(unsigned int statics_id) {
        return increase_by_statid(statics_id, 0, 0, 1);
    }
    int set_by_statid(unsigned int statics_id, unsigned int app_id,
                      unsigned int classify_id, uint64_t set_value);
    int set_by_statid(unsigned int statics_id, unsigned int app_id,
                      uint64_t set_value) {
        return set_by_statid(statics_id, app_id, 0, set_value);
    }
    int set_by_statid(unsigned int statics_id, uint64_t set_value) {
        return set_by_statid(statics_id, 0, 0, set_value);
    }
    uint64_t get_counter(unsigned int statics_id, unsigned int app_id,
                         unsigned int classify_id);

    size_t num_of_counter();
    uint32_t get_copy_time();
    void check_overtime(time_t now_time);
    void copy_stat_counter();
    void dump_all(std::vector<ZEN_STATUS_ITEM_WITHNAME> &array_status,
                  bool dump_copy);
    void dump_status_info(std::ostringstream &strstream, bool dump_copy);
    void dump_status_info(bool dump_copy);
    void get_stat_head(ZEN_STATUS_HEAD *stat_head);
    void report_monitor_time(uint32_t report_time);

    static ZEN_Server_Status *instance();
    static void instance(ZEN_Server_Status *pinstance);
    static void clean_instance();

protected:
    static const size_t MAX_MONITOR_STAT_ITEM = 0x10000;
    static const time_t FIVE_MINTUE_SECONDS = 300;
    static const time_t ONE_HOURS_SECONDS = 3600;
    static const time_t ONE_DAY_SECONDS = 86400;
    static const uint64_t INCREASE_VALUE_ONCE = 1;

    ZEN_Lock_Base *stat_lock_;              // +0x04
    time_t clear_time_;                     // +0x08
    ZEN_ShareMem_Posix stat_file_;          // +0x0c
    ZEN_STATUS_HEAD *stat_file_head_;       // +0x1c
    ZEN_LIB::shm_vector<ZEN_STATUS_ITEM> *status_stat_sandy_; // +0x20
    ZEN_LIB::shm_vector<ZEN_STATUS_ITEM> *status_copy_mandy_; // +0x24
    std::vector<ZEN_STATUS_ITEM_WITHNAME> conf_stat_ary_;     // +0x28
    ZEN_STATUS_ITEM_ID_MAP statid_to_index_;                  // +0x34
    bool multi_thread_guard_;               // +0x48
    bool initialized_;                      // +0x49

    int initialize(const char *stat_filename, bool restore_mmap,
                   bool multi_thread);
    int find_insert_idx(unsigned int statics_id, unsigned int app_id,
                        unsigned int classify_id, unsigned int *sandy_idx);

    static ZEN_Server_Status *instance_;
};

// ORIG 394 layout contracts.  These are compile-time guards against another
// local model creeping into the include path; they are not behavioural
// evidence.  The values are the DWARF/objdump results quoted in the file
// header and in probes/startup_ipc/startup_ipc_probe.cpp.
typedef char tss_recon_status_head_size[
    sizeof(ZEN_STATUS_HEAD) == 16 ? 1 : -1];
typedef char tss_recon_status_item_id_size[
    sizeof(ZEN_STATUS_ITEM_ID) == 12 ? 1 : -1];
typedef char tss_recon_status_item_size[
    sizeof(ZEN_STATUS_ITEM) == 24 ? 1 : -1];
typedef char tss_recon_status_item_withname_size[
    sizeof(ZEN_STATUS_ITEM_WITHNAME) == 92 ? 1 : -1];
typedef char tss_recon_status_shm_vector_size[
    sizeof(ZEN_LIB::shm_vector<ZEN_STATUS_ITEM>) == 12 ? 1 : -1];
typedef char tss_recon_status_thread_mutex_size[
    sizeof(ZEN_Thread_Mutex) == 28 ? 1 : -1];
typedef char tss_recon_status_null_mutex_size[
    sizeof(ZEN_Null_Mutex) == 8 ? 1 : -1];
typedef char tss_recon_status_sharemem_size[
    sizeof(ZEN_ShareMem_Posix) == 16 ? 1 : -1];
typedef char tss_recon_status_object_size[
    sizeof(ZEN_Server_Status) == 76 ? 1 : -1];

#endif // TSS_RECON_ZEN_SERVER_STATUS_H_
