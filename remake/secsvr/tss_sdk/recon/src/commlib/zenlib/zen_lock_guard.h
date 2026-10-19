// Tag-394 lock guards, reconstructed from libtss_sdk.so.1.0.394.
//
// ORIG DWARF (identical layout for every instantiation):
//   ZEN_Lock_Guard<ZEN_Thread_Mutex>      size 4
//   ZEN_Read_Guard<ZEN_Thread_RW_Mutex>   size 4
//   ZEN_Write_Guard<ZEN_Thread_RW_Mutex>  size 4
//   ZEN_Write_Guard<ZEN_File_Lock>        size 4
//     { base ZEN_NON_Copyable @0, member `lock_` (MUTEX*) @0 }
//   ZEN_Lock_Guard<ZEN_Thread_Mutex> CUs include comm_socket_channel.cpp;
//   ZEN_Read_Guard<ZEN_Thread_RW_Mutex> and ZEN_Write_Guard<...> appear in
//   comm_socket_channel.cpp and comm_socket_channel_info.cpp.
//
// Locking behaviour verified from the ORIG disassembly of
// SocketChannelBusinessInfo::get_first_valid_handler() (0x000d8f30): it
// dispatches ZEN_Lock_Base slot 7 (`lock_read`) on the object at +0x0c and
// slot 4 (`unlock`) on the same object, i.e. the read guard takes lock_read()
// and releases with unlock().
//
// The secagent zen_lock_guard.h only models ZEN_Lock_Guard; this header is the
// complete 394 model and takes precedence for recon translation units.
#ifndef TSS_RECON_ZEN_LOCK_GUARD_H_
#define TSS_RECON_ZEN_LOCK_GUARD_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"

template <typename MUTEX>
class ZEN_Lock_Guard : public ZEN_NON_Copyable {
public:
    explicit ZEN_Lock_Guard(MUTEX &lock) : lock_(&lock) {
        lock_->lock();
    }

    ZEN_Lock_Guard(MUTEX *lock) : lock_(lock) {
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

template <typename MUTEX>
class ZEN_Read_Guard : public ZEN_NON_Copyable {
public:
    explicit ZEN_Read_Guard(MUTEX &lock) : lock_(&lock) {
        lock_->lock_read();
    }

    ZEN_Read_Guard(MUTEX *lock) : lock_(lock) {
        if (lock_ != 0) {
            lock_->lock_read();
        }
    }

    ~ZEN_Read_Guard() {
        if (lock_ != 0) {
            lock_->unlock();
        }
    }

    void lock() {
        lock_->lock_read();
    }

    bool try_lock() {
        return lock_->try_lock_read();
    }

    void unlock() {
        lock_->unlock();
    }

protected:
    MUTEX *lock_;
};

// ZEN_Write_Guard<MUTEX> -- ORIG tag-394 zen_lock_guard.h:148, base
// ZEN_NON_Copyable, protected `MUTEX *lock_` at :193.
//
// The one-argument constructor (:151, body :154) takes the *shared* lock:
// every ZEN_Write_Guard<ZEN_File_Lock> instance in the image inlines
// `ZEN_File_Lock::lock_read()@plt` (dp_code_mgr.cpp:80/95, dp_config.cpp,
// BusChannelIdMgr::get_bus_channel_id 0x18cccc/0x18cec2/0x18d132) and every
// ZEN_Write_Guard<ZEN_Thread_RW_Mutex> instance dispatches ZEN_Lock_Base
// slot 7, i.e. ZEN_Thread_RW_Mutex::lock_read (vtable 0x293cc8+0x1c ->
// 0x1a1b40): SocketChannel::fini 0xd76b4, SocketChannel::init_connection
// 0xd7e9f.  The image contains no call site of ZEN_File_Lock::lock_write()
// and no virtual dispatch of slot 11 (lock_write) on this mutex, so the guard
// is a shared-lock guard despite its name; the destructor (:168, body :170)
// calls unlock().  The `lock_write()/try_lock_write()` wrappers (:174/:179,
// renamed here to the ORIG member names) are never called in the image.
template <typename MUTEX>
class ZEN_Write_Guard : public ZEN_NON_Copyable {
public:
    explicit ZEN_Write_Guard(MUTEX &lock) : lock_(&lock) {
        lock_->lock_read();
    }

    ZEN_Write_Guard(MUTEX *lock) : lock_(lock) {
        if (lock_ != 0) {
            lock_->lock_read();
        }
    }

    ~ZEN_Write_Guard() {
        if (lock_ != 0) {
            lock_->unlock();
        }
    }

    void lock_write() {
        lock_->lock_write();
    }

    bool try_lock_write() {
        return lock_->try_lock_write();
    }

    void unlock() {
        lock_->unlock();
    }

protected:
    MUTEX *lock_;
};

#endif // TSS_RECON_ZEN_LOCK_GUARD_H_
