// BusChannelIdMgr -- reconstructed from libtss_sdk.so.1.0.394, CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//       src/formmog/appcomm/app_bus_channel_id_mgr.cpp
// Code range 0x0018c930-0x0018d29e.  Every function below was recovered from
// its disassembly (objdump -dl of the ORIG) and from the CU's DWARF locals;
// see AGENT_REPORT_app_bus_channel_id_mgr.md for the per-function evidence.
//
// Shipped behaviour that is reproduced verbatim (do not "fix"):
//   * the default ctor does not initialise max_channel_id_ (+0x08);
//   * read/write use the object's first dword (used_svr_id_) as the 4-byte
//     file header, not max_channel_id_;
//   * the shared/exclusive file lock used here is ZEN_File_Lock::lock_read()
//     (flock LOCK_SH) even though the guard class is called ZEN_Write_Guard;
//   * init() leaks lock_ (and never allocates channel_info_) if the lock file
//     cannot be opened;
//   * init_bus_channel_info_list() does not fclose() the new file when the
//     initial write fails;
//   * get_bus_channel_id() returns -4 when not initialised while
//     get_bus_channel_id(id) returns -1.

// The ORIG TU was compiled with _FILE_OFFSET_BITS=64: it calls fopen64()
// (e.g. 0x18ccc6) and ZEN_OS::stat() receives a 96-byte struct stat with
// st_size at +44 (CU DWARF: zen_os_stat, size 96).  Keep it in the file so the
// object is correct under the plain c5build.sh command line as well.
#define _FILE_OFFSET_BITS 64

#include "src/appcomm/app_bus_channel_id_mgr.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <new>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "src/commlib/zenlib/zen_lock_file_lock.h"

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
// TSS_SVR_ID_POOL_FILE / TSS_SVR_ID_POOL_LOCK_FILE are already defined at
// namespace scope below (ORIG .data.rel.ro 0x00292350 / 0x00292354).
#include "src/ioinit_block.h"

// _ZN6ZEN_OS4statEPKcP4stat -- ZEN_OS wrapper of stat(2) (ORIG 0x001a2570).
namespace ZEN_OS {
int stat(const char *path, struct stat *stat_buf);
}

// zen_os_adapt_predefine.h:13 -- typedef stat zen_os_stat;
typedef struct stat zen_os_stat;

// ---------------------------------------------------------------------------
// ZEN_Write_Guard<LOCK> -- output/commlib/zenlib/release/include/
// zen_lock_guard.h, class at line 148 (sizeof 4, base ZEN_NON_Copyable):
//   lock_            (protected, ZEN_File_Lock *)
//   ctor             line 151  -- body line 154: lock_->lock_read()
//   ctor(LOCK&, bool) line 158 -- not used by this CU, body not recoverable here
//   dtor             line 168  -- body line 170: lock_->unlock()
//   lock_write()     line 174
//   try_lock_write() line 179
//   unlock()         line 185
//
// All three use sites of this CU (0x18cccc, 0x18cec2, 0x18d132) inline ctor
// line 151 and call ZEN_File_Lock::lock_read()@plt, i.e. the guard takes the
// *shared* lock although it is called ZEN_Write_Guard; lock_write() is never
// called through it in this CU.  (zen_lock_guard.h has not been reconstructed
// yet, hence the local model; it belongs in commlib/zenlib once it is.)
// ---------------------------------------------------------------------------
template <typename LOCK>
class ZEN_Write_Guard : public ZEN_NON_Copyable {
public:
    explicit ZEN_Write_Guard(LOCK &lock) : lock_(&lock) {
        lock_->lock_read();
    }

    // The ORIG inlines this dtor unconditionally (no NULL test: 0x18d1e2 and
    // 0x18d1a8 are a bare `call ZEN_File_Lock::unlock()@plt`), which holds for
    // the reference ctor used everywhere in this CU.
    ~ZEN_Write_Guard() {
        lock_->unlock();
    }

    void lock_write() { lock_->lock_write(); }
    bool try_lock_write() { return lock_->try_lock_write(); }
    void unlock() { lock_->unlock(); }

protected:
    LOCK *lock_;
};

