// Independent dlopen probe for the reconstructed AntibotInterface factory.
//
// What it proves (ABI/dispatch, not merely "non-null"):
//   * The rebuilt libtss_sdk .so loads with RTLD_NOW and its static
//     initializer constructs g_antibot_if without ORIG present.
//   * GetAntiBotInterfaceAll() returns an object whose vptr is the address
//     point of _ZTV16AntibotInterface (vtable symbol + 8).
//   * Factory slot 0 is exactly dlsym() of ORIG's Initialize target
//     _ZN16AntibotInterface10InitializeEP9IBusinessPc, and every one of the 18
//     business slots matches its exact ORIG mangled method symbol, in ORIG
//     order (relocations 0x00292ca8..0x00292cec):
//       Initialize, AddClient, DeleteClient, OnRecvBufferFromClient,
//       ProcessAntiBotInput, CheckTimer, SetLogPath, ShowInfo,
//       P2PAddClient, P2PDeleteClient, RecvBufferForEncrypt,
//       RecvBufferForDecrypt, IsCmdInUpList, IsCmdInDownList,
//       OnNotifyServer, AntiCheatPacket, QueryProtectPolicy, ForwardData.
//   * Slots 18/19 are ~AntibotInterface D1/D0 (ORIG 0x00292cf0/f4), and no
//     destructor occupies slot 0 or 1.  The old ctor-TU model emitted a
//     2-slot [D1, D0] table; the failing-before control below builds exactly
//     that table and asserts the detector rejects it.
//   * The factory object's real virtual slot 5 (CheckTimer, ORIG 0x885c0:
//     xor eax,eax; ret) is invoked and must return 0.  With the old 2-slot
//     dtor-only table this call cannot dispatch correctly; the probe fails
//     before the call when the old shape is detected.  The other five ORIG-
//     verified trivial slots are also invoked (P2PAddClient/P2PDeleteClient/
//     ForwardData return 0; SetLogPath/ShowInfo are empty) to cover more of
//     the real table.
//
// It deliberately does NOT call Initialize/AddClient/DeleteClient/notify/proc:
// Initialize creates framework runtime state, threads and config/log mappings,
// and the plugin paths are outside this local probe safe boundary.  Slot 0 is
// verified against the exact ORIG Initialize symbol instead; the executable
// dispatch test uses CheckTimer, whose ORIG body is side-effect free.
//
// Build/run (see build_and_run.sh):
//   g++ -m32 -std=gnu++98 -O2 -fno-stack-protector -D_GNU_SOURCE \
//       -o build/check_antibot_factory check_antibot_factory_probe.cpp -ldl
//   ./build/check_antibot_factory <rebuilt libtss_sdk.so.1.0.394>
//
// Exit: 0 = all checks pass; 1 = mismatch/regression; 2 = load/setup failure.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>

#define DEFAULT_LIBRARY \
    "/home/loyieking/dnf_workspace/dnf_secsvr/tss_sdk/build-server/libtss_sdk.so.1.0.394"

typedef void *(*GetAntiBotInterfaceAllFn)(void);

// i386 member-function ABI: `this` is the first stack argument.
typedef int (*Int3Fn)(void *, int, int, unsigned int);
typedef int (*Int1Fn)(void *, unsigned int);
typedef int (*P2PAddFn)(void *, unsigned int, int, unsigned int);
typedef int (*ForwardDataFn)(void *, int, int, unsigned int, unsigned int,
                             unsigned int, const unsigned char *);
typedef void (*SetLogPathFn)(void *, char *);
typedef void (*ShowInfoFn)(void *);

struct SlotSpec {
    const char *symbol;
    const char *name;
};

// ORIG vtable 0x00292ca0, address point 0x00292ca8.  Relocation order.
static const SlotSpec kBusinessSlots[18] = {
    {"_ZN16AntibotInterface10InitializeEP9IBusinessPc", "Initialize"},
    {"_ZN16AntibotInterface9AddClientEiijjP9TUserInfo", "AddClient"},
    {"_ZN16AntibotInterface12DeleteClientEiij", "DeleteClient"},
    {"_ZN16AntibotInterface22OnRecvBufferFromClientEiijPht",
     "OnRecvBufferFromClient"},
    {"_ZN16AntibotInterface19ProcessAntiBotInputEv", "ProcessAntiBotInput"},
    {"_ZN16AntibotInterface10CheckTimerEiij", "CheckTimer"},
    {"_ZN16AntibotInterface10SetLogPathEPc", "SetLogPath"},
    {"_ZN16AntibotInterface8ShowInfoEv", "ShowInfo"},
    {"_ZN16AntibotInterface12P2PAddClientEjij", "P2PAddClient"},
    {"_ZN16AntibotInterface15P2PDeleteClientEj", "P2PDeleteClient"},
    {"_ZN16AntibotInterface20RecvBufferForEncryptEjPhjS0_Rj",
     "RecvBufferForEncrypt"},
    {"_ZN16AntibotInterface20RecvBufferForDecryptEjPhtS0_Rt",
     "RecvBufferForDecrypt"},
    {"_ZN16AntibotInterface13IsCmdInUpListEji", "IsCmdInUpList"},
    {"_ZN16AntibotInterface15IsCmdInDownListEji", "IsCmdInDownList"},
    {"_ZN16AntibotInterface14OnNotifyServerEiijPhs", "OnNotifyServer"},
    {"_ZN16AntibotInterface15AntiCheatPacketEji", "AntiCheatPacket"},
    {"_ZN16AntibotInterface18QueryProtectPolicyEiijP12SSafeModePkg",
     "QueryProtectPolicy"},
    {"_ZN16AntibotInterface11ForwardDataEiijjjPKh", "ForwardData"},
};

