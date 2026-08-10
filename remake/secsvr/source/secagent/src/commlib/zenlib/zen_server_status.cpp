// ZEN_Server_Status 还原实现（语义对照 gunnersvr oracle，2026-08-10）。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_server_status.cpp
// 说明：
//  - 状态计数通过共享内存（0x300028 字节）维护两块 ZEN_STATUS_ITEM 区：
//    sandy（在线）@+0x10、mandy（镜像）@+0x18001c，头部均为 12 字节
//    _shm_vector_head（magic=0x18000c / max=0x10000 / size），数据区偏移 +0x0c。
//  - statid_to_index_ 为 __gnu_cxx::hash_map<ZEN_STATUS_ITEM_ID,uint,HASH_...>，
//    哈希 = statics_id_ + classify_id_ + (app_id_ << 8)（oracle 内联计算）。
//  - 原版怪癖保留：initialize() 成功路径才会建锁；dump_status_info(bool) 只与
//    sandy/mandy 第 0 项比对；ostringstream 版头部打印 0；setw(64) 打印 name。

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <iomanip>
#include <new>
#include <sstream>
#include <string>
#include <vector>
#include <ext/hash_map>

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_lock_base.h"
#include "src/commlib/zenlib/zen_lock_null_lock.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"
#include "src/commlib/zenlib/zen_shm_vector.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ---------- ZEN_STATUS_ITEM_ID ----------

ZEN_STATUS_ITEM_ID::ZEN_STATUS_ITEM_ID(unsigned int arg0, unsigned int arg1,
                                       unsigned int arg2) {
    statics_id_ = arg0;
    app_id_ = arg1;
    classify_id_ = arg2;
}

ZEN_STATUS_ITEM_ID::ZEN_STATUS_ITEM_ID() {
    statics_id_ = 0;
    app_id_ = 0;
    classify_id_ = 0;
}

ZEN_STATUS_ITEM_ID::~ZEN_STATUS_ITEM_ID() {
}

bool ZEN_STATUS_ITEM_ID::operator==(const ZEN_STATUS_ITEM_ID &others) const {
    return statics_id_ == others.statics_id_ && app_id_ == others.app_id_
           && classify_id_ == others.classify_id_;
}

// ---------- ZEN_STATUS_ITEM ----------

ZEN_STATUS_ITEM::ZEN_STATUS_ITEM() {
    item_id_.statics_id_ = 0;
    item_id_.app_id_ = 0;
    item_id_.classify_id_ = 0;
    statics_type_ = STATICS_PER_FIVE_MINTUES;
    counter_ = 0;
}

ZEN_STATUS_ITEM::ZEN_STATUS_ITEM(unsigned int arg0, ZEN_STATUS_STATICS_TYPE arg1) {
    item_id_.statics_id_ = arg0;
    item_id_.app_id_ = 0;
    item_id_.classify_id_ = 0;
    statics_type_ = arg1;
}

ZEN_STATUS_ITEM::~ZEN_STATUS_ITEM() {
}

// ---------- ZEN_STATUS_ITEM_WITHNAME ----------

ZEN_STATUS_ITEM_WITHNAME::ZEN_STATUS_ITEM_WITHNAME() {
    item_name_end_ = 0;
}

ZEN_STATUS_ITEM_WITHNAME::ZEN_STATUS_ITEM_WITHNAME(unsigned int arg0,
                                                   ZEN_STATUS_STATICS_TYPE arg1,
                                                   const char *arg2)
    : statics_item_(arg0, arg1) {
    strncpy(item_name_, arg2, MAX_COUNTER_NAME_LEN);
    item_name_end_ = 0;
}

ZEN_STATUS_ITEM_WITHNAME::~ZEN_STATUS_ITEM_WITHNAME() {
}

// ---------- ZEN_Server_Status 基础访问 ----------

uint32_t ZEN_Server_Status::get_copy_time() {
    return stat_file_head_->copy_time_;
}

void ZEN_Server_Status::get_stat_head(ZEN_STATUS_HEAD *stat_head) {
    *stat_head = *stat_file_head_;
}

void ZEN_Server_Status::report_monitor_time(uint32_t report_time) {
    stat_file_head_->report_monitor_time_ = report_time;
}

