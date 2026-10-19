// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.cpp
// Source lines observed through DWARF: ctor 20, dtor 34, register_plugin 41,
// init 81, activate_plugin 128, deactivate_plugin 173, get_active_plugin 200,
// get_last_error 281.
//
// Reconstructed definitions (addresses are the shipped symbol starts; the
// end excludes inter-symbol padding):
//   TssSdkPluginMgr::TssSdkPluginMgr() (C1)        0x00097ca0-0x00097e77
//   TssSdkPluginMgr::TssSdkPluginMgr() (C2)        0x00098290-0x00098467
//   TssSdkPluginMgr::~TssSdkPluginMgr() (D2)       0x00098470-0x000985b4
//   TssSdkPluginMgr::~TssSdkPluginMgr() (D1)       0x000985c0-0x00098704
//   TssSdkPluginMgr::get_registered_plugin_count() 0x00097990-0x00097999
//   TssSdkPluginMgr::get_active_plugin_id_table()  0x000979a0-0x000979aa
//   TssSdkPluginMgr::is_id_invalid(...)            0x000979b0-0x000979c3
//   TssSdkPluginMgr::get_last_error_code()         0x00097a40-0x00097a67
//   TssSdkPluginMgr::is_id_repeated(...)           0x00097a70-0x00097a88
//   TssSdkPluginMgr::is_id_activated(...)          0x00097a90-0x00097aef
//   TssSdkPluginMgr::register_plugin(...)          0x00097af0-0x00097bcf
//   TssSdkPluginMgr::get_last_error()              0x00097bd0-0x00097c9f
//   TssSdkPluginMgr::get_active_plugin(...)        0x00098740-0x00099204
//   TssSdkPluginMgr::deactivate_plugin(...)        0x00099210-0x00099824
//   TssSdkPluginMgr::activate_plugin(...)          0x00099830-0x00099f59
//   TssSdkPluginMgr::init()                        0x00099f60-0x0009a794
//
// Class layout from DWARF (sizeof == 56):
//   +0x00 unsigned int registered_plugin_count_
//   +0x04 std::vector<tss_sdk_if::ETssSdkPluginId> invalid_plugin_id_table_
//   +0x10 std::vector<tss_sdk_if::ETssSdkPluginId> repeated_plugin_id_table_
//   +0x1c std::vector<tss_sdk_if::ETssSdkPluginId> active_plugin_id_table_
//   +0x28 std::vector<std::pair<ITssSdkPlugin*, bool> > total_plugin_table_
//   +0x34 CThreadErrorInfo error_info_ (4-byte pthread_key_t)
//
// total_plugin_table_ is indexed directly by ETssSdkPluginId and is resized
// to ETSS_SDK_PLUGIN_ID_END (8) entries, all {NULL,false}, in the ctor.
// Slot 0 and slot 8 are never used: is_id_invalid accepts only ids 1..7.
//
// The manager never calls an ITssSdkPlugin virtual method.  register_plugin
// calls the non-virtual ITssSdkPlugin::get_id() (direct call at 0x97b13 to
// the shipped 0x97120 symbol); the interface's four virtual slots are only
// consumed by TssSdkFramework / business callers.

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h"

#include "src/commlib/zenlib/zen_singleton.h"

// ORIG tss_sdk_plugin_mgr.cpp static-initialization block: independent
// 16-entry COMM_STAT_ITEM_WITH_NAME + __ioinit + status dtor plus the weak
// Singleton<TssSdkPluginMgr>::ObjectCreator pre-create rooted in this CU
// (0x0029f... in the shipped DSO; the same weak object is also rooted by the
// framework and comm-service CUs, so the guard makes only one construction).
#define TSS_SDK_STATIC_INIT_ID tss_sdk_plugin_mgr
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

template Singleton<tss_sdk_impl::TssSdkPluginMgr>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkPluginMgr>::object_creator;

// error_info_ access macro shipped by tss_sdk_plugin_mgr.h line 18.
#define m_plugin_mgr_last_error_code \
    ((error_info_.last_error_info())->error_code)
