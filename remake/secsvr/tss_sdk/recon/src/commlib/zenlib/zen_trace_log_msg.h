// Tag-394 declaration of the ZEN logging boundary
// (ORIG src/commlib/zenlib/zen_trace_log_msg.h + zen_trace_log_basic.h).
//
// WHY THIS HEADER EXISTS (single authoritative declaration for wave-2):
// The ORIG class is `struct ZEN_Trace_LogMsg : public ZEN_LogTrace_Basic` with
// byte_size 364; ZEN_LogTrace_Basic's tail member is a libstdc++ 4.1
// `std::ofstream`, which is 276 bytes there and a completely different size in
// host GCC 13.  Therefore no host-built translation unit can reproduce 364,
// and the ORIG TUs that merely *call* the static entry points prove the point:
// the whole ORIG image contains exactly ONE sized ZEN_Trace_LogMsg DIE (CU
// zen_trace_log_msg.cpp); every calling TU has no DIE for the type at all.
//
// Consequence for the reconstruction: the only safe, drift-free declaration is
// this static-only boundary with the exact ORIG member signatures.  Include it
// (instead of a per-TU local model) wherever the logging entry points are
// used; do not invent members for the unreachable 364-byte tail.  The one
// 364-byte definition in the image stays the c5/GCC-4.1-built reuse TU
// zen_trace_log_msg.cpp, which is where the ORIG layout actually lives.
//
// This path shadows the secagent copy of zen_trace_log_msg.h for TUs compiled
// with `-I recon`; the secagent copy (gunnersvr flavour, includes
// zen_lock_thread_mutex.h with the 394-incompatible constructor set) must not
// be used by recon TUs.
#ifndef TSS_RECON_ZEN_TRACE_LOG_MSG_H_
#define TSS_RECON_ZEN_TRACE_LOG_MSG_H_

#include <stdarg.h>
#include <stddef.h>

// ORIG enumerator values (DWARF, zen_trace_log_basic.h):
// RS_TRACE 1, RS_DEBUG 2, RS_INFO 3, RS_ERROR 4, RS_ALERT 5, RS_FATAL 6.
// The ZEN_LOG_PRIORITY_LEVEL_* entries are recon-local spellings kept for
// not-yet-migrated TUs; they carry the same numeric values.
//
// The enum is declared once per TU: the reused zenlib header
// (src/commlib/zenlib/zen_trace_log_basic.h) declares the same ORIG enum
// (six ORIG names) and both headers are now co-includable by comm_cfgsvr_sdk.h.
#ifndef TSS_ZEN_LOG_PRIORITY_DECLARED
#define TSS_ZEN_LOG_PRIORITY_DECLARED
enum ZEN_LOG_PRIORITY {
    RS_TRACE = 1,
    RS_DEBUG = 2,
    RS_INFO = 3,
    RS_ERROR = 4,
    RS_ALERT = 5,
    RS_FATAL = 6,

    ZEN_LOG_PRIORITY_LEVEL_0 = 0,
    ZEN_LOG_PRIORITY_LEVEL_1 = 1,
    ZEN_LOG_PRIORITY_LEVEL_2 = 2,
    ZEN_LOG_PRIORITY_LEVEL_3 = 3,
    ZEN_LOG_PRIORITY_ERROR = 4
};
#endif  // TSS_ZEN_LOG_PRIORITY_DECLARED

class ZEN_Trace_LogMsg {
public:
    static void debug_output(ZEN_LOG_PRIORITY dbglevel, const char *str_format, ...);
    static void debug_traceex(const char *str_format, ...);
    static void debug_debugEx(const char *str_format, ...);
    static void debug_infoex(const char *str_format, ...);
    static void debug_errorex(const char *str_format, ...);
    static void debug_alertex(const char *str_format, ...);
    static void debug_fatalex(const char *str_format, ...);
    static void debug_assert(const char *file_name, int file_line,
                             const char *function_name,
                             const char *expression_name);
    static void debug_assert_ex(const char *file_name, int file_line,
                                const char *function_name,
                                const char *expression_name,
                                const char *out_string);
    static ZEN_Trace_LogMsg *instance();
    static void instance(ZEN_Trace_LogMsg *instance);
    static void clean_instance();

private:
    static ZEN_Trace_LogMsg *log_instance_;   // ORIG: declared only here
};

#endif // TSS_RECON_ZEN_TRACE_LOG_MSG_H_
