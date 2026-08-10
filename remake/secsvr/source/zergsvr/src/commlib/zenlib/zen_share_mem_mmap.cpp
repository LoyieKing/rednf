// Restored from gunnersvr DWARF + disassembly.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_share_mem_mmap.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// NOTE: the original header tree defines a global typedef `string` (= std::string,
// DWARF: typedef string at zen_os_adapt_predefine.h:60).  The DWARF stub headers no
// longer carry it, so it is re-declared here before including the class header.

#include <string>

typedef std::string string;

#include "src/commlib/zenlib/zen_share_mem_mmap.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>

// Declarations of the ZEN_OS adaptation layer (restored in separate TUs) and the
// trace-log entry used on the error paths.  Signatures follow the mangled names
// present in the gunnersvr binary.
namespace ZEN_OS {
int open(const char *path, int oflag, unsigned int mode);
int close(int fd);
int unlink(const char *path);
int filesize(int fd, unsigned int *file_size);
int ftruncate(int fd, unsigned int length);
void *mmap(void *address, unsigned int length, int prot, int flags, int fd,
           unsigned int offset);
int munmap(void *address, unsigned int length);
int msync(void *address, unsigned int length, int flags);
} // namespace ZEN_OS

class ZEN_Trace_LogMsg {
public:
    static void debug_errorex(const char *str_format, ...);
};

ZEN_ShareMem_Mmap::ZEN_ShareMem_Mmap() {
    mmap_addr_ = NULL;
    mmap_handle_ = -1;
    shm_size_ = 0;
}

ZEN_ShareMem_Mmap::~ZEN_ShareMem_Mmap() {
    if (mmap_addr_ != NULL) {
        close();
    }
}

// line 52
int ZEN_ShareMem_Mmap::open(const char *file_name, size_t shm_size,
                            int file_open_mode, int file_perms_mode,
                            const void *want_address, int mmap_prot,
                            int mmap_flags, size_t offset) {
    int ret;
    void *nonconst_addr;
    size_t filelen;

    mmap_handle_ = ZEN_OS::open(file_name, file_open_mode, file_perms_mode);
    if (mmap_handle_ == -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] Mmap(file map) memory open fail ,ZEN_OS::shm_open fail. last error =%d",
            errno);
        return -1;
    }

    mmap_file_name_.assign(file_name, strlen(file_name));

    if ((file_open_mode & O_TRUNC) != 0) {
        ret = ZEN_OS::ftruncate(mmap_handle_, shm_size + offset);
    } else {
        filelen = 0;
        ret = ZEN_OS::filesize(mmap_handle_, &filelen);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib]  Mmap(file map) memory  open fail ,ZEN_OS::file size ret =%ld last error=%d",
                filelen, errno);
            ZEN_OS::close(mmap_handle_);
            mmap_handle_ = -1;
            return -1;
        }
        if (filelen == 0) {
            ret = ZEN_OS::ftruncate(mmap_handle_, shm_size + offset);
        } else if (filelen < shm_size + offset) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib] Posix memory open fail ,old file size(%lu) < request file size(%lu). ",
                filelen, shm_size + offset);
            ZEN_OS::close(mmap_handle_);
            mmap_handle_ = -1;
            return -1;
        }
    }

    nonconst_addr = (void *)want_address;
    mmap_addr_ = ZEN_OS::mmap(nonconst_addr, shm_size, mmap_prot, mmap_flags,
                              mmap_handle_, offset);
    if (mmap_addr_ == NULL || mmap_addr_ == (void *)-1) {
        ZEN_OS::close(mmap_handle_);
        mmap_handle_ = -1;
        return -1;
    }

    shm_size_ = shm_size;
    return 0;
}

// line 145
int ZEN_ShareMem_Mmap::open(const char *file_name, size_t shm_size,
                            bool if_restore, bool read_only, bool share_file,
                            const void *want_address, size_t offset) {
    int file_open_mode;
    int mmap_prot;
    int mmap_flags;
    int file_perms_mode;

    file_perms_mode = share_file ? 0666 : 0600;
    mmap_flags = share_file ? MAP_SHARED : MAP_PRIVATE;
    file_open_mode =
        (if_restore ? O_CREAT : O_CREAT | O_TRUNC) | (read_only ? 0 : O_RDWR);
    mmap_prot = read_only ? PROT_READ : PROT_READ | PROT_WRITE;
    if (want_address != NULL) {
        mmap_flags |= MAP_FIXED;
    }
    return open(file_name, shm_size, file_open_mode, file_perms_mode,
                want_address, mmap_prot, mmap_flags, offset);
}

// line 203
int ZEN_ShareMem_Mmap::close() {
    int ret;

    ret = ZEN_OS::munmap(mmap_addr_, shm_size_);
    mmap_addr_ = NULL;
    ZEN_OS::close(mmap_handle_);
    mmap_handle_ = -1;
    return (ret == 0) ? 0 : -1;
}

// line 226
int ZEN_ShareMem_Mmap::remove() {
    return ZEN_OS::unlink(mmap_file_name_.c_str());
}

// line 232
int ZEN_ShareMem_Mmap::flush() {
    return ZEN_OS::msync(mmap_addr_, shm_size_, MS_SYNC);
}

// line 238
void *ZEN_ShareMem_Mmap::addr() {
    return mmap_addr_;
}
