// Reconstructed from libtss_sdk.so.1.0.394
//   src/formmog/tss_sdk/plugins/proto_proc/dp_bill_adapter.cpp
//   src/formmog/tss_sdk/plugins/proto_proc/dp_monitor_reporter.cpp
//
// ORIG address map (all symbols are global unless noted):
//   DPBillAdapter::~DPBillAdapter()            0x000a26c0 (D1)
//   DPBillAdapter::DPBillAdapter(...)          0x000a2730 (C2)
//   DPBillAdapter::DPBillAdapter(...)          0x000a27d0 (C1)
//   DPBillAdapter::~DPBillAdapter()            0x000a2870 (D2)
//   DPBillAdapter::reload()                    0x000a28e0
//   DPBillAdapter::init(unsigned int)          0x000a2a20
//   DPBillAdapter::dump(...)                   0x000a2a70
//   DPMonitorReporter::reload()                0x000adad0
//   DPMonitorReporter::report_data(uint,uint)  0x000adae0
//   DPMonitorReporter::check_timer(long)       0x000adb20
//   DPMonitorReporter::init()                  0x000adc00
//
// Static storage (nm -S):
//   tss_dpsdk::DPBillAdapter::channel_id_      0x002b6de0 size 4
//   tss_dpsdk::DPBillAdapter::bill_            0x002b6e00 size 5988 (0x1764)
//   tss_dpsdk::bill_locker_                    0x002b8b40 size 28   (local)
//   tss_dpsdk::DPMonitorReporter::last_send_time_ 0x002bd840 size 4
//   tss_dpsdk::DPMonitorReporter::report_value_   0x002bd860 size 804 (201 uints)
//
// Exact string constants used below (file offsets 0x0021adf3..0x0021ae60):
//   "%sdp_%d_%d"
//   "DPBillAdapter::init, init begin, file path %s"
//   "DPBillAdapter reload successful, file=%s"
//   "%s/%u.log", "a+", "%Y-%m-%d %H:%M:%S", "%s|%s|%d|%s\n"
//
// The file models only the two assigned translation units.  Every call that
// lands outside them (CommBillRecord / DPBillRecorder / ZEN trace log /
// ZEN_Thread_Mutex / DP_CONFIG) stays an external boundary.

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

// ---------------------------------------------------------------------------
// ZEN thread primitives (partial ABI model)
// ---------------------------------------------------------------------------

// The canonical tag-394 model (28 bytes: ZEN_Lock_Base + pthread_mutex_t)
// replaces the local opaque copy.  DWARF: the original global
// tss_dpsdk::bill_locker_ is constructed by
// ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*) with a null name
// (0x000a2574) and destroyed by __tcf_2 (0x000a2660).
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

// ZEN_Lock_Guard<ZEN_Thread_Mutex> (byte_size 4) comes from the canonical
// tag-394 zen_lock_guard.h.  reload() has the local `tmp_lock` of this type;
// GCC inlined its ctor/dtor into the explicit lock() at 0x000a2906 and
// unlock() at 0x000a29dd, with an unwind pad at 0x000a29f9 that also unlocks.
#include "src/commlib/zenlib/zen_lock_guard.h"

// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h); the local stub is gone.
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ---------------------------------------------------------------------------
// tss_dp_comm configuration (partial view)
// ---------------------------------------------------------------------------

namespace tss_dp_comm {

// DWARF: DPLogConf byte_size 296.  Only the members read by reload() are
// named: game_id is at +0x04 and bill_path at +0x08 (char[256]).
struct DPLogConf {
    unsigned int bill_level;   // +0x00
    unsigned int game_id;      // +0x04
    char bill_path[256];       // +0x08
};

// DWARF: DPConfig byte_size 22842; dp_base_conf +0x0000 (6 bytes),
// dp_proto_conf +0x0006, dp_log_conf +0x0010.  The intermediate bytes are
// opaque here because only dp_log_conf is touched by these functions.
struct DPConfig {
    unsigned char opaque_0x0000[0x10];
    DPLogConf dp_log_conf;     // +0x0010
};

// DWARF: tss_dp_comm::DP_CONFIG::glb_dp_config_ is a 4-byte pointer object
// (symbol _ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E, defined by
// dp_config.cpp).  reload()/dump() read through it via GOT slot 0x002943ac.
class DP_CONFIG {
public:
    static DPConfig *glb_dp_config_;
};

}  // namespace tss_dp_comm

