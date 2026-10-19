// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/framework/tss_sdk_framework.cpp
//
// Reconstructed definitions (shipped symbol address / size):
//   TssSdkFramework::notify(TssSdkPluginNotifyData)          0x00093230-0x00093b85
//   TssSdkFramework::init_plugins(TssSdkPluginInitData const*,
//                                 unsigned int)               0x00093b90-0x00094e95
//   TssSdkFramework::proc_buf_from_backend_(char const*,
//                                           size_t)             0x00094ea0-0x000957e5
//   TssSdkFramework::proc()                                  0x000957f0-0x000966a5
//
// The four functions are static members: none of them consumes an incoming
// frame pointer.  They are reconstructed against the process-wide framework
// state declared in tss_sdk_framework.h:
//   volatile bool is_init_succ_                (BSS 0x002a00ab)
//   ZEN_Thread_Mutex thread_mutex_             (BSS 0x002a00ac)
//   CThreadErrorInfo error_info_               (BSS 0x002a00c8)
//   TssSdkPluginMgr *plugin_mgr_               (BSS 0x002a00cc)
//   TssSdkCommSvc *comm_svc_                   (BSS 0x002a00d0)
//   std::vector<ETssSdkPluginId>
//       not_need_channel_plugin_id_table_      (BSS 0x002a00d4)
//   TssSdkTimerHandler *timer_handler_         (BSS 0x002a00e0)
// There is no TssSdkFramework instance in these paths.
//
// Interface layouts used below (DWARF tss_sdk_plugin.h / tss_sdk_comm_svc.h):
//   ITssSdkPlugin (sizeof 16)
//     +0x00 vptr
//     +0x04 TssSdkPluginCallbackProc callback_proc_
//     +0x08 CThreadErrorInfo error_info_
//     +0x0c ETssSdkPluginId sdk_plugin_id_
//     vtable at 0x00292d80, address point 0x00292d88:
//       [0] D1 dtor 0x00097250, [1] D0 dtor 0x00097200,
//       [2] init, [3] fini, [4] proc, [5] notify.
//     The dispatcher calls +0x08 (init), +0x10 (proc) and +0x14 (notify).
//   TssSdkCommSvc (sizeof 40)
//     +0x00 channel_id_, +0x04 plugin_mgr_, +0x08 dp_if_,
//     +0x0c six unsigned counters (24 bytes), +0x24 error_info_.
//   TssSdkPluginMgr (sizeof 56)
//     +0x00 registered_plugin_count_,
//     +0x04/+0x10/+0x1c std::vector<ETssSdkPluginId> invalid/repeated/active,
//     +0x28 std::vector<std::pair<ITssSdkPlugin*, bool> > total_plugin_table_,
//     +0x34 CThreadErrorInfo error_info_.
//
// Error codes written to CThreadErrorInfo (plain integer constants in ORIG):
//   0x494a9 invalid init_plugins arguments
//   0x494aa plugin init failed
//   0x494ac plugin proc failed (framework level)
//   0x494ad plugin notify failed (framework level)
//   0x494ae plugin-manager init or DP config init failed
//   0x494af plugin activation failed
//   0x494b1 active-plugin lookup failed
//   0x494b2 comm-svc backend receive failed (non-timeout/non-lookup)
//
// The log strings are copied verbatim, including the original
// "noitfy failed." typo in notify().

#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ---------------------------------------------------------------------------
// External libc / zenlib boundaries: canonical declarations.
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_guard.h"

// ZEN_Lock_Guard<T> comes from zen_lock_guard.h (sizeof == 4, lock_ at +0).

// ---------------------------------------------------------------------------
// tss_sdk_if declarations.
// ---------------------------------------------------------------------------

namespace tss_sdk_if {


typedef int (*TssSdkPluginCallbackProc)(void *arg);

// tss_sdk_plugin.h line 41, sizeof 12.
struct TssSdkPluginInitData {
    ETssSdkPluginId plugin_id_;                  // +0x00
    TssSdkPluginCallbackProc callback_proc_;     // +0x04
    void *init_data_;                            // +0x08
};

// tss_sdk_plugin.h line 49, sizeof 8.
struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;                  // +0x00
    void *notify_data_;                          // +0x04
};

}  // namespace tss_sdk_if

