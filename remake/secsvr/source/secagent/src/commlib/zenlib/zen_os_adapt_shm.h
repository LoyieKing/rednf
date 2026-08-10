// Restored header for zen_os_adapt_shm.cpp (gunnersvr oracle).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SHM_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SHM_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

#include <stddef.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace ZEN_OS {
    extern int shmctl(ZEN_HANDLE shmid, int cmd, struct shmid_ds *buf);
    extern int shmdt(const void *shmaddr);
    extern void *shmat(ZEN_HANDLE shmid, const void *shmaddr, int shmflg);
    extern ZEN_HANDLE shmget(key_t sysv_key, size_t size, int oflag);
    extern int shm_stat(char *mmap_file_name, struct stat *buf);
    extern int shm_unlink(const char *file_path);
    extern ZEN_HANDLE shm_open(const char *file_path, int mode, mode_t perms);
    extern int munmap(void *addr, size_t len);
    extern int msync(void *addr, size_t len, int sync);
    extern int mprotect(const void *addr, size_t len, int prot);
    extern void *mmap(void *addr, size_t len, int prot, int flags, ZEN_HANDLE file_handle, size_t off);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SHM_H_H_
