// Canonical layout of tss_dpsdk::DPBillRecorder and its global base.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_dpsdk::DPBillRecorder'
//     struct tss_dpsdk::DPBillRecorder size=5988 bases=1
//       base +0 CommBillRecord
//       +5984   enbale_bill_to_log_  bool  size=1      (ORIG spelling)
//   python3 audit/tu_report.py --type 'CommBillRecord'
//     struct CommBillRecord size=5984, vptr +0 (virtual destructor) and the
//     26 data members declared below; member order and offsets come from the
//     DWARF definition DIE in CU comm_bill_record.cpp.
//     bill_name_ is a libstdc++-4.1 std::string (4 bytes, _M_dataplus->_M_p),
//     which the host build reproduces with -D_GLIBCXX_USE_CXX11_ABI=0.
//     self_svr_ is SERVICES_ID (packed 6 bytes: uint16 + uint32).
//
// Every rebuilt TU currently carries its own partial CommBillRecord /
// DPBillRecorder model. They are wave-2 migration input:
//   // TODO(migrate): bill_cf_hackers_agent.cpp, dp_bill_monitor_agent.cpp,
//        dp_msg_handler_buffer_agent.cpp, dp_msg_handler_core_agent.cpp,
//        dp_vmbase_engine_agent.cpp, hackers_b_agent.cpp,
//        dpinterface_lifecycle_agent.cpp declare their own
//        ::CommBillRecord / tss_dpsdk::DPBillRecorder.
#ifndef TSS_SDK_LAYOUT_DP_BILL_RECORDER_H_
#define TSS_SDK_LAYOUT_DP_BILL_RECORDER_H_

#include <stddef.h>
#include <string>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

// This header is deliberately self-contained: it must NOT pull
// src/commlib/framework/comm_cfgsvr_sdk.h, because that header and the
// src/origsrc TDR chain (reached through src/closing/dp_config.h, which the
// dpinterface translation units already include) both define
// tsf4g_tdr::TdrXmlFormat.
//
// SERVICES_ID and ZEN_LOGFILE_DEVIDE are therefore declared here behind the
// shared guards used by their canonical owners, so that whichever header comes
// first wins and the other is skipped:
//   TSS_RECON_SERVICES_ID_DECLARED    <- comm_service_info.h /
//                                        comm_cfgsvr_sdk.h
//   TSS_RECON_ZEN_LOGFILE_DEVIDE_DECLARED <- zen_trace_log_basic.h
// The tag394 layout / enumerator values below are unchanged.
#ifndef TSS_RECON_SERVICES_ID_DECLARED
#define TSS_RECON_SERVICES_ID_DECLARED
struct SERVICES_ID {
    unsigned short services_type_;   // +0
    unsigned int services_id_;       // +2 (unaligned)
} __attribute__((packed));
#endif  // TSS_RECON_SERVICES_ID_DECLARED

#ifndef TSS_RECON_ZEN_LOGFILE_DEVIDE_DECLARED
#define TSS_RECON_ZEN_LOGFILE_DEVIDE_DECLARED
// 18 characters in the mangled form
// (_ZN14CommBillRecord4initEPKc18ZEN_LOGFILE_DEVIDEibb).
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
    LOGDEVIDE_BY_TIMEEND = 208
};
#endif  // TSS_RECON_ZEN_LOGFILE_DEVIDE_DECLARED

namespace sec_proto { struct LogSvrCommBillRecord; }

class ZEN_LogTrace_Basic;



// 4-byte operator<< wrappers; all constructors are inline in ORIG.
struct ConstDataSource {
    unsigned int data_source_;      // +0
};
struct UserID {
    unsigned int uin_;              // +0
};
struct LogType {
    unsigned int log_type_;         // +0
};
struct GameID {
    unsigned int game_id_;          // +0
};
struct ConstGameID {
    unsigned int game_id_;          // +0
};
struct ClientIP {
    unsigned int client_ip_;        // +0
};
struct LogComment {
    char *comment_;                 // +0
};

// 5984 bytes.
// 5984 bytes.  Every member below has an ORIG GLOBAL symbol (nm --defined-only):
//   _ZN14CommBillRecordC1Ev / C2Ev / D0Ev / D1Ev / D2Ev
//   _ZN14CommBillRecord4initEPKc18ZEN_LOGFILE_DEVIDEibb
//   _ZN14CommBillRecord10set_logwayEi   _ZN14CommBillRecord12set_bill_numEj
//   _ZN14CommBillRecord6outputEv        _ZN14CommBillRecord5resetEv
//   _ZN14CommBillRecord11make_stringEv  _ZN14CommBillRecord13init_log_fileE18ZEN_LOGFILE_DEVIDE
//   _ZN14CommBillRecord15write_to_logsvrERKN9sec_proto20LogSvrCommBillRecordE
//   _ZN14CommBillRecordlsEx / lsEPKc / lsERK15ConstDataSource / lsERK6UserID
//     / lsERK7LogType / lsERK6GameID / lsERK11ConstGameID / lsERK8ClientIP
//     / lsERK10LogComment / lsEPFRS_S0_E
// Return types are not encoded in any mangled name; `output` returns the
// record reference, the setters/init return void/int as declared.
class CommBillRecord {
public:
    CommBillRecord();
    virtual ~CommBillRecord();

