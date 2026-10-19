// Differential oracle: reconstructed formmog_proto_tss_sdk_uic.cpp vs ORIG.
// ORIG is dlopen()ed RTLD_GLOBAL so the reconstructed object's TdrBufUtil
// references resolve to the ORIG runtime; the reconstructed object is
// dlopen()ed RTLD_LOCAL and called through dlsym.
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "formmog_proto_tss_sdk_uic.h"

using namespace sec_proto;

namespace {

const char *kOrigPath =
    "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
const char *kReconPath = "/tmp/libuic_recon.so";

const unsigned kCap = 65536;
const unsigned kIters = 200;
int g_cases = 0;
int g_fail = 0;

uint32_t g_rng = 0x12345678u;
uint32_t rnd() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng >> 8;
}

void fill_random(void *p, size_t n) {
    unsigned char *b = static_cast<unsigned char *>(p);
    for (size_t i = 0; i < n; ++i) b[i] = (unsigned char)(rnd() & 0xff);
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
        printf("FAIL %-22s %-8s ret %d/%d used %u/%u first-diff %u (%02x/%02x)\n",
               label, what, a, b, ua, ub, d, x, y);
    }
}

void fix_req(ReqUserInputCheck &r) {
    memset(&r, 0, sizeof(r));
    r.uin_ = rnd();
    r.session_id_ = rnd();
    r.session_seq_ = rnd();
    r.user_input_code_ = (int8_t)(rnd() & 1);
    unsigned n = rnd() % 1025;
    r.user_input_data_len_ = (uint16_t)n;
    for (unsigned i = 0; i + 1 < n; ++i)
        r.user_input_data_[i] = (uint8_t)(1 + (rnd() % 0x7e));
    if (n) r.user_input_data_[n - 1] = 0;
    unsigned m = rnd() % 129;
    r.user_game_data_len_ = (uint16_t)m;
    for (unsigned i = 0; i + 1 < m; ++i)
        r.user_game_data_[i] = (uint8_t)(1 + (rnd() % 0x7e));
    if (m) r.user_game_data_[m - 1] = 0;
}

void fix_rsp(RspUserInputCheck &r) {
    memset(&r, 0, sizeof(r));
    r.uin_ = rnd();
    r.session_id_ = rnd();
    r.session_seq_ = rnd();
    r.check_result_ = rnd();
}

void fix_head(UserInputCheckPkgHead &h) {
    h.proto_id_ = 0x1000 + (uint16_t)(rnd() % 0x1000);
    h.version_ = (uint16_t)(rnd() % 4);
    h.pkg_len_ = (uint16_t)(rnd() % 2048);
    h.cmd_ = (uint16_t)(rnd() % 4);
    h.game_id_ = (uint16_t)(rnd() % 16);
}

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
    for (unsigned it = 0; it < kIters; ++it) {
        memset(bo, 0, kCap);
        memset(br, 0, kCap);
        memset(vo, 0, kCap);
        memset(vr, 0, kCap);
        memset(src, 0, sizeof(T));
        memset(uo, 0, sizeof(T));
        memset(ur, 0, sizeof(T));
        fix(*src);
        unsigned uuo = 0, uur = 0, uu2 = 0, uu3 = 0;
        int ro = pk_o(src, bo, kCap, &uuo, 1);
        int rr = pk_r(src, br, kCap, &uur, 1);
        check(label, "pack", ro, rr, uuo, uur, bo, br, kCap);
        if (ro == 0 && rr == 0) {
            int ro2 = up_o(uo, bo, uuo, &uu2, 1);
            int rr2 = up_r(ur, bo, uuo, &uu3, 1);
            check(label, "unpack", ro2, rr2, uu2, uu3,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));
        }
        memset(vo, 0, kCap);
        memset(vr, 0, kCap);
        unsigned uvo = 0, uvr = 0;
        int rvo = vi_o(src, vo, kCap, &uvo, 2, '\n');
        int rvr = vi_r(src, vr, kCap, &uvr, 2, '\n');
        check(label, "visualize", rvo, rvr, uvo, uvr, vo, vr, kCap);
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

