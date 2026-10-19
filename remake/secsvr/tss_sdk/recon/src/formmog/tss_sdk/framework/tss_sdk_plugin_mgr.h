// tss_sdk_plugin.h + tss_sdk_plugin_mgr.h, reconstructed from
// libtss_sdk.so.1.0.394.
//
// This is the single complete declaration of ITssSdkPlugin and
// TssSdkPluginMgr used by every translation unit.  The manager is 56 bytes;
// its four std::vector members and 4-byte CThreadErrorInfo match DWARF and
// allow the guarded singleton constructor (ORIG 0x000914f0) to call
// TssSdkPluginMgr::TssSdkPluginMgr() at 0x00097ca0.
//
// Singleton storage and the explicit manager instantiation remain owned by
// tss_sdk_comm_svc_agent.cpp.  Consumers include this header instead of
// maintaining local member-only class views.
#ifndef TSS_RECON_FRAMEWORK_TSS_SDK_PLUGIN_MGR_H_
#define TSS_RECON_FRAMEWORK_TSS_SDK_PLUGIN_MGR_H_

#include <string>
#include <utility>
#include <vector>
#include "src/commlib/zenlib/zen_thread_error_info.h"

namespace tss_sdk_if {

enum ETssSdkPluginId {
    ETSS_SDK_PLUGIN_ID_INVALID = -2,
    ETSS_SDK_PLUGIN_ID_ALL = -1,
    ETSS_SDK_PLUGIN_ID_BEGIN = 0,
    ETSS_SDK_PLUGIN_ID_CHANNEL = 1,
    ETSS_SDK_PLUGIN_ID_USER_CONTROL = 2,
    ETSS_SDK_PLUGIN_ID_PROTO_PROC = 3,
    ETSS_SDK_PLUGIN_ID_SAFEMODE = 4,
    ETSS_SDK_PLUGIN_ID_CAPTCHA = 5,
    ETSS_SDK_PLUGIN_ID_UIC = 6,
    ETSS_SDK_PLUGIN_ID_DATA_TRANS = 7,
    ETSS_SDK_PLUGIN_ID_END = 8
};

}  // namespace tss_sdk_if


namespace tss_sdk_impl {

typedef int (*TssSdkPluginCallbackProc)(void *arg);

// tss_sdk_plugin.h, sizeof == 16.  Virtual order is fixed by the shipped
// vtables: dtor D1, dtor D0, init, fini, proc, notify.  The non-virtual
// helpers are direct symbols (get_id 0x00097120, get_last_error 0x000972d0).
class ITssSdkPlugin {
public:
    ITssSdkPlugin(tss_sdk_if::ETssSdkPluginId sdk_plugin_id);
    virtual ~ITssSdkPlugin();

    virtual int init(void *init_data) = 0;
    virtual int fini() = 0;
    virtual int proc(void *data) = 0;
    virtual int notify(void *data) = 0;

    void register_callback(TssSdkPluginCallbackProc callback_proc);
    int get_last_error_code();
    std::string get_last_error();
    tss_sdk_if::ETssSdkPluginId get_id();

protected:
    TssSdkPluginCallbackProc callback_proc_;              // +0x04
    CThreadErrorInfo error_info_;                         // +0x08
    tss_sdk_if::ETssSdkPluginId sdk_plugin_id_;           // +0x0c
};

// tss_sdk_plugin_mgr.h, sizeof == 56.  total_plugin_table_ is indexed directly
// by ETssSdkPluginId and resized to ETSS_SDK_PLUGIN_ID_END (8) entries, all
// {NULL,false}, by the constructor.
class TssSdkPluginMgr {
public:
    TssSdkPluginMgr();
    ~TssSdkPluginMgr();

    void register_plugin(ITssSdkPlugin *plugin);
    unsigned int get_registered_plugin_count();
    int init();
    ITssSdkPlugin *activate_plugin(tss_sdk_if::ETssSdkPluginId id);
    int deactivate_plugin(tss_sdk_if::ETssSdkPluginId id);
    ITssSdkPlugin *get_active_plugin(tss_sdk_if::ETssSdkPluginId id);
    std::vector<tss_sdk_if::ETssSdkPluginId> *get_active_plugin_id_table();
    std::string get_last_error();
    int get_last_error_code();
    bool is_id_invalid(tss_sdk_if::ETssSdkPluginId id);
    bool is_id_repeated(tss_sdk_if::ETssSdkPluginId id);
    bool is_id_activated(tss_sdk_if::ETssSdkPluginId id);

private:
    unsigned int registered_plugin_count_;                // +0x00
    std::vector<tss_sdk_if::ETssSdkPluginId>
        invalid_plugin_id_table_;                         // +0x04
    std::vector<tss_sdk_if::ETssSdkPluginId>
        repeated_plugin_id_table_;                        // +0x10
    std::vector<tss_sdk_if::ETssSdkPluginId>
        active_plugin_id_table_;                          // +0x1c
    std::vector<std::pair<ITssSdkPlugin *, bool> >
        total_plugin_table_;                              // +0x28
    CThreadErrorInfo error_info_;                         // +0x34
};

}  // namespace tss_sdk_impl

// Layout contracts from DWARF.  A negative array bound fails the build if a
// member is accidentally added, removed or given the wrong type.
typedef char tss_recon_plugin_mgr_size_must_be_56[
    sizeof(tss_sdk_impl::TssSdkPluginMgr) == 56 ? 1 : -1];
typedef char tss_recon_itss_sdk_plugin_size_must_be_16[
    sizeof(tss_sdk_impl::ITssSdkPlugin) == 16 ? 1 : -1];
typedef char tss_recon_thread_error_info_size_must_be_4[
    sizeof(CThreadErrorInfo) == 4 ? 1 : -1];

#endif  // TSS_RECON_FRAMEWORK_TSS_SDK_PLUGIN_MGR_H_
