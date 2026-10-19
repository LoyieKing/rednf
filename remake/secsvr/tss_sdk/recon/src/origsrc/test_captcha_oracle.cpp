// Differential oracle: reconstructed captcha / safe-mode-adapt CUs vs ORIG.
//
// Covers, in one binary:
//   * formmog_proto_captcha_client.cpp              (new captcha protocol)
//   * formmog_proto_captcha_client_old.cpp          (old captcha protocol)
//   * formmog_proto_public_safemodetransadapt.cpp   (safe-mode transfer adapt)
//
// Method (same as test_uic_oracle.cpp): the ORIG library is dlopen()ed
// RTLD_GLOBAL so the reconstructed objects' TdrBufUtil references resolve to
// the ORIG runtime; /tmp/libcaptcha_recon.so (the three reconstructed TUs) is
// dlopen()ed RTLD_LOCAL and every entry point is reached through dlsym by
// mangled name.  For each generated object we compare
//   pack       : return code, *usedSize and the whole kCap output buffer,
//   unpack     : return code, *usedSize and the whole destination struct,
//   visualize  : return code, *usedSize and the whole kCap text buffer,
// plus truncated unpacks and a small output buffer to drive the -2/-1 paths.
//
// Users' constraints honoured: randomized length fields stay within the array
// bounds, text arrays are printable and NUL-terminated, binary arrays are
// filled with arbitrary bytes, and every recovered union selector is exercised
// explicitly (new: 3201-3204; old: 1, 3073, 3075, 3076, 3078, 3090, 3091, 3092,
// plus out-of-range selectors for the default branch).
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "formmog_proto_captcha_client.h"
#include "formmog_proto_captcha_client_old.h"
#include "formmog_proto_public_safemodetransadapt.h"

using namespace sec_proto;

namespace {

const char *kOrigPath =
    "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
const char *kReconPath = "/tmp/libcaptcha_recon.so";

const unsigned kCap = 65536;
const unsigned kIters = 100;
int g_cases = 0;
int g_fail = 0;

uint32_t g_rng = 0x3940cafeu;
uint32_t rnd() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng >> 8;
}

void fill_random(void *p, size_t n) {
    unsigned char *b = static_cast<unsigned char *>(p);
    for (size_t i = 0; i < n; ++i) b[i] = (unsigned char)(rnd() & 0xff);
}

// Printable, NUL-terminated text of n bytes (last byte is always 0).
void fill_text(int8_t *p, unsigned n) {
    for (unsigned i = 0; i + 1 < n; ++i)
        p[i] = (int8_t)(1 + (rnd() % 0x7e));
    if (n) p[n - 1] = 0;
}

void fill_text_u(uint8_t *p, unsigned n) {
    for (unsigned i = 0; i + 1 < n; ++i)
        p[i] = (uint8_t)(1 + (rnd() % 0x7e));
    if (n) p[n - 1] = 0;
}

typedef int (*pack_t)(const void *, char *, unsigned, unsigned *, unsigned);
typedef int (*unpack_t)(void *, const char *, unsigned, unsigned *, unsigned);
typedef int (*vis_t)(const void *, char *, unsigned, unsigned *, int, char);
typedef int (*pack_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                          unsigned);
typedef int (*unpack_sel_t)(void *, int64_t, const char *, unsigned,
                            unsigned *, unsigned);
typedef int (*vis_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                         int, char);

void check(const char *label, const char *what, int a, int b, unsigned ua,
           unsigned ub, const char *ba, const char *bb, unsigned n) {
    ++g_cases;
    if (a != b || ua != ub || memcmp(ba, bb, n) != 0) {
        ++g_fail;
        unsigned d = n;
        for (unsigned i = 0; i < n; ++i) {
            if (ba[i] != bb[i]) {
                d = i;
                break;
            }
        }
        unsigned x = d < n ? (unsigned char)ba[d] : 0;
        unsigned y = d < n ? (unsigned char)bb[d] : 0;
        printf("FAIL %-22s %-12s ret %d/%d used %u/%u first-diff %u (%02x/%02x)\n",
               label, what, a, b, ua, ub, d, x, y);
    }
}

// ---------------------------------------------------------------------------
// new captcha protocol fixtures
// ---------------------------------------------------------------------------

void fix_new_head(TSSCaptchaHead &h) {
    memset(&h, 0, sizeof(h));
    h.msg_id_ = (uint16_t)(3201 + rnd() % 4);
    h.uin_ = rnd();
    h.busi_seq_ = rnd();
    h.seq_ = rnd();
    h.body_len_ = (uint16_t)(rnd() % 4096);
}

void fix_set_captcha(TSSSetCaptcha &s) {
    memset(&s, 0, sizeof(s));
    s.client_ip_ = rnd();
    s.platform_busid_ = rnd();
    s.max_pic_change_num_ = (uint16_t)(rnd() % 16);
    s.max_verify_num_ = (uint16_t)(rnd() % 16);
    s.timeout_ = rnd();
    s.busi_id_ = (uint8_t)(rnd() % 8);
    unsigned n = rnd() % 513;
    s.busi_info_len_ = (uint16_t)n;
    fill_text(s.busi_info_, n);
    // Keep the untouched tail deterministic (memset already zeroed it).
}

void fix_captcha_pic(TSSCaptchaPic &p) {
    memset(&p, 0, sizeof(p));
    p.timeout_ = rnd();
    p.verify_times_limit_ = (uint16_t)(rnd() % 32);
    p.verify_times_ = (uint16_t)(rnd() % 32);
    p.change_times_limit_ = (uint16_t)(rnd() % 32);
    p.change_times_ = (uint16_t)(rnd() % 32);
    unsigned n = rnd() % 4097;
    p.picture_len_ = (uint16_t)n;
    fill_random(p.picture_, n);
    p.busi_id_ = (uint8_t)(rnd() % 8);
    unsigned m = rnd() % 513;
    p.busi_info_len_ = (uint16_t)m;
    fill_text(p.busi_info_, m);
}

