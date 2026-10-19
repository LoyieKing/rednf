// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/framework/tss_sdk_framework.cpp
// (DWARF producer path
//  /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/).
//
// Reconstructed definitions (addresses are shipped symbol starts; DWARF
// high_pc, inter-symbol padding excluded):
//   TssSdkFramework::TssSdkFramework()  C1/C2   0x00091680 / 0x00091690
//   TssSdkFramework::~TssSdkFramework() D1/D2   0x000916a0 / 0x000916b0
//   TssSdkFramework::get_plugin_id(unsigned short)   0x00091860-0x000918da
//   TssSdkFramework::get_last_error_code()           0x000918e0-0x00091907
//   TssSdkFramework::get_last_error()                0x00091910-0x00091956
//   TssSdkFramework::init_config()                   0x00091960-0x00091b25
//   TssSdkFramework::get_not_need_channel_plugin_id_table_()
//                                                    0x00091b30-0x00091c33
//   TssSdkFramework::init_log(char const*, ZEN_LOG_PRIORITY)
//                                                    0x000922f0-0x0009244a
//   TssSdkFramework::init(unsigned int)              0x00092450-0x00092727
//   TssSdkFramework::fini(ETssSdkPluginId)           0x00092730-0x00093224
//
// TssSdkFramework has no non-static data members (DWARF sizeof == 1); all
// state is file-scope static.  The ctor/dtor are the shipped empty functions.
// The original TU's global initialization (0x00091c80-0x000922c3) constructs
// thread_mutex_, error_info_, the two singletons, the empty vector and the
// timer handler; that order is reproduced by the static member definitions
// below.  It also constructs an unrelated 16-entry
// COMM_STAT_ITEM_WITH_NAME table (0x2a0100, 0x5c0 bytes) belonging to the
// ZEN status-monitor machinery; see the report, it is not part of this
// assignment.
//
// Object layouts are taken from DWARF; types owned by other translation units
// are declared only partially with their exact extents and the exact methods
// used here.  Their constructors/methods remain external symbols.

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <new>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ---------------------------------------------------------------------------
// commlib/zenlib external types.  The local lock models are gone: the
// canonical tag-394 headers are the single definitions in the build
// (ZEN_Lock_Base 4 / ZEN_Thread_Mutex 28 / ZEN_Lock_Guard<ZEN_Thread_Mutex> 4).
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_guard.h"

// Canonical tag-394 logging boundary: ZEN_LOG_PRIORITY and ZEN_Trace_LogMsg
// come from src/commlib/zenlib/zen_trace_log_msg.h (reached through
// zen_server_status.h).  The canonical header is static-only and omits the
// ORIG 364-byte base; the two inherited non-virtual declarations this CU
// needs are provided below.

// ORIG tss_sdk_framework.cpp static-initialization block: independent
// 16-entry COMM_STAT_ITEM_WITH_NAME + __ioinit + status dtor.  Emitted before
// the framework's own static members to preserve the shipped order.
#define TSS_SDK_STATIC_INIT_ID tss_sdk_framework
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

// The canonical zen_trace_log_msg.h is a static-only boundary and does not
// model the ORIG 364-byte ZEN_LogTrace_Basic tail.  This CU calls the two
// inherited non-virtual base methods through ZEN_Trace_LogMsg::instance(), so
// declare exactly those ORIG signatures here.  The definitions are the
// out-of-line c5/GCC-4.1 symbols; the ORIG base subobject is at offset 0, so
// the calls use the same ABI/mangled targets as the shipped code.
enum ZEN_LOGFILE_DEVIDE {
    LOGDEVIDE_NONE = 1,
    LOGDEVIDE_BY_SIZE = 101,
    LOGDEVIDE_BY_TIMEBEGIN = 200,
    LOGDEVIDE_BY_FIVEMINUTE = 201,
    LOGDEVIDE_BY_QUARTER = 202,
    LOGDEVIDE_BY_HOUR = 203,
    LOGDEVIDE_BY_SIXHOUR = 204,
    LOGDEVIDE_BY_DATE = 205,
    LOGDEVIDE_BY_MONTH = 206
};

class ZEN_LogTrace_Basic {
public:
    int initialize(ZEN_LOGFILE_DEVIDE div_log_file,
                   char const *log_file_prefix,
                   bool if_thread_synchro,
                   bool auto_new_line,
                   std::size_t max_size_log_file,
                   std::size_t reserve_file_num,
                   unsigned int output_way,
                   unsigned int head_record);
    ZEN_LOG_PRIORITY set_log_priority(ZEN_LOG_PRIORITY outlevel);
};


namespace ZEN_OS {
int access(char const *path_name, int mode);
}

