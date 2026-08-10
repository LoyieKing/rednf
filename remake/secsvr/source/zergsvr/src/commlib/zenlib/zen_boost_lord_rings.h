// Reconstructed from zergsvr binary (zen_boost_lord_rings.h DWARF + disassembly).
// ZEN_LIB::lordrings<T>: 环形缓冲，sizeof=16，
// cycdeque_start_@0 cycdeque_size_@4 cycdeque_capacity_@8 value_ptr_@12（T*）。
// 语义按二进制内联实例对拍：push_back/pop_front/resize/默认构造。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_

#include <stddef.h>
#include <new>

namespace ZEN_LIB {

template <typename T>
struct lordrings {
public:
    lordrings() {  // line 169
        cycdeque_start_ = 0;
        cycdeque_size_ = 0;
        cycdeque_capacity_ = 0;
        value_ptr_ = NULL;
    }

    explicit lordrings(unsigned int capacity) {  // line 178
        cycdeque_start_ = 0;
        cycdeque_size_ = 0;
        cycdeque_capacity_ = 0;
        value_ptr_ = NULL;
        resize(capacity);
    }

    ~lordrings() {  // line 188：析构为空，内存由调用方 finit/手动管理
    }

    void initialize(unsigned int capacity) {  // line 198
        resize(capacity);
    }

    void finit() {  // line 217
        if (value_ptr_ != NULL) {
            delete[] value_ptr_;
            value_ptr_ = NULL;
        }
        cycdeque_capacity_ = 0;
        cycdeque_start_ = 0;
        cycdeque_size_ = 0;
    }

    void clear() {  // line 232
        cycdeque_start_ = 0;
        cycdeque_size_ = 0;
    }

    size_t size() const {  // line 239
        return cycdeque_size_;
    }

    size_t freesize() const {  // line 244
        return cycdeque_capacity_ - cycdeque_size_;
    }

    size_t capacity() const {  // line 250
        return cycdeque_capacity_;
    }

    bool full() const {  // line 256
        return cycdeque_size_ >= cycdeque_capacity_;
    }

    bool empty() const {  // line 268
        return cycdeque_size_ == 0;
    }

    bool resize(unsigned int capacity) {  // line 280
        T *new_ptr = capacity > 0 ? new T[capacity] : static_cast<T *>(NULL);
        size_t copy_num = cycdeque_size_ < (size_t)capacity ? cycdeque_size_
                                                            : (size_t)capacity;
        size_t i;
        for (i = 0; i < copy_num; ++i) {
            new_ptr[i] = (*this)[i];
        }
        if (value_ptr_ != NULL) {
            delete[] value_ptr_;
        }
        value_ptr_ = new_ptr;
        cycdeque_capacity_ = capacity;
        cycdeque_start_ = 0;
        cycdeque_size_ = copy_num;
        return true;
    }

    bool push_back(const T &value, bool discard_when_full) {  // line 317
        if (full()) {
            if (!discard_when_full) {
                return false;
            }
            value_ptr_[cycdeque_start_ % cycdeque_capacity_] = value;
            cycdeque_start_ = (cycdeque_start_ + 1) % cycdeque_capacity_;
            return true;
        }
        size_t pos = (cycdeque_start_ + cycdeque_size_) % cycdeque_capacity_;
        value_ptr_[pos] = value;
        ++cycdeque_size_;
        return true;
    }

    bool push_front(const T &value, bool discard_when_full) {  // line 348
        if (full()) {
            if (!discard_when_full) {
                return false;
            }
            cycdeque_start_ =
                (cycdeque_start_ + cycdeque_capacity_ - 1) % cycdeque_capacity_;
            value_ptr_[cycdeque_start_] = value;
            return true;
        }
        cycdeque_start_ =
            (cycdeque_start_ + cycdeque_capacity_ - 1) % cycdeque_capacity_;
        value_ptr_[cycdeque_start_] = value;
        ++cycdeque_size_;
        return true;
    }

    bool pop_front(T &value) {  // line 381
        value = value_ptr_[cycdeque_start_ % cycdeque_capacity_];
        value_ptr_[cycdeque_start_ % cycdeque_capacity_] = T();
        if (cycdeque_size_ != 0) {
            --cycdeque_size_;
            cycdeque_start_ = (cycdeque_start_ + 1) % cycdeque_capacity_;
        }
        return true;
    }

    bool pop_front() {  // line 397
        if (empty()) {
            return false;
        }
        --cycdeque_size_;
        cycdeque_start_ = (cycdeque_start_ + 1) % cycdeque_capacity_;
        return true;
    }

    bool pop_back(T &value) {  // line 412
        if (empty()) {
            return false;
        }
        --cycdeque_size_;
        size_t pos = (cycdeque_start_ + cycdeque_size_) % cycdeque_capacity_;
        value = value_ptr_[pos];
        value_ptr_[pos] = T();
        return true;
    }

    bool pop_back() {  // line 427
        if (empty()) {
            return false;
        }
        --cycdeque_size_;
        return true;
    }

    T &operator[](unsigned int index) {  // line 441
        return value_ptr_[(cycdeque_start_ + index) % cycdeque_capacity_];
    }

    const T &operator[](unsigned int index) const {  // line 447
        return value_ptr_[(cycdeque_start_ + index) % cycdeque_capacity_];
    }

    class iterator {
    public:
        iterator(lordrings *owner, size_t index) : owner_(owner), index_(index) {
        }
        T &operator*() const {
            return (*owner_)[(unsigned int)index_];
        }
        iterator &operator++() {
            ++index_;
            return *this;
        }
        bool operator==(const iterator &other) const {
            return owner_ == other.owner_ && index_ == other.index_;
        }
        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }
    private:
        lordrings *owner_;
        size_t index_;
    };

    iterator begin() {  // line 453
        return iterator(this, 0);
    }

    iterator end() {  // line 459
        return iterator(this, cycdeque_size_);
    }

    size_t cycdeque_start_;      // line 158
    size_t cycdeque_size_;       // line 161
    size_t cycdeque_capacity_;   // line 163
    T *value_ptr_;               // line 165
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BOOST_LORD_RINGS_H_H_
