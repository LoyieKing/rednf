// Restored from gunnersvr oracle binary (DWARF + disassembly).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_trace_log_msg.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 语义要点（与 0x0809fc90..0x080a0340 反汇编一致）：
//  - vwrite_logmsg：if_output_log_==false 或 out_level_mask_ > outlevel 时直接返回；
//    gettimeofday + stringbuf_loghead 组头，vsnprintf 到 4095 剩余空间，
//    截断（返回值 >= 剩余或 <0）时 sz_use_len=4096，auto_new_line 追加 '\n'，
//    最后 output_log_info。
//  - debug_*ex 系列：仅当 log_instance_ 非空时经 vwrite_logmsg 输出对应级别。
//  - debug_assert_ex/debug_assert：以 RS_FATAL 调 debug_output，格式串与二进制一致。
//  - instance()：为空时 new ZEN_Trace_LogMsg()（sizeof 364）。
//  - clean_instance()：delete log_instance_ 并置空。

#include <iostream> // 保留：原 TU 包含 <iostream>，产生同款 ios_base::Init 静态初始化

#include <cstdio>
#include <cstring>
#include <stdarg.h>
#include <sys/time.h>

#include "src/commlib/zenlib/zen_trace_log_msg.h"

ZEN_Trace_LogMsg *ZEN_Trace_LogMsg::log_instance_ = 0;

ZEN_Trace_LogMsg::ZEN_Trace_LogMsg() {
}

ZEN_Trace_LogMsg::~ZEN_Trace_LogMsg() {
}

void ZEN_Trace_LogMsg::clean_instance() {
    if (log_instance_ != NULL) {
        delete log_instance_;
    }
    log_instance_ = NULL;
}

void ZEN_Trace_LogMsg::instance(ZEN_Trace_LogMsg *instatnce) {
    clean_instance();
    log_instance_ = instatnce;
}

ZEN_Trace_LogMsg * ZEN_Trace_LogMsg::instance() {
    if (log_instance_ == NULL) {
        log_instance_ = new ZEN_Trace_LogMsg();
    }
    return log_instance_;
}

void ZEN_Trace_LogMsg::vwrite_logmsg(ZEN_LOG_PRIORITY outlevel,
                                     const char *str_format, va_list args) {
    timeval now_time_val;
    char log_tmp_buffer[ZEN_LogTrace_Basic::LOG_TMP_BUFFER_SIZE + 1];
    size_t sz_buf_len = ZEN_LogTrace_Basic::LOG_TMP_BUFFER_SIZE - 1;
    size_t sz_use_len = 0;
    int len_of_out;

    if (if_output_log_ == false || out_level_mask_ > outlevel) {
        return;
    }

    gettimeofday(&now_time_val, NULL);
    stringbuf_loghead(outlevel, now_time_val, log_tmp_buffer, sz_buf_len,
                      sz_use_len);
    len_of_out = vsnprintf(log_tmp_buffer + sz_use_len,
                           sz_buf_len - sz_use_len, str_format, args);
    if (len_of_out < (int)(sz_buf_len - sz_use_len)) {
        if (len_of_out < 0) {
            sz_use_len = ZEN_LogTrace_Basic::LOG_TMP_BUFFER_SIZE;
        } else {
            sz_use_len += len_of_out;
        }
    } else {
        sz_use_len = ZEN_LogTrace_Basic::LOG_TMP_BUFFER_SIZE;
    }
    if (auto_new_line_) {
        log_tmp_buffer[sz_use_len] = '\n';
        ++sz_use_len;
    }
    output_log_info(now_time_val, log_tmp_buffer, sz_use_len);
}

void ZEN_Trace_LogMsg::debug_fatalex(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_FATAL, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_alertex(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_ALERT, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_errorex(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_ERROR, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_infoex(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_INFO, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_debugEx(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_DEBUG, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_traceex(const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(RS_TRACE, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY dbglevel,
                                    const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    if (log_instance_ != NULL) {
        log_instance_->vwrite_logmsg(dbglevel, str_format, args);
    }
    va_end(args);
}

void ZEN_Trace_LogMsg::debug_assert_ex(const char *file_name, int file_line,
                                       const char *function_name,
                                       const char *expression_name,
                                       const char *out_string) {
    debug_output(RS_FATAL,
                 "Assertion failed: FILENAME:[%s],LINENO:[%d],FUN:[%s],EXPRESSION:[%s] OutString[%s].",
                 file_name, file_line, function_name, expression_name,
                 out_string);
}

void ZEN_Trace_LogMsg::debug_assert(const char *file_name, int file_line,
                                    const char *function_name,
                                    const char *expression_name) {
    debug_output(RS_FATAL,
                 "Assertion failed: FILENAME:[%s],LINENO:[%d],FUN:[%s],EXPRESSION:[%s].",
                 file_name, file_line, function_name, expression_name);
}

void ZEN_Trace_LogMsg::write_logmsg(ZEN_LOG_PRIORITY outlevel,
                                    const char *str_format, ...) {
    va_list args;
    va_start(args, str_format);
    vwrite_logmsg(outlevel, str_format, args);
    va_end(args);
}