// zen_singleton.h: canonical declaration shared by every reconstructed TU.
// The out-of-line weak instance() bodies are owned by
// tss_sdk_comm_svc_agent.cpp.
#include "src/commlib/zenlib/zen_singleton.h"

// ---------------------------------------------------------------------------
// Protocol configuration structs: the DWARF-exact generated declarations
// (src/origsrc), taken from the ORIG CU's own DWARF.  The local byte-opaque
// stand-ins for tss_sdk_conf_desc::CommConfig/UicConfig and
// tss_sdk_conf_path_desc::path_config are gone; layouts (CommConfig 1226,
// UicConfig 277, config 1515, path_config 1024) are unchanged.
// ---------------------------------------------------------------------------
#include "src/origsrc/formmog_conf_tsssdk_conf.h"
#include "src/origsrc/formmog_conf_tsssdk_conf_path.h"

typedef tss_sdk_conf_desc::config CONFIG;
typedef tss_sdk_conf_path_desc::path_config PATH_CONFIG;

// Global configuration objects are defined by this TU (plain, unmangled
// symbols in the shipped ELF).  g_is_old_sdk lives in .data and is 1 at load
// time; the two configuration structs live in .bss.
volatile int g_is_old_sdk = 1;
PATH_CONFIG g_path_config;
CONFIG g_config;

