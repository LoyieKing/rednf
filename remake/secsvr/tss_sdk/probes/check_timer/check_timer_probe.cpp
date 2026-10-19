// Isolated differential probe for tss_dpsdk::DPMsgHandler::check_timer.
//
// What is exercised
// -----------------
//   * The real ORIG function exported by libtss_sdk.so.1.0.394
//     (0x000afe70, symbol _ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl),
//     loaded with dlopen(RTLD_GLOBAL).
//   * One or two rebuilt shared objects built from
//     recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_recv_agent.cpp
//     (the "after" fixed source and, optionally, a patched "before" copy).
//     They are loaded RTLD_LOCAL and built with -Bsymbolic, so their own
//     check_timer/helper calls bind locally instead of being preempted by ORIG.
//   * DPMsgHandler::SendDpMsgToClient is deliberately left undefined in the
//     rebuilt objects.  The probe executable defines that exact mangled symbol
//     and exports it (-rdynamic); both ORIG's PLT call and the rebuilt call hit
//     this output-boundary hook.
//
// What is captured
// ----------------
//   The hook copies the DPInterData head and body, then invokes ORIG's real
//   sec_proto::DPInterData::pack(char*, unsigned, unsigned*, unsigned)
//   (0x0015c8c0) on the captured object.  Thus the wire bytes/size are produced
//   by the original serializer, not by the reconstructed packer.  The probe
//   does NOT execute the real SendDpMsgToClient encryption/send tail; that is
//   the intentionally isolated external boundary.
//
// State matrix
// ------------
//   Only ECS_INIT_CONFIRM (5) is used.  last_send_bin_file_time_,
//   last_send_bin_code_time_ and last_recv_stat_pkg_time_ are set to `now`
//   (gaps 1000/1000 and max_static 300), so no unrelated timer path can run.
//   test timeout is 200 and the three cases use elapsed 199/200/201.  All eight
//   values of the three DPProtoConfig switches are covered.
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
#define CHECK_TIMER_SYM \
    "_ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl"
#define SEND_HOOK_SYM \
    "_ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoERN9sec_proto11DPInterDataEPht"
#define PACK_SYM "_ZNK9sec_proto11DPInterData4packEPcjPjj"
#define CONFIG_SYM "_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E"
#define STAT_SYM \
    "_ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto15DPClientStatPtcE"
#define STAT_HOOK_SYM \
    "_ZN9tss_dpsdk11DPMsgPacker21send_error_report_msgEPN12tss_sdk_impl8UserInfoEii"

typedef int (*CheckTimerFn)(tss_dpsdk::DPMsgHandler *self,
                            tss_sdk_impl::UserInfo *user, time_t now);
// i386 member-function ABI: `this` is the first stack argument.
typedef int (*PackFn)(const sec_proto::DPInterData *self, char *buf,
                      unsigned int buf_len, unsigned int *used,
                      unsigned int mode);
typedef int (*StatFn)(tss_dpsdk::DPMsgHandler *self,
                      tss_sdk_impl::UserInfo *user,
                      sec_proto::DPClientStatPtc *cmd);

static const int kMaxCaptures = 4;
static const unsigned int kTestTimeout = 200;
static const unsigned int kOtherGap = 1000;
static const unsigned int kStaticTimeout = 300;
static const time_t kNow = (time_t)1700000000;

struct Capture {
    int valid;
    int self_ok;
    unsigned short packet_size;
    unsigned short protocol_type;
    unsigned char data_type;
    unsigned char raw_head[5];
    unsigned char body[16];
    int pack_res;
    unsigned int pack_used;
    unsigned char wire[64];
};

struct UserSnapshot {
    int state;
    unsigned int flags;
    unsigned int last_check_init;
    unsigned int last_test;
    unsigned int last_recv_stat;
    unsigned int last_bin_code;
    unsigned int last_bin_file;
};

struct RunResult {
    int ret;
    UserSnapshot user;
    int cap_count;
    Capture caps[kMaxCaptures];
};

struct Case {
    unsigned int delta; // elapsed since last_send_test_pkg_time_
    unsigned char proto_check_switch;
    unsigned char seq_check_switch;
    unsigned char raw_check_switch;
};

// ---------------------------------------------------------------------------
// output-boundary hook
// ---------------------------------------------------------------------------
static int g_hook_active;
static int g_hook_calls;
static int g_cap_count;
static Capture g_caps[kMaxCaptures];
static PackFn g_pack;
static const char *g_side = "?";
static void *g_expected_self;
// The stat regression uses the same recv TU, but only the error-report
// boundary is replaced; the handler body and packet are real.
struct StatErrorCapture {
    int id;
    int num;
};

static int g_stat_hook_active;
static int g_stat_error_count;
static StatErrorCapture g_stat_errors[16];

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
class DPMsgPacker {
public:
    static int send_error_report_msg(tss_sdk_impl::UserInfo *user,
                                     int error_id, int error_num);
};
}  // namespace tss_dpsdk

struct StatCase {
    const char *label;
    unsigned int user_game_fix_decrypt;
    unsigned int user_dynamic_encrypt;
    unsigned int cmd_fix_encrypt;
    unsigned int cmd_game_dynamic_decrypt;
};

