// Reconstructed from zergsvr DWARF + disassembly.
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_DEBUG_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_DEBUG_H_H_

#include "src/commlib/zenlib/zen_trace_log_basic.h"

const size_t SIZE_OF_BACKTRACE_FUNC = 0x20;

class _BACK_TRACE_STACK_SYMBOLS {
public:
    static void back_trace_stack(ZEN_LOG_PRIORITY log_priority);
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_DEBUG_H_H_
