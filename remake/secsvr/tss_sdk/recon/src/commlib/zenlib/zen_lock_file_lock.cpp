// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_lock_file_lock.cpp
// Address range 0x0019fcd0-0x001a01d7.
//
// All locking goes through ZEN_OS::flock(); the operation constants are the
// ORIG immediates: 1 LOCK_SH, 2 LOCK_EX, 5 LOCK_SH|LOCK_NB, 6 LOCK_EX|LOCK_NB,
// 8 LOCK_UN.  Every failure logs through ZEN_Trace_LogMsg::debug_output with
// the exact ORIG format string and fail-info text.
#include "src/commlib/zenlib/zen_lock_file_lock.h"

#include <errno.h>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace {

const char *const ZEN_FILE_LOCK_SOURCE =
    "/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/"
    "src/commlib/zenlib/zen_lock_file_lock.cpp";

} // namespace

ZEN_File_Lock::ZEN_File_Lock(ZEN_HANDLE handle)
    : lock_file_hadle_(handle), open_by_self_(false) {
}

ZEN_File_Lock::ZEN_File_Lock()
    : lock_file_hadle_(-1), open_by_self_(false) {
}

ZEN_File_Lock::~ZEN_File_Lock() {
    close();
}

int ZEN_File_Lock::open(const char *file_name, int open_mode, mode_t perms) {
    lock_file_hadle_ = ZEN_OS::open(file_name, open_mode, perms);
    if (lock_file_hadle_ == -1) {
        return -1;
    }
    open_by_self_ = true;
    return 0;
}

int ZEN_File_Lock::open(ZEN_HANDLE handle) {
    lock_file_hadle_ = handle;
    return 0;
}

int ZEN_File_Lock::close() {
    unlock();
    if (open_by_self_) {
        ZEN_OS::close(lock_file_hadle_);
    }
    return 0;
}

ZEN_HANDLE ZEN_File_Lock::get_file_handle() {
    return lock_file_hadle_;
}

void ZEN_File_Lock::lock_read() {
    int ret = ZEN_OS::flock(lock_file_hadle_, 1 /* LOCK_SH */);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_FILE_LOCK_SOURCE, 94,
                               "void ZEN_File_Lock::lock_read()",
                               "ZEN_OS::flock LOCK_SH", ret);
    }
}

bool ZEN_File_Lock::try_lock_read() {
    int ret = ZEN_OS::flock(lock_file_hadle_, 5 /* LOCK_SH|LOCK_NB */);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_FILE_LOCK_SOURCE, 109,
                               "bool ZEN_File_Lock::try_lock_read()",
                               "ZEN_OS::flock LOCK_SH|LOCK_NB", ret);
        return false;
    }
    return true;
}

void ZEN_File_Lock::lock_write() {
    int ret = ZEN_OS::flock(lock_file_hadle_, 2 /* LOCK_EX */);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_FILE_LOCK_SOURCE, 124,
                               "void ZEN_File_Lock::lock_write()",
                               "ZEN_OS::flock LOCK_EX", ret);
    }
}

bool ZEN_File_Lock::try_lock_write() {
    int ret = ZEN_OS::flock(lock_file_hadle_, 6 /* LOCK_EX|LOCK_NB */);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_FILE_LOCK_SOURCE, 137,
                               "bool ZEN_File_Lock::try_lock_write()",
                               "ZEN_OS::flock LOCK_EX|LOCK_NB", ret);
        return false;
    }
    return true;
}

void ZEN_File_Lock::unlock() {
    int ret = ZEN_OS::flock(lock_file_hadle_, 8 /* LOCK_UN */);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_FILE_LOCK_SOURCE, 153,
                               "void ZEN_File_Lock::unlock()",
                               "ZEN_OS::flock LOCK_UN", ret);
    }
}
