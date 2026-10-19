// Independent dlopen(RTLD_NOW) probe for the reconstructed libtss_sdk.
//
// NOT built by build_server.py (test_ prefix).  It must be run only against
// the newly built library, with no ORIG libtss_sdk loaded:
//
//   g++ -m32 -std=gnu++98 -O2 -fPIC -o /tmp/test_tss_get_sdk_smoke \
//       recon/src/closing/test_tss_get_sdk_smoke.cpp -ldl
//   /tmp/test_tss_get_sdk_smoke build-server/libtss_sdk.so.1.0.394
//
// What it proves (ABI, not just non-null):
//   * RTLD_NOW resolves every relocation without ORIG (no lazy fallback).
//   * tss_get_sdk returns the 28-byte table; the same pointer on every call.
//   * Each of the seven table slots equals dlsym() of the exact ORIG mangled
//     symbol (so the field order/types match the real TssSdkInterface).
//   * The two side-effect-free accessors are callable with the real ABI:
//     get_last_error_code() returns an int, get_last_error() returns a
//     NUL-terminated const char* (never dereferenced beyond 4096 bytes).
//   * Every other symbol restored by this task resolves by its exact ORIG
//     name (data tables, get_host_addr, three vtables, two typeinfo symbols).
//
// It does NOT call init/init_plugins/fini/proc/notify on the framework table
// and does NOT call tss_sdk_release_busi_interf: those are the side-effecting
// operations the real server startup owns.
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef int (*TssSdkPluginCallbackProc)(void *arg);

struct TssSdkPluginInitData {
    ETssSdkPluginId plugin_id_;
    TssSdkPluginCallbackProc callback_proc_;
    void *init_data_;
};

struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;
    void *notify_data_;
};

}  // namespace tss_sdk_if

struct TssSdkInterface {
    int (*init)(unsigned int);
    int (*init_plugins)(const tss_sdk_if::TssSdkPluginInitData *,
                        unsigned int);
    int (*fini)(tss_sdk_if::ETssSdkPluginId);
    int (*proc)();
    int (*notify)(tss_sdk_if::TssSdkPluginNotifyData);
    int (*get_last_error_code)();
    const char *(*get_last_error)();
};

typedef TssSdkInterface *(*TssGetSdkFn)();
typedef int (*TssReleaseBusiInterfFn)();

static int failures = 0;

static void fail(const char *what) {
    printf("SMOKE_FAIL %s\n", what);
    ++failures;
}

static int same_file(const char *a, const char *b) {
    char ra[PATH_MAX];
    char rb[PATH_MAX];
    if (!a || !b || !realpath(a, ra) || !realpath(b, rb)) {
        return 0;
    }
    return strcmp(ra, rb) == 0;
}

static void check_symbol(void *handle, const char *name, const char *lib_path) {
    void *p = dlsym(handle, name);
    if (!p) {
        printf("SMOKE_FAIL missing %s\n", name);
        ++failures;
        return;
    }
    Dl_info info;
    memset(&info, 0, sizeof(info));
    if (!dladdr(p, &info) || !same_file(info.dli_fname, lib_path)) {
        printf("SMOKE_FAIL %s not owned by probed library (%s)\n", name,
               info.dli_fname ? info.dli_fname : "?");
        ++failures;
    }
}