void ZEN_Server_Status::clean_instance() {
    if (instance_) {
        delete instance_;
    }
    instance_ = NULL;
}

void ZEN_Server_Status::instance(ZEN_Server_Status *pinstatnce) {
    clean_instance();
    instance_ = pinstatnce;
}

void ZEN_Server_Status::modify_multi_thread_guard(bool multi_thread) {
    if (stat_lock_) {
        delete stat_lock_;
        stat_lock_ = NULL;
    }
    multi_thread_guard_ = multi_thread;
    if (multi_thread) {
        // secagent (tag435) 的 ZEN_Thread_Mutex 无 const char* 构造器，
        // 二进制 initialize 内联确认用默认构造（gunnersvr 版带 NULL 参数）。
        stat_lock_ = new ZEN_Thread_Mutex();
    } else {
        stat_lock_ = new ZEN_Null_Mutex(NULL);
    }
}

void ZEN_Server_Status::copy_stat_counter() {
    size_t stat_sandy_begin = status_stat_sandy_->vector_head_->size_;
    size_t stat_mandy_begin = status_copy_mandy_->vector_head_->size_;
    if (stat_sandy_begin > stat_mandy_begin) {
        size_t add_num = stat_sandy_begin - stat_mandy_begin;
        for (size_t i = 0; i < add_num; ++i) {
            new (status_copy_mandy_->data_base_ + stat_mandy_begin + i)
                ZEN_STATUS_ITEM();
        }
    }
    status_copy_mandy_->vector_head_->size_ = stat_sandy_begin;
    memcpy(status_copy_mandy_->data_base_, status_stat_sandy_->data_base_,
           stat_sandy_begin * sizeof(ZEN_STATUS_ITEM));
    stat_file_head_->copy_time_ = time(NULL);
}

size_t ZEN_Server_Status::num_of_counter() {
    return status_stat_sandy_->vector_head_->size_;
}

// ---------- 初始化 ----------

int ZEN_Server_Status::initialize(const char *stat_filename, bool restore_mmap,
                                  bool multi_thread) {
    int ret = stat_file_.open(stat_filename, 0x300028, 0x42, 0x1b6, NULL, 3, 1, 0);
    if (ret != 0) {
        return ret;
    }
    stat_file_head_ = (ZEN_STATUS_HEAD *)stat_file_.addr();
    char *shm_addr = (char *)stat_file_.addr();

    if (restore_mmap) {
        if (*(uint32_t *)(shm_addr + 0x10) == 0x18000c
            && *(uint32_t *)(shm_addr + 0x14) == 0x10000) {
            *(uint32_t *)(shm_addr + 0x14) = 0x10000;
            *(uint32_t *)(shm_addr + 0x10) = 0x18000c;
            status_stat_sandy_ = new ZEN_LIB::shm_vector<ZEN_STATUS_ITEM>();
            status_stat_sandy_->smem_base_ = shm_addr + 0x10;
            status_stat_sandy_->vector_head_ =
                (ZEN_LIB::_shm_vector_head *)(shm_addr + 0x10);
            status_stat_sandy_->data_base_ = (ZEN_STATUS_ITEM *)(shm_addr + 0x1c);
            if (*(uint32_t *)(shm_addr + 0x18001c) == 0x18000c
                && *(uint32_t *)(shm_addr + 0x180020) == 0x10000) {
                *(uint32_t *)(shm_addr + 0x180020) = 0x10000;
                *(uint32_t *)(shm_addr + 0x18001c) = 0x18000c;
                status_copy_mandy_ = new ZEN_LIB::shm_vector<ZEN_STATUS_ITEM>();
                status_copy_mandy_->smem_base_ = shm_addr + 0x18001c;
                status_copy_mandy_->vector_head_ =
                    (ZEN_LIB::_shm_vector_head *)(shm_addr + 0x18001c);
                status_copy_mandy_->data_base_ =
                    (ZEN_STATUS_ITEM *)(shm_addr + 0x180028);
                modify_multi_thread_guard(multi_thread);
            } else {
                status_copy_mandy_ = NULL;
                ret = -1;
            }
        } else {
            status_stat_sandy_ = NULL;
            ret = -1;
        }
    } else {
        *(uint32_t *)(shm_addr + 0x14) = 0x10000;
        *(uint32_t *)(shm_addr + 0x10) = 0x18000c;
        status_stat_sandy_ = new ZEN_LIB::shm_vector<ZEN_STATUS_ITEM>();
        status_stat_sandy_->smem_base_ = shm_addr + 0x10;
        status_stat_sandy_->vector_head_ =
            (ZEN_LIB::_shm_vector_head *)(shm_addr + 0x10);
        status_stat_sandy_->data_base_ = (ZEN_STATUS_ITEM *)(shm_addr + 0x1c);
        status_stat_sandy_->vector_head_->size_ = 0;
        *(uint32_t *)(shm_addr + 0x18001c) = 0x18000c;
        *(uint32_t *)(shm_addr + 0x180020) = 0x10000;
        status_copy_mandy_ = new ZEN_LIB::shm_vector<ZEN_STATUS_ITEM>();
        status_copy_mandy_->smem_base_ = shm_addr + 0x18001c;
        status_copy_mandy_->vector_head_ =
            (ZEN_LIB::_shm_vector_head *)(shm_addr + 0x18001c);
        status_copy_mandy_->data_base_ =
            (ZEN_STATUS_ITEM *)(shm_addr + 0x180028);
        status_copy_mandy_->vector_head_->size_ = 0;
        modify_multi_thread_guard(multi_thread);
    }
    return ret;
}

