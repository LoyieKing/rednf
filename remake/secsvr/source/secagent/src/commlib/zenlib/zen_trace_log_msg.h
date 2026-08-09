// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_trace_log_msg.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TRACE_LOG_MSG_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TRACE_LOG_MSG_H_H_

#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include <stdio.h>

// sizeof = 364
struct ZEN_Trace_LogMsg : public ZEN_LogTrace_Basic { // line 30
protected:
static ZEN_Trace_LogMsg *log_instance_;
public:
ZEN_Trace_LogMsg(const ZEN_Trace_LogMsg &arg0);
ZEN_Trace_LogMsg(); // line 31
virtual ~ZEN_Trace_LogMsg(); // line 37
void write_logmsg(ZEN_LOG_PRIORITY outlevel, const char *str_format); // line 139
void write_logmsg(const char *arg0, ...); // line 48
protected:
void vwrite_logmsg(ZEN_LOG_PRIORITY outlevel, const char *str_format, va_list args); // line 60
public:
static void instance(ZEN_Trace_LogMsg *instatnce); // line 238
static ZEN_Trace_LogMsg * instance(); // line 227
static void clean_instance(); // line 246
static void debug_assert(const char *file_name, int file_line, const char *function_name, const char *expression_name); // line 165
static void debug_assert_ex(const char *file_name, int file_line, const char *function_name, const char *expression_name, const char *out_string); // line 180
static void debug_output(ZEN_LOG_PRIORITY dbglevel, const char *str_format); // line 205
static void debug_traceex(const char *str_format); // line 263
static void debug_debugEx(const char *str_format); // line 276
static void debug_infoex(const char *str_format); // line 289
static void debug_errorex(const char *str_format); // line 302
static void debug_alertex(const char *str_format); // line 315
static void debug_fatalex(const char *str_format); // line 329
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TRACE_LOG_MSG_H_H_
