// Semantic reconstruction of the tss_dpsdk::DPCodeMgr code-table manager from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// Original translation unit:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr.cpp
// Original header:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr.h
//
// ORIG ranges covered here (virtual addresses, half-open, sizes from nm -S):
//   CodeInfo::load_cfg(char const*)              0x000a7460..0x000a7619
//   DPCodeMgr::reload_code()                     0x000a7620..0x000a79ee
//   DPCodeMgr::reload()                          0x000a79f0..0x000a7a70
//   DPCodeMgr::init()                            0x000a7a70..0x000a7b38
//   CodeItems<2000>::load_cfg(char const*)       0x000a7b40..0x000a7d1c
//   CodeItems<20000>::load_cfg(char const*)      0x000a7d20..0x000a7efc
//   read_file(char const*, unsigned char*, uint) 0x000a73d0..0x000a745d
//   read_file(char const*, ID_SET&)              inlined only
//
// DpCodeInfo::load_cfg(char const*) has no out-of-line symbol; it is inlined
// into reload_code() at 0xa76af..0xa79c6 and is reconstructed as an inline
// member in this file.
//
// The file IO, shared-memory mapping and lock helpers are external boundaries;
// their declarations produce the exact mangled references from the original.
// No code-table contents are fabricated: the loaders only parse the on-disk
// config format that the original parses.

#define _FILE_OFFSET_BITS 64

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <set>

// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h); the local stub is gone.
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ---------------------------------------------------------------------------
// External ZEN boundaries.  The local models are gone: the canonical tag-394
// headers are the single definitions in the build.
//   src/commlib/zenlib/zen_lock_file_lock.h  ZEN_File_Lock    (8 bytes)
//   src/commlib/zenlib/zen_lock_guard.h      ZEN_Write_Guard  (4 bytes)
//   src/closing/dp_config.h                  ZEN_ShareMem_Mmap (16 bytes),
//                                            tss_dp_comm::DPConfig /
//                                            DP_CONFIG / get_comm_mem
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_lock_file_lock.h"
#include "src/commlib/zenlib/zen_lock_guard.h"
#include "src/closing/dp_config.h"

// The shared mapping object has an unqualified symbol in the ORIG.
static ZEN_ShareMem_Mmap s_dp_code_info_;

namespace tss_dpsdk {

// dp_code_mgr.cpp:47.
typedef std::set<unsigned int> ID_SET;

// File-scope state.  Symbols/storage in the original:
//   _ZN9tss_dpsdk19dp_code_config_pathE  .data.rel.ro 0x00292340 (4 bytes)
//   _ZN9tss_dpsdk12dp_code_lockE         .bss         0x002bb4b0 (8 bytes)
//   s_dp_code_info_                      .bss         0x002bb4a0 (16 bytes)
static const char *dp_code_config_path __attribute__((used)) =
    "/tmp/dp_code_config";
static ZEN_File_Lock dp_code_lock;

// dp_code_mgr.cpp:33 / :48.  Both have internal linkage in the ORIG.  The
// first is emitted at 0xa73d0; the second is inlined into reload_code().
static int read_file(const char *path, unsigned char *buf, size_t len) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        ZEN_Trace_LogMsg::debug_errorex("DPCodeMgr read_file fail, %s", path);
        return -1;
    }
    int ret = (int)fread(buf, 1, len, f);
    fclose(f);
    return ret;
}

static int read_file(const char *path, ID_SET &store) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        ZEN_Trace_LogMsg::debug_errorex("DPCodeMgr read_file fail, %s", path);
        return -1;
    }
    unsigned int id = 0;
    while (!feof(f)) {
        if (fscanf(f, "%u ", &id) == 1)
            store.insert(id);
    }
    fclose(f);
    return 0;
}

class DPCodeMgr {
public:
    // dp_code_mgr.h:111.  sizeof = 4 + N.
    template <int N>
    struct CodeItem {
        unsigned short weight_;        // +0
        unsigned short code_len_;      // +2
        unsigned char code_buf_[N];    // +4
    };

    // dp_code_mgr.h:119.  sizeof = 2 + 100 * (4 + N).
    template <int N>
    struct CodeItems {
        unsigned short total_weight_;  // +0
        CodeItem<N> items_[100];       // +2

        int load_cfg(const char *dir); // dp_code_mgr.h:152
    };