// File-scope constants of the ORIG TU (local 'd' symbols in .data.rel.ro at
// 0x00292350 / 0x00292354, DWARF decl lines 16/17, type `const char *const`).
// They are at global scope with the ORIG names (the ORIG symbols are local
// but not in an anonymous namespace), and `used` keeps them emitted although
// every use is inlined - the per-CU variable inventory and the DWARF
// DW_OP_addr locations are part of the static-initialization contract.
const char *const TSS_SVR_ID_POOL_FILE __attribute__((used)) =
    "/tmp/tss_sdk_game_svr_id_pool_file";
const char *const TSS_SVR_ID_POOL_LOCK_FILE __attribute__((used)) =
    "/tmp/tss_sdk_game_svr_id_pool_file.lock";

// ---------------------------------------------------------------------------
// is_process_active -- file-static helper of the ORIG TU (DWARF line 21,
// local rc at line 38).  Inlined into alloc_bus_channel_id_according_pid
// (0x18cc38..0x18cc51: kill(pid, 0), `add $0x1,%eax; jne` == rc != -1).
// ---------------------------------------------------------------------------
static bool is_process_active(pid_t pid) {
    int rc = kill(pid, 0);
    return (rc != -1);
}

// ===========================================================================
// ctor 0x0018c930 / 0x0018c960   line 51
// Stores 0 to used_svr_id_, is_init_, lock_, channel_info_, is_use_pid_;
// max_channel_id_ (+0x08) is deliberately left untouched (lines 53-57).
// ===========================================================================
BusChannelIdMgr::BusChannelIdMgr() {
    used_svr_id_ = 0;
    is_init_ = false;
    lock_ = NULL;
    channel_info_ = NULL;
    is_use_pid_ = false;
}

// ===========================================================================
// dtor 0x0018d1e0 / 0x0018d240   line 60
// ===========================================================================
BusChannelIdMgr::~BusChannelIdMgr() {
    if (is_use_pid_) {
        free_svr_id();
    }
    if (lock_ != NULL) {
        delete lock_;
    }
    if (channel_info_ != NULL) {
        delete[] channel_info_;
    }
}

// ===========================================================================
// init 0x0018d040   line 72
//
// new ZEN_File_Lock() (operator new(8) + C1, 0x18cfeb..0x18d00f), then
// lock_->open(TSS_SVR_ID_POOL_LOCK_FILE, 0x42 == O_RDWR|O_CREAT,
//             0x1b0 == 0660)
// On failure: debug_errorex("open lock file fail. %s", <lock path>) and -2
// (lock_ is *not* deleted -> leak, channel_info_ stays NULL).
// Success: max_channel_id_ = arg, channel_info_ = new unsigned int[arg],
//          init_bus_channel_info_list() (return value discarded), return 0.
// ===========================================================================
int BusChannelIdMgr::init(unsigned int game_id_count) {
    lock_ = new ZEN_File_Lock();
    if (lock_->open(TSS_SVR_ID_POOL_LOCK_FILE, O_RDWR | O_CREAT, 0660) != 0) {
        ZEN_Trace_LogMsg::debug_errorex("open lock file fail. %s",
                                        TSS_SVR_ID_POOL_LOCK_FILE);
        return -2;
    }

    max_channel_id_ = game_id_count;
    channel_info_ = new unsigned int[game_id_count];

    init_bus_channel_info_list();
    return 0;
}

// ===========================================================================
// get_bus_channel_id(unsigned) 0x0018cd90   line 95
//
//  is_init_ == false                     -> -1
//  fopen64(TSS_SVR_ID_POOL_FILE, "rb+") == NULL -> -2
//  read_channel_info_from_file_without_lock != 0 -> fclose, -3
//  find() > 0 -> that id; else alloc(); if alloc() > 0 write the file back
//  -> fclose, return the id (negative on failure)
// The ZEN_Write_Guard at line 103 releases the lock on every exit path.
// ===========================================================================
int BusChannelIdMgr::get_bus_channel_id(unsigned int game_channel_id) {
    if (!is_init_) {
        return -1;
    }

    ZEN_Write_Guard<ZEN_File_Lock> scope_lock(*lock_);

    FILE *fp = fopen(TSS_SVR_ID_POOL_FILE, "rb+");
    if (fp == NULL) {
        return -2;
    }

    int ret = read_channel_info_from_file_without_lock(fp);
    if (ret != 0) {
        fclose(fp);
        return -3;
    }

    int channel_id = find_bus_channel_id_from_mem(game_channel_id);
    if (channel_id <= 0) {
        channel_id = alloc_bus_channel_id_from_mem(game_channel_id);
        if (channel_id > 0) {
            write_channel_info_to_file_without_lock(fp);
        }
    }

    fclose(fp);
    return channel_id;
}

