// Restored from gunnersvr binary (DWARF layout + disassembly 对照).
// sizeof = 5984；全部成员偏移经二进制 mov disp(%reg) 核对：
//   vptr 0, data_source_ 4, need_reset 8, log_time_ 12, user_id_ 16,
//   log_type_ 20, game_id_ 24, game_id_need_reset_ 28, client_ip_ 32,
//   server_ip_ 36, log_comment_[1025] 40, num_of_char_data_ 1068,
//   char_data_array_[4][65] 1072, num_of_int64_data_ 1332,
//   int64_data_array_[64] 1336, file_log_ 1848, log_buffer_[4097] 1852,
//   log_used_buffer_len_ 5952, log_way_ 5956, bill_name_ 5960,
//   time_provider_ 5964, zerg_mmap_buspipe_ 5968, if_thread_sync_ 5972,
//   self_svr_ 5973 (packed SERVICES_ID), use_app_framework_ 5979,
//   monitor_ 5980.
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BILL_RECORD_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BILL_RECORD_H_H_

#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_time_provider.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

struct Zerg_MMAP_BusPipe;
struct Comm_Stat_Monitor;
namespace sec_proto {
struct LogSvrCommBillRecord;
}

// sizeof = 4
struct ConstDataSource { // line 18
public:
    unsigned int data_source_;
    ConstDataSource(unsigned int arg0) : data_source_(arg0) {} // line 19
};

// sizeof = 4
struct UserID { // line 28
public:
    unsigned int uin_;
    UserID(unsigned int arg0) : uin_(arg0) {} // line 29
};

// sizeof = 4
struct LogType { // line 39
public:
    unsigned int log_type_;
    LogType(unsigned int arg0) : log_type_(arg0) {} // line 40
};

// sizeof = 4
struct GameID { // line 49
public:
    unsigned int game_id_;
    GameID(unsigned int arg0) : game_id_(arg0) {} // line 50
};

// sizeof = 4
struct ConstGameID { // line 60
public:
    unsigned int game_id_;
    ConstGameID(unsigned int arg0) : game_id_(arg0) {} // line 61
};

// sizeof = 4
struct ClientIP { // line 70
public:
    unsigned int client_ip_;
    ClientIP(unsigned int arg0) : client_ip_(arg0) {} // line 71
};

// sizeof = 4
struct LogComment { // line 79
public:
    const char *comment_;
    LogComment(const char *arg0) : comment_(arg0) {} // line 80
};

// sizeof = 5984
struct CommBillRecord { // line 89
private:
    static const size_t LEN_OF_LOGGER_COMMENT_INFO = 1025; // 数组含结尾 NUL
    static const size_t LEN_OF_LOG_BUFFER = 4097;
    static const size_t NUM_OF_LOGGER_INT64_DATA = 64;
    static const size_t NUM_OF_LOGGER_CHAR_DATA = 4;
    static const size_t LEN_OF_LOGGER_CHAR_DATA = 65;
    uint32_t data_source_;                 // 0x04
    bool data_source_need_reset_;          // 0x08
    uint32_t log_time_;                    // 0x0c
    uint32_t user_id_;                     // 0x10
    uint32_t log_type_;                    // 0x14
    uint32_t game_id_;                     // 0x18
    bool game_id_need_reset_;              // 0x1c
    uint32_t client_ip_;                   // 0x20
    uint32_t server_ip_;                   // 0x24
    char log_comment_[LEN_OF_LOGGER_COMMENT_INFO]; // 0x28
    uint32_t num_of_char_data_;            // 0x42c
    char char_data_array_[NUM_OF_LOGGER_CHAR_DATA][LEN_OF_LOGGER_CHAR_DATA]; // 0x430
    uint32_t num_of_int64_data_;           // 0x534
    int64_t int64_data_array_[NUM_OF_LOGGER_INT64_DATA]; // 0x538
    ZEN_LogTrace_Basic *file_log_;         // 0x738
    char log_buffer_[LEN_OF_LOG_BUFFER];   // 0x73c
    size_t log_used_buffer_len_;           // 0x1740
    int log_way_;                          // 0x1744
    std::string bill_name_;                // 0x1748
    Comm_Time_Provider *time_provider_;    // 0x174c
    Zerg_MMAP_BusPipe *zerg_mmap_buspipe_; // 0x1750
    bool if_thread_sync_;                  // 0x1754
    SERVICES_ID self_svr_;                 // 0x1755
    bool use_app_framework_;               // 0x175b
    Comm_Stat_Monitor *monitor_;           // 0x175c
public:
    CommBillRecord(const CommBillRecord &arg0); // 声明但不定义（二进制无符号）
    CommBillRecord(); // line 26
    virtual ~CommBillRecord(); // line 39
    int init(const char *bill_name, int log_way, bool if_thread_sync, bool arg4); // line 73
    void set_logway(int log_way); // line 358
    void set_bill_num(unsigned int bill_num); // line 364
    CommBillRecord & operator<<(const ConstDataSource &data_source); // line 115
    CommBillRecord & operator<<(const UserID &user_id); // line 123
    CommBillRecord & operator<<(const LogType &log_type); // line 130
    CommBillRecord & operator<<(const GameID &game_id); // line 137
    CommBillRecord & operator<<(const ConstGameID &game_id); // line 145
    CommBillRecord & operator<<(const ClientIP &client_ip); // line 153
    CommBillRecord & operator<<(const LogComment &log_comment); // line 160
    CommBillRecord & operator<<(const char *s); // line 170
    CommBillRecord & operator<<(int64_t i); // line 182
    CommBillRecord & output(); // line 277
    CommBillRecord & operator<<(CommBillRecord & (*op)(CommBillRecord &)); // line 353
    uint32_t get_userid() { return user_id_; } // line 134 (内联，无符号)
    void reset(); // line 46
protected:
    virtual int write_to_logsvr(const sec_proto::LogSvrCommBillRecord &pkg); // line 333
    const char * make_string(); // line 199
    void init_log_file(); // line 371
    time_t get_cur_time() const { // line 150 (内联，无符号)
        return time_provider_ ? time_provider_->time() : time(NULL);
    }
};

extern CommBillRecord g_bill; // line 251
extern CommBillRecord g_stat; // line 253
extern const size_t LEN_OF_LOGGER_COMMENT_INFO; // line 165
extern const size_t LEN_OF_LOG_BUFFER; // line 167
extern const size_t NUM_OF_LOGGER_INT64_DATA; // line 170
extern const size_t NUM_OF_LOGGER_CHAR_DATA; // line 173
extern const size_t LEN_OF_LOGGER_CHAR_DATA; // line 176

extern CommBillRecord & endl(CommBillRecord &os); // line 423

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BILL_RECORD_H_H_
