// Tag-394 model of the shared-memory base class declared by the ORIG
// src/commlib/zenlib/zen_shm_predefine.h.
//
// ORIG DWARF (libtss_sdk.so.1.0.394, every CU that defines it):
//   ZEN_LIB::_shm_memory_base   size 4, member `smem_base_` (ptr) @0,
//                               no vptr, no other member.
// The secagent copy of zen_shm_predefine.h models the *gunnersvr* variant,
// where the class is polymorphic (vptr + smem_base_ = 8 bytes) and every
// derived shm container is 4 bytes larger.  It is therefore not usable here;
// this header carries the 394 layout instead.  Keep exactly one definition:
// include this header from every recon TU that needs the shm containers.
#ifndef TSS_RECON_ZEN_SHM_MODELS_H_
#define TSS_RECON_ZEN_SHM_MODELS_H_

#include <stddef.h>   // offsetof

namespace ZEN_LIB {

struct _shm_memory_base {
    void *smem_base_;   // +0x00
};

} // namespace ZEN_LIB

// Layout contract: a negative bound fails the build on drift.
typedef char tss_recon_shm_memory_base_size[
    sizeof(ZEN_LIB::_shm_memory_base) == 4 ? 1 : -1];
typedef char tss_recon_shm_memory_base_off[
    offsetof(ZEN_LIB::_shm_memory_base, smem_base_) == 0 ? 1 : -1];

#endif // TSS_RECON_ZEN_SHM_MODELS_H_