static void check_table_slot(void *handle, const char *name, void *slot,
                             const char *lib_path, int index) {
    if (!slot) {
        printf("SMOKE_FAIL table slot %d (%s) null\n", index, name);
        ++failures;
        return;
    }
    void *resolved = dlsym(handle, name);
    if (!resolved) {
        printf("SMOKE_FAIL slot %d dlsym %s failed\n", index, name);
        ++failures;
        return;
    }
    if (resolved != slot) {
        printf("SMOKE_FAIL slot %d %s ptr=%p dlsym=%p\n", index, name, slot,
               resolved);
        ++failures;
        return;
    }
    Dl_info info;
    memset(&info, 0, sizeof(info));
    if (!dladdr(slot, &info) || !same_file(info.dli_fname, lib_path)) {
        printf("SMOKE_FAIL slot %d %s owner %s\n", index, name,
               info.dli_fname ? info.dli_fname : "?");
        ++failures;
    }
    printf("SMOKE_SLOT %d %s ok\n", index, name);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <libtss_sdk.so.1.0.394>\n", argv[0]);
        return 2;
    }
    const char *lib_path = argv[1];

    void *handle = dlopen(lib_path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        printf("SMOKE_FAIL dlopen: %s\n", dlerror());
        return 1;
    }

    void *get_sym = dlsym(handle, "tss_get_sdk");
    void *rel_sym = dlsym(handle, "tss_sdk_release_busi_interf");
    if (!get_sym) {
        fail("dlsym tss_get_sdk");
    }
    if (!rel_sym) {
        fail("dlsym tss_sdk_release_busi_interf");
    }

    if (!failures) {
        TssGetSdkFn get_sdk = (TssGetSdkFn)get_sym;
        TssSdkInterface *table = get_sdk();
        if (!table) {
            fail("tss_get_sdk returned null");
        } else if (table != get_sdk()) {
            fail("tss_get_sdk is not a stable singleton");
        } else {
            Dl_info info;
            memset(&info, 0, sizeof(info));
            if (!dladdr(table, &info) || !same_file(info.dli_fname, lib_path)) {
                fail("tss_get_sdk table not in probed library");
            }
            printf("SMOKE_TABLE %p\n", (void *)table);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework4initEj",
                (void *)table->init, lib_path, 0);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework12init_pluginsE"
                "PKN10tss_sdk_if20TssSdkPluginInitDataEj",
                (void *)table->init_plugins, lib_path, 1);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework4finiE"
                "N10tss_sdk_if15ETssSdkPluginIdE",
                (void *)table->fini, lib_path, 2);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework4procEv",
                (void *)table->proc, lib_path, 3);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework6notifyE"
                "N10tss_sdk_if22TssSdkPluginNotifyDataE",
                (void *)table->notify, lib_path, 4);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework19get_last_error_codeEv",
                (void *)table->get_last_error_code, lib_path, 5);
            check_table_slot(handle,
                "_ZN12tss_sdk_impl15TssSdkFramework14get_last_errorEv",
                (void *)table->get_last_error, lib_path, 6);

            // Side-effect-free queries only.
            int err_code = table->get_last_error_code();
            const char *err_msg = table->get_last_error();
            if (!err_msg) {
                fail("get_last_error returned null");
            } else if (memchr(err_msg, 0, 4096) == 0) {
                fail("get_last_error result is not NUL-terminated in 4096B");
            } else {
                printf("SMOKE_QUERY error_code=%d error_msg=\"%s\"\n",
                       err_code, err_msg);
            }
        }
    }

    check_symbol(handle, "dea_pc1", lib_path);
    check_symbol(handle, "s_and_p", lib_path);
    check_symbol(handle, "dea_ls", lib_path);
    check_symbol(handle, "dea_b", lib_path);
    check_symbol(handle, "dea_er", lib_path);
    check_symbol(handle, "dea_ksp", lib_path);
    check_symbol(handle, "dea_obp", lib_path);
    check_symbol(handle, "dea_oc", lib_path);
    check_symbol(handle, "dea_od", lib_path);
    check_symbol(handle, "dea_ol", lib_path);
    check_symbol(handle, "dea_or", lib_path);
    check_symbol(handle, "dea_r0", lib_path);
    check_symbol(handle, "_ZNK15ZEN_Sockaddr_In13get_host_addrEv", lib_path);
    check_symbol(handle, "_ZTVN9tss_dpsdk9IDPHackerE", lib_path);
    check_symbol(handle, "_ZTV7CVMBase", lib_path);
    check_symbol(handle, "_ZTV17IAntibotInterface", lib_path);
    check_symbol(handle, "_ZTI17IAntibotInterface", lib_path);
    check_symbol(handle, "_ZTS17IAntibotInterface", lib_path);

    dlclose(handle);
    if (failures) {
        printf("SMOKE_RESULT FAIL failures=%d\n", failures);
        return 1;
    }
    printf("SMOKE_RESULT PASS slots=7 tables=12 vtable=3\n");
    return 0;
}
