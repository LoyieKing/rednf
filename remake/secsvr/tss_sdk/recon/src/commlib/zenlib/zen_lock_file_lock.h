// ZEN_File_Lock, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_lock_file_lock.cpp
//
// ORIG DWARF: sizeof(ZEN_File_Lock) == 8
//   +0x00 ZEN_HANDLE lock_file_hadle_
//   +0x04 bool       open_by_self_
// Non-polymorphic: the class derives from ZEN_NON_Copyable and emits only
// C1/C2 constructors and D1/D2 destructors (no virtual table).
#ifndef TSS_RECON_ZEN_LOCK_FILE_LOCK_H_
#define TSS_RECON_ZEN_LOCK_FILE_LOCK_H_

#include <sys/types.h>
#include "src/commlib/zenlib/zen_recon_support.h"

typedef int ZEN_HANDLE;

namespace ZEN_OS {
int open(const char *filename, int open_mode, unsigned int perms);
int close(int handle);
int flock(int handle, int operation);
}

class ZEN_File_Lock : public ZEN_NON_Copyable {
protected:
    ZEN_HANDLE lock_file_hadle_;
    bool open_by_self_;

public:
    ZEN_File_Lock(ZEN_HANDLE handle);
    ZEN_File_Lock();
    ~ZEN_File_Lock();

    int open(const char *file_name, int open_mode, mode_t perms);
    int open(ZEN_HANDLE handle);
    int close();
    ZEN_HANDLE get_file_handle();

    void lock_read();
    bool try_lock_read();
    void lock_write();
    bool try_lock_write();
    void unlock();
};

#endif // TSS_RECON_ZEN_LOCK_FILE_LOCK_H_