struct StatResult {
    int ret;
    int state;
    unsigned int code_crc;
    unsigned int stat_fix_encrypt;
    unsigned int stat_internal_fix_encrypt;
    unsigned int stat_game_fix_decrypt;
    unsigned int stat_internal_fix_decrypt;
    unsigned int stat_dynamic_encrypt;
    unsigned int stat_game_dynamic_decrypt;
    int err_count;
    StatErrorCapture errs[16];
};

__attribute__((used, visibility("default")))
int tss_dpsdk::DPMsgHandler::SendDpMsgToClient(
        tss_sdk_impl::UserInfo *user, sec_proto::DPInterData &pkg,
        unsigned char *buf, unsigned short buf_len) {
    if (!g_hook_active) {
        fprintf(stderr,
                "FATAL: SendDpMsgToClient called outside an enabled probe "
                "invocation (side=%s)\n", g_side);
        return -1;
    }
    ++g_hook_calls;
    if (g_cap_count >= kMaxCaptures) {
        fprintf(stderr, "FATAL: more than %d SendDpMsgToClient calls\n",
                kMaxCaptures);
        return -1;
    }

    Capture &cap = g_caps[g_cap_count++];
    memset(&cap, 0, sizeof(cap));
    cap.valid = 1;
    cap.self_ok = (this == g_expected_self) ? 1 : 0;
    cap.packet_size = pkg.head.packet_size_;
    cap.protocol_type = pkg.head.protocol_type_;
    cap.data_type = pkg.head.data_type_;
    memcpy(cap.raw_head, &pkg.head, sizeof(cap.raw_head));
    memcpy(cap.body, &pkg.body,
           sizeof(cap.body) < sizeof(pkg.body) ? sizeof(cap.body)
                                                : sizeof(pkg.body));

    if (g_pack == 0) {
        cap.pack_res = -999;
        return -1;
    }

    // Use ORIG's serializer on a private copy.  The copy also keeps the
    // captured stack object untouched for the caller.
    sec_proto::DPInterData copy;
    memcpy(&copy, &pkg, sizeof(copy));
    cap.pack_res = g_pack(&copy, (char *)cap.wire, sizeof(cap.wire),
                          &cap.pack_used, 0);

    (void)user;
    (void)buf;
    (void)buf_len;
    return 0;
}
void ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, const char *, ...) {}

int tss_dpsdk::DPMsgPacker::send_error_report_msg(
        tss_sdk_impl::UserInfo *, int error_id, int error_num) {
    if (!g_stat_hook_active)
        return 0;
    if (g_stat_error_count < (int)(sizeof(g_stat_errors) /
                                  sizeof(g_stat_errors[0]))) {
        g_stat_errors[g_stat_error_count].id = error_id;
        g_stat_errors[g_stat_error_count].num = error_num;
    }
    ++g_stat_error_count;
    return 0;
}

// ---------------------------------------------------------------------------
// configuration / user state
// ---------------------------------------------------------------------------
static tss_dp_comm::DPConfig g_config;
static void **g_config_slot;
static tss_dpsdk::DPMsgHandler g_self;

static void apply_config(const Case &c) {
    if (g_config_slot == 0) {
        fprintf(stderr, "FATAL: glb_dp_config_ slot is null\n");
        exit(2);
    }
    memset(&g_config, 0, sizeof(g_config));
    g_config.dp_proto_conf.proto_check_switch = c.proto_check_switch;
    g_config.dp_proto_conf.seq_check_switch = c.seq_check_switch;
    g_config.dp_proto_conf.raw_check_switch = c.raw_check_switch;
    g_config.dp_time_confg.max_send_test_pkg_timeout = kTestTimeout;
    g_config.dp_time_confg.max_static_pkg_timeout = kStaticTimeout;
    g_config.dp_time_confg.send_bin_file_gap = kOtherGap;
    g_config.dp_time_confg.send_bin_code_gap = kOtherGap;
    *g_config_slot = &g_config;
}

static void snapshot_user(const tss_sdk_impl::UserInfo &u,
                          UserSnapshot &s) {
    memset(&s, 0, sizeof(s));
    s.state = (int)u.user_code_info_.client_state_;
    s.flags = u.base_info_.flags_;
    s.last_check_init = (unsigned int)u.user_timer_.last_check_init_pkg_time_;
    s.last_test = (unsigned int)u.user_timer_.last_send_test_pkg_time_;
    s.last_recv_stat = (unsigned int)u.user_timer_.last_recv_stat_pkg_time_;
    s.last_bin_code = (unsigned int)u.user_timer_.last_send_bin_code_time_;
    s.last_bin_file = (unsigned int)u.user_timer_.last_send_bin_file_time_;
}

static void run_impl(const char *side, CheckTimerFn fn, const Case &c,
                     RunResult &out) {
    memset(&out, 0, sizeof(out));

    tss_sdk_impl::UserInfo user;
    memset(&user, 0, sizeof(user));
    user.user_code_info_.client_state_ =
        tss_sdk_impl::UserInfo::ECS_INIT_CONFIRM;
    user.base_info_.flags_ = 0;
    user.user_timer_.last_check_init_pkg_time_ = kNow;
    user.user_timer_.last_send_bin_file_time_ = kNow;
    user.user_timer_.last_send_bin_code_time_ = kNow;
    user.user_timer_.last_recv_stat_pkg_time_ = kNow;
    user.user_timer_.last_send_test_pkg_time_ = kNow - (time_t)c.delta;

    apply_config(c);

    g_cap_count = 0;
    g_hook_calls = 0;
    g_side = side;
    g_expected_self = &g_self;
    g_hook_active = 1;
    out.ret = fn(&g_self, &user, kNow);
    g_hook_active = 0;

    out.cap_count = g_cap_count;
    if (out.cap_count > kMaxCaptures)
        out.cap_count = kMaxCaptures;
    memcpy(out.caps, g_caps, sizeof(Capture) * out.cap_count);
    snapshot_user(user, out.user);
}

