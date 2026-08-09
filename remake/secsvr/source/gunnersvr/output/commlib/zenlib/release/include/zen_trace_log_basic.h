// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_trace_log_basic.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_BASIC_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_BASIC_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include <bits/stringfwd.h>
#include <iosfwd>
#include <stddef.h>
#include <time.h>

enum ZEN_LOG_PRIORITY {
    RS_TRACE = 1,
    RS_DEBUG = 2,
    RS_INFO = 3,
    RS_ERROR = 4,
    RS_ALERT = 5,
    RS_FATAL = 6,
};
enum ZEN_LOGFILE_DEVIDE {
    LOGDEVIDE_NONE = 1,
    LOGDEVIDE_BY_SIZE = 101,
    LOGDEVIDE_BY_TIMEBEGIN = 200,
    LOGDEVIDE_BY_FIVEMINUTE = 201,
    LOGDEVIDE_BY_QUARTER = 202,
    LOGDEVIDE_BY_HOUR = 203,
    LOGDEVIDE_BY_SIXHOUR = 204,
    LOGDEVIDE_BY_DATE = 205,
    LOGDEVIDE_BY_MONTH = 206,
    LOGDEVIDE_BY_YEAR = 207,
    LOGDEVIDE_BY_TIMEEND = 208,
};
// sizeof = 364
struct ZEN_LogTrace_Basic : public ZEN_NON_Copyable { // line 94
    typedef basic_ofstream<char,std::char_traits<char> > ofstream;
public:
int (***_vptr.ZEN_LogTrace_Basic)(...);
protected:
static const size_t LOG_TMP_BUFFER_SIZE;
static const size_t DEFAULT_RESERVE_FILENUM;
static const size_t MIN_LOG_SIZE;
static const size_t MAX_LOG_SIZE;
static const size_t DEFAULT_LOG_SIZE;
static const size_t LEN_LOG_POSTFIX;
static const char STR_LOG_POSTFIX[];
ZEN_LOGFILE_DEVIDE div_log_file_;
string log_file_prefix_;
string log_file_name_;
string log_file_dir_;
unsigned int output_way_;
bool if_thread_synchro_;
ZEN_Thread_Mutex protect_lock_;
bool auto_new_line_;
size_t max_size_log_file_;
size_t reserve_file_num_;
unsigned int record_info_;
time_t current_click_;
ZEN_LOG_PRIORITY out_level_mask_;
size_t size_log_file_;
bool if_output_log_;
ofstream log_file_handle_;
public:
ZEN_LogTrace_Basic(const ZEN_LogTrace_Basic &arg0);
ZEN_LogTrace_Basic(); // line 49
virtual ~ZEN_LogTrace_Basic(); // line 70
int init_time_log(ZEN_LOGFILE_DEVIDE div_log_file, const char *log_file_prefix, bool if_thread_synchro, bool auto_new_line, size_t reserve_file_num, unsigned int output_way, unsigned int head_record); // line 84
int init_size_log(const char *log_file_prefix, bool if_thread_synchro, bool auto_new_line, size_t max_size_log_file, unsigned int reserve_file_num, unsigned int output_way, unsigned int head_record); // line 105
int init_stdout(bool if_thread_synchro, bool use_err_out, bool auto_new_line, unsigned int head_record); // line 130
int initialize(ZEN_LOGFILE_DEVIDE div_log_file, const char *log_file_prefix, bool if_thread_synchro, bool auto_new_line, size_t max_size_log_file, size_t reserve_file_num, unsigned int output_way, unsigned int head_record); // line 161
void finalize(); // line 201
void enable_output_logmsg(); // line 302
void disable_output_logmsg(); // line 307
ZEN_LOG_PRIORITY set_log_priority(ZEN_LOG_PRIORITY outlevel); // line 315
ZEN_LOG_PRIORITY get_log_priority(); // line 322
unsigned int set_log_head(unsigned int recdinfo); // line 328
unsigned int get_log_head(); // line 335
unsigned int set_output_way(unsigned int output_way); // line 354
unsigned int get_output_way(); // line 364
void set_thread_synchro(bool if_thread_synchro); // line 371
bool get_thread_synchro(); // line 376
void set_reserve_file_num(unsigned int file_num); // line 910
unsigned int get_reserve_file_num() const; // line 917
void output_log_info(const timeval &now_time, char *log_tmp_buffer, size_t sz_use_len); // line 813
protected:
void create_id_logname(size_t logfileid, string &idlogfilename); // line 716
void create_time_logname(time_t tmt, string &logfilename); // line 661
void del_old_logfile(time_t cur_time, bool init); // line 922
void del_old_time_logfile(time_t cur_time, bool init); // line 514
void del_old_id_logfile(); // line 636
void stringbuf_loghead(ZEN_LOG_PRIORITY outlevel, const timeval &now_time, char *log_tmp_buffer, size_t sz_buf_len, size_t &sz_use_len); // line 741
void make_configure(); // line 221
void make_new_logfile(time_t cur_time, bool init); // line 394
public:
static ZEN_LOG_PRIORITY log_priorities(const char *str_priority); // line 875
};
extern const size_t DEFAULT_RESERVE_FILENUM; // line 104
extern const size_t DEFAULT_LOG_SIZE; // line 111
extern const size_t LEN_LOG_POSTFIX; // line 114
extern const char STR_LOG_POSTFIX[]; // line 116

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TRACE_LOG_BASIC_H_H_
