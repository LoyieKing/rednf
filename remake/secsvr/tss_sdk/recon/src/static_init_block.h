// One-shot per-TU static-initialization block of the ORIG
// libtss_sdk.so.1.0.394.
//
// Every ORIG TSS SDK TU carries:
//   - std::ios_base::Init __ioinit       (from <iostream>; GCC 4.1)
//   - COMM_STAT_ITEM_WITH_NAME[16]       (ZEN_STATUS_ITEM_WITHNAME, ids
//                                         9101..9116 = 0x238d..0x239c)
// plus its module-specific namespace-scope objects.
//
// This header is for a .cpp that covers exactly one ORIG CU: it keeps the
// ORIG object names (COMM_STAT_ITEM_WITH_NAME / __ioinit).  A .cpp that
// merges several ORIG CUs must include static_init_block_impl.h once per ORIG
// CU with a unique TSS_SDK_STATIC_INIT_ID, so each CU keeps its own
// independent array and iostream initializer (see that header).
//
// The block uses the tag-394 `ZEN_STATUS_ITEM_WITHNAME` (92 bytes:
// statics_item_ + item_name_[65]) from
// recon/src/commlib/zenlib/zen_server_status.h, not the reuse tree's 64+1
// split model.  GCC 13's <iostream> no longer emits the per-TU __ioinit
// object, so it is declared explicitly (same name/type/size 1, dynamic init +
// __cxa_atexit) to reproduce the ORIG TU-local block.
//
// Include from a .cpp only, never from another header: the objects have
// internal linkage and must exist once per ORIG translation unit.  Do not
// include comm_stat_define.h in the same TU (duplicate array definition).
// Module-specific objects (e.g. Singleton<T>::ObjectCreator, g_tss_sdk,
// g_antibot_if) still have to be declared by the TU itself.
#ifndef TSS_SDK_RECON_STATIC_INIT_BLOCK_H_
#define TSS_SDK_RECON_STATIC_INIT_BLOCK_H_

#include "src/static_init_block_impl.h"

#endif  // TSS_SDK_RECON_STATIC_INIT_BLOCK_H_
