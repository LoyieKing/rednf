// Closing exports: the two SDK entry points that the reconstructed trees had
// only declared.
//
// Target: /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
// (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
//   tss_get_sdk                   ORIG 0x00091700, 0x16 bytes, T, C linkage
//   tss_sdk_release_busi_interf   ORIG 0x0008afa0, 0x28 bytes, T, C linkage
//
// Owned CUs (DWARF producer paths):
//   src/formmog/tss_sdk/framework/tss_sdk_framework.cpp:833
//   src/formmog/tss_sdk/busi_interf/busi_impl/tss_sdk_busi_impl.cpp:85
//
// tss_get_sdk body (ORIG 0x91700..0x91715):
//   lea eax,[tss_get_sdk::tss_sdk]   ; local static at 0x00297944, 28 bytes
//   ret
// The 28-byte table is statically initialised with seven function pointers;
// ORIG .rel.data shows R_386_32 relocations to exactly:
//   +0x00 TssSdkFramework::init(unsigned int)
//   +0x04 TssSdkFramework::init_plugins(TssSdkPluginInitData const*, unsigned int)
//   +0x08 TssSdkFramework::fini(ETssSdkPluginId)
//   +0x0c TssSdkFramework::proc()
//   +0x10 TssSdkFramework::notify(TssSdkPluginNotifyData)
//   +0x14 TssSdkFramework::get_last_error_code()
//   +0x18 TssSdkFramework::get_last_error()
// DWARF (tss_sdk_framework.cpp:833) gives the return type as a pointer to the
// anonymous 28-byte structure; the members are named in the struct DIE as
// init, init_plugins, fini, proc, notify, get_last_error_code, get_last_error.
//
// tss_sdk_release_busi_interf body (ORIG 0x8afa0..0x8afc7):
//   mov DWORD PTR [esp],0xffffffff      ; (ETssSdkPluginId)-1 =
//                                        ETSS_SDK_PLUGIN_ID_ALL
//   mov eax,[g_tss_sdk]                 ; local static at 0x0029de40,
//                                        initialised by the TU's static
//                                        initialiser at 0x8b4a3:
//                                        g_tss_sdk = tss_get_sdk();
//   call DWORD PTR [eax+0x8]            ; table field 2 = fini(plugin_id)
//   ret                                 ; the callee's int result is returned
//
// g_tss_sdk is a file-scope static (ORIG symbol: local b g_tss_sdk), so the
// plain unmangled name is kept here.  The local table symbol follows ORIG:
// _ZZ11tss_get_sdkE7tss_sdk.

#include <stddef.h>

namespace tss_sdk_if {

// DWARF: tss_sdk_if.h:18, 4-byte enum.
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

typedef int (*TssSdkPluginCallbackProc)(void *arg);

// tss_sdk_plugin.h:44, sizeof 12.
struct TssSdkPluginInitData {
    ETssSdkPluginId plugin_id_;               // +0
    TssSdkPluginCallbackProc callback_proc_;  // +4
    void *init_data_;                         // +8
};

// tss_sdk_plugin.h:49, sizeof 8.
struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;  // +0
    void *notify_data_;          // +4
};

}  // namespace tss_sdk_if

namespace tss_sdk_impl {

// tss_sdk_framework.h:21..28.  Only the seven members stored in the public
// table are declared here; their definitions remain in the reconstructed
// framework TUs (tss_sdk_framework_lifecycle_agent.cpp / _dispatch_agent.cpp).
// All seven are static members (the DWARF subprogram DIEs have no artificial
// `this` parameter, and the ORIG table stores plain function pointers).
class TssSdkFramework {
public:
    static int init(unsigned int instance_id);
    static int init_plugins(
        const tss_sdk_if::TssSdkPluginInitData *plugin_init_data,
        unsigned int plugin_count);
    static int fini(tss_sdk_if::ETssSdkPluginId plugin_id);
    static int proc();
    static int notify(tss_sdk_if::TssSdkPluginNotifyData plugin_notify_data);
    static int get_last_error_code();
    static const char *get_last_error();
};

}  // namespace tss_sdk_impl

// tss_sdk_if.h (anonymous structure, sizeof 28); the typedef names below are
// the DWARF names of the function-pointer types (lines 61..77).  Field order
// is the DWARF member order and the ORIG relocation order, not inferred from
// the disassembly alone.
typedef int (*TssSdkInit)(unsigned int instance_id);
typedef int (*TssSdkInitPlugins)(
    const tss_sdk_if::TssSdkPluginInitData *plugin_init_data,
    unsigned int plugin_count);
typedef int (*TssSdkFini)(tss_sdk_if::ETssSdkPluginId plugin_id);
typedef int (*TssSdkProc)();
typedef int (*TssSdkNotify)(
    tss_sdk_if::TssSdkPluginNotifyData plugin_notify_data);
typedef int (*TssSdkGetLastErrorCode)();
typedef const char *(*TssSdkGetLastError)();

struct TssSdkInterface {
    TssSdkInit init;                                      // +0x00
    TssSdkInitPlugins init_plugins;                       // +0x04
    TssSdkFini fini;                                      // +0x08
    TssSdkProc proc;                                      // +0x0c
    TssSdkNotify notify;                                  // +0x10
    TssSdkGetLastErrorCode get_last_error_code;           // +0x14
    TssSdkGetLastError get_last_error;                    // +0x18
};

typedef char exports_surface_size_check_interface[
    sizeof(TssSdkInterface) == 28 ? 1 : -1];
typedef char exports_surface_size_check_init_data[
    sizeof(tss_sdk_if::TssSdkPluginInitData) == 12 ? 1 : -1];
typedef char exports_surface_size_check_notify_data[
    sizeof(tss_sdk_if::TssSdkPluginNotifyData) == 8 ? 1 : -1];
typedef char exports_surface_offset_check_fini[
    __builtin_offsetof(TssSdkInterface, fini) == 8 ? 1 : -1];

// ORIG 0x00091700.  The local static is constant-initialised (addresses only),
// so GCC emits it in .data with relocations and no guard variable, matching
// ORIG's 28-byte d object at 0x00297944.
extern "C" TssSdkInterface *tss_get_sdk() {
    static TssSdkInterface tss_sdk = {
        &tss_sdk_impl::TssSdkFramework::init,
        &tss_sdk_impl::TssSdkFramework::init_plugins,
        &tss_sdk_impl::TssSdkFramework::fini,
        &tss_sdk_impl::TssSdkFramework::proc,
        &tss_sdk_impl::TssSdkFramework::notify,
        &tss_sdk_impl::TssSdkFramework::get_last_error_code,
        &tss_sdk_impl::TssSdkFramework::get_last_error
    };
    return &tss_sdk;
}

// ORIG 0x0029de40 (local b, 4 bytes); writer is the TU static initialiser at
// 0x8b4a3.
static TssSdkInterface *g_tss_sdk = tss_get_sdk();

// ORIG 0x0008afa0.  The argument is ETSS_SDK_PLUGIN_ID_ALL (-1); the return
// value is TssSdkFramework::fini's int result.
extern "C" int tss_sdk_release_busi_interf() {
    return g_tss_sdk->fini(tss_sdk_if::ETSS_SDK_PLUGIN_ID_ALL);
}
