// BusChannelIdMgr -- reconstructed from libtss_sdk.so.1.0.394, CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//       src/formmog/appcomm/app_bus_channel_id_mgr.cpp
// Code range 0x0018c930-0x0018d29e (18 symbols, see
// AGENT_MATRIX_app_bus_channel_id_mgr.tsv).
//
// The CU's DWARF carries the complete class layout and the original
// declaration lines; both are reproduced below.
//
//   sizeof(BusChannelIdMgr) == 24, non-polymorphic
//     +0x00 uint32_t        used_svr_id_
//     +0x04 bool            is_init_
//     +0x08 unsigned int    max_channel_id_   <- NOT initialised by the ctor
//     +0x0c uint32_t *      channel_info_
//     +0x10 ZEN_File_Lock * lock_
//     +0x14 bool            is_use_pid_
//
// is_valid_channel_id() is declared in the class (line 57 of the CU's DWARF)
// but has no definition anywhere in the library, hence it is only declared
// here as well.
#ifndef TSS_RECON_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_
#define TSS_RECON_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_

#include <stdint.h>
#include <stdio.h>

#include "src/commlib/zenlib/zen_lock_file_lock.h"

class BusChannelIdMgr {
public:
    BusChannelIdMgr();                                     // line 17
    ~BusChannelIdMgr();                                    // line 18

    int init(unsigned int game_id_count);                  // line 27
    int get_bus_channel_id();                              // line 34
    int get_bus_channel_id(unsigned int game_channel_id);  // line 35

private:
    int init_bus_channel_info_list();                      // line 38
    int read_channel_info_from_file_without_lock(FILE *fp);   // line 41
    int write_channel_info_to_file_without_lock(FILE *fp);    // line 42
    int count_channel_info_size() const;                      // line 45
    int find_bus_channel_id_from_mem(unsigned int game_channel_id);   // 48
    int alloc_bus_channel_id_from_mem(unsigned int game_channel_id);  // 51
    int alloc_bus_channel_id_according_pid(unsigned int game_channel_id);
                                                              // line 53
    void free_svr_id();                                       // line 55
    bool is_valid_channel_id(int bus_channel_id);              // line 57

    uint32_t used_svr_id_;          // +0x00
    bool is_init_;                  // +0x04
    unsigned int max_channel_id_;   // +0x08 (never touched by the ctor)
    uint32_t *channel_info_;        // +0x0c
    ZEN_File_Lock *lock_;           // +0x10
    bool is_use_pid_;               // +0x14

    BusChannelIdMgr(const BusChannelIdMgr &);             // line 78
    BusChannelIdMgr &operator=(const BusChannelIdMgr &);  // line 79
};

#endif // TSS_RECON_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_