static int g_failures = 0;

static void failf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "FAIL: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    ++g_failures;
}

static void notef(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

static void *resolve(void *handle, const char *symbol) {
    void *p;
    const char *err;

    dlerror();
    p = dlsym(handle, symbol);
    err = dlerror();
    if (err != 0) {
        failf("dlsym(%s): %s", symbol, err);
        return 0;
    }
    if (p == 0) {
        failf("dlsym(%s) returned null", symbol);
    }
    return p;
}

// Returns 1/2 when slot 0/1 is a destructor entry (the old 2-slot [D1, D0]
// partial-class table shape), otherwise 0.  Slot 0 must be Initialize for this
// ABI.
static int old_vtable_shape(void **vt, void *d1, void *d0) {
    if (vt[0] == d1 || vt[0] == d0) {
        return 1;
    }
    if (vt[1] == d1 || vt[1] == d0) {
        return 2;
    }
    return 0;
}

static int validate_derived_vtable(void **vt, void **expected, void *d1,
                                   void *d0) {
    int i;
    int local_failures = 0;
    int business_ok = 1;
    int old = old_vtable_shape(vt, d1, d0);

    if (old != 0) {
        failf("REGRESSION: destructor occupies business slot %d "
              "(old ctor-TU vtable shape)", old - 1);
        // Do not read further: the old table has only two entries.
        return 1;
    }

    if (expected[0] != 0 && vt[0] != expected[0]) {
        failf("factory slot 0 = %p, ORIG Initialize target = %p", vt[0],
              expected[0]);
        ++local_failures;
        business_ok = 0;
    }

    for (i = 0; i < 18; ++i) {
        if (expected[i] == 0) {
            failf("slot %d (%s): expected symbol was not resolved", i,
                  kBusinessSlots[i].name);
            ++local_failures;
            business_ok = 0;
        } else if (vt[i] != expected[i]) {
            failf("slot %d (%s) = %p, expected dlsym %p", i,
                  kBusinessSlots[i].name, vt[i], expected[i]);
            ++local_failures;
            business_ok = 0;
        }
    }

    if (!business_ok) {
        // The table is not the expected 18-slot prefix; reading past it could
        // touch an unmapped/adjacent object, so the D1/D0 check is skipped.
        notef("dtor slots 18/19 not read after business-slot mismatch");
        return local_failures;
    }

    if (vt[18] != d1) {
        failf("slot 18 = %p, expected ~AntibotInterface D1 %p", vt[18], d1);
        ++local_failures;
    }
    if (vt[19] != d0) {
        failf("slot 19 = %p, expected ~AntibotInterface D0 %p", vt[19], d0);
        ++local_failures;
    }
    return local_failures;
}

static void negative_control(void **expected, void *d1, void *d0) {
    // The old ctor TU emitted a 2-slot vtable: D1 at +0, D0 at +4.  The
    // detector must reject exactly that shape and must not false-positive on
    // the correct Initialize/AddClient prefix.
    void *bad_two_slot[2];
    void *good_two_slot[2];

    bad_two_slot[0] = d1;
    bad_two_slot[1] = d0;
    good_two_slot[0] = expected[0];
    good_two_slot[1] = expected[1];

    if (old_vtable_shape(bad_two_slot, d1, d0) == 0) {
        failf("negative control: old 2-slot [D1,D0] table was not detected");
    } else {
        notef("negative control: old 2-slot [D1,D0] ctor-TU table rejected");
    }
    if (old_vtable_shape(good_two_slot, d1, d0) != 0) {
        failf("negative control: correct Initialize/AddClient prefix was "
              "misclassified as old");
    }
}

int main(int argc, char **argv) {
    const char *library = (argc > 1) ? argv[1] : DEFAULT_LIBRARY;
    void *handle;
    GetAntiBotInterfaceAllFn factory;
    void *iface;
    void **vt;
    void *expected[18];
    void *vt_symbol;
    void *base_vt_symbol;
    void *ti_symbol;
    void *base_ti_symbol;
    void *d1;
    void *d0;
    void *pure = 0;
    int i;

    printf("library: %s\n", library);
    printf("boundary: Initialize not called; slot 0 checked against its symbol; "
           "CheckTimer(slot 5) invoked\n");
    handle = dlopen(library, RTLD_NOW | RTLD_LOCAL);
    if (handle == 0) {
        fprintf(stderr, "SETUP FAIL: dlopen(%s): %s\n", library, dlerror());
        return 2;
    }

    factory = (GetAntiBotInterfaceAllFn)dlsym(handle,
                                              "GetAntiBotInterfaceAll");
    if (factory == 0) {
        fprintf(stderr,
                "SETUP FAIL: dlsym(GetAntiBotInterfaceAll): %s\n", dlerror());
        dlclose(handle);
        return 2;
    }

    iface = factory();
    if (iface == 0) {
        failf("GetAntiBotInterfaceAll returned null");
        dlclose(handle);
        return 1;
    }

    vt = *(void ***)iface;

    vt_symbol = resolve(handle, "_ZTV16AntibotInterface");
    base_vt_symbol = resolve(handle, "_ZTV17IAntibotInterface");
    ti_symbol = resolve(handle, "_ZTI16AntibotInterface");
    base_ti_symbol = resolve(handle, "_ZTI17IAntibotInterface");

    if (vt_symbol != 0) {
        if ((char *)vt != (char *)vt_symbol + 8) {
            failf("object vptr %p != _ZTV16AntibotInterface %p + 8", vt,
                  vt_symbol);
        } else {
            notef("object vptr = _ZTV16AntibotInterface + 8 (%p)", vt);
            if (ti_symbol != 0 &&
                ((void **)((char *)vt - 4))[0] != ti_symbol) {
                failf("derived vtable typeinfo %p, expected %p",
                      ((void **)((char *)vt - 4))[0], ti_symbol);
            }
        }
    }

    for (i = 0; i < 18; ++i) {
        expected[i] = resolve(handle, kBusinessSlots[i].symbol);
    }

    d1 = resolve(handle, "_ZN16AntibotInterfaceD1Ev");
    d0 = resolve(handle, "_ZN16AntibotInterfaceD0Ev");

    if (base_vt_symbol != 0) {
        void **base_slots = (void **)((char *)base_vt_symbol + 8);
        if (base_ti_symbol != 0 && base_slots[-1] != base_ti_symbol) {
            failf("base vtable typeinfo %p, expected %p", base_slots[-1],
                  base_ti_symbol);
        }
        pure = dlsym(RTLD_DEFAULT, "__cxa_pure_virtual");
        if (pure == 0) {
            void *cxx = dlopen("libstdc++.so.6", RTLD_NOW | RTLD_NOLOAD);
            if (cxx != 0) {
                pure = dlsym(cxx, "__cxa_pure_virtual");
            }
        }
        if (pure != 0) {
            for (i = 0; i < 18; ++i) {
                if (base_slots[i] != pure) {
                    failf("base vtable slot %d = %p, expected "
                          "__cxa_pure_virtual %p", i, base_slots[i], pure);
                }
            }
            notef("base vtable: 18/18 __cxa_pure_virtual slots");
        } else {
            void *first = base_slots[0];
            for (i = 1; i < 18; ++i) {
                if (base_slots[i] != first) {
                    failf("base vtable slot %d differs from slot 0 "
                          "(__cxa_pure_virtual unresolved)", i);
                }
            }
            notef("base vtable: 18 identical pure slots "
                  "(__cxa_pure_virtual unresolved)");
        }
    }

    if (d1 != 0 && d0 != 0) {
        negative_control(expected, d1, d0);
        if (validate_derived_vtable(vt, expected, d1, d0) == 0) {
            // Real dispatch test: ORIG CheckTimer (0x885c0) returns 0 and has
            // no side effects.  This is slot 5 of the factory object's real
            // vtable.  The old 2-slot dtor-only table fails validation above
            // before this call.
            {
                Int3Fn check_timer = (Int3Fn)vt[5];
                int rc = check_timer(iface, 7, 9, 0x11223344u);
                if (rc != 0) {
                    failf("CheckTimer returned %d, ORIG returns 0", rc);
                } else {
                    notef("CheckTimer(slot 5) returned 0");
                }
            }
            // Additional ORIG-verified trivial slots, all side-effect free.
            {
                Int1Fn p2p_delete = (Int1Fn)vt[9];
                int rc = p2p_delete(iface, 0x55667788u);
                if (rc != 0) {
                    failf("P2PDeleteClient returned %d, ORIG returns 0", rc);
                }
            }
            {
                P2PAddFn p2p_add = (P2PAddFn)vt[8];
                int rc = p2p_add(iface, 0x99aabbccu, -5, 3);
                if (rc != 0) {
                    failf("P2PAddClient returned %d, ORIG returns 0", rc);
                }
            }
            {
                ForwardDataFn forward = (ForwardDataFn)vt[17];
                int rc = forward(iface, 1, 2, 3, 4, 5, 0);
                if (rc != 0) {
                    failf("ForwardData returned %d, ORIG returns 0", rc);
                }
            }
            {
                SetLogPathFn set_log_path = (SetLogPathFn)vt[6];
                ShowInfoFn show_info = (ShowInfoFn)vt[7];
                set_log_path(iface, (char *)"antibot-factory-probe");
                show_info(iface);
            }
        }
    }

    dlclose(handle);

    if (g_failures != 0) {
        fprintf(stderr, "RESULT: FAIL (%d failures)\n", g_failures);
        return 1;
    }
    printf("RESULT: PASS\n");
    return 0;
}