// ===========================================================================
// alloc_bus_channel_id_according_pid 0x0018cbe0   line 139
//
// Fast path (line 142): used_svr_id_ + 1 <= max_channel_id_ -> append and
// return the new count.  Otherwise scan for a pool slot whose pid is no
// longer alive (is_process_active(), inlined, lines 38/39), claim it
// (line 158) and return index + 1 (line 159).  -1 when the pool is full.
// ===========================================================================
int BusChannelIdMgr::alloc_bus_channel_id_according_pid(
    unsigned int game_channel_id) {
    if (used_svr_id_ + 1 <= max_channel_id_) {
        channel_info_[used_svr_id_] = game_channel_id;
        return (int)++used_svr_id_;
    }

    for (unsigned int i = 0; i < max_channel_id_; ++i) {
        if (is_process_active((pid_t)channel_info_[i])) {
            continue;
        }
        channel_info_[i] = game_channel_id;
        return (int)(i + 1);
    }
    return -1;
}

// ===========================================================================
// get_bus_channel_id() 0x0018cc70   line 168
//
//  is_use_pid_ = true (line 171), pid = getpid() (line 174)
//  is_init_ == false                     -> -4
//  fopen64(..., "rb+") == NULL           -> -2
//  read_channel_info_from_file_without_lock != 0 -> fclose, -3
//  find(pid) > 0 -> that id, else alloc_according_pid(pid); on success write
//  the file back; fclose; return the id
// ===========================================================================
int BusChannelIdMgr::get_bus_channel_id() {
    is_use_pid_ = true;

    pid_t pid = getpid();

    if (!is_init_) {
        return -4;
    }

    ZEN_Write_Guard<ZEN_File_Lock> scope_lock(*lock_);

    FILE *fp = fopen(TSS_SVR_ID_POOL_FILE, "rb+");
    if (fp == NULL) {
        return -2;
    }

    int ret = read_channel_info_from_file_without_lock(fp);
    if (ret != 0) {
        fclose(fp);
        return -3;
    }

    int channel_id = find_bus_channel_id_from_mem((unsigned int)pid);
    if (channel_id <= 0) {
        channel_id = alloc_bus_channel_id_according_pid((unsigned int)pid);
        if (channel_id > 0) {
            write_channel_info_to_file_without_lock(fp);
        }
    }

    fclose(fp);
    return channel_id;
}

// ===========================================================================
// free_svr_id 0x0018d100   line 218
//
//  is_init_ == false        -> return (line 220)
//  fopen64(..., "rb+") == NULL -> return (line 230)
//  read error               -> fclose, return (line 236)
//  find(getpid()) > 0       -> clear the slot, shrink used_svr_id_ when the
//                              cleared slot was the last one, write the file
//  fclose (line 263)
// ===========================================================================
void BusChannelIdMgr::free_svr_id() {
    if (!is_init_) {
        return;
    }

    ZEN_Write_Guard<ZEN_File_Lock> scope_lock(*lock_);

    FILE *fp = fopen(TSS_SVR_ID_POOL_FILE, "rb+");
    if (fp == NULL) {
        return;
    }

    int ret = read_channel_info_from_file_without_lock(fp);
    if (ret == 0) {
        pid_t pid = getpid();
        int channel_id = find_bus_channel_id_from_mem((unsigned int)pid);
        if (channel_id > 0) {
            channel_info_[channel_id - 1] = 0;
            if ((unsigned int)channel_id == used_svr_id_) {
                used_svr_id_ = (unsigned int)(channel_id - 1);
            }
            write_channel_info_to_file_without_lock(fp);
        }
    }

    fclose(fp);
}