namespace tss_dp_comm {

int init_dp_config();

}  // namespace tss_dp_comm

namespace tss_sdk_impl {


// tss_sdk_timer_handler.h line 14, sizeof 12.
class TssSdkTimerHandler {
public:
    void check_monitor(long now);
    void update_time(long now);

private:
    void *time_provider_;    // +0x00 (Comm_Time_Provider*)
    void *stat_monitor_;     // +0x04 (Comm_Stat_Monitor*)
    time_t last_check_;      // +0x08
};

// comm_app_frame.h line 33.  Only frame_command_ (+0x08) is read directly.
// For command 300001 the dispatcher reads the 16-bit big-endian business id
// at recv_buf+0x3a, which is appdata+0x08 (SendToSDK::to_busi_id_).
struct Comm_App_Frame {
    unsigned int frame_length_;    // +0x00
    unsigned int frame_option_;    // +0x04 (union)
    unsigned int frame_command_;   // +0x08
};

// tss_sdk_framework.h.  All four reconstructed methods are static.
class TssSdkFramework {
public:
    TssSdkFramework();
    ~TssSdkFramework();

    static int init(unsigned int uin);
    static int init_plugins(
        const tss_sdk_if::TssSdkPluginInitData *plugin_init_data_table,
        unsigned int plugin_count);
    static int fini(tss_sdk_if::ETssSdkPluginId plugin_id);
    static int proc();
    static int notify(tss_sdk_if::TssSdkPluginNotifyData plugin_notify_data);
    static int get_last_error_code();
    static std::string get_last_error();
    static int init_config();
    static int get_not_need_channel_plugin_id_table_();
    static int proc_buf_from_backend_(const char *recv_buf,
                                      size_t recv_buf_size);
    static tss_sdk_if::ETssSdkPluginId get_plugin_id(unsigned short busi_id);

private:
    static volatile bool is_init_succ_;
    static ZEN_Thread_Mutex thread_mutex_;
    static CThreadErrorInfo error_info_;
    static TssSdkPluginMgr *plugin_mgr_;
    static TssSdkCommSvc *comm_svc_;
    static std::vector<tss_sdk_if::ETssSdkPluginId>
        not_need_channel_plugin_id_table_;
    static TssSdkTimerHandler *timer_handler_;
};

// ---------------------------------------------------------------------------
// Local predicates used by std::find_if in init_plugins (0x000916c0 and
// 0x000916e0 in ORIG; both 17-byte leaf functions).
// ---------------------------------------------------------------------------

static bool is_equal_to_usercontrol_plugin(
    const tss_sdk_if::TssSdkPluginInitData &plugin_init_data) {
    return plugin_init_data.plugin_id_ ==
           tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL;
}

static bool is_equal_to_channel_plugin(
    const tss_sdk_if::TssSdkPluginInitData &plugin_init_data) {
    return plugin_init_data.plugin_id_ ==
           tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL;
}

// ---------------------------------------------------------------------------
// 0x00093230, source lines 715-769.
//
// if (!is_init_succ_) return 0;
// lock timer mutex; now = time(0); check_monitor(now); update_time(now); unlock
// plugin = plugin_mgr_->get_active_plugin(notify_data.plugin_id_)
// if (!plugin) -> error 0x494b1 ("... plugin mgr get active plugin <id> failed.
//                                <manager error>")
// rc = plugin->notify(notify_data.notify_data_)       [vtable +0x14]
// if (rc == 0) return 0
// error 0x494ad ("... plugin <id> noitfy failed. rc is <rc>. <plugin error>")
// ---------------------------------------------------------------------------
int TssSdkFramework::notify(
    tss_sdk_if::TssSdkPluginNotifyData plugin_notify_data) {
    if (!is_init_succ_) {
        return 0;
    }

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> lock(thread_mutex_);
        time_t now = time(NULL);
        timer_handler_->check_monitor(now);
        timer_handler_->update_time(now);
    }