int ZEN_Server_Status::initialize(const char *stat_filename) {
    return initialize(stat_filename, true, false);
}

int ZEN_Server_Status::initialize(const char *stat_filename, size_t num_stat_item,
                                  const ZEN_STATUS_ITEM_WITHNAME *item_ary,
                                  bool multi_thread) {
    int ret = initialize(stat_filename, false, multi_thread);
    if (ret != 0) {
        return ret;
    }
    add_status_item(num_stat_item, item_ary);
    statid_to_index_.resize(0x18000);
    stat_file_head_->monitor_start_time_ = time(NULL);
    stat_file_head_->copy_time_ = time(NULL);
    clear_time_ = time(NULL);
    initialized_ = true;
    return ret;
}

// ---------- 状态项增删查改 ----------

void ZEN_Server_Status::add_status_item(size_t num_add_stat_item,
                                        const ZEN_STATUS_ITEM_WITHNAME *item_ary) {
    conf_stat_ary_.reserve(conf_stat_ary_.size() + num_add_stat_item);
    for (size_t i = 0; i < num_add_stat_item; ++i) {
        conf_stat_ary_.push_back(item_ary[i]);
    }
}

int ZEN_Server_Status::find_insert_idx(unsigned int statics_id, unsigned int app_id,
                                       unsigned int classify_id, size_t *sandy_idx) {
    *sandy_idx = (size_t)-1;
    ZEN_STATUS_ITEM_ID stat_item_id(statics_id, app_id, classify_id);
    __gnu_cxx::hash_map<ZEN_STATUS_ITEM_ID, unsigned int,
                        HASH_ZEN_STATUS_ITEM_ID>::iterator it
        = statid_to_index_.find(stat_item_id);
    if (it != statid_to_index_.end()) {
        *sandy_idx = it->second;
        return 0;
    }

    size_t i = 0;
    for (; i < conf_stat_ary_.size(); ++i) {
        if (conf_stat_ary_[i].statics_item_.item_id_.statics_id_ == statics_id) {
            break;
        }
    }
    if (i == conf_stat_ary_.size()) {
        return -1;
    }

    size_t sandy_size = status_stat_sandy_->vector_head_->size_;
    if (sandy_size == status_stat_sandy_->vector_head_->max_) {
        return -1;
    }
    if (sandy_size < status_stat_sandy_->vector_head_->max_) {
        ZEN_STATUS_ITEM status_item;
        status_item.item_id_ = stat_item_id;
        status_item.counter_ = 0;
        status_stat_sandy_->data_base_[sandy_size] = status_item;
        ++status_stat_sandy_->vector_head_->size_;
    }
    // 二进制 0x8094b4b 直接调用 hashtable::find_or_insert；
    // hash_map::operator[] 即其封装（GCC 4.1 内部 _M_ht.find_or_insert）。
    statid_to_index_[stat_item_id] = (unsigned int)sandy_size;
    *sandy_idx = sandy_size;
    return 0;
}