    // dp_code_mgr.h:156.  sizeof = 2401206.
    struct CodeInfo {
        CodeItems<2000> cryptdata_code_; // +0
        CodeItems<2000> crypt_code_;     // +200402 (0x30ed2)
        CodeItems<20000> bin_file_;      // +400804 (0x61da4)

        int load_cfg(const char *dir);   // dp_code_mgr.h:198
    };

    // dp_code_mgr.h:202.  sizeof = 4802464 (0x4947a0).
    struct DpCodeInfo {
        unsigned short version_count_;         // +0
        unsigned int version_support_list_[8]; // +4
        unsigned char version_hash_[16];       // +36 (0x24)
        CodeInfo client_code_;                 // +52 (0x34)
        CodeInfo server_code_;                 // +2401258 (0x24a3ea)

        // dp_code_mgr.h:213.  Inlined into reload_code() in the ORIG.
        inline int load_cfg(const char *dir);
    };

    // dp_code_mgr.h:229.  sizeof = 9604936 (0x928f48).
    struct DpCodeInfos {
        unsigned int code_update_time_; // +0
        unsigned short code_count_;     // +4
        DpCodeInfo code_infos_[2];      // +8
    };

    static const unsigned int MAX_VERSION_COUNT = 8;
    static const unsigned int MAX_VERSION_CODE = 2;

    DPCodeMgr() : code_infos_(0) {}
    ~DPCodeMgr() {}

    int init();        // dp_code_mgr.cpp:69
    int reload();      // dp_code_mgr.cpp:91

private:
    int reload_code(); // dp_code_mgr.cpp:101

    DpCodeInfos *code_infos_; // +0
};

// ---------------------------------------------------------------------------
// DpCodeInfo::load_cfg -- dp_code_mgr.cpp:135, inlined at :117.
//
// Directory layout read from <dir>:
//   <dir>/versionhash.cfg  16-byte hash
//   <dir>/version.ini      whitespace-separated unsigned versions
//   <dir>/servercodedata/servercode...  CodeInfo (dir.cfg driven)
//   <dir>/clientcodedata/clientcode...  CodeInfo (dir.cfg driven)
// ---------------------------------------------------------------------------
inline int DPCodeMgr::DpCodeInfo::load_cfg(const char *dir) {
    char base_dir[256];
    ID_SET versions;

    snprintf(base_dir, 0xff, "%s/versionhash.cfg", dir);
    base_dir[0xff] = '\0';
    if (read_file(base_dir, version_hash_, sizeof(version_hash_)) <= 0)
        return -1;

    snprintf(base_dir, 0xff, "%s/version.ini", dir);
    base_dir[0xff] = '\0';
    read_file(base_dir, versions);

    for (ID_SET::const_iterator it = versions.begin();
         it != versions.end() && version_count_ < MAX_VERSION_COUNT; ++it) {
        version_support_list_[version_count_] = *it;
        ++version_count_;
    }

    snprintf(base_dir, 0xff, "%s/servercodedata", dir);
    base_dir[0xff] = '\0';
    if (server_code_.load_cfg(base_dir) != 0)
        return -1;

    snprintf(base_dir, 0xff, "%s/clientcodedata", dir);
    base_dir[0xff] = '\0';
    return client_code_.load_cfg(base_dir) == 0 ? 0 : -1;
}

// ---------------------------------------------------------------------------
// CodeItems<N>::load_cfg -- dp_code_mgr.cpp:229.
//
// dir.cfg line format: "<file name> <code id> <weight>"
// The referenced file is read into items_[code_id].code_buf_; the item is
// accepted only for 1 <= size < N, code_id <= 99 and a still-empty slot.
// ---------------------------------------------------------------------------
template <int N>
int DPCodeMgr::CodeItems<N>::load_cfg(const char *dir) {
    char dir_cfg[256];
    snprintf(dir_cfg, sizeof(dir_cfg), "%s/dir.cfg", dir);

    FILE *fp = fopen(dir_cfg, "rb");
    if (!fp) {
        ZEN_Trace_LogMsg::debug_errorex("DPCodeMgr read_file fail, %s", dir_cfg);
        return -1;
    }

    int ret = 0;
    while (!feof(fp)) {
        if (!fgets(dir_cfg, sizeof(dir_cfg), fp))
            break;

        unsigned int code_id;
        unsigned int weight;
        char one_file[256];
        if (sscanf(dir_cfg, "%s %u %u", one_file, &code_id, &weight) != 3)
            continue;
        if (code_id > 99)
            continue;

        snprintf(dir_cfg, sizeof(dir_cfg), "%s/%s", dir, one_file);
        int size = read_file(dir_cfg, items_[code_id].code_buf_, N);
        if (size <= 0 || size >= N || items_[code_id].code_len_ != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "DPCodeMgr read_file fail,dir=%s, dir_cfg=%s, size=%d, "
                "code_id=%d, old_len=%d",
                dir, dir_cfg, size, code_id, items_[code_id].code_len_);
            ret = -1;
            break;
        }
        items_[code_id].code_len_ = (unsigned short)size;
        items_[code_id].weight_ = (unsigned short)weight;
        total_weight_ = (unsigned short)(total_weight_ + weight);
    }

    fclose(fp);
    return ret;
}

