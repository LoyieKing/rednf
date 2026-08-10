// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_shm.cpp
// Semantics verified against the oracle binary (thin syscall wrappers + POSIX shm name adaptation).

#define _FILE_OFFSET_BITS 64

#include "src/commlib/zenlib/zen_os_adapt_shm.h"

#include <stdio.h>
#include <string.h>

#ifndef ZEN_POSIX_MMAP_DIRECTORY
#define ZEN_POSIX_MMAP_DIRECTORY "/dev/shm/"
#endif

namespace ZEN_OS {

int shmctl(ZEN_HANDLE shmid, int cmd, struct shmid_ds *buf) {
    return ::shmctl(shmid, cmd, buf);
}

int shmdt(const void *shmaddr) {
    return ::shmdt(shmaddr);
}

void *shmat(ZEN_HANDLE shmid, const void *shmaddr, int shmflg) {
    return ::shmat(shmid, shmaddr, shmflg);
}

ZEN_HANDLE shmget(key_t sysv_key, size_t size, int oflag) {
    return ::shmget(sysv_key, size, oflag);
}

int shm_stat(char *mmap_file_name, struct stat *buf) {
    char shm_file_name[4096];
    // Original calls ZEN_OS::strncasecmp / ZEN_OS::stat (both verified thin
    // passthroughs to ::strncasecmp / ::stat with _FILE_OFFSET_BITS=64).
    if (strncasecmp(mmap_file_name, ZEN_POSIX_MMAP_DIRECTORY,
                    strlen(ZEN_POSIX_MMAP_DIRECTORY)) != 0) {
        snprintf(shm_file_name, sizeof(shm_file_name), "%s%s",
                 ZEN_POSIX_MMAP_DIRECTORY, mmap_file_name);
        mmap_file_name = shm_file_name;
    }
    return stat(mmap_file_name, buf);
}

int shm_unlink(const char *file_path) {
    char shm_file_name[4096];
    if (strncasecmp(file_path, ZEN_POSIX_MMAP_DIRECTORY,
                    strlen(ZEN_POSIX_MMAP_DIRECTORY)) == 0) {
        strncpy(shm_file_name, file_path + strlen(ZEN_POSIX_MMAP_DIRECTORY),
                strlen(file_path) - strlen(ZEN_POSIX_MMAP_DIRECTORY));
        file_path = shm_file_name;
    }
    return ::shm_unlink(file_path);
}

ZEN_HANDLE shm_open(const char *file_path, int mode, mode_t perms) {
    char shm_file_name[4096];
    if (strncasecmp(file_path, ZEN_POSIX_MMAP_DIRECTORY,
                    strlen(ZEN_POSIX_MMAP_DIRECTORY)) == 0) {
        // Original passes size 0xfff here (one less than the buffer), preserved.
        snprintf(shm_file_name, sizeof(shm_file_name) - 1, "%s",
                 file_path + strlen(ZEN_POSIX_MMAP_DIRECTORY));
        file_path = shm_file_name;
    }
    return ::shm_open(file_path, mode, perms);
}

int munmap(void *addr, size_t len) {
    return ::munmap(addr, len);
}

int msync(void *addr, size_t len, int sync) {
    return ::msync(addr, len, sync);
}

int mprotect(const void *addr, size_t len, int prot) {
    return ::mprotect(const_cast<void *>(addr), len, prot);
}

void *mmap(void *addr, size_t len, int prot, int flags, ZEN_HANDLE file_handle, size_t off) {
    return ::mmap(addr, len, prot, flags, file_handle, off);
}

} // namespace ZEN_OS