// ---------------------------------------------------------------------------
// CommBillRecord / DPBillRecorder (partial ABI model)
// ---------------------------------------------------------------------------

// enum values from DWARF (comm_bill_record header, line 16).
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

// DWARF (comm_bill_record.cpp): UserID/LogType/ConstGameID/ClientIP are
// 4-byte wrappers around one uint32_t; LogComment wraps a const char*.
struct UserID {
    explicit UserID(unsigned int uin) : uin_(uin) {}
    unsigned int uin_;              // +0x00
};

struct LogType {
    explicit LogType(unsigned int log_type) : log_type_(log_type) {}
    unsigned int log_type_;         // +0x00
};

struct ConstGameID {
    explicit ConstGameID(unsigned int game_id) : game_id_(game_id) {}
    unsigned int game_id_;          // +0x00
};

struct ClientIP {
    explicit ClientIP(unsigned int client_ip) : client_ip_(client_ip) {}
    unsigned int client_ip_;        // +0x00
};

struct LogComment {
    explicit LogComment(const char *comment) : comment_(comment) {}
    const char *comment_;           // +0x00
};

struct ConstDataSource {
    explicit ConstDataSource(unsigned int data_source)
        : data_source_(data_source) {}
    unsigned int data_source_;      // +0x00
};

// DWARF: CommBillRecord byte_size 5984 (0x1760), vptr at +0x00.
// Field offsets (verified against operator<< at 0x000d1500..0x000d158f and
// init at 0x000d2810): data_source_ +0x04, user_id_ +0x10,
// log_type_ +0x14, game_id_ +0x18, client_ip_ +0x20, log_comment_ +0x28,
// log_way_ +0x1744, bill_name_ +0x1748, if_thread_sync_ +0x1754,
// self_svr_ +0x1755, use_app_framework_ +0x175b.
// The 5980-byte opaque tail keeps sizeof == 5984 exactly; all called members
// are out-of-line in ORIG and remain external.
class CommBillRecord {
public:
    CommBillRecord();
    virtual ~CommBillRecord();

    int init(const char *bill_name, ZEN_LOGFILE_DEVIDE log_divide,
             int log_way, bool if_thread_sync, bool use_app_framework);
    void set_bill_num(unsigned int bill_num);
    void output();

    CommBillRecord &operator<<(const ConstDataSource &value);
    CommBillRecord &operator<<(const UserID &value);
    CommBillRecord &operator<<(const LogType &value);
    CommBillRecord &operator<<(const ConstGameID &value);
    CommBillRecord &operator<<(const ClientIP &value);
    CommBillRecord &operator<<(const LogComment &value);

private:
    unsigned char opaque_0x0004[5980];
};

// DWARF (dp_bill_recorder.cpp): tss_dpsdk::DPBillRecorder derives from the
// global CommBillRecord, byte_size 5988, and adds `bool enbale_bill_to_log_`
// at +0x1760.  The misspelling is from DWARF.  Its ctor is inline:
// CommBillRecord() then enbale_bill_to_log_=false (0x000a259c..0x000a25b1);
// ORIG registers __tcf_3 for the destructor.
namespace tss_dpsdk {
class DPBillRecorder : public ::CommBillRecord {
public:
    DPBillRecorder() : enbale_bill_to_log_(false) {}
    virtual ~DPBillRecorder();

    void enbale_bill_to_log(bool enable) {
        enbale_bill_to_log_ = enable;
    }

private:
    bool enbale_bill_to_log_;       // +0x1760
};
}  // namespace tss_dpsdk