namespace tss_sdk_impl {


// ---------------------------------------------------------------------------
// TssSdkPluginMgr
// ---------------------------------------------------------------------------

// 0x00097ca0 (C1) / 0x00098290 (C2), source line 20.
// The ctor zeroes the counter and all vectors (the vectors' default ctors),
// resizes the id-indexed table to 8 {NULL,false} entries, resets the thread
// error code and logs the object address.
TssSdkPluginMgr::TssSdkPluginMgr()
    : registered_plugin_count_(0) {
    // vector<pair<ITssSdkPlugin*,bool> >::resize(unsigned int, value_type)
    // is called at source line 21 and emits _M_fill_insert(begin, 8, zero).
    total_plugin_table_.resize(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_END,
        std::pair<ITssSdkPlugin *, bool>(static_cast<ITssSdkPlugin *>(0),
                                        false));
    m_plugin_mgr_last_error_code = 0;
    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkPluginMgr ctor. Address is 0x%x.", this);
}

// 0x00098470 (D2) / 0x000985c0 (D1), source line 34.
// Default member destruction order: error_info_ (+0x34), then the vectors
// +0x28, +0x1c, +0x10, +0x04.  No vtable is involved.
TssSdkPluginMgr::~TssSdkPluginMgr() {
}

// 0x00097990, source line 37.
unsigned int TssSdkPluginMgr::get_registered_plugin_count() {
    return registered_plugin_count_;
}

// 0x000979a0, source line 48.  Returns the address of the active-id vector.
std::vector<tss_sdk_if::ETssSdkPluginId> *
TssSdkPluginMgr::get_active_plugin_id_table() {
    return &active_plugin_id_table_;
}

// 0x000979b0, source line 55.
// (unsigned)(id - 1) > 6, i.e. only 1..7 are valid.
bool TssSdkPluginMgr::is_id_invalid(tss_sdk_if::ETssSdkPluginId id) {
    unsigned int value = static_cast<unsigned int>(id) - 1u;
    return value > 6u;
}

// 0x00097a70, source line 58.  Raw slot lookup, no bounds check; callers
// must have passed is_id_invalid first.
bool TssSdkPluginMgr::is_id_repeated(tss_sdk_if::ETssSdkPluginId id) {
    return total_plugin_table_[static_cast<unsigned int>(id)].first != 0;
}

// 0x00097a90, source line 61.  Linear std::find over active_plugin_id_table_.
bool TssSdkPluginMgr::is_id_activated(tss_sdk_if::ETssSdkPluginId id) {
    return std::find(active_plugin_id_table_.begin(),
                     active_plugin_id_table_.end(),
                     id) != active_plugin_id_table_.end();
}

// 0x00097af0, source line 41.
// Invalid ids go to invalid_plugin_id_table_; duplicate ids go to
// repeated_plugin_id_table_; neither increments the count nor overwrites a
// slot.  A fresh registration stores {plugin,false} in slot id.
void TssSdkPluginMgr::register_plugin(ITssSdkPlugin *plugin) {
    tss_sdk_if::ETssSdkPluginId id = plugin->get_id();

    if (is_id_invalid(id)) {
        invalid_plugin_id_table_.push_back(id);
        return;
    }

    if (is_id_repeated(id)) {
        repeated_plugin_id_table_.push_back(id);
        return;
    }

    ++registered_plugin_count_;
    total_plugin_table_[id].first = plugin;
    total_plugin_table_[id].second = false;

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkPluginMgr::register_plugin, registered plugin id is "
        "%d.The address of plugin is 0x%x.The 'first' is 0x%x."
        "The 'second' is %d.",
        static_cast<int>(id), plugin, total_plugin_table_[id].first,
        static_cast<int>(total_plugin_table_[id].second));
}

// 0x00097bd0, source line 281.
// Error code 0 yields "", otherwise the stored message is copied with its
// strlen.  The empty-string representation is the process-wide std::string
// empty rep (result starts life as an empty std::string).
std::string TssSdkPluginMgr::get_last_error() {
    std::string result;

    if (error_info_.last_error_info()->error_code == 0) {
        result.assign("", 0);
    } else {
        const char *message = error_info_.get_last_error_msg();
        result.assign(message, std::strlen(message));
    }

    return result;
}

// 0x00097a40, source line 51 (macro m_plugin_mgr_last_error_code).
int TssSdkPluginMgr::get_last_error_code() {
    return m_plugin_mgr_last_error_code;
}

// 0x00098740, source line 200.
// Inactive (or invalid) slots return NULL after storing an error:
//   invalid id      -> error code 0x494b5 (300213)
//   inactive plugin -> error code 0x494b8 (300216)
// The active flag, not the pointer, decides success.
ITssSdkPlugin *
TssSdkPluginMgr::get_active_plugin(tss_sdk_if::ETssSdkPluginId id) {
    if (is_id_invalid(id)) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::get_active_plugin, the invalid "
                  "plugin id is " << static_cast<int>(id) << ".";
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b5;
        return 0;
    }

    if (!total_plugin_table_[id].second) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::get_active_plugin, the plugin "
               << static_cast<int>(id) << " is not activated.";
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b8;
        return 0;
    }

    return total_plugin_table_[id].first;
}

