// Differential oracle: the reconstructed formmog_proto_tss_sdk_qt.cpp (106
// functions) and formmog_proto_public_uictransadapt.cpp (9 functions) vs the
// ORIG libtss_sdk.so.1.0.394, mirroring test_uic_oracle.cpp.
//
// The ORIG is dlopen()ed RTLD_GLOBAL first, so every undefined tsf4g_tdr /
// TdrBufUtil reference of the reconstructed object binds to the ORIG runtime;
// the reconstructed object is dlopen()ed RTLD_LOCAL and entered through
// dlsym(mangled_name), exactly as the ORIG's own callers would.
//
// Every one of the 115 CU symbols is looked up and exercised at least once:
// construct / pack(TdrWriteBuf&) / pack(char*) / unpack(TdrReadBuf&) /
// unpack(char*) / visualize(TdrWriteBuf&) / visualize(char*) /
// visualize_ex(TdrWriteBuf&) / visualize_ex(char*) for the 11 Target A
// messages + Dp2QtMsg, the 7 selector-form Dp2QtMsgBody entries (selector
// 0..11, i.e. including out-of-range values), and the 9 Target B entries.
// Compared per case: return value, produced used-size, and the full output
// buffer (memset beforehand, so any extra write shows up), plus the full struct
// bytes for unpack/construct.
//
// Safety of the generated inputs:
//   * array lengths are random but in-bounds and NUL-terminated where the
//     payload is textual (Login::name_ <= 256, uicta cmd_buf_ <= 32768,
//     rolename_ <= 256);
//   * oversized lengths (cmd_buf_len_ > 32768, rolename_len_ > 256) are fed on
//     purpose: the ORIG, like the reconstruction, tests the length before
//     touching the array (TDR_ERR_REFER_SURPASS_COUNT), so no out-of-bounds
//     read happens on either side - this is asserted by both implementations
//     returning -7 with used==0 and by pack() writing only the 12-byte prefix;
//   * truncated buffers (size/2, size-1) exercise the SHORT_BUF paths, and the
//     NULL-buffer entry points return TDR_ERR_NULL_POINTER_PARAMETER.
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "formmog_proto_public_uictransadapt.h"
#include "formmog_proto_tss_sdk_qt.h"

using namespace sec_proto;
using namespace tsf4g_tdr;

namespace {

const char *kOrigPath =
    "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
const char *kReconPath = "/tmp/libqt_recon.so";

// Target A messages are <= 266 bytes; 64 KiB also holds their visualizations.
const unsigned kCapA = 65536;
const unsigned kItersA = 200;
// Target B is 33044 bytes and its cmd_buf_ dump is ~5 chars/byte.
const unsigned kCapB = 262144;
const unsigned kItersB = 24;

int g_cases = 0;
int g_fail = 0;

// Per-class case counters so the summary shows the coverage split.
const int kMaxLabels = 16;
const char *g_label[kMaxLabels];
int g_label_cases[kMaxLabels];
int g_labels = 0;

void record(const char *label) {
    ++g_cases;
    for (int i = 0; i < g_labels; ++i) {
        if (g_label[i] == label) {
            ++g_label_cases[i];
            return;
        }
    }
    if (g_labels < kMaxLabels) {
        g_label[g_labels] = label;
        g_label_cases[g_labels] = 1;
        ++g_labels;
    }
}

uint32_t g_rng = 0x12345678u;
uint32_t rnd() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng >> 8;
}
uint32_t rnd_in(uint32_t n) { return n ? rnd() % n : 0; }

void fill_random(void *p, size_t n) {
    unsigned char *b = static_cast<unsigned char *>(p);
    for (size_t i = 0; i < n; ++i) b[i] = (unsigned char)(rnd() & 0xff);
}

// Printable payload with a NUL terminator, so neither side can walk past the
// declared length even if it treated the array as a string.
void fill_printable(uint8_t *p, unsigned n) {
    for (unsigned i = 0; i < n; ++i) p[i] = (uint8_t)(1 + (rnd() % 0x7e));
    if (n) p[n - 1] = 0;
}

typedef int (*pack_t)(const void *, char *, unsigned, unsigned *, unsigned);
typedef int (*unpack_t)(void *, const char *, unsigned, unsigned *, unsigned);
typedef int (*vis_t)(const void *, char *, unsigned, unsigned *, int, char);
typedef const char *(*vise_t)(const void *, char *, unsigned, unsigned *, int,
                              char);
typedef int (*pack_buf_t)(const void *, TdrWriteBuf &, unsigned);
typedef int (*unpack_buf_t)(void *, TdrReadBuf &, unsigned);
typedef int (*vis_buf_t)(const void *, TdrWriteBuf &, int, char);
typedef const char *(*vise_buf_t)(const void *, TdrWriteBuf &, int, char);
typedef int (*construct_t)(void *);
typedef int (*pack_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                          unsigned);
typedef int (*unpack_sel_t)(void *, int64_t, const char *, unsigned,
                            unsigned *, unsigned);
typedef int (*vis_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                         int, char);
typedef int (*pack_sel_buf_t)(const void *, int64_t, TdrWriteBuf &, unsigned);
typedef int (*unpack_sel_buf_t)(void *, int64_t, TdrReadBuf &, unsigned);
typedef int (*vis_sel_buf_t)(const void *, int64_t, TdrWriteBuf &, int, char);
typedef int (*construct_sel_t)(void *, int64_t);

// Full 32-bit draw: values >= 2^31 are needed so that a "%u" operand and a
// "%d" operand on the same field produce different text (a 24-bit-only
// generator would hide such a mutation).
uint32_t rnd32() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng;
}

template <typename F>
F sym(void *h, const char *name, const char *label) {
    if (name == NULL) return NULL;
    F f = reinterpret_cast<F>(dlsym(h, name));
    if (!f) {
        printf("MISSING %-32s %s\n", label, name);
        ++g_fail;
    }
    return f;
}