int ZEN_Server_Status::set_by_statid(unsigned int statics_id, unsigned int app_id,
                                     unsigned int classify_id, uint64_t set_value) {
    if (!initialized_) {
        return -1;
    }
    size_t sandy_idx = (size_t)-1;
    stat_lock_->lock();
    int ret = find_insert_idx(statics_id, app_id, classify_id, &sandy_idx);
    if (ret == 0) {
        status_stat_sandy_->data_base_[sandy_idx].counter_ = set_value;
    }
    stat_lock_->unlock();
    return ret;
}

int ZEN_Server_Status::increase_by_statid(unsigned int statics_id, unsigned int app_id,
                                          unsigned int classify_id,
                                          int64_t incre_value) {
    if (!initialized_) {
        return -1;
    }
    size_t sandy_idx = (size_t)-1;
    stat_lock_->lock();
    int ret = find_insert_idx(statics_id, app_id, classify_id, &sandy_idx);
    if (ret == 0) {
        status_stat_sandy_->data_base_[sandy_idx].counter_ += incre_value;
    }
    stat_lock_->unlock();
    return ret;
}

uint64_t ZEN_Server_Status::get_counter(unsigned int statics_id, unsigned int app_id,
                                        unsigned int classify_id) {
    ZEN_STATUS_ITEM_ID stat_item_id(statics_id, app_id, classify_id);
    __gnu_cxx::hash_map<ZEN_STATUS_ITEM_ID, unsigned int,
                        HASH_ZEN_STATUS_ITEM_ID>::iterator it
        = statid_to_index_.find(stat_item_id);
    if (it != statid_to_index_.end()) {
        stat_lock_->lock();
        uint64_t counter = status_stat_sandy_->data_base_[it->second].counter_;
        stat_lock_->unlock();
        return counter;
    }
    return 0;
}

// ---------- 周期切换与镜像 ----------

void ZEN_Server_Status::check_overtime(time_t now_time) {
    if (now_time / ONE_DAY_SECONDS == clear_time_ / ONE_DAY_SECONDS) {
        return;
    }
    int statics_type = STATICS_PER_HOUR;
    if (now_time / ONE_HOURS_SECONDS != clear_time_ / ONE_HOURS_SECONDS) {
        statics_type = STATICS_PER_FIVE_MINTUES;
        if (now_time / FIVE_MINTUE_SECONDS != clear_time_ / FIVE_MINTUE_SECONDS) {
            statics_type = STATICS_PER_DAYS;
        }
    }
    size_t sandy_size = status_stat_sandy_->vector_head_->size_;
    copy_stat_counter();
    stat_lock_->lock();
    for (size_t i = 0; i < sandy_size; ++i) {
        if (statics_type >= status_stat_sandy_->data_base_[i].statics_type_) {
            status_stat_sandy_->data_base_[i].counter_ = 0;
        }
    }
    clear_time_ = now_time;
    stat_lock_->unlock();
}

// ---------- 导出 ----------

void ZEN_Server_Status::dump_all(std::vector<ZEN_STATUS_ITEM_WITHNAME> &array_status,
                                 bool dump_copy) {
    stat_lock_->lock();
    size_t stat_num = conf_stat_ary_.size();
    size_t process_num = (dump_copy ? status_stat_sandy_ : status_copy_mandy_)
                             ->vector_head_->size_;
    array_status.erase(array_status.begin(), array_status.begin() + process_num);
    ZEN_STATUS_ITEM_WITHNAME tmp_item;
    ZEN_STATUS_ITEM *stat_process_iter =
        (dump_copy ? status_stat_sandy_ : status_copy_mandy_)->data_base_;
    for (size_t i = 0; i < process_num; ++i) {
        array_status[i].statics_item_.item_id_ = stat_process_iter[i].item_id_;
        array_status[i].statics_item_.statics_type_ =
            stat_process_iter[i].statics_type_;
        array_status[i].statics_item_.counter_ = stat_process_iter[i].counter_;
        for (size_t j = 0; j < stat_num; ++j) {
            if (conf_stat_ary_[j].statics_item_.item_id_.statics_id_
                == array_status[i].statics_item_.item_id_.statics_id_) {
                strncpy(array_status[i].item_name_, conf_stat_ary_[j].item_name_,
                        MAX_COUNTER_NAME_LEN);
                break;
            }
        }
    }
    stat_lock_->unlock();
}