// ---------------------------------------------------------------------------
// CodeInfo::load_cfg -- dp_code_mgr.cpp:174.
//
// dir.cfg line format: "<file name> <weight> <type>"
//   type 1 -> bin_file_
//   type 2 -> cryptdata_code_
//   type 3 -> crypt_code_
// The observed ORIG multiplies cryptdata_code_.total_weight_ by <weight> for
// both type 2 and type 3; the type-3 multiplier therefore targets the
// cryptdata table, not crypt_code_.  That behavior is preserved here.
// ---------------------------------------------------------------------------
int DPCodeMgr::CodeInfo::load_cfg(const char *dir) {
    char dir_cfg[256];
    snprintf(dir_cfg, sizeof(dir_cfg), "%s/dir.cfg", dir);

    FILE *fp = fopen(dir_cfg, "rb");
    if (!fp) {
        ZEN_Trace_LogMsg::debug_errorex("DPCodeMgr read_file fail, %s", dir_cfg);
        return -1;
    }

    int ret = 0;
    while (!feof(fp)) {
        if (!fgets(dir_cfg, sizeof(dir_cfg), fp))
            break;

        char one_dir[256];
        int weight;
        int dir_type;
        if (sscanf(dir_cfg, "%s %d %d", one_dir, &weight, &dir_type) != 3) {
            ret = 0;
            continue;
        }

        snprintf(dir_cfg, sizeof(dir_cfg), "%s/%s", dir, one_dir);
        switch (dir_type) {
        case 1:
            bin_file_.load_cfg(dir_cfg);
            ret = 0;
            break;
        case 2:
            ret = cryptdata_code_.load_cfg(dir_cfg);
            cryptdata_code_.total_weight_ =
                (unsigned short)(cryptdata_code_.total_weight_ * weight);
            break;
        case 3:
            ret = crypt_code_.load_cfg(dir_cfg);
            cryptdata_code_.total_weight_ =
                (unsigned short)(cryptdata_code_.total_weight_ * weight);
            break;
        default:
            ret = 0;
            break;
        }

        if (ret != 0)
            break;
    }

    fclose(fp);
    return ret;
}

// ---------------------------------------------------------------------------
// DPCodeMgr::reload_code -- dp_code_mgr.cpp:101.
//
// No-op while the shared block already reflects the config head's update
// time.  Otherwise parse every configured code directory into a zeroed
// temporary DpCodeInfos, copy it into the shared block only on success, then
// publish the update time.  The update time is published even on failure.
// ---------------------------------------------------------------------------
int DPCodeMgr::reload_code() {
    if (code_infos_->code_update_time_ ==
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->update_time) {
        return 0;
    }

    DpCodeInfos *temp_config = new DpCodeInfos;
    memset(temp_config, 0, sizeof(DpCodeInfos));

    int ret = 0;
    // ORIG also caps the loop at 8 even though dp_code_info carries only two
    // 256-byte directory strings.
    for (unsigned int i = 0;
         i < tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.code_dir_count &&
         i < 8;
         ++i) {
        const char *dir = tss_dp_comm::DP_CONFIG::glb_dp_config_->
                              dp_code_info.code_dir_path[i];
        if (temp_config->code_infos_[i].load_cfg(dir) != 0) {
            ret = -1;
            break;
        }
        ++temp_config->code_count_;
    }

    if (ret == 0)
        memcpy(code_infos_, temp_config, sizeof(DpCodeInfos));

    delete temp_config;
    code_infos_->code_update_time_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->update_time;
    return ret;
}