// Compares two returned strings (visualize_ex) without touching buffers.
void check_streq(const char *label, const char *what, const char *a,
                 const char *b) {
    record(label);
    const char *x = a ? a : "";
    const char *y = b ? b : "";
    if (strcmp(x, y) != 0) {
        ++g_fail;
        printf("FAIL %-14s %-12s \"%s\" vs \"%s\"\n", label, what, x, y);
    }
}

void check(const char *label, const char *what, int a, int b, unsigned ua,
           unsigned ub, const char *ba, const char *bb, unsigned n) {
    record(label);
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
        printf("FAIL %-14s %-12s ret %d/%d used %u/%u first-diff %u (%02x/%02x)\n",
               label, what, a, b, ua, ub, d, x, y);
    }
}

void check_str(const char *label, const char *what, int a, int b, unsigned ua,
               unsigned ub, const char *sa, const char *sb, const char *ba,
               const char *bb, unsigned n) {
    record(label);
    const char *x = sa ? sa : "";
    const char *y = sb ? sb : "";
    if (a != b || ua != ub || memcmp(ba, bb, n) != 0 ||
        strncmp(x, y, n) != 0) {
        ++g_fail;
        printf("FAIL %-14s %-12s ret %d/%d used %u/%u str \"%s\"/\"%s\"\n",
               label, what, a, b, ua, ub, x, y);
    }
}

// Symbol names of one plain (non-union) message class.
struct Api {
    const char *label;
    const char *construct;
    const char *pk_char;
    const char *pk_buf;
    const char *up_char;
    const char *up_buf;
    const char *vis_char;
    const char *vis_buf;
    const char *visex_char;
    const char *visex_buf;
};

// ---------------------------------------------------------------------------
// Symbol name table (nm -DC of the ORIG, cu_symbols.py-attributed).
// ---------------------------------------------------------------------------

#define QT_CLASS_SYMS(cls, len)                                     \
    "_ZN9sec_proto" #len #cls "9constructEv",                       \
        "_ZNK9sec_proto" #len #cls "4packEPcjPjj",                  \
        "_ZNK9sec_proto" #len #cls                                  \
        "4packERN9tsf4g_tdr11TdrWriteBufEj",                        \
        "_ZN9sec_proto" #len #cls "6unpackEPKcjPjj",                \
        "_ZN9sec_proto" #len #cls                                   \
        "6unpackERN9tsf4g_tdr10TdrReadBufEj",                       \
        "_ZNK9sec_proto" #len #cls "9visualizeEPcjPjic",            \
        "_ZNK9sec_proto" #len #cls                                  \
        "9visualizeERN9tsf4g_tdr11TdrWriteBufEic",                  \
        "_ZNK9sec_proto" #len #cls "12visualize_exEPcjPjic",        \
        "_ZNK9sec_proto" #len #cls                                  \
        "12visualize_exERN9tsf4g_tdr11TdrWriteBufEic"

const Api kLogin = {"Login", QT_CLASS_SYMS(Login, 5)};
const Api kEnterChannel = {"EnterChannel", QT_CLASS_SYMS(EnterChannel, 12)};
const Api kCreateRoom = {"CreateRoom", QT_CLASS_SYMS(CreateRoom, 10)};
const Api kCreateRoomRes = {"CreateRoomRes", QT_CLASS_SYMS(CreateRoomRes, 13)};
const Api kEnterRoom = {"EnterRoom", QT_CLASS_SYMS(EnterRoom, 9)};
const Api kChangeRole = {"ChangeRole", QT_CLASS_SYMS(ChangeRole, 10)};
const Api kChangePosition = {"ChangePosition", QT_CLASS_SYMS(ChangePosition, 14)};
const Api kRoleAction = {"RoleAction", QT_CLASS_SYMS(RoleAction, 10)};
const Api kRoomMaster = {"RoomMaster", QT_CLASS_SYMS(RoomMaster, 10)};
const Api kChangeRoomType = {"ChangeRoomType", QT_CLASS_SYMS(ChangeRoomType, 14)};
const Api kDp2QtMsg = {"Dp2QtMsg", QT_CLASS_SYMS(Dp2QtMsg, 8)};

const Api kUICTrans = {
    "UICTransAdapt",
    "_ZN9sec_proto29ReqUICTransAdaptMsgForwardPkg9constructEv",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg4packEPcjPjj",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg"
    "4packERN9tsf4g_tdr11TdrWriteBufEj",
    "_ZN9sec_proto29ReqUICTransAdaptMsgForwardPkg6unpackEPKcjPjj",
    "_ZN9sec_proto29ReqUICTransAdaptMsgForwardPkg"
    "6unpackERN9tsf4g_tdr10TdrReadBufEj",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg9visualizeEPcjPjic",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg"
    "9visualizeERN9tsf4g_tdr11TdrWriteBufEic",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg12visualize_exEPcjPjic",
    "_ZNK9sec_proto29ReqUICTransAdaptMsgForwardPkg"
    "12visualize_exERN9tsf4g_tdr11TdrWriteBufEic"};

// ---------------------------------------------------------------------------
// Input builders.  All array lengths stay inside the declared arrays.
// ---------------------------------------------------------------------------

void fix_login(Login &v) {
    memset(&v, 0, sizeof(v));
    v.uin_ = rnd32();
    v.server_ip_ = rnd();
    unsigned n = rnd_in(257);  // 0..256
    v.name_len_ = (uint8_t)n;
    fill_printable(v.name_, n);
}

void fix_enter_channel(EnterChannel &v) {
    memset(&v, 0, sizeof(v));
    v.channel_id_ = (uint8_t)rnd();
}

void fix_create_room(CreateRoom &v) {
    memset(&v, 0, sizeof(v));
    v.room_type_ = (uint8_t)rnd();
    v.visitor_count_ = (uint8_t)rnd();
}