void ZEN_Server_Status::dump_status_info(std::ostringstream &strstream,
                                         bool dump_copy) {
    strstream << "Statistics Number:" << (unsigned int)0 << std::endl;
    size_t stat_num = conf_stat_ary_.size();
    size_t process_num = (dump_copy ? status_stat_sandy_ : status_copy_mandy_)
                             ->vector_head_->size_;
    ZEN_STATUS_ITEM *stat_process_iter =
        (dump_copy ? status_stat_sandy_ : status_copy_mandy_)->data_base_;
    stat_lock_->lock();
    char item_name[MAX_COUNTER_NAME_LEN];
    size_t row = 0;
    for (size_t i = 0; i < stat_num; ++i) {
        if (conf_stat_ary_[i].statics_item_.item_id_.statics_id_
            == stat_process_iter->item_id_.statics_id_) {
            strncpy(item_name, conf_stat_ary_[i].item_name_, MAX_COUNTER_NAME_LEN);
            strstream << std::setw(6) << (unsigned int)row << "." << "statics id<"
                      << std::setw(10) << stat_process_iter->item_id_.statics_id_
                      << ">" << "game app id<" << std::setw(10)
                      << stat_process_iter->item_id_.app_id_ << ">" << "classify id<"
                      << std::setw(10) << stat_process_iter->item_id_.classify_id_
                      << ">" << std::setw(64) << item_name << ":"
                      << stat_process_iter->counter_ << std::endl;
            ++row;
            ++stat_process_iter;
        }
    }
    stat_lock_->unlock();
}

void ZEN_Server_Status::dump_status_info(bool dump_copy) {
    size_t stat_num = conf_stat_ary_.size();
    size_t process_num = (dump_copy ? status_stat_sandy_ : status_copy_mandy_)
                             ->vector_head_->size_;
    ZEN_STATUS_ITEM *stat_process_iter =
        (dump_copy ? status_stat_sandy_ : status_copy_mandy_)->data_base_;
    ZEN_Trace_LogMsg::debug_infoex("Statistics Number: %u", (unsigned int)stat_num);
    ZEN_Trace_LogMsg::debug_infoex(
        "index.<statics id,classify id> name                            :number");
    stat_lock_->lock();
    char item_name[MAX_COUNTER_NAME_LEN];
    size_t row = 0;
    for (size_t i = 0; i < stat_num; ++i) {
        if (conf_stat_ary_[i].statics_item_.item_id_.statics_id_
            == stat_process_iter->item_id_.statics_id_) {
            strncpy(item_name, conf_stat_ary_[i].item_name_, MAX_COUNTER_NAME_LEN);
            ZEN_Trace_LogMsg::debug_infoex(
                "%5u.<%10u, %10u, %10u> %32s : %llu ", (unsigned int)row,
                stat_process_iter->item_id_.statics_id_,
                stat_process_iter->item_id_.app_id_,
                stat_process_iter->item_id_.classify_id_, item_name,
                stat_process_iter->counter_);
            ++row;
        }
    }
    stat_lock_->unlock();
}

// ---------- 单例 ----------

ZEN_Server_Status * ZEN_Server_Status::instance_ = NULL;

ZEN_Server_Status * ZEN_Server_Status::instance() {
    if (!instance_) {
        instance_ = new ZEN_Server_Status();
    }
    return instance_;
}

// ---------- 构造 / 析构 ----------

ZEN_Server_Status::ZEN_Server_Status()
    : stat_lock_(NULL),
      clear_time_(0),
      stat_file_(),
      stat_file_head_(NULL),
      status_stat_sandy_(NULL),
      status_copy_mandy_(NULL),
      conf_stat_ary_(),
      statid_to_index_(),
      multi_thread_guard_(false),
      initialized_(false) {
}

ZEN_Server_Status::~ZEN_Server_Status() {
    if (stat_lock_) {
        delete stat_lock_;
        stat_lock_ = NULL;
    }
    if (status_stat_sandy_) {
        delete status_stat_sandy_;
        status_stat_sandy_ = NULL;
    }
    if (status_copy_mandy_) {
        delete status_copy_mandy_;
        status_copy_mandy_ = NULL;
    }
}