void fix_new_verify_req(TSSVerifyReq &r) {
    memset(&r, 0, sizeof(r));
    unsigned n = rnd() % 65;
    r.answer_len_ = (uint8_t)n;
    fill_text_u(r.answer_, n);
}

void fix_new_verify_res(TSSVerifyRes &r) {
    memset(&r, 0, sizeof(r));
    r.result_ = (uint8_t)(rnd() % 4);
    unsigned n = rnd() % 129;
    r.msg_len_ = (uint8_t)n;
    fill_text(r.msg_, n);
}

void fix_new_pkg(TSSCaptchaPkg &p) {
    memset(&p, 0, sizeof(p));
    fix_new_head(p.head_);
    switch (p.head_.msg_id_) {
        case 3201: fix_set_captcha(p.body_.set_captcha_); break;
        case 3202: fix_captcha_pic(p.body_.captcha_pic_); break;
        case 3203: fix_new_verify_req(p.body_.verify_req_); break;
        default: fix_new_verify_res(p.body_.captcha_res_); break;
    }
}

void fix_new_body(TSSCaptchaBody &b, int64_t sel) {
    memset(&b, 0, sizeof(b));
    switch (sel) {
        case 3201: fix_set_captcha(b.set_captcha_); break;
        case 3202: fix_captcha_pic(b.captcha_pic_); break;
        case 3203: fix_new_verify_req(b.verify_req_); break;
        case 3204: fix_new_verify_res(b.captcha_res_); break;
        default:
            // Unknown selector: the callees must not touch the union at all.
            memset(&b, 0x5a, sizeof(b));
            break;
    }
}

// ---------------------------------------------------------------------------
// old captcha protocol fixtures
// ---------------------------------------------------------------------------

void fix_old_head(TSSCaptchaOldHead &h) {
    memset(&h, 0, sizeof(h));
    static const uint16_t kIds[] = {1, 3073, 3075, 3076, 3078, 3090, 3091, 3092};
    h.msg_id_ = kIds[rnd() % 8];
    h.version_ = (uint16_t)(rnd() % 4);
    h.uin_ = rnd();
    h.client_ip_ = rnd();
    h.seq_ = rnd();
    h.send_time_ = rnd();
    h.head_len_ = (uint16_t)(rnd() % 64);
    h.body_len_ = (uint16_t)(rnd() % 4096);
}

void fix_captcha_req(CaptchaReq &r) {
    memset(&r, 0, sizeof(r));
    r.app_id_ = rnd();
    r.antibot_bus_id_ = rnd();
    r.zone_id_ = rnd();
    r.max_pic_change_num_ = (uint16_t)(rnd() % 16);
    r.max_verify_num_ = (uint16_t)(rnd() % 16);
    r.op_type_ = rnd() % 8;
    r.timeout_ = rnd();
}

void fix_old_verify_req(VerifyReq &r) {
    memset(&r, 0, sizeof(r));
    unsigned n = rnd() % 65;
    r.code_len_ = (uint8_t)n;
    fill_text_u(r.code_, n);
    unsigned m = rnd() % 201;
    r.session_len_ = (uint16_t)m;
    fill_text_u(r.session_, m);
}

void fix_old_verify_res(VerifyRes &r) {
    memset(&r, 0, sizeof(r));
    r.app_id_ = rnd();
    r.antibot_bus_id_ = rnd();
    r.zone_id_ = rnd();
    r.result_ = (uint8_t)(rnd() % 4);
    unsigned n = rnd() % 129;
    r.msg_len_ = (uint8_t)n;
    fill_text(r.msg_, n);
}

void fix_s2c_verify_res(S2CVerifyRes &r) {
    memset(&r, 0, sizeof(r));
    r.result_ = (uint8_t)(rnd() % 4);
    unsigned n = rnd() % 129;
    r.msg_len_ = (uint8_t)n;
    fill_text(r.msg_, n);
}

void fix_report_log(ReportLog &r) {
    memset(&r, 0, sizeof(r));
    r.log_src_ = (uint16_t)(rnd() % 8);
    r.log_type_ = (uint16_t)(rnd() % 8);
    unsigned n = rnd() % 513;
    r.log_len_ = (uint16_t)n;
    fill_text(r.log_msg_, n);
}

void fix_busi_req(BusiCaptchaReq &r) {
    memset(&r, 0, sizeof(r));
    r.app_id_ = rnd();
    r.antibot_bus_id_ = rnd();
    r.zone_id_ = rnd();
    r.max_pic_change_num_ = (uint16_t)(rnd() % 16);
    r.max_verify_num_ = (uint16_t)(rnd() % 16);
    r.op_type_ = rnd() % 8;
    r.timeout_ = rnd();
    r.busi_id_ = (uint8_t)(rnd() % 8);
    unsigned n = rnd() % 513;
    r.busi_info_len_ = (uint16_t)n;
    fill_text(r.busi_info_, n);
}

void fix_busi_finish(CaptchaBusiFinish &r) {
    memset(&r, 0, sizeof(r));
    r.app_id_ = rnd();
    r.antibot_bus_id_ = rnd();
    r.zone_id_ = rnd();
    r.busi_id_ = (uint8_t)(rnd() % 8);
    unsigned n = rnd() % 513;
    r.busi_info_len_ = (uint16_t)n;
    fill_text(r.busi_info_, n);
}

