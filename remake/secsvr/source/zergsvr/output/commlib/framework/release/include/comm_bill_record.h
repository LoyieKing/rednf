// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_bill_record.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_BILL_RECORD_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_BILL_RECORD_H_H_

#include "src/commlib/framework/comm_service_info.h"
#include <bits/stringfwd.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

// sizeof = 4
struct ConstDataSource { // line 18
public:
unsigned int data_source_;
ConstDataSource(unsigned int arg0); // line 19
};
// sizeof = 4
struct LogType { // line 39
public:
unsigned int log_type_;
LogType(unsigned int arg0); // line 40
};
// sizeof = 4
struct LogComment { // line 79
public:
const char *comment_;
LogComment(const char *arg0); // line 80
};
// sizeof = 5984
struct CommBillRecord { // line 89
public:
int (***_vptr.CommBillRecord)(...);
private:
static const size_t LEN_OF_LOGGER_COMMENT_INFO;
static const size_t LEN_OF_LOG_BUFFER;
static const size_t NUM_OF_LOGGER_INT64_DATA;
static const size_t NUM_OF_LOGGER_CHAR_DATA;
static const size_t LEN_OF_LOGGER_CHAR_DATA;
uint32_t data_source_;
bool data_source_need_reset_;
uint32_t log_time_;
uint32_t user_id_;
uint32_t log_type_;
uint32_t game_id_;
bool game_id_need_reset_;
uint32_t client_ip_;
uint32_t server_ip_;
char log_comment_[];
uint32_t num_of_char_data_;
char char_data_array_[][];
uint32_t num_of_int64_data_;
int64_t int64_data_array_[];
ZEN_LogTrace_Basic *file_log_;
char log_buffer_[];
size_t log_used_buffer_len_;
int log_way_;
string bill_name_;
Comm_Time_Provider *time_provider_;
Zerg_MMAP_BusPipe *zerg_mmap_buspipe_;
bool if_thread_sync_;
SERVICES_ID self_svr_;
bool use_app_framework_;
Comm_Stat_Monitor *monitor_;
public:
CommBillRecord(const CommBillRecord &arg0);
CommBillRecord(); // line 26
virtual ~CommBillRecord(); // line 39
int init(const char *bill_name, int log_way, bool if_thread_sync, bool arg3); // line 73
void set_logway(int log_way); // line 357
void set_bill_num(unsigned int bill_num); // line 363
CommBillRecord & operator<<(const ConstDataSource &data_source); // line 114
CommBillRecord & operator<<(const UserID &user_id); // line 122
CommBillRecord & operator<<(const LogType &log_type); // line 129
CommBillRecord & operator<<(const GameID &game_id); // line 136
CommBillRecord & operator<<(const ConstGameID &game_id); // line 144
CommBillRecord & operator<<(const ClientIP &client_ip); // line 152
CommBillRecord & operator<<(const LogComment &log_comment); // line 159
CommBillRecord & operator<<(const char *s); // line 169
CommBillRecord & operator<<(int64_t i); // line 181
CommBillRecord & output(); // line 276
CommBillRecord & operator<<(CommBillRecord & (**op)(CommBillRecord &arg0)); // line 352
uint32_t get_userid(); // line 133
void reset(); // line 46
protected:
virtual int write_to_logsvr(const /*anon struct*/ int &pkg); // line 332
const char * make_string(); // line 198
void init_log_file(); // line 371
time_t get_cur_time() const; // line 149
};
extern const size_t LEN_OF_LOGGER_COMMENT_INFO; // line 164
extern const size_t LEN_OF_LOG_BUFFER; // line 166
extern const size_t NUM_OF_LOGGER_INT64_DATA; // line 169
extern const size_t NUM_OF_LOGGER_CHAR_DATA; // line 172
extern const size_t LEN_OF_LOGGER_CHAR_DATA; // line 175

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_BILL_RECORD_H_H_
