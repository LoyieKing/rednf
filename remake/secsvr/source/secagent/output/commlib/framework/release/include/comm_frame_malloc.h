// Reconstructed from secagent binary (DWARF + 反汇编, 2026-08-10)
// AppFrame_Mallocor_Mgr<MUTEX>：sizeof=60
//   size_appframe_[10]@0（递减帧长档位，initialize 从 [9]=max 逐档减半）
//   frame_pool_@40（vector<lordrings<Comm_App_Frame*>>，10 个环形池）
//   zen_lock_@52
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include "output/commlib/zenlib/release/include/zen_lock_null_lock.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_app_frame.h"
#include <stddef.h>
#include <string.h>
#include <vector>

// sizeof = 60
template<typename MUTEX>
class AppFrame_Mallocor_Mgr {
public:
    static const unsigned int NUM_OF_FRAMELIST = 10;            // line 30
    static const unsigned int NUM_OF_ONCE_INIT_FRAME = 0x200;   // line 33
    static const unsigned int NUM_OF_ALLOW_LIST_IDLE_FRAME = 8; // line 36
protected:
    size_t size_appframe_[NUM_OF_FRAMELIST];                         // 0x00
    std::vector<ZEN_LIB::lordrings<Comm_App_Frame*> > frame_pool_;   // 0x28
    MUTEX zen_lock_;                                                 // 0x34
    static AppFrame_Mallocor_Mgr *instance_; // line 59
public:
    AppFrame_Mallocor_Mgr() : zen_lock_(0) { // line 198
        memset(size_appframe_, 0, sizeof(size_appframe_));
    }