void fix_busi_pic(BusiCaptchaPic &p) {
    memset(&p, 0, sizeof(p));
    p.timeout_ = rnd();
    p.verify_times_limit_ = (uint16_t)(rnd() % 32);
    p.verify_times_ = (uint16_t)(rnd() % 32);
    p.max_change_times_ = (uint16_t)(rnd() % 32);
    p.change_times_ = (uint16_t)(rnd() % 32);
    unsigned n = rnd() % 4097;
    p.picture_len_ = (uint16_t)n;
    fill_random(p.picture_, n);
    unsigned m = rnd() % 201;
    p.session_len_ = (uint16_t)m;
    fill_text_u(p.session_, m);
    p.busi_id_ = (uint8_t)(rnd() % 8);
    unsigned k = rnd() % 513;
    p.busi_info_len_ = (uint16_t)k;
    fill_text(p.busi_info_, k);
}

void fix_old_pkg(TSSCaptchaOldPkg &p) {
    memset(&p, 0, sizeof(p));
    fix_old_head(p.head_);
    switch (p.head_.msg_id_) {
        case 1: fix_busi_req(p.body_.busi_captcha_req_); break;
        case 3073: fix_captcha_req(p.body_.captcha_req_); break;
        case 3075: fix_old_verify_req(p.body_.verify_req_); break;
        case 3076: fix_old_verify_res(p.body_.verify_res_); break;
        case 3078: fix_s2c_verify_res(p.body_.s2c_verify_res_); break;
        case 3090: fix_busi_pic(p.body_.busi_captcha_pic_); break;
        case 3091: fix_report_log(p.body_.report_log_); break;
        default: fix_busi_finish(p.body_.captcha_busi_finish_); break;
    }
}

void fix_old_body(TSSCaptchaOldBody &b, int64_t sel) {
    memset(&b, 0, sizeof(b));
    switch (sel) {
        case 1: fix_busi_req(b.busi_captcha_req_); break;
        case 3073: fix_captcha_req(b.captcha_req_); break;
        case 3075: fix_old_verify_req(b.verify_req_); break;
        case 3076: fix_old_verify_res(b.verify_res_); break;
        case 3078: fix_s2c_verify_res(b.s2c_verify_res_); break;
        case 3090: fix_busi_pic(b.busi_captcha_pic_); break;
        case 3091: fix_report_log(b.report_log_); break;
        case 3092: fix_busi_finish(b.captcha_busi_finish_); break;
        default:
            memset(&b, 0x5a, sizeof(b));
            break;
    }
}

// ---------------------------------------------------------------------------
// safe-mode transfer adapt fixtures
// ---------------------------------------------------------------------------

void fix_login(ReqSafemodeTransAdaptLoginNotify &r) {
    memset(&r, 0, sizeof(r));
    r.world_id_ = (uint16_t)(rnd() % 64);
    r.src_id_ = rnd();
    r.client_ip_ = rnd();
    r.login_time_ = rnd();
}

void fix_stat(RspSafemodeTransAdaptStatUpdate &r) {
    memset(&r, 0, sizeof(r));
    r.world_id_ = (uint16_t)(rnd() % 64);
    r.src_id_ = rnd();
    r.stat_level_ = (uint8_t)(rnd() % 16);
}

void fix_event(ReqSafemodeTransAdaptEventNotify &r) {
    memset(&r, 0, sizeof(r));
    r.world_id_ = (uint16_t)(rnd() % 64);
    r.src_id_ = rnd();
    r.event_type_ = (uint16_t)(rnd() % 16);
    r.event_value_ = rnd();
}

void fix_trade(ReqSafemodeTransAdaptTradeNotify &r) {
    memset(&r, 0, sizeof(r));
    r.world_id_ = (uint16_t)(rnd() % 64);
    r.src_id_ = rnd();
    r.type_ = (uint8_t)(rnd() % 8);
    r.send_uin_ = rnd();
    r.target_uin_ = rnd();
    r.send_ip_ = rnd();
    r.target_ip_ = rnd();
    unsigned n = rnd() % 2049;
    r.buf_len_ = (uint16_t)n;
    fill_text_u(r.trade_buf_, n);
}

void fix_forward(ReqSafemodeTransAdaptDataForward &r) {
    memset(&r, 0, sizeof(r));
    r.world_id_ = (uint16_t)(rnd() % 64);
    r.src_id_ = rnd();
    unsigned n = rnd() % 2049;
    r.data_len_ = (uint16_t)n;
    fill_text_u(r.data_, n);
}

// ---------------------------------------------------------------------------
// drivers
// ---------------------------------------------------------------------------

// Core-layer pass (defined after the char* drivers).  The ORIG library is
// dlopen()ed RTLD_GLOBAL, so a RTLD_LOCAL reconstructed object's *internal*
// calls - e.g. the char* wrapper's call to pack(TdrWriteBuf&) - are interposed
// by the ORIG definition (the global scope is searched first).  The core pass
// calls the buffer-form entry points through dlsym(recon, ...) instead, which
// searches the object's own scope first and therefore reaches the
// reconstructed bodies; see the block comment at its definition.
template <typename T>
void run_core(const char *label, void *orig, void *recon, const char *pk_ch,
              const char *up_ch, const char *vis_ch, void (*fix)(T &));
template <typename B>
void run_core_body(const char *label, void *orig, void *recon,
                   const char *pk_ch, const char *up_ch, const char *vis_ch,
                   const int64_t *sels, unsigned nsel,
                   void (*fix)(B &, int64_t));

