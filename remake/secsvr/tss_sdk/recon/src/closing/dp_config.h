// Shared ABI declarations for the reconstructed dp_config.cpp
// (libtss_sdk.so.1.0.394, CU
//  src/formmog/tss_sdk/plugins/proto_proc/dp_config.cpp).
//
// This header is the integration contract for the remaining dp_config
// surface.  It deliberately reuses the already reconstructed configuration
// header (src/origsrc/formmog_conf_tsssdk_dp.h, generated from the ORIG's own
// DWARF) instead of declaring a second copy of DPConfig and friends.
//
// Layouts that are not part of that shared header are the ORIG DWARF values of
// this CU:
//   DP_CONFIG        280 bytes  (update_time +0, dp_config_path +20,
//                                now_time +276)
//   DP_CONFIG_BODY 22848 bytes  (cur_update_time_ +0, DPConfig +4; the tail
//                                padding is the natural 4-byte alignment)
//   ZEN_ShareMem_Mmap 16 bytes  (std::string +0, mmap_addr_ +4,
//                                mmap_handle_ +8, shm_size_ +12)
#ifndef TSS_RECON_CLOSING_DP_CONFIG_H_
#define TSS_RECON_CLOSING_DP_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "src/commlib/zenlib/zen_lock_file_lock.h"
#include "src/origsrc/formmog_conf_tsssdk_dp.h"

// ZEN_ShareMem_Mmap is declared here only as an ABI boundary: the actual
// implementation lives in the zenlib TU linked into the final library (and in
// the ORIG for the reference library).  ORIG DWARF: sizeof == 16, base
// ZEN_NON_Copyable @0 (empty-base optimised, so the members still start at
// +0), no virtual functions.
class ZEN_ShareMem_Mmap : public ZEN_NON_Copyable {
public:
    ZEN_ShareMem_Mmap();
    ~ZEN_ShareMem_Mmap();

    // ORIG mangling: _ZN17ZEN_ShareMem_Mmap4openEPKcjbbbPKvj
    // if_restore=true keeps an existing file (O_CREAT); false truncates
    // (O_CREAT|O_TRUNC), which is why get_comm_mem retries with false.
    int open(const char *file_name, unsigned int shm_size, bool if_restore,
             bool read_only, bool share_file, const void *want_address,
             unsigned int offset);
    void *addr();

private:
    std::string mmap_file_name_;  // +0x00
    void *mmap_addr_;             // +0x04
    int mmap_handle_;             // +0x08
    unsigned int shm_size_;       // +0x0c
};

namespace tss_dp_comm {

// The 280-byte header mapped from /tmp/dp_comm_config_head.  The three
// static members are the ORIG's exported globals at 0x002bb4c0..0x002bb4c8.
struct DP_CONFIG {
    unsigned int update_time;    // +0
    unsigned int revert;         // +4
    unsigned int game_id;        // +8
    unsigned int world_id;       // +12
    unsigned int secagent_id;    // +16
    char dp_config_path[256];    // +20
    unsigned int now_time;       // +276

    static DPConfig *glb_dp_config_;             // 0x002bb4c0
    static DP_CONFIG *glb_dp_config_head_;       // 0x002bb4c4
    static unsigned int glb_config_update_time_; // 0x002bb4c8
};

// The 22848-byte region mapped from /tmp/dp_comm_config.  glb_dp_config_
// points at &dp_config_ (i.e. at body + 4), exactly as the ORIG does.
struct DP_CONFIG_BODY {
    unsigned int cur_update_time_;  // +0
    DPConfig dp_config_;            // +4, 22842 bytes
};

// tss_dp_comm functions of the CU.  cmp_* and sort_dp_config are global
// namespace functions (their ORIG manglings do not contain a namespace).
void *get_comm_mem(ZEN_ShareMem_Mmap &mmap, const char *path, unsigned int size);
int get_cmd_id(const DPGameCmdStruct &cmd_struct, unsigned char *buf,
               unsigned short len, unsigned int &cmd_id);
// merge_path() and init_dp_config(DPCmdLists&) are file-local in the ORIG
// (local .symtab symbols); they are declared inside dp_config.cpp only.
int init_dp_config();
int do_reload_dp_config();
int reload_dp_config();

}  // namespace tss_dp_comm

int cmp_uint32(const void *lf, const void *rt);
int cmp_dpsubcmdlist(const void *lf, const void *rt);
// Defined by recon/src/formmog/tss_sdk/plugins/proto_proc/dp_hack_cmp_agent.cpp
// (same ORIG CU symbol, already present in the shared link).
int cmp_dphackfragment(const void *lf, const void *rt);
void sort_dp_config(tss_dp_comm::DPConfig &dp_config);

// Compile-time ABI checks for the layouts this CU dereferences.
typedef char dp_config_size_assert[sizeof(tss_dp_comm::DP_CONFIG) == 280 ? 1 : -1];
typedef char dp_config_body_size_assert[sizeof(tss_dp_comm::DP_CONFIG_BODY) == 22848 ? 1 : -1];
typedef char dp_config_path_offset_assert[offsetof(tss_dp_comm::DP_CONFIG, dp_config_path) == 20 ? 1 : -1];
typedef char dp_config_now_time_offset_assert[offsetof(tss_dp_comm::DP_CONFIG, now_time) == 276 ? 1 : -1];
typedef char dp_config_body_member_assert[offsetof(tss_dp_comm::DP_CONFIG_BODY, dp_config_) == 4 ? 1 : -1];
typedef char dp_config_globals_layout_assert[sizeof(tss_dp_comm::DP_CONFIG) == 280 ? 1 : -1];
typedef char zen_share_mem_mmap_size_assert[sizeof(ZEN_ShareMem_Mmap) == 16 ? 1 : -1];
typedef char dp_config_dpconfig_size_assert[sizeof(tss_dp_comm::DPConfig) == 22842 ? 1 : -1];

#endif  // TSS_RECON_CLOSING_DP_CONFIG_H_
