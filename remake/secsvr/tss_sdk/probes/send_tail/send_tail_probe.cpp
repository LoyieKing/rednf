// Isolated differential probe for the actual send tail of
// tss_dpsdk::DPMsgHandler::SendDpMsgToClient(UserInfo*, unsigned char*,
// unsigned short) @0x000ae3b0.
//
// What is exercised
// -----------------
//   * ORIG's real SendDpMsgToClient, exported by
//     reference/libtss_sdk.so.1.0.394.orig.
//   * A rebuilt shared object from
//     recon/.../dp_msg_handler_send_agent.cpp (current "after" source and a
//     mutated copy used as a negative control).
//   * The external transport/crypto boundary is replaced by probe hooks:
//       - DPVMBaseEngine::encrypt() returns a deterministic payload and
//         records the input length / initial output capacity,
//       - DPMsgPacker::send_dp_msg_to_client() records the exact buffer and
//         length handed to the transport and returns the configured code,
//       - DPBillAdapter::dump() records the optional bill buffer.
//     This is the same boundary as the ORIG call sites: the probe does not
//     stub the length prefix, the DPInterDataHead pack, the encrypt-call
//     arguments, the send return propagation, or the user counter update.
//
// Cases
// -----
//   encrypt success with payload 0, 1, 57 and 0xfff6 bytes;
//   encrypt failure;
//   transport return 0 and a non-zero error code.
//   Every case compares the returned int, the entire 400-byte UserInfo, the
//   encrypt input length/initial capacity, and the exact bytes/length passed
//   to DPMsgPacker::send_dp_msg_to_client.
//
// Build and usage: see build_and_run.sh in this directory.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "check_timer_probe_types.h"

#include <dlfcn.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define ORIG_DEFAULT \
    "/home/loyieking/dnf_workspace/dnf_secsvr/tss_sdk/reference/libtss_sdk.so.1.0.394.orig"
#define SEND_SYM \
    "_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoEPht"

// ---------------------------------------------------------------------------
// Probe-side boundary hooks.  The mangled names are the ORIG symbols called
// by SendDpMsgToClient; defining them here preempts the ORIG transports.
// ---------------------------------------------------------------------------
enum ZEN_LOG_PRIORITY {
    RS_TRACE = 1,
    RS_DEBUG = 2,
    RS_INFO = 3,
    RS_ERROR = 4,
    RS_ALERT = 5,
    RS_FATAL = 6
};

class ZEN_Trace_LogMsg {
public:
    static void debug_output(ZEN_LOG_PRIORITY priority, const char *format,
                             ...);
};

namespace tss_dpsdk {

class DPVMBaseEngine {
public:
    int encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                unsigned int buf_len, unsigned char *out,
                unsigned int &out_len, bool is_internal_pkg);
};

class DPMsgPacker {
public:
    static int send_dp_msg_to_client(tss_sdk_impl::UserInfo *user,
                                     unsigned char *data,
                                     unsigned short len);
};

class DPBillAdapter {
public:
    static int dump(const char *msg, unsigned int uin,
                    const unsigned char *buf, unsigned int len);
};

}  // namespace tss_dpsdk

struct HookState {
    int encrypt_ret;
    unsigned int encrypt_len;
    int encrypt_calls;
    unsigned int encrypt_in_len;
    unsigned int encrypt_capacity;

    int send_ret;
    int send_calls;
    unsigned int send_len;
    int send_overflow;
    unsigned char send_buf[70000];

    int dump_calls;
    char dump_msg[128];
    unsigned int dump_uin;
    unsigned int dump_len;
    unsigned char dump_buf[70000];
};

static HookState *g_hook;
static const char *g_side = "?";

void ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, const char *, ...) {}

int tss_dpsdk::DPVMBaseEngine::encrypt(tss_sdk_impl::UserInfo *, unsigned char *,
                                       unsigned int buf_len, unsigned char *out,
                                       unsigned int &out_len, bool) {
    if (g_hook == 0) {
        fprintf(stderr, "FATAL: encrypt hook outside a probe run (side=%s)\n",
                g_side);
        return -1;
    }
    ++g_hook->encrypt_calls;
    g_hook->encrypt_in_len = buf_len;
    g_hook->encrypt_capacity = out_len;
    if (g_hook->encrypt_ret != 0)
        return g_hook->encrypt_ret;

    const unsigned int n = g_hook->encrypt_len;
    for (unsigned int i = 0; i < n; ++i)
        out[i] = (unsigned char)(0xa5u ^ (i * 31u));
    out_len = n;
    return 0;
}