template <typename T>
void run_plain(const char *label, void *orig, void *recon, const char *pk_sym,
               const char *up_sym, const char *vis_sym, void (*fix)(T &)) {
    pack_t pk_o = reinterpret_cast<pack_t>(dlsym(orig, pk_sym));
    pack_t pk_r = reinterpret_cast<pack_t>(dlsym(recon, pk_sym));
    unpack_t up_o = reinterpret_cast<unpack_t>(dlsym(orig, up_sym));
    unpack_t up_r = reinterpret_cast<unpack_t>(dlsym(recon, up_sym));
    vis_t vi_o = reinterpret_cast<vis_t>(dlsym(orig, vis_sym));
    vis_t vi_r = reinterpret_cast<vis_t>(dlsym(recon, vis_sym));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s sym\n", label);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    static const unsigned kCutVer[3] = {1, 0, 3};
    for (unsigned it = 0; it < kIters; ++it) {
        memset(src, 0, sizeof(T));
        memset(uo, 0, sizeof(T));
        memset(ur, 0, sizeof(T));
        fix(*src);
        for (unsigned cv = 0; cv < 3; ++cv) {
            unsigned cut = kCutVer[cv];
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            unsigned uuo = 0, uur = 0;
            int ro = pk_o(src, bo, kCap, &uuo, cut);
            int rr = pk_r(src, br, kCap, &uur, cut);
            check(label, "pack", ro, rr, uuo, uur, bo, br, kCap);
            if (ro != 0 || rr != 0) continue;
            memset(uo, 0, sizeof(T));
            memset(ur, 0, sizeof(T));
            unsigned uu2 = 0, uu3 = 0;
            int ro2 = up_o(uo, bo, uuo, &uu2, cut);
            int rr2 = up_r(ur, bo, uuo, &uu3, cut);
            check(label, "unpack", ro2, rr2, uu2, uu3,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));
            if (cv == 0) {
                const unsigned cuts[3] = {uuo ? uuo - 1 : 0, uuo / 2, 0};
                for (unsigned t = 0; t < 3; ++t) {
                    memset(uo, 0, sizeof(T));
                    memset(ur, 0, sizeof(T));
                    uu2 = 0;
                    uu3 = 0;
                    ro2 = up_o(uo, bo, cuts[t], &uu2, cut);
                    rr2 = up_r(ur, bo, cuts[t], &uu3, cut);
                    check(label, "unpack-trunc", ro2, rr2, uu2, uu3,
                          reinterpret_cast<const char *>(uo),
                          reinterpret_cast<const char *>(ur), sizeof(T));
                }
            }
            memset(vo, 0, kCap);
            memset(vr, 0, kCap);
            unsigned uvo = 0, uvr = 0;
            int rvo = vi_o(src, vo, kCap, &uvo, 2, '\n');
            int rvr = vi_r(src, vr, kCap, &uvr, 2, '\n');
            check(label, "visualize", rvo, rvr, uvo, uvr, vo, vr, kCap);
            memset(vo, 0, 16);
            memset(vr, 0, 16);
            uvo = 0;
            uvr = 0;
            rvo = vi_o(src, vo, 8, &uvo, 2, '\n');
            rvr = vi_r(src, vr, 8, &uvr, 2, '\n');
            check(label, "vis-small", rvo, rvr, uvo, uvr, vo, vr, 8);
        }
    }
    run_core<T>(label, orig, recon, pk_sym, up_sym, vis_sym, fix);
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

