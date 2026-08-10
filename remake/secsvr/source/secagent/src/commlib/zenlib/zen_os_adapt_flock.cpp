// Restored from secagent binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_os_adapt_flock.cpp
// fcntl(F_SETLK/F_SETLKW) record-lock wrappers over zen_flock_t.
// Original quirks preserved:
//   - flock_trywrlock/flock_tryrdlock always return 0; the caller must check
//     errno, and on EACCES/EAGAIN the errno is rewritten to 16 (EBUSY on
//     Linux i386; the original constant compiled to this value).
//   - flock_destroy returns 0 in every path.
//   - secagent's flock_destroy(zen_flock_t*) has no unlock step (the bool
//     parameter of the gunnersvr tag was removed).

#define _FILE_OFFSET_BITS 64

#include "src/commlib/zenlib/zen_os_adapt_flock.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>

namespace ZEN_OS {

ZEN_HANDLE open(const char *filename, int open_mode, mode_t perms);
int close(ZEN_HANDLE handle);

void flock_adjust_params(zen_flock_t *lock, int whence, size_t &start,
                         size_t &len) {
    lock->lock_.l_whence = whence;
    lock->lock_.l_start = start;
    lock->lock_.l_len = len;
}

int flock_init(zen_flock_t *lock, ZEN_HANDLE file_hadle) {
    lock->handle_ = file_hadle;
    return 0;
}

int flock(ZEN_HANDLE file_hadle, int operation) {
    return ::flock(file_hadle, operation);
}

int flock_wrlock(zen_flock_t *lock, int whence, size_t start, size_t len) {
    flock_adjust_params(lock, whence, start, len);
    lock->lock_.l_type = F_WRLCK;
    return fcntl(lock->handle_, F_SETLKW, &lock->lock_);
}

int flock_rdlock(zen_flock_t *lock, int whence, size_t start, size_t len) {
    flock_adjust_params(lock, whence, start, len);
    lock->lock_.l_type = F_RDLCK;
    return fcntl(lock->handle_, F_SETLKW, &lock->lock_);
}

int flock_unlock(zen_flock_t *lock, int whence, size_t start, size_t len) {
    flock_adjust_params(lock, whence, start, len);
    lock->lock_.l_type = F_UNLCK;
    return fcntl(lock->handle_, F_SETLK, &lock->lock_);
}

int flock_trywrlock(zen_flock_t *lock, int whence, size_t start, size_t len) {
    flock_adjust_params(lock, whence, start, len);
    lock->lock_.l_type = F_WRLCK;
    int result = fcntl(lock->handle_, F_SETLK, &lock->lock_);
    if (result == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            errno = 16; // original quirk: constant compiled to 16
        }
    }
    return 0;
}

int flock_tryrdlock(zen_flock_t *lock, int whence, size_t start, size_t len) {
    flock_adjust_params(lock, whence, start, len);
    lock->lock_.l_type = F_RDLCK;
    int result = fcntl(lock->handle_, F_SETLK, &lock->lock_);
    if (result == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            errno = 16; // original quirk: constant compiled to 16
        }
    }
    return 0;
}

int flock_destroy(zen_flock_t *lock) {
    if (lock->handle_ != -1 && lock->open_by_self_) {
        ZEN_OS::close(lock->handle_);
        lock->handle_ = -1;
    }
    return 0;
}

int flock_init(zen_flock_t *lock, const char *file_name, int flags,
               mode_t perms) {
    ZEN_HANDLE fd = ZEN_OS::open(file_name, flags, perms);
    if (fd == -1) {
        return -1;
    }
    lock->open_by_self_ = true;
    return flock_init(lock, fd);
}

} // namespace ZEN_OS
