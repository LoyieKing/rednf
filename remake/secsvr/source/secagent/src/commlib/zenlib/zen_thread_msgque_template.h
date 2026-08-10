// Reconstructed from secagent binary (DWARF + 反汇编, 2026-08-10)
// ZEN_Message_Queue<SYNCH,T,CONTAINER>：sizeof=48（queue_max_size_@0,
// queue_cur_size_@4, message_queue_@8=std::deque 40 字节）
// ZEN_Message_Queue_Deque 继承之（无新增成员）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include <deque>
#include <stddef.h>

template<typename SYNCH, typename T, typename CONTAINER>
class ZEN_Message_Queue : public ZEN_NON_Copyable {
protected:
    size_t queue_max_size_;    // 0x00
    size_t queue_cur_size_;    // 0x04
    CONTAINER message_queue_;  // 0x08
public:
    explicit ZEN_Message_Queue(size_t max_size)
        : queue_max_size_(max_size), queue_cur_size_(0), message_queue_() {
    }

    ~ZEN_Message_Queue() {
    }

    bool empty() const {
        return queue_cur_size_ == 0;
    }

    bool full() const {
        return queue_cur_size_ >= queue_max_size_;
    }

    int enqueue(const T &elem) {
        if (full()) {
            return -1;
        }
        message_queue_.push_back(elem);
        ++queue_cur_size_;
        return 0;
    }

    int enqueue(T &elem) {
        if (full()) {
            return -1;
        }
        message_queue_.push_back(elem);
        ++queue_cur_size_;
        return 0;
    }

    int dequeue(T &elem) {
        if (empty()) {
            return -1;
        }
        elem = message_queue_.front();
        message_queue_.pop_front();
        --queue_cur_size_;
        return 0;
    }

    int dequeue(T *&elem) {
        if (empty()) {
            return -1;
        }
        elem = message_queue_.front();
        message_queue_.pop_front();
        --queue_cur_size_;
        return 0;
    }

    void clear() {
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

    size_t size() const {
        return queue_cur_size_;
    }
};

template<typename SYNCH, typename T>
class ZEN_Message_Queue_Deque
    : public ZEN_Message_Queue<SYNCH, T, std::deque<T> > {
public:
    typedef ZEN_Message_Queue<SYNCH, T, std::deque<T> > base_type;

    explicit ZEN_Message_Queue_Deque(size_t max_size)
        : base_type(max_size) {
    }

    ~ZEN_Message_Queue_Deque() {
    }
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_