template <typename B>
void run_body(const char *label, void *orig, void *recon, const char *pk_sym,
              const char *up_sym, const char *vis_sym, const int64_t *sels,
              unsigned nsel, void (*fix)(B &, int64_t)) {
    pack_sel_t pk_o = reinterpret_cast<pack_sel_t>(dlsym(orig, pk_sym));
    pack_sel_t pk_r = reinterpret_cast<pack_sel_t>(dlsym(recon, pk_sym));
    unpack_sel_t up_o = reinterpret_cast<unpack_sel_t>(dlsym(orig, up_sym));
    unpack_sel_t up_r = reinterpret_cast<unpack_sel_t>(dlsym(recon, up_sym));
    vis_sel_t vi_o = reinterpret_cast<vis_sel_t>(dlsym(orig, vis_sym));
    vis_sel_t vi_r = reinterpret_cast<vis_sel_t>(dlsym(recon, vis_sym));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s sym\n", label);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    B *src = new B;
    B *uo = new B;
    B *ur = new B;
    for (unsigned it = 0; it < kIters; ++it) {
        for (unsigned s = 0; s < nsel; ++s) {
            int64_t sel = sels[s];
            memset(src, 0, sizeof(B));
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            fix(*src, sel);
            unsigned uuo = 0, uur = 0;
            int ro = pk_o(src, sel, bo, kCap, &uuo, 1);
            int rr = pk_r(src, sel, br, kCap, &uur, 1);
            check(label, "pack", ro, rr, uuo, uur, bo, br, kCap);
            if (ro == 0 && rr == 0) {
                memset(uo, 0, sizeof(B));
                memset(ur, 0, sizeof(B));
                unsigned uu2 = 0, uu3 = 0;
                int ro2 = up_o(uo, sel, bo, uuo, &uu2, 1);
                int rr2 = up_r(ur, sel, bo, uuo, &uu3, 1);
                check(label, "unpack", ro2, rr2, uu2, uu3,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(B));
                memset(uo, 0, sizeof(B));
                memset(ur, 0, sizeof(B));
                uu2 = 0;
                uu3 = 0;
                int ro3 = up_o(uo, sel, bo, uuo ? uuo - 1 : 0, &uu2, 1);
                int rr3 = up_r(ur, sel, bo, uuo ? uuo - 1 : 0, &uu3, 1);
                check(label, "unpack-trunc", ro3, rr3, uu2, uu3,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(B));
            }
            // indent 2 and the negative-indent branch (-1) of the union nest.
            static const int kIndent[2] = {2, -1};
            for (unsigned ii = 0; ii < 2; ++ii) {
                memset(vo, 0, kCap);
                memset(vr, 0, kCap);
                unsigned uvo = 0, uvr = 0;
                int rvo = vi_o(src, sel, vo, kCap, &uvo, kIndent[ii], '\n');
                int rvr = vi_r(src, sel, vr, kCap, &uvr, kIndent[ii], '\n');
                check(label, ii ? "vis-indent-1" : "visualize", rvo, rvr, uvo,
                      uvr, vo, vr, kCap);
            }
        }
    }
    run_core_body<B>(label, orig, recon, pk_sym, up_sym, vis_sym, sels, nsel,
                     fix);
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

// ---------------------------------------------------------------------------
// Core-layer differential pass (buffer-form entry points)
// ---------------------------------------------------------------------------

std::string core_sym(const char *sym, const char *method, const char *plain,
                     const char *sel) {
    const char *p = strstr(sym, method);
    if (!p) return std::string();
    size_t head = (size_t)(p - sym) + strlen(method);
    bool with_sel = sym[head] == 'E' && sym[head + 1] == 'x';
    std::string out(sym, head);
    out += with_sel ? sel : plain;
    return out;
}

typedef int (*pack_buf_t)(const void *, tsf4g_tdr::TdrWriteBuf &, unsigned);
typedef int (*unpack_buf_t)(void *, tsf4g_tdr::TdrReadBuf &, unsigned);
typedef int (*vis_buf_t)(const void *, tsf4g_tdr::TdrWriteBuf &, int, char);
typedef int (*pack_bufsel_t)(const void *, int64_t,
                             tsf4g_tdr::TdrWriteBuf &, unsigned);
typedef int (*unpack_bufsel_t)(void *, int64_t, tsf4g_tdr::TdrReadBuf &,
                               unsigned);
typedef int (*vis_bufsel_t)(const void *, int64_t,
                            tsf4g_tdr::TdrWriteBuf &, int, char);

template <typename T>
void run_core(const char *label, void *orig, void *recon, const char *pk_ch,
              const char *up_ch, const char *vis_ch, void (*fix)(T &)) {
    std::string pk_s = core_sym(pk_ch, "4pack", "ERN9tsf4g_tdr11TdrWriteBufEj",
                                "ExRN9tsf4g_tdr11TdrWriteBufEj");
    std::string up_s =
        core_sym(up_ch, "6unpack", "ERN9tsf4g_tdr10TdrReadBufEj",
                 "ExRN9tsf4g_tdr10TdrReadBufEj");
    std::string vi_s =
        core_sym(vis_ch, "9visualize", "ERN9tsf4g_tdr11TdrWriteBufEic",
                 "ExRN9tsf4g_tdr11TdrWriteBufEic");
    pack_buf_t pk_o = reinterpret_cast<pack_buf_t>(dlsym(orig, pk_s.c_str()));
    pack_buf_t pk_r = reinterpret_cast<pack_buf_t>(dlsym(recon, pk_s.c_str()));
    unpack_buf_t up_o =
        reinterpret_cast<unpack_buf_t>(dlsym(orig, up_s.c_str()));
    unpack_buf_t up_r =
        reinterpret_cast<unpack_buf_t>(dlsym(recon, up_s.c_str()));
    vis_buf_t vi_o = reinterpret_cast<vis_buf_t>(dlsym(orig, vi_s.c_str()));
    vis_buf_t vi_r = reinterpret_cast<vis_buf_t>(dlsym(recon, vi_s.c_str()));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s core sym %s\n", label, pk_s.c_str());
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    static const unsigned kCutVer[3] = {1, 0, 3};
    for (unsigned it = 0; it < kIters; ++it) {
        memset(src, 0, sizeof(T));
        fix(*src);
        for (unsigned cv = 0; cv < 3; ++cv) {
            unsigned cut = kCutVer[cv];
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            tsf4g_tdr::TdrWriteBuf wo(bo, kCap);
            tsf4g_tdr::TdrWriteBuf wr(br, kCap);
            int ro = pk_o(src, wo, cut);
            int rr = pk_r(src, wr, cut);
            unsigned uuo = (unsigned)wo.getUsedSize();
            unsigned uur = (unsigned)wr.getUsedSize();
            check(label, "pack-core", ro, rr, uuo, uur, bo, br, kCap);
            if (ro == 0 && rr == 0) {
                memset(uo, 0, sizeof(T));
                memset(ur, 0, sizeof(T));
                tsf4g_tdr::TdrReadBuf rdo(bo, uuo);
                tsf4g_tdr::TdrReadBuf rdr(bo, uuo);
                int ro2 = up_o(uo, rdo, cut);
                int rr2 = up_r(ur, rdr, cut);
                check(label, "unpack-core", ro2, rr2,
                      (unsigned)rdo.getUsedSize(), (unsigned)rdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
                memset(uo, 0, sizeof(T));
                memset(ur, 0, sizeof(T));
                unsigned tlen = uuo ? uuo - 1 : 0;
                tsf4g_tdr::TdrReadBuf tdo(bo, tlen);
                tsf4g_tdr::TdrReadBuf tdr(bo, tlen);
                ro2 = up_o(uo, tdo, cut);
                rr2 = up_r(ur, tdr, cut);
                check(label, "unpack-core-trunc", ro2, rr2,
                      (unsigned)tdo.getUsedSize(), (unsigned)tdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
            }
            memset(vo, 0, kCap);
            memset(vr, 0, kCap);
            tsf4g_tdr::TdrWriteBuf wvo(vo, kCap);
            tsf4g_tdr::TdrWriteBuf wvr(vr, kCap);
            int rvo = vi_o(src, wvo, 2, '\n');
            int rvr = vi_r(src, wvr, 2, '\n');
            check(label, "visualize-core", rvo, rvr,
                  (unsigned)wvo.getUsedSize(), (unsigned)wvr.getUsedSize(), vo,
                  vr, kCap);
            memset(vo, 0, 16);
            memset(vr, 0, 16);
            tsf4g_tdr::TdrWriteBuf svo(vo, 8);
            tsf4g_tdr::TdrWriteBuf svr(vr, 8);
            rvo = vi_o(src, svo, 2, '\n');
            rvr = vi_r(src, svr, 2, '\n');
            check(label, "vis-core-small", rvo, rvr,
                  (unsigned)svo.getUsedSize(), (unsigned)svr.getUsedSize(), vo,
                  vr, 8);
        }
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

template <typename B>
void run_core_body(const char *label, void *orig, void *recon, const char *pk_ch,
                   const char *up_ch, const char *vis_ch, const int64_t *sels,
                   unsigned nsel, void (*fix)(B &, int64_t)) {
    std::string pk_s = core_sym(pk_ch, "4pack", "ERN9tsf4g_tdr11TdrWriteBufEj",
                                "ExRN9tsf4g_tdr11TdrWriteBufEj");
    std::string up_s =
        core_sym(up_ch, "6unpack", "ERN9tsf4g_tdr10TdrReadBufEj",
                 "ExRN9tsf4g_tdr10TdrReadBufEj");
    std::string vi_s =
        core_sym(vis_ch, "9visualize", "ERN9tsf4g_tdr11TdrWriteBufEic",
                 "ExRN9tsf4g_tdr11TdrWriteBufEic");
    pack_bufsel_t pk_o =
        reinterpret_cast<pack_bufsel_t>(dlsym(orig, pk_s.c_str()));
    pack_bufsel_t pk_r =
        reinterpret_cast<pack_bufsel_t>(dlsym(recon, pk_s.c_str()));
    unpack_bufsel_t up_o =
        reinterpret_cast<unpack_bufsel_t>(dlsym(orig, up_s.c_str()));
    unpack_bufsel_t up_r =
        reinterpret_cast<unpack_bufsel_t>(dlsym(recon, up_s.c_str()));
    vis_bufsel_t vi_o =
        reinterpret_cast<vis_bufsel_t>(dlsym(orig, vi_s.c_str()));
    vis_bufsel_t vi_r =
        reinterpret_cast<vis_bufsel_t>(dlsym(recon, vi_s.c_str()));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s core sel sym %s\n", label, pk_s.c_str());
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    B *src = new B;
    B *uo = new B;
    B *ur = new B;
    for (unsigned it = 0; it < kIters; ++it) {
        for (unsigned s = 0; s < nsel; ++s) {
            int64_t sel = sels[s];
            memset(src, 0, sizeof(B));
            fix(*src, sel);
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            tsf4g_tdr::TdrWriteBuf wo(bo, kCap);
            tsf4g_tdr::TdrWriteBuf wr(br, kCap);
            int ro = pk_o(src, sel, wo, 1);
            int rr = pk_r(src, sel, wr, 1);
            unsigned uuo = (unsigned)wo.getUsedSize();
            unsigned uur = (unsigned)wr.getUsedSize();
            check(label, "pack-core", ro, rr, uuo, uur, bo, br, kCap);
            if (ro == 0 && rr == 0) {
                memset(uo, 0, sizeof(B));
                memset(ur, 0, sizeof(B));
                tsf4g_tdr::TdrReadBuf rdo(bo, uuo);
                tsf4g_tdr::TdrReadBuf rdr(bo, uuo);
                int ro2 = up_o(uo, sel, rdo, 1);
                int rr2 = up_r(ur, sel, rdr, 1);
                check(label, "unpack-core", ro2, rr2,
                      (unsigned)rdo.getUsedSize(), (unsigned)rdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(B));
                memset(uo, 0, sizeof(B));
                memset(ur, 0, sizeof(B));
                unsigned tlen = uuo ? uuo - 1 : 0;
                tsf4g_tdr::TdrReadBuf tdo(bo, tlen);
                tsf4g_tdr::TdrReadBuf tdr(bo, tlen);
                ro2 = up_o(uo, sel, tdo, 1);
                rr2 = up_r(ur, sel, tdr, 1);
                check(label, "unpack-core-trunc", ro2, rr2,
                      (unsigned)tdo.getUsedSize(), (unsigned)tdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(B));
            }
            static const int kIndent[2] = {2, -1};
            for (unsigned ii = 0; ii < 2; ++ii) {
                memset(vo, 0, kCap);
                memset(vr, 0, kCap);
                tsf4g_tdr::TdrWriteBuf wvo(vo, kCap);
                tsf4g_tdr::TdrWriteBuf wvr(vr, kCap);
                int rvo = vi_o(src, sel, wvo, kIndent[ii], '\n');
                int rvr = vi_r(src, sel, wvr, kIndent[ii], '\n');
                check(label, ii ? "vis-core-indent-1" : "visualize-core", rvo,
                      rvr, (unsigned)wvo.getUsedSize(),
                      (unsigned)wvr.getUsedSize(), vo, vr, kCap);
            }
        }
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

}  // namespace

int main() {
    void *orig = dlopen(kOrigPath, RTLD_NOW | RTLD_GLOBAL);
    if (!orig) {
        printf("dlopen ORIG failed: %s\n", dlerror());
        return 2;
    }
    void *recon = dlopen(kReconPath, RTLD_NOW | RTLD_LOCAL);
    if (!recon) {
        printf("dlopen recon failed: %s\n", dlerror());
        return 2;
    }
    printf("loaded; sizeof new(head=%u set=%u pic=%u req=%u res=%u body=%u "
           "pkg=%u) old(head=%u req=%u vreq=%u vres=%u s2c=%u log=%u breq=%u "
           "bfin=%u bpic=%u body=%u pkg=%u) safe(login=%u stat=%u event=%u "
           "trade=%u fwd=%u)\n",
           (unsigned)sizeof(TSSCaptchaHead), (unsigned)sizeof(TSSSetCaptcha),
           (unsigned)sizeof(TSSCaptchaPic), (unsigned)sizeof(TSSVerifyReq),
           (unsigned)sizeof(TSSVerifyRes), (unsigned)sizeof(TSSCaptchaBody),
           (unsigned)sizeof(TSSCaptchaPkg),
           (unsigned)sizeof(TSSCaptchaOldHead), (unsigned)sizeof(CaptchaReq),
           (unsigned)sizeof(VerifyReq), (unsigned)sizeof(VerifyRes),
           (unsigned)sizeof(S2CVerifyRes), (unsigned)sizeof(ReportLog),
           (unsigned)sizeof(BusiCaptchaReq), (unsigned)sizeof(CaptchaBusiFinish),
           (unsigned)sizeof(BusiCaptchaPic), (unsigned)sizeof(TSSCaptchaOldBody),
           (unsigned)sizeof(TSSCaptchaOldPkg),
           (unsigned)sizeof(ReqSafemodeTransAdaptLoginNotify),
           (unsigned)sizeof(RspSafemodeTransAdaptStatUpdate),
           (unsigned)sizeof(ReqSafemodeTransAdaptEventNotify),
           (unsigned)sizeof(ReqSafemodeTransAdaptTradeNotify),
           (unsigned)sizeof(ReqSafemodeTransAdaptDataForward));

    // ---- new captcha protocol (formmog_proto_captcha_client.cpp) ----
    run_plain<TSSCaptchaHead>(
        "NewHead", orig, recon,
        "_ZNK9sec_proto14TSSCaptchaHead4packEPcjPjj",
        "_ZN9sec_proto14TSSCaptchaHead6unpackEPKcjPjj",
        "_ZNK9sec_proto14TSSCaptchaHead9visualizeEPcjPjic", fix_new_head);
    run_plain<TSSSetCaptcha>(
        "NewSetCaptcha", orig, recon,
        "_ZNK9sec_proto13TSSSetCaptcha4packEPcjPjj",
        "_ZN9sec_proto13TSSSetCaptcha6unpackEPKcjPjj",
        "_ZNK9sec_proto13TSSSetCaptcha9visualizeEPcjPjic", fix_set_captcha);
    run_plain<TSSCaptchaPic>(
        "NewCaptchaPic", orig, recon,
        "_ZNK9sec_proto13TSSCaptchaPic4packEPcjPjj",
        "_ZN9sec_proto13TSSCaptchaPic6unpackEPKcjPjj",
        "_ZNK9sec_proto13TSSCaptchaPic9visualizeEPcjPjic", fix_captcha_pic);
    run_plain<TSSVerifyReq>(
        "NewVerifyReq", orig, recon,
        "_ZNK9sec_proto12TSSVerifyReq4packEPcjPjj",
        "_ZN9sec_proto12TSSVerifyReq6unpackEPKcjPjj",
        "_ZNK9sec_proto12TSSVerifyReq9visualizeEPcjPjic", fix_new_verify_req);
    run_plain<TSSVerifyRes>(
        "NewVerifyRes", orig, recon,
        "_ZNK9sec_proto12TSSVerifyRes4packEPcjPjj",
        "_ZN9sec_proto12TSSVerifyRes6unpackEPKcjPjj",
        "_ZNK9sec_proto12TSSVerifyRes9visualizeEPcjPjic", fix_new_verify_res);
    {
        static const int64_t kNewSels[] = {3201, 3202, 3203, 3204,
                                           0,    3200, 3205, 9001};
        run_body<TSSCaptchaBody>(
            "NewBody", orig, recon,
            "_ZNK9sec_proto14TSSCaptchaBody4packExPcjPjj",
            "_ZN9sec_proto14TSSCaptchaBody6unpackExPKcjPjj",
            "_ZNK9sec_proto14TSSCaptchaBody9visualizeExPcjPjic", kNewSels,
            sizeof(kNewSels) / sizeof(kNewSels[0]), fix_new_body);
    }
    run_plain<TSSCaptchaPkg>(
        "NewPkg", orig, recon,
        "_ZNK9sec_proto13TSSCaptchaPkg4packEPcjPjj",
        "_ZN9sec_proto13TSSCaptchaPkg6unpackEPKcjPjj",
        "_ZNK9sec_proto13TSSCaptchaPkg9visualizeEPcjPjic", fix_new_pkg);

    // ---- old captcha protocol (formmog_proto_captcha_client_old.cpp) ----
    run_plain<TSSCaptchaOldHead>(
        "OldHead", orig, recon,
        "_ZNK9sec_proto17TSSCaptchaOldHead4packEPcjPjj",
        "_ZN9sec_proto17TSSCaptchaOldHead6unpackEPKcjPjj",
        "_ZNK9sec_proto17TSSCaptchaOldHead9visualizeEPcjPjic", fix_old_head);
    run_plain<CaptchaReq>(
        "OldCaptchaReq", orig, recon,
        "_ZNK9sec_proto10CaptchaReq4packEPcjPjj",
        "_ZN9sec_proto10CaptchaReq6unpackEPKcjPjj",
        "_ZNK9sec_proto10CaptchaReq9visualizeEPcjPjic", fix_captcha_req);
    run_plain<VerifyReq>(
        "OldVerifyReq", orig, recon,
        "_ZNK9sec_proto9VerifyReq4packEPcjPjj",
        "_ZN9sec_proto9VerifyReq6unpackEPKcjPjj",
        "_ZNK9sec_proto9VerifyReq9visualizeEPcjPjic", fix_old_verify_req);
    run_plain<VerifyRes>(
        "OldVerifyRes", orig, recon,
        "_ZNK9sec_proto9VerifyRes4packEPcjPjj",
        "_ZN9sec_proto9VerifyRes6unpackEPKcjPjj",
        "_ZNK9sec_proto9VerifyRes9visualizeEPcjPjic", fix_old_verify_res);
    run_plain<S2CVerifyRes>(
        "OldS2CVerifyRes", orig, recon,
        "_ZNK9sec_proto12S2CVerifyRes4packEPcjPjj",
        "_ZN9sec_proto12S2CVerifyRes6unpackEPKcjPjj",
        "_ZNK9sec_proto12S2CVerifyRes9visualizeEPcjPjic", fix_s2c_verify_res);
    run_plain<ReportLog>(
        "OldReportLog", orig, recon,
        "_ZNK9sec_proto9ReportLog4packEPcjPjj",
        "_ZN9sec_proto9ReportLog6unpackEPKcjPjj",
        "_ZNK9sec_proto9ReportLog9visualizeEPcjPjic", fix_report_log);
    run_plain<BusiCaptchaReq>(
        "OldBusiCaptchaReq", orig, recon,
        "_ZNK9sec_proto14BusiCaptchaReq4packEPcjPjj",
        "_ZN9sec_proto14BusiCaptchaReq6unpackEPKcjPjj",
        "_ZNK9sec_proto14BusiCaptchaReq9visualizeEPcjPjic", fix_busi_req);
    run_plain<CaptchaBusiFinish>(
        "OldCaptchaBusiFinish", orig, recon,
        "_ZNK9sec_proto17CaptchaBusiFinish4packEPcjPjj",
        "_ZN9sec_proto17CaptchaBusiFinish6unpackEPKcjPjj",
        "_ZNK9sec_proto17CaptchaBusiFinish9visualizeEPcjPjic", fix_busi_finish);
    run_plain<BusiCaptchaPic>(
        "OldBusiCaptchaPic", orig, recon,
        "_ZNK9sec_proto14BusiCaptchaPic4packEPcjPjj",
        "_ZN9sec_proto14BusiCaptchaPic6unpackEPKcjPjj",
        "_ZNK9sec_proto14BusiCaptchaPic9visualizeEPcjPjic", fix_busi_pic);
    {
        static const int64_t kOldSels[] = {1,    3073, 3075, 3076, 3078, 3090,
                                           3091, 3092, 0,    2,    3074, 3093};
        run_body<TSSCaptchaOldBody>(
            "OldBody", orig, recon,
            "_ZNK9sec_proto17TSSCaptchaOldBody4packExPcjPjj",
            "_ZN9sec_proto17TSSCaptchaOldBody6unpackExPKcjPjj",
            "_ZNK9sec_proto17TSSCaptchaOldBody9visualizeExPcjPjic", kOldSels,
            sizeof(kOldSels) / sizeof(kOldSels[0]), fix_old_body);
    }
    run_plain<TSSCaptchaOldPkg>(
        "OldPkg", orig, recon,
        "_ZNK9sec_proto16TSSCaptchaOldPkg4packEPcjPjj",
        "_ZN9sec_proto16TSSCaptchaOldPkg6unpackEPKcjPjj",
        "_ZNK9sec_proto16TSSCaptchaOldPkg9visualizeEPcjPjic", fix_old_pkg);

    // ---- safe-mode transfer adapt ----
    run_plain<ReqSafemodeTransAdaptLoginNotify>(
        "SafeLogin", orig, recon,
        "_ZNK9sec_proto32ReqSafemodeTransAdaptLoginNotify4packEPcjPjj",
        "_ZN9sec_proto32ReqSafemodeTransAdaptLoginNotify6unpackEPKcjPjj",
        "_ZNK9sec_proto32ReqSafemodeTransAdaptLoginNotify9visualizeEPcjPjic",
        fix_login);
    run_plain<RspSafemodeTransAdaptStatUpdate>(
        "SafeStat", orig, recon,
        "_ZNK9sec_proto31RspSafemodeTransAdaptStatUpdate4packEPcjPjj",
        "_ZN9sec_proto31RspSafemodeTransAdaptStatUpdate6unpackEPKcjPjj",
        "_ZNK9sec_proto31RspSafemodeTransAdaptStatUpdate9visualizeEPcjPjic",
        fix_stat);
    run_plain<ReqSafemodeTransAdaptEventNotify>(
        "SafeEvent", orig, recon,
        "_ZNK9sec_proto32ReqSafemodeTransAdaptEventNotify4packEPcjPjj",
        "_ZN9sec_proto32ReqSafemodeTransAdaptEventNotify6unpackEPKcjPjj",
        "_ZNK9sec_proto32ReqSafemodeTransAdaptEventNotify9visualizeEPcjPjic",
        fix_event);
    run_plain<ReqSafemodeTransAdaptTradeNotify>(
        "SafeTrade", orig, recon,
        "_ZNK9sec_proto32ReqSafemodeTransAdaptTradeNotify4packEPcjPjj",
        "_ZN9sec_proto32ReqSafemodeTransAdaptTradeNotify6unpackEPKcjPjj",
        "_ZNK9sec_proto32ReqSafemodeTransAdaptTradeNotify9visualizeEPcjPjic",
        fix_trade);
    run_plain<ReqSafemodeTransAdaptDataForward>(
        "SafeForward", orig, recon,
        "_ZNK9sec_proto32ReqSafemodeTransAdaptDataForward4packEPcjPjj",
        "_ZN9sec_proto32ReqSafemodeTransAdaptDataForward6unpackEPKcjPjj",
        "_ZNK9sec_proto32ReqSafemodeTransAdaptDataForward9visualizeEPcjPjic",
        fix_forward);

    printf("cases=%d failures=%d\n", g_cases, g_fail);
    return g_fail ? 1 : 0;
}
