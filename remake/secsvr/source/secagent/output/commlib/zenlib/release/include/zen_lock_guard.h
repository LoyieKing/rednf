// Reconstructed from secagent binary (DWARF, 2026-08-10)
// ZEN_Lock_Guard<MUTEX>：sizeof=4，仅持有 MUTEX*，构造加锁、析构解锁。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"

template<typename MUTEX>
class ZEN_Lock_Guard : public ZEN_NON_Copyable {
public:
    explicit ZEN_Lock_Guard(MUTEX &lock)
        : lock_(&lock) {
        lock_->lock();
    }

    ZEN_Lock_Guard(MUTEX *lock)
        : lock_(lock) {
        if (lock_ != 0) {
            lock_->lock();
        }
    }

    ~ZEN_Lock_Guard() {
        if (lock_ != 0) {
            lock_->unlock();
        }
    }

    void lock() {
        lock_->lock();
    }

    bool try_lock() {
        return lock_->try_lock();
    }

    void unlock() {
        lock_->unlock();
    }

protected:
    MUTEX *lock_;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_