// 0x00099210, source line 173.
// Invalid id returns the error code itself (0x494b5 / 300213).
// The normal path removes every occurrence of id from the active-id vector,
// clears the activation flag, logs, and returns 0.  The error state is not
// cleared on success.
int TssSdkPluginMgr::deactivate_plugin(tss_sdk_if::ETssSdkPluginId id) {
    if (is_id_invalid(id)) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::deactivate_plugin, the invalid "
                  "plugin id is " << static_cast<int>(id) << ".";
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b5;
        return m_plugin_mgr_last_error_code;
    }

    active_plugin_id_table_.erase(
        std::remove(active_plugin_id_table_.begin(),
                    active_plugin_id_table_.end(), id),
        active_plugin_id_table_.end());

    total_plugin_table_[id].second = false;

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkPluginMgr::deactivate_plugin, plugin id is %d."
        "The 'first' is 0x%x.The 'second' is %d.",
        static_cast<int>(id), total_plugin_table_[id].first,
        static_cast<int>(total_plugin_table_[id].second));

    return 0;
}

// 0x00099830, source line 128.
// Invalid id returns NULL with code 0x494b5 (300213).
// An already-active plugin is an error, code 0x494b7 (300215), return NULL,
// with one exception: ETSS_SDK_PLUGIN_ID_CHANNEL (1) is first deactivated
// (removing its id from active_plugin_id_table_) and then re-registered as
// active.  Success sets the slot flag, appends the id, logs and returns the
// slot pointer.
ITssSdkPlugin *
TssSdkPluginMgr::activate_plugin(tss_sdk_if::ETssSdkPluginId id) {
    if (is_id_invalid(id)) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::activate_plugin, the invalid "
                  "plugin id is " << static_cast<int>(id) << ".";
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b5;
        return 0;
    }

    if (is_id_activated(id)) {
        if (id == tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL) {
            deactivate_plugin(id);
        } else {
            std::ostringstream stream;
            stream << "In CTssSdkPluginMgr::activate_plugin, the plugin "
                   << static_cast<int>(id) << " is already activated.";
            std::string message = stream.str();
            error_info_.set_last_error_msg(message.c_str());
            m_plugin_mgr_last_error_code = 0x000494b7;
            return 0;
        }
    }

    total_plugin_table_[id].second = true;
    active_plugin_id_table_.push_back(id);

    ZEN_Trace_LogMsg::debug_debugEx(
        "In CTssSdkPluginMgr::activate_plugin, plugin id is %d. "
        "The 'first' is 0x%x. The 'second' is %d.",
        static_cast<int>(id), total_plugin_table_[id].first,
        static_cast<int>(total_plugin_table_[id].second));

    return total_plugin_table_[id].first;
}

// 0x00099f60, source line 81.
// Validation only; no plugin virtual method is invoked and the active-id
// table is not consulted.  Check order and return codes:
//   1. invalid_plugin_id_table_ non-empty   -> 0x494b5 (300213)
//   2. repeated_plugin_id_table_ non-empty  -> 0x494b6 (300214)
//   3. registered_plugin_count_ == 0        -> 0x494b4 (300212)
//   4. otherwise                            -> 0
// Failure paths store the message/error code and return the stored code.
int TssSdkPluginMgr::init() {
    if (!invalid_plugin_id_table_.empty()) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::init, the invalid plugin id is ";
        for (std::vector<tss_sdk_if::ETssSdkPluginId>::const_iterator it =
                 invalid_plugin_id_table_.begin();
             it != invalid_plugin_id_table_.end(); ++it) {
            stream << static_cast<int>(*it) << ".";
        }
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b5;
        return m_plugin_mgr_last_error_code;
    }

    if (!repeated_plugin_id_table_.empty()) {
        std::ostringstream stream;
        stream << "In CTssSdkPluginMgr::init, the repeated plugin id is ";
        for (std::vector<tss_sdk_if::ETssSdkPluginId>::const_iterator it =
                 repeated_plugin_id_table_.begin();
             it != repeated_plugin_id_table_.end(); ++it) {
            stream << static_cast<int>(*it) << ".";
        }
        std::string message = stream.str();
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b6;
        return m_plugin_mgr_last_error_code;
    }

    if (registered_plugin_count_ == 0) {
        std::string message(
            "In CTssSdkPluginMgr::init, no plugin registered.");
        error_info_.set_last_error_msg(message.c_str());
        m_plugin_mgr_last_error_code = 0x000494b4;
        return m_plugin_mgr_last_error_code;
    }

    return 0;
}

}  // namespace tss_sdk_impl