void fix_create_room_res(CreateRoomRes &v) {
    memset(&v, 0, sizeof(v));
    v.room_id_ = (uint16_t)rnd();
}

void fix_enter_room(EnterRoom &v) {
    memset(&v, 0, sizeof(v));
    v.room_id_ = (uint16_t)rnd();
    v.room_type_ = (uint8_t)rnd();
    v.visitor_count_ = (uint8_t)rnd();
    v.role_type_ = (uint8_t)rnd();
    v.role_position_ = (uint8_t)rnd();
}

void fix_change_role(ChangeRole &v) {
    memset(&v, 0, sizeof(v));
    v.role_position_ = (uint8_t)rnd();
}

void fix_change_position(ChangePosition &v) {
    memset(&v, 0, sizeof(v));
    v.role_position_ = (uint8_t)rnd();
    v.role_type_ = (uint8_t)rnd();
}

void fix_role_action(RoleAction &v) {
    memset(&v, 0, sizeof(v));
    v.role_action_ = (uint8_t)rnd();
    v.revert_ = rnd32();
}

void fix_room_master(RoomMaster &v) {
    memset(&v, 0, sizeof(v));
    v.master_role_type_ = (uint8_t)rnd();
    v.master_role_position_ = (uint8_t)rnd();
}

void fix_change_room_type(ChangeRoomType &v) {
    memset(&v, 0, sizeof(v));
    v.room_type_ = (uint8_t)rnd();
}

void fix_body_arm(Dp2QtMsgBody &b, long long sel);  // defined below

// Dp2QtMsg: cmd_id_ selects the union arm on the wire (1..10).  Every fourth
// case uses an out-of-range cmd_id_ (0 or 11), so pack/unpack/visualize must
// fall through to the body's default arm on both sides.
void fix_dp2qt_msg(Dp2QtMsg &v) {
    memset(&v, 0, sizeof(v));
    unsigned sel;
    if (rnd_in(4) == 0) {
        sel = (rnd() & 1) ? 0 : 11;
    } else {
        sel = 1 + rnd_in(10);
    }
    v.cmd_id_ = (uint8_t)sel;
    fix_body_arm(v.body_, (long long)sel);
}

// Fills the union arm selected by sel (1..10); out-of-range selectors leave the
// object filled with random bytes, so the default paths are still compared.
void fix_body_arm(Dp2QtMsgBody &b, long long sel) {
    switch (sel) {
    case 1:
        fix_login(b.login);
        break;
    case 2:
        fix_enter_channel(b.enter_channel);
        break;
    case 3:
        fix_create_room(b.create_room);
        break;
    case 4:
        fix_create_room_res(b.create_room_res);
        break;
    case 5:
        fix_enter_room(b.enter_room);
        break;
    case 6:
        fix_change_role(b.change_role);
        break;
    case 7:
        fix_change_position(b.change_position);
        break;
    case 8:
        fix_role_action(b.role_action);
        break;
    case 9:
        fix_room_master(b.room_master);
        break;
    case 10:
        fix_change_room_type(b.room_type);
        break;
    default:
        break;
    }
}

// Target B: length fields in-bounds / deliberately oversized, so the
// REFER_SURPASS paths of pack, unpack and visualize are all exercised.
void fix_uic_trans(ReqUICTransAdaptMsgForwardPkg &v, unsigned mode) {
    memset(&v, 0, sizeof(v));
    v.world_id_ = (uint16_t)rnd();
    v.src_id_ = rnd32();
    v.cmd_id_ = rnd32();
    v.need_encrypt_ = (uint8_t)(rnd() & 1);
    v.is_encrypted_ = (uint8_t)(rnd() & 1);
    unsigned cl = rnd_in(32769);  // 0..32768 (in-bounds)
    unsigned rl = rnd_in(257);    // 0..256 (in-bounds)
    if (mode == 1) cl = 32769 + rnd_in(64);   // > sizeof(cmd_buf_)
    if (mode == 2) rl = 257 + rnd_in(64);     // > sizeof(rolename_)
    if (mode == 3) {
        cl = 32768;  // exact capacity, in-bounds
        rl = 256;
    }
    v.cmd_buf_len_ = (uint16_t)cl;
    v.rolename_len_ = (uint16_t)rl;
    if (cl <= 32768) fill_printable(v.cmd_buf_, cl);
    if (rl <= 256) fill_printable(v.rolename_, rl);
    v.client_ip_ = rnd32();
}

// ---------------------------------------------------------------------------
// Runners
// ---------------------------------------------------------------------------