    int init(const char *bill_name, ZEN_LOGFILE_DEVIDE div_log_file,
             int log_way, bool if_thread_sync, bool use_app_framework);
    int init_log_file(ZEN_LOGFILE_DEVIDE div_log_file);
    int make_string();
    int write_to_logsvr(const sec_proto::LogSvrCommBillRecord &record);
    void set_logway(int log_way);
    void set_bill_num(unsigned int bill_num);
    CommBillRecord &output();
    void reset();

    CommBillRecord &operator<<(long long value);
    CommBillRecord &operator<<(const char *value);
    CommBillRecord &operator<<(const ConstDataSource &value);
    CommBillRecord &operator<<(const UserID &value);
    CommBillRecord &operator<<(const LogType &value);
    CommBillRecord &operator<<(const GameID &value);
    CommBillRecord &operator<<(const ConstGameID &value);
    CommBillRecord &operator<<(const ClientIP &value);
    CommBillRecord &operator<<(const LogComment &value);
    CommBillRecord &operator<<(CommBillRecord &(*manipulator)(CommBillRecord &));
protected:
    unsigned int data_source_;            // +0x004
    bool data_source_need_reset_;         // +0x008
    unsigned int log_time_;               // +0x00c
    unsigned int user_id_;                // +0x010
    unsigned int log_type_;               // +0x014
    unsigned int game_id_;                // +0x018
    bool game_id_need_reset_;             // +0x01c
    unsigned int client_ip_;              // +0x020
    unsigned int server_ip_;              // +0x024
    char log_comment_[1025];              // +0x028
    unsigned int num_of_char_data_;       // +0x42c
    char char_data_array_[4][65];         // +0x430  (260 bytes)
    unsigned int num_of_int64_data_;      // +0x534
    long long int64_data_array_[64];      // +0x538  (512 bytes)
    ZEN_LogTrace_Basic *file_log_;        // +0x738
    char log_buffer_[4097];               // +0x73c
    unsigned int log_used_buffer_len_;    // +0x173d
    int log_way_;                         // +0x1741
    std::string bill_name_;               // +0x1745
    void *time_provider_;                 // +0x1749
    void *zerg_mmap_buspipe_;             // +0x174d
    bool if_thread_sync_;                 // +0x1751
    SERVICES_ID self_svr_;                // +0x1752
    bool use_app_framework_;              // +0x1758
    void *monitor_;                       // +0x175c
};

namespace tss_dpsdk {

// 5988 bytes.
class DPBillRecorder : public CommBillRecord {
public:
    // ORIG vtable 0x00292ed0 is 0x14 bytes: D1 (+0x08), D0 (+0x0c) and
    // write_to_logsvr (+0x10, virtual) -
    //   00292ed8 -> _ZN9tss_dpsdk14DPBillRecorderD1Ev            (0x000a3430)
    //   00292edc -> _ZN9tss_dpsdk14DPBillRecorderD0Ev            (0x000a33f0)
    //   00292ee0 -> _ZN9tss_dpsdk14DPBillRecorder15write_to_logsvrERKN9sec_proto20LogSvrCommBillRecordE
    //                                                                 (0x000a3120)
    // Both must be declared virtual; the mangled names are unchanged.
    virtual ~DPBillRecorder();
    virtual int write_to_logsvr(const sec_proto::LogSvrCommBillRecord &record);

    // ORIG header-inline (no symbol): writes the +0x1760 byte.
    void enbale_bill_to_log(bool enable) { enbale_bill_to_log_ = enable; }

private:
    // ORIG member name (sic): enbale_bill_to_log_.
    bool enbale_bill_to_log_;   // +0x1760
};

TSS_SDK_STATIC_ASSERT(sizeof(SERVICES_ID) == 6, services_id_size_must_be_6);
TSS_SDK_STATIC_ASSERT(sizeof(CommBillRecord) == 5984,
                      commbillrecord_size_must_be_5984);
TSS_SDK_STATIC_ASSERT(sizeof(DPBillRecorder) == 5988,
                      dpbillrecorder_size_must_be_5988);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_BILL_RECORDER_H_