// ---------------------------------------------------------------------------
// DPCodeMgr::reload -- dp_code_mgr.cpp:91.
// ---------------------------------------------------------------------------
int DPCodeMgr::reload() {
    if (code_infos_->code_update_time_ ==
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->update_time) {
        return 0;
    }

    ZEN_Write_Guard<ZEN_File_Lock> locker(dp_code_lock);
    return reload_code();
}

// ---------------------------------------------------------------------------
// DPCodeMgr::init -- dp_code_mgr.cpp:69.
//
// open the code-config lock file, take the read lock, map the shared
// DpCodeInfos block named by the same path, and load it.  The shared-memory
// size is sizeof(DpCodeInfos) = 0x928f48.
// ---------------------------------------------------------------------------
int DPCodeMgr::init() {
    int ret = dp_code_lock.open(dp_code_config_path, 0x42, 0x1b0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("open lock fail. ret=%d", ret);
        return -1;
    }

    ZEN_Write_Guard<ZEN_File_Lock> locker(dp_code_lock);
    code_infos_ = (DpCodeInfos *)tss_dp_comm::get_comm_mem(
        s_dp_code_info_, dp_code_config_path, sizeof(DpCodeInfos));
    if (!code_infos_)
        return -1;

    reload_code();
    return 0;
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Layout assertions (DWARF sizes/offsets).
// ---------------------------------------------------------------------------
typedef char check_codeitem_2000[
    sizeof(tss_dpsdk::DPCodeMgr::CodeItem<2000>) == 2004 ? 1 : -1];
typedef char check_codeitem_20000[
    sizeof(tss_dpsdk::DPCodeMgr::CodeItem<20000>) == 20004 ? 1 : -1];
typedef char check_codeitems_2000[
    sizeof(tss_dpsdk::DPCodeMgr::CodeItems<2000>) == 200402 ? 1 : -1];
typedef char check_codeitems_20000[
    sizeof(tss_dpsdk::DPCodeMgr::CodeItems<20000>) == 2000402 ? 1 : -1];
typedef char check_codeinfo[
    sizeof(tss_dpsdk::DPCodeMgr::CodeInfo) == 2401206 ? 1 : -1];
typedef char check_dpcodeinfo[
    sizeof(tss_dpsdk::DPCodeMgr::DpCodeInfo) == 4802464 ? 1 : -1];
typedef char check_dpcodeinfos[
    sizeof(tss_dpsdk::DPCodeMgr::DpCodeInfos) == 9604936 ? 1 : -1];
typedef char check_dpcodeinfo_offsets[
    (__builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo, version_support_list_) == 4 &&
     __builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo, version_hash_) == 0x24 &&
     __builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo, client_code_) == 0x34 &&
     __builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo, server_code_) == 0x24a3ea)
        ? 1 : -1];
typedef char check_dpcodeinfos_offsets[
    (__builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfos, code_count_) == 4 &&
     __builtin_offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfos, code_infos_) == 8)
        ? 1 : -1];
typedef char check_config_offsets[
    (__builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info) == 0x154 &&
     __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info.code_dir_count) == 0x3976 &&
     __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info.code_dir_path) == 0x3978 &&
     sizeof(tss_dp_comm::DPConfig) == 0x593a &&
     sizeof(tss_dp_comm::DPCodeInfo) == 0x3a24)
        ? 1 : -1];
typedef char check_zen_sizes[
    sizeof(ZEN_File_Lock) == 8 && sizeof(ZEN_ShareMem_Mmap) == 16 ? 1 : -1];

// Explicit emission of the ORIG weak CodeItems<20000>::load_cfg symbol; the
// class is otherwise unused in this TU and GCC does not emit the weak method.
template int tss_dpsdk::DPCodeMgr::CodeItems<20000>::load_cfg(const char *);

// ---------------------------------------------------------------------------
// ORIG CU dp_code_mgr.cpp static-initialization block.
//
// Decoded ORIG order (audit/report/init_brief.md, 25 calls, target size 966):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   ZEN_ShareMem_Mmap::ZEN_ShareMem_Mmap()   (s_dp_code_info_, 0x002bb4a0)
//   ZEN_File_Lock::ZEN_File_Lock()           (dp_code_lock, 0x002bb4b0)
//   + one atexit per object, then the COMM_STAT dtor registration and the
//   _Unwind_Resume pad.
//
// dp_code_config_path (ORIG .data.rel.ro 0x00292340, const char* =
// "/tmp/dp_code_config"), s_dp_code_info_ and dp_code_lock are already
// defined above with the ORIG names/types.
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"