// ---------------------------------------------------------------------------
// tss_sdk_impl framework and timer declarations.
// The plugin manager, plugin base, and communication service declarations are
// shared through framework/tss_sdk_comm_svc.h.
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {

class TssSdkTimerHandler {
public:
    TssSdkTimerHandler();
    ~TssSdkTimerHandler();

    void update_time(long now);
    void check_monitor(long now);

private:
    void *time_provider_;  // +0x00 Comm_Time_Provider*
    void *stat_monitor_;   // +0x04 Comm_Stat_Monitor*
    long last_check_;      // +0x08 time_t
};

// tss_sdk_framework.h: no data members (sizeof == 1).
class TssSdkFramework {
public:
    TssSdkFramework();
    ~TssSdkFramework();

    static int init(unsigned int instance_id);
    static int fini(tss_sdk_if::ETssSdkPluginId plugin_id);

    static int get_last_error_code();
    static char const *get_last_error();

private:
    static int init_config();
    static int init_log(char const *log_dir, ZEN_LOG_PRIORITY log_priority);
    static int get_not_need_channel_plugin_id_table_();
    static tss_sdk_if::ETssSdkPluginId get_plugin_id(unsigned short busi_id);

    static volatile bool is_init_succ_;                  // 0x2a00ab, size 1
    static ZEN_Thread_Mutex thread_mutex_;               // 0x2a00ac, size 28
    static CThreadErrorInfo error_info_;                 // 0x2a00c8, size 4
    static TssSdkPluginMgr *plugin_mgr_;                 // 0x2a00cc
    static TssSdkCommSvc *comm_svc_;                     // 0x2a00d0
    static std::vector<tss_sdk_if::ETssSdkPluginId>
        not_need_channel_plugin_id_table_;               // 0x2a00d4, 12 bytes
    static TssSdkTimerHandler *timer_handler_;           // 0x2a00e0
};

// Static initialization order reproduced from 0x91fe6-0x920e4:
// thread_mutex_(NULL), error_info_, plugin singleton/pointer, comm-service
// singleton/pointer, empty vector, timer handler allocation.
volatile bool TssSdkFramework::is_init_succ_;

ZEN_Thread_Mutex TssSdkFramework::thread_mutex_((char const *)0);

CThreadErrorInfo TssSdkFramework::error_info_;

TssSdkPluginMgr *TssSdkFramework::plugin_mgr_ =
    Singleton<TssSdkPluginMgr>::instance();

TssSdkCommSvc *TssSdkFramework::comm_svc_ =
    Singleton<TssSdkCommSvc>::instance();

std::vector<tss_sdk_if::ETssSdkPluginId>
    TssSdkFramework::not_need_channel_plugin_id_table_;

TssSdkTimerHandler *TssSdkFramework::timer_handler_ =
    new TssSdkTimerHandler();

// ---------------------------------------------------------------------------
// Constructor / destructor (shipped no-ops)
// ---------------------------------------------------------------------------

TssSdkFramework::TssSdkFramework() {
}

TssSdkFramework::~TssSdkFramework() {
}

// ---------------------------------------------------------------------------
// Error accessors
// ---------------------------------------------------------------------------

int TssSdkFramework::get_last_error_code() {
    return error_info_.last_error_info()->error_code;
}

// Returns the process-wide empty literal when no error code is stored;
// otherwise the error-info buffer (no std::string temporary is involved).
char const *TssSdkFramework::get_last_error() {
    char const *last_err_msg = "";
    if (error_info_.last_error_info()->error_code != 0) {
        last_err_msg = error_info_.get_last_error_msg();
    }
    return last_err_msg;
}

// ---------------------------------------------------------------------------
// Business id -> plugin id mapping (0x91860)
// ---------------------------------------------------------------------------

tss_sdk_if::ETssSdkPluginId
TssSdkFramework::get_plugin_id(unsigned short busi_id) {
    switch (busi_id) {
    case 1:
    case 2:
    case 10:
        return tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;   // 3
    case 3:
        return tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA;      // 5
    case 13:
        return tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;     // 4
    default:
        ZEN_Trace_LogMsg::debug_errorex("%s, invalid busi id %u.",
                                        __PRETTY_FUNCTION__,
                                        (unsigned int)busi_id);
        return tss_sdk_if::ETSS_SDK_PLUGIN_ID_INVALID;      // -2
    }
}

// ---------------------------------------------------------------------------
// Configuration (0x91960)
// ---------------------------------------------------------------------------

int TssSdkFramework::init_config() {
    int ret = 0;
    char conf_path_file[1024];
    char conf_file[1024];

    // g_config is completely overwritten with generated defaults first.
    g_config.construct();

    std::memset(conf_path_file, 0, sizeof(conf_path_file));
    std::memset(conf_file, 0, sizeof(conf_file));

    // g_is_old_sdk defaults to 1 in the shipped image (.data).
    if (g_is_old_sdk == 0) {
        snprintf(conf_path_file, sizeof(conf_path_file), "%s",
                 "./tss_sdk_conf_path.xml");
        snprintf(conf_file, sizeof(conf_file), "%s", "./tss_sdk_conf.xml");
        g_config.usercontrol_config_.use_idx_ = 0;
    } else {
        snprintf(conf_path_file, sizeof(conf_path_file), "%s",
                 "./antisvrimport_conf_path.xml");
        snprintf(conf_file, sizeof(conf_file), "%s",
                 "./antisvrimport_conf.xml");
        g_config.usercontrol_config_.use_idx_ = 1;
    }

    // The path file names the real configuration file through
    // path_config::config_path.  A path longer than 0x300 bytes cannot be
    // copied into conf_file and is reported as failure.
    if (ZEN_OS::access(conf_path_file, 0) == 0) {
        if (g_path_config.fromXmlFile(conf_path_file, tsf4g_tdr::LIST_ENTRY,
                                      0) ==
            tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            std::size_t len = std::strlen(g_path_config.config_path);
            if (len > 0x300) {
                return -1;
            }
            std::memcpy(conf_file, g_path_config.config_path, len);
            conf_file[len] = '\0';
        }
    }

    if (ZEN_OS::access(conf_file, 0) == 0) {
        g_config.fromXmlFile(conf_file, tsf4g_tdr::LIST_ENTRY, 0);
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Log initialization (0x922f0)
// ---------------------------------------------------------------------------

int TssSdkFramework::init_log(char const *log_dir,
                              ZEN_LOG_PRIORITY log_priority) {
    if (log_dir == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("log_dir == NULL");
        return -1;
    }

    std::string log_file_prefix(log_dir);

    // 100 MiB, split into 2 files of at most 100 MiB; output_way == 1,
    // 15 recorded header fields (arguments observed at the call site).
    const std::size_t LOG_SIZE_MAX = 100 * 1024 * 1024;  // 0x06400000
    // The canonical logging boundary is static-only; the ORIG base subobject
    // is at offset 0, so the inherited non-virtual calls keep the shipped
    // ZEN_LogTrace_Basic mangled targets.
    ZEN_LogTrace_Basic *log_basic =
        reinterpret_cast<ZEN_LogTrace_Basic *>(ZEN_Trace_LogMsg::instance());
    int ret = log_basic->initialize(
        LOGDEVIDE_BY_SIZE, log_file_prefix.c_str(),
        true, true, LOG_SIZE_MAX, 2, 1, 15);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("init_log initialize failed");
        return -1;
    }

    log_basic->set_log_priority(log_priority);
    return 0;
}

// ---------------------------------------------------------------------------
// Framework init (0x92450)
// ---------------------------------------------------------------------------

int TssSdkFramework::init(unsigned int instance_id) {
    int rc = 0;
    ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_);

    if (is_init_succ_) {
        ZEN_Trace_LogMsg::debug_infoex("TssSdkFramework init successful");
        is_init_succ_ = true;
        rc = 0;
    } else {
        rc = init_config();
        if (rc != 0) {
            std::string err_str(
                "In CTssSdkFramework::init, init_config failed.");
            error_info_.set_last_error_msg(err_str.c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494b9;  // 300217
            rc = error_info_.last_error_info()->error_code;
        } else {
            const int LOG_FILE_NAME_MAX = 1024;
            char log_file_name[LOG_FILE_NAME_MAX + 1];
            int log_file_name_len = 0;

            std::memset(log_file_name, 0, sizeof(log_file_name));
            log_file_name_len = snprintf(log_file_name, LOG_FILE_NAME_MAX,
                                         "%s_%d", "/tmp/tss_sdk",
                                         instance_id);
            (void)log_file_name_len;

            rc = init_log(log_file_name,
                          (ZEN_LOG_PRIORITY)
                              g_config.log_config_.log_level_);
            if (rc != 0) {
                // The shipped binary reuses the init_config failure text on
                // this branch as well (same .rodata address 0x219c98).
                std::string err_str(
                    "In CTssSdkFramework::init, init_config failed.");
                error_info_.set_last_error_msg(err_str.c_str());
                ZEN_Trace_LogMsg::debug_errorex(
                    "%s", error_info_.get_last_error_msg());
                error_info_.last_error_info()->error_code = 0x494b9;
                rc = error_info_.last_error_info()->error_code;
            } else {
                ZEN_Trace_LogMsg::debug_infoex(
                    "TssSdkFramework init successful");
                is_init_succ_ = true;
                rc = 0;
            }
        }
    }

    return rc;
}

// ---------------------------------------------------------------------------
// Framework fini (0x92730)
// ---------------------------------------------------------------------------

int TssSdkFramework::fini(tss_sdk_if::ETssSdkPluginId plugin_id) {
    is_init_succ_ = false;
    ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_);

    std::vector<tss_sdk_if::ETssSdkPluginId> plugin_id_table;
    if (plugin_id == tss_sdk_if::ETSS_SDK_PLUGIN_ID_ALL) {
        plugin_id_table = *plugin_mgr_->get_active_plugin_id_table();
    } else {
        plugin_id_table.push_back(plugin_id);
    }

    std::vector<tss_sdk_if::ETssSdkPluginId>::iterator iter;
    for (iter = plugin_id_table.begin(); iter != plugin_id_table.end();
         ++iter) {
        ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(*iter);
        if (plugin == NULL) {
            std::ostringstream err;
            err << "In CTssSdkFramework::fini, plugin mgr get active plugin "
                << *iter << " failed. " << plugin_mgr_->get_last_error();
            error_info_.set_last_error_msg(err.str().c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494b1;  // 300209
            return error_info_.last_error_info()->error_code;
        }

        if (plugin->fini() != 0) {
            std::ostringstream err;
            err << "In CTssSdkFramework::fini, plugin " << *iter
                << " fini failed. " << plugin->get_last_error();
            error_info_.set_last_error_msg(err.str().c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494ab;  // 300203
            return error_info_.last_error_info()->error_code;
        }

        if (plugin_mgr_->deactivate_plugin(*iter) != 0) {
            std::ostringstream err;
            err << "In CTssSdkFramework::fini, plugin mgr deactivate plugin "
                << *iter << " failed. " << plugin_mgr_->get_last_error();
            error_info_.set_last_error_msg(err.str().c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494b0;  // 300208
            return error_info_.last_error_info()->error_code;
        }
    }

    get_not_need_channel_plugin_id_table_();
    return 0;
}

// ---------------------------------------------------------------------------
// Active-plugin snapshot filtered for the channel-independent set (0x91b30)
// ---------------------------------------------------------------------------

int TssSdkFramework::get_not_need_channel_plugin_id_table_() {
    not_need_channel_plugin_id_table_ =
        *plugin_mgr_->get_active_plugin_id_table();

    std::vector<tss_sdk_if::ETssSdkPluginId>::iterator iter;
    iter = std::remove(not_need_channel_plugin_id_table_.begin(),
                       not_need_channel_plugin_id_table_.end(),
                       tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL);
    iter = std::remove(not_need_channel_plugin_id_table_.begin(), iter,
                       tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE);
    iter = std::remove(not_need_channel_plugin_id_table_.begin(), iter,
                       tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA);
    iter = std::remove(not_need_channel_plugin_id_table_.begin(), iter,
                       tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC);
    not_need_channel_plugin_id_table_.erase(
        iter, not_need_channel_plugin_id_table_.end());

    return 0;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// ORIG framework static-init step 9 (0x920e8 and 0x92101): the real
// ObjectCreator members/guards, in shipped order (PluginMgr, then CommSvc).
// These definitions are weak; the guard/object symbols are shared with the
// owner instantiation in tss_sdk_comm_svc_agent.cpp, so whichever initializer
// runs first constructs the singleton and the other calls the guarded
// instance() again as a no-op.  The old report's claim that the two pointer
// initializers above made these unnecessary is superseded.
// ---------------------------------------------------------------------------
template Singleton<tss_sdk_impl::TssSdkPluginMgr>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkPluginMgr>::object_creator;
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