static void run_stat(const char *side, StatFn fn, const StatCase &c,
                     StatResult &out) {
    memset(&out, 0, sizeof(out));

    tss_sdk_impl::UserInfo user;
    memset(&user, 0, sizeof(user));
    user.base_info_.uin_ = 0x12345678u;
    user.base_info_.game_usr_id_ = 0xffffffffu;
    user.base_info_.flags_ = 0;
    user.user_code_info_.client_state_ =
        tss_sdk_impl::UserInfo::ECS_INIT_CONFIRM;
    user.user_code_info_.down_cmd_list_crc_ = 0x11223344u;
    user.user_stat_.fix_encrypt_count_ = 0;
    user.user_stat_.internal_fix_encrypt_count_ = 0;
    user.user_stat_.game_fix_decrypt_count_ = c.user_game_fix_decrypt;
    user.user_stat_.internal_fix_decrypt_count_ = 0;
    user.user_stat_.dynamic_encrypt_count_ = c.user_dynamic_encrypt;

    sec_proto::DPClientStatPtc cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.down_cmd_list_crc_ = user.user_code_info_.down_cmd_list_crc_;
    cmd.fix_encrypt_count_ = c.cmd_fix_encrypt;
    cmd.game_dynamic_decrypt_count_ = c.cmd_game_dynamic_decrypt;
    cmd.game_fix_decrypt_count_ = 0;

    g_stat_error_count = 0;
    g_stat_hook_active = 1;
    out.ret = fn(&g_self, &user, &cmd);
    g_stat_hook_active = 0;

    out.state = (int)user.user_code_info_.client_state_;
    out.code_crc = user.user_code_info_.down_cmd_list_crc_;
    out.stat_fix_encrypt = user.user_stat_.fix_encrypt_count_;
    out.stat_internal_fix_encrypt =
        user.user_stat_.internal_fix_encrypt_count_;
    out.stat_game_fix_decrypt = user.user_stat_.game_fix_decrypt_count_;
    out.stat_internal_fix_decrypt =
        user.user_stat_.internal_fix_decrypt_count_;
    out.stat_dynamic_encrypt = user.user_stat_.dynamic_encrypt_count_;
    out.stat_game_dynamic_decrypt =
        user.user_stat_.game_dynamic_decrypt_count_;

    out.err_count = g_stat_error_count;
    if (out.err_count > (int)(sizeof(out.errs) / sizeof(out.errs[0])))
        out.err_count = (int)(sizeof(out.errs) / sizeof(out.errs[0]));
    memcpy(out.errs, g_stat_errors,
           sizeof(StatErrorCapture) * out.err_count);
}

// ---------------------------------------------------------------------------
// diagnostics
// ---------------------------------------------------------------------------
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

