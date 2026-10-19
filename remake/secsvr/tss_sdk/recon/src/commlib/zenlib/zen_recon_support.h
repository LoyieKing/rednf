// Declarations shared by the reconstructed zenlib translation units in this
// directory.
//
// The types that the ORIG src/commlib/zenlib headers describe are NOT
// re-declared here any more: the secagent headers match the ORIG tag-394
// DWARF (size, member offsets, base classes, virtual order) and are included
// directly, so the reconstruction cannot drift from them:
//
//   ZEN_NON_Copyable  sizeof 1  src/commlib/zenlib/zen_boost_non_copyable.h
//   ZEN_Time_Value    sizeof 8  src/commlib/zenlib/zen_time_value.h
//                               (timeval zen_time_value_ @0)
//   ZEN_Lock_Base     sizeof 4  src/commlib/zenlib/zen_lock_base.h
//                               (ZEN_NON_Copyable base, vptr @0, 14 virtuals
//                               in ORIG order: ~, lock, try_lock, unlock,
//                               systime_lock, duration_lock, lock_read,
//                               try_lock_read, timed_lock_read,
//                               duration_lock_read, lock_write,
//                               try_lock_write, timed_lock_write,
//                               duration_lock_write; _ZTV = 0x44)
//
// Headers that must stay recon-local because the secagent copy models the
// (different) gunnersvr variant:
//   src/commlib/zenlib/zen_lock_thread_mutex.h  (recon header, 394)
//   src/commlib/zenlib/zen_shm_models.h         (recon header, 394)
//   src/commlib/zenlib/zen_lock_guard.h         (recon header, 394)
//   src/commlib/zenlib/zen_trace_log_msg.h      (recon header, single logging
//                                                boundary; see that file)
#ifndef TSS_RECON_ZENLIB_SUPPORT_H_
#define TSS_RECON_ZENLIB_SUPPORT_H_

#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_lock_base.h"

// ---------------------------------------------------------------------------
// logging (zen_trace_log_basic.cpp / zen_trace_log_msg.cpp)
// ---------------------------------------------------------------------------
// ZEN_LOG_PRIORITY and ZEN_Trace_LogMsg live in exactly one header:
// src/commlib/zenlib/zen_trace_log_msg.h.  That header also records why the
// ORIG's 364-byte class (libstdc++ 4.1 std::ofstream tail) cannot be modelled
// by a host-built TU, and why the static-only boundary is the drift-free
// choice.  Local per-TU models must be replaced by this include.
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// The ORIG error-return macro expands to one debug_output call:
//   [FAIL RETRUN]Fail in file [%s|%d],function:%s,fail info:%s,return %d,last error %d.
#define ZEN_RECON_ERROR_RETURN(priority, file, line, function, info, ret)     \
    ZEN_Trace_LogMsg::debug_output((ZEN_LOG_PRIORITY)(priority),            \
        "[FAIL RETRUN]Fail in file [%s|%d],function:%s,fail info:%s,"        \
        "return %d,last error %d.",                                          \
        (file), (line), (function), (info), (int)(ret), errno)

// Layout contracts (ORIG DWARF).  A negative bound fails the build on drift.
typedef char tss_recon_non_copyable_size[
    sizeof(ZEN_NON_Copyable) == 1 ? 1 : -1];
typedef char tss_recon_time_value_size[sizeof(ZEN_Time_Value) == 8 ? 1 : -1];
typedef char tss_recon_lock_base_size[sizeof(ZEN_Lock_Base) == 4 ? 1 : -1];

#endif // TSS_RECON_ZENLIB_SUPPORT_H_