// Calls all nine entry points of one message class and compares each result.
template <typename T>
void run_plain(const Api &api, void *orig, void *recon, void (*fix)(T &),
               unsigned cap, unsigned iters, bool edge_cases) {
    const char *lab = api.label;
    construct_t co = sym<construct_t>(orig, api.construct, lab);
    construct_t cr = sym<construct_t>(recon, api.construct, lab);
    pack_t pko = sym<pack_t>(orig, api.pk_char, lab);
    pack_t pkr = sym<pack_t>(recon, api.pk_char, lab);
    pack_buf_t pbo = sym<pack_buf_t>(orig, api.pk_buf, lab);
    pack_buf_t pbr = sym<pack_buf_t>(recon, api.pk_buf, lab);
    unpack_t upo = sym<unpack_t>(orig, api.up_char, lab);
    unpack_t upr = sym<unpack_t>(recon, api.up_char, lab);
    unpack_buf_t ubo = sym<unpack_buf_t>(orig, api.up_buf, lab);
    unpack_buf_t ubr = sym<unpack_buf_t>(recon, api.up_buf, lab);
    vis_t vio = sym<vis_t>(orig, api.vis_char, lab);
    vis_t vir = sym<vis_t>(recon, api.vis_char, lab);
    vis_buf_t vbo = sym<vis_buf_t>(orig, api.vis_buf, lab);
    vis_buf_t vbr = sym<vis_buf_t>(recon, api.vis_buf, lab);
    vise_t veo = sym<vise_t>(orig, api.visex_char, lab);
    vise_t ver = sym<vise_t>(recon, api.visex_char, lab);
    vise_buf_t wbo = sym<vise_buf_t>(orig, api.visex_buf, lab);
    vise_buf_t wbr = sym<vise_buf_t>(recon, api.visex_buf, lab);
    if (!co || !cr || !pko || !pkr || !pbo || !pbr || !upo || !upr || !ubo ||
        !ubr || !vio || !vir || !vbo || !vbr || !veo || !ver || !wbo || !wbr)
        return;

    char *bo = new char[cap];
    char *br = new char[cap];
    char *vo = new char[cap];
    char *vr = new char[cap];
    char *so = new char[cap];
    char *sr = new char[cap];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    T *ko = new T;
    T *kr = new T;
    for (unsigned it = 0; it < iters; ++it) {
        memset(bo, 0, cap);
        memset(br, 0, cap);
        memset(vo, 0, cap);
        memset(vr, 0, cap);
        memset(so, 0, cap);
        memset(sr, 0, cap);
        memset(src, 0, sizeof(T));
        memset(uo, 0, sizeof(T));
        memset(ur, 0, sizeof(T));
        fix(*src);

        // construct() on identically pre-filled garbage: the zeroed field set
        // must match byte for byte.
        fill_random(ko, sizeof(T));
        memcpy(kr, ko, sizeof(T));
        int c1 = co(ko);
        int c2 = cr(kr);
        check(lab, "construct", c1, c2, 0, 0, reinterpret_cast<const char *>(ko),
              reinterpret_cast<const char *>(kr), sizeof(T));

        unsigned uuo = 0, uur = 0;
        int ro = pko(src, bo, cap, &uuo, 1);
        int rr = pkr(src, br, cap, &uur, 1);
        check(lab, "pack", ro, rr, uuo, uur, bo, br, cap);

        // TdrWriteBuf-based overload on the same input.
        unsigned ubo1 = 0, ubr1 = 0;
        {
            TdrWriteBuf wo(bo, cap);
            TdrWriteBuf wr(br, cap);
            int r1 = pbo(src, wo, 1);
            int r2 = pbr(src, wr, 1);
            ubo1 = (unsigned)wo.getUsedSize();
            ubr1 = (unsigned)wr.getUsedSize();
            check(lab, "pack-buf", r1, r2, ubo1, ubr1, bo, br, cap);
        }

        if (ro == 0 && rr == 0) {
            unsigned uu2 = 0, uu3 = 0;
            int ro2 = upo(uo, bo, uuo, &uu2, 1);
            int rr2 = upr(ur, bo, uuo, &uu3, 1);
            check(lab, "unpack", ro2, rr2, uu2, uu3,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));

            memset(uo, 0, sizeof(T));
            memset(ur, 0, sizeof(T));
            unsigned uu4 = 0, uu5 = 0;
            {
                TdrReadBuf rdo(bo, uuo);
                TdrReadBuf rdr(br, uur);
                int r3 = ubo(uo, rdo, 1);
                int r4 = ubr(ur, rdr, 1);
                uu4 = (unsigned)rdo.getUsedSize();
                uu5 = (unsigned)rdr.getUsedSize();
                check(lab, "unpack-buf", r3, r4, uu4, uu5,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
            }

            if (edge_cases && uuo > 1) {
                // Truncated input: the SHORT_BUF_FOR_READ path must agree.
                memset(uo, 0, sizeof(T));
                memset(ur, 0, sizeof(T));
                unsigned t1 = 0, t2 = 0;
                int r5 = upo(uo, bo, uuo / 2, &t1, 1);
                int r6 = upr(ur, bo, uuo / 2, &t2, 1);
                check(lab, "unpack-half", r5, r6, t1, t2,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
                memset(uo, 0, sizeof(T));
                memset(ur, 0, sizeof(T));
                int r7 = upo(uo, bo, uuo - 1, &t1, 1);
                int r8 = upr(ur, bo, uuo - 1, &t2, 1);
                check(lab, "unpack-short", r7, r8, t1, t2,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
            }
            if (edge_cases) {
                // Small write buffers: SHORT_BUF_FOR_WRITE path.
                const unsigned caps[3] = {1, 4, 16};
                for (int ci = 0; ci < 3; ++ci) {
                    unsigned c = caps[ci];
                    memset(bo, 0, cap);
                    memset(br, 0, cap);
                    unsigned s1 = 0, s2 = 0;
                    int r9 = pko(src, bo, c, &s1, 1);
                    int r10 = pkr(src, br, c, &s2, 1);
                    check(lab, "pack-small", r9, r10, s1, s2, bo, br, c);
                }
                memset(bo, 0, cap);
                memset(br, 0, cap);
            }
        }

        unsigned uvo = 0, uvr = 0;
        int rvo = vio(src, vo, cap, &uvo, 2, '\n');
        int rvr = vir(src, vr, cap, &uvr, 2, '\n');
        check(lab, "visualize", rvo, rvr, uvo, uvr, vo, vr, cap);

        unsigned uwo = 0, uwr = 0;
        {
            TdrWriteBuf wo(vo, cap);
            TdrWriteBuf wr(vr, cap);
            int r1 = vbo(src, wo, 2, '\n');
            int r2 = vbr(src, wr, 2, '\n');
            uwo = (unsigned)wo.getUsedSize();
            uwr = (unsigned)wr.getUsedSize();
            check(lab, "visualize-buf", r1, r2, uwo, uwr, vo, vr, cap);
        }

        memset(so, 0, cap);
        memset(sr, 0, cap);
        unsigned uxo = 0, uxr = 0;
        const char *xo = veo(src, so, cap, &uxo, 2, '\n');
        const char *xr = ver(src, sr, cap, &uxr, 2, '\n');
        check_str(lab, "visualize_ex", 0, 0, uxo, uxr, xo, xr, so, sr, cap);

        memset(so, 0, cap);
        memset(sr, 0, cap);
        {
            TdrWriteBuf wo(so, cap);
            TdrWriteBuf wr(sr, cap);
            const char *x1 = wbo(src, wo, 2, '\n');
            const char *x2 = wbr(src, wr, 2, '\n');
            uxo = (unsigned)wo.getUsedSize();
            uxr = (unsigned)wr.getUsedSize();
            check_str(lab, "visex-buf", 0, 0, uxo, uxr, x1, x2, so, sr, cap);
        }

        if (edge_cases && it == 0) {
            // NULL-buffer entry points: both sides must report
            // TDR_ERR_NULL_POINTER_PARAMETER and leave the used-sizes alone.
            unsigned pn1 = 0, pn2 = 0;
            int m1 = pko(src, NULL, cap, &pn1, 1);
            int m2 = pkr(src, NULL, cap, &pn2, 1);
            check(lab, "pack-null", m1, m2, pn1, pn2, bo, br, 0);
            unsigned an1 = 0, an2 = 0;
            int m3 = upo(uo, NULL, cap, &an1, 1);
            int m4 = upr(ur, NULL, cap, &an2, 1);
            check(lab, "unpack-null", m3, m4, an1, an2,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));
            unsigned vn1 = 0, vn2 = 0;
            int m5 = vio(src, NULL, cap, &vn1, 2, '\n');
            int m6 = vir(src, NULL, cap, &vn2, 2, '\n');
            check(lab, "vis-null", m5, m6, vn1, vn2, vo, vr, 0);
            const char *m7 = veo(src, NULL, 0, NULL, 2, '\n');
            const char *m8 = ver(src, NULL, 0, NULL, 2, '\n');
            check_streq(lab, "visex-null", m7, m8);

            // usedSize == NULL, zero-size / zero-capacity visualize_ex, and the
            // negative-indent plus alternate-separator paths.
            int q1 = pko(src, bo, cap, NULL, 1);
            int q2 = pkr(src, br, cap, NULL, 1);
            check(lab, "pack-nous", q1, q2, 0, 0, bo, br, 0);
            int q3 = vio(src, vo, cap, NULL, 2, '\n');
            int q4 = vir(src, vr, cap, NULL, 2, '\n');
            check(lab, "vis-nous", q3, q4, 0, 0, vo, vr, 0);
            unsigned un1 = 0, un2 = 0;
            const char *q5 = veo(src, so, 0, &un1, 2, '\n');
            const char *q6 = ver(src, sr, 0, &un2, 2, '\n');
            check_streq(lab, "visex-size0", q5, q6);
            TdrWriteBuf eo;
            TdrWriteBuf er;
            const char *q7 = wbo(src, eo, 2, '\n');
            const char *q8 = wbr(src, er, 2, '\n');
            check_streq(lab, "visex-emptybuf", q7, q8);
            memset(vo, 0, cap);
            memset(vr, 0, cap);
            unsigned un3 = 0, un4 = 0;
            int q9 = vio(src, vo, cap, &un3, -1, ' ');
            int q10 = vir(src, vr, cap, &un4, -1, ' ');
            check(lab, "vis-negindent", q9, q10, un3, un4, vo, vr, cap);
            {
                TdrWriteBuf wo(vo, cap);
                TdrWriteBuf wr(vr, cap);
                int q11 = vbo(src, wo, -1, ' ');
                int q12 = vbr(src, wr, -1, ' ');
                check(lab, "vis-buf-negindent", q11, q12,
                      (unsigned)wo.getUsedSize(), (unsigned)wr.getUsedSize(),
                      vo, vr, cap);
            }
        }
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete[] so;
    delete[] sr;
    delete src;
    delete uo;
    delete ur;
    delete ko;
    delete kr;
}

// Dp2QtMsgBody: selector-form entry points, selectors 0..11 (0 and 11 fall
// through to the default arm on both sides), construct(sel) included.
void run_body(void *orig, void *recon, unsigned cap, unsigned iters) {
    const char *lab = "Body";
    construct_sel_t co = sym<construct_sel_t>(
        orig, "_ZN9sec_proto12Dp2QtMsgBody9constructEx", lab);
    construct_sel_t cr = sym<construct_sel_t>(
        recon, "_ZN9sec_proto12Dp2QtMsgBody9constructEx", lab);
    pack_sel_t pko = sym<pack_sel_t>(
        orig, "_ZNK9sec_proto12Dp2QtMsgBody4packExPcjPjj", lab);
    pack_sel_t pkr = sym<pack_sel_t>(
        recon, "_ZNK9sec_proto12Dp2QtMsgBody4packExPcjPjj", lab);
    pack_sel_buf_t pbo = sym<pack_sel_buf_t>(
        orig, "_ZNK9sec_proto12Dp2QtMsgBody4packExRN9tsf4g_tdr11TdrWriteBufEj",
        lab);
    pack_sel_buf_t pbr = sym<pack_sel_buf_t>(
        recon, "_ZNK9sec_proto12Dp2QtMsgBody4packExRN9tsf4g_tdr11TdrWriteBufEj",
        lab);
    unpack_sel_t upo = sym<unpack_sel_t>(
        orig, "_ZN9sec_proto12Dp2QtMsgBody6unpackExPKcjPjj", lab);
    unpack_sel_t upr = sym<unpack_sel_t>(
        recon, "_ZN9sec_proto12Dp2QtMsgBody6unpackExPKcjPjj", lab);
    unpack_sel_buf_t ubo = sym<unpack_sel_buf_t>(
        orig, "_ZN9sec_proto12Dp2QtMsgBody6unpackExRN9tsf4g_tdr10TdrReadBufEj",
        lab);
    unpack_sel_buf_t ubr = sym<unpack_sel_buf_t>(
        recon, "_ZN9sec_proto12Dp2QtMsgBody6unpackExRN9tsf4g_tdr10TdrReadBufEj",
        lab);
    vis_sel_t vio = sym<vis_sel_t>(
        orig, "_ZNK9sec_proto12Dp2QtMsgBody9visualizeExPcjPjic", lab);
    vis_sel_t vir = sym<vis_sel_t>(
        recon, "_ZNK9sec_proto12Dp2QtMsgBody9visualizeExPcjPjic", lab);
    vis_sel_buf_t vbo = sym<vis_sel_buf_t>(
        orig,
        "_ZNK9sec_proto12Dp2QtMsgBody9visualizeExRN9tsf4g_tdr11TdrWriteBufEic",
        lab);
    vis_sel_buf_t vbr = sym<vis_sel_buf_t>(
        recon,
        "_ZNK9sec_proto12Dp2QtMsgBody9visualizeExRN9tsf4g_tdr11TdrWriteBufEic",
        lab);
    if (!co || !cr || !pko || !pkr || !pbo || !pbr || !upo || !upr || !ubo ||
        !ubr || !vio || !vir || !vbo || !vbr)
        return;

    char *bo = new char[cap];
    char *br = new char[cap];
    char *vo = new char[cap];
    char *vr = new char[cap];
    Dp2QtMsgBody *src = new Dp2QtMsgBody;
    Dp2QtMsgBody *uo = new Dp2QtMsgBody;
    Dp2QtMsgBody *ur = new Dp2QtMsgBody;
    Dp2QtMsgBody *ko = new Dp2QtMsgBody;
    Dp2QtMsgBody *kr = new Dp2QtMsgBody;
    for (unsigned it = 0; it < iters; ++it) {
        for (int sel = 0; sel <= 11; ++sel) {
            memset(bo, 0, cap);
            memset(br, 0, cap);
            memset(vo, 0, cap);
            memset(vr, 0, cap);
            memset(src, 0, sizeof(*src));
            fix_body_arm(*src, sel);

            fill_random(ko, sizeof(*ko));
            memcpy(kr, ko, sizeof(*ko));
            int c1 = co(ko, sel);
            int c2 = cr(kr, sel);
            check(lab, "construct-sel", c1, c2, 0, 0,
                  reinterpret_cast<const char *>(ko),
                  reinterpret_cast<const char *>(kr), sizeof(*ko));

            unsigned uuo = 0, uur = 0;
            int ro = pko(src, sel, bo, cap, &uuo, 1);
            int rr = pkr(src, sel, br, cap, &uur, 1);
            check(lab, "pack-sel", ro, rr, uuo, uur, bo, br, cap);

            {
                TdrWriteBuf wo(bo, cap);
                TdrWriteBuf wr(br, cap);
                int r1 = pbo(src, sel, wo, 1);
                int r2 = pbr(src, sel, wr, 1);
                check(lab, "pack-sel-buf", r1, r2,
                      (unsigned)wo.getUsedSize(), (unsigned)wr.getUsedSize(),
                      bo, br, cap);
            }

            if (ro == 0 && rr == 0) {
                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                unsigned uu2 = 0, uu3 = 0;
                int ro2 = upo(uo, sel, bo, uuo, &uu2, 1);
                int rr2 = upr(ur, sel, bo, uuo, &uu3, 1);
                check(lab, "unpack-sel", ro2, rr2, uu2, uu3,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));

                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                TdrReadBuf rdo(bo, uuo);
                TdrReadBuf rdr(br, uur);
                int r3 = ubo(uo, sel, rdo, 1);
                int r4 = ubr(ur, sel, rdr, 1);
                check(lab, "unpack-sel-buf", r3, r4,
                      (unsigned)rdo.getUsedSize(), (unsigned)rdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));

                if (uuo > 1) {
                    memset(uo, 0, sizeof(*uo));
                    memset(ur, 0, sizeof(*ur));
                    unsigned t1 = 0, t2 = 0;
                    int r5 = upo(uo, sel, bo, uuo / 2, &t1, 1);
                    int r6 = upr(ur, sel, bo, uuo / 2, &t2, 1);
                    check(lab, "unpack-sel-half", r5, r6, t1, t2,
                          reinterpret_cast<const char *>(uo),
                          reinterpret_cast<const char *>(ur), sizeof(*uo));
                }
            }

            unsigned uvo = 0, uvr = 0;
            int rvo = vio(src, sel, vo, cap, &uvo, 2, '\n');
            int rvr = vir(src, sel, vr, cap, &uvr, 2, '\n');
            check(lab, "vis-sel", rvo, rvr, uvo, uvr, vo, vr, cap);

            {
                TdrWriteBuf wo(vo, cap);
                TdrWriteBuf wr(vr, cap);
                int r1 = vbo(src, sel, wo, 2, '\n');
                int r2 = vbr(src, sel, wr, 2, '\n');
                check(lab, "vis-sel-buf", r1, r2,
                      (unsigned)wo.getUsedSize(), (unsigned)wr.getUsedSize(),
                      vo, vr, cap);
            }

            if (it == 0 && sel == 1) {
                // NULL-buffer and negative-indent paths of the selector forms.
                unsigned z1 = 0, z2 = 0;
                int y1 = pko(src, sel, NULL, cap, &z1, 1);
                int y2 = pkr(src, sel, NULL, cap, &z2, 1);
                check(lab, "pack-sel-null", y1, y2, z1, z2, bo, br, 0);
                unsigned z3 = 0, z4 = 0;
                int y3 = upo(uo, sel, NULL, cap, &z3, 1);
                int y4 = upr(ur, sel, NULL, cap, &z4, 1);
                check(lab, "unpack-sel-null", y3, y4, z3, z4,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));
                unsigned z5 = 0, z6 = 0;
                int y5 = vio(src, sel, NULL, cap, &z5, 2, '\n');
                int y6 = vir(src, sel, NULL, cap, &z6, 2, '\n');
                check(lab, "vis-sel-null", y5, y6, z5, z6, vo, vr, 0);
                memset(vo, 0, cap);
                memset(vr, 0, cap);
                unsigned z7 = 0, z8 = 0;
                int y7 = vio(src, sel, vo, cap, &z7, -1, ' ');
                int y8 = vir(src, sel, vr, cap, &z8, -1, ' ');
                check(lab, "vis-sel-negindent", y7, y8, z7, z8, vo, vr, cap);
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
    delete ko;
    delete kr;
}

// Target B: three length modes (in-bounds, oversized cmd_buf_len_, oversized
// rolename_len_) plus the exact-capacity case, over the same nine entry points.
void run_uictrans(const Api &api, void *orig, void *recon) {
    const char *lab = api.label;
    pack_t pko = sym<pack_t>(orig, api.pk_char, lab);
    pack_t pkr = sym<pack_t>(recon, api.pk_char, lab);
    pack_buf_t pbo = sym<pack_buf_t>(orig, api.pk_buf, lab);
    pack_buf_t pbr = sym<pack_buf_t>(recon, api.pk_buf, lab);
    unpack_t upo = sym<unpack_t>(orig, api.up_char, lab);
    unpack_t upr = sym<unpack_t>(recon, api.up_char, lab);
    unpack_buf_t ubo = sym<unpack_buf_t>(orig, api.up_buf, lab);
    unpack_buf_t ubr = sym<unpack_buf_t>(recon, api.up_buf, lab);
    vis_t vio = sym<vis_t>(orig, api.vis_char, lab);
    vis_t vir = sym<vis_t>(recon, api.vis_char, lab);
    vis_buf_t vbo = sym<vis_buf_t>(orig, api.vis_buf, lab);
    vis_buf_t vbr = sym<vis_buf_t>(recon, api.vis_buf, lab);
    vise_t veo = sym<vise_t>(orig, api.visex_char, lab);
    vise_t ver = sym<vise_t>(recon, api.visex_char, lab);
    vise_buf_t wbo = sym<vise_buf_t>(orig, api.visex_buf, lab);
    vise_buf_t wbr = sym<vise_buf_t>(recon, api.visex_buf, lab);
    construct_t co = sym<construct_t>(orig, api.construct, lab);
    construct_t cr = sym<construct_t>(recon, api.construct, lab);
    if (!co || !cr || !pko || !pkr || !pbo || !pbr || !upo || !upr || !ubo ||
        !ubr || !vio || !vir || !vbo || !vbr || !veo || !ver || !wbo || !wbr)
        return;

    const unsigned cap = kCapB;
    char *bo = new char[cap];
    char *br = new char[cap];
    char *vo = new char[cap];
    char *vr = new char[cap];
    char *so = new char[cap];
    char *sr = new char[cap];
    ReqUICTransAdaptMsgForwardPkg *src = new ReqUICTransAdaptMsgForwardPkg;
    ReqUICTransAdaptMsgForwardPkg *uo = new ReqUICTransAdaptMsgForwardPkg;
    ReqUICTransAdaptMsgForwardPkg *ur = new ReqUICTransAdaptMsgForwardPkg;
    ReqUICTransAdaptMsgForwardPkg *ko = new ReqUICTransAdaptMsgForwardPkg;
    ReqUICTransAdaptMsgForwardPkg *kr = new ReqUICTransAdaptMsgForwardPkg;
    for (unsigned it = 0; it < kItersB; ++it) {
        for (unsigned mode = 0; mode < 4; ++mode) {
            memset(bo, 0, cap);
            memset(br, 0, cap);
            memset(vo, 0, cap);
            memset(vr, 0, cap);
            memset(so, 0, cap);
            memset(sr, 0, cap);
            memset(src, 0, sizeof(*src));
            fix_uic_trans(*src, mode);

            fill_random(ko, sizeof(*ko));
            memcpy(kr, ko, sizeof(*ko));
            int c1 = co(ko);
            int c2 = cr(kr);
            check(lab, "construct", c1, c2, 0, 0,
                  reinterpret_cast<const char *>(ko),
                  reinterpret_cast<const char *>(kr), sizeof(*ko));

            unsigned uuo = 0, uur = 0;
            int ro = pko(src, bo, cap, &uuo, 1);
            int rr = pkr(src, br, cap, &uur, 1);
            check(lab, "pack", ro, rr, uuo, uur, bo, br, cap);

            {
                TdrWriteBuf wo(bo, cap);
                TdrWriteBuf wr(br, cap);
                int r1 = pbo(src, wo, 1);
                int r2 = pbr(src, wr, 1);
                check(lab, "pack-buf", r1, r2, (unsigned)wo.getUsedSize(),
                      (unsigned)wr.getUsedSize(), bo, br, cap);
            }

            if (ro == 0 && rr == 0 && uuo > 1) {
                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                unsigned uu2 = 0, uu3 = 0;
                int ro2 = upo(uo, bo, uuo, &uu2, 1);
                int rr2 = upr(ur, bo, uuo, &uu3, 1);
                check(lab, "unpack", ro2, rr2, uu2, uu3,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));

                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                TdrReadBuf rdo(bo, uuo);
                TdrReadBuf rdr(br, uur);
                int r3 = ubo(uo, rdo, 1);
                int r4 = ubr(ur, rdr, 1);
                check(lab, "unpack-buf", r3, r4,
                      (unsigned)rdo.getUsedSize(), (unsigned)rdr.getUsedSize(),
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));

                memset(uo, 0, sizeof(*uo));
                memset(ur, 0, sizeof(*ur));
                unsigned t1 = 0, t2 = 0;
                int r5 = upo(uo, bo, uuo / 2, &t1, 1);
                int r6 = upr(ur, bo, uuo / 2, &t2, 1);
                check(lab, "unpack-half", r5, r6, t1, t2,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(*uo));
            }

            unsigned uvo = 0, uvr = 0;
            int rvo = vio(src, vo, cap, &uvo, 2, '\n');
            int rvr = vir(src, vr, cap, &uvr, 2, '\n');
            check(lab, "visualize", rvo, rvr, uvo, uvr, vo, vr, cap);

            {
                TdrWriteBuf wo(vo, cap);
                TdrWriteBuf wr(vr, cap);
                int r1 = vbo(src, wo, 2, '\n');
                int r2 = vbr(src, wr, 2, '\n');
                check(lab, "visualize-buf", r1, r2,
                      (unsigned)wo.getUsedSize(), (unsigned)wr.getUsedSize(),
                      vo, vr, cap);
            }

            unsigned uxo = 0, uxr = 0;
            const char *xo = veo(src, so, cap, &uxo, 2, '\n');
            const char *xr = ver(src, sr, cap, &uxr, 2, '\n');
            check_str(lab, "visualize_ex", 0, 0, uxo, uxr, xo, xr, so, sr, cap);

            {
                TdrWriteBuf wo(so, cap);
                TdrWriteBuf wr(sr, cap);
                const char *x1 = wbo(src, wo, 2, '\n');
                const char *x2 = wbr(src, wr, 2, '\n');
                check_str(lab, "visex-buf", 0, 0, (unsigned)wo.getUsedSize(),
                          (unsigned)wr.getUsedSize(), x1, x2, so, sr, cap);
            }

            if (it == 0 && mode == 0) {
                unsigned n1u = 0, n2u = 0;
                int n1 = pko(src, NULL, cap, &n1u, 1);
                int n2 = pkr(src, NULL, cap, &n2u, 1);
                check(lab, "pack-null", n1, n2, n1u, n2u, bo, br, 0);
                int n3 = vio(src, NULL, cap, &uvo, 2, '\n');
                int n4 = vir(src, NULL, cap, &uvr, 2, '\n');
                check(lab, "vis-null", n3, n4, uvo, uvr, vo, vr, 0);
            }

            // Capacity boundaries of the 33044-byte message: exactly enough,
            // one byte short, and a visualize buffer too small for the 32 KiB
            // cmd_buf_ dump (the mid-array short-buffer path).
            {
                const unsigned exact = 33044;
                unsigned e1 = 0, e2 = 0;
                int z1 = pko(src, bo, exact, &e1, 1);
                int z2 = pkr(src, br, exact, &e2, 1);
                check(lab, "pack-exact", z1, z2, e1, e2, bo, br, exact);
                z1 = pko(src, bo, exact - 1, &e1, 1);
                z2 = pkr(src, br, exact - 1, &e2, 1);
                check(lab, "pack-short", z1, z2, e1, e2, bo, br, exact);
                memset(vo, 0, cap);
                memset(vr, 0, cap);
                unsigned t1 = 0, t2 = 0;
                int z3 = vio(src, vo, 1000, &t1, 2, '\n');
                int z4 = vir(src, vr, 1000, &t2, 2, '\n');
                check(lab, "vis-small", z3, z4, t1, t2, vo, vr, 1000);
                memset(vo, 0, cap);
                memset(vr, 0, cap);
                int z5 = vio(src, vo, cap, &t1, -1, ' ');
                int z6 = vir(src, vr, cap, &t2, -1, ' ');
                check(lab, "vis-negindent", z5, z6, t1, t2, vo, vr, cap);
            }
        }
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete[] so;
    delete[] sr;
    delete src;
    delete uo;
    delete ur;
    delete ko;
    delete kr;
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
    printf("loaded; sizeof Login=%u EnterRoom=%u RoleAction=%u Dp2QtMsg=%u "
           "Body=%u UICTrans=%u\n",
           (unsigned)sizeof(Login), (unsigned)sizeof(EnterRoom),
           (unsigned)sizeof(RoleAction), (unsigned)sizeof(Dp2QtMsg),
           (unsigned)sizeof(Dp2QtMsgBody),
           (unsigned)sizeof(ReqUICTransAdaptMsgForwardPkg));

    run_plain<Login>(kLogin, orig, recon, fix_login, kCapA, kItersA, true);
    run_plain<EnterChannel>(kEnterChannel, orig, recon, fix_enter_channel,
                            kCapA, kItersA, true);
    run_plain<CreateRoom>(kCreateRoom, orig, recon, fix_create_room, kCapA,
                          kItersA, true);
    run_plain<CreateRoomRes>(kCreateRoomRes, orig, recon, fix_create_room_res,
                             kCapA, kItersA, true);
    run_plain<EnterRoom>(kEnterRoom, orig, recon, fix_enter_room, kCapA,
                         kItersA, true);
    run_plain<ChangeRole>(kChangeRole, orig, recon, fix_change_role, kCapA,
                          kItersA, true);
    run_plain<ChangePosition>(kChangePosition, orig, recon, fix_change_position,
                              kCapA, kItersA, true);
    run_plain<RoleAction>(kRoleAction, orig, recon, fix_role_action, kCapA,
                          kItersA, true);
    run_plain<RoomMaster>(kRoomMaster, orig, recon, fix_room_master, kCapA,
                          kItersA, true);
    run_plain<ChangeRoomType>(kChangeRoomType, orig, recon, fix_change_room_type,
                              kCapA, kItersA, true);
    run_plain<Dp2QtMsg>(kDp2QtMsg, orig, recon, fix_dp2qt_msg, kCapA, kItersA,
                        true);
    run_body(orig, recon, kCapA, kItersA);
    run_uictrans(kUICTrans, orig, recon);

    printf("cases=%d failures=%d\n", g_cases, g_fail);
    for (int i = 0; i < g_labels; ++i)
        printf("  %-16s %d\n", g_label[i], g_label_cases[i]);
    return g_fail ? 1 : 0;
}
