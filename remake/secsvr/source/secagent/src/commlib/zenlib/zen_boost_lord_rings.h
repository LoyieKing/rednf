// Reconstructed from secagent binary (2026-08-10)
// ZEN_LIB::lordrings<T>: 循环队列，sizeof=16
//   cycdeque_start_@0  cycdeque_size_@4  cycdeque_capacity_@8  value_ptr_@12
// push: data_[(start_+size_)%capacity_] = v; size_++（满时跳过）
// pop : 读 data_[start_]，start_=(start_+1)%capacity_，size_--（不清槽，与二进制一致）
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_

#include <stddef.h>

namespace ZEN_LIB {

template <typename T>
class lordrings {
public:
    lordrings()
        : cycdeque_start_(0), cycdeque_size_(0), cycdeque_capacity_(0),
          value_ptr_(0) {
    }

    explicit lordrings(unsigned int capacity)
        : cycdeque_start_(0), cycdeque_size_(0), cycdeque_capacity_(0),
          value_ptr_(0) {
        resize(capacity);
    }

    ~lordrings() {
        if (value_ptr_ != 0) {
            delete[] value_ptr_;
            value_ptr_ = 0;
        }
    }

    size_t size() const {
        return cycdeque_size_;
    }

    size_t freesize() const {
        return cycdeque_capacity_ - cycdeque_size_;
    }

    size_t capacity() const {
        return cycdeque_capacity_;
    }

    bool full() const {
        return cycdeque_size_ == cycdeque_capacity_;
    }

    bool empty() const {
        return cycdeque_size_ == 0;
    }

    bool resize(unsigned int capacity) {
        T *new_ptr = capacity > 0 ? new T[capacity] : static_cast<T *>(0);
        size_t copy_num = cycdeque_size_ < (size_t)capacity ? cycdeque_size_
                                                            : (size_t)capacity;
        for (size_t i = 0; i < copy_num; ++i) {
            new_ptr[i] = (*this)[i];
        }
        if (value_ptr_ != 0) {
            delete[] value_ptr_;
        }
        value_ptr_ = new_ptr;
        cycdeque_capacity_ = capacity;
        cycdeque_start_ = 0;
        cycdeque_size_ = copy_num;
        return true;
    }

    void clear() {
        cycdeque_start_ = 0;
        cycdeque_size_ = 0;
    }

    // 满时返回 false 不插入（与 secagent 二进制行为一致）
    bool push_back(const T &value) {
        if (cycdeque_capacity_ == 0 || full()) {
            return false;
        }
        size_t pos = (cycdeque_start_ + cycdeque_size_) % cycdeque_capacity_;
        value_ptr_[pos] = value;
        ++cycdeque_size_;
        return true;
    }

    bool pop_front(T &value) {
        if (empty()) {
            return false;
        }
        value = value_ptr_[cycdeque_start_];
        cycdeque_start_ = (cycdeque_start_ + 1) % cycdeque_capacity_;
        --cycdeque_size_;
        return true;
    }

    T &operator[](unsigned int index) {
        return value_ptr_[(cycdeque_start_ + index) % cycdeque_capacity_];
    }

    const T &operator[](unsigned int index) const {
        return value_ptr_[(cycdeque_start_ + index) % cycdeque_capacity_];
    }

public:
    size_t cycdeque_start_;
    size_t cycdeque_size_;
    size_t cycdeque_capacity_;
    T *value_ptr_;
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_