    ~AppFrame_Mallocor_Mgr() { // line 216
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] AppFrame_Mallocor_Mgr::~AppFrame_Mallocor_Mgr.");
        for (unsigned int i = 0; i < NUM_OF_FRAMELIST; ++i) {
            ZEN_LIB::lordrings<Comm_App_Frame*> &pool = frame_pool_[i];
            if (pool.capacity() == pool.size()) {
                ZEN_Trace_LogMsg::debug_infoex(
                    "[framework] List %u(frame size:%u):,free node:%u,capacity node:%u,list node:%u.Ok.",
                    i, size_appframe_[i], 0, pool.capacity(), pool.size());
            } else {
                ZEN_Trace_LogMsg::debug_infoex(
                    "[framework] List %u(frame size:%u):,free node:%u,capacity node:%u,list node:%u.Have memory leak.Please check your code.",
                    i, size_appframe_[i], 0, pool.capacity(), pool.size());
            }
            while (pool.size() != 0) {
                Comm_App_Frame *proc_frame = 0;
                pool.pop_front(proc_frame);
                if (proc_frame != 0) {
                    proc_frame->~Comm_App_Frame();
                    Comm_App_Frame::operator delete(proc_frame,
                                                    LEN_OF_APPFRAME_HEAD);
                }
            }
        }
        zen_lock_.~MUTEX();
        for (size_t i = 0; i < frame_pool_.size(); ++i) {
            if (frame_pool_[i].value_ptr_ != 0) {
                delete[] frame_pool_[i].value_ptr_;
                frame_pool_[i].value_ptr_ = 0;
            }
        }
    }

    void initialize(unsigned int init_num, unsigned int max_frame_len) { // line 121
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] AppFrame_Mallocor_Mgr::AppFrame_Mallocor_Mgr init num=%u,max_frame_len=%u.",
            init_num, max_frame_len);
        size_t sz_frame = max_frame_len;
        for (int i = (int)NUM_OF_FRAMELIST - 1; i >= 0; --i) {
            size_appframe_[i] = sz_frame;
            sz_frame >>= 1;
        }
        if (frame_pool_.size() > NUM_OF_FRAMELIST) {
            frame_pool_.erase(frame_pool_.begin() + NUM_OF_FRAMELIST,
                              frame_pool_.end());
        } else if (frame_pool_.size() < NUM_OF_FRAMELIST) {
            frame_pool_.insert(frame_pool_.end(),
                               NUM_OF_FRAMELIST - frame_pool_.size(),
                               ZEN_LIB::lordrings<Comm_App_Frame *>());
        }
        for (unsigned int i = 0; i < NUM_OF_FRAMELIST; ++i) {
            ExtendFrameListCapacity(i, init_num);
        }
    }

    size_t GetMaxFrameLength(unsigned int list_no) { // line 149
        return size_appframe_[list_no];
    }

    unsigned int GetRoundUp(unsigned int size) { // line 167
        unsigned int round = 1;
        while (round < size) {
            round <<= 1;
        }
        return round;
    }

    void AdjustPoolListCapacity(unsigned int list_no, unsigned int extend_num) { // line 350
        ExtendFrameListCapacity(list_no, extend_num);
    }

    Comm_App_Frame *alloc_appframe(unsigned int frame_len) { // line 273
        ZEN_Lock_Guard<MUTEX> tmp_guard(zen_lock_);
        size_t hk = 0;
        if (frame_len > size_appframe_[0]) {
            for (hk = 1; hk < NUM_OF_FRAMELIST; ++hk) {
                if (frame_len <= size_appframe_[hk]) {
                    break;
                }
            }
            if (hk == NUM_OF_FRAMELIST) {
                // 二进制：超出最大档位时按 -1 号池处理（与反汇编一致）
                hk = (size_t)-1;
            }
        }
        if (frame_pool_[hk].size() == 0) {
            ExtendFrameListCapacity((unsigned int)hk, NUM_OF_ONCE_INIT_FRAME);
        }
        Comm_App_Frame *new_frame = 0;
        if (frame_pool_[hk].size() != 0) {
            frame_pool_[hk].pop_front(new_frame);
        }
        new_frame->init_framehead(frame_len, 0, 0);
        return new_frame;
    }

    void clone_appframe(const Comm_App_Frame *model_freame,
                        Comm_App_Frame *&cloned_frame) { // line 308
        size_t frame_len = model_freame->frame_length_;
        cloned_frame = alloc_appframe((unsigned int)frame_len);
        memcpy(cloned_frame, model_freame, frame_len);
    }

    void free_appframe(Comm_App_Frame *proc_frame) { // line 329
        ZEN_Lock_Guard<MUTEX> tmp_guard(zen_lock_);
        size_t hk = 0;
        for (; hk < NUM_OF_FRAMELIST; ++hk) {
            if (proc_frame->frame_length_ <= size_appframe_[hk]) {
                break;
            }
        }
        if (hk == NUM_OF_FRAMELIST) {
            hk = (size_t)-1; // 二进制行为：越界池（与原版一致）
        }
        frame_pool_[hk].push_back(proc_frame);
    }

    void ExtendFrameListCapacity(unsigned int list_no, unsigned int extend_num) { // line 385
        size_t old_capacity;
        size_t j;
        Comm_App_Frame *proc_frame;
        ZEN_LIB::lordrings<Comm_App_Frame *> &pool = frame_pool_[list_no];
        old_capacity = pool.capacity();
        size_t new_capacity = old_capacity + extend_num;
        if (new_capacity >= pool.size()) {
            Comm_App_Frame **new_data = new Comm_App_Frame *[new_capacity];
            size_t old_size = pool.size();
            if (old_size != 0) {
                for (size_t i = 0; i < old_size; ++i) {
                    new_data[i] = pool[(unsigned int)i];
                }
            }
            if (pool.value_ptr_ != 0) {
                delete[] pool.value_ptr_;
                pool.value_ptr_ = 0;
            }
            pool.cycdeque_start_ = 0;
            pool.cycdeque_capacity_ = new_capacity;
            pool.value_ptr_ = new_data;
        }
        for (j = 0; j < extend_num; ++j) {
            proc_frame = new (size_appframe_[list_no] + 1)
                Comm_App_Frame(0, LEN_OF_APPFRAME_HEAD, 0x1000000);
            pool.push_back(proc_frame);
        }
    }

    static AppFrame_Mallocor_Mgr * instance() { // line 400
        if (instance_ == 0) {
            instance_ = new AppFrame_Mallocor_Mgr();
        }
        return instance_;
    }

    static void instance(AppFrame_Mallocor_Mgr *pinstatnce) { // line 408
        clean_instance();
        instance_ = pinstatnce;
    }

    static void clean_instance() { // line 412
        if (instance_ != 0) {
            delete instance_;
            instance_ = 0;
        }
    }
};

template<typename MUTEX>
AppFrame_Mallocor_Mgr<MUTEX> *AppFrame_Mallocor_Mgr<MUTEX>::instance_ = 0;

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_
