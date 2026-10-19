// Parameterised per-ORIG-CU static-initialization block of the ORIG
// libtss_sdk.so.1.0.394.
//
// Every ORIG TSS SDK translation unit carries its own, independent:
//   - std::ios_base::Init __ioinit       (from <iostream>; GCC 4.1)
//   - COMM_STAT_ITEM_WITH_NAME[16]       (ZEN_STATUS_ITEM_WITHNAME, ids
//                                         9101..9116 = 0x238d..0x239c)
// A reconstruction TU that merges several ORIG CUs must emit one block per
// ORIG CU, not one shared block; audit/accept.py compares the independent
// construction count of every ORIG CU.  Define TSS_SDK_STATIC_INIT_ID to a
// unique identifier fragment before including this file.  The generated
// objects carry that suffix:
//   COMM_STAT_ITEM_WITH_NAME_<id>, COMM_STAT_FRATURE_NUM_<id>, __ioinit_<id>
// With no TSS_SDK_STATIC_INIT_ID defined the ORIG names are kept, which is
// what a TU covering a single ORIG CU wants; static_init_block.h is that
// one-shot form.
//
// This file intentionally has no include guard: it must be includable several
// times from the same .cpp.  It uses the tag-394
// ZEN_STATUS_ITEM_WITHNAME (92 bytes: statics_item_ + item_name_[65]) from
// the canonical zen_server_status.h.  Do not include comm_stat_define.h in
// the same TU (duplicate array definition).
//
// Include from a .cpp only, never from another header: the objects have
// internal linkage.  Module-specific objects (e.g.
// Singleton<T>::ObjectCreator, g_tss_sdk, g_antibot_if) still have to be
// declared by the TU itself.
#ifndef TSS_SDK_STATIC_INIT_ID
#define TSS_SDK_STATIC_INIT_ID
#define TSS_SDK_RECON_STATIC_INIT_ID_WAS_DEFAULT
#endif

#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_server_status.h"

#define TSS_SDK_RECON_STATIC_INIT_CAT_(a, b) a##b
#define TSS_SDK_RECON_STATIC_INIT_CAT(a, b) TSS_SDK_RECON_STATIC_INIT_CAT_(a, b)
#define TSS_SDK_RECON_STATIC_INIT_NAME(a) \
    TSS_SDK_RECON_STATIC_INIT_CAT(a, TSS_SDK_STATIC_INIT_ID)

static const unsigned int
    TSS_SDK_RECON_STATIC_INIT_NAME(COMM_STAT_FRATURE_NUM) = 16;

static ZEN_STATUS_ITEM_WITHNAME
    TSS_SDK_RECON_STATIC_INIT_NAME(COMM_STAT_ITEM_WITH_NAME)
        [TSS_SDK_RECON_STATIC_INIT_NAME(COMM_STAT_FRATURE_NUM)] = {
    ZEN_STATUS_ITEM_WITHNAME(0x238d, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x238e, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x238f, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2390, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x2391, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2392, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_TIMEOUT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2393, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_ERRNO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2394, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_USE_TIME"),
    ZEN_STATUS_ITEM_WITHNAME(0x2395, STATICS_PER_FIVE_MINTUES, "COMM_STAT_APP_ALIVE"),
    ZEN_STATUS_ITEM_WITHNAME(0x2396, STATICS_PER_FIVE_MINTUES, "COMM_STAT_BILL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2397, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2398, STATICS_PER_FIVE_MINTUES, "COMM_STAT_PROCESS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2399, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_MEM_USE_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x239a, STATICS_PER_FIVE_MINTUES, "COMM_STAT_AVAILABLE_MEM_SIZE"),
    ZEN_STATUS_ITEM_WITHNAME(0x239b, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_BYTES"),
    ZEN_STATUS_ITEM_WITHNAME(0x239c, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_BYTES"),
};

static std::ios_base::Init
    TSS_SDK_RECON_STATIC_INIT_NAME(__ioinit);

#undef TSS_SDK_RECON_STATIC_INIT_NAME
#undef TSS_SDK_RECON_STATIC_INIT_CAT
#undef TSS_SDK_RECON_STATIC_INIT_CAT_
#ifdef TSS_SDK_RECON_STATIC_INIT_ID_WAS_DEFAULT
#undef TSS_SDK_STATIC_INIT_ID
#undef TSS_SDK_RECON_STATIC_INIT_ID_WAS_DEFAULT
#endif