// ===========================================================================
// init_bus_channel_info_list 0x0018cea0   line 270
//
//  is_init_ = false (line 272), guard taken (line 274)
//  stat(TSS_SVR_ID_POOL_FILE, &st) == 0 && st.st_size != count_channel_info_size()
//      -> unlink(TSS_SVR_ID_POOL_FILE)            (line 280)
//  fopen64(..., "rb+") != NULL -> fclose, is_init_ = true, 0   (lines 283-310)
//  fopen64(..., "wb+") == NULL -> debug_errorex("open %s fail. err=%d:%s",
//                                  path, errno, strerror(errno)), -1
//  used_svr_id_ = 0 (line 295), memset(channel_info_, 0, max_channel_id_ * 4)
//      (line 296), write_channel_info_to_file_without_lock (line 297)
//  write error -> debug_errorex("write init data to %s fail. ret=%d", path,
//                  ret), -2, and the new file is *not* closed
//  fclose, is_init_ = true, 0
// ===========================================================================
int BusChannelIdMgr::init_bus_channel_info_list() {
    is_init_ = false;

    ZEN_Write_Guard<ZEN_File_Lock> scope_lock(*lock_);

    zen_os_stat st;
    if (ZEN_OS::stat(TSS_SVR_ID_POOL_FILE, &st) == 0) {
        if (st.st_size != count_channel_info_size()) {
            unlink(TSS_SVR_ID_POOL_FILE);
        }
    }

    FILE *fp = fopen(TSS_SVR_ID_POOL_FILE, "rb+");
    if (fp != NULL) {
        fclose(fp);
        is_init_ = true;
        return 0;
    }

    fp = fopen(TSS_SVR_ID_POOL_FILE, "wb+");
    if (fp == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("open %s fail. err=%d:%s",
                                        TSS_SVR_ID_POOL_FILE, errno,
                                        strerror(errno));
        return -1;
    }

    used_svr_id_ = 0;
    memset(channel_info_, 0, max_channel_id_ * 4);

    int ret = write_channel_info_to_file_without_lock(fp);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("write init data to %s fail. ret=%d",
                                        TSS_SVR_ID_POOL_FILE, ret);
        return -2;
    }

    fclose(fp);
    is_init_ = true;
    return 0;
}

// ===========================================================================
// read_channel_info_from_file_without_lock 0x0018cb60   line 312
//
// fread(this, 4, 1, fp) <= 0 -> -1; fread(channel_info_, 4 * max_channel_id_,
// 1, fp) > 0 -> 0 else -2.  DWARF local: read_count (line 314).
// ===========================================================================
int BusChannelIdMgr::read_channel_info_from_file_without_lock(FILE *fp) {
    int read_count = 0;

    if (fread(&used_svr_id_, 4, 1, fp) <= 0) {
        return -1;
    }

    read_count = (int)fread(channel_info_, 4 * max_channel_id_, 1, fp);
    return (read_count > 0) ? 0 : -2;
}

// ===========================================================================
// write_channel_info_to_file_without_lock 0x0018cac0   line 332
//
// fseek(fp, 0, SEEK_SET); fwrite(this, 4, 1, fp) != 1 -> -1;
// fwrite(channel_info_, 4 * max_channel_id_, 1, fp) == 1 -> 0 else -2.
// DWARF local: write_count (line 336).
// ===========================================================================
int BusChannelIdMgr::write_channel_info_to_file_without_lock(FILE *fp) {
    fseek(fp, 0, SEEK_SET);

    int write_count = 0;
    if (fwrite(&used_svr_id_, 4, 1, fp) != 1) {
        return -1;
    }

    write_count = (int)fwrite(channel_info_, 4 * max_channel_id_, 1, fp);
    return (write_count == 1) ? 0 : -2;
}

// ===========================================================================
// count_channel_info_size 0x0018c990   line 354
// `lea 0x4(,%eax,4),%eax` == 4 + max_channel_id_ * 4.
// ===========================================================================
int BusChannelIdMgr::count_channel_info_size() const {
    return 4 + (int)(max_channel_id_ * 4);
}

// ===========================================================================
// alloc_bus_channel_id_from_mem 0x0018c9b0   line 359
// ret = -1; if (used_svr_id_ + 1 <= max_channel_id_) { append; ret = ++n; }
// ===========================================================================
int BusChannelIdMgr::alloc_bus_channel_id_from_mem(
    unsigned int game_channel_id) {
    int ret = -1;
    if (used_svr_id_ + 1 <= max_channel_id_) {
        channel_info_[used_svr_id_] = game_channel_id;
        ret = (int)++used_svr_id_;
    }
    return ret;
}

// ===========================================================================
// find_bus_channel_id_from_mem 0x0018c9e0   line 373
// Linear scan of the used part of the pool; returns index + 1 or -1.
// ===========================================================================
int BusChannelIdMgr::find_bus_channel_id_from_mem(
    unsigned int game_channel_id) {
    for (uint32_t i = 0; i < used_svr_id_; ++i) {
        if (channel_info_[i] == game_channel_id) {
            return (int)(i + 1);
        }
    }
    return -1;
}