template <typename T>
void run_body(const char *label, void *orig, void *recon) {
    pack_sel_t pk_o = reinterpret_cast<pack_sel_t>(
        dlsym(orig, "_ZNK9sec_proto21UserInputCheckPkgBody4packExPcjPjj"));
    pack_sel_t pk_r = reinterpret_cast<pack_sel_t>(
        dlsym(recon, "_ZNK9sec_proto21UserInputCheckPkgBody4packExPcjPjj"));
    unpack_sel_t up_o = reinterpret_cast<unpack_sel_t>(
        dlsym(orig, "_ZN9sec_proto21UserInputCheckPkgBody6unpackExPKcjPjj"));
    unpack_sel_t up_r = reinterpret_cast<unpack_sel_t>(
        dlsym(recon, "_ZN9sec_proto21UserInputCheckPkgBody6unpackExPKcjPjj"));
    vis_sel_t vi_o = reinterpret_cast<vis_sel_t>(
        dlsym(orig, "_ZNK9sec_proto21UserInputCheckPkgBody9visualizeExPcjPjic"));
    vis_sel_t vi_r = reinterpret_cast<vis_sel_t>(
        dlsym(recon, "_ZNK9sec_proto21UserInputCheckPkgBody9visualizeExPcjPjic"));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s sym\n", label);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    UserInputCheckPkgBody *src = new UserInputCheckPkgBody;
    UserInputCheckPkgBody *uo = new UserInputCheckPkgBody;
    UserInputCheckPkgBody *ur = new UserInputCheckPkgBody;
    for (unsigned it = 0; it < kIters; ++it) {
        for (int sel = 0; sel <= 1; ++sel) {
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            memset(src, 0, sizeof(*src));
            if (sel == 0) {
                fix_req(src->req_user_input_check_);
            } else {
                src->rsp_user_input_check_.uin_ = rnd();
                src->rsp_user_input_check_.session_id_ = rnd();
                src->rsp_user_input_check_.session_seq_ = rnd();
                src->rsp_user_input_check_.check_result_ = rnd();
            }
            unsigned uuo = 0, uur = 0;
            int ro = pk_o(src, sel, bo, kCap, &uuo, 1);
            int rr = pk_r(src, sel, br, kCap, &uur, 1);
            check(label, sel ? "pack-rsp" : "pack-req", ro, rr, uuo, uur, bo,
                  br, kCap);
            if (ro == 0 && rr == 0) {
                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                unsigned uu2 = 0, uu3 = 0;
                int ro2 = up_o(uo, sel, bo, uuo, &uu2, 1);
                int rr2 = up_r(ur, sel, bo, uuo, &uu3, 1);
                check(label, sel ? "unpack-rsp" : "unpack-req", ro2, rr2, uu2,
                      uu3, reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));
            }
            memset(vo, 0, kCap);
            memset(vr, 0, kCap);
            unsigned uvo = 0, uvr = 0;
            int rvo = vi_o(src, sel, vo, kCap, &uvo, 2, '\n');
            int rvr = vi_r(src, sel, vr, kCap, &uvr, 2, '\n');
            check(label, sel ? "vis-rsp" : "vis-req", rvo, rvr, uvo, uvr, vo,
                  vr, kCap);
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

void fix_pkg(UserInputCheckPkg &p) {
    memset(&p, 0, sizeof(p));
    fix_head(p.head_);
    fix_req(p.body_.req_user_input_check_);
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
    printf("loaded; sizeof head=%u req=%u rsp=%u body=%u pkg=%u\n",
           (unsigned)sizeof(UserInputCheckPkgHead),
           (unsigned)sizeof(ReqUserInputCheck),
           (unsigned)sizeof(RspUserInputCheck),
           (unsigned)sizeof(UserInputCheckPkgBody),
           (unsigned)sizeof(UserInputCheckPkg));

    run_plain<UserInputCheckPkgHead>(
        "Head", orig, recon,
        "_ZNK9sec_proto21UserInputCheckPkgHead4packEPcjPjj",
        "_ZN9sec_proto21UserInputCheckPkgHead6unpackEPKcjPjj",
        "_ZNK9sec_proto21UserInputCheckPkgHead9visualizeEPcjPjic", fix_head);
    run_plain<ReqUserInputCheck>(
        "Req", orig, recon, "_ZNK9sec_proto17ReqUserInputCheck4packEPcjPjj",
        "_ZN9sec_proto17ReqUserInputCheck6unpackEPKcjPjj",
        "_ZNK9sec_proto17ReqUserInputCheck9visualizeEPcjPjic", fix_req);
    run_plain<RspUserInputCheck>(
        "Rsp", orig, recon, "_ZNK9sec_proto17RspUserInputCheck4packEPcjPjj",
        "_ZN9sec_proto17RspUserInputCheck6unpackEPKcjPjj",
        "_ZNK9sec_proto17RspUserInputCheck9visualizeEPcjPjic", fix_rsp);
    run_body<UserInputCheckPkgBody>("Body", orig, recon);
    run_plain<UserInputCheckPkg>(
        "Pkg", orig, recon, "_ZNK9sec_proto17UserInputCheckPkg4packEPcjPjj",
        "_ZN9sec_proto17UserInputCheckPkg6unpackEPKcjPjj",
        "_ZNK9sec_proto17UserInputCheckPkg9visualizeEPcjPjic", fix_pkg);

    printf("cases=%d failures=%d\n", g_cases, g_fail);
    return g_fail ? 1 : 0;
}