namespace tss_dpsdk {

// Constructed before bill_ by the original static init (ZEN_Thread_Mutex(char
// const*) with a null name at 0x000a2574, before CommBillRecord() at
// 0x000a259c).  Internal linkage matches nm type 'b'.
static ZEN_Thread_Mutex bill_locker_;

// DWARF: DPBillAdapter byte_size 4, single member `user_` at +0x00.
// DPBillAdapter::init returns int in DWARF (dp_bill_adapter.cpp line 114).
class DPBillAdapter {
public:
    DPBillAdapter(const char *msg, unsigned int log_type,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();

    static int init(unsigned int channel_id);
    static int reload();
    static int dump(const char *msg, unsigned int user_id,
                    const unsigned char *data, unsigned int len);

private:
    tss_sdk_impl::UserInfo *user_;      // +0x00
    static unsigned int channel_id_;    // 0x002b6de0
    static DPBillRecorder bill_;        // 0x002b6e00
};

// DWARF: all four methods return int; check_timer takes time_t (mangled as
// long on i386).  Static storage: last_send_time_ and report_value_[201].
class DPMonitorReporter {
public:
    static int init();
    static int reload();
    static int report_data(unsigned int id, unsigned int value);
    static int check_timer(time_t now);

private:
    static time_t last_send_time_;              // 0x002bd840
    static unsigned int report_value_[201];     // 0x002bd860 (804 bytes)
};

// D2 at 0x000a2870 is instruction-identical to D1 at 0x000a26c0.  The ctor
// locks bill_locker_; the dtor does not lock, it always unlocks.  Because the
// operator<< calls can escape, the condition is evaluated before output() and
// the flag is cleared afterwards:
//   user_ && (user_->base_info_.flags_ & 1)
// then output(), then false, then unlock.
DPBillAdapter::~DPBillAdapter() {
    bill_.enbale_bill_to_log(
        user_ != 0 && (user_->base_info_.flags_ & 1) != 0);
    bill_.output();
    bill_.enbale_bill_to_log(false);
    bill_locker_.unlock();
}

// C2 at 0x000a2730 / C1 at 0x000a27d0.  user_ is stored first, then the
// process-wide bill mutex is locked.  A null UserInfo contributes UserID(0)
// and ClientIP(0); msg and log_type are streamed unconditionally (there is no
// flags check in the constructor).
DPBillAdapter::DPBillAdapter(const char *msg, unsigned int log_type,
                             tss_sdk_impl::UserInfo *user)
    : user_(user) {
    bill_locker_.lock();
    bill_ << UserID(user_ != 0 ? user_->base_info_.uin_ : 0)
          << LogType(log_type)
          << LogComment(msg)
          << ClientIP(user_ != 0 ? user_->base_info_.client_ip_ : 0);
}

// ORIG 0x000a28e0..0x000a29ee (unwind pads through 0x000a2a10).
// Order: scoped lock -> build path -> debug_debugEx -> CommBillRecord::init ->
// set_bill_num(2) -> stream ConstGameID -> debug_infoex -> return 0.
int DPBillAdapter::reload() {
    ZEN_Lock_Guard<ZEN_Thread_Mutex> tmp_lock(bill_locker_);

    char bill_file_path[4096];
    snprintf(bill_file_path, sizeof(bill_file_path) - 1, "%sdp_%d_%d",
             tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_log_conf.bill_path,
             tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_log_conf.game_id,
             channel_id_);
    bill_file_path[sizeof(bill_file_path) - 1] = '\0';

    ZEN_Trace_LogMsg::debug_debugEx(
        "DPBillAdapter::init, init begin, file path %s", bill_file_path);

    bill_.init(bill_file_path, LOGDEVIDE_BY_DATE, 3, false, true);
    bill_.set_bill_num(2);
    bill_ << ConstGameID(
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_log_conf.game_id);

    ZEN_Trace_LogMsg::debug_infoex(
        "DPBillAdapter reload successful, file=%s", bill_file_path);
    return 0;
}

// ORIG 0x000a2a20..0x000a2a63.  EAX is left untouched after reload(), i.e.
// the source returns reload() rather than a constant 0.
int DPBillAdapter::init(unsigned int channel_id) {
    channel_id_ = channel_id;
    bill_ << ConstDataSource(43);
    return reload();
}

// ORIG 0x000a2a70..0x000a2bd7.
//
// File name: "<dp_log_conf.bill_path>/<user_id>.log", mode "a+".
// On fopen64 failure returns -1.  Otherwise one line is appended:
//   "<YYYY-mm-dd HH:MM:SS>|<msg>|<len>|<hex buffer>\n"
// The hex buffer is 0x2800 bytes.  Conversion runs only when
// len*2+1 <= 0x2800 (unsigned wrap preserved); in that case buffer[2*len] is
// terminated.  buffer[0x27ff] is terminated unconditionally.  ORIG does not
// initialize the remaining bytes, so for len >= 0x1400 the %s argument reads
// indeterminate stack bytes until the final byte; this defect is preserved.
int DPBillAdapter::dump(const char *msg, unsigned int user_id,
                        const unsigned char *data, unsigned int len) {
    char file_name[256];
    snprintf(file_name, sizeof(file_name), "%s/%u.log",
             tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_log_conf.bill_path,
             user_id);

    FILE *fd = fopen64(file_name, "a+");
    if (fd == 0)
        return -1;

    time_t now;
    time(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
             localtime(&now));

    char buffer[0x2800];
    if (static_cast<unsigned int>(len * 2 + 1) <= 0x2800u) {
        static const char hex_digits[] = "0123456789ABCDEF";
        for (unsigned int i = 0; i < len; ++i) {
            const unsigned char value = data[i];
            buffer[i * 2] = hex_digits[value >> 4];
            buffer[i * 2 + 1] = hex_digits[value & 0x0f];
        }
        buffer[len * 2] = '\0';
    }
    buffer[sizeof(buffer) - 1] = '\0';

    fprintf(fd, "%s|%s|%d|%s\n", time_str, msg, len, buffer);
    fclose(fd);
    return 0;
}

// DPMonitorReporter ---------------------------------------------------------

// ORIG 0x000adad0..0x000adad6: xor eax,eax; ret.
int DPMonitorReporter::reload() {
    return 0;
}

// ORIG 0x000adae0..0x000adb13.
// Valid ids are 1001..1200 inclusive (unsigned: id-1001 <= 199).
// report_value_ has 201 elements and is indexed with id-1000, so slot 0 is
// never accumulated.  value is added with 32-bit wraparound.
int DPMonitorReporter::report_data(unsigned int id, unsigned int value) {
    if (static_cast<unsigned int>(id - 1001) <= 199)
        report_value_[id - 1000] += value;
    return 0;
}

// ORIG 0x000adb20..0x000adb5f.  Signed time_t comparison:
// return 1 while now < last_send_time_ + 60; otherwise store now and return 0.
int DPMonitorReporter::check_timer(time_t now) {
    if (last_send_time_ + 60 > now)
        return 1;
    last_send_time_ = now;
    return 0;
}

// ORIG 0x000adc00..0x000adc45: time(&last_send_time_);
// memset(report_value_, 0, 0x324); return 0.
int DPMonitorReporter::init() {
    time(&last_send_time_);
    memset(report_value_, 0, sizeof(report_value_));
    return 0;
}

// ---------------------------------------------------------------------------
// Static storage definitions
// ---------------------------------------------------------------------------

unsigned int DPBillAdapter::channel_id_;

DPBillRecorder DPBillAdapter::bill_;

time_t DPMonitorReporter::last_send_time_;
unsigned int DPMonitorReporter::report_value_[201];

}  // namespace tss_dpsdk

// Compile-time layout guards for the partial models.
typedef char dp_bill_adapter_size_must_be_4[
    sizeof(tss_dpsdk::DPBillAdapter) == 4 ? 1 : -1];
typedef char dp_bill_recorder_size_must_be_5988[
    sizeof(tss_dpsdk::DPBillRecorder) == 5988 ? 1 : -1];
typedef char comm_bill_record_size_must_be_5984[
    sizeof(::CommBillRecord) == 5984 ? 1 : -1];

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice is the primary reconstruction
// for two ORIG CUs:
//   dp_bill_adapter.cpp
//   dp_monitor_reporter.cpp
// (dp_bill_recorder.cpp carries the same block but its primary reconstruction
// is bill_cf_hackers_agent.cpp.)
//
// Both ORIG blocks carry the same multiset (26 calls, target size 996):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   CommBillRecord::CommBillRecord()      (DPBillRecorder base-subobject)
//   ZEN_Thread_Mutex::ZEN_Thread_Mutex()  (bill_locker_, 0x002c3c00)
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() /
//     __cxa_guard_release + atexit; guard_abort + _Unwind_Resume pad
// The 16-entry status array and __ioinit are TU-local, so emit one real block
// per ORIG CU.  The weak Singleton<TssSdkCommSvc>::ObjectCreator pre-create
// below is one shared object in the shipped DSO.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID dp_bill_adapter
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_monitor_reporter
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG .bss 0x0029d800 (weak): Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