    ITssSdkPlugin *plugin =
        plugin_mgr_->get_active_plugin(plugin_notify_data.plugin_id_);
    if (plugin == NULL) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream
            << "In CTssSdkFramework::notify, plugin mgr get active plugin "
            << (int)plugin_notify_data.plugin_id_ << " failed. " << err_str;
        std::string err_msg = err_str_stream.str();
        error_info_.set_last_error_msg(err_msg.c_str());
        ZEN_Trace_LogMsg::debug_errorex(
            "%s", error_info_.get_last_error_msg());
        error_info_.last_error_info()->error_code = 0x494b1;
        return error_info_.last_error_info()->error_code;
    }

    int rc = plugin->notify(plugin_notify_data.notify_data_);
    if (rc == 0) {
        return 0;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkFramework::notify, plugin "
                   << (int)plugin_notify_data.plugin_id_
                   << " noitfy failed. "
                   << "rc is " << rc << ". " << err_str;
    std::string err_msg = err_str_stream.str();
    error_info_.set_last_error_msg(err_msg.c_str());
    ZEN_Trace_LogMsg::debug_errorex("%s", error_info_.get_last_error_msg());
    error_info_.last_error_info()->error_code = 0x494ad;
    return error_info_.last_error_info()->error_code;
}

// ---------------------------------------------------------------------------
// 0x00093b90, source lines 250-... (locals at 265/282/283/316/318).
//
// Rejects a NULL table or zero plugin_count (error 0x494a9).
// Logs plugin_mgr_->get_registered_plugin_count().
// plugin_mgr_->init() must return 0 (else 0x494ae).
// Copies the table to a vector, moves the CHANNEL entry to vector[0] when
// present, then looks for USER_CONTROL; if present, tss_dp_comm::init_dp_config()
// must return 0 (else 0x494ae).  The activation loop itself walks the original
// caller table, not the vector.
// Under thread_mutex_: activate_plugin(id), register_callback(cb),
// init(init_data).  Activation failure -> 0x494af; init failure -> 0x494aa.
// On success calls get_not_need_channel_plugin_id_table_() and returns 0.
// ---------------------------------------------------------------------------
int TssSdkFramework::init_plugins(
    const tss_sdk_if::TssSdkPluginInitData *plugin_init_data_table,
    unsigned int plugin_count) {
    if (plugin_init_data_table == NULL || plugin_count == 0) {
        error_info_.set_last_error_msg(
            "In CTssSdkFramework::init_plugins, plugin_init_data_table is "
            "NULL or plugin_count <= 0.");
        ZEN_Trace_LogMsg::debug_errorex(
            "%s", error_info_.get_last_error_msg());
        error_info_.last_error_info()->error_code = 0x494a9;
        return error_info_.last_error_info()->error_code;
    }

    ZEN_Trace_LogMsg::debug_infoex(
        "The count of registered plugins is %d.",
        plugin_mgr_->get_registered_plugin_count());

    if (plugin_mgr_->init() != 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream
            << "In CTssSdkFramework::init_plugins, plugin mgr init failed. "
            << err_str;
        std::string err_msg = err_str_stream.str();
        error_info_.set_last_error_msg(err_msg.c_str());
        ZEN_Trace_LogMsg::debug_errorex(
            "%s", error_info_.get_last_error_msg());
        error_info_.last_error_info()->error_code = 0x494ae;
        return error_info_.last_error_info()->error_code;
    }

    std::vector<tss_sdk_if::TssSdkPluginInitData> plugin_init_data_vec(
        plugin_init_data_table, plugin_init_data_table + plugin_count);

    std::vector<tss_sdk_if::TssSdkPluginInitData>::iterator iter_channel =
        std::find_if(plugin_init_data_vec.begin(),
                     plugin_init_data_vec.end(),
                     is_equal_to_channel_plugin);
    if (iter_channel != plugin_init_data_vec.end()) {
        std::swap(plugin_init_data_vec[0], *iter_channel);
    }

    std::vector<tss_sdk_if::TssSdkPluginInitData>::iterator iter =
        std::find_if(plugin_init_data_vec.begin(),
                     plugin_init_data_vec.end(),
                     is_equal_to_usercontrol_plugin);
    if (iter != plugin_init_data_vec.end()) {
        if (tss_dp_comm::init_dp_config() != 0) {
            std::ostringstream err_str_stream;
            std::string err_str = plugin_mgr_->get_last_error();
            err_str_stream
                << "In CTssSdkFramework::init_plugins, plugin mgr init failed. "
                << err_str;
            std::string err_msg = err_str_stream.str();
            error_info_.set_last_error_msg(err_msg.c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494ae;
            return error_info_.last_error_info()->error_code;
        }
    }

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_);

        for (unsigned int i = 0; i < plugin_count; ++i) {
            ITssSdkPlugin *plugin = plugin_mgr_->activate_plugin(
                plugin_init_data_table[i].plugin_id_);
            if (plugin == NULL) {
                std::ostringstream err_str_stream;
                std::string err_str = plugin_mgr_->get_last_error();
                err_str_stream
                    << "In CTssSdkFramework::init_plugins, plugin mgr "
                       "activate plugin "
                    << (int)plugin_init_data_table[i].plugin_id_
                    << " failed. " << err_str;
                std::string err_msg = err_str_stream.str();
                error_info_.set_last_error_msg(err_msg.c_str());
                ZEN_Trace_LogMsg::debug_errorex(
                    "%s", error_info_.get_last_error_msg());
                error_info_.last_error_info()->error_code = 0x494af;
                return error_info_.last_error_info()->error_code;
            }

            plugin->register_callback(
                plugin_init_data_table[i].callback_proc_);

            int rc = plugin->init(plugin_init_data_table[i].init_data_);
            if (rc != 0) {
                std::ostringstream err_str_stream;
                std::string err_str = plugin->get_last_error();
                err_str_stream
                    << "In CTssSdkFramework::init_plugins, plugin "
                    << (int)plugin_init_data_table[i].plugin_id_
                    << " init failed. " << err_str;
                std::string err_msg = err_str_stream.str();
                error_info_.set_last_error_msg(err_msg.c_str());
                ZEN_Trace_LogMsg::debug_errorex(
                    "%s", error_info_.get_last_error_msg());
                error_info_.last_error_info()->error_code = 0x494aa;
                return error_info_.last_error_info()->error_code;
            }
        }

        get_not_need_channel_plugin_id_table_();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// 0x00094ea0, source lines 447-557.
//
// dest_plugin_id is selected from Comm_App_Frame::frame_command_:
//   0x493e1 (300001): get_plugin_id(ntohs(*(uint16*)(recv_buf + 0x3a)))
//   0x493e3 (300003): ETSS_SDK_PLUGIN_ID_USER_CONTROL
//   0x493e5, 0x493e7, 0x493ea: ETSS_SDK_PLUGIN_ID_PROTO_PROC
//   0x493ec, 0x493ee:          ETSS_SDK_PLUGIN_ID_SAFEMODE
//   0x493ef (300015):          ETSS_SDK_PLUGIN_ID_UIC
//   everything else:           ETSS_SDK_PLUGIN_ID_INVALID (-2), after
//                              debug_errorex("%s, invalid cmd %u", ...)
// Every path then logs dest_plugin_id and looks up the active plugin.  A
// missing plugin produces 0x494b1; a nonzero plugin->proc [vtable +0x10]
// produces 0x494ac.  Those two paths do not call debug_errorex; they only set
// CThreadErrorInfo and return its code.  recv_buf_size is unused by ORIG.
// ---------------------------------------------------------------------------
int TssSdkFramework::proc_buf_from_backend_(const char *recv_buf,
                                            size_t recv_buf_size) {
    (void)recv_buf_size;

    const Comm_App_Frame *app_frame =
        reinterpret_cast<const Comm_App_Frame *>(recv_buf);
    tss_sdk_if::ETssSdkPluginId dest_plugin_id =
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_INVALID;

    switch (app_frame->frame_command_) {
    case 0x493e1:  // 300001
        dest_plugin_id = get_plugin_id(ntohs(
            *reinterpret_cast<const unsigned short *>(recv_buf + 0x3a)));
        break;
    case 0x493e3:  // 300003
        dest_plugin_id = tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL;
        break;
    case 0x493e5:  // 300005
    case 0x493e7:  // 300007
    case 0x493ea:  // 300010
        dest_plugin_id = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
        break;
    case 0x493ec:  // 300012
    case 0x493ee:  // 300014
        dest_plugin_id = tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;
        break;
    case 0x493ef:  // 300015
        dest_plugin_id = tss_sdk_if::ETSS_SDK_PLUGIN_ID_UIC;
        break;
    default:
        dest_plugin_id = tss_sdk_if::ETSS_SDK_PLUGIN_ID_INVALID;
        ZEN_Trace_LogMsg::debug_errorex(
            "%s, invalid cmd %u", __PRETTY_FUNCTION__,
            app_frame->frame_command_);
        break;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkFramework::proc_buf_from_backend_, dest_plugin_id=%d.",
        (int)dest_plugin_id);

    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(dest_plugin_id);
    if (plugin == NULL) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream
            << "In CTssSdkFramework::proc_buf_from_backend_, Plugin mgr get "
               "active plugin "
            << (int)dest_plugin_id << " failed. " << err_str;
        std::string err_msg = err_str_stream.str();
        error_info_.set_last_error_msg(err_msg.c_str());
        error_info_.last_error_info()->error_code = 0x494b1;
        return error_info_.last_error_info()->error_code;
    }

    int rc = plugin->proc(
        const_cast<void *>(reinterpret_cast<const void *>(recv_buf)));
    if (rc == 0) {
        return 0;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream
        << "In CTssSdkFramework::proc_buf_from_backend_, plugin "
        << (int)dest_plugin_id << " proc failed. " << err_str;
    std::string err_msg = err_str_stream.str();
    error_info_.set_last_error_msg(err_msg.c_str());
    error_info_.last_error_info()->error_code = 0x494ac;
    return error_info_.last_error_info()->error_code;
}

// ---------------------------------------------------------------------------
// 0x000957f0, source lines 563-711.
//
// if (!is_init_succ_) return 0;
// One-shot s_last_proc_thread_id = gettid (syscall 0xe0); a different caller
// thread is reported but does not abort:
//   "In CTssSdkFramework::proc, multi-thread call proc,|%u|%u|%u"
// now = time(0); static s_last_count_timestamp = now (guarded one-shot).
// ++proc_call_times.  If (unsigned)(now - timestamp) > 299, recompute
//   max_recv_pkg_per_time = (now - timestamp) * 1000 / proc_call_times + 1,
// log "count max_recv_pkg_per_time=%u proc_call_times=%u", clear the call
// count and reset the timestamp.  max_recv_pkg_per_time defaults to 2000
// (0x000297940 in .data).
// Under the timer mutex: check_monitor(now); update_time(now).
// Clear error code; loop at most max_recv_pkg_per_time times:
//   comm_svc_->recv_buf_from_backend(&buf, &size)
//     rc == 0x49449 (active channel plugin missing) or
//     rc == 0x4944c (channel timeout/no data) -> break
//     other nonzero: log comm-svc error, set 0x494b2, break
//   proc_buf_from_backend_(buf, size); nonzero -> log the already-set error
//   message and continue.
// Then walk not_need_channel_plugin_id_table_ (vector<ETssSdkPluginId>):
//   get_active_plugin(id); missing -> log + 0x494b1, continue;
//   plugin->proc(NULL) [vtable +0x10]; nonzero -> log + 0x494ac, continue.
// Return error_info_.last_error_info()->error_code.
// ---------------------------------------------------------------------------
int TssSdkFramework::proc() {
    if (!is_init_succ_) {
        return 0;
    }

    static unsigned int s_last_proc_thread_id = 0;  // 0x002a06d0
    unsigned int current_thread_id = (unsigned int)syscall(0xe0);
    if (s_last_proc_thread_id == 0) {
        s_last_proc_thread_id = current_thread_id;
    } else if (current_thread_id != s_last_proc_thread_id) {
        unsigned int retry_thread_id = (unsigned int)syscall(0xe0);
        ZEN_Trace_LogMsg::debug_errorex(
            "In CTssSdkFramework::proc, multi-thread call proc,|%u|%u|%u",
            (unsigned int)getpid(), s_last_proc_thread_id, retry_thread_id);
    }

    time_t now = time(NULL);
    // ORIG's guard-init branch stores the just-read `now` into this static
    // (0x002a06cc); the guard variable itself is 0x002a06c0.
    static time_t s_last_count_timestamp = now;
    static unsigned int proc_call_times = 0;            // 0x002a06c8
    static unsigned int max_recv_pkg_per_time = 2000;   // 0x00297940

    ++proc_call_times;
    unsigned int elapsed =
        (unsigned int)(now - s_last_count_timestamp);
    if (elapsed > 299) {
        max_recv_pkg_per_time =
            elapsed * 1000 / proc_call_times + 1;
        ZEN_Trace_LogMsg::debug_errorex(
            "count max_recv_pkg_per_time=%u proc_call_times=%u",
            max_recv_pkg_per_time, proc_call_times);
        proc_call_times = 0;
        s_last_count_timestamp = now;
    }

    {
        ZEN_Lock_Guard<ZEN_Thread_Mutex> scope_lock(thread_mutex_);
        timer_handler_->check_monitor(now);
        timer_handler_->update_time(now);
    }

    error_info_.last_error_info()->error_code = 0;

    for (unsigned int proc_pkg_count = 0;
         proc_pkg_count < max_recv_pkg_per_time; ++proc_pkg_count) {
        const char *recv_buf = NULL;
        size_t recv_buf_size = 0;

        int rc = comm_svc_->recv_buf_from_backend(&recv_buf,
                                                  &recv_buf_size);
        if (rc != 0) {
            if (rc == 0x49449 || rc == 0x4944c) {
                break;
            }
            std::ostringstream err_str_stream;
            std::string err_str = comm_svc_->get_last_error();
            err_str_stream
                << "In CTssSdkFramework::proc, sdk comm svc recv buf from "
                   "backend failed. "
                << err_str;
            std::string err_msg = err_str_stream.str();
            error_info_.set_last_error_msg(err_msg.c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494b2;
            break;
        }

        rc = proc_buf_from_backend_(recv_buf, recv_buf_size);
        if (rc != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
        }
    }

    for (std::vector<tss_sdk_if::ETssSdkPluginId>::iterator iter =
             not_need_channel_plugin_id_table_.begin();
         iter != not_need_channel_plugin_id_table_.end(); ++iter) {
        ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(*iter);
        if (plugin == NULL) {
            std::ostringstream err_str_stream;
            std::string err_str = plugin_mgr_->get_last_error();
            err_str_stream
                << "In CTssSdkFramework::proc, plugin mgr get active plugin "
                << (int)*iter << " failed. " << err_str;
            std::string err_msg = err_str_stream.str();
            error_info_.set_last_error_msg(err_msg.c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494b1;
            continue;
        }

        int rc = plugin->proc(NULL);
        if (rc != 0) {
            std::ostringstream err_str_stream;
            std::string err_str = plugin->get_last_error();
            err_str_stream << "In CTssSdkFramework::proc, Plugin "
                           << (int)*iter << " proc failed. " << err_str;
            std::string err_msg = err_str_stream.str();
            error_info_.set_last_error_msg(err_msg.c_str());
            ZEN_Trace_LogMsg::debug_errorex(
                "%s", error_info_.get_last_error_msg());
            error_info_.last_error_info()->error_code = 0x494ac;
        }
    }

    return error_info_.last_error_info()->error_code;
}

}  // namespace tss_sdk_impl