int tss_dpsdk::DPMsgPacker::send_dp_msg_to_client(
        tss_sdk_impl::UserInfo *, unsigned char *data, unsigned short len) {
    if (g_hook == 0) {
        fprintf(stderr,
                "FATAL: send_dp_msg_to_client hook outside a probe run "
                "(side=%s)\n", g_side);
        return -1;
    }
    ++g_hook->send_calls;
    g_hook->send_len = len;
    if (len > sizeof(g_hook->send_buf)) {
        g_hook->send_overflow = 1;
    } else if (len != 0) {
        memcpy(g_hook->send_buf, data, len);
    }
    return g_hook->send_ret;
}

int tss_dpsdk::DPBillAdapter::dump(const char *msg, unsigned int uin,
                                  const unsigned char *buf,
                                  unsigned int len) {
    if (g_hook == 0) {
        fprintf(stderr,
                "FATAL: DPBillAdapter::dump hook outside a probe run "
                "(side=%s)\n", g_side);
        return -1;
    }
    ++g_hook->dump_calls;
    if (msg != 0) {
        strncpy(g_hook->dump_msg, msg, sizeof(g_hook->dump_msg) - 1);
        g_hook->dump_msg[sizeof(g_hook->dump_msg) - 1] = 0;
    }
    g_hook->dump_uin = uin;
    g_hook->dump_len = len;
    if (len <= sizeof(g_hook->dump_buf) && len != 0)
        memcpy(g_hook->dump_buf, buf, len);
    return 0;
}

// ---------------------------------------------------------------------------
// Driver
// ---------------------------------------------------------------------------
// i386 member-function ABI: `this` is the first stack argument.
typedef int (*SendFn)(tss_dpsdk::DPMsgHandler *self,
                      tss_sdk_impl::UserInfo *user, unsigned char *buf,
                      unsigned short len);

struct Case {
    const char *label;
    unsigned short input_len;
    unsigned int encrypt_len;
    int encrypt_ret;
    int send_ret;
    unsigned int flags;
};

struct Result {
    int ret;
    unsigned char user[sizeof(tss_sdk_impl::UserInfo)];
    HookState hook;
};

static tss_dpsdk::DPMsgHandler g_self;
static unsigned char g_input[65536];

static void run_case(const char *side, SendFn fn, const Case &c,
                     Result &out) {
    memset(&out, 0, sizeof(out));
    memset(&out.hook, 0, sizeof(out.hook));
    out.hook.encrypt_len = c.encrypt_len;
    out.hook.encrypt_ret = c.encrypt_ret;
    out.hook.send_ret = c.send_ret;

    tss_sdk_impl::UserInfo user;
    memset(&user, 0, sizeof(user));
    user.base_info_.uin_ = 0x12345678u;
    user.base_info_.game_usr_id_ = 0xffffffffu;  // fall back to uin_
    user.base_info_.flags_ = c.flags;

    for (unsigned int i = 0; i < c.input_len; ++i)
        g_input[i] = (unsigned char)(i * 17u + 9u);

    g_hook = &out.hook;
    g_side = side;
    out.ret = fn(&g_self, &user, g_input, c.input_len);
    g_hook = 0;
    memcpy(out.user, &user, sizeof(user));
}

static int g_checks;
static int g_failures;

