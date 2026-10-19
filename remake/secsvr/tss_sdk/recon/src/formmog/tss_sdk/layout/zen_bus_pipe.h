// Canonical layout of the zenlib two-way bus pipe used by the channel plugin.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'ZEN_BusPipe_TwoWay'
//     struct ZEN_BusPipe_TwoWay size=10292 bases=1
//       base +0 ZEN_Bus_MMAPPipe (10292)
//       no own data members; methods ZEN_BusPipe_TwoWay(), ~ZEN_BusPipe_TwoWay(),
//       initialize(...), pop_front_recvpipe(...), pop_front_sendpipe(...);
//       static two_way_instance_, const BUS_PIPE_NAME.
//   python3 audit/tu_report.py --type 'ZEN_Bus_MMAPPipe'
//     struct ZEN_Bus_MMAPPipe size=10292
//       base +0 ZEN_NON_Copyable (1, empty-base optimised)
//       +0     bus_mmap_name_     char[4097]
//       +4100  bus_head_          ZEN_BUS_PIPE_HEAD (4104)
//       +8204  bus_pipe_pointer_  shm_dequechunk*[512]  (2048)
//       +10252 mmap_file_         ZEN_ShareMem_Mmap (16)
//       +10268 posix_file_        ZEN_ShareMem_Posix (16)
//       +10284 mmap_addr_         void*
//       +10288 sharemem_type_     unsigned char
//       nested ZEN_BUS_PIPE_HEAD 4104:
//         +0 size_of_sizet_ u32, +4 number_of_pipe_ u32,
//         +8 size_of_pipe_ u32[512], +2056 size_of_room_ u32[512]
//   ZEN_ShareMem_Mmap / ZEN_ShareMem_Posix size=16 (base ZEN_NON_Copyable):
//     +0 std::string name (libstdc++ 4.1 4-byte string), +4 void* mmap_addr_,
//     +8 int mmap_handle_, +12 unsigned int shm_size_.
//
// These declarations live here because no shared header carries them yet: the
// only other copies are the local models inside
// plugins/channel_plugin_agent.cpp and plugins/proto_proc/channel_misc_agent.cpp,
// which also redefine ZEN_Thread_Mutex.  Those two TUs are the wave-2 migration
// input for this header (and for layout/tss_sdk_channel.h).
//   // TODO(migrate): channel_plugin_agent.cpp, channel_misc_agent.cpp
#ifndef TSS_SDK_LAYOUT_ZEN_BUS_PIPE_H_
#define TSS_SDK_LAYOUT_ZEN_BUS_PIPE_H_

#include <stddef.h>
#include <string>

#include <sys/types.h>

#include "src/commlib/zenlib/zen_server_status.h"  // ZEN_ShareMem_Posix
#include "src/closing/dp_config.h"                // ZEN_ShareMem_Mmap
#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace ZEN_LIB {
// zen_bus_mmap_pipe.h: 24-byte pipe node (base 4 + payload).
class shm_dequechunk;
}  // namespace ZEN_LIB

// ZEN_ShareMem_Mmap and ZEN_ShareMem_Posix are declared once by
// src/closing/dp_config.h and src/commlib/zenlib/zen_server_status.h
// respectively; both are included above instead of being repeated here, so
// this header carries no zenlib duplicate.

// 10292 bytes; zen_bus_mmap_pipe.h.
class ZEN_Bus_MMAPPipe : public ZEN_NON_Copyable {
public:
    struct ZEN_BUS_PIPE_HEAD {          // 4104 bytes
        unsigned int size_of_sizet_;      // +0
        unsigned int number_of_pipe_;     // +4 (size_t)
        unsigned int size_of_pipe_[512];  // +8
        unsigned int size_of_room_[512];  // +2056

        ZEN_BUS_PIPE_HEAD();
        ~ZEN_BUS_PIPE_HEAD();
    };

    // No virtual functions in ORIG: bus_mmap_name_ sits at +0, so the class
    // has no vptr (the model reports vfuncs=0 for ZEN_Bus_MMAPPipe).
    ZEN_Bus_MMAPPipe();
    ~ZEN_Bus_MMAPPipe();

private:
    char bus_mmap_name_[4097];                            // +0x0000
    ZEN_BUS_PIPE_HEAD bus_head_;                          // +0x1004
    ZEN_LIB::shm_dequechunk *bus_pipe_pointer_[512];      // +0x200c
    ZEN_ShareMem_Mmap mmap_file_;                         // +0x280c
    ZEN_ShareMem_Posix posix_file_;                       // +0x281c
    void *mmap_addr_;                                     // +0x282c
    unsigned char sharemem_type_;                         // +0x2830

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, bus_mmap_name_, 0,
                                zen_bus_mmappipe_name_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, bus_head_, 4100,
                                zen_bus_mmappipe_head_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, bus_pipe_pointer_, 8204,
                                zen_bus_mmappipe_pointer_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, mmap_file_, 10252,
                                zen_bus_mmappipe_mmap_file_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, posix_file_, 10268,
                                zen_bus_mmappipe_posix_file_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, mmap_addr_, 10284,
                                zen_bus_mmappipe_addr_offset);
        TSS_SDK_OFFSET_CONTRACT(ZEN_Bus_MMAPPipe, sharemem_type_, 10288,
                                zen_bus_mmappipe_sharemem_type_offset);
    }
};

// 10292 bytes; zen_bus_two_way.h.  Derives from ZEN_Bus_MMAPPipe and adds no
// data members.
class ZEN_BusPipe_TwoWay : public ZEN_Bus_MMAPPipe {
public:
    ZEN_BusPipe_TwoWay();
    ~ZEN_BusPipe_TwoWay();

    int initialize(const char *bus_name, unsigned int send_bus_size,
                   unsigned int recv_bus_size, unsigned int max_node_size,
                   bool creator, bool lock_free, unsigned char sharemem_type);
};

TSS_SDK_STATIC_ASSERT(sizeof(ZEN_ShareMem_Posix) == 16,
                      zen_sharemem_posix_size_must_be_16);
TSS_SDK_STATIC_ASSERT(sizeof(ZEN_Bus_MMAPPipe::ZEN_BUS_PIPE_HEAD) == 4104,
                      zen_bus_pipe_head_size_must_be_4104);
TSS_SDK_STATIC_ASSERT(sizeof(ZEN_Bus_MMAPPipe) == 10292,
                      zen_bus_mmappipe_size_must_be_10292);
TSS_SDK_STATIC_ASSERT(sizeof(ZEN_BusPipe_TwoWay) == 10292,
                      zen_buspipe_twoway_size_must_be_10292);

#endif  // TSS_SDK_LAYOUT_ZEN_BUS_PIPE_H_