static void hex(const unsigned char *p, unsigned int n, char *out,
                size_t out_size) {
    size_t used = 0;
    if (out_size == 0)
        return;
    out[0] = 0;
    for (unsigned int i = 0; i < n && used + 3 < out_size; ++i) {
        int nw = snprintf(out + used, out_size - used, "%02x", p[i]);
        if (nw < 0)
            break;
        used += (size_t)nw;
    }
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

// ---------------------------------------------------------------------------
// comparison
// ---------------------------------------------------------------------------
static void compare_user(const RunResult &a, const RunResult &b,
                         const char *label) {
    ++g_checks;
    if (a.user.state != b.user.state)
        failf("%s client_state %d/%d", label, a.user.state, b.user.state);
    if (a.user.flags != b.user.flags)
        failf("%s flags %u/%u", label, a.user.flags, b.user.flags);
    if (a.user.last_check_init != b.user.last_check_init)
        failf("%s last_check_init %u/%u", label, a.user.last_check_init,
              b.user.last_check_init);
    if (a.user.last_test != b.user.last_test)
        failf("%s last_send_test %u/%u", label, a.user.last_test,
              b.user.last_test);
    if (a.user.last_recv_stat != b.user.last_recv_stat)
        failf("%s last_recv_stat %u/%u", label, a.user.last_recv_stat,
              b.user.last_recv_stat);
    if (a.user.last_bin_code != b.user.last_bin_code)
        failf("%s last_send_bin_code %u/%u", label, a.user.last_bin_code,
              b.user.last_bin_code);
    if (a.user.last_bin_file != b.user.last_bin_file)
        failf("%s last_send_bin_file %u/%u", label, a.user.last_bin_file,
              b.user.last_bin_file);
}

static void compare_capture(const Capture &a, const Capture &b,
                            const char *label, int index) {
    char wa[160];
    char wb[160];
    ++g_checks;
    if (a.valid != b.valid)
        failf("%s cap[%d] valid %d/%d", label, index, a.valid, b.valid);
    if (a.self_ok != b.self_ok)
        failf("%s cap[%d] self pointer %d/%d", label, index, a.self_ok,
              b.self_ok);
    if (a.packet_size != b.packet_size)
        failf("%s cap[%d] packet_size %u/%u", label, index, a.packet_size,
              b.packet_size);
    if (a.protocol_type != b.protocol_type)
        failf("%s cap[%d] protocol_type %u/%u", label, index, a.protocol_type,
              b.protocol_type);
    if (a.data_type != b.data_type)
        failf("%s cap[%d] data_type %u/%u", label, index, a.data_type,
              b.data_type);
    if (a.pack_res != b.pack_res)
        failf("%s cap[%d] pack_res %d/%d", label, index, a.pack_res,
              b.pack_res);
    if (a.pack_used != b.pack_used)
        failf("%s cap[%d] pack_used %u/%u", label, index, a.pack_used,
              b.pack_used);
    if (memcmp(a.body, b.body, 3) != 0)
        failf("%s cap[%d] body[0..2] differs", label, index);
    if (a.pack_res == 0 && b.pack_res == 0) {
        unsigned int n = a.pack_used < b.pack_used ? a.pack_used : b.pack_used;
        if (n > sizeof(a.wire))
            n = sizeof(a.wire);
        if (memcmp(a.wire, b.wire, n) != 0) {
            hex(a.wire, a.pack_used, wa, sizeof(wa));
            hex(b.wire, b.pack_used, wb, sizeof(wb));
            failf("%s cap[%d] wire differs: %s / %s", label, index, wa, wb);
        }
    }
}

static void compare_run(const char *label, const RunResult &a,
                        const RunResult &b) {
    ++g_checks;
    if (a.ret != b.ret)
        failf("%s check_timer ret %d/%d", label, a.ret, b.ret);
    if (a.cap_count != b.cap_count)
        failf("%s capture count %d/%d", label, a.cap_count, b.cap_count);
    compare_user(a, b, label);
    int n = a.cap_count < b.cap_count ? a.cap_count : b.cap_count;
    for (int i = 0; i < n; ++i)
        compare_capture(a.caps[i], b.caps[i], label, i);
}

static int stat_scalars_equal(const StatResult &a, const StatResult &b) {
    return a.ret == b.ret && a.state == b.state && a.code_crc == b.code_crc &&
           a.stat_fix_encrypt == b.stat_fix_encrypt &&
           a.stat_internal_fix_encrypt == b.stat_internal_fix_encrypt &&
           a.stat_game_fix_decrypt == b.stat_game_fix_decrypt &&
           a.stat_internal_fix_decrypt == b.stat_internal_fix_decrypt &&
           a.stat_dynamic_encrypt == b.stat_dynamic_encrypt &&
           a.stat_game_dynamic_decrypt == b.stat_game_dynamic_decrypt;
}

static void compare_stat(const char *side, const char *label,
                         const StatResult &orig, const StatResult &got) {
    ++g_checks;
    if (!stat_scalars_equal(orig, got))
        failf("%s/%s ret/state/counters %d/%d state %d/%d", side, label,
              orig.ret, got.ret, orig.state, got.state);
    if (orig.err_count != got.err_count) {
        failf("%s/%s error report count %d/%d", side, label, orig.err_count,
              got.err_count);
    }
    const int n = orig.err_count < got.err_count ? orig.err_count
                                                 : got.err_count;
    for (int i = 0; i < n; ++i) {
        ++g_checks;
        if (orig.errs[i].id != got.errs[i].id ||
            orig.errs[i].num != got.errs[i].num) {
            failf("%s/%s error[%d] %d/%d  %d/%d", side, label, i,
                  orig.errs[i].id, orig.errs[i].num, got.errs[i].id,
                  got.errs[i].num);
        }
    }
}

static int stat_result_differs(const StatResult &a, const StatResult &b) {
    if (!stat_scalars_equal(a, b))
        return 1;
    if (a.err_count != b.err_count)
        return 1;
    for (int i = 0; i < a.err_count; ++i) {
        if (a.errs[i].id != b.errs[i].id || a.errs[i].num != b.errs[i].num)
            return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// expected wire shape for the fixed/ORIG implementation
// ---------------------------------------------------------------------------
static void check_expected(const char *side, const RunResult &r,
                           const Case &c) {
    const int due = c.delta >= kTestTimeout;
    const unsigned int exp_last_test =
        due ? (unsigned int)kNow : (unsigned int)(kNow - (time_t)c.delta);
    ++g_checks;
    if (r.ret != 0)
        failf("%s expected ret 0, got %d", side, r.ret);
    if (r.user.state != (int)tss_sdk_impl::UserInfo::ECS_INIT_CONFIRM)
        failf("%s expected state 5, got %d", side, r.user.state);
    if (r.user.flags != 0)
        failf("%s expected flags 0, got %u", side, r.user.flags);
    if (r.user.last_test != exp_last_test)
        failf("%s last_send_test %u, expected %u", side, r.user.last_test,
              exp_last_test);
    if (r.user.last_recv_stat != (unsigned int)kNow)
        failf("%s last_recv_stat changed to %u", side, r.user.last_recv_stat);
    if (r.user.last_bin_code != (unsigned int)kNow)
        failf("%s last_send_bin_code changed to %u", side,
              r.user.last_bin_code);
    if (r.user.last_bin_file != (unsigned int)kNow)
        failf("%s last_send_bin_file changed to %u", side,
              r.user.last_bin_file);
    if (r.user.last_check_init != (unsigned int)kNow)
        failf("%s last_check_init changed to %u", side, r.user.last_check_init);

    if (!due) {
        if (r.cap_count != 0)
            failf("%s before-timeout sent %d packets", side, r.cap_count);
        return;
    }

    if (r.cap_count != 2) {
        failf("%s due case sent %d packets, expected 2", side, r.cap_count);
        return;
    }

    // First packet: the existing 15/15 test packet.
    const Capture &t = r.caps[0];
    ++g_checks;
    if (!t.valid || !t.self_ok)
        failf("%s test pkg hook self pointer/validity mismatch", side);
    if (t.packet_size != 15 || t.protocol_type != 15 || t.data_type != 0)
        failf("%s test pkg head %u/%u/%u, expected 15/15/0", side,
              t.packet_size, t.protocol_type, t.data_type);
    if (t.pack_res != 0)
        failf("%s test pkg pack_res %d", side, t.pack_res);
    if (t.pack_used != 15)
        failf("%s test pkg pack_used %u, expected 15", side, t.pack_used);
    {
        static const unsigned char exp_head[5] = {0x00, 0x0f, 0x00, 0x0f,
                                                   0x00};
        static const unsigned char exp_body[10] = {'0', '1', '2', '3', '4',
                                                    '5', '6', '7', '8', '9'};
        if (memcmp(t.wire, exp_head, 5) != 0)
            failf("%s test pkg wire head mismatch", side);
        if (memcmp(t.wire + 5, exp_body, 10) != 0)
            failf("%s test pkg wire body mismatch", side);
    }

    // Second packet: 8-byte config packet.  packet_size_ is written
    // big-endian by the real serializer; protocol_type_ selects the
    // DPConfigData union arm.
    const Capture &cfg = r.caps[1];
    ++g_checks;
    if (!cfg.valid || !cfg.self_ok)
        failf("%s config pkg hook self pointer/validity mismatch", side);
    if (cfg.packet_size != 8)
        failf("%s config pkg packet_size %u, expected 8", side,
              cfg.packet_size);
    if (cfg.protocol_type != 0x14)
        failf("%s config pkg protocol_type %u, expected 0x14", side,
              cfg.protocol_type);
    if (cfg.data_type != 0)
        failf("%s config pkg data_type %u, expected 0", side, cfg.data_type);
    if (cfg.pack_res != 0)
        failf("%s config pkg pack_res %d", side, cfg.pack_res);
    if (cfg.pack_used != 8)
        failf("%s config pkg pack_used %u, expected 8", side, cfg.pack_used);
    {
        static const unsigned char exp_head[5] = {0x00, 0x08, 0x00, 0x14,
                                                   0x00};
        if (memcmp(cfg.wire, exp_head, 5) != 0)
            failf("%s config pkg wire head mismatch", side);
    }
    if (cfg.body[0] != c.proto_check_switch ||
        cfg.body[1] != c.seq_check_switch ||
        cfg.body[2] != c.raw_check_switch)
        failf("%s config body %u/%u/%u, expected %u/%u/%u", side,
              cfg.body[0], cfg.body[1], cfg.body[2], c.proto_check_switch,
              c.seq_check_switch, c.raw_check_switch);
    if (cfg.wire[5] != c.proto_check_switch ||
        cfg.wire[6] != c.seq_check_switch ||
        cfg.wire[7] != c.raw_check_switch)
        failf("%s config wire switches %u/%u/%u, expected %u/%u/%u", side,
              cfg.wire[5], cfg.wire[6], cfg.wire[7], c.proto_check_switch,
              c.seq_check_switch, c.raw_check_switch);
}

// The old (pre-fix) source is expected to fail only at the second packet:
// packet_size_=20/protocol_type_=8 means DPInterDataBody::packEx has no case
// and writes no body, so the real packer emits 5 bytes instead of 8.
static void check_before_negative(const Case &c, const RunResult &orig,
                                  const RunResult &before) {
    const int due = c.delta >= kTestTimeout;
    if (!due) {
        compare_run("before(non-due)", orig, before);
        return;
    }

    ++g_checks;
    if (before.ret != orig.ret)
        failf("before ret %d/%d", before.ret, orig.ret);
    if (before.cap_count != 2 || orig.cap_count != 2) {
        failf("before capture count %d/%d", before.cap_count, orig.cap_count);
        return;
    }
    compare_user(orig, before, "before");
    compare_capture(orig.caps[0], before.caps[0], "before", 0);

    const Capture &o = orig.caps[1];
    const Capture &b = before.caps[1];
    if (!b.valid || !b.self_ok)
        failf("before config pkg hook self pointer/validity mismatch");
    ++g_checks;
    int differs = 0;
    if (b.packet_size != o.packet_size || b.protocol_type != o.protocol_type ||
        b.pack_used != o.pack_used)
        differs = 1;
    {
        unsigned int n = b.pack_used < o.pack_used ? b.pack_used : o.pack_used;
        if (n > sizeof(b.wire))
            n = sizeof(b.wire);
        if (memcmp(b.wire, o.wire, n) != 0)
            differs = 1;
    }
    if (!differs) {
        failf("NEGATIVE CONTROL FAILED: before variant matched ORIG for "
              "switches %u/%u/%u; rebuilt check_timer may have been preempted",
              c.proto_check_switch, c.seq_check_switch, c.raw_check_switch);
        return;
    }

    // Verify it differs for the exact pre-fix reason, not random corruption.
    if (b.packet_size != 20)
        failf("before config packet_size %u, expected old 20", b.packet_size);
    if (b.protocol_type != 8)
        failf("before config protocol_type %u, expected old 8",
              b.protocol_type);
    if (b.pack_res != 0)
        failf("before config pack_res %d", b.pack_res);
    if (b.pack_used != 5)
        failf("before config pack_used %u, expected old bodyless 5",
              b.pack_used);
    {
        static const unsigned char exp_old_head[5] = {0x00, 0x14, 0x00, 0x08,
                                                      0x00};
        if (memcmp(b.wire, exp_old_head, 5) != 0)
            failf("before config wire head is not the old swapped header");
    }
    if (b.body[0] != c.proto_check_switch ||
        b.body[1] != c.seq_check_switch ||
        b.body[2] != c.raw_check_switch)
        failf("before config stack body lost the switch bytes");
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
static void usage(const char *argv0) {
    fprintf(stderr,
            "usage: %s [--orig PATH] --after PATH [--before PATH] "
            "[--stat-before PATH]\n"
            "  --orig         ORIG libtss_sdk backup (default: %s; env "
            "TSS_SDK_ORIG overrides)\n"
            "  --after        rebuilt check_timer/stat shared object (fixed "
            "source)\n"
            "  --before       optional rebuilt check_timer pre-fix object; its "
            "config frame must differ from ORIG\n"
            "  --stat-before  optional rebuilt stat pre-fix object; its "
            "DPClientStatPtc error report must differ from ORIG\n",
            argv0, ORIG_DEFAULT);
}

int main(int argc, char **argv) {
    const char *orig_path = getenv("TSS_SDK_ORIG");
    if (orig_path == 0 || orig_path[0] == 0)
        orig_path = ORIG_DEFAULT;
    const char *after_path = 0;
    const char *before_path = 0;
    const char *stat_before_path = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--orig") == 0 && i + 1 < argc) {
            orig_path = argv[++i];
        } else if (strcmp(argv[i], "--after") == 0 && i + 1 < argc) {
            after_path = argv[++i];
        } else if (strcmp(argv[i], "--before") == 0 && i + 1 < argc) {
            before_path = argv[++i];
        } else if (strcmp(argv[i], "--stat-before") == 0 && i + 1 < argc) {
            stat_before_path = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 2;
        }
    }
    if (after_path == 0) {
        usage(argv[0]);
        return 2;
    }

    void *h_orig = must_dlopen(orig_path, RTLD_NOW | RTLD_GLOBAL, "ORIG");
    CheckTimerFn orig_fn =
        (CheckTimerFn)must_dlsym(h_orig, CHECK_TIMER_SYM, "ORIG check_timer");
    g_pack = (PackFn)must_dlsym(h_orig, PACK_SYM, "ORIG DPInterData::pack");
    g_config_slot = (void **)must_dlsym(h_orig, CONFIG_SYM,
                                        "ORIG glb_dp_config_");
    {
        void *hook = dlsym(RTLD_DEFAULT, SEND_HOOK_SYM);
        Dl_info hi;
        Dl_info mi;
        memset(&hi, 0, sizeof(hi));
        memset(&mi, 0, sizeof(mi));
        if (hook == 0 || dladdr(hook, &hi) == 0 || hi.dli_fname == 0 ||
            dladdr((void *)&usage, &mi) == 0 || mi.dli_fname == 0 ||
            !same_realpath(hi.dli_fname, mi.dli_fname)) {
            fprintf(stderr,
                    "FATAL: %s is not exported by the probe executable "
                    "(dlsym=%p, owner=%s); link with -rdynamic\n",
                    SEND_HOOK_SYM, hook,
                    (hook && dladdr(hook, &hi) && hi.dli_fname)
                        ? hi.dli_fname
                        : "<null>");
            return 2;
        }
        printf("hook symbol : %p (probe executable)\n", hook);
    }

    void *h_after = must_dlopen(after_path, RTLD_NOW | RTLD_LOCAL, "after");
    CheckTimerFn after_fn =
        (CheckTimerFn)must_dlsym(h_after, CHECK_TIMER_SYM, "after check_timer");
    if (after_fn == orig_fn) {
        fprintf(stderr,
                "FATAL: after check_timer resolved to ORIG address (%p); "
                "refusing a vacuous comparison\n", (void *)after_fn);
        return 2;
    }
    {
        Dl_info info;
        memset(&info, 0, sizeof(info));
        if (dladdr((void *)after_fn, &info) == 0 || info.dli_fname == 0 ||
            !same_realpath(info.dli_fname, after_path)) {
            fprintf(stderr,
                    "FATAL: after check_timer (%p) is not in %s (dladdr=%s)\n",
                    (void *)after_fn, after_path,
                    (info.dli_fname ? info.dli_fname : "<null>"));
            return 2;
        }
    }
    {
        void *after_cfg = dlsym(h_after, CONFIG_SYM);
        if (after_cfg != 0 && after_cfg != (void *)g_config_slot) {
            fprintf(stderr,
                    "FATAL: after DSO glb_dp_config_ lookup %p != ORIG %p; "
                    "the DSO is not using the probe configuration\n",
                    after_cfg, (void *)g_config_slot);
            return 2;
        }
    }

    StatFn orig_stat_fn =
        (StatFn)must_dlsym(h_orig, STAT_SYM, "ORIG OnRecvMsgFromClient(stat)");
    StatFn after_stat_fn =
        (StatFn)must_dlsym(h_after, STAT_SYM, "after OnRecvMsgFromClient(stat)");
    if (after_stat_fn == orig_stat_fn) {
        fprintf(stderr,
                "FATAL: after stat handler resolved to ORIG address (%p)\n",
                (void *)after_stat_fn);
        return 2;
    }
    {
        void *stat_hook = dlsym(RTLD_DEFAULT, STAT_HOOK_SYM);
        Dl_info hi;
        Dl_info mi;
        memset(&hi, 0, sizeof(hi));
        memset(&mi, 0, sizeof(mi));
        if (stat_hook == 0 || dladdr(stat_hook, &hi) == 0 ||
            hi.dli_fname == 0 || dladdr((void *)&usage, &mi) == 0 ||
            mi.dli_fname == 0 ||
            !same_realpath(hi.dli_fname, mi.dli_fname)) {
            fprintf(stderr,
                    "FATAL: %s is not exported by the probe executable; "
                    "stat error reports would be uncaptured\n",
                    STAT_HOOK_SYM);
            return 2;
        }
    }

    void *h_before = 0;
    CheckTimerFn before_fn = 0;
    if (before_path) {
        h_before = must_dlopen(before_path, RTLD_NOW | RTLD_LOCAL, "before");
        before_fn = (CheckTimerFn)must_dlsym(h_before, CHECK_TIMER_SYM,
                                             "before check_timer");
        if (before_fn == orig_fn || before_fn == after_fn) {
            fprintf(stderr,
                    "FATAL: before check_timer (%p) aliases ORIG (%p) or "
                    "after (%p)\n", (void *)before_fn, (void *)orig_fn,
                    (void *)after_fn);
            return 2;
        }
        Dl_info info;
        memset(&info, 0, sizeof(info));
        if (dladdr((void *)before_fn, &info) == 0 || info.dli_fname == 0 ||
            !same_realpath(info.dli_fname, before_path)) {
            fprintf(stderr,
                    "FATAL: before check_timer (%p) is not in %s "
                    "(dladdr=%s)\n", (void *)before_fn, before_path,
                    (info.dli_fname ? info.dli_fname : "<null>"));
            return 2;
        }
        {
            void *before_cfg = dlsym(h_before, CONFIG_SYM);
            if (before_cfg != 0 && before_cfg != (void *)g_config_slot) {
                fprintf(stderr,
                        "FATAL: before DSO glb_dp_config_ lookup %p != ORIG "
                        "%p; the DSO is not using the probe configuration\n",
                        before_cfg, (void *)g_config_slot);
                return 2;
            }
        }
    }

    void *h_stat_before = 0;
    StatFn stat_before_fn = 0;
    if (stat_before_path) {
        h_stat_before = must_dlopen(stat_before_path, RTLD_NOW | RTLD_LOCAL,
                                    "stat-before");
        stat_before_fn = (StatFn)must_dlsym(
            h_stat_before, STAT_SYM, "stat-before OnRecvMsgFromClient(stat)");
        if (stat_before_fn == orig_stat_fn ||
            stat_before_fn == after_stat_fn) {
            fprintf(stderr,
                    "FATAL: stat-before handler (%p) aliases ORIG (%p) or "
                    "after (%p)\n", (void *)stat_before_fn,
                    (void *)orig_stat_fn, (void *)after_stat_fn);
            return 2;
        }
        Dl_info info;
        memset(&info, 0, sizeof(info));
        if (dladdr((void *)stat_before_fn, &info) == 0 ||
            info.dli_fname == 0 ||
            !same_realpath(info.dli_fname, stat_before_path)) {
            fprintf(stderr,
                    "FATAL: stat-before handler (%p) is not in %s "
                    "(dladdr=%s)\n", (void *)stat_before_fn,
                    stat_before_path,
                    (info.dli_fname ? info.dli_fname : "<null>"));
            return 2;
        }
    }

    printf("ORIG        : %s\n", orig_path);
    printf("ORIG check_timer : %p\n", (void *)orig_fn);
    printf("ORIG pack   : %p (%s)\n", (void *)g_pack, PACK_SYM);
    printf("AFTER       : %s (%p)\n", after_path, (void *)after_fn);
    if (before_path)
        printf("BEFORE      : %s (%p)\n", before_path, (void *)before_fn);
    if (before_path)
        printf("NOTE: BEFORE is the pre-fix negative control; its config "
               "frame must differ (5 vs 8 bytes), not match.\n");
    printf("output hook : %s\n", SEND_HOOK_SYM);
    printf("ORIG stat   : %p\n", (void *)orig_stat_fn);
    printf("AFTER stat  : %p\n", (void *)after_stat_fn);
    if (stat_before_path)
        printf("STAT-BEFORE : %s (%p)\n", stat_before_path,
               (void *)stat_before_fn);
    if (stat_before_path)
        printf("NOTE: STAT-BEFORE is the DPClientStatPtc pre-fix negative "
               "control; it must differ from ORIG.\n");
    printf("stat hook   : %s\n", STAT_HOOK_SYM);
    printf("\n");

    static const unsigned int deltas[3] = {199, 200, 201};
    int cases = 0;
    int negative_cases = 0;

    for (int di = 0; di < 3; ++di) {
        for (int proto = 0; proto < 2; ++proto) {
            for (int seq = 0; seq < 2; ++seq) {
                for (int raw = 0; raw < 2; ++raw) {
                    Case c;
                    c.delta = deltas[di];
                    c.proto_check_switch = (unsigned char)proto;
                    c.seq_check_switch = (unsigned char)seq;
                    c.raw_check_switch = (unsigned char)raw;
                    ++cases;

                    RunResult o;
                    RunResult a;
                    run_impl("ORIG", orig_fn, c, o);
                    run_impl("AFTER", after_fn, c, a);

                    char label[96];
                    snprintf(label, sizeof(label),
                             "delta=%u switches=%u/%u/%u", c.delta, proto, seq,
                             raw);

                    int fail_before_case = g_failures;
                    compare_run(label, o, a);
                    check_expected("ORIG", o, c);
                    check_expected("AFTER", a, c);

                    char owire[160] = "";
                    char awire[160] = "";
                    if (o.cap_count >= 2)
                        hex(o.caps[1].wire, o.caps[1].pack_used, owire,
                            sizeof(owire));
                    if (a.cap_count >= 2)
                        hex(a.caps[1].wire, a.caps[1].pack_used, awire,
                            sizeof(awire));

                    printf("CASE %-28s caps=%d/%d cfg %s / %s\n", label,
                           o.cap_count, a.cap_count, owire, awire);

                    if (before_fn) {
                        RunResult b;
                        run_impl("BEFORE", before_fn, c, b);
                        check_before_negative(c, o, b);
                        if (c.delta >= kTestTimeout)
                            ++negative_cases;
                        if (c.delta >= kTestTimeout && b.cap_count >= 2) {
                            char bwire[160];
                            hex(b.caps[1].wire, b.caps[1].pack_used, bwire,
                                sizeof(bwire));
                            printf("  BEFORE cfg used=%u wire=%s\n",
                                   b.caps[1].pack_used, bwire);
                        }
                    }

                    if (g_failures == fail_before_case) {
                        printf("  MATCH\n");
                    } else {
                        printf("  MISMATCH\n");
                    }
                }
            }
        }
    }

    // -----------------------------------------------------------------
    // DPClientStatPtc regression: ORIG 0xaf7fc reads cmd.fix_encrypt_count_
    // (+0x0c) for fix_decrypt_mismatch.  The pre-fix reconstruction read
    // cmd.game_dynamic_decrypt_count_ (+0x14), which selects different error
    // reports for exactly these two counter combinations.
    // -----------------------------------------------------------------
    static const StatCase stat_cases[] = {
        {"stat_baseline", 0, 0, 0, 0},
        {"stat_fix_field_zero", 5, 0, 10, 0},
        {"stat_fix_field_high", 5, 10, 0, 10},
    };
    const int stat_case_count =
        (int)(sizeof(stat_cases) / sizeof(stat_cases[0]));
    int stat_cases_run = 0;
    int stat_negative_diff = 0;
    for (int si = 0; si < stat_case_count; ++si) {
        ++stat_cases_run;
        StatResult o;
        StatResult a;
        run_stat("ORIG", orig_stat_fn, stat_cases[si], o);
        run_stat("AFTER", after_stat_fn, stat_cases[si], a);
        compare_stat("AFTER", stat_cases[si].label, o, a);

        // Expected ORIG oracle values for this small matrix.
        if (si == 0 || si == 1) {
            if (o.err_count != 0)
                failf("ORIG/%s expected no error report, got %d",
                      stat_cases[si].label, o.err_count);
        } else if (si == 2) {
            if (o.err_count != 1 || o.errs[0].id != 0x199 ||
                o.errs[0].num != 5) {
                failf("ORIG/%s expected 0x199/5, got count=%d id=%d num=%d",
                      stat_cases[si].label, o.err_count,
                      o.err_count ? o.errs[0].id : -1,
                      o.err_count ? o.errs[0].num : -1);
            }
        }
        printf("STAT CASE %-24s errors ORIG=%d AFTER=%d\n",
               stat_cases[si].label, o.err_count, a.err_count);

        if (stat_before_fn) {
            StatResult b;
            run_stat("STAT-BEFORE", stat_before_fn, stat_cases[si], b);
            if (stat_result_differs(o, b))
                ++stat_negative_diff;
            if (si == 1 &&
                (b.err_count != 1 || b.errs[0].id != 0x199 ||
                 b.errs[0].num != 5)) {
                failf("STAT-BEFORE/%s expected pre-fix 0x199/5, count=%d",
                      stat_cases[si].label, b.err_count);
            }
            if (si == 2 && b.err_count != 0)
                failf("STAT-BEFORE/%s expected pre-fix no report, got %d",
                      stat_cases[si].label, b.err_count);
        }
    }
    if (stat_before_fn && stat_negative_diff == 0) {
        failf("NEGATIVE CONTROL FAILED: stat-before matched ORIG for every "
              "DPClientStatPtc case");
    }

    printf("\nSUMMARY cases=%d stat-cases=%d checks=%d failures=%d", cases,
           stat_cases_run, g_checks, g_failures);
    if (before_fn)
        printf(" negative-before-cases=%d", negative_cases);
    if (stat_before_fn)
        printf(" stat-negative-diff=%d", stat_negative_diff);
    printf("\n");

    if (g_failures != 0) {
        printf("RESULT: FAIL\n");
        return 1;
    }
    printf("RESULT: PASS\n");
    return 0;
}