static void failf(const char *fmt, ...) {
    va_list ap;
    ++g_failures;
    fprintf(stderr, "FAIL ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

static int same_bytes(const unsigned char *a, const unsigned char *b,
                      unsigned int n) {
    return memcmp(a, b, n) == 0;
}

static int compare(const char *side, const char *label, const Result &orig,
                   const Result &got, int require_match) {
    const int before = g_failures;

    ++g_checks;
    if (orig.ret != got.ret)
        failf("%s/%s ret %d/%d", side, label, orig.ret, got.ret);
    if (!same_bytes(orig.user, got.user, sizeof(orig.user)))
        failf("%s/%s UserInfo bytes differ", side, label);

    ++g_checks;
    if (orig.hook.encrypt_calls != got.hook.encrypt_calls)
        failf("%s/%s encrypt_calls %d/%d", side, label,
              orig.hook.encrypt_calls, got.hook.encrypt_calls);
    if (orig.hook.encrypt_in_len != got.hook.encrypt_in_len)
        failf("%s/%s encrypt_in_len %u/%u", side, label,
              orig.hook.encrypt_in_len, got.hook.encrypt_in_len);
    if (orig.hook.encrypt_capacity != got.hook.encrypt_capacity)
        failf("%s/%s encrypt_capacity %u/%u", side, label,
              orig.hook.encrypt_capacity, got.hook.encrypt_capacity);

    ++g_checks;
    if (orig.hook.send_calls != got.hook.send_calls)
        failf("%s/%s send_calls %d/%d", side, label, orig.hook.send_calls,
              got.hook.send_calls);
    if (orig.hook.send_len != got.hook.send_len)
        failf("%s/%s send_len %u/%u", side, label, orig.hook.send_len,
              got.hook.send_len);
    if (orig.hook.send_overflow != got.hook.send_overflow)
        failf("%s/%s send_overflow %d/%d", side, label,
              orig.hook.send_overflow, got.hook.send_overflow);
    if (!same_bytes(orig.hook.send_buf, got.hook.send_buf,
                    orig.hook.send_len < got.hook.send_len
                        ? orig.hook.send_len
                        : got.hook.send_len) &&
        orig.hook.send_len == got.hook.send_len) {
        failf("%s/%s send buffer differs", side, label);
    }

    ++g_checks;
    if (orig.hook.dump_calls != got.hook.dump_calls)
        failf("%s/%s dump_calls %d/%d", side, label, orig.hook.dump_calls,
              got.hook.dump_calls);
    if (orig.hook.dump_calls == got.hook.dump_calls &&
        orig.hook.dump_calls != 0) {
        if (strcmp(orig.hook.dump_msg, got.hook.dump_msg) != 0)
            failf("%s/%s dump msg '%s'/'%s'", side, label,
                  orig.hook.dump_msg, got.hook.dump_msg);
        if (orig.hook.dump_uin != got.hook.dump_uin)
            failf("%s/%s dump uin %u/%u", side, label, orig.hook.dump_uin,
                  got.hook.dump_uin);
        if (orig.hook.dump_len != got.hook.dump_len)
            failf("%s/%s dump len %u/%u", side, label, orig.hook.dump_len,
                  got.hook.dump_len);
        if (!same_bytes(orig.hook.dump_buf, got.hook.dump_buf,
                        orig.hook.dump_len < got.hook.dump_len
                            ? orig.hook.dump_len
                            : got.hook.dump_len) &&
            orig.hook.dump_len == got.hook.dump_len) {
            failf("%s/%s dump buffer differs", side, label);
        }
    }

    if (require_match)
        return g_failures == before;
    return g_failures != before;
}

// Pure comparison for the negative control: no diagnostics, no counters.
static int result_differs(const Result &a, const Result &b) {
    if (a.ret != b.ret)
        return 1;
    if (memcmp(a.user, b.user, sizeof(a.user)) != 0)
        return 1;
    if (a.hook.encrypt_calls != b.hook.encrypt_calls ||
        a.hook.encrypt_in_len != b.hook.encrypt_in_len ||
        a.hook.encrypt_capacity != b.hook.encrypt_capacity)
        return 1;
    if (a.hook.send_calls != b.hook.send_calls ||
        a.hook.send_len != b.hook.send_len ||
        a.hook.send_overflow != b.hook.send_overflow)
        return 1;
    if (a.hook.send_len != 0 &&
        memcmp(a.hook.send_buf, b.hook.send_buf, a.hook.send_len) != 0)
        return 1;
    if (a.hook.dump_calls != b.hook.dump_calls ||
        a.hook.dump_uin != b.hook.dump_uin ||
        a.hook.dump_len != b.hook.dump_len ||
        strcmp(a.hook.dump_msg, b.hook.dump_msg) != 0)
        return 1;
    if (a.hook.dump_len != 0 &&
        memcmp(a.hook.dump_buf, b.hook.dump_buf, a.hook.dump_len) != 0)
        return 1;
    return 0;
 }

static void *must_dlopen(const char *path, int flags, const char *role) {
    void *h = dlopen(path, flags);
    if (h == 0) {
        fprintf(stderr, "FATAL: dlopen(%s, %s) failed: %s\n", role, path,
                dlerror());
        exit(2);
    }
    return h;
}

static void *must_dlsym(void *h, const char *symbol, const char *what) {
    void *p = dlsym(h, symbol);
    if (p == 0) {
        fprintf(stderr, "FATAL: dlsym(%s) failed: %s\n", what, dlerror());
        exit(2);
    }
    return p;
}

static int same_realpath(const char *a, const char *b) {
    char ra[PATH_MAX];
    char rb[PATH_MAX];
    const char *pa = realpath(a, ra);
    const char *pb = realpath(b, rb);
    if (pa == 0 || pb == 0)
        return 0;
    return strcmp(pa, pb) == 0;
}

int main(int argc, char **argv) {
    const char *orig_path = ORIG_DEFAULT;
    const char *after_path = 0;
    const char *mutant_path = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--orig") == 0 && i + 1 < argc) {
            orig_path = argv[++i];
        } else if (strcmp(argv[i], "--after") == 0 && i + 1 < argc) {
            after_path = argv[++i];
        } else if (strcmp(argv[i], "--mutant") == 0 && i + 1 < argc) {
            mutant_path = argv[++i];
        } else {
            fprintf(stderr,
                    "usage: %s --orig ORIG --after AFTER [--mutant MUTANT]\n",
                    argv[0]);
            return 2;
        }
    }
    if (after_path == 0) {
        fprintf(stderr, "FATAL: --after is required\n");
        return 2;
    }

    void *h_orig = must_dlopen(orig_path, RTLD_NOW | RTLD_GLOBAL, "ORIG");
    void *h_after = must_dlopen(after_path, RTLD_NOW | RTLD_LOCAL, "after");
    void *h_mutant = 0;
    if (mutant_path != 0)
        h_mutant = must_dlopen(mutant_path, RTLD_NOW | RTLD_LOCAL, "mutant");

    SendFn orig_fn = (SendFn)must_dlsym(h_orig, SEND_SYM, "ORIG SendDpMsgToClient");
    SendFn after_fn = (SendFn)must_dlsym(h_after, SEND_SYM, "after SendDpMsgToClient");
    SendFn mutant_fn = 0;
    if (h_mutant != 0)
        mutant_fn = (SendFn)must_dlsym(h_mutant, SEND_SYM,
                                       "mutant SendDpMsgToClient");

    if (same_realpath(orig_path, after_path)) {
        fprintf(stderr, "FATAL: after resolves to ORIG\n");
        return 2;
    }

    g_self.vm_engine_ = (void *)0x1;  // ignored by the probe encrypt hook

    static const Case cases[] = {
        {"encrypt_len_0",          0,      0,      0, 0, 0},
        {"encrypt_len_1",          1,      1,      0, 0, 0},
        {"encrypt_len_57",         57,     57,     0, 0, 0},
        {"encrypt_len_max",        0xffff, 0xfff6, 0, 0, 0},
        {"encrypt_fail",           57,     57,     1, 0, 0},
        {"send_fail",              57,     57,     0, -2, 0},
        {"flags_bill_dump",        57,     57,     0, 0, 1},
    };
    const int case_count = (int)(sizeof(cases) / sizeof(cases[0]));

    printf("ORIG SendDpMsgToClient : %p\n", (void *)orig_fn);
    printf("after SendDpMsgToClient: %p\n", (void *)after_fn);
    if (mutant_fn != 0)
        printf("mutant SendDpMsgToClient: %p\n", (void *)mutant_fn);

    for (int i = 0; i < case_count; ++i) {
        Result a;
        Result b;
        run_case("ORIG", orig_fn, cases[i], a);
        run_case("after", after_fn, cases[i], b);

        // A hook that never fired means the call bound to the library's own
        // implementation rather than the probe boundary; fail loudly instead
        // of comparing an unexercised path.
        if (a.hook.encrypt_calls != 1 || b.hook.encrypt_calls != 1)
            failf("%s boundary encrypt_calls %d/%d", cases[i].label,
                  a.hook.encrypt_calls, b.hook.encrypt_calls);
        if (cases[i].encrypt_ret == 0 &&
            (a.hook.send_calls != 1 || b.hook.send_calls != 1))
            failf("%s boundary send_calls %d/%d", cases[i].label,
                  a.hook.send_calls, b.hook.send_calls);
        if (cases[i].encrypt_ret != 0 &&
            (a.hook.send_calls != 0 || b.hook.send_calls != 0))
            failf("%s failure-path send_calls %d/%d", cases[i].label,
                  a.hook.send_calls, b.hook.send_calls);

        compare("after", cases[i].label, a, b, true);

        if (mutant_fn != 0) {
            Result m;
            run_case("mutant", mutant_fn, cases[i], m);
            if (m.hook.encrypt_calls != 1)
                failf("%s mutant boundary encrypt_calls %d", cases[i].label,
                      m.hook.encrypt_calls);
        }
    }

    printf("checks=%d failures=%d\n", g_checks, g_failures);
    if (g_failures != 0)
        return 1;
    if (mutant_fn != 0) {
        // The negative control must disagree with ORIG in at least one case.
        int mutant_differs = 0;
        for (int i = 0; i < case_count; ++i) {
            Result a;
            Result m;
            run_case("ORIG", orig_fn, cases[i], a);
            run_case("mutant", mutant_fn, cases[i], m);
            if (m.hook.encrypt_calls != 1)
                failf("%s mutant boundary encrypt_calls %d", cases[i].label,
                      m.hook.encrypt_calls);
            if (result_differs(a, m))
                mutant_differs = 1;
        }
        if (!mutant_differs) {
            failf("mutant control never diverged from ORIG");
            printf("checks=%d failures=%d\n", g_checks, g_failures);
            return 1;
        }
    }
    return 0;
}
