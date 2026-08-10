// Restored from zergsvr binary (DWARF CU zen_trace_log_debug.cpp).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_trace_log_debug.cpp

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_debug.h"
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void _BACK_TRACE_STACK_SYMBOLS::back_trace_stack(ZEN_LOG_PRIORITY log_priority) {
    void *backtrace_buffer[SIZE_OF_BACKTRACE_FUNC];
    char **symbols_strings;
    int sz_of_stack;
    int j;

    ZEN_Trace_LogMsg::debug_output(log_priority, "[zenlib][BACKTRACE]This program compiled by GCC.");
    sz_of_stack = backtrace(backtrace_buffer, SIZE_OF_BACKTRACE_FUNC);
    ZEN_Trace_LogMsg::debug_output(log_priority, "[zenlib] [BACKTRACE] backtrace() returned %d addresses.", sz_of_stack);
    symbols_strings = backtrace_symbols(backtrace_buffer, sz_of_stack);
    if (symbols_strings == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[zenlib] [BACKTRACE] backtrace_symbols return fail.");
    } else {
        for (j = 0; j < sz_of_stack; ++j) {
            ZEN_Trace_LogMsg::debug_output(log_priority, "[zenlib] [BACKTRACE]%s.", symbols_strings[j]);
        }
        free(symbols_strings);
    }
}
